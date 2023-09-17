#include <gimbal/core/gimbal_thread.h>
#include <gimbal/containers/gimbal_array_map.h>
#include <gimbal/utils/gimbal_date_time.h>
#include <gimbal/containers/gimbal_linked_list.h>
#include <gimbal/meta/signals/gimbal_marshal.h>
#include <tinycthread.h>

#define GBL_THREAD_(self)               (GBL_PRIVATE(GblThread, self))
#define GBL_THREAD_PUBLIC_(priv)        (GBL_PUBLIC(GblThread, priv))
#define GBL_THREAD_ENTRY_(node)         (GBL_DOUBLY_LINKED_LIST_ENTRY(node, GblThread_, listNode))
#define GBL_THREAD_ENTRY_PUBLIC_(node)  (GBL_THREAD_PUBLIC_(GBL_THREAD_ENTRY_(node)))
#define GBL_THREAD_SHUTDOWN_RETRIES     10

GBL_DECLARE_STRUCT(GblThread_) {
    union {
        GblDoublyLinkedListNode listNode;
        GblThread_*             pNext;
    };
    thrd_t  nativeHandle;
    struct {
        uint8_t joined  : 1;
    };
};

static mtx_t        listMtx_;
static once_flag    initOnce_       = ONCE_FLAG_INIT;
static GblDoublyLinkedListNode
                    threadList_     = {
                        .pNext = &threadList_,
                        .pPrev = &threadList_
                    };

static GblQuark     closureQuark_   = GBL_QUARK_INVALID;
static GblQuark     callbackQuark_  = GBL_QUARK_INVALID;

static GBL_THREAD_LOCAL
GblThread*          pCurThread_     = NULL;

static void GblThread_init_(void) {
    mtx_init(&listMtx_, mtx_recursive);
   // atexit(GblThread_finalize_);
}

GBL_RESULT GblThread_final_(void) {
    GBL_CTX_BEGIN(NULL);
    call_once(&initOnce_, &GblThread_init_);

    mtx_lock(&listMtx_);

    size_t                   retries = 0;
    GblThread*               pCur    = NULL;
    GblThread*               pPrev   = NULL;
    GblDoublyLinkedListNode* pIt     = NULL;

    while((pIt = GblDoublyLinkedList_front(&threadList_))) {
        pCur = GBL_THREAD_ENTRY_PUBLIC_(pIt);

        if(!pCur) break;
        else if(pCur != pPrev) {
            GblThread_yield();
            pPrev = pCur;
            retries = 0;
        } else if(++retries >= GBL_THREAD_SHUTDOWN_RETRIES) {
            GblDoublyLinkedList_remove(pIt);
            retries = 0;
            GBL_CTX_RECORD_SET(GBL_RESULT_TIMEOUT,
                               "Timed out waiting for thread to join!");
        }
    }

    GBL_UNREF(pCurThread_);

    mtx_unlock(&listMtx_);
    mtx_destroy(&listMtx_);

    GBL_CTX_END();
}

static void GblThread_register_(GblThread* pSelf) {
    GBL_UNUSED(pSelf);
    call_once(&initOnce_, &GblThread_init_);

    mtx_lock(&listMtx_);

    GblDoublyLinkedList_pushBack(&threadList_,
                                 &GBL_THREAD_(pSelf)->listNode);

    mtx_unlock(&listMtx_);
}

static void GblThread_unregister_(GblThread* pSelf) {
    GBL_UNUSED(pSelf);
    mtx_lock(&listMtx_);

    GblDoublyLinkedList_remove(&GBL_THREAD_(pSelf)->listNode);

    mtx_unlock(&listMtx_);
}

static void GblThread_initSelf_(GblThread* pSelf, GblBool mainThread) {
    pCurThread_  = pSelf;
    GblThread_* pSelf_ = GBL_THREAD_(pSelf);
    if(mainThread)
        pSelf_->nativeHandle = thrd_current();
    // set up thread priority
    // set up thread affinity
    // set up thread name
}

static void GblThread_signalHandler_(int signal) {
    // Probably supposed to just defer handling until later
    GblThread* pSelf = GblThread_current();
    volatile sig_atomic_t prevStatus = pSelf->signalStatus;
    pSelf->signalStatus = signal;

    GBL_LOG_WARN("gimbal", "Caught signal [%d] on thread [%s].",
                 signal, GblObject_name(GBL_OBJECT(pSelf)));

    GBL_THREAD_GET_CLASS(pSelf)->pFnSignal(pSelf, signal);
    pSelf->signalStatus = prevStatus;
}

static GblThread* GblThread_initUnknown_(GblBool mainThread) {
    char buffer[16+1] = { '\0' };

    if(!mainThread) {
        snprintf(buffer, sizeof(buffer), "%zu", GblThread_count());
    } else {
        strcpy(buffer, "main");
        signal(SIGTERM, GblThread_signalHandler_);
    }

    pCurThread_ = GBL_NEW(GblThread,
                          "name", buffer);

    GblThread_initSelf_(pCurThread_, mainThread);

    return pCurThread_;
}

static GBL_RESULT GblThread_signal_(GblThread* pSelf, int signal) {
    GBL_CTX_BEGIN(NULL);
    GBL_EMIT(pSelf, "signaled", signal);
    GBL_CTX_END();
}

// default implementation of run virtual function
static GBL_RESULT GblThread_run_(GblThread* pSelf) {
    GBL_CTX_BEGIN(NULL);

    // fetch C callback function from box field
    GblThreadFn pCallback = GblThread_callback(pSelf);

    // invoke if we have it
    if(pCallback) {
        GBL_CTX_VERIFY_CALL(pCallback(pSelf));
    }

    // fetch generic closure from box field
    GblClosure* pClosure = GblThread_closure(pSelf);

    // invoke if we have it
    if(pClosure) {
        GblVariant ret = GBL_VARIANT_INIT;
        GblVariant arg;

        GblVariant_constructObjectCopy(&arg, GBL_OBJECT(pSelf));

        // invoke, storing result code
        GBL_CTX_CALL(GblClosure_invoke(pClosure, &ret, 1, &arg));

        // only extract result code if previous result succeeded
        if(GBL_RESULT_SUCCESS(GBL_CTX_RESULT())) {
            // extract result as current code
            GBL_CTX_CALL(GblVariant_toEnum(&ret));
        }

        GblVariant_destruct(&ret);
        GblVariant_destruct(&arg);
    }

    GBL_CTX_END();
}

// Low-level thread exit point
static GBL_RESULT GblThread_exit_(GblThread* pSelf) {
    // update status for finished
    pCurThread_->state = GBL_THREAD_STATE_FINISHED;

    // copy thread-local status into this thread's return status
    memcpy(&pSelf->returnStatus, GblThd_callRecord(NULL), sizeof(GblCallRecord));

    // remove thread from active list
    GblThread_unregister_(pSelf);

    // inform connected slots that thread has finished
    GBL_EMIT(pSelf, "finished", pSelf->returnStatus.result);

    // commit suicide and clean itself up if detached
    GBL_UNREF(pSelf);

    // return casted result code
    return pSelf->returnStatus.result;
}

// Low-level thread entry point
static int GblThread_start_(void* pThread) {
    // cast userdata back to GblThread instance
    GblThread* pSelf = GBL_THREAD(pThread);

    // initialize thread settings
    GblThread_initSelf_(pSelf, GBL_FALSE);

    // update status
    pCurThread_->state = GBL_THREAD_STATE_RUNNING;

    // inform connected slots that thread has started
    GBL_EMIT(pSelf, "started");

    // clear thread-local status
    GBL_CTX_BEGIN(NULL);
    // perform actual virtual method dispatch for called logic
    GBL_VCALL(GblThread, pFnRun, pSelf);
    // end capturing thread status
    GBL_CTX_END_BLOCK();

    return GblThread_exit_(pSelf);
}

GBL_EXPORT size_t GblThread_count(void) {
    call_once(&initOnce_, &GblThread_init_);
    mtx_lock(&listMtx_);
    const size_t size = GblDoublyLinkedList_count(&threadList_);
    mtx_unlock(&listMtx_);
    return size;
}

GBL_EXPORT GBL_RESULT GblThread_start(GblThread* pSelf) {
    call_once(&initOnce_, &GblThread_init_);
    GBL_CTX_BEGIN(NULL);

    GblThread_* pSelf_ = GBL_THREAD_(pSelf);

    GBL_CTX_VERIFY(pSelf->state == GBL_THREAD_STATE_READY,
                   GBL_RESULT_ERROR_INVALID_OPERATION,
                   "Failed to start thread that isn't READY!");

    GblThread_register_(pSelf);

    /* Notice we IMMEDIATEY give the new thread its own reference,
       before we return to the caller who could possibly unref and
       delete it. */
    const int result = thrd_create(&pSelf_->nativeHandle,
                                  GblThread_start_,
                                  GBL_REF(pSelf));

    GBL_CTX_VERIFY(result == thrd_success,
                   GBL_RESULT_ERROR_INVALID_OPERATION,
                   "Failed to create thread with code: %d",
                   result);

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblThread_join(GblThread* pSelf) {
    GblThread_* pSelf_ = GBL_THREAD_(pSelf);

    GBL_CTX_BEGIN(NULL);

    GBL_CTX_VERIFY(pSelf->state != GBL_THREAD_STATE_UNKNOWN &&
                   pSelf->state <= GBL_THREAD_STATE_FINISHED,
                   GBL_RESULT_ERROR_INVALID_OPERATION,
                   "Attempt to join thread [%s] in invalid state: [%u]",
                   GblObject_name(GBL_OBJECT(pSelf)),
                   pSelf->state);

    GBL_CTX_VERIFY(!pSelf_->joined,
                   GBL_RESULT_ERROR_INVALID_OPERATION,
                   "Attempt to join already joined thread: [%s]",
                   GblObject_name(GBL_OBJECT(pSelf)));

    int result;
    const int status = thrd_join(pSelf_->nativeHandle, &result);

    GBL_CTX_VERIFY(status == thrd_success,
                   GBL_RESULT_ERROR_INVALID_OPERATION,
                   "Failed to join thread[%s]: [%d]",
                   GblObject_name(GBL_OBJECT(pSelf)),
                   status);

    pSelf_->joined = GBL_TRUE;

    GBL_CTX_RESULT() = result;

    GBL_CTX_END();
}

GBL_EXPORT GblBool GblThread_isJoined(const GblThread* pSelf) {
    return GBL_THREAD_(pSelf)->joined;
}

GBL_EXPORT GblThread* GblThread_current(void) {
    if(!pCurThread_) {
        GblThread_initUnknown_(!GblThread_count()? GBL_TRUE : GBL_FALSE);
    }
    return pCurThread_;
}

GBL_EXPORT GblClosure* GblThread_closure(const GblThread* pSelf) {
    return (GblClosure*)GblBox_field(GBL_BOX(pSelf), closureQuark_);
}

GBL_EXPORT GblThreadFn GblThread_callback(const GblThread* pSelf) {
    return (GblThreadFn)GblBox_field(GBL_BOX(pSelf), callbackQuark_);
}

static GBL_RESULT GblThread_closureDtor_(const GblArrayMap* pMap, uintptr_t key, void* pUd) {
    GBL_UNUSED(pMap, key);
    GBL_UNREF(pUd);
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT void GblThread_setClosure(GblThread* pSelf, GblClosure* pClosure) {
    GblBox_setField(GBL_BOX(pSelf), closureQuark_, (uintptr_t)pClosure, GblThread_closureDtor_);
}

GBL_EXPORT void GblThread_setCallback(GblThread* pSelf, GblThreadFn pCb) {
    GblBox_setField(GBL_BOX(pSelf), callbackQuark_, (uintptr_t)pCb, NULL);
}

GBL_EXPORT GblThread* (GblThread_create)(GblThreadFn pCallback,
                                         void*       pUserdata,
                                         GblBool     autoStart)
{
    GblThread* pThread = GBL_NEW(GblThread,
                                 "callback", pCallback,
                                 "userdata", pUserdata);

    if(autoStart) GblThread_start(pThread);

    return pThread;
}

GBL_EXPORT GblThread* GblThread_ref(const GblThread* pSelf) {
    return GBL_THREAD(GBL_REF(pSelf));
}

GBL_EXPORT GblRefCount GblThread_unref(GblThread* pSelf) {
    return GBL_UNREF(pSelf);
}

GBL_EXPORT GBL_RESULT GblThread_yield(void) {
    GBL_CTX_BEGIN(NULL);

    GblThread* pSelf = GblThread_current();

    GBL_CTX_VERIFY(pSelf,
                   GBL_RESULT_ERROR_INVALID_THREAD);

    thrd_yield();

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT (GblThread_nanoSleep)(uint64_t  duration,
                                            uint64_t* pRemainder) {
    GBL_CTX_BEGIN(NULL);
    GblThread* pSelf = GblThread_current();

    GBL_CTX_VERIFY(pSelf,
                   GBL_RESULT_ERROR_INVALID_THREAD);

    struct timespec rem = { 0 };
    const int retVal = thrd_sleep(&(struct timespec) {
                   .                  tv_nsec = duration
                                  },
                                  &rem);

    if(pRemainder)
        *pRemainder = rem.tv_nsec;

    GBL_CTX_VERIFY(retVal != -1,
                   GBL_RESULT_INCOMPLETE,
                   "Thread [%s] was woken prematurely!",
                   GblObject_name(GBL_OBJECT(pSelf)));

    GBL_CTX_VERIFY(retVal == 0,
                   GBL_RESULT_ERROR_INVALID_OPERATION,
                   "Thread [%s] failed to sleep for %u nsec.",
                   GblObject_name(GBL_OBJECT(pSelf)),
                   duration);

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT (GblThread_exit)(GBL_RESULT  result,
                                       const char* pMessage,
                                       const char* pFile,
                                       const char* pFunc,
                                       size_t      line)
{
    GBL_CTX_BEGIN(NULL);

    GblThread* pSelf = GblThread_current();

    GBL_CTX_VERIFY(pSelf,
                   GBL_RESULT_ERROR_INVALID_THREAD);

    GBL_CTX_RESULT() = result;
    if(pMessage) strcpy(GBL_CTX_RECORD().message, pMessage);
    GBL_CTX_RECORD().srcLocation.pFile = pFile;
    GBL_CTX_RECORD().srcLocation.pFunc = pFunc;
    GBL_CTX_RECORD().srcLocation.line  = line;
    GblThd_setCallRecord(NULL, &GBL_CTX_RECORD());

    GBL_CTX_VERIFY_CALL(GblThread_exit_(pSelf));

    GBL_CTX_END();

}

static GBL_RESULT GblThread_GblObject_constructed_(GblObject* pObject) {
    GBL_CTX_BEGIN(NULL);

    GblThread* pSelf = GBL_THREAD(pObject);
    pSelf->state = GBL_THREAD_STATE_READY;

    GBL_CTX_END();
}

static GBL_RESULT GblThread_GblObject_setProperty_(GblObject* pObject, const GblProperty* pProp, GblVariant* pValue) {
    GBL_CTX_BEGIN(NULL);

    GblThread*  pSelf  = GBL_THREAD(pObject);
    GblThread_* pSelf_ = GBL_THREAD_(pSelf);

    switch(pProp->id) {
    case GblThread_Property_Id_priority:
        GblThread_setPriority(pSelf, GblVariant_toEnum(pValue)); break;
    case GblThread_Property_Id_affinity:
        GblThread_setAffinity(pSelf, GblVariant_toFlags(pValue)); break;
    case GblThread_Property_Id_closure: {
        GblClosure* pClosure = NULL;
        GblVariant_valueMove(pValue, &pClosure);
        GblThread_setClosure(pSelf, pClosure);
        break;
    }
    case GblThread_Property_Id_callback:
        GblThread_setCallback(pSelf, GblVariant_toPointer(pValue));
        break;
    default:
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY,
                           "Failed to set property %s for type %s",
                           GblProperty_nameString(pProp), GblType_name(GBL_TYPEOF(pSelf)));
    }
    GBL_CTX_END();
}

static GBL_RESULT GblThread_GblObject_property_(const GblObject* pObject, const GblProperty* pProp, GblVariant* pValue) {
    GBL_CTX_BEGIN(NULL);

    GblThread*  pSelf  = GBL_THREAD(pObject);
    GblThread_* pSelf_ = GBL_THREAD_(pSelf);

    switch(pProp->id) {
    case GblThread_Property_Id_result:
        GblVariant_setEnum(pValue, GBL_ENUM_TYPE, pSelf->returnStatus.result); break;
    case GblThread_Property_Id_state:
        GblVariant_setEnum(pValue, GBL_ENUM_TYPE, pSelf->state); break;
    case GblThread_Property_Id_signal:
        GblVariant_setEnum(pValue, GBL_ENUM_TYPE, pSelf->signalStatus); break;
    case GblThread_Property_Id_closure:
        GblVariant_setValueCopy(pValue, GBL_CLOSURE_TYPE, GblThread_closure(pSelf)); break;
    case GblThread_Property_Id_callback:
        GblVariant_setPointer(pValue, GBL_POINTER_TYPE, GblThread_callback(pSelf)); break;
    case GblThread_Property_Id_joined:
        GblVariant_setBool(pValue, GblThread_isJoined(pSelf)); break;
    default:
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY,
                           "Failed to get property %s for type %s",
                           GblProperty_nameString(pProp), GblType_name(GBL_TYPEOF(pSelf)));
    }

    GBL_CTX_END();
}

static GBL_RESULT GblThread_GblBox_destructor_(GblBox* pBox) {
    GBL_CTX_BEGIN(NULL);

    GblThread*  pSelf = GBL_THREAD(pBox);
    GblThread_* pSelf_  = GBL_THREAD_(pSelf);

    if(!pSelf_->joined) {
        const int retVal = thrd_detach(pSelf_->nativeHandle);
        GBL_CTX_VERIFY(retVal == thrd_success,
                       GBL_RESULT_ERROR_INVALID_OPERATION,
                       "Failed to detach thread[%s]: [%d]",
                       GblObject_name(GBL_OBJECT(pSelf)),
                       retVal);

    }

    GBL_VCALL_DEFAULT(GblObject, base.pFnDestructor, GBL_BOX(pBox));

    GBL_CTX_END();
}

static GBL_RESULT GblThread_initialize_(GblInstance* pInstance) {
    GBL_CTX_BEGIN(NULL);
    GblThread*  pThread  = GBL_THREAD(pInstance);
    pThread->state = GBL_THREAD_STATE_INITIALIZING;
    GBL_CTX_END();
}

static GBL_RESULT GblThreadClass_initialize_(GblClass* pClass, const void* pUd) {
    GBL_UNUSED(pUd);
    GBL_CTX_BEGIN(NULL);

    if(!GblType_classRefCount(GBL_THREAD_TYPE)) {
        GBL_PROPERTIES_REGISTER(GblThread);

        GblSignal_install(GBL_THREAD_TYPE,
                          "started",
                          GblMarshal_CClosure_VOID__INSTANCE,
                          0);

        GblSignal_install(GBL_THREAD_TYPE,
                          "finished",
                          GblMarshal_CClosure_VOID__INSTANCE_ENUM,
                          1,
                          GBL_ENUM_TYPE);

        GblSignal_install(GBL_THREAD_TYPE,
                          "signaled",
                          GblMarshal_CClosure_VOID__INSTANCE_ENUM,
                          1,
                          GBL_ENUM_TYPE);

        closureQuark_  = GblQuark_fromStatic("_GblThread_closure");
        callbackQuark_ = GblQuark_fromStatic("_GblThread_callback");
    }

    GBL_BOX_CLASS(pClass)   ->pFnDestructor  = GblThread_GblBox_destructor_;
    GBL_OBJECT_CLASS(pClass)->pFnConstructed = GblThread_GblObject_constructed_;
    GBL_OBJECT_CLASS(pClass)->pFnProperty    = GblThread_GblObject_property_;
    GBL_OBJECT_CLASS(pClass)->pFnSetProperty = GblThread_GblObject_setProperty_;
    GBL_THREAD_CLASS(pClass)->pFnRun         = GblThread_run_;
    GBL_THREAD_CLASS(pClass)->pFnSignal      = GblThread_signal_;

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblThread_setPriority(GblThread* pSelf,
                                            GBL_THREAD_PRIORITY priority) {
    GBL_UNUSED(pSelf, priority);
    return GBL_RESULT_UNIMPLEMENTED;
}

GBL_EXPORT GBL_RESULT GblThread_setAffinity(GblThread* pSelf,
                                            GblThreadAffinity affinity) {
    GBL_UNUSED(pSelf, affinity);
    return GBL_RESULT_UNIMPLEMENTED;
}

GBL_EXPORT const char* GblThread_name(const GblThread* pSelf) {
    return GblObject_name(GBL_OBJECT(pSelf));
}

GBL_EXPORT GBL_RESULT GblThread_setName(GblThread* pSelf, const char* pName) {
    GblObject_setName(GBL_OBJECT(pSelf), pName);
    return GBL_RESULT_UNIMPLEMENTED;
}

GBL_EXPORT GblType GblThread_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static const GblTypeInfo info = {
        .classSize           = sizeof(GblThreadClass),
        .pFnClassInit        = GblThreadClass_initialize_,
        .instanceSize        = sizeof(GblThread),
        .instancePrivateSize = sizeof(GblThread_),
        .pFnInstanceInit     = GblThread_initialize_
    };

    if(type == GBL_INVALID_TYPE) GBL_UNLIKELY {
        type = GblType_register(GblQuark_internStatic("GblThread"),
                                GBL_OBJECT_TYPE,
                                &info,
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}

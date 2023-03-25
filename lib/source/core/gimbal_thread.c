#include <gimbal/core/gimbal_thread.h>
#include <gimbal/containers/gimbal_array_map.h>
#include <gimbal/utils/gimbal_date_time.h>
#include <tinycthread.h>

#define GBL_THREAD_(self)   ((GblThread_*)GBL_INSTANCE_PRIVATE(self, GBL_THREAD_TYPE))

GBL_DECLARE_STRUCT(GblThread_) {
    thrd_t  nativeHandle;
    struct {
        uint32_t autoStart : 1;
        uint32_t detached  : 1;
    };
};

static mtx_t        mapMtx_;
static once_flag    initOnce_       = ONCE_FLAG_INIT;
static GblArrayMap* pThreadMap_     = NULL;

static GblQuark     closureQuark_   = GBL_QUARK_INVALID;
static GblQuark     callbackQuark_  = GBL_QUARK_INVALID;

static GBL_THREAD_LOCAL
GblThread*          pCurThread_     = NULL;

static void GblThread_finalize_(void) {
    mtx_lock(&mapMtx_);
    GblArrayMap_destroy(&pThreadMap_);
    mtx_unlock(&mapMtx_);
    mtx_destroy(&mapMtx_);
}

static void GblThread_initialize_(void) {
    mtx_init(&mapMtx_, mtx_plain);
    pThreadMap_ = GblArrayMap_create(NULL, GBL_TRUE, NULL);
    atexit(GblThread_finalize_);
}

static void GblThread_register_(GblThread* pSelf) {
    GBL_UNUSED(pSelf);
    call_once(&initOnce_, &GblThread_initialize_);

    mtx_lock(&mapMtx_);

    // add to vector

    mtx_unlock(&mapMtx_);
}

static void GblThread_unregister_(GblThread* pSelf) {
    GBL_UNUSED(pSelf);
    mtx_lock(&mapMtx_);

    // remove from vector

    mtx_unlock(&mapMtx_);
}

static void GblThread_initSelf_(GblThread* pSelf) {
    pCurThread_  = pSelf;
    GblThread_* pSelf_ = GBL_THREAD_(pSelf);
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
        snprintf(buffer, sizeof(buffer), "%p", (void*)GblThread_nativeHandle_());
    } else {
        strcpy(buffer, "main");
        signal(SIGTERM, GblThread_signalHandler_);
    }

    pCurThread_ = GBL_OBJECT_NEW(GblThread,
                                 "name", buffer);
    pCurThread_->state = GBL_THREAD_STATE_UNKNOWN;
    GblThread_initSelf_(pCurThread_);
    GblThread_register_(pCurThread_);

    return pCurThread_;
}

static GBL_RESULT GblThread_signal_(GblThread* pSelf, int signal) {
    GBL_CTX_BEGIN(NULL);
    GblSignal_emit(GBL_INSTANCE(pSelf), "signaled", signal);
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
    GblThread_* pSelf_ = GBL_THREAD_(pSelf);

    // update status for finished
    pCurThread_->state = GBL_THREAD_STATE_FINISHED;

    // copy thread-local status into this thread's return status
    memcpy(&pSelf->returnStatus, GblThd_callRecord(NULL), sizeof(GblCallRecord));

    // inform connected slots that thread has finished
    GblSignal_emit(GBL_INSTANCE(pSelf), "finished", pSelf->returnStatus.result);

    // commit suicide and clean itself up if detached
    if(pSelf_->detached) GBL_BOX_UNREF(pSelf);

    // return casted result code
    return pSelf->returnStatus.result;
}

// Low-level thread entry point
static int GblThread_start_(void* pThread) {
    // cast userdata back to GblThread instance
    GblThread* pSelf = GBL_THREAD(pThread);

    // initialize thread settings
    GblThread_initSelf_(pSelf);

    // update status
    pCurThread_->state = GBL_THREAD_STATE_RUNNING;

    // inform connected slots that thread has started
    GblSignal_emit(GBL_INSTANCE(pSelf), "started");

    // clear thread-local status
    GBL_CTX_BEGIN(NULL);
    // perform actual virtual method dispatch for called logic
    GBL_INSTANCE_VCALL(GblThread, pFnRun, pSelf);
    // end capturing thread status
    GBL_CTX_END_BLOCK();

    return GblThread_exit_(pSelf);
}

GBL_EXPORT GBL_RESULT GblThread_start(GblThread* pSelf) {
    GBL_CTX_BEGIN(NULL);

    GblThread_* pSelf_ = GBL_THREAD_(pSelf);

    GBL_CTX_VERIFY(pSelf->state == GBL_THREAD_STATE_READY,
                   GBL_RESULT_ERROR_INVALID_OPERATION,
                   "Failed to start thread that isn't READY!");

    /* Notice we IMMEDIATEY give the new thread its own reference,
       before we return to the caller who could possibly unref and
       delete it. */
    int result = thrd_create(&pSelf_->nativeHandle,
                             GblThread_start_,
                             GBL_BOX_REF(pSelf));

    GBL_CTX_VERIFY(result == thrd_success,
                   GBL_RESULT_ERROR_INVALID_OPERATION,
                   "Failed to create thread with code: %d",
                   result);

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblThread_wait(GblThread* pSelf) {
    GblThread_* pSelf_ = GBL_THREAD_(pSelf);

    GBL_CTX_BEGIN(NULL);

    GBL_CTX_VERIFY(pSelf->state != GBL_THREAD_STATE_UNKNOWN &&
                   pSelf->state < GBL_THREAD_STATE_FINISHED,
                   GBL_RESULT_ERROR_INVALID_OPERATION,
                   "Attempt to wait on thread [%s] in invalid state: [%u]",
                   GblObject_name(GBL_OBJECT(pSelf)),
                   pSelf->state);

    GBL_CTX_VERIFY(!pSelf_->detached,
                   GBL_RESULT_ERROR_INVALID_OPERATION,
                   "Attempt to wait on detached thread: [%s]",
                   GblObject_name(GBL_OBJECT(pSelf)));

    int result;
    const int status = thrd_join(pSelf_->nativeHandle, &result);

    GBL_CTX_VERIFY(status == thrd_success,
                   GBL_RESULT_ERROR_INVALID_OPERATION,
                   "Failed to wait on thread[%s]: [%d]",
                   GblObject_name(GBL_OBJECT(pSelf)),
                   status);


    GBL_CTX_RESULT() = result;

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblThread_detach(GblThread* pSelf) {
    GblThread_* pSelf_ = GBL_THREAD_(pSelf);

    GBL_CTX_BEGIN(NULL);

    GBL_CTX_VERIFY(pSelf->state != GBL_THREAD_STATE_UNKNOWN,
                   GBL_RESULT_ERROR_INVALID_OPERATION,
                   "Attempt to detach thread [%s] in invalid state: [%u]",
                   GblObject_name(GBL_OBJECT(pSelf)),
                   pSelf->state);

    GBL_CTX_VERIFY(!pSelf_->detached,
                   GBL_RESULT_ERROR_INVALID_OPERATION,
                   "Attempt to detach an already detached thread: [%s]",
                   GblObject_name(GBL_OBJECT(pSelf)));

    const int status = thrd_detach(pSelf_->nativeHandle);

    GBL_CTX_VERIFY(status == thrd_success,
                   GBL_RESULT_ERROR_INVALID_OPERATION,
                   "Failed to detach thread[%s]: [%d]",
                   GblObject_name(GBL_OBJECT(pSelf)),
                   status);

    GBL_CTX_END();
}

GBL_EXPORT GblThread* GblThread_current(void) {
    if(!pCurThread_) {
        GblThread_initUnknown_(!GblThread_count()? GBL_TRUE : GBL_FALSE);
    }
    return pCurThread_;
}

GBL_EXPORT GblThread* GblThread_main(void) {
    return NULL;
}

GBL_EXPORT GblClosure* GblThread_closure(const GblThread* pSelf) {
    return GBL_CLOSURE(GblBox_getField(GBL_BOX(pSelf), closureQuark_));
}

GBL_EXPORT GblThreadFn GblThread_callback(const GblThread* pSelf) {
    return (GblThreadFn)GblBox_getField(GBL_BOX(pSelf), callbackQuark_);
}

static GBL_RESULT GblThread_closureDtor_(const GblArrayMap* pMap, uintptr_t key, void* pUd) {
    GBL_UNUSED(pMap, key);
    GBL_BOX_UNREF(pUd);
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
    return GBL_THREAD(GBL_OBJECT_NEW(GblThread,
                                     "callback",  pCallback,
                                     "userdata",  pUserdata,
                                     "autoStart", autoStart));
}

GBL_EXPORT GblRefCount GblThread_unref(GblThread* pSelf) {
    return GBL_BOX_UNREF(pSelf);
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

    GBL_CTX_VERIFY(retVal == thrd_success,
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

    if(GBL_THREAD_(pSelf)->autoStart) {
        GblThread_start(pSelf);
    } else {
        pSelf->state = GBL_THREAD_STATE_READY;
    }

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
    case GblThread_Property_Id_autoStart:
        pSelf_->autoStart = GblVariant_toBool(pValue); break;
    case GblThread_Property_Id_detached:
        if(GblVariant_toBool(pValue)) GblThread_detach(pSelf); break;
    case GblThread_Property_Id_closure: {
        GblClosure* pClosure = NULL;
        GblVariant_getValueMove(pValue, &pClosure);
        GblThread_setClosure(pSelf, pClosure);
        break;
    }
    case GblThread_Property_Id_callback:
        GblThread_setCallback(pSelf, GblVariant_toPointer(pValue));
        break;
    default:
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY,
                           "Failed to set property %s for type %s",
                           GblProperty_nameString(pProp), GBL_INSTANCE_TYPEOF(pSelf));
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
    case GblThread_Property_Id_autoStart:
        GblVariant_setBool(pValue, pSelf_->autoStart); break;
    case GblThread_Property_Id_closure:
        GblVariant_setValueCopy(pValue, GBL_CLOSURE_TYPE, GblThread_closure(pSelf)); break;
    case GblThread_Property_Id_callback:
        GblVariant_setPointer(pValue, GBL_POINTER_TYPE, GblThread_callback(pSelf)); break;
    default:
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY,
                           "Failed to get property %s for type %s",
                           GblProperty_nameString(pProp), GBL_INSTANCE_TYPEOF(pSelf));
    }

    GBL_CTX_END();
}

static GBL_RESULT GblThread_GblBox_destructor_(GblBox* pBox) {
    GBL_CTX_BEGIN(NULL);
    GblThread* pThread = GBL_THREAD(pBox);
    pThread->state = GBL_THREAD_STATE_DESTRUCTING;
    GblThread_unregister_(pThread);
    GBL_CTX_END();
}

static GBL_RESULT GblThread_init_(GblInstance* pInstance, GblContext* pCtx) {
    GBL_UNUSED(pCtx);
    GBL_CTX_BEGIN(NULL);
    GblThread* pThread = GBL_THREAD(pInstance);
    pThread->state = GBL_THREAD_STATE_CONSTRUCTING;
    GblThread_register_(pThread);
    GBL_CTX_END();
}

static GBL_RESULT GblThreadClass_init_(GblClass* pClass, const void* pUd, GblContext* pCtx) {
    GBL_UNUSED(pUd, pCtx);
    GBL_CTX_BEGIN(NULL);

    if(!GblType_classRefCount(GBL_CLASS_TYPEOF(pClass))) {
        GBL_PROPERTIES_REGISTER(GblThread);

        closureQuark_  = GblQuark_fromStringStatic("_GblThread_closure");
        callbackQuark_ = GblQuark_fromStringStatic("_GblThread_callback");
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

GBL_EXPORT size_t  GblThread_coreCount(void) {
    return 1;
}

GBL_EXPORT GblBool GblThread_isMain(const GblThread* pSelf) {
    // reested, fixme! POSIX: pid == tid
    return pSelf->state == GBL_THREAD_STATE_UNKNOWN;
}

GBL_EXPORT GblType GblThread_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static const GblTypeInfo info = {
        .classSize           = sizeof(GblThreadClass),
        .pFnClassInit        = GblThreadClass_init_,
        .instanceSize        = sizeof(GblThread),
        .instancePrivateSize = sizeof(GblThread_),
        .pFnInstanceInit     = GblThread_init_
    };

    if(type == GBL_INVALID_TYPE) {
        type = GblType_registerStatic(GblQuark_internStringStatic("GblThread"),
                                      GBL_OBJECT_TYPE,
                                      &info,
                                      GBL_TYPE_FLAG_TYPEINFO_STATIC |
                                      GBL_TYPE_FLAG_CLASS_PINNED    |
                                      GBL_TYPE_FLAG_CLASS_PREINIT);
    }

    return type;
}

#include <gimbal/core/gimbal_thread.h>
#include <gimbal/containers/gimbal_array_map.h>

#ifdef _WIN32
#   define NOGDI
# else
#   define __USE_UNIX98
#endif
#include <tinycthread.h>

#define GBL_THREAD_(self)   ((GblThread_*)GBL_INSTANCE_PRIVATE(self, GBL_THREAD_TYPE))

GBL_DECLARE_STRUCT(GblThread_) {
    thrd_t  nativeThread;
    GblBool autoStart;
    GblBool detached;
};

static mtx_t        mapMtx_;
static once_flag    initOnce_       = ONCE_FLAG_INIT;
static GblArrayMap* pThreadMap_     = NULL;

static GblQuark     closureQuark_   = GBL_QUARK_INVALID;
static GblQuark     callbackQuark_  = GBL_QUARK_INVALID;

GBL_THREAD_LOCAL    GblThread*  pCurThread_  = NULL;
GBL_THREAD_LOCAL    GblThread_* pCurThread__ = NULL;


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
    call_once(&initOnce_, &GblThread_initialize_);

    mtx_lock(&mapMtx_);

    // add to vector

    mtx_unlock(&mapMtx_);
}

static void GblThread_unregister_(GblThread* pSelf) {

    mtx_lock(&mapMtx_);

    // remove from vector

    mtx_unlock(&mapMtx_);
}

static void GblThread_initSelf_(GblThread* pSelf) {
    pCurThread_  = pSelf;
    pCurThread__ = GBL_THREAD_(pSelf);
    pCurThread__->nativeThread = (uintptr_t)thrd_current();
    // set up thread priority
    // set up thread affinity
    // set up thread name
}

// default implementation of run virtual function
static GBL_RESULT GblThread_run_(GblThread* pSelf) {
    GBL_CTX_BEGIN(NULL);

    // fetch C callback function from box field
    GblThreadFn pCallback =
            (GblThreadFn)(GblBox_getField(GBL_BOX(pSelf), callbackQuark_));

    // invoke if we have it
    if(pCallback) {
        GBL_CTX_VERIFY_CALL(pCallback(pSelf));
    }

    // fetch generic closure from box field
    GblClosure* pClosure =
            GBL_CLOSURE(GblBox_getField(GBL_BOX(pSelf), closureQuark_));

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

// Low-level thread entry point
static int GblThread_start_(void* pThread) {
    // cast userdata back to GblThread instance
    GblThread* pSelf = GBL_THREAD(pThread);
    // retreive private instance struct
    GblThread_* pSelf_ = GBL_THREAD_(pSelf);

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

    // update status for finished
    pCurThread_->state = GBL_THREAD_STATE_FINISHED;

    // copy thread-local status into this thread's return status
    memcpy(&pSelf->returnStatus, GblThd_callRecord(NULL), sizeof(GblCallRecord));

    // inform connected slots that thread has finished
    GblSignal_emit(GBL_INSTANCE(pSelf), "finished", pSelf->returnStatus.result);

    // commit suicide and clean itself up if detached
    if(pSelf_->detached) GBL_BOX_UNREF(pSelf);

    // return casted result code
    return (int)pSelf->returnStatus.result;
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
    int result = thrd_create(&pSelf_->nativeThread,
                             GblThread_start_,
                             GBL_BOX_REF(pSelf));

    GBL_CTX_VERIFY(result == thrd_success,
                   GBL_RESULT_ERROR_INVALID_OPERATION,
                   "Failed to create thread with code: %d",
                   result);

    GBL_CTX_END();

}

GBL_EXPORT GblThread* GblThread_current(void) {
    if(!pCurThread_) {
        pCurThread_ = GBL_OBJECT_NEW(GblThread);
        pCurThread_->state = GBL_THREAD_STATE_RUNNING;
        GblThread_initSelf_(pCurThread_);
        GblThread_register_(pCurThread_);
    }
    return pCurThread_;
}


static GBL_RESULT GblThread_GblObject_constructed_(GblObject* pObject) {
    GBL_CTX_BEGIN(NULL);

    GblThread* pSelf = GBL_THREAD(pObject);

    if(GBL_THREAD_(pSelf)->autoStart) {
        GblThread_start(pSelf);
    }

    GBL_CTX_END();
}

static GBL_RESULT GblThread_GblObject_setProperty_(GblObject* pSelf, const GblProperty* pProp, GblVariant* pValue) {
    GBL_CTX_BEGIN(NULL);
    (void)pValue;
    switch(pProp->id) {
    default:
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY,
                           "Failed to set property %s for type %s",
                           GblProperty_nameString(pProp), GBL_INSTANCE_TYPEOF(pSelf));
    }
    GBL_CTX_END();
}

static GBL_RESULT GblThread_GblObject_property_(const GblObject* pSelf, const GblProperty* pProp, GblVariant* pValue) {
    GBL_CTX_BEGIN(NULL);
    (void)pValue;
    switch(pProp->id) {
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

    GBL_CTX_END();
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

GBL_EXPORT GBL_RESULT GblThread_setName(GblThread* pSelf, const char* pName) {
    GblObject_setName(GBL_OBJECT(pSelf), pName);
    return GBL_RESULT_UNIMPLEMENTED;
}

GBL_EXPORT GblSize GblThread_coreCount(void) {
    return 1;
}

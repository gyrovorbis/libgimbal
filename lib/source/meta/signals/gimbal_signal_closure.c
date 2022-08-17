#include <gimbal/meta/signals/gimbal_signal_closure.h>

GBL_EXPORT GBL_RESULT GblSignalClosure_setSignal(GblSignalClosure* pSelf, const char* pName)
{
    GBL_RESULT result = GBL_RESULT_SUCCESS;
    GblQuark quark = GblQuark_tryString(pName);
    if(quark == GBL_QUARK_INVALID) {
        GBL_API_BEGIN(NULL);
        GBL_API_VERIFY_ARG(quark != GBL_QUARK_INVALID);
        GBL_API_END_BLOCK();
        result = GBL_API_RESULT();
    } else {
        GBL_PRIV_REF(pSelf).signalName = quark;
    }
    return result;
}

GBL_EXPORT GblSignalClosure* GblSignalClosure_create(const char*  pSignalName,
                                                     void*        pUserdata)
{
    GblSignalClosure* pClosure = NULL;
    GBL_API_BEGIN(NULL);
    pClosure = GBL_SIGNAL_CLOSURE(GblClosure_create(GBL_SIGNAL_CLOSURE_TYPE,
                                                    sizeof(GblSignalClosure),
                                                    pUserdata,
                                                    NULL));

    GBL_API_VERIFY_CALL(GblSignalClosure_setSignal(pClosure,
                                                   pSignalName));
    GBL_API_END_BLOCK();
    return pClosure;
}

static GBL_RESULT GblSignalClosure_init_(GblInstance* pInstance, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblClosure* pClosure = GBL_CLOSURE(pInstance);
    GblClosure_setMarshal(pClosure, GblMarshal_SignalForwarder);
    GBL_API_END();
}

GBL_EXPORT GblType GblSignalClosure_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static GblTypeInfo info = {
        .classSize         = sizeof(GblSignalClosureClass),
        .pFnInstanceInit   = GblSignalClosure_init_,
        .instanceSize      = sizeof(GblSignalClosure)
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_API_BEGIN(NULL);

        type = GblType_registerStatic(GblQuark_internStringStatic("GblSignalClosure"),
                                      GBL_CLOSURE_TYPE,
                                      &info,
                                      GBL_TYPE_FLAG_TYPEINFO_STATIC);



        GBL_API_VERIFY_LAST_RECORD();
        GBL_API_END_BLOCK();

    }
    return type;
}

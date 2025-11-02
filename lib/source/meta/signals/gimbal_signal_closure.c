#include <gimbal/meta/signals/gimbal_signal_closure.h>

GBL_EXPORT GBL_RESULT GblSignalClosure_setSignal(GblSignalClosure* pSelf, const char* pName)
{
    GBL_RESULT result = GBL_RESULT_SUCCESS;
    GblQuark quark = GblQuark_tryString(pName);
    if(quark == GBL_QUARK_INVALID) {
        GBL_CTX_BEGIN(NULL);
        GBL_CTX_VERIFY_ARG(quark != GBL_QUARK_INVALID);
        GBL_CTX_END_BLOCK();
        result = GBL_CTX_RESULT();
    } else {
        GBL_PRIV_REF(pSelf).signalName = quark;
    }
    return result;
}

GBL_EXPORT GblSignalClosure* GblSignalClosure_create(const char*  pSignalName,
                                                     void*        pUserdata)
{
    GblSignalClosure* pClosure = NULL;
    GBL_CTX_BEGIN(NULL);
    pClosure = GBL_SIGNAL_CLOSURE(GblClosure_create(GBL_SIGNAL_CLOSURE_TYPE,
                                                    sizeof(GblSignalClosure),
                                                    pUserdata,
                                                    NULL));

    GBL_CTX_VERIFY_CALL(GblSignalClosure_setSignal(pClosure,
                                                   pSignalName));
    GBL_CTX_END_BLOCK();
    return pClosure;
}

static GBL_RESULT GblSignalClosure_init_(GblInstance* pInstance) {
    GBL_CTX_BEGIN(NULL);
    GblClosure* pClosure = GBL_CLOSURE(pInstance);
    GblClosure_setMarshal(pClosure, GblMarshal_SignalForwarder);
    GBL_CTX_END();
}

GBL_EXPORT GblType GblSignalClosure_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static GblTypeInfo info = {
        .classSize         = sizeof(GblSignalClosureClass),
        .pFnInstanceInit   = GblSignalClosure_init_,
        .instanceSize      = sizeof(GblSignalClosure)
    };

    if GBL_UNLIKELY(type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GblSignalClosure"),
                                GBL_CLOSURE_TYPE,
                                &info,
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}

#include <gimbal/meta/signals/gimbal_c_closure.h>
#include <gimbal/strings/gimbal_quark.h>
#include <gimbal/core/gimbal_ctx.h>

GBL_EXPORT GblCClosure* GblCClosure_create(GblFnPtr pFnCallback, void* pUserdata) {
    GblCClosure* pClosure = NULL;
    GBL_CTX_BEGIN(NULL);

    pClosure = GBL_C_CLOSURE(GblClosure_create(GBL_C_CLOSURE_TYPE,
                                               sizeof(GblCClosure),
                                               pUserdata,
                                               NULL));

    GblCClosure_setCallback(pClosure, pFnCallback);
    GBL_CTX_END_BLOCK();
    return pClosure;
}

GBL_EXPORT void GblCClosure_setCallback(GblCClosure* pSelf, GblFnPtr pFnCallback) {
    GBL_PRIV_REF(pSelf).pFnCallback = pFnCallback;
}

GBL_EXPORT GblType GblCClosure_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static GblTypeInfo info = {
        .classSize = sizeof(GblCClosureClass),
        .instanceSize = sizeof(GblCClosure)
    };

    if(type == GBL_INVALID_TYPE) GBL_UNLIKELY {
        type = GblType_register(GblQuark_internStringStatic("GblCClosure"),
                                GBL_CLOSURE_TYPE,
                                &info,
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }
    return type;
}

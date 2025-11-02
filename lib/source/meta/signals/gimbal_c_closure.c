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

GBL_EXPORT GblCClosure* GblCClosure_ref(GblCClosure* pSelf) {
    return (GblCClosure*)GBL_REF(pSelf);
}

GBL_EXPORT GblRefCount GblCClosure_unref(GblCClosure* pSelf) {
    return GBL_UNREF(pSelf);
}

GBL_EXPORT GblFnPtr GblCClosure_callback(const GblCClosure* pSelf) {
    return GBL_PRIV_REF(pSelf).pFnCallback;
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

    if GBL_UNLIKELY(type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GblCClosure"),
                                GBL_CLOSURE_TYPE,
                                &info,
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}

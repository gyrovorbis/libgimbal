#include <gimbal/meta/signals/gimbal_closure.h>
#include <gimbal/core/gimbal_ctx.h>
#include <gimbal/strings/gimbal_quark.h>

static GBL_THREAD_LOCAL GblClosure* pCurrentClosure_ = NULL;

GBL_EXPORT GblBool GblClosure_hasMarshal(const GblClosure* pSelf) GBL_NOEXCEPT {
    return GBL_PRIV_REF(pSelf).pFnMarshal? GBL_TRUE : GBL_FALSE;
}

GBL_EXPORT GblBool GblClosure_hasMetaMarshal(const GblClosure* pSelf) GBL_NOEXCEPT {
    return GBL_CLOSURE_GET_CLASS(pSelf)->pFnMetaMarshal? GBL_TRUE : GBL_FALSE;
}

GBL_EXPORT void GblClosure_setMarshal(GblClosure* pSelf, GblMarshalFn pFnMarshal) GBL_NOEXCEPT {
    GBL_PRIV_REF(pSelf).pFnMarshal = pFnMarshal;
}

GBL_EXPORT GblClosure* GblClosure_current(void) {
    return pCurrentClosure_;
}

GBL_EXPORT void* GblClosure_currentUserdata(void) {
    return GblBox_userdata(GBL_BOX(GblClosure_current()));
}

GBL_EXPORT GblClosure* GblClosure_create(GblType           derivedType,
                                         size_t            size,
                                         void*             pUserdata,
                                         GblArrayMapDtorFn pFnDtor)
{
    GblClosure* pClosure = NULL;
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_TYPE(derivedType, GBL_CLOSURE_TYPE);
    pClosure = GBL_CLOSURE(GblBox_create(derivedType, size, pUserdata, pFnDtor));
    GBL_CTX_END_BLOCK();
    return pClosure;
}

GBL_EXPORT GBL_RESULT GblClosure_setMetaMarshal(GblClosure* pSelf,
                                                GblMarshalFn pFnMeta)
{
    GBL_CTX_BEGIN(NULL);

    if(GblClass_isOwned(GBL_INSTANCE_GET_CLASS(pSelf))) {
        GBL_CLOSURE_GET_CLASS(pSelf)->pFnMetaMarshal = pFnMeta;
    } else {
        GblClosureClass* pClass =
            GBL_CLOSURE_CLASS(
                GblClass_createFloating(
                    GBL_TYPEOF(pSelf)));

        pClass->pFnMetaMarshal = pFnMeta;

        GBL_CTX_VERIFY_CALL(
            GblInstance_swizzleClass(GBL_INSTANCE(pSelf),
                                     GBL_CLASS(pClass)));
        GBL_CTX_VERIFY_CALL(
            GblInstance_sinkClass(GBL_INSTANCE(pSelf)));
    }

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblClosure_invoke(GblClosure*     pSelf,
                                        GblVariant*     pRetValue,
                                        size_t          argCount,
                                        GblVariant*     pArgValues)
{
    GBL_CTX_BEGIN(NULL);

    GblClosureClass* pClass = GBL_CLOSURE_GET_CLASS(pSelf);

    GblMarshalFn    pFnMarshal;
    GblPtr          pMarshalData;

    if(pClass->pFnMetaMarshal) {
        pFnMarshal = pClass->pFnMetaMarshal;
        pMarshalData.pData = NULL;
    } else {
        pFnMarshal = GBL_PRIV_REF(pSelf).pFnMarshal;
        pMarshalData.pData = NULL;
    }

    GBL_CTX_VERIFY_POINTER(pFnMarshal);

    GblClosure* prevActiveClosure = pCurrentClosure_;
    pCurrentClosure_ = pSelf;

    GBL_CTX_VERIFY_CALL(pFnMarshal(pSelf,
                                   pRetValue,
                                   argCount,
                                   pArgValues,
                                   pMarshalData));

    pCurrentClosure_ = prevActiveClosure;

    GBL_CTX_END();
}

GBL_EXPORT GblType GblClosure_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static GblTypeInfo info = {
        .classSize = sizeof(GblClosureClass),
        .instanceSize = sizeof(GblClosure)
    };

    if(type == GBL_INVALID_TYPE) GBL_UNLIKELY {
        type = GblType_register(GblQuark_internStringStatic("GblClosure"),
                                GBL_BOX_TYPE,
                                &info,
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }
    return type;

}

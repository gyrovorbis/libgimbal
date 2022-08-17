#include <gimbal/meta/signals/gimbal_closure.h>
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/strings/gimbal_quark.h>

static GBL_THREAD_LOCAL GblClosure* pCurrentClosure_ = NULL;

GBL_EXPORT GblClosure* GblClosure_current(void) {
    return pCurrentClosure_;
}

GBL_EXPORT GblClosure* GblClosure_create(GblType           derivedType,
                                         GblSize           size,
                                         void*             pUserdata,
                                         GblArrayMapDtorFn pFnDtor)
{
    GblClosure* pClosure = NULL;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_TYPE(derivedType, GBL_CLOSURE_TYPE);
    GBL_API_VERIFY_ARG(size >= sizeof(GblClosure));
    pClosure = GBL_CLOSURE(GblBox_createExt(derivedType, size, pUserdata, pFnDtor, NULL));
    GBL_API_END_BLOCK();
    return pClosure;
}

GBL_EXPORT GBL_RESULT GblClosure_setMetaMarshal(GblClosure* pSelf,
                                                GblMarshalFn pFnMeta)
{
    GBL_API_BEGIN(NULL);

    if(GblClass_isOwned(GBL_INSTANCE_CLASS(pSelf))) {
        GBL_CLOSURE_GET_CLASS(pSelf)->pFnMetaMarshal = pFnMeta;
    } else {
        GblClosureClass* pClass = GBL_CLOSURE_CLASS(GblClass_createFloating(GBL_INSTANCE_TYPEOF(pSelf)));
        pClass->pFnMetaMarshal = pFnMeta;
        GBL_API_VERIFY_CALL(GblInstance_swizzleClass(GBL_INSTANCE(pSelf), GBL_CLASS(pClass)));
        GBL_API_VERIFY_CALL(GblInstance_sinkClass(GBL_INSTANCE(pSelf)));
    }
    GBL_API_END();

}

GBL_EXPORT GBL_RESULT GblClosure_invoke(GblClosure*     pSelf,
                                        GblVariant*     pRetValue,
                                        GblSize         argCount,
                                        GblVariant*     pArgValues)
{
    GBL_API_BEGIN(NULL);

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

    GBL_API_VERIFY_POINTER(pFnMarshal);

    GblClosure* prevActiveClosure = pCurrentClosure_;
    pCurrentClosure_ = pSelf;

    GBL_API_VERIFY_CALL(pFnMarshal(pSelf,
                                   pRetValue,
                                   argCount,
                                   pArgValues,
                                   pMarshalData));

    pCurrentClosure_ = prevActiveClosure;

    GBL_API_END();
}

GBL_EXPORT GblType GblClosure_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static GblTypeInfo info = {
        .classSize = sizeof(GblClosureClass),
        .instanceSize = sizeof(GblClosure)
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_API_BEGIN(NULL);
        type = GblType_registerStatic(GblQuark_internStringStatic("GblClosure"),
                                      GBL_BOX_TYPE,
                                      &info,
                                      GBL_TYPE_FLAG_TYPEINFO_STATIC);
        GBL_API_VERIFY_LAST_RECORD();
        GBL_API_END_BLOCK();
    }
    return type;

}

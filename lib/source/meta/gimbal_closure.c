#include <gimbal/meta/gimbal_closure.h>
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/strings/gimbal_quark.h>

static GBL_THREAD_LOCAL GblClosure* pCurrentClosure_ = NULL;

GBL_EXPORT GblClosure* GblClosure_current(void) {
    return pCurrentClosure_;
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
        type = GblType_registerStatic(GblQuark_internStringStatic("Closure"),
                                      GBL_RECORD_TYPE,
                                      &info,
                                      GBL_TYPE_FLAG_TYPEINFO_STATIC);
        GBL_API_VERIFY_LAST_RECORD();
        GBL_API_END_BLOCK();
    }
    return type;

}

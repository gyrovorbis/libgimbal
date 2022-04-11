#include <gimbal/meta/gimbal_itable.h>
#include <gimbal/core/gimbal_api_frame.h>


GBL_EXPORT GblType GblITable_type(void) GBL_NOEXCEPT {
    static GblType type = GBL_TYPE_INVALID;
    if(type == GBL_TYPE_INVALID) {
        type = gblTypeRegisterStatic(GBL_TYPE_INTERFACE,
                                     "ITable",
                                     &((const GblTypeInfo) {
                                         .classSize    = sizeof(GblITableIFace),
                                         .classAlign   = GBL_ALIGNOF(GblITableIFace),
                                     }),
                                     GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED | GBL_TYPE_FLAG_ABSTRACT);

    }
    return type;
}

GBL_API GblITable_index(const GblITable* pSelf, const GblVariant* pKey, GblVariant* pValue) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL); {
        GBL_API_VERIFY_POINTER(pKey);
        GBL_API_VERIFY_POINTER(pValue);
        GblITableIFace* pIFace = GBL_ITABLE_GET_IFACE(pSelf);
        GBL_API_VERIFY(pIFace->pFnIndex, GBL_RESULT_ERROR_INVALID_VIRTUAL_CALL);
        GBL_API_CALL(pIFace->pFnIndex(pSelf, pKey, pValue));
    } GBL_API_END();
}

GBL_API GblITable_newIndex(GblITable* pSelf, const GblVariant* pKey, const GblVariant* pValue) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL); {
        GBL_API_VERIFY_POINTER(pKey);
        GBL_API_VERIFY_POINTER(pValue);
        GblITableIFace* pIFace = GBL_ITABLE_GET_IFACE(pSelf);
        GBL_API_VERIFY(pIFace->pFnNewIndex, GBL_RESULT_ERROR_INVALID_VIRTUAL_CALL);
        GBL_API_CALL(pIFace->pFnNewIndex(pSelf, pKey, pValue));
    } GBL_API_END();
}

GBL_API GblITable_nextIndex(const GblITable* pSelf, const GblVariant* pKey, GblVariant* pNextKey, GblVariant* pNextValue) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL); {
        GBL_API_VERIFY_POINTER(pNextKey);
        GBL_API_VERIFY_POINTER(pNextValue);
        GblITableIFace* pIFace = GBL_ITABLE_GET_IFACE(pSelf);
        GBL_API_VERIFY(pIFace->pFnNextIndex, GBL_RESULT_ERROR_INVALID_VIRTUAL_CALL);
        GBL_API_CALL(pIFace->pFnNextIndex(pSelf, pKey, pNextKey, pNextValue));
    } GBL_API_END();
}

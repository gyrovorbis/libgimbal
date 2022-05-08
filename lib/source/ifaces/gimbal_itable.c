#include <gimbal/ifaces/gimbal_itable.h>
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/meta/gimbal_variant.h>
#include <gimbal/meta/gimbal_value_types.h>
#include <gimbal/types/gimbal_quark.h>

static GBL_RESULT GblITableIFace_index_(const GblITable* pTable, const GblVariant* pKey, GblVariant* pValue) {
    GBL_UNUSED(pTable); GBL_UNUSED(pKey);
    GBL_API_BEGIN(NULL);
    GBL_API_CALL(GblVariant_setValueCopy(pValue, GBL_NIL_TYPE));
    GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY);
    GBL_API_END();
}


static GBL_RESULT GblITableIFace_newIndex_(GblITable* pTable, const GblVariant* pKey, const GblVariant* pValue) {
    GBL_UNUSED(pTable); GBL_UNUSED(pKey); GBL_UNUSED(pValue);
    GBL_API_BEGIN(NULL);
    GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY);
    GBL_API_END();
}


static GBL_RESULT GblITableIFace_nextIndex_(const GblITable* pTable, const GblVariant* pKey, GblVariant* pNextKey, GblVariant* pNextValue) {
    GBL_UNUSED(pTable); GBL_UNUSED(pKey);
    GBL_API_BEGIN(NULL);
    GBL_API_CALL(GblVariant_setValueCopy(pNextKey, GBL_NIL_TYPE));
    GBL_API_CALL(GblVariant_setValueCopy(pNextValue, GBL_NIL_TYPE));
    GBL_API_END();
}

static GBL_RESULT GblITableIFace_init_(GblITableIFace* pIFace, void* pClassData, GblContext* pCtx) {
    GBL_UNUSED(pClassData);
    GBL_API_BEGIN(pCtx);
    pIFace->pFnIndex        = GblITableIFace_index_;
    pIFace->pFnNewIndex     = GblITableIFace_newIndex_;
    pIFace->pFnNextIndex    = GblITableIFace_nextIndex_;
    GBL_API_END();
}


extern GBL_RESULT GblITable_typeRegister_(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblType_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_ITABLE,
                            GBL_INTERFACE_TYPE,
                           GblQuark_internStringStatic("ITable"),
                           &((const GblTypeInfo) {
                               .pFnClassInit = (GblTypeClassInitializeFn)GblITableIFace_init_,
                               .classSize    = sizeof(GblITableIFace)
                           }),
                           GBL_TYPE_FLAG_ABSTRACT);

    GBL_API_END();
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

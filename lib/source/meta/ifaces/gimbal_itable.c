#include <gimbal/meta/ifaces/gimbal_itable.h>
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/meta/types/gimbal_variant.h>
#include <gimbal/meta/classes/gimbal_primitives.h>
#include <gimbal/strings/gimbal_quark.h>
#include "../types/gimbal_type_.h"

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


GBL_EXPORT GblType GblITable_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTypeInfo info = {
        .pFnClassInit = (GblTypeClassInitializeFn)GblITableIFace_init_,
        .classSize = sizeof(GblITableIFace)
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_API_BEGIN(NULL);
        type = GblType_registerStatic(GblQuark_internStringStatic("GblITable"),
                                      GBL_INTERFACE_TYPE,
                                      &info,
                                      GBL_TYPE_FLAG_TYPEINFO_STATIC);
        GBL_API_END_BLOCK();
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

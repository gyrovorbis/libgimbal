#include <gimbal/core/gimbal_api_generators.h>
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/types/gimbal_variant.h>
#include <metalang99.h>

GBL_API gblMetaTypeStatic(const GblMetaType** ppMetaType) {
   static GblMetaType metaType = {
        "MetaType",
        GBL_VARIANT_TYPE_NIL,
        GBL_META_TYPE_ID_INVALID,
        GBL_META_TYPE_FLAG_NONE,
        NULL,
        {
            0,
            0,
            {
                0,
                NULL//&(const GblMetaTypeVTable){NULL, NULL, NULL, NULL, NULL, NULL}
            }
        }

    };

    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(ppMetaType);

    GBL_API_END();
}

GBL_API gblMetaTypeConstruct(const GblMetaType* pMetaType, GblContext hCtx, void* pData, const GblVariant* pVariant) {
    GBL_API_BEGIN(hCtx);
    GBL_API_VERIFY_POINTER(pMetaType);
    GBL_API_VERIFY_POINTER(pData);
    if(GBL_API_METATYPE_DISPATCH_METHOD(pMetaType, Construct)) {
        GBL_API_METATYPE_DISPATCH(pMetaType, Construct, pData, hCtx, pVariant);
    } else {
        if(pVariant) {
            if(pVariant->type == pMetaType->typeId) {
                memcpy(pData, &pVariant->pVoidPtr, pMetaType->metaInstance.sizeOf);
            } else {
                GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_OPERATION, "Default gblMetaTypeConstruct cannot copy construct %s from typeId %d", pMetaType->pTypeName, pVariant->type);
            }
        } else {
            memset(pData, 0, pMetaType->metaInstance.sizeOf);
        }
    }
    GBL_API_END();
}

GBL_API gblMetaTypeCreate(const GblMetaType* pMetaType, GblContext hCtx, const GblVariant* pVariant, void** ppData) {
    GBL_API_BEGIN(hCtx);
    GBL_API_VERIFY_POINTER(pMetaType);
    GBL_API_VERIFY_POINTER(ppData);
    *ppData = GBL_API_MALLOC(pMetaType->metaInstance.sizeOf, pMetaType->metaInstance.alignOf, pMetaType->pTypeName);
    GBL_API_CALL(gblMetaTypeConstruct(pMetaType, hCtx, *ppData, pVariant));
    GBL_API_END();
}

GBL_API gblMetaTypeDestruct(const GblMetaType* pMetaType, GblContext hCtx, void* pData) {
    GBL_API_BEGIN(hCtx);
    GBL_API_VERIFY_POINTER(pMetaType);
    GBL_API_VERIFY_POINTER(pData);
    if(GBL_API_METATYPE_DISPATCH_METHOD(pMetaType, Destruct))
        GBL_API_METATYPE_DISPATCH(pMetaType, Destruct, pData, hCtx);
    GBL_API_END();
}

GBL_API gblMetaTypeDestroy(const GblMetaType* pMetaType, GblContext hCtx, void* pData) {
    GBL_API_BEGIN(hCtx);
    GBL_API_VERIFY_POINTER(pMetaType);
    GBL_API_VERIFY_POINTER(pData);
    GBL_API_CALL(gblMetaTypeDestruct(pMetaType, hCtx, pData));
    GBL_API_FREE(pData);
    GBL_API_END();
}
GBL_API gblMetaTypeConvert(const GblMetaType* pMetaType, GblContext hCtx, const void* pData, GblVariant* pTo) {
    GBL_API_BEGIN(hCtx);
    GBL_API_VERIFY_POINTER(pMetaType);
    GBL_API_VERIFY_POINTER(pData);
    GBL_API_VERIFY_POINTER(pTo);
    if(GBL_API_METATYPE_DISPATCH_METHOD(pMetaType, Convert)) {
            GBL_API_METATYPE_DISPATCH(pMetaType, Convert, pData, hCtx, pTo);
    } else {
        if(pTo->type == pMetaType->typeId) {
            memcpy(&pTo->pVoidPtr, pData, pMetaType->metaInstance.sizeOf);
        } else {
            GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_OPERATION, "Default gblMetaTypeConvert cannot convert from %s to typeId %d", pMetaType->pTypeName, pTo->type);
        }
    }
    GBL_API_END();
}

GBL_API gblMetaTypeCompare(const GblMetaType* pMetaType, GblContext hCtx, const void* pData, const GblVariant* pRhs, GblInt* pResult) {
    GBL_API_BEGIN(hCtx);
    GBL_API_VERIFY_POINTER(pMetaType);
    GBL_API_VERIFY_POINTER(pData);
    GBL_API_VERIFY_POINTER(pRhs);
    GBL_API_VERIFY_POINTER(pResult);
    if(GBL_API_METATYPE_DISPATCH_METHOD(pMetaType, Convert)) {
        GBL_API_METATYPE_DISPATCH(pMetaType, Compare, pData, hCtx, pRhs, pResult);
    } else {
        if(pRhs->type == pMetaType->typeId) {
            return memcmp(&pRhs->pVoidPtr, pData, pMetaType->metaInstance.sizeOf);
        } else {
            GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_OPERATION, "Default gblMetaTypeCompare cannot compare %s to typeId %d", pMetaType->pTypeName, pRhs->type);
        }
    }
    GBL_API_METATYPE_DISPATCH(pMetaType, Compare, pData, hCtx, pRhs, pResult);
    GBL_API_END();
}

GBL_API gblMetaTypeSerialize(const GblMetaType* pMetaType, GblContext hCtx, const void* pData, void* pBuffer, GblSize* pSize) {
    GBL_API_BEGIN(hCtx);
    GBL_API_VERIFY_POINTER(pMetaType);
    GBL_API_VERIFY_POINTER(pData);
    GBL_API_VERIFY_POINTER(pSize);
    if(GBL_API_METATYPE_DISPATCH_METHOD(pMetaType, Serialize)) {
        GBL_API_METATYPE_DISPATCH(pMetaType, Serialize, pData, hCtx, pBuffer, pSize);
    } else {
        if(!pBuffer) {
            *pSize = pMetaType->metaInstance.sizeOf;
         } else {
            GBL_API_VERIFY_ARG(*pSize >= pMetaType->metaInstance.sizeOf);
            memcpy(pBuffer, pData, pMetaType->metaInstance.sizeOf);
            *pSize = pMetaType->metaInstance.sizeOf;
        }
    }
    GBL_API_END();
}

GBL_API gblMetaTypeDeserialize(const GblMetaType* pMetaType, GblContext hCtx, void* pData, const void* pBuffer, GblSize* pSize) {
    GBL_API_BEGIN(hCtx);
    GBL_API_VERIFY_POINTER(pMetaType);
    GBL_API_VERIFY_POINTER(pData);
    GBL_API_VERIFY_POINTER(pBuffer);
    GBL_API_VERIFY_POINTER(pSize);
    GBL_API_VERIFY_ARG(*pSize > 0);
    if(GBL_API_METATYPE_DISPATCH_METHOD(pMetaType, Deserialize)) {
        GBL_API_METATYPE_DISPATCH(pMetaType, Deserialize, pData, hCtx, pBuffer, pSize);
    } else {
        GBL_API_VERIFY_ARG(*pSize >= pMetaType->metaInstance.sizeOf);
        memcpy(pData, pBuffer, pMetaType->metaInstance.sizeOf);
        *pSize = pMetaType->metaInstance.sizeOf;
    }
    GBL_API_END();
}

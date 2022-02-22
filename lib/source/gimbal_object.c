#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/objects/gimbal_object.h>
#include <gimbal/types/gimbal_variant.h>

GBL_API gblObjectMetaObjectStatic   (const GblMetaObject** ppMetaObject) {
    const GblMetaType* pParentType = NULL;

    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(ppMetaObject);
    GBL_API_CALL(gblMetaTypeStatic(&pParentType));

    static GblObjectVTable vtable = {
        .pFnPropertySet = NULL,
        .pFnPropertyGet = NULL,
        .pFnPropertyNext = NULL
    };

    memcpy(&vtable.base, pParentType->metaInstance.dispatcher.pVTable, sizeof(vtable.base));

    static GblMetaObject metaObject = {
        {
            "Object",
            GBL_VARIANT_TYPE_OBJECT,
            GBL_META_TYPE_ID_INVALID,
            GBL_META_TYPE_FLAG_NONE,
            NULL,
            {
                sizeof(GblObject_),
                GBL_ALIGNOF(GblObject_),
                {
                    sizeof(GblObjectVTable),
                    NULL
                }
            }
        },
        GBL_OBJECT_TYPE_OBJECT,
        0,
        NULL

    };
    metaObject.baseMetaType.pMetaObject = &metaObject;
    metaObject.baseMetaType.metaInstance.dispatcher.pVTable = &vtable;
    *ppMetaObject = &metaObject;
    GBL_API_END();
}


GBL_API gblObjectFromExtraSpace     (GblContext hCtx, const GblMetaObject* pMetaObject, const void* pExtra, GblObject* phObject) {
    GBL_API_BEGIN(hCtx);
    GBL_API_VERIFY_POINTER(pMetaObject);
    GBL_API_VERIFY_POINTER(pExtra);
    GBL_API_VERIFY_POINTER(phObject);
    *phObject = (GblObject)(pExtra - pMetaObject->baseMetaType.metaInstance.sizeOf);
    // could do some shit to verify sanity of object...
    GBL_API_END();

}

// Instance
GBL_API gblObjectMetaObject         (GblObject hObject, GblContext hCtx, const GblMetaObject** ppMetaObject) {
    GBL_API_BEGIN(hCtx);
    GBL_API_VERIFY_HANDLE(hObject);
    GBL_API_VERIFY_POINTER(ppMetaObject);
    *ppMetaObject = hObject->pMetaObject;
    GBL_API_END();

}
GBL_API gblObjectMetaInstance       (GblObject hObject, GblContext hCtx, const GblMetaInstance** ppMetaInstance) {
    GBL_API_BEGIN(hCtx);
    GBL_API_VERIFY_HANDLE(hObject);
    GBL_API_VERIFY_POINTER(ppMetaInstance);
    *ppMetaInstance = hObject->pMetaInstance;
    GBL_API_END();
}

GBL_API gblObjectExtraSpace(GblObject hObject, GblContext hCtx, void** ppExtraData, GblSize* pSize) {
    const GblMetaObject*    pMetaObject = NULL;
    const GblMetaInstance*  pMetaInstance = NULL;
    uint8_t*          pRawPtr     = (uint8_t*)hObject;
    GblSize extraSize = 0;
    void* pExtraPtr = NULL;
    GblSize sizeDelta = 0;
    GBL_API_BEGIN(hCtx);
    GBL_API_VERIFY_EXPRESSION(ppExtraData || pSize);
    GBL_API_CALL(gblObjectMetaObject(hObject, hCtx, &pMetaObject));
    GBL_API_VERIFY_POINTER(pMetaObject);
    GBL_API_CALL(gblObjectMetaInstance(hObject, hCtx, &pMetaInstance));
    GBL_API_VERIFY_POINTER(pMetaInstance);
    sizeDelta = pMetaInstance->sizeOf - pMetaObject->baseMetaType.metaInstance.sizeOf;
    GBL_API_VERIFY_EXPRESSION(sizeDelta >= 0);
    if(sizeDelta) {
        uint8_t* pStaticTypeEnd = (uint8_t*)&pRawPtr[pMetaObject->baseMetaType.metaInstance.sizeOf];
        extraSize = (uintptr_t)pMetaInstance - (uintptr_t)pStaticTypeEnd;
        GBL_API_VERIFY_EXPRESSION(extraSize < pMetaInstance->sizeOf);
        if(extraSize) pExtraPtr = pStaticTypeEnd;
    }
    if(ppExtraData) *ppExtraData = pExtraPtr;
    if(pSize) *pSize = extraSize;
    GBL_API_END();
}


GBL_API gblObjectDispatcher(GblObject hObject, GblContext hCtx, const GblDispatcher** ppDispatcher) {
    const GblMetaInstance* pInstance = NULL;
    GBL_API_BEGIN(hCtx);
    GBL_API_VERIFY_POINTER(ppDispatcher);
    GBL_API_CALL(gblObjectMetaInstance(hObject, hCtx, &pInstance));
    GBL_API_VERIFY_EXPRESSION(pInstance);
    *ppDispatcher = &pInstance->dispatcher;
    GBL_API_END();
}

GBL_API gblObjectCreate             (GblObject* phObject, GblContext hCtx, const GblObjectCreateInfo* pCreateInfo) {
    GBL_API_BEGIN(hCtx);
    GBL_API_VERIFY_POINTER(phObject);
    {
        const GblMetaObject* pStaticType = NULL;
        const GblVariant variant = { .type = GBL_VARIANT_TYPE_NIL };
        GblBool baseType = GBL_FALSE;
        GblObjectCreateInfo createInfo = {
            NULL,
            NULL,
            NULL,
            NULL
        };

        GBL_API_CALL(gblObjectMetaObjectStatic(&pStaticType));

        if(pCreateInfo) {
            memcpy(&createInfo, pCreateInfo, sizeof(GblObjectCreateInfo));
        } else {
            memset(&createInfo, 0, sizeof(GblObjectCreateInfo));
        }

        if(!createInfo.pStaticType) {
            createInfo.pStaticType = pStaticType;
        }

        GBL_API_CALL(gblMetaObjectBaseType(createInfo.pStaticType, pStaticType, &baseType));
        GBL_API_VERIFY(baseType, GBL_RESULT_ERROR_TYPE_MISMATCH, "Attempting to construct a %s from a %s: UNRELATED TYPES!", createInfo.pStaticType->baseMetaType.pTypeName, pStaticType->baseMetaType.pTypeName);

        if(!createInfo.pVariant) {
            createInfo.pVariant = &variant;
        }


        GblSize staticSize  = createInfo.pStaticType->baseMetaType.metaInstance.sizeOf;
        GblSize align       = createInfo.pStaticType->baseMetaType.metaInstance.alignOf;
        GblSize extraSize   = 0;
        GblSize vtableSize  = 0;
        GblSize dynamicSize = 0;
        GblSize totalSize   = 0;

        if(createInfo.pExtraSpaceInfo) {
            extraSize = createInfo.pExtraSpaceInfo->extraBytes;
            if(createInfo.pExtraSpaceInfo->align > align) align = createInfo.pExtraSpaceInfo->align;
        }

        if(createInfo.pOverrideDispatcher) {
            vtableSize = createInfo.pOverrideDispatcher->vTableSizeOf;
        }

        dynamicSize = extraSize + vtableSize;
        if(dynamicSize > 0) {
            dynamicSize += sizeof(GblMetaInstance);
        }

        totalSize = staticSize + dynamicSize;
        *phObject = GBL_API_MALLOC(totalSize, align, createInfo.pStaticType->baseMetaType.pTypeName);

        // check if we're initializing extra shit
        if(dynamicSize) {
            uint8_t* pRaw = (uint8_t*)*phObject;
            // zero initialize
            memset(&pRaw[staticSize], 0, dynamicSize);

            // initialize new instance info
            GblMetaInstance instance = createInfo.pStaticType->baseMetaType.metaInstance;
            instance.sizeOf     = totalSize;
            instance.alignOf    = align;
            GblMetaInstance* pDestInstance = (GblMetaInstance*)&pRaw[staticSize + extraSize];
            memcpy(pDestInstance, &instance, sizeof(GblMetaInstance));
            (*phObject)->pMetaInstance = pDestInstance;

            // initialize dispatcher + new vtable
            if(createInfo.pOverrideDispatcher) {
                if(createInfo.pOverrideDispatcher->pVTable != instance.dispatcher.pVTable) {
                    GBL_API_VERIFY_EXPRESSION(createInfo.pOverrideDispatcher->vTableSizeOf >= instance.dispatcher.vTableSizeOf);
                    void* pVTableDest = pDestInstance + sizeof(GblMetaInstance);
                    memcpy(pVTableDest, createInfo.pOverrideDispatcher->pVTable, instance.dispatcher.vTableSizeOf);
                    pDestInstance->dispatcher.pVTable = pVTableDest;
                    pDestInstance->dispatcher.vTableSizeOf = instance.dispatcher.vTableSizeOf;
                }
            }
        }

        GBL_API_CALL(gblMetaTypeConstruct(&createInfo.pStaticType->baseMetaType,
                          hCtx,
                          phObject,
                          createInfo.pVariant
                    ));
    }
    GBL_API_END();
}


GBL_API gblObjectDestroy(GblObject hObject, GblContext hCtx) {
    GBL_API_BEGIN(hCtx);
    GBL_API_CALL(gblMetaTypeDestroy(&hObject->pMetaObject->baseMetaType, hCtx, hObject));
    GBL_API_END();
}

GBL_API gblObjectConvert(GblObject hObject, GblContext hCtx, GblVariant* pTo) {
    GBL_API_BEGIN(hCtx);
    GBL_API_CALL(gblMetaTypeConvert(&hObject->pMetaObject->baseMetaType, hCtx, hObject, pTo));
    GBL_API_END();

}
GBL_API gblObjectCompare(GblObject hObject, GblContext hCtx, const GblVariant* pRhs, GblInt* pResult) {
    GBL_API_BEGIN(hCtx);
    GBL_API_CALL(gblMetaTypeCompare(&hObject->pMetaObject->baseMetaType, hCtx, hObject, pRhs, pResult));
    GBL_API_END();
}
GBL_API gblObjectSerialize(GblObject hObject, GblContext hCtx, void* pBuffer, GblSize* pSize) {
    GBL_API_BEGIN(hCtx);
    GBL_API_CALL(gblMetaTypeSerialize(&hObject->pMetaObject->baseMetaType, hCtx, hObject, pBuffer, pSize));
    GBL_API_END();
}
GBL_API gblObjectDeserialize(GblObject hObject, GblContext hCtx, const void* pBuffer, GblSize* pSize) {
    GBL_API_BEGIN(hCtx);
    GBL_API_CALL(gblMetaTypeDeserialize(&hObject->pMetaObject->baseMetaType, hCtx, hObject, pBuffer, pSize));
    GBL_API_END();
}

// New Virtuals
GBL_API gblObjectPropertyGet(GblObject hObject, GblContext hCtx, const GblVariant* pKey, GblVariant* pValue) {
    const GblObjectVTable* pVTable = NULL;
    GBL_API_BEGIN(hCtx);
    GBL_API_VERIFY_HANDLE(hObject);
    GBL_API_VERIFY_POINTER(pKey);
    GBL_API_VERIFY_POINTER(pValue);
    GBL_API_VERIFY_EXPRESSION(hObject->pMetaInstance->dispatcher.vTableSizeOf >= sizeof(GblObjectVTable));
    pVTable = hObject->pMetaInstance->dispatcher.pVTable;
    GBL_API_VERIFY_POINTER(pVTable);
    //GBL_API_VERIFY(pVTable->pFnPropertyGet, GBL_RESULT_ERROR_INVALID_VIRTUAL_CALL, "Type[%s] has no propertyGet VTable entry!");
    if(pVTable->pFnPropertyGet) {
        GBL_API_CALL(pVTable->pFnPropertyGet(hObject, hCtx, pKey, pValue));
    } else {
        GBL_API_CALL(gblVariantSetNil(pValue));
    }
    GBL_API_END();
}

GBL_API gblObjectPropertySet(GblObject hObject, GblContext hCtx, const GblVariant* pKey, const GblVariant* pValue) {
    const GblVariant nilVariant = { .type = GBL_VARIANT_TYPE_NIL };
    const GblObjectVTable* pVTable = NULL;
    GBL_API_BEGIN(hCtx);
    GBL_API_VERIFY_HANDLE(hObject);
    GBL_API_VERIFY_POINTER(pKey);
    GBL_API_VERIFY_EXPRESSION(hObject->pMetaInstance->dispatcher.vTableSizeOf >= sizeof(GblObjectVTable));
    pVTable = hObject->pMetaInstance->dispatcher.pVTable;
    GBL_API_VERIFY_POINTER(pVTable);
    if(pVTable->pFnPropertySet) {
        if(!pValue) pValue = &nilVariant;
        GBL_API_CALL(pVTable->pFnPropertySet(hObject, hCtx, pKey, pValue));
    } else {
        GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_VIRTUAL_CALL, "Type[%s] has no propertySet VTable entry!");
    }
    GBL_API_END();
}

GBL_API gblObjectPropertyNext(GblObject hObject, GblContext hCtx, const GblVariant* pCurKey, GblVariant* pNextKey, GblVariant* pNextValue) {
    const GblObjectVTable* pVTable = NULL;
    const GblVariant nilVariant = { .type = GBL_VARIANT_TYPE_NIL };
    GBL_API_BEGIN(hCtx);
    GBL_API_VERIFY_HANDLE(hObject);
    GBL_API_VERIFY_EXPRESSION(pNextKey || pNextValue);
    //GBL_API_VERIFY_POINTER(pNextKey);
    //GBL_API_VERIFY_POINTER(pNextValue);
    GBL_API_VERIFY_EXPRESSION(hObject->pMetaInstance->dispatcher.vTableSizeOf >= sizeof(GblObjectVTable));
    pVTable = hObject->pMetaInstance->dispatcher.pVTable;
    GBL_API_VERIFY_POINTER(pVTable);
    //GBL_API_VERIFY(pVTable->pFnPropertyNext, GBL_RESULT_ERROR_INVALID_VIRTUAL_CALL, "Type[%s] has no propertyNext VTable entry!");
    if(pVTable->pFnPropertyNext) {
        if(!pCurKey) pCurKey = &nilVariant;
        GBL_API_CALL(pVTable->pFnPropertyNext(hObject, hCtx, pCurKey, pNextKey, pNextValue));
    } else {
        if(pNextKey) GBL_API_CALL(gblVariantSetNil(pNextKey));
        if(pNextValue) GBL_API_CALL(gblVariantSetNil(pNextValue));
    }
    GBL_API_END();
}


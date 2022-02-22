#include <gimbal/objects/gimbal_context_object.h>
#include <gimbal/core/gimbal_api_frame.h>

GBL_API gblContextObjectMetaObjectStatic   (const GblMetaObject** ppMetaObject) {
    const GblMetaObject* pParent = NULL;

    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(ppMetaObject);
    GBL_API_CALL(gblObjectMetaObjectStatic(&pParent));

    static GblContextObjectVTable vtable = {
        {
            .pFnConstruct = NULL
        },
        .pFnPropertySet = NULL,
        .pFnPropertyGet = NULL,
        .pFnPropertyNext = NULL
    };

    memcpy(&vtable, pParent->baseMetaType.metaInstance.dispatcher.pVTable, pParent->baseMetaType.metaInstance.dispatcher.vTableSizeOf);

    static GblMetaObject metaObject = {
        {
            "ContextObject",

            GBL_VARIANT_TYPE_OBJECT,
            GBL_META_TYPE_ID_INVALID,
            GBL_META_TYPE_FLAG_NONE,
            NULL,
            {
                sizeof(GblContextObject_),
                GBL_ALIGNOF(GblContextObject_),
                {
                    sizeof(GblContextObjectVTable),
                    NULL
                }
            }
        },
        GBL_OBJECT_TYPE_CONTEXT_OBJECT,
        0,
        NULL
    };
    metaObject.baseMetaType.pMetaObject = &metaObject;
    metaObject.baseMetaType.metaInstance.dispatcher.pVTable = &vtable;
    metaObject.pParent = pParent;
    *ppMetaObject = &metaObject;
    GBL_API_END();
}

GBL_API gblContextObjectMetaInstance       (GblContextObject hObject, const GblMetaInstance** ppMetaInstance) {
    return gblObjectMetaInstance((GblObject)hObject, hObject->hCtx, ppMetaInstance);
}
GBL_API gblContextObjectExtraSpace         (GblContextObject hObject, void** ppExtraData, GblSize* pSize) {
    return gblObjectExtraSpace((GblObject)hObject, hObject->hCtx, ppExtraData, pSize);
}
GBL_API gblContextObjectDispatcher         (GblContextObject hObject, const GblDispatcher** ppDispatcher) {
    return gblObjectDispatcher((GblObject)hObject, hObject->hCtx, ppDispatcher);
}
GBL_API gblContextObjectContext            (GblContextObject hObject, GblContext* phCtx) {
    if(hObject == GBL_CONTEXT_OBJECT_INVALID) return GBL_RESULT_ERROR_INVALID_HANDLE;
    if(!phCtx) return GBL_RESULT_ERROR_INVALID_POINTER;
    *phCtx = hObject->hCtx;
    return GBL_RESULT_SUCCESS;
}

GBL_API gblContextObjectCreate             (GblContextObject* phObject, GblContext hCtx, const GblContextObjectCreateInfo* pCreateInfo) {
    GblBool baseType = GBL_FALSE;
    const GblMetaObject* pStaticType = NULL;
    GBL_API_BEGIN(hCtx);
    GblContextObjectCreateInfo createInfo;
    GBL_API_CALL(gblContextObjectMetaObjectStatic(&createInfo.objectInfo.pStaticType));
    if(pCreateInfo) {
        memcpy(&createInfo, pCreateInfo, sizeof(GblContextObjectCreateInfo));
    } else memset(&createInfo, 0, sizeof(GblContextObjectCreateInfo));
    if(!createInfo.objectInfo.pStaticType) {
        createInfo.objectInfo.pStaticType = pStaticType;
    }
    GBL_API_VERIFY_POINTER(phObject);

    GBL_API_CALL(gblMetaObjectBaseType(createInfo.objectInfo.pStaticType, pStaticType, &baseType));
    GBL_API_VERIFY(baseType, GBL_RESULT_ERROR_TYPE_MISMATCH, "Attempting to construct a %s from a %s: UNRELATED TYPES!", createInfo.objectInfo.pStaticType->baseMetaType.pTypeName, pStaticType->baseMetaType.pTypeName);

    GBL_API_CALL(gblObjectCreate((GblObject*)phObject, hCtx, pCreateInfo));
    (*phObject)->hCtx = hCtx;
    GBL_API_END();
}

GBL_API gblContextObjectDestroy            (GblContextObject hObject) {
    return gblObjectDestroy((GblObject)hObject, hObject->hCtx);
}
GBL_API gblContextObjectConvert            (GblContextObject hObject, GblVariant* pTo) {
    return gblObjectConvert((GblObject)hObject, hObject->hCtx, pTo);
}
GBL_API gblContextObjectCompare            (GblContextObject hObject, const GblVariant* pRhs, GblInt* pResult) {
    return gblObjectCompare((GblObject)hObject, hObject->hCtx, pRhs, pResult);
}
GBL_API gblContextObjectSerialize          (GblContextObject hObject, void* pBuffer, GblSize* pSize) {
    return gblObjectSerialize((GblObject)hObject, hObject->hCtx, pBuffer, pSize);
}
GBL_API gblContextObjectDeserialize        (GblContextObject hObject, const void* pBuffer, GblSize* pSize) {
    return gblObjectDeserialize((GblObject)hObject, hObject->hCtx, pBuffer, pSize);
}
GBL_API gblContextObjectPropertyGet        (GblContextObject hObject, const GblVariant* pKey, GblVariant* pValue) {
    return gblObjectPropertyGet((GblObject)hObject, hObject->hCtx, pKey, pValue);
}
GBL_API gblContextObjectPropertySet        (GblContextObject hObject, const GblVariant* pKey, const GblVariant* pValue) {
    return gblObjectPropertySet((GblObject)hObject, hObject->hCtx, pKey, pValue);
}
GBL_API gblContextObjectPropertyNext       (GblContextObject hObject, const GblVariant* pCurKey, GblVariant* pNextKey, GblVariant* pNextValue) {
    return gblObjectPropertyNext((GblObject)hObject, hObject->hCtx, pCurKey, pNextKey, pNextValue);
}

#include <gimbal/meta/gimbal_object.h>
#include <gimbal/types/gimbal_variant.h>
#include <gimbal/meta/gimbal_property.h>

#define GBL_OBJECT_PROPERTY_TABLE_CAPACITY_DEFAULT_ 64
#define GBL_OBJECT_EVENT_FILTER_VECTOR_SIZE_        (sizeof(GblVector) + sizeof(GblIEventFilter*)*8)

static GblQuark     objectNameQuark_            = GBL_QUARK_INVALID;
static GblQuark     objectUserdataQuark_        = GBL_QUARK_INVALID;
static GblQuark     objectRefCountQuark_        = GBL_QUARK_INVALID;
static GblQuark     objectParentQuark_          = GBL_QUARK_INVALID;
//static GblQuark   objectEventFiltersQuark_


static GBL_RESULT GblObjectClass_ivariantIFace_construct_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_UNUSED(argc);
    GBL_API_BEGIN(NULL);
    if(op == GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT) {
        pVariant->pVoid = gblTypeInstanceCreate(pVariant->type);
    } else if(op == GBL_IVARIANT_OP_FLAG_CONSTRUCT_COPY || op == GBL_IVARIANT_OP_FLAG_CONSTRUCT_VALUE_COPY) {
        pVariant->pVoid = GblObject_ref(GBL_OBJECT(pArgs[0].pVoid));
    } else {
        pVariant->pVoid = pArgs[0].pVoid;
     }
    GBL_API_END();
}

static GBL_RESULT GblObjectClass_ivariantIFace_destruct_(GblVariant* pVariant) {
    GBL_API_BEGIN(NULL);
    GblObject_unref(pVariant->pVoid);
    GBL_API_END();
}

static GBL_RESULT GblObjectClass_ivariantIFace_compare_(const GblVariant* pVariant, const GblVariant* pOther, GblInt* pResult) {
    GBL_API_BEGIN(NULL);
    *pResult = (pVariant->pVoid == pOther->pVoid);
    GBL_API_END();
}

static GBL_RESULT GblObjectClass_ivariantIFace_get_(GblVariant* pSelf, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_UNUSED(argc);
    GBL_API_BEGIN(NULL);
    *(GblObject**)pArgs[0].pVoid = pSelf->pVoid;
    if(op == GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY) {
        GblObject_ref(GBL_OBJECT(pSelf->pVoid));
    } else if(op == GBL_IVARIANT_OP_FLAG_GET_VALUE_TAKE) {
        pSelf->type = GBL_TYPE_NIL;
    }
    GBL_API_END();
}


static GBL_RESULT GblObjectClass_ivariantIFace_set_(GblVariant* pSelf, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_UNUSED(argc);
    pSelf->pVoid = pArgs[0].pVoid;
    if(op == GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY || op == GBL_IVARIANT_OP_FLAG_SET_COPY) {
        GblObject_ref(GBL_OBJECT(pSelf->pVoid));
    } else if(op == GBL_IVARIANT_OP_FLAG_SET_MOVE || op == GBL_IVARIANT_OP_FLAG_SET_VALUE_MOVE) {
        pArgs[0].type = GBL_TYPE_NIL;
    }
}

static GBL_RESULT GblObjectClass_ivariantIFace_load_(GblVariant* pVariant, const GblString* pString) {
    return GBL_RESULT_UNIMPLEMENTED;
}

static GBL_RESULT GblObjectClass_ivariantIFace_save_(const GblVariant* pVariant, GblString* pString) {
    GblVariant variant = GBL_VARIANT_INIT;
    GblBool first = GBL_TRUE;
    GBL_API_BEGIN(NULL);
    gblStringConcat(pString, "{\n");

    for(const GblProperty* pIt = GblObject_propertyNext(pVariant->pVoid, NULL, GBL_PROPERTY_FLAG_SAVE);
        pIt != NULL;
        pIt = GblObject_propertyNext(pVariant->pVoid, pIt, GBL_PROPERTY_FLAG_SAVE))
    {
        if(!first) gblStringConcat(pString, ",\n");
        const char* pKey = GblProperty_nameString(pIt);
        gblStringConcat(pString, "\t");
        gblStringConcat(pString, pKey);
        gblStringConcat(pString, " = ");
        GblObject_propertyGetString(pVariant->pVoid, pKey, &variant);
        GblVariant_save(&variant, pString);
        first = GBL_FALSE;

    }
    gblStringConcat(pString, "}\n");
    GBL_API_END();
}

// ========== ITABLE IFACE ==========

static GBL_RESULT GblObjectClass_iTableIFace_index_(const GblITable* pITable, const GblVariant* pKey, GblVariant* pValue) {
    GBL_API_BEGIN(NULL);
    GBL_API_RESULT() = GBL_RESULT_ERROR_INVALID_PROPERTY;
    GblObject* pObject = GBL_OBJECT(pITable);
    if(GblVariant_type(pKey) == GBL_TYPE_STRING) {
        const char* pString = NULL;
        GBL_API_CALL(GblVariant_getValuePeek(pKey, &pString));
        GBL_API_RESULT() = GblObject_propertyGetString(pObject, pString, pValue);
        if(!GBL_RESULT_SUCCESS(GBL_API_RESULT()) && pObject->parentITableIndexFallthrough) {
            const GblObject* pParent = GblObject_parentGet(pObject);
            if(pParent) {
                GBL_API_CALL(GblITable_index(GBL_ITABLE(pParent), pKey, pValue));
                GBL_API_RESULT() = GBL_RESULT_SUCCESS;
            }
        }
    }
    GBL_API_END();
}

static GBL_RESULT GblObjectClass_iTableIFace_newIndex_(GblITable* pITable, const GblVariant* pKey, const GblVariant* pValue) {
    GBL_API_BEGIN(NULL);
    GBL_API_RESULT() = GBL_RESULT_ERROR_INVALID_PROPERTY;
    GblObject* pObject = GBL_OBJECT(pITable);
    if(GblVariant_type(pKey) == GBL_TYPE_STRING) {
        const char* pString = NULL;
        GBL_API_CALL(GblVariant_getValuePeek(pKey, &pString));
        GBL_API_RESULT() = GblObject_propertySetString(pObject, pString, pValue);
        if(!GBL_RESULT_SUCCESS(GBL_API_RESULT()) && pObject->parentITableIndexFallthrough) {
            const GblObject* pParent = GblObject_parentGet(pObject);
            if(pParent) {
                GBL_API_CALL(GblITable_newIndex(GBL_ITABLE(pParent), pKey, pValue));
                GBL_API_RESULT() = GBL_RESULT_SUCCESS;
            }
        }
    }
    GBL_API_END();
}

static GBL_RESULT GblObjectClass_iTableIFace_nextIndex_(const GblITable* pITable, const GblVariant* pKey, GblVariant* pNextKey, GblVariant* pNextValue) {
    GBL_API_BEGIN(NULL);
    GBL_API_RESULT() = GBL_RESULT_ERROR_INVALID_PROPERTY;
    GBL_API_CALL(GblVariant_setValueCopy(pNextKey, GBL_TYPE_NIL, NULL));
    GBL_API_CALL(GblVariant_setValueCopy(pNextValue, GBL_TYPE_NIL, NULL));
    GblObject* pObject = GBL_OBJECT(pITable);
    // get the first one
    if(!pKey || GblVariant_type(pKey) == GBL_TYPE_NIL) {
        const GblProperty* pProp = GblObject_propertyNext(pObject, NULL, GBL_PROPERTY_FLAG_READ);
        if(pProp) {
            GBL_API_CALL(GblVariant_setValueCopy(pNextKey, GBL_TYPE_STRING, GblProperty_nameString(pProp)));
            GBL_API_CALL(GblObject_propertyGet(pObject, pProp, pNextValue));
            GBL_API_RESULT() = GBL_RESULT_SUCCESS;
        }
    } else if(GblVariant_type(pKey) == GBL_TYPE_STRING) {
        const char* pString = NULL;
        GBL_API_CALL(GblVariant_getValuePeek(pKey, &pString));
        const GblProperty* pProp = GblObject_propertyFindString(pObject, pString);
        if(pProp) {
            pProp = GblObject_propertyNext(pObject, pProp, GBL_PROPERTY_FLAG_READ);
            if(pProp) {
                GBL_API_CALL(GblVariant_setValueCopy(pNextKey, GBL_TYPE_STRING, GblProperty_nameString(pProp)));
                GBL_API_CALL(GblObject_propertyGet(pObject, pProp, pNextValue));
            }
            GBL_API_RESULT() = GBL_RESULT_SUCCESS;
        }
    }

    GBL_API_END();
}

// ========== IEVENTHANDLER IFACE ==========

static GBL_RESULT GblObjectClass_iEventHandlerIFace_event_(GblIEventHandler* pHandler, GblEvent* pEvent) {
    GBL_API_BEGIN(NULL);
    GblObject* pObject = GBL_OBJECT(pHandler);
    GBL_API_RESULT() = GblObject_eventNotify(pObject, pEvent, pObject);
    GBL_API_END();
}


GBL_API GblObject_eventNotify(GblObject* pSelf, GblEvent* pEvent, GblObject* pSource) GBL_NOEXCEPT {
    GBL_API_BEGIN(gblTypeContext(GBL_INSTANCE_TYPE(pSelf)));
    GblObject* pObject = GBL_OBJECT(pSelf);

    // If we aren't the object that sent the event
     if(pSource != pSelf) {
         // Runthe event through the event filters
         const GblSize eventFilterCount = GblObject_eventFilterCount(pObject);
         for(GblSize f = 0; f < eventFilterCount; ++f)
         {
             GblIEventFilter* pFilter = GblObject_eventFilterAt(pSelf, f);
             GBL_API_RESULT() = GblIEventFilter_eventFilter(pFilter, GBL_IEVENT_HANDLER(pObject), pEvent);
             GBL_API_CALL(GBL_API_RESULT());
             if(GBL_API_RESULT() == GBL_RESULT_EVENT_ACCEPTED) {
                 GBL_API_DONE();
             }
         }

         // Send it to the event handler if the event handler didn't just send it here
        GBL_API_RESULT() = GblIEventHandler_event(GBL_IEVENT_HANDLER(pSelf), pEvent);
        GBL_API_CALL(GBL_API_RESULT());
        if(GBL_API_RESULT() == GBL_RESULT_EVENT_ACCEPTED) {
            GBL_API_DONE();
        }
    }

    // If enabled, forward event to all children
    if(pObject->childEventsSend) {
        for(GblObject* pChild = GblObject_childFirst(pObject);
            pChild != NULL;
            pChild = GblObject_siblingNext(pChild))
        {
            if(pChild != pSource) {
                GBL_API_RESULT() = GblObject_eventNotify(pChild, pEvent, pObject);
                GBL_API_CALL(GBL_API_RESULT());
                if(GBL_API_RESULT() == GBL_RESULT_EVENT_ACCEPTED) {
                    GBL_API_DONE();
                }
            }
        }
    }

    // If enabled, forward event to parent
    GblObject* pParent = GblObject_parentGet(pObject);
    if(pParent && pParent->childEventsReceive) {
        if(pParent != pSource) {
            GBL_API_RESULT() = GblObject_eventNotify(pParent, pEvent, pObject);
            GBL_API_CALL(GBL_API_RESULT());
            if(GBL_API_RESULT() == GBL_RESULT_EVENT_ACCEPTED) {
                GBL_API_DONE();
            }
        }
    }

    GBL_API_END();
}

static GBL_RESULT GblObject_ensureEventFilters_(GblObject* pSelf) GBL_NOEXCEPT {
    GBL_API_BEGIN(gblTypeContext(GBL_INSTANCE_TYPE(pSelf)));
    GBL_API_CALL(GblObject_ensureExtraData_(pSelf));
    if(!pSelf->pExtraData->pEventFilters) {
        pSelf->pExtraData->pEventFilters = GBL_API_MALLOC(GBL_OBJECT_EVENT_FILTER_VECTOR_SIZE_);
        GBL_API_CALL(gblVectorConstruct(pSelf->pExtraData->pEventFilters,
                                        gblTypeContext(GBL_INSTANCE_TYPE(pSelf)),
                                        sizeof(GblIEventFilter*),
                                        GBL_OBJECT_EVENT_FILTER_VECTOR_SIZE_));
    }
    GBL_API_END();
}

GBL_API GblObject_eventFilterInstall(GblObject* pSelf, GblIEventFilter* pFilter) GBL_NOEXCEPT {
    GBL_API_BEGIN(gblTypeContext(GBL_INSTANCE_TYPE(pSelf)));
    GBL_API_VERIFY_POINTER(pFilter);
    GBL_API_CALL(GblObject_ensureEventFilters_(pSelf));
    GBL_API_CALL(gblVectorPushBack(pSelf->pExtraData->pEventFilters, &pFilter));
    GBL_API_END();
}
GBL_API GblObject_eventFilterUninstall(GblObject* pSelf, GblIEventFilter* pFilter) GBL_NOEXCEPT {
    GblVector* pVector = NULL;
    GBL_API_BEGIN(gblTypeContext(GBL_INSTANCE_TYPE(pSelf)));
    GBL_API_VERIFY_POINTER(pFilter);
    GBL_API_VERIFY_POINTER(pSelf->pExtraData && pSelf->pExtraData->pEventFilters);
    pVector = pSelf->pExtraData->pEventFilters;
    GblSize count = 0;
    GBL_API_CALL(gblVectorSize(pVector, &count));
    GBL_API_VERIFY(count, GBL_RESULT_ERROR_INVALID_HANDLE,
                   "Object has no event filter list to remove filter from: %x", pFilter);
    GblBool found = GBL_FALSE;
    for(GblSize f = 0; f < count; ++f) {
        GblIEventFilter** ppFilter;
        GBL_API_CALL(gblVectorAt(pVector, f, (void**)&ppFilter));
        if(*ppFilter == pFilter) {
            found = GBL_TRUE;
            GBL_API_CALL(gblVectorErase(pVector, f, 1));
            break;
        }
    }
    GBL_API_VERIFY(found, GBL_RESULT_ERROR_INVALID_HANDLE,
                   "EventFilter wasn't found on Object: %x", pFilter);
    GBL_API_END();
}
GblIEventFilter* GblObject_eventFilterAt(const GblObject* pSelf, GblSize index) GBL_NOEXCEPT {
    GblIEventFilter** ppFilter = NULL;
    GBL_API_BEGIN(gblTypeContext(GBL_INSTANCE_TYPE(pSelf)));
    if(pSelf->pExtraData && pSelf->pExtraData->pEventFilters)
        GBL_API_CALL(gblVectorAt(pSelf->pExtraData->pEventFilters, index, (void**)&ppFilter));
    GBL_API_END_BLOCK();
    return ppFilter? *ppFilter : NULL;
}
GblSize GblObject_eventFilterCount(const GblObject* pSelf) GBL_NOEXCEPT {
    GblSize count = 0;
    GBL_API_BEGIN(gblTypeContext(GBL_INSTANCE_TYPE(pSelf)));
    if(pSelf->pExtraData && pSelf->pExtraData->pEventFilters) {
        GBL_API_CALL(gblVectorSize(pSelf->pExtraData->pEventFilters, &count));
    }
    GBL_API_END_BLOCK();
    return count;
}

// ======== OBJECT CLASS ==========

typedef enum OBJECT_PROPERTY_IDS_ {
    OBJECT_PROPERTY_ID_REFCOUNT_,
    OBJECT_PROPERTY_ID_NAME_,
    OBJECT_PROPERTY_ID_PARENT_,
    OBJECT_PROPERTY_ID_USERDATA_,
    OBJECT_PROPERTY_ID_COUNT_
} OBJECT_PROPERTY_IDS_;

static GBL_RESULT GblObjectClass_constructor_(GblObject* pSelf) {
    GBL_API_BEGIN(gblTypeContext(GBL_INSTANCE_TYPE(pSelf)));
    pSelf->refCounter = 1;
    GBL_API_END();
}

static GBL_RESULT GblObjectClass_destructor_(GblObject* pSelf) {
    GBL_API_BEGIN(gblTypeContext(GBL_INSTANCE_TYPE(pSelf)));
    GblObject_parentSet(pSelf, NULL);
    for(GblObject* pIt = GblObject_childFirst(pSelf);
        pIt != NULL;
        pIt = GblObject_siblingNext(pSelf))
    {
       GblObject_parentSet(pIt, NULL);
    }
    if(pSelf->pExtraData) {
        if(pSelf->pExtraData->pEventFilters) {
            GBL_API_CALL(gblVectorDestruct(pSelf->pExtraData->pEventFilters));
            GBL_API_FREE(pSelf->pExtraData->pEventFilters);
        }
        GBL_API_FREE(pSelf->pExtraData);
    }
    GBL_API_END();
}


static GBL_RESULT GblObjectClass_propertyGet_(const GblObject* pSelf, GblUint slot, GblVariant* pValue, const GblProperty* pProp)  {
    GBL_API_BEGIN(gblTypeContext(GBL_INSTANCE_TYPE(pSelf)));
    switch(slot) {
    case OBJECT_PROPERTY_ID_NAME_:      GblVariant_setValueCopy(pValue, GblProperty_valueType(pProp), GblObject_nameGet(pSelf));   break;
    case OBJECT_PROPERTY_ID_PARENT_:    GblVariant_setValueMove(pValue, GblProperty_valueType(pProp), GblObject_parentGet(pSelf)); break;
    case OBJECT_PROPERTY_ID_REFCOUNT_:  GblVariant_setValueCopy(pValue, GblProperty_valueType(pProp), GblObject_refCount(pSelf));  break;
    case OBJECT_PROPERTY_ID_USERDATA_:  GblVariant_setValueCopy(pValue, GblProperty_valueType(pProp), GblObject_userdataGet(pSelf)); break;
    default: GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY, "Reading unhandled property: %s", GblProperty_nameString(pProp));
    }
    GBL_API_END();
}

static GBL_RESULT GblObjectClass_propertySet_(GblObject* pSelf, GblUint slot, const GblVariant* pValue, const GblProperty* pProp) {
    GBL_API_BEGIN(gblTypeContext(GBL_INSTANCE_TYPE(pSelf)));
    GBL_UNUSED(pProp);
    switch(slot) {
    case OBJECT_PROPERTY_ID_NAME_: {
        const char* pName = NULL;
        GBL_API_CALL(GblVariant_getValueCopy(pValue, &pName));
        GblObject_nameSet(pSelf, pName);
        break;
    }
    case OBJECT_PROPERTY_ID_PARENT_: {
        GblObject* pParent = NULL;
        GBL_API_CALL(GblVariant_getValuePeek(pValue, &pParent));
        GblObject_parentSet(pSelf, pParent);
        break;
    }
    case OBJECT_PROPERTY_ID_USERDATA_: {
        const char* pUserdata = NULL;
        GBL_API_CALL(GblVariant_getValueCopy(pValue, &pUserdata));
        GblObject_userdataSet(pSelf,(void*)pUserdata);
        break;
    }
    default: GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY,
                                "Writing unhandled property: %s",
                                GblProperty_nameString(pProp));
    }
    GBL_API_END();
}


static GBL_RESULT GblObjectClass_init_(GblObjectClass* pClass, void* pData, GblContext hCtx) GBL_NOEXCEPT {
    GBL_UNUSED(pData);
    GBL_API_BEGIN(hCtx);

    // static constructor for first instance of class
    if(!gblTypeClassRefCount(GBL_TYPE_OBJECT)) {
        GBL_API_CALL(gblPropertyTableInitialize(hCtx, GBL_OBJECT_PROPERTY_TABLE_CAPACITY_DEFAULT_));
        objectNameQuark_            = gblQuarkFromStringStatic("name");
        objectUserdataQuark_        = gblQuarkFromStringStatic("userdata");
        objectRefCountQuark_        = gblQuarkFromStringStatic("refCount");
        objectParentQuark_          = gblQuarkFromStringStatic("parent");

        gblPropertyTableInsert(GBL_CLASS_TYPE(pClass),
                               objectRefCountQuark_,
                               OBJECT_PROPERTY_ID_REFCOUNT_,
                               GBL_TYPE_UINT32,
                               GBL_PROPERTY_FLAG_READ);

        gblPropertyTableInsert(GBL_CLASS_TYPE(pClass),
                               objectNameQuark_,
                               OBJECT_PROPERTY_ID_NAME_,
                               GBL_TYPE_STRING,
                               GBL_PROPERTY_FLAG_READ | GBL_PROPERTY_FLAG_WRITE |
                               GBL_PROPERTY_FLAG_LOAD | GBL_PROPERTY_FLAG_SAVE  |
                               GBL_PROPERTY_FLAG_CONSTRUCT);

        gblPropertyTableInsert(GBL_CLASS_TYPE(pClass),
                               objectParentQuark_,
                               OBJECT_PROPERTY_ID_PARENT_,
                               GBL_TYPE_OBJECT,
                               GBL_PROPERTY_FLAG_READ | GBL_PROPERTY_FLAG_WRITE |
                               GBL_PROPERTY_FLAG_CONSTRUCT);

        gblPropertyTableInsert(GBL_CLASS_TYPE(pClass),
                               objectUserdataQuark_,
                               OBJECT_PROPERTY_ID_USERDATA_,
                               GBL_TYPE_POINTER,
                               GBL_PROPERTY_FLAG_READ | GBL_PROPERTY_FLAG_WRITE |
                               GBL_PROPERTY_FLAG_CONSTRUCT);
    }

    strcpy(pClass->iVariantIFace.pSetValueFmt, "p");
    strcpy(pClass->iVariantIFace.pGetValueFmt, "p");
    pClass->iVariantIFace.supportedOps =    GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT      |
                                            GBL_IVARIANT_OP_FLAG_CONSTRUCT_COPY         |
                                            GBL_IVARIANT_OP_FLAG_CONSTRUCT_MOVE         |
                                            GBL_IVARIANT_OP_FLAG_CONSTRUCT_VALUE_COPY   |
                                            GBL_IVARIANT_OP_FLAG_CONSTRUCT_VALUE_MOVE   |
                                            GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY         |
                                            GBL_IVARIANT_OP_FLAG_SET_COPY               |
                                            GBL_IVARIANT_OP_FLAG_SET_VALUE_MOVE         |
                                            GBL_IVARIANT_OP_FLAG_SET_MOVE               |
                                            GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY         |
                                            GBL_IVARIANT_OP_FLAG_GET_VALUE_TAKE         |
                                            GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK;
    pClass->iVariantIFace.pFnConstruct =    GblObjectClass_ivariantIFace_construct_;
    pClass->iVariantIFace.pFnDestruct  =    GblObjectClass_ivariantIFace_destruct_;
    pClass->iVariantIFace.pFnCompare   =    GblObjectClass_ivariantIFace_compare_;
    pClass->iVariantIFace.pFnGet       =    GblObjectClass_ivariantIFace_get_;
    pClass->iVariantIFace.pFnSet       =    GblObjectClass_ivariantIFace_set_;
    pClass->iVariantIFace.pFnLoad      =    GblObjectClass_ivariantIFace_load_;
    pClass->iVariantIFace.pFnSave      =    GblObjectClass_ivariantIFace_save_;

    pClass->iTableIFace.pFnIndex       =    GblObjectClass_iTableIFace_index_;
    pClass->iTableIFace.pFnNewIndex    =    GblObjectClass_iTableIFace_newIndex_;
    pClass->iTableIFace.pFnNextIndex   =    GblObjectClass_iTableIFace_nextIndex_;

    pClass->iEventHandlerIFace.pFnEvent = GblObjectClass_iEventHandlerIFace_event_;

    pClass->pFnConstructor = GblObjectClass_constructor_;
    pClass->pFnDestructor  = GblObjectClass_destructor_;
    pClass->pFnPropertyGet = GblObjectClass_propertyGet_;
    pClass->pFnPropertySet = GblObjectClass_propertySet_;
    GBL_API_END();
}

static GBL_RESULT GblObjectClass_final_(GblObjectClass* pClass, void* pClassData, GblContext hCtx) GBL_NOEXCEPT {
    GBL_UNUSED(pClassData);
    GBL_API_BEGIN(hCtx);
    const GblBool result = gblPropertyTableEraseAll(gblTypeFromClass(GBL_CLASS(pClass)));
    GBL_API_VERIFY_EXPRESSION(result);

    // static destructor for last instance of class
    if(!gblTypeClassRefCount(GBL_TYPE_OBJECT)) {
        GBL_API_CALL(gblPropertyTableFinalize());
    }
    GBL_API_END();
}

static GBL_RESULT GblObject_init_(GblInstance* pInstance, GblContext hCtx) {
    GBL_API_BEGIN(hCtx); {
        GblObjectClass* pClass = GBL_OBJECT_GET_CLASS(pInstance);
        GBL_API_CALL(pClass->pFnConstructor(GBL_OBJECT(pInstance)));
    } GBL_API_END();
}

extern GBL_RESULT GblObject_typeRegister_(GblContext hCtx) {
    GBL_API_BEGIN(hCtx);
    gblTypeRegisterBuiltin(23,
      GBL_TYPE_INVALID,
      "Object",
      &((const GblTypeInfo) {
          .pFnClassInit     = (GblTypeClassInitFn)GblObjectClass_init_,
          .pFnClassFinalize = (GblTypeClassFinalizeFn)GblObjectClass_final_,
          .classSize        = sizeof(GblObjectClass),
          .classAlign       = GBL_ALIGNOF(GblObjectClass),
          .pFnInstanceInit  = (GblTypeInstanceInitFn)GblObject_init_,
          .instanceSize     = sizeof(GblObject),
          .instanceAlign    = GBL_ALIGNOF(GblObject),
          .interfaceCount   = 4,
          .pInterfaceMap    = ((const GblTypeInterfaceMapEntry[]) {
              {
                  .interfaceType = GBL_TYPE_IVARIANT,
                  .classOffset   = offsetof(GblObjectClass, iVariantIFace)
              }, {
                  .interfaceType = GBL_TYPE_ITABLE,
                  .classOffset   = offsetof(GblObjectClass, iTableIFace)
              }, {
                  .interfaceType = GBL_TYPE_IEVENT_HANDLER,
                  .classOffset   = offsetof(GblObjectClass, iEventHandlerIFace)
              }, {
                  .interfaceType = GBL_TYPE_IEVENT_FILTER,
                  .classOffset   = offsetof(GblObjectClass, iEventFilterIFace)
              }
          })
      }),
      (GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED      |
      GBL_TYPE_FUNDAMENTAL_FLAG_INSTANTIABLE  |
      GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE));
    GBL_API_END();
}


GBL_EXPORT GblObject* GblObject_new(GblType type, ...) GBL_NOEXCEPT {
    GblObject* pObject = NULL;
    va_list    varArgs;
    GBL_API_BEGIN(gblTypeContext(type));
    va_start(varArgs, type);
    pObject = GblObject_newVaList(type, &varArgs);
    va_end(varArgs);
    GBL_API_END_BLOCK();
    return pObject;
}

GBL_EXPORT GblObject* GblObject_newVaList(GblType type, va_list* pVarArgs) {
    GblObject* pObject = NULL;
    GBL_API_BEGIN(gblTypeContext(type));
    GBL_API_VERIFY_TYPE(type, GBL_TYPE_OBJECT);
    pObject = GBL_OBJECT(gblTypeInstanceCreate(type));
    GBL_API_VERIFY_POINTER(pObject);
    GBL_API_VERIFY_POINTER(pVarArgs);

    // check whether there are even constructable properties or not

    const char* pKey = NULL;
    while((pKey = va_arg(*pVarArgs, const char*))) {
        if(!pKey) break;
        GblVariant variant = GBL_VARIANT_INIT;
        const GblProperty* pProp = GblObject_propertyFindString(pObject, pKey);
        if(pProp) {
            GBL_API_CALL(GblVariant_constructValueCopyVaList(&variant, GblProperty_valueType(pProp), pVarArgs));
            if(GblProperty_flags(pProp) & GBL_PROPERTY_FLAG_CONSTRUCT) {
                GBL_API_CALL(GblObject_propertySet(pObject, pProp, &variant));
            } else {
                GBL_API_WARN("[GblObject] Failed to construct with non-constructible property: %s[%s]",
                                   gblTypeName(type), pKey);
            }
        } else {
            GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY, "[GblObject] Failed to construct with unknown property: %s[%s]",
                               gblTypeName(type), pKey);
        }
        GblVariant_destruct(&variant);
    }

    GblObjectClass* pClass = GBL_OBJECT_GET_CLASS(pObject);
    if(pClass->pFnConstructed) GBL_API_CALL(pClass->pFnConstructed(pObject));

    GBL_API_END_BLOCK();
    return pObject;
}

GBL_EXPORT GblObject* GblObject_newVariants(GblType type, GblUint propertyCount, const char* pNames[], const GblVariant* pValues) GBL_NOEXCEPT {
    GblObject* pObject = NULL;
    GBL_API_BEGIN(gblTypeContext(type));
    GBL_API_VERIFY_TYPE(type, GBL_TYPE_OBJECT);
    pObject = GBL_OBJECT(gblTypeInstanceCreate(type));
    GBL_API_VERIFY_POINTER(pObject);

    for(GblUint p = 0; p < propertyCount; ++p) {
        const GblProperty* pProp = GblObject_propertyFindString(pObject, pNames[p]);

        if(pProp) {
            if(GblProperty_flags(pProp) & GBL_PROPERTY_FLAG_CONSTRUCT) {
                GBL_API_CALL(GblObject_propertySet(pObject, pProp, &pValues[p]));
            } else {
                GBL_API_WARN("[GblObject]: Failed to construct with non-constructible property: %s[%s]",
                             gblTypeName(type), pNames[p]);
            }
        } else {
            GBL_API_WARN("[GblObject]: Failed to construct with unknown property: %s[%s]",
                         gblTypeName(type), pNames[p]);
        }
    }

    GblObjectClass* pClass = GBL_OBJECT_GET_CLASS(pObject);
    // can initialize based on constructor properties that got set
    if(pClass->pFnConstructed) GBL_API_CALL(pClass->pFnConstructed(pObject));

    GBL_API_END_BLOCK();
    return pObject;
}

GBL_API GblObject_get(const GblObject* pSelf, ...) GBL_NULL_TERMINATED GBL_NOEXCEPT {
    va_list varArgs;
    va_start(varArgs, pSelf);
    GBL_API_BEGIN(gblTypeContext(GBL_INSTANCE_TYPE(pSelf)));
    GBL_API_CALL(GblObject_getVaList(pSelf, &varArgs));
    GBL_API_END_BLOCK();
    va_end(varArgs);
    return GBL_API_RESULT();
}

GBL_API GblObject_set(GblObject* pSelf, ...) GBL_NULL_TERMINATED GBL_NOEXCEPT {
    va_list varArgs;
    va_start(varArgs, pSelf);
    GBL_API_BEGIN(gblTypeContext(GBL_INSTANCE_TYPE(pSelf)));
    GBL_API_CALL(GblObject_setVaList(pSelf, &varArgs));
    GBL_API_END_BLOCK();
    va_end(varArgs);
    return GBL_API_RESULT();
}

GBL_API GblObject_getVaList(const GblObject* pSelf, va_list* pVarArgs) GBL_NOEXCEPT {
    GBL_API_BEGIN(gblTypeContext(GBL_INSTANCE_TYPE(pSelf)));
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_POINTER(pVarArgs);

    const char* pKey = NULL;
    while((pKey = va_arg(*pVarArgs, const char*))) {
        const GblProperty* pProp = GblObject_propertyFindString(pSelf, pKey);
        if(pProp) {
            GblVariant variant;
            GBL_API_CALL(GblVariant_constructDefault(&variant, GBL_TYPE_NIL));
            GBL_API_CALL(GblObject_propertyGetString(pSelf, pKey, &variant));
            GBL_API_CALL(GblVariant_getValueCopyVaList(&variant, pVarArgs));
            if(GblProperty_flags(pProp) & GBL_PROPERTY_FLAG_READ) {
                ;
            } else {
                GBL_API_WARN("[GblObject]: Failed to get non-readable property: %s[%s]",
                             gblTypeName(GBL_INSTANCE_TYPE(pSelf)), pKey);
            }
            GblVariant_destruct(&variant);
        } else {
            GBL_API_WARN("[GblObject]: Failed to get unknown property: %s[%s]",
                         gblTypeName(GBL_INSTANCE_TYPE(pSelf)), pKey);
        }


    }
/*
    GBL_API_CALL(GblObject_getVariants(pSelf, p, pKeys, pVariants));

    for(GblUint v = 0; v < p; ++v) {
        gblVariantDestruct(&pVariants[v]);
    }
    */
    GBL_API_END();
}

GBL_API GblObject_setVaList(GblObject* pSelf, va_list* pVarArgs) GBL_NOEXCEPT {
    GBL_API_BEGIN(gblTypeContext(GBL_INSTANCE_TYPE(pSelf)));
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_POINTER(pVarArgs);

    GblSize count = gblPropertyTableCount(GBL_INSTANCE_TYPE(pSelf));
    GblVariant*  pVariants  = GBL_ALLOCA(sizeof(GblVariant) * count);
    const char** pKeys      = GBL_ALLOCA(sizeof(const char*) * count);

    GblUint p = 0;
    const char* pKey = NULL;
    while((pKey = va_arg(*pVarArgs, const char*))) {
        pKeys[p] = pKey;
        const GblProperty* pProp = GblObject_propertyFindString(pSelf, pKey);
        if(pProp) {
            GBL_API_CALL(GblVariant_constructValueCopyVaList(&pVariants[p], GblProperty_valueType(pProp), pVarArgs));
            if(GblProperty_flags(pProp) & GBL_PROPERTY_FLAG_WRITE) {
                ++p;
            } else {
                GBL_API_WARN("[GblObject]: Failed to set non-writable property: %s[%s]",
                             gblTypeName(GBL_INSTANCE_TYPE(pSelf)), pKey);
            }
        } else {
            GBL_API_WARN("[GblObject]: Failed to set unknown property: %s[%s]",
                         gblTypeName(GBL_INSTANCE_TYPE(pSelf)), pKey);
        }
    }

    GBL_API_CALL(GblObject_setVariants(pSelf, p, pKeys, pVariants));
    GBL_API_END();
}

GBL_API GblObject_getVariants(const GblObject* pSelf, GblUint propertyCount, const char* pNames[], GblVariant* pValues) GBL_NOEXCEPT {
    GBL_API_BEGIN(gblTypeContext(GBL_INSTANCE_TYPE(pSelf)));
    for(GblUint p = 0; p < propertyCount; ++p) {
        GBL_API_CALL(GblObject_propertyGetString(pSelf, pNames[p], &pValues[p]));
    }
    GBL_API_END();
}

GBL_API GblObject_setVariants(GblObject* pSelf, GblUint propertyCount, const char* pNames[], const GblVariant* pValues) GBL_NOEXCEPT {
    GBL_API_BEGIN(gblTypeContext(GBL_INSTANCE_TYPE(pSelf)));
    for(GblUint p = 0; p < propertyCount; ++p) {
        GBL_API_CALL(GblObject_propertySetString(pSelf, pNames[p], &pValues[p]));
    }
    GBL_API_END();
}

GBL_EXPORT GblObject* GblObject_ref(GblObject* pObject) GBL_NOEXCEPT {
    GBL_API_BEGIN(gblTypeContext(GBL_INSTANCE_TYPE(pObject)));
    GBL_API_VERIFY_POINTER(pObject);
    if(pObject) {
       ++pObject->refCounter;
    }
    GBL_API_END_BLOCK();
    return pObject;
}
GBL_EXPORT GblRefCount GblObject_unref(GblObject* pObject) GBL_NOEXCEPT {
    GblRefCount count = 0;
    GBL_API_BEGIN(gblTypeContext(GBL_INSTANCE_TYPE(pObject)));
    if(pObject) {
        GBL_API_VERIFY_EXPRESSION(pObject->refCounter);
        if(--pObject->refCounter == 0) {
            GblObjectClass* pClass = GBL_OBJECT_GET_CLASS(pObject);
            pClass->pFnDestructor(pObject);
            gblTypeInstanceDestroy(&pObject->base);
        } else {
            count = pObject->refCounter;
        }
    }
    GBL_API_END_BLOCK();
    return count;
}

GBL_EXPORT GblRefCount GblObject_refCount(const GblObject* pObject) GBL_NOEXCEPT {
    return pObject->refCounter;
}




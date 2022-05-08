#include <gimbal/objects/gimbal_object.h>
#include <gimbal/types/gimbal_variant.h>
#include <gimbal/objects/gimbal_property.h>
#include <gimbal/objects/gimbal_context.h>
#include <gimbal/objects/gimbal_event.h>

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
        pVariant->pVoid = GblInstance_create(pVariant->type);
    } else if(op == GBL_IVARIANT_OP_FLAG_CONSTRUCT_COPY || op == GBL_IVARIANT_OP_FLAG_CONSTRUCT_VALUE_COPY) {
        pVariant->pVoid = pArgs[0].pVoid? GblObject_ref(GBL_OBJECT(pArgs[0].pVoid)) : NULL;
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
        pSelf->type = GBL_NIL_TYPE;
    }
    GBL_API_END();
}


static GBL_RESULT GblObjectClass_ivariantIFace_set_(GblVariant* pSelf, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_UNUSED(argc);
    pSelf->pVoid = pArgs[0].pVoid;
    if(op == GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY || op == GBL_IVARIANT_OP_FLAG_SET_COPY) {
        GblObject_ref(GBL_OBJECT(pSelf->pVoid));
    } else if(op == GBL_IVARIANT_OP_FLAG_SET_MOVE || op == GBL_IVARIANT_OP_FLAG_SET_VALUE_MOVE) {
        pArgs[0].type = GBL_NIL_TYPE;
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
    if(GblVariant_type(pKey) == GBL_STRING_TYPE) {
        const char* pString = NULL;
        GBL_API_CALL(GblVariant_getValuePeek(pKey, &pString));
        GBL_API_RESULT() = GblObject_propertyGetString(pObject, pString, pValue);
        if(!GBL_RESULT_SUCCESS(GBL_API_RESULT()) && pObject->parentITableIndexFallthrough) {
            const GblObject* pParent = GblObject_parent(pObject);
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
    if(GblVariant_type(pKey) == GBL_STRING_TYPE) {
        const char* pString = NULL;
        GBL_API_CALL(GblVariant_getValuePeek(pKey, &pString));
        GBL_API_RESULT() = GblObject_propertySetString(pObject, pString, pValue);
        if(!GBL_RESULT_SUCCESS(GBL_API_RESULT()) && pObject->parentITableIndexFallthrough) {
            const GblObject* pParent = GblObject_parent(pObject);
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
    GBL_API_CALL(GblVariant_setValueCopy(pNextKey, GBL_NIL_TYPE, NULL));
    GBL_API_CALL(GblVariant_setValueCopy(pNextValue, GBL_NIL_TYPE, NULL));
    GblObject* pObject = GBL_OBJECT(pITable);
    // get the first one
    if(!pKey || GblVariant_type(pKey) == GBL_NIL_TYPE) {
        const GblProperty* pProp = GblObject_propertyNext(pObject, NULL, GBL_PROPERTY_FLAG_READ);
        if(pProp) {
            GBL_API_CALL(GblVariant_setValueCopy(pNextKey, GBL_STRING_TYPE, GblProperty_nameString(pProp)));
            GBL_API_CALL(GblObject_propertyGet(pObject, pProp, pNextValue));
            GBL_API_RESULT() = GBL_RESULT_SUCCESS;
        }
    } else if(GblVariant_type(pKey) == GBL_STRING_TYPE) {
        const char* pString = NULL;
        GBL_API_CALL(GblVariant_getValuePeek(pKey, &pString));
        const GblProperty* pProp = GblObject_propertyFindString(pObject, pString);
        if(pProp) {
            pProp = GblObject_propertyNext(pObject, pProp, GBL_PROPERTY_FLAG_READ);
            if(pProp) {
                GBL_API_CALL(GblVariant_setValueCopy(pNextKey, GBL_STRING_TYPE, GblProperty_nameString(pProp)));
                GBL_API_CALL(GblObject_propertyGet(pObject, pProp, pNextValue));
            }
            GBL_API_RESULT() = GBL_RESULT_SUCCESS;
        }
    }

    GBL_API_END();
}

// ========== IEVENTHANDLER IFACE ==========

static GBL_RESULT GblObjectClass_iEventHandlerIFace_event_(GblIEventHandler* pHandler, GblEvent* pEvent) {
    GBL_UNUSED(pHandler && pEvent);
    GBL_API_BEGIN(NULL);
    GBL_API_END();
}


GBL_API GblObject_eventSend(GblObject* pSelf, GblEvent* pEvent) GBL_NOEXCEPT {
    GBL_API_BEGIN(GblType_context(GBL_INSTANCE_TYPE(pSelf)));
    GblObject* pParent = GblObject_parent(pSelf);
    if(pParent) {
        GBL_API_CALL(GblObject_eventNotify(pParent, pEvent));
    }
    GBL_API_END();
}


GBL_API GblObject_eventNotify(GblObject* pSelf, GblEvent* pEvent) GBL_NOEXCEPT {
    GBL_API_BEGIN(GblType_context(GBL_INSTANCE_TYPE(pSelf)));
    GblObject* pObject = GBL_OBJECT(pSelf);


     // Run the event through the event filters
     const GblSize eventFilterCount = GblObject_eventFilterCount(pObject);
     for(GblSize f = 0; f < eventFilterCount; ++f)
     {
         GblIEventFilter* pFilter = GblObject_eventFilterAt(pSelf, f);
         GBL_API_CALL(GblIEventFilter_eventFilter(pFilter, GBL_IEVENT_HANDLER(pObject), pEvent));
         if(GblEvent_state(pEvent) == GBL_EVENT_STATE_ACCEPTED) {
             GBL_API_DONE();
         }
     }

     // Send it to the event handler if the event handler
    GBL_API_CALL(GblIEventHandler_event(GBL_IEVENT_HANDLER(pSelf), pEvent));
    if(GblEvent_state(pEvent) == GBL_EVENT_STATE_ACCEPTED) {
        GBL_API_DONE();
    }

    // Propagate up to parent
    GBL_API_CALL(GblObject_eventSend(pSelf, pEvent));

    GBL_API_END();
}

static GBL_RESULT GblObject_ensureEventFilters_(GblObject* pSelf) GBL_NOEXCEPT {
    GBL_API_BEGIN(GblType_context(GBL_INSTANCE_TYPE(pSelf)));
    GBL_API_CALL(GblObject_ensureExtraData_(pSelf));
    if(!pSelf->pExtraData->pEventFilters) {
        pSelf->pExtraData->pEventFilters = GBL_API_MALLOC(GBL_OBJECT_EVENT_FILTER_VECTOR_SIZE_);
        GBL_API_CALL(gblVectorConstruct(pSelf->pExtraData->pEventFilters,
                                        GblType_context(GBL_INSTANCE_TYPE(pSelf)),
                                        sizeof(GblIEventFilter*),
                                        GBL_OBJECT_EVENT_FILTER_VECTOR_SIZE_));
    }
    GBL_API_END();
}

GBL_API GblObject_eventFilterInstall(GblObject* pSelf, GblIEventFilter* pFilter) GBL_NOEXCEPT {
    GBL_API_BEGIN(GblType_context(GBL_INSTANCE_TYPE(pSelf)));
    GBL_API_VERIFY_POINTER(pFilter);
    GBL_API_CALL(GblObject_ensureEventFilters_(pSelf));
    GBL_API_CALL(gblVectorPushBack(pSelf->pExtraData->pEventFilters, &pFilter));
    GBL_API_END();
}
GBL_API GblObject_eventFilterUninstall(GblObject* pSelf, GblIEventFilter* pFilter) GBL_NOEXCEPT {
    GblVector* pVector = NULL;
    GBL_API_BEGIN(GblType_context(GBL_INSTANCE_TYPE(pSelf)));
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
    GBL_API_BEGIN(GblType_context(GBL_INSTANCE_TYPE(pSelf)));
    if(pSelf->pExtraData && pSelf->pExtraData->pEventFilters)
        GBL_API_CALL(gblVectorAt(pSelf->pExtraData->pEventFilters, index, (void**)&ppFilter));
    GBL_API_END_BLOCK();
    return ppFilter? *ppFilter : NULL;
}
GblSize GblObject_eventFilterCount(const GblObject* pSelf) GBL_NOEXCEPT {
    GblSize count = 0;
    GBL_API_BEGIN(GblType_context(GBL_INSTANCE_TYPE(pSelf)));
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
    GBL_API_BEGIN(GblType_context(GBL_INSTANCE_TYPE(pSelf)));
    pSelf->refCounter = 1;
    GBL_API_END();
}

static GBL_RESULT GblObjectClass_destructor_(GblObject* pSelf) {
    GBL_API_BEGIN(GblType_context(GBL_INSTANCE_TYPE(pSelf)));
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
    GBL_API_BEGIN(GblType_context(GBL_INSTANCE_TYPE(pSelf)));
    switch(slot) {
    case OBJECT_PROPERTY_ID_NAME_:      GblVariant_setValueCopy(pValue, GblProperty_valueType(pProp), GblObject_name(pSelf));   break;
    case OBJECT_PROPERTY_ID_PARENT_:    GblVariant_setValueMove(pValue, GblProperty_valueType(pProp), GblObject_parent(pSelf)); break;
    case OBJECT_PROPERTY_ID_REFCOUNT_:  GblVariant_setValueCopy(pValue, GblProperty_valueType(pProp), GblObject_refCount(pSelf));  break;
    case OBJECT_PROPERTY_ID_USERDATA_:  GblVariant_setValueCopy(pValue, GblProperty_valueType(pProp), GblObject_userdata(pSelf)); break;
    default: GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY, "Reading unhandled property: %s", GblProperty_nameString(pProp));
    }
    GBL_API_END();
}

static GBL_RESULT GblObjectClass_propertySet_(GblObject* pSelf, GblUint slot, const GblVariant* pValue, const GblProperty* pProp) {
    GBL_API_BEGIN(GblType_context(GBL_INSTANCE_TYPE(pSelf)));
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


static GBL_RESULT GblObjectClass_init_(GblObjectClass* pClass, void* pData, GblContext* pCtx) GBL_NOEXCEPT {
    GBL_UNUSED(pData);
    GBL_API_BEGIN(pCtx);

    // static constructor for first instance of class
    if(!GblClass_refCountFromType(GBL_OBJECT_TYPE)) {
        GBL_API_CALL(gblPropertyTableInitialize(pCtx, GBL_OBJECT_PROPERTY_TABLE_CAPACITY_DEFAULT_));
        objectNameQuark_            = GblQuark_fromStringStatic("name");
        objectUserdataQuark_        = GblQuark_fromStringStatic("userdata");
        objectRefCountQuark_        = GblQuark_fromStringStatic("refCount");
        objectParentQuark_          = GblQuark_fromStringStatic("parent");

        gblPropertyTableInsert(GBL_CLASS_TYPE(pClass),
                               objectRefCountQuark_,
                               OBJECT_PROPERTY_ID_REFCOUNT_,
                               GBL_UINT32_TYPE,
                               GBL_PROPERTY_FLAG_READ);

        gblPropertyTableInsert(GBL_CLASS_TYPE(pClass),
                               objectNameQuark_,
                               OBJECT_PROPERTY_ID_NAME_,
                               GBL_STRING_TYPE,
                               GBL_PROPERTY_FLAG_READ | GBL_PROPERTY_FLAG_WRITE |
                               GBL_PROPERTY_FLAG_LOAD | GBL_PROPERTY_FLAG_SAVE  |
                               GBL_PROPERTY_FLAG_CONSTRUCT);

        gblPropertyTableInsert(GBL_CLASS_TYPE(pClass),
                               objectParentQuark_,
                               OBJECT_PROPERTY_ID_PARENT_,
                               GBL_OBJECT_TYPE,
                               GBL_PROPERTY_FLAG_READ | GBL_PROPERTY_FLAG_WRITE |
                               GBL_PROPERTY_FLAG_CONSTRUCT);

        gblPropertyTableInsert(GBL_CLASS_TYPE(pClass),
                               objectUserdataQuark_,
                               OBJECT_PROPERTY_ID_USERDATA_,
                               GBL_POINTER_TYPE,
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

static GBL_RESULT GblObjectClass_final_(GblObjectClass* pClass, void* pClassData, GblContext* pCtx) GBL_NOEXCEPT {
    GBL_UNUSED(pClassData);
    GBL_API_BEGIN(pCtx);

    if(!GblClass_refCountFromType(GBL_CLASS_TYPE(pClass))) {
        const GblBool result = gblPropertyTableEraseAll(GblType_fromClass(GBL_CLASS(pClass)));
        GBL_API_VERIFY_EXPRESSION(result);
    }

    // static destructor for last instance of class
    if(!GblClass_refCountFromType(GBL_OBJECT_TYPE)) {
        GBL_API_CALL(gblPropertyTableFinalize());
    }
    GBL_API_END();
}

static GBL_RESULT GblObject_init_(GblInstance* pInstance, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx); {
        GblObjectClass* pClass = GBL_OBJECT_GET_CLASS(pInstance);
        GBL_API_CALL(pClass->pFnConstructor(GBL_OBJECT(pInstance)));
    } GBL_API_END();
}

extern GBL_RESULT GblObject_typeRegister_(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblType_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_OBJECT,
      GBL_INVALID_TYPE,
      GblQuark_internStringStatic("Object"),
      &((const GblTypeInfo) {
          .pFnClassInit     = (GblTypeClassInitializeFn)GblObjectClass_init_,
          .pFnClassFinal    = (GblTypeClassFinalizeFn)GblObjectClass_final_,
          .classSize        = sizeof(GblObjectClass),
          .pFnInstanceInit  = (GblTypeInstanceInitializeFn)GblObject_init_,
          .instanceSize     = sizeof(GblObject),
          .interfaceCount   = 4,
          .pInterfaceMap    = ((const GblTypeInterfaceMapEntry[]) {
              {
                  .interfaceType = GBL_IVARIANT_TYPE,
                  .classOffset   = offsetof(GblObjectClass, iVariantIFace)
              }, {
                  .interfaceType = GBL_ITABLE_TYPE,
                  .classOffset   = offsetof(GblObjectClass, iTableIFace)
              }, {
                  .interfaceType = GBL_IEVENT_HANDLER_TYPE,
                  .classOffset   = offsetof(GblObjectClass, iEventHandlerIFace)
              }, {
                  .interfaceType = GBL_IEVENT_FILTER_TYPE,
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
    va_start(varArgs, type);
    GBL_API_BEGIN(GblType_context(type));
    pObject = GblObject_newVaList(type, &varArgs);
    GBL_API_END_BLOCK();
    va_end(varArgs);
    return pObject;
}

GBL_EXPORT GblObject* GblObject_newWithClass(GblObjectClass* pClass, ...) GBL_NOEXCEPT
{
    const GblType type = GBL_CLASS_TYPE(pClass);
    GblObject* pObject = NULL;
    va_list    varArgs;
    va_start(varArgs, pClass);
    GBL_API_BEGIN(GblType_context(type));
    pObject = GblObject_newVaListWithClass(pClass, &varArgs);
    GBL_API_END_BLOCK();
    va_end(varArgs);
    return pObject;

}

GBL_EXPORT GBL_RESULT GblObject_newInPlace(GblObject* pSelf, GblType type, ...) GBL_NOEXCEPT {
    va_list varArgs;
    va_start(varArgs, type);
    GBL_API_BEGIN(GblType_context(type));
    GBL_API_CALL(GblObject_newInPlaceVaList(pSelf, type, &varArgs));
    GBL_API_END_BLOCK();
    va_end(varArgs);
    return GBL_API_RESULT();
}

GBL_EXPORT GBL_RESULT GblObject_newInPlaceWithClass(GblObject* pSelf, GblObjectClass* pClass, ...) GBL_NOEXCEPT {
    const GblType type = GBL_CLASS_TYPE(pClass);
    va_list    varArgs;
    va_start(varArgs, pClass);
    GBL_API_BEGIN(GblType_context(type));
    GBL_API_CALL(GblObject_newInPlaceVaListWithClass(pSelf, pClass, &varArgs));
    GBL_API_END_BLOCK();
    va_end(varArgs);
    return GBL_API_RESULT();
}

static  GBL_RESULT GblObject_newInPlaceVaList_(GblObject* pSelf, GblType type, va_list* pVarArgs) GBL_NOEXCEPT {
    GBL_API_BEGIN(GblType_context(type));
    GBL_API_VERIFY_TYPE(type, GBL_OBJECT_TYPE);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_POINTER(pVarArgs);

    // check whether there are even constructable properties or not

    const char* pKey = NULL;
    while((pKey = va_arg(*pVarArgs, const char*))) {
        if(!pKey) break;
        GblVariant variant = GBL_VARIANT_INIT;
        const GblProperty* pProp = GblObject_propertyFindString(pSelf, pKey);
        if(pProp) {
            GBL_API_CALL(GblVariant_constructValueCopyVaList(&variant, GblProperty_valueType(pProp), pVarArgs));
            if(GblProperty_flags(pProp) & GBL_PROPERTY_FLAG_CONSTRUCT) {
                GBL_API_CALL(GblObject_propertySet(pSelf, pProp, &variant));
            } else {
                GBL_API_WARN("[GblObject] Failed to construct with non-constructible property: %s[%s]",
                                   GblType_name(type), pKey);
            }
        } else {
            GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY,
                               "[GblObject] Failed to construct with unknown property: %s[%s]",
                               GblType_name(type), pKey);
        }
        GblVariant_destruct(&variant);
    }

    GblObjectClass* pClass = GBL_OBJECT_GET_CLASS(pSelf);
    if(pClass->pFnConstructed) GBL_API_CALL(pClass->pFnConstructed(pSelf));

    GBL_API_END();
}

GBL_EXPORT GblObject* GblObject_newVaList(GblType type, va_list* pVarArgs) GBL_NOEXCEPT {
    GblObject* pObject = NULL;
    GBL_API_BEGIN(GblType_context(type));
    GBL_API_VERIFY_TYPE(type, GBL_OBJECT_TYPE);
    pObject = GBL_OBJECT(GblInstance_create(type));
    GBL_API_CALL(GblObject_newInPlaceVaList_(pObject, type, pVarArgs));
    GBL_API_END_BLOCK();
    return pObject;
}

GBL_EXPORT GblObject* GblObject_newVaListWithClass(GblObjectClass* pClass, va_list* pVarArgs) GBL_NOEXCEPT {
    GblObject* pObject = NULL;
    const GblType type = GBL_CLASS_TYPE(pClass);
    GBL_API_BEGIN(GblType_context(type));
    GBL_API_VERIFY_TYPE(type, GBL_OBJECT_TYPE);
    pObject = GBL_OBJECT(GblInstance_createWithClass(GBL_CLASS(pClass)));
    GBL_API_CALL(GblObject_newInPlaceVaList_(pObject, type, pVarArgs));
    GBL_API_END_BLOCK();
    return pObject;
}

GBL_EXPORT GBL_RESULT GblObject_newInPlaceVaList(GblObject* pSelf, GblType type, va_list* pVarArgs) GBL_NOEXCEPT {
    GBL_API_BEGIN(GblType_context(type));
    GBL_API_VERIFY_TYPE(type, GBL_OBJECT_TYPE);
    GBL_API_CALL(GblInstance_construct(GBL_INSTANCE(pSelf), type));
    pSelf->constructedInPlace = 1;
    GBL_API_CALL(GblObject_newInPlaceVaList_(pSelf, type, pVarArgs));
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblObject_newInPlaceVaListWithClass(GblObject* pSelf, GblObjectClass* pClass, va_list* pVarArgs) GBL_NOEXCEPT {
    const GblType type = GBL_CLASS_TYPE(pClass);
    GBL_API_BEGIN(GblType_context(type));
    GBL_API_VERIFY_TYPE(type, GBL_OBJECT_TYPE);
    GBL_API_CALL(GblInstance_constructWithClass(GBL_INSTANCE(pSelf), GBL_CLASS(pClass)));
    pSelf->constructedInPlace = 1;
    GBL_API_CALL(GblObject_newInPlaceVaList_(pSelf, type, pVarArgs));
    GBL_API_END();
}

static GBL_RESULT GblObject_newInPlaceVariants_(GblObject* pSelf,
                                                   GblType type,
                                                   GblUint propertyCount,
                                                   const char* pNames[],
                                                   const GblVariant* pValues) GBL_NOEXCEPT
{
    GBL_API_BEGIN(GblType_context(type));
    GBL_API_VERIFY_TYPE(type, GBL_OBJECT_TYPE);
    GBL_API_VERIFY_POINTER(pSelf);

    for(GblUint p = 0; p < propertyCount; ++p) {
        const GblProperty* pProp = GblObject_propertyFindString(pSelf, pNames[p]);

        if(pProp) {
            if(GblProperty_flags(pProp) & GBL_PROPERTY_FLAG_CONSTRUCT) {
                GBL_API_CALL(GblObject_propertySet(pSelf, pProp, &pValues[p]));
            } else {
                GBL_API_WARN("[GblObject]: Failed to construct with non-constructible property: %s[%s]",
                             GblType_name(type), pNames[p]);
            }
        } else {
            GBL_API_WARN("[GblObject]: Failed to construct with unknown property: %s[%s]",
                         GblType_name(type), pNames[p]);
        }
    }

    GblObjectClass* pClass = GBL_OBJECT_GET_CLASS(pSelf);
    // can initialize based on constructor properties that got set
    if(pClass->pFnConstructed) GBL_API_CALL(pClass->pFnConstructed(pSelf));

    GBL_API_END();
}

GBL_EXPORT GblObject* GblObject_newVariants(GblType type, GblUint propertyCount, const char* pNames[], const GblVariant* pValues) GBL_NOEXCEPT {
    GblObject* pObject = NULL;
    GBL_API_BEGIN(GblType_context(type));
    GBL_API_VERIFY_TYPE(type, GBL_OBJECT_TYPE);
    pObject = GBL_OBJECT(GblInstance_create(type));
    GBL_API_CALL(GblObject_newInPlaceVariants_(pObject, type, propertyCount, pNames, pValues));
    GBL_API_END_BLOCK();
    return pObject;
}

GBL_EXPORT GblObject* GblObject_newVariantsWithClass(GblObjectClass* pClass,
                                                     GblUint propertyCount,
                                                     const char* pNames[],
                                                     const GblVariant* pValues) GBL_NOEXCEPT
{
    const GblType type = GBL_CLASS_TYPE(pClass);
    GblObject* pObject = NULL;
    GBL_API_BEGIN(GblType_context(type));
    GBL_API_VERIFY_TYPE(type, GBL_OBJECT_TYPE);
    pObject = GBL_OBJECT(GblInstance_createWithClass(GBL_CLASS(pClass)));
    GBL_API_CALL(GblObject_newInPlaceVariants_(pObject, type, propertyCount, pNames, pValues));
    GBL_API_END_BLOCK();
    return pObject;
}

GBL_EXPORT GBL_RESULT GblObject_newInPlaceVariants(GblObject* pSelf,
                                                   GblType type,
                                                   GblUint propertyCount,
                                                   const char* pNames[],
                                                   const GblVariant* pValues) GBL_NOEXCEPT
{
    GBL_API_BEGIN(GblType_context(type));
    GBL_API_VERIFY_TYPE(type, GBL_OBJECT_TYPE);
    GBL_API_CALL(GblInstance_construct(GBL_INSTANCE(pSelf), type));
    pSelf->constructedInPlace = 1;
    GBL_API_CALL(GblObject_newInPlaceVariants_(pSelf, type, propertyCount, pNames, pValues));
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblObject_newInPlaceVariantsWithClass(GblObject* pSelf,
                                                            GblObjectClass* pClass,
                                                            GblUint propertyCount,
                                                            const char* pNames[],
                                                            const GblVariant* pValues) GBL_NOEXCEPT
{
    const GblType type = GBL_CLASS_TYPE(pClass);
    GBL_API_BEGIN(GblType_context(type));
    GBL_API_VERIFY_TYPE(type, GBL_OBJECT_TYPE);
    GBL_API_CALL(GblInstance_constructWithClass(GBL_INSTANCE(pSelf), GBL_CLASS(pClass)));
    pSelf->constructedInPlace = 1;
    GBL_API_CALL(GblObject_newInPlaceVariants_(pSelf, type, propertyCount, pNames, pValues));
    GBL_API_END();
}

GBL_API GblObject_get(const GblObject* pSelf, ...) GBL_NULL_TERMINATED GBL_NOEXCEPT {
    va_list varArgs;
    va_start(varArgs, pSelf);
    GBL_API_BEGIN(GblType_context(GBL_INSTANCE_TYPE(pSelf)));
    GBL_API_CALL(GblObject_getVaList(pSelf, &varArgs));
    GBL_API_END_BLOCK();
    va_end(varArgs);
    return GBL_API_RESULT();
}

GBL_API GblObject_set(GblObject* pSelf, ...) GBL_NULL_TERMINATED GBL_NOEXCEPT {
    va_list varArgs;
    va_start(varArgs, pSelf);
    GBL_API_BEGIN(GblType_context(GBL_INSTANCE_TYPE(pSelf)));
    GBL_API_CALL(GblObject_setVaList(pSelf, &varArgs));
    GBL_API_END_BLOCK();
    va_end(varArgs);
    return GBL_API_RESULT();
}

GBL_API GblObject_getValue(const GblObject* pSelf, const char* pName, ...) GBL_NOEXCEPT {
    va_list varArgs;
    va_start(varArgs, pName);
    GBL_API_BEGIN(GblType_context(GBL_INSTANCE_TYPE(pSelf)));
    GblVariant variant;
    const GblProperty* pProp = GblObject_propertyFindString(pSelf, pName);
    GBL_API_VERIFY(GblProperty_flags(pProp) & GBL_PROPERTY_FLAG_READ,
                   GBL_RESULT_ERROR_INVALID_PROPERTY,
                   "[GblObject] Tried to get unreadable property: [%s]", pName);
    GBL_API_CALL(GblVariant_constructDefault(&variant, GBL_NIL_TYPE));
    GBL_API_CALL(GblObject_propertyGetString(pSelf, pName, &variant));
    GBL_API_CALL(GblVariant_getValueCopyVaList(&variant, &varArgs));
    GBL_API_CALL(GblVariant_destruct(&variant));
    GBL_API_END_BLOCK();
    va_end(varArgs);
    return GBL_API_RESULT();
}

GBL_API GblObject_getVaList(const GblObject* pSelf, va_list* pVarArgs) GBL_NOEXCEPT {
    GBL_API_BEGIN(GblType_context(GBL_INSTANCE_TYPE(pSelf)));
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_POINTER(pVarArgs);

    const char* pKey = NULL;
    while((pKey = va_arg(*pVarArgs, const char*))) {
        const GblProperty* pProp = GblObject_propertyFindString(pSelf, pKey);
        if(pProp) {
            GblVariant variant;
            GBL_API_CALL(GblVariant_constructDefault(&variant, GBL_NIL_TYPE));
            GBL_API_CALL(GblObject_propertyGetString(pSelf, pKey, &variant));
            GBL_API_CALL(GblVariant_getValueCopyVaList(&variant, pVarArgs));
            if(GblProperty_flags(pProp) & GBL_PROPERTY_FLAG_READ) {
                ;
            } else {
                GBL_API_WARN("[GblObject]: Failed to get non-readable property: %s[%s]",
                             GblType_name(GBL_INSTANCE_TYPE(pSelf)), pKey);
            }
            GBL_API_CALL(GblVariant_destruct(&variant));
        } else {
            GBL_API_WARN("[GblObject]: Failed to get unknown property: %s[%s]",
                         GblType_name(GBL_INSTANCE_TYPE(pSelf)), pKey);
        }


    }
    GBL_API_END();
}

GBL_API GblObject_setValue(GblObject* pSelf, const char* pName, ...) GBL_NOEXCEPT {
    va_list varArgs;
    va_start(varArgs, pName);
    GBL_API_BEGIN(GblType_context(GBL_INSTANCE_TYPE(pSelf)));
    GblVariant variant;
    const GblProperty* pProp = GblObject_propertyFindString(pSelf, pName);
    GBL_API_VERIFY(GblProperty_flags(pProp) & GBL_PROPERTY_FLAG_WRITE,
                   GBL_RESULT_ERROR_INVALID_PROPERTY,
                   "[GblObject] Tried to get unwritable property: [%s]", pName);
    GBL_API_CALL(GblVariant_constructValueCopyVaList(&variant, GblProperty_valueType(pProp), &varArgs));
    GBL_API_CALL(GblObject_setVariants(pSelf, 1, &pName, &variant));
    GBL_API_CALL(GblVariant_destruct(&variant));
    GBL_API_END_BLOCK();
    va_end(varArgs);
    return GBL_API_RESULT();
}


GBL_API GblObject_setVaList(GblObject* pSelf, va_list* pVarArgs) GBL_NOEXCEPT {
    GBL_API_BEGIN(GblType_context(GBL_INSTANCE_TYPE(pSelf)));
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
                             GblType_name(GBL_INSTANCE_TYPE(pSelf)), pKey);
            }
        } else {
            GBL_API_WARN("[GblObject]: Failed to set unknown property: %s[%s]",
                         GblType_name(GBL_INSTANCE_TYPE(pSelf)), pKey);
        }
    }

    GBL_API_CALL(GblObject_setVariants(pSelf, p, pKeys, pVariants));

    for(GblSize v = 0; v < p; ++v) {
       GBL_API_CALL(GblVariant_destruct(&pVariants[v]));
    }

    GBL_API_END();
}

GBL_API GblObject_getVariants(const GblObject* pSelf, GblUint propertyCount, const char* pNames[], GblVariant* pValues) GBL_NOEXCEPT {
    GBL_API_BEGIN(GblType_context(GBL_INSTANCE_TYPE(pSelf)));
    for(GblUint p = 0; p < propertyCount; ++p) {
        GBL_API_CALL(GblObject_propertyGetString(pSelf, pNames[p], &pValues[p]));
    }
    GBL_API_END();
}

GBL_API GblObject_setVariants(GblObject* pSelf, GblUint propertyCount, const char* pNames[], const GblVariant* pValues) GBL_NOEXCEPT {
    GBL_API_BEGIN(GblType_context(GBL_INSTANCE_TYPE(pSelf)));
    for(GblUint p = 0; p < propertyCount; ++p) {
        GBL_API_CALL(GblObject_propertySetString(pSelf, pNames[p], &pValues[p]));
    }
    GBL_API_END();
}

GBL_EXPORT GblObject* GblObject_ref(GblObject* pObject) GBL_NOEXCEPT {
    GBL_API_BEGIN(GblType_context(GBL_INSTANCE_TYPE(pObject)));
    GBL_API_VERIFY_POINTER(pObject);
    if(pObject) {
       ++pObject->refCounter;
    }
    GBL_API_END_BLOCK();
    return pObject;
}
GBL_EXPORT GblRefCount GblObject_unref(GblObject* pObject) GBL_NOEXCEPT {
    GblRefCount count = 0;
    GBL_API_BEGIN(GblType_context(GBL_INSTANCE_TYPE(pObject)));
    if(pObject) {
        GBL_API_VERIFY_EXPRESSION(pObject->refCounter);
        if(--pObject->refCounter == 0) {
            GblObjectClass* pClass = GBL_OBJECT_GET_CLASS(pObject);
            const GblBool classOwned              = pObject->classOwned;
            const GblBool classConstructedInPlace = pObject->classConstructedInPlace;
            GBL_API_CALL(pClass->pFnDestructor(pObject));
            if(!pObject->constructedInPlace)
                GBL_API_CALL(GblInstance_destroy(&pObject->base));
            else
                GBL_API_CALL(GblInstance_destruct(&pObject->base));
#if 0
            if(classOwned) {
                if(!classConstructedInPlace)
                    GBL_API_CALL(GblClass_destroyFloating(GBL_CLASS(pClass)));
                else
                    GBL_API_CALL(GblClass_destructFloating(GBL_CLASS(pClass)));
            }
#endif
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


GBL_EXPORT GblContext* GblObject_contextFind(GblObject* pSelf) GBL_NOEXCEPT {
    GblObject* pContext = NULL;
    GblObject* pIt = pSelf;
    while(pIt) {
        if(pIt->contextType) {
            pContext = pIt;
            break;
        }
        if(!pIt->pExtraData) break;
        pIt = pIt->pExtraData->pParent;
    }
    return (GblContext*)pContext;
}



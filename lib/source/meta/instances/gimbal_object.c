#include <gimbal/meta/instances/gimbal_object.h>
#include <gimbal/meta/types/gimbal_variant.h>
#include <gimbal/meta/properties/gimbal_property.h>
#include <gimbal/meta/instances/gimbal_context.h>
#include <gimbal/meta/instances/gimbal_event.h>
#include <gimbal/strings/gimbal_string_ref.h>
#include <gimbal/containers/gimbal_linked_list.h>
#include <gimbal/strings/gimbal_string_buffer.h>
#include <gimbal/containers/gimbal_nary_tree.h>
#include "../types/gimbal_type_.h"

#define GBL_OBJECT_EVENT_FILTER_VECTOR_SIZE_        (sizeof(GblArrayList))

static GblQuark     objectNameQuark_            = GBL_QUARK_INVALID;
static GblQuark     objectRefCountQuark_        = GBL_QUARK_INVALID;
static GblQuark     objectParentQuark_          = GBL_QUARK_INVALID;
static GblQuark     objectFamilyQuark_          = GBL_QUARK_INVALID;
static GblQuark     objectEventFiltersQuark_    = GBL_QUARK_INVALID;

static GBL_RESULT GblObjectClass_ivariantIFace_construct_(GblVariant* pVariant, GblSize argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_UNUSED(argc);
    GBL_API_BEGIN(NULL);
    if(op == GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT) {
        pVariant->pVoid = GblInstance_create(pVariant->type);
    } else if(op == GBL_IVARIANT_OP_FLAG_CONSTRUCT_COPY || op == GBL_IVARIANT_OP_FLAG_CONSTRUCT_VALUE_COPY) {
        pVariant->pVoid = pArgs[0].pVoid? GblBox_ref(GBL_BOX(pArgs[0].pVoid)) : NULL;
    } else {
        pVariant->pVoid = pArgs[0].pVoid;
     }
    GBL_API_END();
}

static GBL_RESULT GblObjectClass_ivariantIFace_destruct_(GblVariant* pVariant) {
    GBL_API_BEGIN(NULL);
    GblBox_unref(pVariant->pVoid);
    GBL_API_END();
}

static GBL_RESULT GblObjectClass_ivariantIFace_compare_(const GblVariant* pVariant, const GblVariant* pOther, GblInt* pResult) {
    GBL_API_BEGIN(NULL);
    *pResult = (pVariant->pVoid == pOther->pVoid);
    GBL_API_END();
}

static GBL_RESULT GblObjectClass_ivariantIFace_get_(GblVariant* pSelf, GblSize argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_UNUSED(argc);
    GBL_API_BEGIN(NULL);
    *(GblObject**)pArgs[0].pVoid = pSelf->pVoid;
    if(op == GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY) {
        GblBox_ref(GBL_BOX(pSelf->pVoid));
    } else if(op == GBL_IVARIANT_OP_FLAG_GET_VALUE_MOVE) {
        pSelf->type = GBL_NIL_TYPE;
    }
    GBL_API_END();
}


static GBL_RESULT GblObjectClass_ivariantIFace_set_(GblVariant* pSelf, GblSize argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_UNUSED(argc);
    GBL_API_BEGIN(NULL);
    pSelf->pVoid = pArgs[0].pVoid;
    if(op == GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY || op == GBL_IVARIANT_OP_FLAG_SET_COPY) {
        GblBox_ref(GBL_BOX(pSelf->pVoid));
    } else if(op == GBL_IVARIANT_OP_FLAG_SET_MOVE || op == GBL_IVARIANT_OP_FLAG_SET_VALUE_MOVE) {
        pArgs[0].type = GBL_NIL_TYPE;
    }
    GBL_API_END();
}

static GBL_RESULT GblObjectClass_ivariantIFace_load_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_UNUSED(pVariant && pString);
    return GBL_RESULT_UNIMPLEMENTED;
}

static GBL_RESULT GblObjectClass_ivariantIFace_save_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GblVariant variant = GBL_VARIANT_INIT;
    GblBool first = GBL_TRUE;
    GBL_API_BEGIN(NULL);
    GblStringBuffer_append(pString, GBL_STRING_VIEW("{\n"));

    for(const GblProperty* pIt = GblProperty_next(GblVariant_typeOf(pVariant), NULL, GBL_PROPERTY_FLAG_SAVE);
        pIt != NULL;
        pIt = GblProperty_next(GblVariant_typeOf(pVariant), pIt, GBL_PROPERTY_FLAG_SAVE))
    {
        if(!first) GblStringBuffer_append(pString, GBL_STRING_VIEW(",\n"));
        const char* pKey = GblProperty_nameString(pIt);
        GblStringBuffer_append(pString, GBL_STRING_VIEW("\t"));
        GblStringBuffer_append(pString, GBL_STRING_VIEW(pKey));
        GblStringBuffer_append(pString, GBL_STRING_VIEW(" = "));
        GblObject_propertyVariant(pVariant->pVoid, pKey, &variant);
        GblVariant_save(&variant, pString);
        first = GBL_FALSE;

    }
    GblStringBuffer_append(pString, GBL_STRING_VIEW("}\n"));
    GBL_API_END();
}

// ========== ITABLE IFACE ==========

static GBL_RESULT GblObjectClass_iTableIFace_index_(const GblITable* pITable, const GblVariant* pKey, GblVariant* pValue) {
    GBL_API_BEGIN(NULL);
    GBL_API_RESULT() = GBL_RESULT_ERROR_INVALID_PROPERTY;
    GblObject* pObject = GBL_OBJECT(pITable);
    if(GblVariant_typeOf(pKey) == GBL_STRING_TYPE) {
        const char* pString = NULL;
        GBL_API_CALL(GblVariant_getValuePeek(pKey, &pString));
        GBL_API_RESULT() = GblObject_propertyVariant(pObject, pString, pValue);
        if(!GBL_RESULT_SUCCESS(GBL_API_RESULT()) && GBL_PRIV_REF(GBL_BOX(pObject)).derivedFlags & 0x1) {
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
    if(GblVariant_typeOf(pKey) == GBL_STRING_TYPE) {
        const char* pString = NULL;
        GBL_API_CALL(GblVariant_getValuePeek(pKey, &pString));
        GBL_API_RESULT() = GblObject_setPropertyVariant(pObject, pString, (GblVariant*)pValue);
        if(!GBL_RESULT_SUCCESS(GBL_API_RESULT()) && GBL_PRIV_REF(GBL_BOX(pObject)).derivedFlags & 0x1) {
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
    if(!pKey || GblVariant_typeOf(pKey) == GBL_NIL_TYPE) {
        const GblProperty* pProp = GblProperty_next(GBL_INSTANCE_TYPEOF(pObject), NULL, GBL_PROPERTY_FLAG_READ);
        if(pProp) {
            GBL_API_CALL(GblVariant_setValueCopy(pNextKey, GBL_STRING_TYPE, GblProperty_nameString(pProp)));
            GBL_API_CALL(GblObject_propertyVariant(pObject, GblProperty_nameString(pProp), pNextValue));
            GBL_API_RESULT() = GBL_RESULT_SUCCESS;
        }
    } else if(GblVariant_typeOf(pKey) == GBL_STRING_TYPE) {
        const char* pString = NULL;
        GBL_API_CALL(GblVariant_getValuePeek(pKey, &pString));
        const GblProperty* pProp = GblProperty_find(GBL_INSTANCE_TYPEOF(pObject), pString);
        if(pProp) {
            pProp = GblProperty_next(GBL_INSTANCE_TYPEOF(pObject), pProp, GBL_PROPERTY_FLAG_READ);
            if(pProp) {
                GBL_API_CALL(GblVariant_setValueCopy(pNextKey, GBL_STRING_TYPE, GblProperty_nameString(pProp)));
                GBL_API_CALL(GblObject_propertyVariant(pObject, GblProperty_nameString(pProp), pNextValue));
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


GBL_API GblObject_sendEvent(GblObject* pSelf, GblEvent* pEvent) {
    GBL_API_BEGIN(NULL);
    GblObject* pParent = GblObject_parent(pSelf);
    if(pParent) {
        GBL_API_CALL(GblObject_notifyEvent(pParent, pEvent));
    }
    GBL_API_END();
}


GBL_API GblObject_notifyEvent(GblObject* pSelf, GblEvent* pEvent) {
    GBL_API_BEGIN(NULL);
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
    GBL_API_CALL(GblObject_sendEvent(pSelf, pEvent));

    GBL_API_END();
}

static GBL_RESULT GblObject_eventFiltersDestruct_(const GblArrayMap* pMap, uintptr_t key, void* pData) {
    GBL_UNUSED(key, pMap);
    GBL_API_BEGIN(NULL);
    GBL_API_CALL(GblArrayList_destruct(pData));
    GBL_API_FREE(pData);
    GBL_API_END();
}

static GblArrayList* GblObject_eventFilters_(const GblObject* pSelf) {
    return (GblArrayList*)GblArrayMap_getValue(&GBL_PRIV(pSelf->base).pFields, objectEventFiltersQuark_);
}

static GblArrayList* GblObject_ensureEventFilters_(GblObject* pSelf) {
    GBL_API_BEGIN(NULL);
    GblArrayList* pEventFilters = GblObject_eventFilters_(pSelf);
    if(!pEventFilters) {
        pEventFilters = GBL_API_MALLOC(GBL_OBJECT_EVENT_FILTER_VECTOR_SIZE_);
        GBL_API_CALL(GblArrayList_construct(pEventFilters,
                                        sizeof(GblIEventFilter*),
                                        0,
                                        NULL,
                                        GBL_OBJECT_EVENT_FILTER_VECTOR_SIZE_));
        GBL_API_CALL(GblArrayMap_setUserdata(&GBL_PRIV(pSelf->base).pFields,
                                             objectEventFiltersQuark_,
                                             (uintptr_t)pEventFilters,
                                             GblObject_eventFiltersDestruct_));
        pEventFilters = GblObject_eventFilters_(pSelf);
    }
    GBL_API_END_BLOCK();
    return pEventFilters;
}

GBL_API GblObject_installEventFilter(GblObject* pSelf, GblIEventFilter* pFilter) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pFilter);
    GblArrayList* pFilters = GblObject_ensureEventFilters_(pSelf);
    GBL_API_CALL(GblArrayList_pushBack(pFilters, &pFilter));
    GBL_API_END();
}

GBL_API GblObject_uninstallEventFilter(GblObject* pSelf, GblIEventFilter* pFilter) {
    GblArrayList* pVector = NULL;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pFilter);
    pVector = GblObject_eventFilters_(pSelf);
    GBL_API_VERIFY_POINTER(pVector);
    GblSize count = GblArrayList_size(pVector);
    GBL_API_VERIFY(count, GBL_RESULT_ERROR_INVALID_HANDLE,
                   "Object has no event filter list to remove filter from: %x", pFilter);
    GblBool found = GBL_FALSE;
    for(GblSize f = 0; f < count; ++f) {
        GblIEventFilter** ppFilter = GblArrayList_at(pVector, f);
        if(*ppFilter == pFilter) {
            found = GBL_TRUE;
            GBL_API_CALL(GblArrayList_erase(pVector, f, 1));
            break;
        }
    }
    GBL_API_VERIFY(found, GBL_RESULT_ERROR_INVALID_HANDLE,
                   "EventFilter wasn't found on Object: %x", pFilter);
    GBL_API_END();
}

GBL_EXPORT GblIEventFilter* GblObject_eventFilterAt(const GblObject* pSelf, GblSize index) {
    GblIEventFilter** ppFilter = NULL;
    GBL_API_BEGIN(NULL);
    GblArrayList* pFilters = GblObject_eventFilters_(pSelf);
    if(pFilters)
        ppFilter = GblArrayList_at(pFilters, index);
    GBL_API_END_BLOCK();
    return ppFilter? *ppFilter : NULL;
}

GBL_EXPORT GblSize GblObject_eventFilterCount(const GblObject* pSelf) {
    GblSize count = 0;
    GBL_API_BEGIN(NULL);
    GblArrayList* pFilters = GblObject_eventFilters_(pSelf);
    if(pFilters) {
        count = GblArrayList_size(pFilters);
    }
    GBL_API_END_BLOCK();
    return count;
}

// ======== OBJECT CLASS ==========

static GBL_RESULT GblObjectClass_constructor_(GblObject* pSelf) {
    GBL_API_BEGIN(pSelf);
    GBL_API_END();
}

static GBL_RESULT GblObjectClass_destructor_(GblBox* pRecord) {
    GBL_API_BEGIN(NULL);
    GblObject* pSelf = (GblObject*)pRecord;
    GblObject_setParent(pSelf, NULL);
    for(GblObject* pIt = GblObject_childFirst(pSelf);
        pIt != NULL;
        pIt = GblObject_siblingNext(pSelf))
    {
       GblObject_setParent(pIt, NULL);
    }

    GblBoxClass* pRecordClass = GBL_BOX_CLASS(GblClass_weakRefDefault(GBL_BOX_TYPE));
    GBL_API_VERIFY_CALL(pRecordClass->pFnDestructor(pRecord));
    GBL_API_END();
}

static GBL_RESULT GblObjectClass_property_(const GblObject* pSelf, const GblProperty* pProp, GblVariant* pValue)  {
    GBL_API_BEGIN(NULL);
    switch(pProp->id) {
    case GblObject_Property_Id_name:      GblVariant_setValueCopy(pValue, pProp->valueType, GblObject_name(pSelf));           break;
    case GblObject_Property_Id_parent:    GblVariant_setValueMove(pValue, pProp->valueType, GblObject_parent(pSelf));         break;
    case GblObject_Property_Id_refCount:  GblVariant_setValueCopy(pValue, pProp->valueType, GblBox_refCount(GBL_BOX(pSelf))); break;
    case GblObject_Property_Id_userdata:  GblVariant_setValueCopy(pValue, pProp->valueType, GblBox_userdata(GBL_BOX(pSelf))); break;
    default: GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY, "Reading unhandled property: %s", GblProperty_nameString(pProp));
    }
    GBL_API_END();
}

static GBL_RESULT GblObjectClass_setProperty_(GblObject* pSelf, const GblProperty* pProp, GblVariant* pValue) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(pProp);
    switch(pProp->id) {
    case GblObject_Property_Id_name: {
        const char* pName = NULL;
        GBL_API_CALL(GblVariant_getValueCopy(pValue, &pName));
        GblObject_setName(pSelf, pName);
        break;
    }
    case GblObject_Property_Id_parent: {
        GblObject* pParent = NULL;
        GBL_API_CALL(GblVariant_getValuePeek(pValue, &pParent));
        GblObject_setParent(pSelf, pParent);
        break;
    }
    case GblObject_Property_Id_userdata: {
        void* pUserdata = NULL;
        GBL_API_CALL(GblVariant_getValueCopy(pValue, &pUserdata));
        GblBox_setUserdata(GBL_BOX(pSelf), pUserdata);
        break;
    }
    default: GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY,
                                "Writing unhandled property: %s",
                                GblProperty_nameString(pProp));
    }
    GBL_API_END();
}

static GBL_RESULT GblObjectClass_constructed_(GblObject* pSelf) {
    GBL_UNUSED(pSelf);
    return GBL_RESULT_SUCCESS;
}


static GBL_RESULT GblObjectClass_init_(GblObjectClass* pClass, void* pData, GblContext* pCtx) {
    GBL_UNUSED(pData);
    GBL_API_BEGIN(pCtx);

    // static constructor for first instance of class
    if(!GblType_classRefCount(GBL_OBJECT_TYPE)) {
        objectNameQuark_            = GblQuark_fromStringStatic("_name");
        objectRefCountQuark_        = GblQuark_fromStringStatic("_refCount");
        objectParentQuark_          = GblQuark_fromStringStatic("_parent");
        objectFamilyQuark_          = GblQuark_fromStringStatic("_family");
        objectEventFiltersQuark_    = GblQuark_fromStringStatic("_eventFilters");

        GBL_PROPERTIES_REGISTER(GblObject);
    }

    static const GblIVariantClassVTable iVariantVTable = {
        .supportedOps = GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT      |
                        GBL_IVARIANT_OP_FLAG_CONSTRUCT_COPY         |
                        GBL_IVARIANT_OP_FLAG_CONSTRUCT_MOVE         |
                        GBL_IVARIANT_OP_FLAG_CONSTRUCT_VALUE_COPY   |
                        GBL_IVARIANT_OP_FLAG_CONSTRUCT_VALUE_MOVE   |
                        GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY         |
                        GBL_IVARIANT_OP_FLAG_SET_COPY               |
                        GBL_IVARIANT_OP_FLAG_SET_VALUE_MOVE         |
                        GBL_IVARIANT_OP_FLAG_SET_MOVE               |
                        GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY         |
                        GBL_IVARIANT_OP_FLAG_GET_VALUE_MOVE         |
                        GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK,
        .pGetValueFmt = "p",
        .pSetValueFmt = "p",
        .pFnConstruct =    GblObjectClass_ivariantIFace_construct_,
        .pFnDestruct  =    GblObjectClass_ivariantIFace_destruct_,
        .pFnCompare   =    GblObjectClass_ivariantIFace_compare_,
        .pFnGet       =    GblObjectClass_ivariantIFace_get_,
        .pFnSet       =    GblObjectClass_ivariantIFace_set_,
        .pFnLoad      =    GblObjectClass_ivariantIFace_load_,
        .pFnSave      =    GblObjectClass_ivariantIFace_save_,
    };

    pClass->base.GblIVariantImpl.pVTable = &iVariantVTable;

    pClass->GblITableImpl.pFnIndex       =    GblObjectClass_iTableIFace_index_;
    pClass->GblITableImpl.pFnNewIndex    =    GblObjectClass_iTableIFace_newIndex_;
    pClass->GblITableImpl.pFnNextIndex   =    GblObjectClass_iTableIFace_nextIndex_;

    pClass->GblIEventHandlerImpl.pFnEvent = GblObjectClass_iEventHandlerIFace_event_;

    pClass->pFnConstructor      = GblObjectClass_constructor_;
    pClass->pFnConstructed      = GblObjectClass_constructed_;
    pClass->base.pFnDestructor  = GblObjectClass_destructor_;
    pClass->pFnProperty         = GblObjectClass_property_;
    pClass->pFnSetProperty      = GblObjectClass_setProperty_;

    GBL_API_END();
}

static GBL_RESULT GblObjectClass_final_(GblObjectClass* pClass, void* pClassData, GblContext* pCtx) {
    GBL_UNUSED(pClassData);
    GBL_API_BEGIN(pCtx);

    if(!GblType_classRefCount(GBL_CLASS_TYPEOF(pClass))) {
        const GblBool result = GblProperty_uninstallAll(GBL_CLASS_TYPEOF(pClass));
        GBL_API_VERIFY_EXPRESSION(result);
    }

    GBL_API_END();
}

static GBL_RESULT GblObject_init_(GblInstance* pInstance, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx); {
        GblObjectClass* pClass = GBL_OBJECT_GET_CLASS(pInstance);
        GBL_API_CALL(pClass->pFnConstructor(GBL_OBJECT(pInstance)));
    } GBL_API_END();
}

GBL_INLINE GBL_RESULT GblObject_property_(const GblObject* pSelf,
                                          const GblProperty* pProp,
                                          GblVariant* pValue)
{
    const GblObjectClass* pClass =
            GBL_OBJECT_CLASS(GblClass_weakRefDefault(GblProperty_objectType(pProp)));

    return pClass->pFnProperty(pSelf, pProp, pValue);
}

GBL_EXPORT GBL_RESULT GblObject_propertyVariantByQuark(const GblObject* pSelf,
                                                       GblQuark name,
                                                       GblVariant* pValue)
{
    const GblProperty* pProp = GblProperty_findQuark(GBL_INSTANCE_TYPEOF(pSelf),
                                                     name);
    if(!pProp) GBL_UNLIKELY {
        GBL_API_BEGIN(NULL);
        GBL_API_VERIFY(GBL_FALSE,
                       GBL_RESULT_ERROR_INVALID_PROPERTY,
                       "[GblObject] Attempt to get unknown property %s on object type %s",
                       GblQuark_toString(name), GblType_name(GBL_INSTANCE_TYPEOF(pSelf)));
        GBL_API_END();
    }

    return GblObject_property_(pSelf, pProp, pValue);
}

GBL_EXPORT GBL_RESULT GblObject_propertyVariant(const GblObject* pSelf,
                                                const char* pName,
                                                GblVariant* pValue)
{
    const GblQuark name = GblQuark_tryString(pName);

    if(name == GBL_QUARK_INVALID) GBL_UNLIKELY {
        GBL_API_BEGIN(NULL);
        GBL_API_VERIFY(GBL_FALSE,
                       GBL_RESULT_ERROR_INVALID_PROPERTY,
                       "[GblObject] Attempt to get unknown property %s on object type %s",
                       pName, GblType_name(GBL_INSTANCE_TYPEOF(pSelf)));
        GBL_API_END();
    }

    return GblObject_propertyVariantByQuark(pSelf, name, pValue);
}

static GBL_RESULT GblObject_setProperty_(GblObject*         pSelf,
                                         const GblProperty* pProp,
                                         GblVariant*        pValue)
{
    GBL_API_BEGIN(NULL);

    if((pProp->flags & GBL_PROPERTY_FLAG_WRITE)) {

        if(!GblType_check(GblVariant_typeOf(pValue), pProp->valueType)) {
            GblVariant v;
            GblVariant_constructDefault(&v, pProp->valueType);
            GBL_API_VERIFY_CALL(GblVariant_convert(pValue, &v));
            GblVariant_setMove(pValue, &v);
            GblVariant_destruct(&v);
        }

        if(!GblProperty_checkValue(pProp, pValue)) {
            GBL_API_VERIFY_CALL(GblProperty_validateValue(pProp, pValue));
        }

        const GblObjectClass* pClass =
                GBL_OBJECT_CLASS(GblClass_weakRefDefault(GblProperty_objectType(pProp)));

        GBL_API_VERIFY_CALL(pClass->pFnSetProperty(pSelf, pProp, pValue));

    } else {
        GBL_API_RECORD_SET(GBL_RESULT_UNSUPPORTED,
                           "[GblObject] Attempt to set NON-WRITE property %s[%s].",
                           GblType_name(GBL_INSTANCE_TYPEOF(pSelf)),
                           GblProperty_nameString(pProp));
    }

    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblObject_setPropertyVariantByQuark(GblObject*  pSelf,
                                                          GblQuark    name,
                                                          GblVariant* pValue)
{

    const GblProperty* pProp = GblProperty_findQuark(GBL_INSTANCE_TYPEOF(pSelf), name);

    if(!pProp) GBL_UNLIKELY {
        GBL_API_BEGIN(NULL);
        GBL_API_VERIFY(GBL_FALSE,
                       GBL_RESULT_ERROR_INVALID_PROPERTY,
                       "[GblObject] Attempt to set unknown property %s on object type %s",
                       GblQuark_toString(name), GblType_name(GBL_INSTANCE_TYPEOF(pSelf)));
        GBL_API_END();
    }

    return GblObject_setProperty_(pSelf, pProp, pValue);
}

GBL_EXPORT GBL_RESULT GblObject_setPropertyVariant(GblObject* pSelf,
                                                   const char* pName,
                                                   GblVariant* pValue)
{
    const GblQuark name = GblQuark_tryString(pName);

    if(name == GBL_QUARK_INVALID) GBL_UNLIKELY {
        GBL_API_BEGIN(NULL);
        GBL_API_VERIFY(GBL_FALSE,
                       GBL_RESULT_ERROR_INVALID_PROPERTY,
                       "[GblObject] Attempt to set unknown property %s on object type %s",
                       pName, GblType_name(GBL_INSTANCE_TYPEOF(pSelf)));
        GBL_API_END();
    }

    return GblObject_setPropertyVariantByQuark(pSelf, name, pValue);
}

GBL_EXPORT GBL_RESULT GblObject_properties(const GblObject* pSelf, ...) {
    va_list varArgs;
    va_start(varArgs, pSelf);
    GBL_API_BEGIN(NULL);
    GBL_API_CALL(GblObject_propertiesVaList(pSelf, &varArgs));
    GBL_API_END_BLOCK();
    va_end(varArgs);
    return GBL_API_RESULT();
}

GBL_EXPORT GBL_RESULT GblObject_setProperties(GblObject* pSelf, ...) {
    va_list varArgs;
    va_start(varArgs, pSelf);
    GBL_API_BEGIN(NULL);
    GBL_API_CALL(GblObject_setPropertiesVaList(pSelf, &varArgs));
    GBL_API_END_BLOCK();
    va_end(varArgs);
    return GBL_API_RESULT();
}

GBL_EXPORT GBL_RESULT GblObject_propertyVaList_(const GblObject* pSelf, const GblProperty* pProp, va_list* pList) {
    GBL_API_BEGIN(NULL);
    GblVariant variant;
    GBL_API_VERIFY((pProp->flags & GBL_PROPERTY_FLAG_READ),
                   GBL_RESULT_ERROR_INVALID_PROPERTY,
                   "[GblObject] Tried to get unreadable property: [%s]",
                   GblProperty_nameString(pProp));
    GBL_API_CALL(GblVariant_constructDefault(&variant, GBL_NIL_TYPE));
    GBL_API_CALL(GblObject_property_(pSelf, pProp, &variant));
    GBL_API_CALL(GblVariant_getValueCopyVaList(&variant, pList));
    GBL_API_CALL(GblVariant_destruct(&variant));
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblObject_propertyVaList(const GblObject* pSelf, const char* pName, va_list* pList) {
    GBL_API_BEGIN(NULL);
    const GblProperty* pProp = GblProperty_find(GBL_INSTANCE_TYPEOF(pSelf), pName);
    GBL_API_VERIFY(pProp,
                   GBL_RESULT_ERROR_INVALID_PROPERTY,
                   "[GblObject] Tried to get unknown property: %s[%s]",
                   GblType_name(GBL_INSTANCE_TYPEOF(pSelf)),
                   pName);
    GBL_API_CALL(GblObject_propertyVaList_(pSelf, pProp, pList));
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblObject_propertyVaListByQuark(const GblObject* pSelf, GblQuark name, va_list* pList) {
    GBL_API_BEGIN(NULL);
    const GblProperty* pProp = GblProperty_findQuark(GBL_INSTANCE_TYPEOF(pSelf), name);
    GBL_API_VERIFY(pProp,
                   GBL_RESULT_ERROR_INVALID_PROPERTY,
                   "[GblObject] Tried to get unknown property: %s[%s]",
                   GblType_name(GBL_INSTANCE_TYPEOF(pSelf)),
                   GblQuark_toString(name));
    GBL_API_CALL(GblObject_propertyVaList_(pSelf, pProp, pList));
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblObject_property(const GblObject* pSelf, const char* pName, ...) {
    va_list varArgs;
    va_start(varArgs, pName);
    const GBL_RESULT result = GblObject_propertyVaList(pSelf, pName, &varArgs);
    va_end(varArgs);
    return result;
}

GBL_EXPORT GBL_RESULT GblObject_propertyByQuark(const GblObject* pSelf, GblQuark name, ...) {
    va_list varArgs;
    va_start(varArgs, name);
    const GBL_RESULT result = GblObject_propertyVaListByQuark(pSelf, name, &varArgs);
    va_end(varArgs);
    return result;
}

GBL_EXPORT GBL_RESULT GblObject_setPropertyVaList_(GblObject* pSelf, const GblProperty* pProp, va_list* pList) {
    GBL_API_BEGIN(NULL);
    GblVariant variant;
    GBL_API_VERIFY(pProp->flags & GBL_PROPERTY_FLAG_WRITE,
                   GBL_RESULT_ERROR_INVALID_PROPERTY,
                   "[GblObject] Tried to get unwritable property: [%s]", GblProperty_nameString(pProp));
    GBL_API_CALL(GblVariant_constructValueCopyVaList(&variant, pProp->valueType, pList));
    GBL_API_CALL(GblObject_setProperty_(pSelf, pProp, &variant));
    GBL_API_CALL(GblVariant_destruct(&variant));
    GBL_API_END_BLOCK();
    return GBL_API_RESULT();
}


GBL_EXPORT GBL_RESULT GblObject_setPropertyVaList(GblObject* pSelf, const char* pName, va_list* pList) {
    GBL_API_BEGIN(NULL);
    const GblProperty* pProp = GblProperty_find(GBL_INSTANCE_TYPEOF(pSelf), pName);
    GBL_API_VERIFY(pProp->flags & GBL_PROPERTY_FLAG_WRITE,
                   GBL_RESULT_ERROR_INVALID_PROPERTY,
                   "[GblObject] Tried to get unwritable property: [%s]", pName);
    GBL_API_VERIFY_CALL(GblObject_setPropertyVaList_(pSelf, pProp, pList));
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblObject_setPropertyVaListByQuark(GblObject* pSelf, GblQuark name, va_list* pList) {
    GBL_API_BEGIN(NULL);
    const GblProperty* pProp = GblProperty_findQuark(GBL_INSTANCE_TYPEOF(pSelf), name);
    GBL_API_VERIFY(pProp->flags & GBL_PROPERTY_FLAG_WRITE,
                   GBL_RESULT_ERROR_INVALID_PROPERTY,
                   "[GblObject] Tried to get unwritable property: [%s]", GblQuark_toString(name));
    GBL_API_VERIFY_CALL(GblObject_setPropertyVaList_(pSelf, pProp, pList));
    GBL_API_END();
}


GBL_EXPORT GBL_RESULT GblObject_setProperty(GblObject* pSelf, const char* pName, ...) {
    va_list varArgs;
    va_start(varArgs, pName);
    const GBL_RESULT result = GblObject_setPropertyVaList(pSelf, pName, &varArgs);
    va_end(varArgs);
    return result;
}

GBL_EXPORT GBL_RESULT GblObject_setPropertyByQuark(GblObject* pSelf, GblQuark name, ...) {
    va_list varArgs;
    va_start(varArgs, name);
    const GBL_RESULT result = GblObject_setPropertyVaListByQuark(pSelf, name, &varArgs);
    va_end(varArgs);
    return result;
}

GBL_EXPORT GBL_RESULT GblObject_propertiesVaList(const GblObject* pSelf, va_list* pVarArgs) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_POINTER(pVarArgs);

    const char* pKey = NULL;
    while((pKey = va_arg(*pVarArgs, const char*))) {


        const GblProperty* pProp = GblProperty_find(GBL_INSTANCE_TYPEOF(pSelf), pKey);

        if(pProp) {
            GblVariant variant;
            GBL_API_CALL(GblVariant_constructDefault(&variant, GBL_NIL_TYPE));
            GBL_API_CALL(GblObject_property_(pSelf, pProp, &variant));
            GBL_API_CALL(GblVariant_getValueCopyVaList(&variant, pVarArgs));
            if(pProp->flags & GBL_PROPERTY_FLAG_READ) {
                ;
            } else {
                GBL_API_WARN("[GblObject]: Failed to get non-readable property: %s[%s]",
                             GblType_name(GBL_INSTANCE_TYPEOF(pSelf)), pKey);
            }
            GBL_API_CALL(GblVariant_destruct(&variant));
        } else {
            GBL_API_WARN("[GblObject]: Failed to get unknown property: %s[%s]",
                         GblType_name(GBL_INSTANCE_TYPEOF(pSelf)), pKey);
        }
    }
    GBL_API_END();
}

GBL_API GblObject_setPropertiesVaList(GblObject* pSelf, va_list* pVarArgs) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_POINTER(pVarArgs);

    GblSize count = GblProperty_count(GBL_INSTANCE_TYPEOF(pSelf));
    GblVariant*  pVariants  = GBL_ALLOCA(sizeof(GblVariant) * count);
    const char** pKeys      = GBL_ALLOCA(sizeof(const char*) * count);

    GblSize p = 0;
    const char* pKey = NULL;
    while((pKey = va_arg(*pVarArgs, const char*))) {
        pKeys[p] = pKey;
        const GblProperty* pProp = GblProperty_find(GBL_INSTANCE_TYPEOF(pSelf), pKey);
        if(pProp) {
            GBL_API_CALL(GblVariant_constructValueCopyVaList(&pVariants[p], pProp->valueType, pVarArgs));
            if(pProp->flags & GBL_PROPERTY_FLAG_WRITE) {
                ++p;
            } else {
                GBL_API_WARN("[GblObject]: Failed to set non-writable property: %s[%s]",
                             GblType_name(GBL_INSTANCE_TYPEOF(pSelf)), pKey);
            }
        } else {
            GBL_API_WARN("[GblObject]: Failed to set unknown property: %s[%s]",
                         GblType_name(GBL_INSTANCE_TYPEOF(pSelf)), pKey);
        }
    }

    GBL_API_CALL(GblObject_setPropertiesVariants(pSelf, p, pKeys, pVariants));

    for(GblSize v = 0; v < p; ++v) {
       GBL_API_CALL(GblVariant_destruct(&pVariants[v]));
    }

    GBL_API_END();
}

GBL_API GblObject_propertiesVariant(const GblObject* pSelf, GblSize propertyCount, const char* pNames[], GblVariant* pValues) {
    GBL_API_BEGIN(NULL);
    for(GblSize p = 0; p < propertyCount; ++p) {
        GBL_API_CALL(GblObject_propertyVariant(pSelf, pNames[p], &pValues[p]));
    }
    GBL_API_END();
}

GBL_API GblObject_setPropertiesVariants(GblObject* pSelf, GblSize propertyCount, const char* pNames[], GblVariant* pValues) {
    GBL_API_BEGIN(NULL);
    for(GblSize p = 0; p < propertyCount; ++p) {
        GBL_API_CALL(GblObject_setPropertyVariant(pSelf, pNames[p], &pValues[p]));
    }
    GBL_API_END();
}

static GBL_RESULT GblObject_construct_(GblObject*         pSelf,
                                       GblSize            argCount,
                                       const GblProperty* pProperties[],
                                       GblVariant*        pValues)
{
    GBL_API_BEGIN(NULL);

    const GblType selfType  = GBL_INSTANCE_TYPEOF(pSelf);
    const GblSize depth     = GblType_depth(selfType);

    // Iterate bases from base -> derived type
    for(GblSize d = 0; d <= depth; ++d) {
        const GblType      curType  = GblType_base(selfType, depth);
        const GblProperty* pCurProp = NULL;

        // Iterate over each required CONSTRUCT-flagged property
        while((pCurProp = GblProperty_next(curType, pCurProp, GBL_PROPERTY_FLAG_CONSTRUCT))) {
            GblBool found = GBL_FALSE;

            // Check whether we were passed a value for the property
            for(GblSize v = 0; v < argCount; ++v) {

                 // Set the required property to the given value
                if(pProperties[v] == pCurProp) {
                    GBL_API_VERIFY_CALL(GblObject_setProperty_(pSelf, pCurProp, &pValues[v]));
                    found = GBL_TRUE;
                    break;
                }
            }

            // Bail if required property wasn't found
            GBL_API_VERIFY(found,
                           GBL_RESULT_ERROR_INVALID_PROPERTY,
                           "[GblObject] Cannot instantiate %s without required CONSTRUCT property %s[%s]",
                           GblType_name(selfType),
                           GblType_name(curType),
                           GblProperty_nameString(pCurProp));
        }
    }

    // Notify post-constructor that we constructed the object with the required properties
    GBL_API_VERIFY_CALL(GBL_OBJECT_GET_CLASS(pSelf)->pFnConstructed(pSelf));

    // Iterate over the remaining properties, doing regular writes.
    for(GblSize v = 0; v < argCount; ++v) {
        if(pProperties[v] && !(pProperties[v]->flags & GBL_PROPERTY_FLAG_CONSTRUCT)) {
            GBL_API_CALL(GblObject_setProperty_(pSelf, pProperties[v], &pValues[v]));
        }
    }

    GBL_API_END();
}


GBL_EXPORT GblObject* GblObject_create(GblType type, ...) {
    GblObject* pObject = NULL;
    va_list    varArgs;
    va_start(varArgs, type);
    GBL_API_BEGIN(NULL);
    pObject = GblObject_createVaList(type, &varArgs);
    GBL_API_END_BLOCK();
    va_end(varArgs);
    return pObject;
}

GBL_EXPORT GblObject* GblObject_createWithClass(GblObjectClass* pClass, ...)
{
    GblObject* pObject = NULL;
    va_list    varArgs;
    va_start(varArgs, pClass);
    GBL_API_BEGIN(NULL);
    pObject = GblObject_createVaListWithClass(pClass, &varArgs);
    GBL_API_END_BLOCK();
    va_end(varArgs);
    return pObject;

}

GBL_EXPORT GBL_RESULT GblObject_construct(GblObject* pSelf, GblType type, ...) {
    va_list varArgs;
    va_start(varArgs, type);
    GBL_API_BEGIN(NULL);
    GBL_API_CALL(GblObject_constructVaList(pSelf, type, &varArgs));
    GBL_API_END_BLOCK();
    va_end(varArgs);
    return GBL_API_RESULT();
}

GBL_EXPORT GBL_RESULT GblObject_constructWithClass(GblObject* pSelf, GblObjectClass* pClass, ...) {
    va_list    varArgs;
    va_start(varArgs, pClass);
    GBL_API_BEGIN(NULL);
    GBL_API_CALL(GblObject_constructVaListWithClass(pSelf, pClass, &varArgs));
    GBL_API_END_BLOCK();
    va_end(varArgs);
    return GBL_API_RESULT();
}

static  GBL_RESULT GblObject_constructVaList_(GblObject* pSelf, GblType type, va_list* pVarArgs) {
    GblSize count = 0;

    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_TYPE(type, GBL_OBJECT_TYPE);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_POINTER(pVarArgs);

    const GblType       selfType       = GBL_INSTANCE_TYPEOF(pSelf);
    const GblSize       totalCount     = GblProperty_count(GBL_INSTANCE_TYPEOF(pSelf));
    GblVariant*         pVariants      = GBL_ALLOCA(sizeof(GblVariant)   * totalCount);
    const GblProperty** ppProperties   = GBL_ALLOCA(sizeof(GblProperty*) * totalCount);


    // check whether there are even constructable properties or not
    const char* pKey = NULL;
    while((pKey = va_arg(*pVarArgs, const char*))) {
        if(!pKey) break;

        ppProperties[count] = GblProperty_find(selfType, pKey);

        if(!ppProperties[count]) {
            GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY,
                               "[GblObject] Failed to construct with unknown property: %s[%s]",
                               GblType_name(type), pKey);
        }

        GBL_API_VERIFY_CALL(GblVariant_constructValueCopyVaList(&pVariants[count],
                                                                ppProperties[count]->valueType,
                                                                pVarArgs));
        ++count;

    }

    GBL_API_VERIFY_CALL(GblObject_construct_(pSelf, count, ppProperties, pVariants));

    GBL_API_END_BLOCK();
    for(GblSize v = 0; v < count; ++v)
        GblVariant_destruct(&pVariants[v]);

    return GBL_API_RESULT();
}

GBL_EXPORT GblObject* GblObject_createVaList(GblType type, va_list* pVarArgs) {
    GblObject* pObject = NULL;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_TYPE(type, GBL_OBJECT_TYPE);
    pObject = GBL_OBJECT(GblBox_create(type));
    GBL_API_CALL(GblObject_constructVaList_(pObject, type, pVarArgs));
    GBL_API_END_BLOCK();
    return pObject;
}

GBL_EXPORT GblObject* GblObject_createVaListWithClass(GblObjectClass* pClass, va_list* pVarArgs) {
    GblObject* pObject = NULL;
    const GblType type = GBL_CLASS_TYPEOF(pClass);
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_TYPE(type, GBL_OBJECT_TYPE);
    pObject = GBL_OBJECT(GblBox_createWithClass(GBL_BOX_CLASS(pClass)));
    GBL_API_CALL(GblObject_constructVaList_(pObject, type, pVarArgs));
    GBL_API_END_BLOCK();
    return pObject;
}

GBL_EXPORT GBL_RESULT GblObject_constructVaList(GblObject* pSelf, GblType type, va_list* pVarArgs) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_TYPE(type, GBL_OBJECT_TYPE);
    GBL_API_CALL(GblBox_construct((GblBox*)pSelf, type));
    GBL_API_CALL(GblObject_constructVaList_(pSelf, type, pVarArgs));
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblObject_constructVaListWithClass(GblObject* pSelf, GblObjectClass* pClass, va_list* pVarArgs) {
    const GblType type = GBL_CLASS_TYPEOF(pClass);
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_TYPE(type, GBL_OBJECT_TYPE);
    GBL_API_CALL(GblBox_constructWithClass((GblBox*)pSelf, GBL_BOX_CLASS(pClass)));
    GBL_API_CALL(GblObject_constructVaList_(pSelf, type, pVarArgs));
    GBL_API_END();
}

static GBL_RESULT GblObject_constructVariants_(GblObject*  pSelf,
                                               GblType     type,
                                               GblSize     propertyCount,
                                               const char* pNames[],
                                               GblVariant* pValues)
{
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_TYPE(type, GBL_OBJECT_TYPE);
    GBL_API_VERIFY_POINTER(pSelf);

    const GblProperty** pProps = GBL_ALLOCA(sizeof(GblProperty*) * propertyCount);

    for(GblSize p = 0; p < propertyCount; ++p) {
        pProps[p] = GblProperty_find(GBL_INSTANCE_TYPEOF(pSelf), pNames[p]);

        if(!pProps[p]) {
            GBL_API_WARN("[GblObject]: Failed to construct with unknown property: %s[%s]",
                         GblType_name(type), pNames[p]);
        }
    }

    GBL_API_VERIFY_CALL(GblObject_construct_(pSelf, propertyCount, pProps, pValues));

    GBL_API_END();
}

GBL_EXPORT GblObject* GblObject_createVariants(GblType type, GblSize propertyCount, const char* pNames[], GblVariant* pValues) {
    GblObject* pObject = NULL;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_TYPE(type, GBL_OBJECT_TYPE);
    pObject = GBL_OBJECT(GblBox_create(type));
    GBL_API_CALL(GblObject_constructVariants_(pObject, type, propertyCount, pNames, pValues));
    GBL_API_END_BLOCK();
    return pObject;
}

GBL_EXPORT GblObject* GblObject_createVariantsWithClass(GblObjectClass* pClass,
                                                        GblSize         propertyCount,
                                                        const char*     pNames[],
                                                        GblVariant*     pValues)
{
    const GblType type = GBL_CLASS_TYPEOF(pClass);
    GblObject* pObject = NULL;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_TYPE(type, GBL_OBJECT_TYPE);
    pObject = GBL_OBJECT(GblBox_createWithClass(GBL_BOX_CLASS(pClass)));
    GBL_API_CALL(GblObject_constructVariants_(pObject, type, propertyCount, pNames, pValues));
    GBL_API_END_BLOCK();
    return pObject;
}

GBL_EXPORT GBL_RESULT GblObject_constructVariants(GblObject*  pSelf,
                                                  GblType     type,
                                                  GblSize     propertyCount,
                                                  const char* pNames[],
                                                  GblVariant* pValues)
{
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_TYPE(type, GBL_OBJECT_TYPE);
    GBL_API_CALL(GblBox_construct((GblBox*)pSelf, type));
    GBL_API_CALL(GblObject_constructVariants_(pSelf, type, propertyCount, pNames, pValues));
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblObject_constructVariantsWithClass(GblObject*      pSelf,
                                                           GblObjectClass* pClass,
                                                           GblSize         propertyCount,
                                                           const char*     pNames[],
                                                           GblVariant*     pValues)
{
    const GblType type = GBL_CLASS_TYPEOF(pClass);
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_TYPE(type, GBL_OBJECT_TYPE);
    GBL_API_CALL(GblBox_constructWithClass((GblBox*)pSelf, GBL_BOX_CLASS(pClass)));
    GBL_API_CALL(GblObject_constructVariants_(pSelf, type, propertyCount, pNames, pValues));
    GBL_API_END();
}


static GBL_RESULT GblObject_nameDestruct_(const GblArrayMap* pMap, uintptr_t key, void* pName) {
    GBL_UNUSED(key, pMap);
    GblStringRef_release(pName);
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT void GblObject_setName(GblObject* pSelf, const char* pName) {

    GblArrayMap_setUserdata(&GBL_PRIV(pSelf->base).pFields,
                            objectNameQuark_,
                            (uintptr_t)GblStringRef_create(pName),
                            GblObject_nameDestruct_);
}

GBL_EXPORT const char* GblObject_name(const GblObject* pSelf) {
    return (const char*)GblArrayMap_getValue(&GBL_PRIV(pSelf->base).pFields,
                                             objectNameQuark_);
}

#define GBL_OBJECT_FAMILY_ENTRY_(node) (!node? GBL_NULL : GBL_NARY_TREE_ENTRY(node, GblObjectFamily_, treeNode)->pSelf)

typedef struct GblObjectFamily_ {
    GblNaryTreeNode     treeNode;
    GblObject*          pSelf;
} GblObjectFamily_;

static GblObjectFamily_* GblObject_family_(const GblObject* pSelf) {
    return (GblObjectFamily_*)GblArrayMap_getValue(&GBL_PRIV(pSelf->base).pFields,
                                                   objectFamilyQuark_);
}

static GBL_RESULT GblObject_family_destruct_(const GblArrayMap* pMap, uintptr_t key, void* pValue) {
    GBL_UNUSED(key, pMap);
    GBL_API_BEGIN(NULL);
    GBL_API_FREE(pValue);
    GBL_API_END();
}

static GblObjectFamily_* GblObject_ensureFamily_(GblObject* pSelf) {
    GBL_API_BEGIN(NULL);
    GblObjectFamily_* pFamily = GblObject_family_(pSelf);
    if(!pFamily) {
        pFamily = GBL_API_MALLOC(gblAlignedAllocSize(sizeof(GblObjectFamily_)),
                                 GBL_ALIGNOF(GBL_MAX_ALIGN_T),
                                  "family");
        memset(pFamily, 0, sizeof(GblObjectFamily_));
        pFamily->pSelf = pSelf;
        GBL_API_CALL(GblArrayMap_setUserdata(&GBL_PRIV(pSelf->base).pFields,
                                             objectFamilyQuark_,
                                             (uintptr_t)pFamily,
                                             GblObject_family_destruct_));
    }
    GBL_API_END_BLOCK();
    return pFamily;
}

GBL_EXPORT GblObject* GblObject_parent(const GblObject* pSelf) {
    GblObjectFamily_* pFamily = GblObject_family_(pSelf);
    return pFamily? GBL_OBJECT_FAMILY_ENTRY_(pFamily->treeNode.pParent): NULL;
}

GBL_EXPORT void GblObject_setParent(GblObject* pSelf, GblObject* pParent) {
    GblObject* pOldParent = GblObject_parent(pSelf);
    if(pOldParent) {
        GblObject_removeChild(pOldParent, pSelf);
    }
    if(pParent) GblObject_addChild(pParent, pSelf);
}

GBL_EXPORT GblObject* GblObject_childFirst(const GblObject* pSelf) {
    GblObject* pChild = NULL;
    GblObjectFamily_* pFamily = GblObject_family_(pSelf);
    if(pFamily) {
        pChild = GBL_OBJECT_FAMILY_ENTRY_(pFamily->treeNode.pChildFirst);
    }
    return pChild;
}
GBL_EXPORT GblObject* GblObject_siblingNext(const GblObject* pSelf) {
    GblObject* pSibling = NULL;
    GblObjectFamily_* pFamily = GblObject_family_(pSelf);
    if(pFamily && pFamily->treeNode.pParent) {
        pSibling = GBL_OBJECT_FAMILY_ENTRY_(pFamily->treeNode.pSiblingNext);
    }
    return pSibling;
}

GBL_EXPORT void GblObject_addChild(GblObject* pSelf, GblObject* pChild) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pChild);
    {
        GblObjectFamily_* pParentFamily = GblObject_ensureFamily_(pSelf);
        GblObjectFamily_* pChildFamily  = GblObject_ensureFamily_(pChild);
        // yeah, slower than push-front, but we want to probably preserve ordering...
        GblNaryTree_addChildBack(&pParentFamily->treeNode,
                                 &pChildFamily->treeNode);
    }
    GBL_API_END_BLOCK();
}

GBL_EXPORT GblBool GblObject_removeChild(GblObject* pSelf, GblObject* pChild) {
    GblBool success = GBL_FALSE;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pChild);
    GblObjectFamily_* pParentFamily = GblObject_family_(pSelf);
    GblObjectFamily_* pChildFamily  = GblObject_family_(pChild);
    if(pParentFamily && pChildFamily && pChildFamily->treeNode.pParent == &pParentFamily->treeNode) {
        GblNaryTree_removeChild(&pParentFamily->treeNode,
                                &pChildFamily->treeNode);
        success = GBL_TRUE;
    }
    GBL_API_END_BLOCK();
    return success;
}

GBL_EXPORT GblObject* GblObject_findChildByType(const GblObject* pSelf, GblType childType) {
    GblObject* pChild = NULL;
    GBL_API_BEGIN(NULL); {
        GblObjectFamily_* pFamily = GblObject_family_(pSelf);
        if(pFamily) {
            for(GblNaryTreeNode* pIt = pFamily->treeNode.pChildFirst;
                pIt;
                pIt = pIt->pSiblingNext)
            {
                GblObject* pChildIt = GBL_OBJECT_FAMILY_ENTRY_(pIt);
                if(pChildIt) {
                    if(GBL_INSTANCE_CHECK(pChildIt, childType)) {
                        pChild = pChildIt;
                        break;
                    }
                }
            }
        }
    } GBL_API_END_BLOCK();
    return pChild;
}

GBL_EXPORT GblObject* GblObject_findChildByName(const GblObject* pSelf, const char* pName) {
    GblObject* pChild = NULL;
    GBL_API_BEGIN(NULL); {
        GblObjectFamily_* pFamily = GblObject_family_(pSelf);
        if(pFamily) {
            for(GblNaryTreeNode* pIt = pFamily->treeNode.pChildFirst;
                pIt;
                pIt = pIt->pSiblingNext)
            {
                GblObject* pChildIt = GBL_OBJECT_FAMILY_ENTRY_(pIt);
                if(pChildIt) {
                    const char* pNodeName = GblObject_name(pChildIt);
                    if(pNodeName && strcmp(pNodeName, pName) == 0) {
                        pChild = pChildIt;
                        break;
                    }
                }
            }
        }
    } GBL_API_END_BLOCK();
    return pChild;
}

GBL_EXPORT GblObject* GblObject_findChildByIndex(const GblObject* pSelf, GblSize index) {
    GblObject* pChild = NULL;
    GBL_API_BEGIN(NULL);
    GblObjectFamily_* pFamily = GblObject_family_(pSelf);
    if(pFamily) {
        pChild = GBL_OBJECT_FAMILY_ENTRY_(GblNaryTree_childAt(&pFamily->treeNode, index));
    }
    GBL_API_END_BLOCK();
    return pChild;
}

GBL_EXPORT GblSize GblObject_childCount(const GblObject* pSelf) {
    GblSize count = 0;
    GBL_API_BEGIN(NULL);
    GblObjectFamily_* pFamily = GblObject_family_(pSelf);
    if(pFamily) {
        count = GblNaryTree_childCount(&pFamily->treeNode);
    }
    GBL_API_END_BLOCK();
    return count;
}

GBL_EXPORT GblSize GblObject_depth(const GblObject* pSelf) {
    GblSize depth = 0;
    GblObjectFamily_* pFamily = GblObject_family_(pSelf);
    if(pFamily) {
        depth = GblNaryTree_depth(&pFamily->treeNode);
    }
    return depth;
}

GBL_EXPORT GblContext* GblObject_findContext(GblObject* pSelf) {
    GblObject* pContext = NULL;
    GblObject* pIt = pSelf;
    while(pIt) {
        if(GBL_PRIV_REF((GblBox*)pIt).contextType) {
            pContext = pIt;
            break;
        }
        pIt = GblObject_parent(pIt);
    }
    return (GblContext*)pContext;
}

GBL_EXPORT GblObject* GblObject_findAncestorByType(const GblObject* pSelf, GblType ancestorType) {
    GblObject* pAncestor = NULL;
    //GBL_API_BEGIN(GblType_context(GBL_INSTANCE_TYPEOF(pSelf))); {
        GblObject* pNode = GblObject_parent(pSelf);
        while(pNode) {
            if(GBL_INSTANCE_CHECK(pNode, ancestorType)) {
                pAncestor = pNode;
                break;
            }
            pNode = GblObject_parent(pNode);
        }

    //} GBL_API_END_BLOCK();
    return pAncestor;
}

GBL_EXPORT GblObject* GblObject_findAncestorByName(const GblObject* pSelf, const char* pName) {
    GblObject* pAncestor = NULL;
    GBL_API_BEGIN(NULL); {
        GblObject* pNode = GblObject_parent(pSelf);
        while(pNode) {
            const char* pNodeName = GblObject_name(pNode);
            if(pNodeName && strcmp(pNodeName, pName) == 0) {
                pAncestor = pNode;
                break;
            }
            pNode = GblObject_parent(pNode);
        }

    } GBL_API_END_BLOCK();
    return pAncestor;
}

GBL_EXPORT GblObject* GblObject_findAncestorByHeight(const GblObject* pSelf, GblSize height) {
    GblObject* pAncestor = NULL;
    GblObjectFamily_* pFamily = GblObject_family_(pSelf);
    if(pFamily) {
        pAncestor = GBL_OBJECT_FAMILY_ENTRY_(GblNaryTree_ancestor(&pFamily->treeNode, height));
    }
    return pAncestor;
}


GBL_EXPORT GblObject* GblObject_findAncestorBaseByDepth(const GblObject* pSelf, GblSize depth) {
    GblObject* pBase = NULL;
    GblObjectFamily_* pFamily = GblObject_family_(pSelf);
    if(pFamily) {
        pBase = GBL_OBJECT_FAMILY_ENTRY_(GblNaryTree_base(&pFamily->treeNode, depth));
    }
    return pBase;
}


GBL_EXPORT GblObject* GblObject_findSiblingByType(const GblObject* pSelf, GblType siblingType) {
    GblObject* pObject = NULL;
    if(GblObject_parent(pSelf)) {
        GblObject_findChildByType(GblObject_parent(pSelf), siblingType);
    }
    return pObject;
}

GBL_EXPORT GblObject* GblObject_findSiblingByName(const GblObject* pSelf, const char* pName) {
    GblObject* pObject = NULL;
    GblObject* pParent = GblObject_parent(pSelf);
    if(pParent) {
        pObject = GblObject_findChildByName(pParent, pName);
    }
    return pObject;
}

GBL_EXPORT GblObject* GblObject_findSiblingByIndex(const GblObject* pSelf, GblSize index) {
    GblObject* pAncestor = NULL;
    GblObjectFamily_* pFamily = GblObject_family_(pSelf);
    if(pFamily) {
        pAncestor = GBL_OBJECT_FAMILY_ENTRY_(GblNaryTree_siblingAt(&pFamily->treeNode, index));
    }
    return pAncestor;
}


GBL_EXPORT GblType GblObject_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static GblTypeInterfaceMapEntry ifaceEntries[] = {
        {
            .interfaceType = GBL_INVALID_TYPE,
            .classOffset   = offsetof(GblObjectClass, GblITableImpl)
        }, {
            .interfaceType = GBL_INVALID_TYPE,
            .classOffset   = offsetof(GblObjectClass, GblIEventHandlerImpl)
        }, {
            .interfaceType = GBL_INVALID_TYPE,
            .classOffset   = offsetof(GblObjectClass, GblIEventFilterImpl)
        }
    };

    static GblTypeInfo typeInfo = {
        .pFnClassInit     = (GblTypeClassInitializeFn)GblObjectClass_init_,
        .pFnClassFinal    = (GblTypeClassFinalizeFn)GblObjectClass_final_,
        .classSize        = sizeof(GblObjectClass),
        .pFnInstanceInit  = (GblTypeInstanceInitializeFn)GblObject_init_,
        .instanceSize     = sizeof(GblObject),
        .interfaceCount   = 3,
        .pInterfaceMap    = ifaceEntries
    };


    if(type == GBL_INVALID_TYPE) GBL_UNLIKELY {
        GBL_API_BEGIN(NULL);
        ifaceEntries[0].interfaceType = GBL_ITABLE_TYPE;
        ifaceEntries[1].interfaceType = GBL_IEVENT_HANDLER_TYPE;
        ifaceEntries[2].interfaceType = GBL_IEVENT_FILTER_TYPE;

        type = GblType_registerStatic(GblQuark_internStringStatic("GblObject"),
                                      GBL_BOX_TYPE,
                                      &typeInfo,
                                      GBL_TYPE_FLAG_TYPEINFO_STATIC);

        GBL_API_VERIFY_LAST_RECORD();
        GBL_API_END_BLOCK();
    }
    return type;
}

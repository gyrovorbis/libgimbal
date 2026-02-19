#include <gimbal/meta/instances/gimbal_object.h>
#include <gimbal/meta/types/gimbal_variant.h>
#include <gimbal/meta/properties/gimbal_property.h>
#include <gimbal/meta/instances/gimbal_context.h>
#include <gimbal/meta/instances/gimbal_event.h>
#include <gimbal/strings/gimbal_string_ref.h>
#include <gimbal/containers/gimbal_linked_list.h>
#include <gimbal/strings/gimbal_string_buffer.h>
#include <gimbal/containers/gimbal_nary_tree.h>
#include <gimbal/containers/gimbal_ring_list.h>
#include <gimbal/meta/signals/gimbal_marshal.h>
#include "../types/gimbal_type_.h"

#define GBL_OBJECT_EVENT_FILTER_VECTOR_SIZE_        sizeof(GblArrayList)
#define GBL_OBJECT_DERIVED_FLAG_INSTANTIATED_MASK_  0x1
#define GBL_OBJECT_DERIVED_FLAG_CONSTRUCTED_MASK_   0x2

static GblQuark objectNameQuark_           = GBL_QUARK_INVALID;
static GblQuark objectParentQuark_         = GBL_QUARK_INVALID;
static GblQuark objectFamilyQuark_         = GBL_QUARK_INVALID;
static GblQuark objectEventFiltersQuark_   = GBL_QUARK_INVALID;
static GblQuark objectNamePropertyQuark_   = GBL_QUARK_INVALID;
static GblQuark objectParentPropertyQuark_ = GBL_QUARK_INVALID;

// External private function for doing raw-dogged GblBox::userdata set.
GBL_RESULT GblBox_setUserdata_(GblBox* pSelf, void* pUserdata);

static uint16_t GblObject_derivedFlags_(const GblObject* pObject) {
    return GBL_PRIV_REF(GBL_BOX(pObject)).derivedFlags;
}

static void GblObject_setDerivedFlags_(GblObject* pObject, uint16_t mask) {
    GBL_PRIV_REF(GBL_BOX(pObject)).derivedFlags |= mask;
}

static void GblObject_clearDerivedFlags_(GblObject* pObject, uint16_t mask) {
    GBL_PRIV_REF(GBL_BOX(pObject)).derivedFlags &= ~mask;
}

GblBool GblObject_isInstantiating(const GblObject* pObject) {
    return !(GblObject_derivedFlags_(pObject) & GBL_OBJECT_DERIVED_FLAG_INSTANTIATED_MASK_);
}

GblBool GblObject_isConstructing(const GblObject* pObject) {
    return !(GblObject_derivedFlags_(pObject) & GBL_OBJECT_DERIVED_FLAG_CONSTRUCTED_MASK_);
}

GBL_EXPORT GBL_RESULT GblObject_sendEvent(GblObject* pSelf, GblEvent* pEvent) {
    GBL_CTX_BEGIN(NULL);

    GblObject* pParent = GblObject_parent(pSelf);
    if(pParent)
        GBL_CTX_CALL(GblObject_notifyEvent(pParent, pEvent));

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblObject_notifyEvent(GblObject* pSelf, GblEvent* pEvent) {
    GBL_CTX_BEGIN(NULL);
    GblObject* pObject = GBL_OBJECT(pSelf);

     // Run the event through the event filters
     const size_t eventFilterCount = GblObject_eventFilterCount(pObject);
     for(size_t  f = 0; f < eventFilterCount; ++f)
     {
         GblIEventReceiver* pFilter = GblObject_eventFilter(pSelf, f);
         GBL_CTX_CALL(GblIEventReceiver_receiveEvent(pFilter, GBL_IEVENT_RECEIVER(pObject), pEvent));
         if(GblEvent_state(pEvent) == GBL_EVENT_STATE_ACCEPTED) {
             GBL_CTX_DONE();
         }
     }

     // Send it to the event handler if the event handler
    GBL_CTX_CALL(GblIEventReceiver_receiveEvent(GBL_IEVENT_RECEIVER(pSelf), GBL_IEVENT_RECEIVER(pSelf), pEvent));
    if(GblEvent_state(pEvent) == GBL_EVENT_STATE_ACCEPTED) {
        GBL_CTX_DONE();
    }

    // Propagate up to parent
    GBL_CTX_CALL(GblObject_sendEvent(pSelf, pEvent));

    GBL_CTX_END();
}

static GBL_RESULT GblObject_eventFiltersDestruct_(const GblArrayMap* pMap, uintptr_t key, void* pData) {
    GBL_UNUSED(key, pMap);
    GBL_CTX_BEGIN(NULL);

    GBL_CTX_CALL(GblArrayList_destruct(pData));
    GBL_CTX_FREE(pData);

    GBL_CTX_END();
}

static GblArrayList* GblObject_eventFilters_(const GblObject* pSelf) {
    return (GblArrayList*)GblArrayMap_getValue(&GBL_PRIV(pSelf->base).pFields, objectEventFiltersQuark_);
}

static GblArrayList* GblObject_ensureEventFilters_(GblObject* pSelf) {
    GBL_CTX_BEGIN(NULL);

    GblArrayList* pEventFilters = GblObject_eventFilters_(pSelf);
    if(!pEventFilters) {
        pEventFilters = GBL_CTX_MALLOC(GBL_OBJECT_EVENT_FILTER_VECTOR_SIZE_);

        GBL_CTX_CALL(GblArrayList_construct(pEventFilters,
                                            sizeof(GblIEventReceiver*),
                                            0,
                                            NULL,
                                            GBL_OBJECT_EVENT_FILTER_VECTOR_SIZE_));
        GBL_CTX_CALL(GblArrayMap_setUserdata(&GBL_PRIV(pSelf->base).pFields,
                                             objectEventFiltersQuark_,
                                             (uintptr_t)pEventFilters,
                                             GblObject_eventFiltersDestruct_));
        pEventFilters = GblObject_eventFilters_(pSelf);
    }

    GBL_CTX_END_BLOCK();
    return pEventFilters;
}

GBL_EXPORT GBL_RESULT GblObject_installEventFilter(GblObject* pSelf, GblIEventReceiver* pFilter) {
    GBL_CTX_BEGIN(NULL);

    GBL_ASSERT(pFilter);
    GblArrayList* pFilters = GblObject_ensureEventFilters_(pSelf);
    GBL_CTX_CALL(GblArrayList_pushBack(pFilters, &pFilter));

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblObject_uninstallEventFilter(GblObject* pSelf, GblIEventReceiver* pFilter) {
    GblArrayList* pVector = NULL;
    GBL_CTX_BEGIN(NULL);

    GBL_ASSERT(pFilter);

    pVector = GblObject_eventFilters_(pSelf);
    GBL_CTX_VERIFY_POINTER(pVector);

    size_t count = GblArrayList_size(pVector);
    GBL_CTX_VERIFY(count, GBL_RESULT_ERROR_INVALID_HANDLE,
                   "Object has no event filter list to remove filter from: %x", pFilter);

    GblBool found = GBL_FALSE;
    for(size_t f = 0; f < count; ++f) {
        GblIEventReceiver** ppFilter = GblArrayList_at(pVector, f);
        if(*ppFilter == pFilter) {
            found = GBL_TRUE;
            GBL_CTX_CALL(GblArrayList_erase(pVector, f, 1));
            break;
        }
    }

    GBL_CTX_VERIFY(found, GBL_RESULT_ERROR_INVALID_HANDLE,
                   "EventFilter wasn't found on Object: %x", pFilter);

    GBL_CTX_END();
}

GBL_EXPORT GblIEventReceiver* GblObject_eventFilter(const GblObject* pSelf, size_t index) {
    GblIEventReceiver** ppFilter = NULL;

    GBL_CTX_BEGIN(NULL);

    GblArrayList* pFilters = GblObject_eventFilters_(pSelf);
    if(pFilters)
        ppFilter = GblArrayList_at(pFilters, index);

    GBL_CTX_END_BLOCK();
    return ppFilter? *ppFilter : NULL;
}

GBL_EXPORT size_t GblObject_eventFilterCount(const GblObject* pSelf) {
    size_t  count = 0;
    GBL_CTX_BEGIN(NULL);

    GblArrayList* pFilters = GblObject_eventFilters_(pSelf);

    if(pFilters) {
        count = GblArrayList_size(pFilters);
    }

    GBL_CTX_END_BLOCK();
    return count;
}

GBL_INLINE GBL_RESULT GblObject_propertyVCall_(const GblObject*   pSelf,
                                               const GblProperty* pProp,
                                               GblVariant*        pValue)
{
    const GblObjectClass* pClass =
            GBL_OBJECT_CLASS(GblClass_weakRefDefault(GblProperty_objectType(pProp)));

    return pClass->pFnProperty(pSelf, pProp, pValue);
}

GBL_EXPORT GBL_RESULT GblObject_propertyVariantByQuark(const GblObject* pSelf,
                                                       GblQuark         name,
                                                       GblVariant*      pValue)
{
    const GblProperty* pProp = GblProperty_findQuark(GBL_TYPEOF(pSelf),
                                                     name);
    if GBL_UNLIKELY(!pProp) {
        GBL_CTX_BEGIN(NULL);
        GBL_CTX_VERIFY(GBL_FALSE,
                       GBL_RESULT_ERROR_INVALID_PROPERTY,
                       "[GblObject] Attempt to get unknown property %s on object type %s",
                       GblQuark_toString(name), GblType_name(GBL_TYPEOF(pSelf)));
        GBL_CTX_END();
    }

    return GblObject_propertyVCall_(pSelf, pProp, pValue);
}

GBL_EXPORT GBL_RESULT GblObject_propertyVariant(const GblObject* pSelf,
                                                const char* pName,
                                                GblVariant* pValue)
{
    const GblQuark name = GblQuark_tryString(pName);

    if GBL_UNLIKELY(name == GBL_QUARK_INVALID) {
        GBL_CTX_BEGIN(NULL);
        GBL_CTX_VERIFY(GBL_FALSE,
                       GBL_RESULT_ERROR_INVALID_PROPERTY,
                       "[GblObject] Attempt to get unknown property %s on object type %s",
                       pName, GblType_name(GBL_TYPEOF(pSelf)));
        GBL_CTX_END();
    }

    return GblObject_propertyVariantByQuark(pSelf, name, pValue);
}

static GBL_RESULT GblObject_setPropertyVCall_(GblObject*         pSelf,
                                              const GblProperty* pProp,
                                              GblVariant*        pValue,
                                              GblFlags           flag) {
    // Clear status code and any pending errors.
    GBL_CTX_BEGIN(NULL);

    // Check whether the property we're attempting to set even supports being set from here.
    if GBL_LIKELY(pProp->flags & flag) {

        // Check whether the incoming value is of the expected property type
        if GBL_UNLIKELY(!GblType_check(GblVariant_typeOf(pValue), pProp->valueType)) {
            GBL_RESULT result;
            GblVariant v;

            // Convert the incoming value to the expected property type.
            GblVariant_constructDefault(&v, pProp->valueType);
            result = GblVariant_convert(pValue, &v);

            // Move the converted value back into the incoming value's variant as its new value.
            if(GBL_RESULT_SUCCESS(result))
                GblVariant_setMove(pValue, &v);

            // Clean up our conversion buffer variant regardless of whether conversion succeeded.
            GblVariant_destruct(&v);

            // Exit early if the result signified a failure.
            GBL_CTX_VERIFY_CALL(result);
        }

        // Check whether the incoming value is valid for the given property.
        if GBL_UNLIKELY(!GblProperty_checkValue(pProp, pValue))
            // Attempt to modify the incoming value to be within the valid range.
            GBL_CTX_VERIFY_CALL(GblProperty_validateValue(pProp, pValue));

        // Check whether the incoming value is actually different from the current value.
        GblBool changed = GBL_TRUE;
        {
            GblVariant currentValue = GBL_VARIANT_INIT;
            GBL_RESULT result       = GBL_RESULT_UNKNOWN;

            // If we're constructing the property, we compare it to the default property value.
            if GBL_UNLIKELY(flag & GBL_PROPERTY_FLAG_CONSTRUCT)
                result = GblProperty_defaultValue(pProp, &currentValue);
            // If we're writing the property, we compare it to the current property value.
            else if GBL_LIKELY(pProp->flags & GBL_PROPERTY_FLAG_READ)
                result = GblObject_propertyVCall_(pSelf, pProp, &currentValue);

            // If we failed to retrieve a comparison value or the values different, the property has changed.
            changed = (!GBL_RESULT_SUCCESS(result) ||
                        GblProperty_compareValues(pProp, pValue, &currentValue) != 0);

            // Destroy the variant containing the current property value.
            GblVariant_destruct(&currentValue);
        }

        // Only set the property to its new value if it actually changed.
        if GBL_LIKELY(changed) {
            // Retrieve GblClass vtable of the type associated with the property we're setting.
            const GblObjectClass* pClass =
                    GBL_OBJECT_CLASS(GblClass_weakRefDefault(GblProperty_objectType(pProp)));

            /* Call into virtual function to perform actual setting of the property from the variant,
               exiting early upon encountering reest. */
            GBL_CTX_VERIFY_CALL(pClass->pFnSetProperty(pSelf, pProp, pValue));

            /* Only bother signalling that the property has been modified if not done during construction,
               since you cannot even connect a signal to an instance until it has been constructed. */
            if(!GblObject_isInstantiating(pSelf))
                GBL_EMIT(pSelf, "propertyChange", pProp);
        }
    } else {
        // Report back that we tried to do an invalid write to a property that didn't support the operation.
        GBL_CTX_RECORD_SET(GBL_RESULT_UNSUPPORTED,
                           (flag & GBL_PROPERTY_FLAG_WRITE)?
                               "[GblObject] Attempt to set NON-WRITE property %s[%s]." :
                               "{GblObject] Attempt to construct NON-CONSTRUCT property %s[%s].",
                           GblType_name(GBL_TYPEOF(pSelf)),
                           GblProperty_name(pProp));
    }

    // Return accumulated status code.
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblObject_setPropertyVariantByQuark(GblObject*  pSelf,
                                                          GblQuark    name,
                                                          GblVariant* pValue)
{

    const GblProperty* pProp = GblProperty_findQuark(GBL_TYPEOF(pSelf), name);

    if GBL_UNLIKELY(!pProp) {
        GBL_CTX_BEGIN(NULL);
        GBL_CTX_VERIFY(GBL_FALSE,
                       GBL_RESULT_ERROR_INVALID_PROPERTY,
                       "[GblObject] Attempt to set unknown property %s on object type %s",
                       GblQuark_toString(name), GblType_name(GBL_TYPEOF(pSelf)));
        GBL_CTX_END();
    }

    return GblObject_setPropertyVCall_(pSelf, pProp, pValue, GBL_PROPERTY_FLAG_WRITE);
}

GBL_EXPORT GBL_RESULT GblObject_setPropertyVariant(GblObject* pSelf,
                                                   const char* pName,
                                                   GblVariant* pValue)
{
    const GblQuark name = GblQuark_tryString(pName);

    if GBL_UNLIKELY(name == GBL_QUARK_INVALID) {
        GBL_CTX_BEGIN(NULL);
        GBL_CTX_VERIFY(GBL_FALSE,
                       GBL_RESULT_ERROR_INVALID_PROPERTY,
                       "[GblObject] Attempt to set unknown property %s on object type %s",
                       pName, GblType_name(GBL_TYPEOF(pSelf)));
        GBL_CTX_END();
    }

    return GblObject_setPropertyVariantByQuark(pSelf, name, pValue);
}

GBL_EXPORT GBL_RESULT GblObject_properties(const GblObject* pSelf, ...) {
    va_list varArgs;
    va_start(varArgs, pSelf);

    GBL_CTX_BEGIN(NULL);
    GBL_CTX_CALL(GblObject_propertiesVa(pSelf, &varArgs));

    GBL_CTX_END_BLOCK();
    va_end(varArgs);
    return GBL_CTX_RESULT();
}

GBL_EXPORT GBL_RESULT GblObject_setProperties(GblObject* pSelf, ...) {
    va_list varArgs;
    va_start(varArgs, pSelf);

    GBL_CTX_BEGIN(NULL);
    GBL_CTX_CALL(GblObject_setPropertiesVa(pSelf, &varArgs));

    GBL_CTX_END_BLOCK();
    va_end(varArgs);
    return GBL_CTX_RESULT();
}

GBL_EXPORT GBL_RESULT GblObject_propertyVa_(const GblObject* pSelf, const GblProperty* pProp, va_list* pList) {
    GBL_CTX_BEGIN(NULL);
    GblVariant variant;

    GBL_CTX_VERIFY((pProp->flags & GBL_PROPERTY_FLAG_READ),
                   GBL_RESULT_ERROR_INVALID_PROPERTY,
                   "[GblObject] Tried to get unreadable property: [%s]",
                   GblProperty_name(pProp));

    GBL_CTX_CALL(GblVariant_constructDefault(&variant, GBL_NIL_TYPE));
    GBL_CTX_CALL(GblObject_propertyVCall_(pSelf, pProp, &variant));

    if(pProp->flags & GBL_PROPERTY_FLAG_OUT)
        GBL_CTX_CALL(GblVariant_valueMoveVa(&variant, pList));
    else
        GBL_CTX_CALL(GblVariant_valueCopyVa(&variant, pList));

    GBL_CTX_CALL(GblVariant_destruct(&variant));

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblObject_propertyVa(const GblObject* pSelf, const char* pName, va_list* pList) {
    GBL_CTX_BEGIN(NULL);

    const GblProperty* pProp = GblProperty_find(GBL_TYPEOF(pSelf), pName);
    GBL_CTX_VERIFY(pProp,
                   GBL_RESULT_ERROR_INVALID_PROPERTY,
                   "[GblObject] Tried to get unknown property: [%s::%s]",
                   GblType_name(GBL_TYPEOF(pSelf)),
                   pName);

    GBL_CTX_CALL(GblObject_propertyVa_(pSelf, pProp, pList));

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblObject_propertyVaByQuark(const GblObject* pSelf, GblQuark name, va_list* pList) {
    GBL_CTX_BEGIN(NULL);

    const GblProperty* pProp = GblProperty_findQuark(GBL_TYPEOF(pSelf), name);
    GBL_CTX_VERIFY(pProp,
                   GBL_RESULT_ERROR_INVALID_PROPERTY,
                   "[GblObject] Tried to get unknown property: %s[%s]",
                   GblType_name(GBL_TYPEOF(pSelf)),
                   GblQuark_toString(name));

    GBL_CTX_CALL(GblObject_propertyVa_(pSelf, pProp, pList));

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblObject_property(const GblObject* pSelf, const char* pName, ...) {
    va_list varArgs;

    va_start(varArgs, pName);
    const GBL_RESULT result = GblObject_propertyVa(pSelf, pName, &varArgs);
    va_end(varArgs);

    return result;
}

GBL_EXPORT GBL_RESULT GblObject_propertyByQuark(const GblObject* pSelf, GblQuark name, ...) {
    va_list varArgs;

    va_start(varArgs, name);
    const GBL_RESULT result = GblObject_propertyVaByQuark(pSelf, name, &varArgs);
    va_end(varArgs);

    return result;
}

GBL_EXPORT GBL_RESULT GblObject_setPropertyVa_(GblObject* pSelf, const GblProperty* pProp, va_list* pList) {
    GBL_CTX_BEGIN(NULL);

    GblVariant variant;
    GBL_CTX_VERIFY(pProp->flags & GBL_PROPERTY_FLAG_WRITE,
                   GBL_RESULT_ERROR_INVALID_PROPERTY,
                   "[GblObject] Tried to get unwritable property: [%s::%s]",
                   GblType_name(GBL_TYPEOF(pSelf)),
                   GblProperty_name(pProp));

    if(pProp->flags & GBL_PROPERTY_FLAG_IN)
        GBL_CTX_CALL(GblVariant_constructValueMoveVa(&variant, pProp->valueType, pList));
    else
        GBL_CTX_CALL(GblVariant_constructValueCopyVa(&variant, pProp->valueType, pList));

    GBL_CTX_CALL(GblObject_setPropertyVCall_(pSelf, pProp, &variant, GBL_PROPERTY_FLAG_WRITE));
    GBL_CTX_CALL(GblVariant_destruct(&variant));

    GBL_CTX_END();
}


GBL_EXPORT GBL_RESULT GblObject_setPropertyVa(GblObject* pSelf, const char* pName, va_list* pList) {
    GBL_CTX_BEGIN(NULL);

    const GblProperty* pProp = GblProperty_find(GBL_TYPEOF(pSelf), pName);
    GBL_CTX_VERIFY(pProp && pProp->flags & GBL_PROPERTY_FLAG_WRITE,
                   GBL_RESULT_ERROR_INVALID_PROPERTY,
                   "[GblObject] Tried to get unwritable property: [%s::%s]",
                   GblType_name(GBL_TYPEOF(pSelf)),
                   pName);

    GBL_CTX_VERIFY_CALL(GblObject_setPropertyVa_(pSelf, pProp, pList));

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblObject_setPropertyVaByQuark(GblObject* pSelf, GblQuark name, va_list* pList) {
    GBL_CTX_BEGIN(NULL);

    const GblProperty* pProp = GblProperty_findQuark(GBL_TYPEOF(pSelf), name);
    GBL_CTX_VERIFY(pProp->flags & GBL_PROPERTY_FLAG_WRITE,
                   GBL_RESULT_ERROR_INVALID_PROPERTY,
                   "[GblObject] Tried to get unwritable property: [%s]", GblQuark_toString(name));

    GBL_CTX_VERIFY_CALL(GblObject_setPropertyVa_(pSelf, pProp, pList));

    GBL_CTX_END();
}


GBL_EXPORT GBL_RESULT GblObject_setProperty(GblObject* pSelf, const char* pName, ...) {
    va_list varArgs;
    va_start(varArgs, pName);

    const GBL_RESULT result = GblObject_setPropertyVa(pSelf, pName, &varArgs);

    va_end(varArgs);
    return result;
}

GBL_EXPORT GBL_RESULT GblObject_setPropertyByQuark(GblObject* pSelf, GblQuark name, ...) {
    va_list varArgs;
    va_start(varArgs, name);

    const GBL_RESULT result = GblObject_setPropertyVaByQuark(pSelf, name, &varArgs);

    va_end(varArgs);
    return result;
}

GBL_EXPORT GBL_RESULT GblObject_propertiesVa(const GblObject* pSelf, va_list* pVarArgs) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_POINTER(pSelf);
    GBL_CTX_VERIFY_POINTER(pVarArgs);

    const char* pKey = NULL;
    while((pKey = va_arg(*pVarArgs, const char*))) {


        const GblProperty* pProp = GblProperty_find(GBL_TYPEOF(pSelf), pKey);

        if(pProp) {
            GblVariant variant;
            GBL_CTX_CALL(GblVariant_constructDefault(&variant, GBL_NIL_TYPE));
            GBL_CTX_CALL(GblObject_propertyVCall_(pSelf, pProp, &variant));

            if(pProp->flags & GBL_PROPERTY_FLAG_OUT)
                GBL_CTX_CALL(GblVariant_valueMoveVa(&variant, pVarArgs));
            else
                GBL_CTX_CALL(GblVariant_valueCopyVa(&variant, pVarArgs));

            if(pProp->flags & GBL_PROPERTY_FLAG_READ) {
                ;
            } else {
                GBL_CTX_WARN("[GblObject]: Failed to get non-readable property: %s[%s]",
                             GblType_name(GBL_TYPEOF(pSelf)), pKey);
            }
            GBL_CTX_CALL(GblVariant_destruct(&variant));
        } else {
            GBL_CTX_WARN("[GblObject]: Failed to get unknown property: %s[%s]",
                         GblType_name(GBL_TYPEOF(pSelf)), pKey);
        }
    }
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblObject_setPropertiesVa(GblObject* pSelf, va_list* pVarArgs) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_POINTER(pSelf);
    GBL_CTX_VERIFY_POINTER(pVarArgs);

    size_t  count = GblProperty_count(GBL_TYPEOF(pSelf));
    GblVariant*  pVariants = GBL_ALLOCA(sizeof(GblVariant) * count);
    const char** pKeys     = GBL_ALLOCA(sizeof(const char*) * count);

    size_t p = 0;
    const char* pKey = NULL;

    while((pKey = va_arg(*pVarArgs, const char*))) {
        pKeys[p] = pKey;
        const GblProperty* pProp = GblProperty_find(GBL_TYPEOF(pSelf), pKey);

        if(pProp) {
            if(pProp->flags & GBL_PROPERTY_FLAG_IN)
                GBL_CTX_CALL(GblVariant_constructValueMoveVa(&pVariants[p], pProp->valueType, pVarArgs));
            else
                GBL_CTX_CALL(GblVariant_constructValueCopyVa(&pVariants[p], pProp->valueType, pVarArgs));

            if(pProp->flags & GBL_PROPERTY_FLAG_WRITE) {
                ++p;
            } else {
                GBL_CTX_WARN("[GblObject]: Failed to set non-writable property: %s[%s]",
                             GblType_name(GBL_TYPEOF(pSelf)), pKey);
            }
        } else {
            GBL_CTX_WARN("[GblObject]: Failed to set unknown property: %s[%s]",
                         GblType_name(GBL_TYPEOF(pSelf)), pKey);
        }
    }

    GBL_CTX_CALL(GblObject_setPropertiesVariants(pSelf, p, pKeys, pVariants));

    for(size_t  v = 0; v < p; ++v) {
       GBL_CTX_CALL(GblVariant_destruct(&pVariants[v]));
    }

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblObject_propertiesVariant(const GblObject* pSelf, size_t propertyCount, const char* pNames[], GblVariant* pValues) {
    GBL_CTX_BEGIN(NULL);
    for(size_t  p = 0; p < propertyCount; ++p) {
        GBL_CTX_CALL(GblObject_propertyVariant(pSelf, pNames[p], &pValues[p]));
    }
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblObject_setPropertiesVariants(GblObject* pSelf, size_t propertyCount, const char* pNames[], GblVariant* pValues) {
    GBL_RESULT result = GBL_RESULT_SUCCESS;

    for(size_t  p = 0; p < propertyCount; ++p) {
        GBL_RESULT result2 = GblObject_setPropertyVariant(pSelf, pNames[p], &pValues[p]);

        if(!GBL_RESULT_SUCCESS(result2))
            result = result2;
    }

    // Return last error code.
    return result;
}

GBL_DECLARE_STRUCT_PRIVATE(GblObjectCtorIterClosure) {
    GblObject*          pObject;
    size_t              argCount;
    const GblProperty** ppProperties;
    GblVariant*         pValues;
};

static GblBool GblObject_iterateCtorProperties_(const GblProperty* pProp,
                                                void* pClosure)
{
    GBL_CTX_BEGIN(NULL);
    GblObjectCtorIterClosure_* pSelf = pClosure;
    /* Only require CONSTRUCT-able properties at construction-time
     * which are not also WRITE-able and cannot be set later on! */
    GblBool exitLoop = !(pProp->flags & GBL_PROPERTY_FLAG_WRITE);

    // Check whether we were passed a value for the property
    for(size_t  v = 0; v < pSelf->argCount; ++v) {
         // Set the required property to the given value
        if(pSelf->ppProperties[v] == pProp) {
            GBL_CTX_VERIFY_CALL(GblObject_setPropertyVCall_(pSelf->pObject,
                                                            pProp,
                                                            &pSelf->pValues[v],
                                                            GBL_PROPERTY_FLAG_CONSTRUCT));
            exitLoop = GBL_FALSE;
            break;
        }
    }

    // Bail if required property wasn't found
    GBL_CTX_VERIFY(!exitLoop,
                   GBL_RESULT_ERROR_INVALID_PROPERTY,
                   "[GblObject] Cannot instantiate %s without required CONSTRUCT-only property %s[%s]",
                   GblType_name(GBL_TYPEOF(pSelf->pObject)),
                   GblType_name(GblProperty_objectType(pProp)),
                   GblProperty_name(pProp));

    GBL_CTX_END_BLOCK();
    return exitLoop;
}

GBL_EXPORT GBL_RESULT GblObject_emitPropertyChange(const GblObject* pSelf, const char* pName) {
    return GblObject_emitPropertyChangeByQuark(pSelf, GblQuark_tryString(pName));
}

GBL_EXPORT GBL_RESULT GblObject_emitPropertyChangeByQuark(const GblObject* pSelf, GblQuark name) {
    GBL_CTX_BEGIN(NULL);

    const GblProperty* pProp;

    GBL_CTX_VERIFY(name != GBL_QUARK_INVALID,
                   GBL_RESULT_ERROR_INVALID_PROPERTY);

    GBL_CTX_VERIFY((pProp = GblProperty_findQuark(GBL_TYPEOF(pSelf), name)),
                   GBL_RESULT_ERROR_INVALID_PROPERTY,
                   "Failed to emit propertyChange: [%s]",
                   GblQuark_toString(name));

    GBL_EMIT(pSelf, "propertyChange", pProp);

    GBL_CTX_END();
}

GBL_EXPORT GblBool GblObject_blockPropertyChange(GblObject* pSelf, GblBool blocked) {
    return GblSignal_block(GBL_INSTANCE(pSelf), "propertyChange", blocked);
}

GBL_EXPORT GblBool GblObject_propertyChangeBlocked(const GblObject* pSelf) {
    return GblSignal_blocked(GBL_INSTANCE(pSelf), "propertyChange");
}

static GBL_RESULT GblObject_construct_(GblObject*         pSelf,
                                       size_t             argCount,
                                       const GblProperty* pProperties[],
                                       GblVariant*        pValues)
{
    GBL_CTX_BEGIN(NULL);

    const GblType selfType  = GBL_TYPEOF(pSelf);
    GblObjectCtorIterClosure_ closure = {
        .pObject      = pSelf,
        .argCount     = argCount,
        .ppProperties = pProperties,
        .pValues      = pValues
    };

    GblBool earlyExit = GblProperty_foreach(selfType,
                                            GBL_PROPERTY_FLAG_CONSTRUCT,
                                            GblObject_iterateCtorProperties_,
                                            &closure);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_VERIFY(!earlyExit,
                   GBL_RESULT_ERROR_INVALID_ARG,
                   "Unexpected error finding CTOR property");

    // Notify post-constructor that we constructed the object with the required properties
    GBL_CTX_VERIFY_CALL(GBL_OBJECT_GET_CLASS(pSelf)->pFnConstructed(pSelf));

    // Iterate over the remaining properties, doing regular writes.
    for(size_t  v = 0; v < argCount; ++v) {
        if(pProperties[v] && !(pProperties[v]->flags & GBL_PROPERTY_FLAG_CONSTRUCT)) {
            GBL_CTX_CALL(GblObject_setPropertyVCall_(pSelf,
                                                     pProperties[v],
                                                     &pValues[v],
                                                     GBL_PROPERTY_FLAG_WRITE));
        }
    }

    GBL_VCALL(GblObject, pFnInstantiated, pSelf);

    GBL_CTX_END();
}


GBL_EXPORT GblObject* GblObject_create(GblType type, ...) {
    GblObject* pObject = NULL;
    va_list    varArgs;
    va_start(varArgs, type);

    GBL_CTX_BEGIN(NULL);

    pObject = GblObject_createVa(type, &varArgs);

    GBL_CTX_END_BLOCK();
    va_end(varArgs);
    return pObject;
}

GBL_EXPORT GblObject* GblObject_createExt(GblType type, size_t size, ...) {
    GblObject* pObject = NULL;
    va_list    varArgs;
    va_start(varArgs, size);

    GBL_CTX_BEGIN(NULL);

    pObject = GblObject_createExtVa(type, size, &varArgs);

    GBL_CTX_END_BLOCK();
    va_end(varArgs);
    return pObject;
}

GBL_EXPORT GblObject* GblObject_createWithClass(GblObjectClass* pClass, ...)
{
    GblObject* pObject = NULL;
    va_list    varArgs;
    va_start(varArgs, pClass);

    GBL_CTX_BEGIN(NULL);

    pObject = GblObject_createVaWithClass(pClass, &varArgs);

    GBL_CTX_END_BLOCK();
    va_end(varArgs);
    return pObject;
}

GBL_EXPORT GblObject* GblObject_createExtWithClass(GblObjectClass* pClass, size_t size, ...) {
    GblObject* pObject = NULL;
    va_list    varArgs;
    va_start(varArgs, size);

    GBL_CTX_BEGIN(NULL);

    pObject = GblObject_createExtVaWithClass(pClass, size, &varArgs);

    GBL_CTX_END_BLOCK();
    va_end(varArgs);
    return pObject;
}

GBL_EXPORT GBL_RESULT GblObject_construct(GblObject* pSelf, GblType type, ...) {
    va_list varArgs;
    va_start(varArgs, type);

    GBL_CTX_BEGIN(NULL);
    GBL_CTX_CALL(GblObject_constructVa(pSelf, type, &varArgs));

    GBL_CTX_END_BLOCK();
    va_end(varArgs);
    return GBL_CTX_RESULT();
}

GBL_EXPORT GBL_RESULT GblObject_constructWithClass(GblObject* pSelf, GblObjectClass* pClass, ...) {
    va_list varArgs;
    va_start(varArgs, pClass);

    GBL_CTX_BEGIN(NULL);

    GBL_CTX_CALL(GblObject_constructVaWithClass(pSelf, pClass, &varArgs));

    GBL_CTX_END_BLOCK();
    va_end(varArgs);
    return GBL_CTX_RESULT();
}

static  GBL_RESULT GblObject_constructVa_(GblObject* pSelf, GblType type, va_list* pVarArgs) {
    size_t count = 0;
    GBL_CTX_BEGIN(NULL);

    GBL_CTX_VERIFY_TYPE(type, GBL_OBJECT_TYPE);
    GBL_ASSERT(pSelf);
    GBL_ASSERT(pVarArgs);

    const GblType       selfType     = GBL_TYPEOF(pSelf);
    const size_t        totalCount   = GblProperty_count(selfType);
    GblVariant*         pVariants    = GBL_ALLOCA(sizeof(GblVariant)   * totalCount);
    const GblProperty** ppProperties = GBL_ALLOCA(sizeof(GblProperty*) * totalCount);

    // check whether there are even constructable properties or not
    const char* pKey = NULL;
    while((pKey = va_arg(*pVarArgs, const char*))) {
        if(!pKey) break;

        ppProperties[count] = GblProperty_find(selfType, pKey);

        if(!ppProperties[count]) {
            GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY,
                               "[GblObject] Failed to construct with unknown property: [%s::%s]",
                               GblType_name(type), pKey);
        } else {

            if(ppProperties[count]->flags & GBL_PROPERTY_FLAG_IN)
                GBL_CTX_VERIFY_CALL(
                    GblVariant_constructValueMoveVa(&pVariants[count],
                                                    ppProperties[count]->valueType,
                                                    pVarArgs)
                );
            else
                GBL_CTX_VERIFY_CALL(
                    GblVariant_constructValueCopyVa(&pVariants[count],
                                                    ppProperties[count]->valueType,
                                                    pVarArgs)
                );

            ++count;
        }

    }

    GBL_CTX_VERIFY_CALL(
        GblObject_construct_(pSelf, count, ppProperties, pVariants)
    );

    GBL_CTX_END_BLOCK();
    for(size_t  v = 0; v < count; ++v)
        GblVariant_destruct(&pVariants[v]);

    return GBL_CTX_RESULT();
}

GBL_EXPORT GblObject* GblObject_createVa(GblType type, va_list* pVarArgs) {
    return GblObject_createExtVa(type, 0, pVarArgs);
}

GBL_EXPORT GblObject* GblObject_createExtVa(GblType type, size_t size, va_list* pVarArgs) {
    GblObject* pObject = NULL;
    GBL_CTX_BEGIN(NULL);

    GBL_CTX_VERIFY_TYPE(type, GBL_OBJECT_TYPE);

    pObject = GBL_OBJECT(GblBox_create(type, size));
    GBL_CTX_CALL(GblObject_constructVa_(pObject, type, pVarArgs));

    GBL_CTX_END_BLOCK();
    return pObject;
}

GBL_EXPORT GblObject* GblObject_createVaWithClass(GblObjectClass* pClass, va_list* pVarArgs) {
    return GblObject_createExtVaWithClass(pClass, 0, pVarArgs);
}

GBL_EXPORT GblObject* GblObject_createExtVaWithClass(GblObjectClass* pClass, size_t size, va_list* pVarArgs) {
    GblObject* pObject = NULL;
    GBL_CTX_BEGIN(NULL);

    GBL_CTX_VERIFY_TYPE(GBL_CLASS_TYPEOF(pClass), GBL_OBJECT_TYPE);

    pObject = GBL_OBJECT(GblBox_create(GBL_CLASS_TYPEOF(pClass),
                                       size,
                                       NULL,
                                       NULL,
                                       GBL_BOX_CLASS(pClass)));

    GBL_CTX_CALL(GblObject_constructVa_(pObject, GBL_CLASS_TYPEOF(pClass), pVarArgs));

    GBL_CTX_END_BLOCK();
    return pObject;
}

GBL_EXPORT GBL_RESULT GblObject_constructVa(GblObject* pSelf, GblType type, va_list* pVarArgs) {
    GBL_CTX_BEGIN(NULL);

    GBL_CTX_VERIFY_TYPE(type, GBL_OBJECT_TYPE);

    GBL_CTX_CALL(GblBox_construct((GblBox*)pSelf, type));
    GBL_CTX_CALL(GblObject_constructVa_(pSelf, type, pVarArgs));

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblObject_constructVaWithClass(GblObject* pSelf, GblObjectClass* pClass, va_list* pVarArgs) {
    const GblType type = GBL_CLASS_TYPEOF(pClass);

    GBL_CTX_BEGIN(NULL);

    GBL_CTX_VERIFY_TYPE(type, GBL_OBJECT_TYPE);

    GBL_CTX_CALL(GblBox_construct((GblBox*)pSelf,
                                  GBL_CLASS_TYPEOF(pClass),
                                  NULL,
                                  NULL,
                                  GBL_BOX_CLASS(pClass)));
    GBL_CTX_CALL(GblObject_constructVa_(pSelf, type, pVarArgs));

    GBL_CTX_END();
}

static GBL_RESULT GblObject_constructVariants_(GblObject*  pSelf,
                                               GblType     type,
                                               size_t      propertyCount,
                                               const char* pNames[],
                                               GblVariant* pValues)
{
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_TYPE(type, GBL_OBJECT_TYPE);
    GBL_CTX_VERIFY_POINTER(pSelf);

    const GblProperty** pProps = GBL_ALLOCA(sizeof(GblProperty*) * propertyCount);

    for(size_t  p = 0; p < propertyCount; ++p) {
        pProps[p] = GblProperty_find(GBL_TYPEOF(pSelf), pNames[p]);

        if(!pProps[p]) {
            GBL_CTX_WARN("[GblObject]: Failed to construct with unknown property: %s[%s]",
                         GblType_name(type), pNames[p]);
        }
    }

    GBL_CTX_VERIFY_CALL(GblObject_construct_(pSelf, propertyCount, pProps, pValues));

    GBL_CTX_END();
}

GBL_EXPORT GblObject* GblObject_createVariants(GblType type, size_t propertyCount, const char* pNames[], GblVariant* pValues) {
    return GblObject_createVariantsExt(type, 0, propertyCount, pNames, pValues);
}

GBL_EXPORT GblObject* GblObject_createVariantsExt(GblType type, size_t size, size_t propertyCount, const char* pNames[], GblVariant* pValues) {
    GblObject* pObject = NULL;

    GBL_CTX_BEGIN(NULL);

    GBL_CTX_VERIFY_TYPE(type, GBL_OBJECT_TYPE);

    pObject = GBL_OBJECT(GblBox_create(type, size));
    GBL_CTX_CALL(GblObject_constructVariants_(pObject, type, propertyCount, pNames, pValues));

    GBL_CTX_END_BLOCK();
    return pObject;
}

GBL_EXPORT GblObject* GblObject_createVariantsWithClass(GblObjectClass* pClass,
                                                        size_t          propertyCount,
                                                        const char*     pNames[],
                                                        GblVariant*     pValues)
{
    return GblObject_createVariantsExtWithClass(pClass,
                                                0,
                                                propertyCount,
                                                pNames,
                                                pValues);
}

GBL_EXPORT GblObject* GblObject_createVariantsExtWithClass(GblObjectClass* pClass,
                                                           size_t          size,
                                                           size_t          propertyCount,
                                                           const char*     pNames[],
                                                           GblVariant*     pValues)
{
    const GblType type = GBL_CLASS_TYPEOF(pClass);
    GblObject* pObject = NULL;

    GBL_CTX_BEGIN(NULL);

    GBL_CTX_VERIFY_TYPE(type, GBL_OBJECT_TYPE);

    pObject = GBL_OBJECT(GblBox_create(GBL_CLASS_TYPEOF(pClass),
                                       size,
                                       NULL,
                                       NULL,
                                       GBL_BOX_CLASS(pClass)));

    GBL_CTX_CALL(GblObject_constructVariants_(pObject, type, propertyCount, pNames, pValues));

    GBL_CTX_END_BLOCK();
    return pObject;
}

GBL_EXPORT GBL_RESULT GblObject_constructVariants(GblObject*  pSelf,
                                                  GblType     type,
                                                  size_t      propertyCount,
                                                  const char* pNames[],
                                                  GblVariant* pValues)
{
    GBL_CTX_BEGIN(NULL);

    GBL_CTX_VERIFY_TYPE(type, GBL_OBJECT_TYPE);

    GBL_CTX_CALL(GblBox_construct((GblBox*)pSelf, type));
    GBL_CTX_CALL(GblObject_constructVariants_(pSelf, type, propertyCount, pNames, pValues));

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblObject_constructVariantsWithClass(GblObject*      pSelf,
                                                           GblObjectClass* pClass,
                                                           size_t          propertyCount,
                                                           const char*     pNames[],
                                                           GblVariant*     pValues)
{
    const GblType type = GBL_CLASS_TYPEOF(pClass);

    GBL_CTX_BEGIN(NULL);

    GBL_CTX_VERIFY_TYPE(type, GBL_OBJECT_TYPE);

    GBL_CTX_CALL(GblBox_construct((GblBox*)pSelf,
                                  GBL_CLASS_TYPEOF(pClass),
                                  NULL,
                                  NULL,
                                  GBL_BOX_CLASS(pClass)));
    GBL_CTX_CALL(GblObject_constructVariants_(pSelf, type, propertyCount, pNames, pValues));

    GBL_CTX_END();
}


static GBL_RESULT GblObject_nameDestruct_(const GblArrayMap* pMap, uintptr_t key, void* pName) {
    GBL_UNUSED(key, pMap);

    GblStringRef_unref(pName);

    return GBL_RESULT_SUCCESS;
}


static GBL_RESULT GblObject_setNameRef_(GblObject* pSelf, GblStringRef* pRef) {
    return GblArrayMap_setUserdata(&GBL_PRIV(pSelf->base).pFields,
                                   objectNameQuark_,
                                   (uintptr_t)pRef,
                                   GblObject_nameDestruct_);
}

GBL_EXPORT void GblObject_setNameRef(GblObject* pSelf, GblStringRef* pRef) {
    GblStringRef* pName = GblObject_name(pSelf);

    if(!pName && !pRef)
        return;
    else if(pName && pRef) {
        if(!strcmp(pName, pRef))
            return;
    }

    GblObject_setNameRef_(pSelf, pRef);

    GblObject_emitPropertyChangeByQuark(pSelf, objectNamePropertyQuark_);
}

GBL_EXPORT void GblObject_setName(GblObject* pSelf, const char* pName) {
    GblObject_setNameRef(pSelf, GblStringRef_create(pName));
}

GBL_EXPORT GblStringRef* GblObject_name(const GblObject* pSelf) {
    return (GblStringRef*)GblArrayMap_getValue(&GBL_PRIV(pSelf->base).pFields,
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
    GBL_CTX_BEGIN(NULL);

    GBL_CTX_FREE(pValue);

    GBL_CTX_END();
}

static GblObjectFamily_* GblObject_ensureFamily_(GblObject* pSelf) {
    GBL_CTX_BEGIN(NULL);

    GblObjectFamily_* pFamily = GblObject_family_(pSelf);
    if(!pFamily) {
        pFamily = GBL_CTX_MALLOC(gblAlignedAllocSizeDefault(sizeof(GblObjectFamily_)),
                                 GBL_ALIGNOF(GBL_MAX_ALIGN_T),
                                 "family");

        memset(pFamily, 0, sizeof(GblObjectFamily_));
        pFamily->pSelf = pSelf;

        GBL_CTX_CALL(GblArrayMap_setUserdata(&GBL_PRIV(pSelf->base).pFields,
                                             objectFamilyQuark_,
                                             (uintptr_t)pFamily,
                                             GblObject_family_destruct_));
    }

    GBL_CTX_END_BLOCK();
    return pFamily;
}

GBL_EXPORT GblObject* GblObject_parent(const GblObject* pSelf) {
    GblObjectFamily_* pFamily = GblObject_family_(pSelf);

    return pFamily? GBL_OBJECT_FAMILY_ENTRY_(pFamily->treeNode.pParent): NULL;
}

static GblBool GblObject_setParent_(GblObject* pSelf, GblObject* pParent) {
    GblObject* pOldParent = GblObject_parent(pSelf);

    if(pParent == pOldParent)
        return GBL_FALSE;

    if(pOldParent)
        GblObject_removeChild(pOldParent, pSelf);

    if(pParent)
        GblObject_addChild(pParent, pSelf);

    return GBL_TRUE;
}

GBL_EXPORT void GblObject_setParent(GblObject* pSelf, GblObject* pParent) {
    if(GblObject_setParent_(pSelf, pParent))
        GblObject_emitPropertyChangeByQuark(pSelf, objectParentPropertyQuark_);
}

GBL_EXPORT GblObject* GblObject_childFirst(const GblObject* pSelf) {
    GblObject* pChild = NULL;
    GblObjectFamily_* pFamily = GblObject_family_(pSelf);

    if(pFamily) {
        pChild = GBL_OBJECT_FAMILY_ENTRY_(pFamily->treeNode.pChildFirst);
    }

    return pChild;
}

GBL_EXPORT GblObject* GblObject_childLast(const GblObject* pSelf) {
    GblObject* pChildLast = NULL;
    GblObject_foreachChild(pSelf, pChild) {
        pChildLast = pChild;
    }
    return pChildLast;
}

GBL_EXPORT GblObject* GblObject_siblingNext(const GblObject* pSelf) {
    GblObject* pSibling = NULL;
    GblObjectFamily_* pFamily = GblObject_family_(pSelf);

    if(pFamily && pFamily->treeNode.pParent) {
        pSibling = GBL_OBJECT_FAMILY_ENTRY_(pFamily->treeNode.pSiblingNext);
    }

    return pSibling;
}

GBL_EXPORT GblObject* GblObject_siblingNextByType(const GblObject* pSelf, GblType type) {
    do pSelf = GblObject_siblingNext(pSelf);
    while (pSelf != NULL && GBL_TYPEOF(pSelf) != type);
    return pSelf;
}

GBL_EXPORT GblObject* GblObject_siblingNextByName(const GblObject* pSelf, const char *name) {
    do pSelf = GblObject_siblingNext(pSelf);
    while (pSelf != NULL && strcmp(GblObject_name(pSelf), name) != 0);
    return pSelf;
}

GBL_EXPORT GblObject* GblObject_siblingPrevious(const GblObject* pSelf) {
    GblObject* pParent   = GblObject_parent(pSelf);
    GblObject* pPrevious = NULL;
    if(!pParent) return NULL;

    GblObject_foreachChild(pParent, pChild) {
        if (pChild == pSelf)
            return pPrevious;
        pPrevious = pChild;
    }

    return NULL;
}

GBL_EXPORT GblObject* GblObject_siblingPreviousByType(const GblObject* pSelf, GblType type) {
    do pSelf = GblObject_siblingPrevious(pSelf);
    while (pSelf != NULL && GBL_TYPEOF(pSelf) != type);
    return pSelf;
}

GBL_EXPORT GblObject* GblObject_siblingPreviousByName(const GblObject *pSelf, const char *name) {
    do pSelf = GblObject_siblingPrevious(pSelf);
    while (pSelf != NULL && strcmp(GblObject_name(pSelf), name) != 0);
    return pSelf;
}

GBL_EXPORT void GblObject_addChild(GblObject* pSelf, GblObject* pChild) {
    GBL_CTX_BEGIN(NULL);

    GBL_CTX_VERIFY_POINTER(pChild);
    {
        GblObjectFamily_* pParentFamily = GblObject_ensureFamily_(pSelf);
        GblObjectFamily_* pChildFamily  = GblObject_ensureFamily_(pChild);
        // yeah, slower than push-front, but we want to probably preserve ordering...
        GblNaryTree_addChildBack(&pParentFamily->treeNode,
                                 &pChildFamily->treeNode);
    }

    GBL_CTX_END_BLOCK();
}

GBL_EXPORT GblBool GblObject_removeChild(GblObject* pSelf, GblObject* pChild) {
    GblBool success = GBL_FALSE;

    GBL_CTX_BEGIN(NULL);

    GBL_CTX_VERIFY_POINTER(pChild);

    GblObjectFamily_* pParentFamily = GblObject_family_(pSelf);
    GblObjectFamily_* pChildFamily  = GblObject_family_(pChild);
    if(pParentFamily && pChildFamily && pChildFamily->treeNode.pParent == &pParentFamily->treeNode) {
        GblNaryTree_removeChild(&pParentFamily->treeNode,
                                &pChildFamily->treeNode);
        success = GBL_TRUE;
    }

    GBL_CTX_END_BLOCK();
    return success;
}

GBL_EXPORT void GblObject_addChildren(GblObject* pSelf, const GblRingList* pList) {
    GBL_ASSERT(pSelf && GBL_TYPEOF(pSelf) == GBL_OBJECT_TYPE);

    GblRingList_foreach(pList, pChild, GblObject*)
        GblObject_addChild(pSelf, pChild);

    GblObject_emitPropertyChange(pSelf, "children");
}

GBL_EXPORT void GblObject_removeChildren(GblObject* pSelf, const GblRingList* pList) {
    GBL_ASSERT(pSelf && GBL_TYPEOF(pSelf) == GBL_OBJECT_TYPE);

    GblRingList_foreach(pList, pChild, GblObject*)
        GblObject_removeChild(pSelf, pChild);

    GblObject_emitPropertyChange(pSelf, "children");
}

GBL_EXPORT void GblObject_clearChildren(GblObject* pSelf) {
    GBL_ASSERT(pSelf && GBL_TYPEOF(pSelf) == GBL_OBJECT_TYPE);

    GblObject_foreachChild(pSelf, pChild)
        GblObject_removeChild(pSelf, pChild);

    GblObject_emitPropertyChange(pSelf, "children");
}

GBL_EXPORT void GblObject_setChildren(GblObject* pSelf, const GblRingList* pList) {
    GblObject_clearChildren(pSelf);
    GblObject_addChildren(pSelf, pList);

    GblObject_emitPropertyChange(pSelf, "children");
}

GBL_EXPORT GblBool GblObject_iterateChildren(const GblObject* pSelf, GblObjectIterFn pFnIt, void* pUd) {
    GBL_ASSERT(pSelf && GBL_TYPEOF(pSelf) == GBL_OBJECT_TYPE);

    GblObject_foreachChild(pSelf, pChild)
        if (pFnIt(pChild, pUd)) return GBL_TRUE;

    return GBL_FALSE;
}

GBL_EXPORT GblObject* GblObject_findChildByType(const GblObject* pSelf, GblType childType) {
    GblObject* pChild = NULL;

    GBL_CTX_BEGIN(NULL); {
        GblObjectFamily_* pFamily = GblObject_family_(pSelf);
        if(pFamily) {
            for(GblNaryTreeNode* pIt = pFamily->treeNode.pChildFirst;
                pIt;
                pIt = pIt->pSiblingNext)
            {
                GblObject* pChildIt = GBL_OBJECT_FAMILY_ENTRY_(pIt);

                if(pChildIt) {
                    if(GblInstance_check(GBL_INSTANCE(pChildIt), childType)) {
                        pChild = pChildIt;
                        break;
                    }
                }
            }
        }
    } GBL_CTX_END_BLOCK();

    return pChild;
}

GBL_EXPORT GblObject* GblObject_findChildByName(const GblObject* pSelf, const char* pName) {
    GblObject* pChild = NULL;

    GBL_CTX_BEGIN(NULL); {
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
    } GBL_CTX_END_BLOCK();

    return pChild;
}

GBL_EXPORT GblObject* GblObject_findChildByIndex(const GblObject* pSelf, size_t  index) {
    GblObject* pChild = NULL;

    GBL_CTX_BEGIN(NULL);
    GblObjectFamily_* pFamily = GblObject_family_(pSelf);

    if(pFamily) {
        pChild = GBL_OBJECT_FAMILY_ENTRY_(GblNaryTree_childAt(&pFamily->treeNode, index));
    }

    GBL_CTX_END_BLOCK();
    return pChild;
}

GBL_EXPORT size_t  GblObject_childCount(const GblObject* pSelf) {
    size_t  count = 0;

    GBL_CTX_BEGIN(NULL);

    GblObjectFamily_* pFamily = GblObject_family_(pSelf);
    if(pFamily) {
        count = GblNaryTree_childCount(&pFamily->treeNode);
    }

    GBL_CTX_END_BLOCK();
    return count;
}

GBL_EXPORT size_t GblObject_childIndex(const GblObject* pSelf) {
    GblObject* pParent = GblObject_parent(pSelf);
    if (!pParent) return GBL_INDEX_INVALID;

    size_t i = 0;
    GblObject_foreachChild(pParent, pChild) {
        if (pChild == pSelf)
            return i;
        i++;
    }

    return GBL_INDEX_INVALID;
}

GBL_EXPORT size_t GblObject_depth(const GblObject* pSelf) {
    size_t  depth = 0;
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
    GblObject* pNode = GblObject_parent(pSelf);

    while(pNode) {
        if(GblInstance_check(GBL_INSTANCE(pNode), ancestorType)) {
            pAncestor = pNode;
            break;
        }

        pNode = GblObject_parent(pNode);
    }

    return pAncestor;
}

GBL_EXPORT GblObject* GblObject_findAncestorByName(const GblObject* pSelf, const char* pName) {
    GblObject* pAncestor = NULL;

    GBL_CTX_BEGIN(NULL); {
        GblObject* pNode = GblObject_parent(pSelf);
        while(pNode) {
            const char* pNodeName = GblObject_name(pNode);

            if(pNodeName && strcmp(pNodeName, pName) == 0) {
                pAncestor = pNode;
                break;
            }

            pNode = GblObject_parent(pNode);
        }

    } GBL_CTX_END_BLOCK();

    return pAncestor;
}

GBL_EXPORT GblObject* GblObject_findAncestorByHeight(const GblObject* pSelf, size_t  height) {
    GblObject* pAncestor = NULL;
    GblObjectFamily_* pFamily = GblObject_family_(pSelf);

    if(pFamily) {
        pAncestor = GBL_OBJECT_FAMILY_ENTRY_(GblNaryTree_ancestor(&pFamily->treeNode, height));
    }

    return pAncestor;
}


GBL_EXPORT GblObject* GblObject_findAncestorBaseByDepth(const GblObject* pSelf, size_t  depth) {
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

GBL_EXPORT GblObject* GblObject_findSiblingByIndex(const GblObject* pSelf, size_t  index) {
    GblObject* pAncestor = NULL;
    GblObjectFamily_* pFamily = GblObject_family_(pSelf);

    if(pFamily) {
        pAncestor = GBL_OBJECT_FAMILY_ENTRY_(GblNaryTree_siblingAt(&pFamily->treeNode, index));
    }

    return pAncestor;
}

static GBL_RESULT GblObject_IVariant_construct_(GblVariant* pVariant, size_t  argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_UNUSED(argc);
    GBL_CTX_BEGIN(NULL);

    if(op == GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT) {
        pVariant->pVoid = GblInstance_create(pVariant->type);
    } else if(op == GBL_IVARIANT_OP_FLAG_CONSTRUCT_COPY ||
              op == GBL_IVARIANT_OP_FLAG_CONSTRUCT_VALUE_COPY)
    {
        pVariant->pVoid = pArgs[0].pVoid?
                          GblBox_ref(GBL_BOX(pArgs[0].pVoid)) : NULL;
    } else {
        pVariant->pVoid = pArgs[0].pVoid;
    }

    GBL_CTX_END();
}

static GBL_RESULT GblObject_IVariant_destruct_(GblVariant* pVariant) {
    GBL_CTX_BEGIN(NULL);

    GblBox_unref(pVariant->pVoid);

    GBL_CTX_END();
}

static GBL_RESULT GblObject_IVariant_compare_(const GblVariant* pVariant, const GblVariant* pOther, int* pResult) {
    GBL_CTX_BEGIN(NULL);

    *pResult = (pVariant->pVoid == pOther->pVoid);

    GBL_CTX_END();
}

static GBL_RESULT GblObject_IVariant_get_(GblVariant* pSelf, size_t  argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_UNUSED(argc);

    GBL_CTX_BEGIN(NULL);

    *(GblObject**)pArgs[0].pVoid = pSelf->pVoid;

    if(op == GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY) {
        GblBox_ref(GBL_BOX(pSelf->pVoid));
    } else if(op == GBL_IVARIANT_OP_FLAG_GET_VALUE_MOVE) {
        pSelf->type = GBL_NIL_TYPE;
    }

    GBL_CTX_END();
}

static GBL_RESULT GblObject_IVariant_set_(GblVariant* pSelf, size_t  argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_UNUSED(argc);
    GBL_CTX_BEGIN(NULL);

    pSelf->pVoid = pArgs[0].pVoid;

    if(op == GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY || op == GBL_IVARIANT_OP_FLAG_SET_COPY) {
        GblBox_ref(GBL_BOX(pSelf->pVoid));
    } else if(op == GBL_IVARIANT_OP_FLAG_SET_MOVE || op == GBL_IVARIANT_OP_FLAG_SET_VALUE_MOVE) {
        pArgs[0].type = GBL_NIL_TYPE;
    }

    GBL_CTX_END();
}

static GBL_RESULT GblObject_IVariant_load_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_UNUSED(pVariant && pString);
    return GBL_RESULT_UNIMPLEMENTED;
}

static GBL_RESULT GblObject_IVariant_save_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GblVariant variant = GBL_VARIANT_INIT;
    GblBool first = GBL_TRUE;

    GBL_CTX_BEGIN(NULL);
    GblStringBuffer_append(pString, "{\n");

    for(const GblProperty* pIt = GblProperty_next(GblVariant_typeOf(pVariant), NULL, GBL_PROPERTY_FLAG_SAVE);
         pIt != NULL;
         pIt = GblProperty_next(GblVariant_typeOf(pVariant), pIt, GBL_PROPERTY_FLAG_SAVE))
    {
        if(!first) GblStringBuffer_append(pString, ",\n");
        const char* pKey = GblProperty_name(pIt);

        GblStringBuffer_append(pString, "\t");
        GblStringBuffer_append(pString, pKey);
        GblStringBuffer_append(pString, " = ");
        GblObject_propertyVariant(pVariant->pVoid, pKey, &variant);

        GblVariant_save(&variant, pString);
        first = GBL_FALSE;

    }

    GblStringBuffer_append(pString, "}\n");

    GBL_CTX_END();
}

// ========== ITABLEVARIANT IFACE ==========

static GBL_RESULT GblObject_ITableVariant_index_(const GblVariant* pSelf,
                                                 const GblVariant* pKey,
                                                 GblVariant*       pValue)
{
    GblVariant k;
    GblVariant_constructCopy(&k, pKey);

    const GBL_RESULT retValue =
        GblObject_propertyVariant(GblVariant_objectPeek(pSelf),
                                  GblVariant_toString(&k),
                                  pValue);

    GblVariant_destruct(&k);

    return retValue;
}

static GBL_RESULT GblObject_ITableVariant_setIndex_(const GblVariant* pSelf,
                                                    const GblVariant* pKey,
                                                    GblVariant*       pValue)
{
    GblVariant k;
    GblVariant_constructCopy(&k, pKey);

    const GBL_RESULT retValue =
        GblObject_setPropertyVariant(GblVariant_objectPeek(pSelf),
                                     GblVariant_toString(&k),
                                     pValue);

    GblVariant_destruct(&k);

    return retValue;
}

typedef struct PropertyIterClosure_ {
    const GblProperty* pPrev;
    const GblProperty* pNext;
    GblBool            nextOne;
} PropertyIterClosure_;

static GblBool GblObject_ITableVariant_next_propertyIterator_(const GblProperty* pProp,
                                                              void*              pClosure)
{
    PropertyIterClosure_* pCl = pClosure;

    if(!pCl->pPrev) {
        pCl->pNext = pProp;
        return GBL_TRUE;
    } else if(pCl->nextOne) {
        pCl->pNext = pProp;
        return GBL_TRUE;
    } else if(pProp == pCl->pPrev)
        pCl->nextOne = GBL_TRUE;

    return GBL_FALSE;
}

static GBL_RESULT GblObject_ITableVariant_next_(const GblVariant* pSelf,
                                                GblVariant*       pKey,
                                                GblVariant*       pValue)
{
    const GblProperty* pPrev = NULL;
    const GblType      type  = GblVariant_typeOf(pSelf);

    if(GblVariant_isValid(pKey))
        pPrev = GblProperty_find(type, GblVariant_toString(pKey));

    PropertyIterClosure_ cl = {
        .pPrev = pPrev
    };

    GblProperty_foreach(type,
                        GBL_PROPERTY_FLAG_READ,
                        GblObject_ITableVariant_next_propertyIterator_,
                        &cl);

    if(!cl.pNext) {
        GblVariant_destruct(pKey);
        GblVariant_destruct(pValue);

        return cl.nextOne? GBL_RESULT_SUCCESS : GBL_RESULT_ERROR_INVALID_PROPERTY;
    } else {
        GblVariant_set(pKey, GblProperty_name(cl.pNext));
        return GblObject_propertyVCall_(GblVariant_objectPeek(pSelf),
                                        cl.pNext,
                                        pValue);
    }

}

static GBL_RESULT GblObject_ITableVariant_count_(const GblVariant* pSelf,
                                                 size_t*           pSize) {
    *pSize = GblProperty_count(GblVariant_typeOf(pSelf));
    return GBL_RESULT_SUCCESS;
}

// ======== OBJECT CLASS ==========

static GBL_RESULT GblObject_instantiated_(GblObject* pSelf) {
    GblObject_setDerivedFlags_(pSelf, GBL_OBJECT_DERIVED_FLAG_INSTANTIATED_MASK_);
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GblObject_Box_destructor_(GblBox* pBox) {
    GBL_CTX_BEGIN(NULL);
    GblObject* pSelf = (GblObject*)pBox;

    GblObject_setParent(pSelf, NULL);

    GblRingList* pChildren;
    GblObject_property(pSelf, "children", &pChildren);

    GblRingList_foreach(pChildren, pChild)
        GBL_UNREF(pChild);
    GblRingList_unref(pChildren);

    GblBoxClass* pBoxClass = GBL_BOX_CLASS(GblClass_weakRefDefault(GBL_BOX_TYPE));
    GBL_CTX_VERIFY_CALL(pBoxClass->pFnDestructor(pBox));
    GBL_CTX_END();
}

static GBL_RESULT GblObject_property_(const GblObject* pSelf, const GblProperty* pProp, GblVariant* pValue)  {
    GBL_CTX_BEGIN(NULL);

    switch(pProp->id) {
    case GblObject_Property_Id_name: {
        GblStringRef* pName = GblObject_name(pSelf);
        if(pName)
            GblVariant_setValueMove(pValue,
                                    pProp->valueType,
                                    GblStringRef_ref(pName));
        else
            GblVariant_setNil(pValue);
    } break;
    case GblObject_Property_Id_parent:
        GblVariant_setValueMove(pValue,
                                pProp->valueType,
                                GblObject_parent(pSelf));
        break;
    case GblObject_Property_Id_refCount:
        GblVariant_setValueCopy(pValue,
                                pProp->valueType,
                                GblBox_refCount(GBL_BOX(pSelf)));
        break;
    case GblObject_Property_Id_userdata:
        GblVariant_setValueCopy(pValue,
                                pProp->valueType,
                                GblBox_userdata(GBL_BOX(pSelf)));
        break;
    case GblObject_Property_Id_children: {
        GblObject*   pChild    = GblObject_childFirst(pSelf);
        GblRingList* pChildren = GblRingList_createEmpty();

        while (pChild) {
            GblRingList_pushBack(pChildren, pChild);
            pChild = GblObject_siblingNext(pChild);
        }

        GblVariant_setValueMove(pValue,
                                pProp->valueType,
                                pChildren);
        break;
    }
    default: GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY,
                                "Reading unhandled property: %s",
                                GblProperty_name(pProp));
    }

    GBL_CTX_END();
}

static GBL_RESULT GblObject_setProperty_(GblObject* pSelf, const GblProperty* pProp, GblVariant* pValue) {
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(pProp);

    switch(pProp->id) {
    case GblObject_Property_Id_name: {
        GblStringRef* pName = NULL;
        GBL_CTX_CALL(GblVariant_valueMove(pValue, &pName));
        GblObject_setNameRef_(pSelf, pName);
        break;
    }
    case GblObject_Property_Id_parent: {
        GblObject* pParent = NULL;
        GBL_CTX_CALL(GblVariant_valuePeek(pValue, &pParent));
        GblObject_setParent_(pSelf, pParent);
        break;
    }
    case GblObject_Property_Id_children: {
        GblRingList* pChildren = NULL;
        GBL_CTX_CALL(GblVariant_valuePeek(pValue, &pChildren));

        GblRingList_foreach(pChildren, pObject, GblObject*)
            GblObject_setParent(pObject, pSelf);

        break;
    }
    case GblObject_Property_Id_userdata: {
        void* pUserdata = NULL;
        GBL_CTX_CALL(GblVariant_valueCopy(pValue, &pUserdata));
        GblBox_setUserdata_(GBL_BOX(pSelf), pUserdata);
        break;
    }
    default: GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY,
                                "Writing unhandled property: %s",
                                GblProperty_name(pProp));
    }

    GBL_CTX_END();
}

static GBL_RESULT GblObject_constructed_(GblObject* pSelf) {
    GblObject_setDerivedFlags_(pSelf, GBL_OBJECT_DERIVED_FLAG_CONSTRUCTED_MASK_);
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GblObjectClass_init_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);
    GBL_CTX_BEGIN(NULL);

    static GblIVariantVTable iVariantVTable;

    // static constructor for first instance of class
    if(!GblType_classRefCount(GBL_OBJECT_TYPE)) {
        objectNameQuark_           = GblQuark_fromStatic("_name");
        objectParentQuark_         = GblQuark_fromStatic("_parent");
        objectFamilyQuark_         = GblQuark_fromStatic("_family");
        objectEventFiltersQuark_   = GblQuark_fromStatic("_eventFilters");
        objectNamePropertyQuark_   = GblQuark_fromStatic("name");
        objectParentPropertyQuark_ = GblQuark_fromStatic("parent");

        GBL_PROPERTIES_REGISTER(GblObject);

        GblSignal_install(GBL_OBJECT_TYPE,
                          "propertyChange",
                          GblMarshal_CClosure_VOID__INSTANCE_BOX,
                          1,
                          GBL_BOX_TYPE);

        memcpy(&iVariantVTable, GBL_IVARIANT_CLASS(pClass)->pVTable, sizeof(GblIVariantVTable));
        iVariantVTable.pFnLoad = GblObject_IVariant_load_;
        iVariantVTable.pFnSave = GblObject_IVariant_save_;
    }

    GBL_IVARIANT_CLASS(pClass)      ->pVTable         = &iVariantVTable;
    GBL_BOX_CLASS(pClass)           ->pFnDestructor   = GblObject_Box_destructor_;
    GBL_OBJECT_CLASS(pClass)        ->pFnConstructed  = GblObject_constructed_;
    GBL_OBJECT_CLASS(pClass)        ->pFnInstantiated = GblObject_instantiated_;
    GBL_OBJECT_CLASS(pClass)        ->pFnProperty     = GblObject_property_;
    GBL_OBJECT_CLASS(pClass)        ->pFnSetProperty  = GblObject_setProperty_;
    GBL_ITABLE_VARIANT_CLASS(pClass)->pFnIndex        = GblObject_ITableVariant_index_;
    GBL_ITABLE_VARIANT_CLASS(pClass)->pFnSetIndex     = GblObject_ITableVariant_setIndex_;
    GBL_ITABLE_VARIANT_CLASS(pClass)->pFnNext         = GblObject_ITableVariant_next_;
    GBL_ITABLE_VARIANT_CLASS(pClass)->pFnCount        = GblObject_ITableVariant_count_;

    GBL_CTX_END();
}

static GBL_RESULT GblObjectClass_final_(GblClass* pClass, const void* pClassData) {
    GBL_UNUSED(pClassData);
    GBL_CTX_BEGIN(NULL);

    if(!GblType_classRefCount(GBL_CLASS_TYPEOF(pClass))) {
        GBL_CTX_CALL(GblProperty_uninstallAll(GBL_CLASS_TYPEOF(pClass)));

        if(GBL_CLASS_TYPEOF(pClass) == GBL_OBJECT_TYPE)
            GblSignal_uninstall(GBL_OBJECT_TYPE, "propertyChange");
    }

    GBL_CTX_END();
}


GBL_EXPORT GblType GblObject_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static GblInterfaceImpl ifaceEntries[] = {
        {
            .interfaceType = GBL_INVALID_TYPE,
            .classOffset   = offsetof(GblObjectClass, GblITableVariantImpl)
        }, {
            .interfaceType = GBL_INVALID_TYPE,
            .classOffset   = offsetof(GblObjectClass, GblIEventReceiverImpl)
        }
    };

    static GblTypeInfo typeInfo = {
        .pFnClassInit     = GblObjectClass_init_,
        .pFnClassFinal    = GblObjectClass_final_,
        .classSize        = sizeof(GblObjectClass),
        .instanceSize     = sizeof(GblObject),
        .interfaceCount   = 2,
        .pInterfaceImpls  = ifaceEntries
    };

    if GBL_UNLIKELY(type == GBL_INVALID_TYPE) {
        ifaceEntries[0].interfaceType = GBL_ITABLE_VARIANT_TYPE;
        ifaceEntries[1].interfaceType = GBL_IEVENT_RECEIVER_TYPE;

        type = GblType_register(GblQuark_internStatic("GblObject"),
                                GBL_BOX_TYPE,
                                &typeInfo,
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }
    return type;
}

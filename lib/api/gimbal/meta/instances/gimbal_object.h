/*! \file
 *  \brief GblObject structure and related functions
 *  \ingroup metaBuiltinTypes
 */

#ifndef GIMBAL_OBJECT_H
#define GIMBAL_OBJECT_H

#include "../../core/gimbal_api_frame.h"
#include "gimbal_box.h"
#include "../ifaces/gimbal_ivariant.h"
#include "../ifaces/gimbal_itable.h"
#include "../ifaces/gimbal_ievent_handler.h"
#include "../ifaces/gimbal_ievent_filter.h"
#include "../properties/gimbal_property.h"
#include "../../strings/gimbal_quark.h"

/*! \ingroup metaBuiltinTypes
 * \details  ::GblType UUID of a GblObject
 */
#define GBL_OBJECT_TYPE                     (GBL_BUILTIN_TYPE(OBJECT))
#define GBL_OBJECT_STRUCT                   GblObject
#define GBL_OBJECT_CLASS_STRUCT             GblObjectClass

#define GBL_OBJECT(instance)                GBL_INSTANCE_CAST_PREFIX(instance, GBL_OBJECT)
#define GBL_OBJECT_CHECK(instance)          GBL_INSTANCE_CHECK_PREFIX(instance, GBL_OBJECT)
#define GBL_OBJECT_TRY(instance)            GBL_INSTANCE_TRY_PREFIX(instance, GBL_OBJECT)

#define GBL_OBJECT_CLASS(klass)             GBL_CLASS_CAST_PREFIX(klass, GBL_OBJECT)
#define GBL_OBJECT_CLASS_CHECK(klass)       GBL_CLASS_CHECK_PREFIX(klass, GBL_OBJECT)
#define GBL_OBJECT_CLASS_TRY(klass)         GBL_CLASS_TRY_PREFIX(klass, GBL_OBJECT)

#define GBL_OBJECT_GET_CLASS(instance)      GBL_INSTANCE_GET_CLASS_PREFIX(instance, GBL_OBJECT)
#define GBL_OBJECT_TRY_CLASS(instance)      GBL_INSTANCE_TRY_CLASS_PREFIX(instance, GBL_OBJECT)

#define GBL_OBJECT_REF(object)              (GblBox_ref((GblBox*)object))
#define GBL_OBJECT_UNREF(object)            (GblBox_unref((GblBox*)object))

#define SELF GblObject* pSelf
#define CSELF const SELF

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblObject);
GBL_FORWARD_DECLARE_STRUCT(GblEvent);

/*! \class GblObjectClass
 *  \brief GblClass providing extended OO functionality
 * \ingroup metaBuiltinTypes
 *  \sa GblClass, GblObject
 *  \extends GblBoxClass
 *  \implements GblITableIFace, GblIEventHandlerIFace, GblIEventFilterIFace
 */
GBL_CLASS_DERIVE(GblObjectClass,
                 GblBoxClass,
                 GblITableIFace, GblIEventHandlerIFace, GblIEventFilterIFace)

    GBL_RESULT (*pFnConstructor)        (SELF);
    GBL_RESULT (*pFnConstructed)        (SELF);
    GBL_RESULT (*pFnPropertyGet)        (CSELF, GblSize id, GblVariant* pValue, const GblProperty* pProp);
    GBL_RESULT (*pFnPropertySet)        (SELF,  GblSize id, const GblVariant* pValue, const GblProperty* pProp);
    //GBL_RESULT (*pFnSlotCall)           (SELF,  GblSize id, GblVariant* pRet, GblUint argc, GblVariant* pArgs, const GblSlot* pSlot);
    //GBL_RESULT (*pFnPropertyNotify)     (SELF,  GblProperty* pProperty);    // signal
GBL_CLASS_END

typedef enum GBL_OBJECT_ATTRIBUTE {
    GBL_OBJECT_ATTRIBUTE_CONSTRUCTED_IN_PLACE,
    GBL_OBJECT_ATTRIBUTE_COUNT
} GBL_OBJECT_ATTRIBUTE;

/*! \class GblObject
 *  \brief GblInstance providing extended OO functionality
 *  \ingroup metaBuiltinTypes
 *  \details
 *  A GblObject is an GblInstance type which provides support
 *  for reference counting semantics, GblVariant interop,
 *  event handling, parent/child relationships, arbitrary
 *  userdata storage, and properties.
 *  \sa GblInstance, GblObjectClass
 *  \extends GblBox
 *  \implements GblITable, GblIEventHandler, GblIEventFilter
 */
GBL_INSTANCE_DERIVE_EMPTY(GblObject, GblBox,
                    GblObjectClass)

typedef enum GBL_OBJECT_PROPERTY_ID {
    GBL_OBJECT_PROPERTY_ID_FIRST,
    GBL_OBJECT_PROPERTY_ID_REFCOUNT=GBL_OBJECT_PROPERTY_ID_FIRST,
    GBL_OBJECT_PROPERTY_ID_NAME,
    GBL_OBJECT_PROPERTY_ID_PARENT,
    GBL_OBJECT_PROPERTY_ID_USERDATA,
    GBL_OBJECT_PROPERTY_ID_COUNT
} GBL_OBJECT_PROPERTY_ID;

GBL_PROPERTY_TABLE_BEGIN(GBL_OBJECT)
    GBL_PROPERTY_ENTRY("refCount",
                       "Reference Count",
                       "Number of remaining references to object",
                       GBL_OBJECT_PROPERTY_ID_REFCOUNT,
                       GBL_UINT32_TYPE,
                       GBL_PROPERTY_FLAGS_MASK(READ))
    GBL_PROPERTY_ENTRY("name",
                       "Name",
                       "String name identifier for object",
                       GBL_OBJECT_PROPERTY_ID_NAME,
                       GBL_POINTER_TYPE,
                       GBL_PROPERTY_FLAGS_MASK(READ, WRITE, LOAD, SAVE, CONSTRUCT))
    GBL_PROPERTY_ENTRY("parent",
                       "Parent",
                       "Parent Object to this object",
                       GBL_OBJECT_PROPERTY_ID_PARENT,
                       GBL_OBJECT_TYPE,
                       GBL_PROPERTY_FLAGS_MASK(READ, WRITE, CONSTRUCT))
    GBL_PROPERTY_ENTRY("userdata",
                       "User Data",
                       "Generic Userdata void* for miscellaneous storage",
                       GBL_OBJECT_PROPERTY_ID_USERDATA,
                       GBL_POINTER_TYPE,
                       GBL_PROPERTY_FLAGS_MASK(READ, WRITE, CONSTRUCT))
GBL_PROPERTY_TABLE_END()
#if 1
#if 0
GBL_PROPERTIES(GblObject,
    ("refCount", GBL_UINT32_TYPE, (READ, WRITE)),
    ("name",     GBL_STRING_TYPE, (READ)),
    ("duder",     GBL_STRING_TYPE, (SAVE)),
    ("duderette", GBL_STRING_TYPE, (LOAD))
)
#endif

#endif
/*!
* Creates a new instance of a GBL_OBJECT_TYPE compatible type, setting
* the values of the given properties upon construction.
* \relatesalso GblObject
* \param type GBL_OBJECT_TYPE or derived type
* \param ... NULL-terminated list of [key, value] pairs, where keys
*            are of type const char* and value is the property type
* \returns pointer to a new GblObject
*/
GBL_EXPORT GblObject*         GblObject_new                 (GblType type, ...)                 GBL_NULL_TERMINATED             GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT         GblObject_newInPlace          (SELF, GblType type, ...)           GBL_NULL_TERMINATED             GBL_NOEXCEPT;
GBL_EXPORT GblObject*         GblObject_newWithClass        (GblObjectClass* pClass, ...)       GBL_NULL_TERMINATED             GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT         GblObject_newInPlaceWithClass (SELF, GblObjectClass* pClass, ...) GBL_NULL_TERMINATED             GBL_NOEXCEPT;

GBL_EXPORT GblObject*         GblObject_newVaList           (GblType type, va_list* pList)                                      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT         GblObject_newInPlaceVaList    (SELF, GblType type, va_list* pList)                                GBL_NOEXCEPT;
GBL_EXPORT GblObject*         GblObject_newVaListWithClass  (GblObjectClass* pClass, va_list* pList)                            GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT         GblObject_newInPlaceVaListWithClass
                                                            (SELF, GblObjectClass* pClass, va_list* pList)                      GBL_NOEXCEPT;

GBL_EXPORT GblObject*         GblObject_newVariants         (GblType type,
                                                             GblUint propertyCount,
                                                             const char* pNames[],
                                                             const GblVariant* pValues)                                         GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT         GblObject_newInPlaceVariants  (SELF,
                                                             GblType type,
                                                             GblUint propertyCount,
                                                             const char* pNames[],
                                                             const GblVariant* pValues)                                         GBL_NOEXCEPT;
GBL_EXPORT GblObject*         GblObject_newVariantsWithClass(GblObjectClass* pClass,
                                                             GblUint propertyCount,
                                                             const char* pNames[],
                                                             const GblVariant* pValues)                                         GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT         GblObject_newInPlaceVariantsWithClass
                                                            (SELF,
                                                             GblObjectClass* pClass,
                                                             GblUint propertyCount,
                                                             const char* pNames[],
                                                             const GblVariant* pValues)                                         GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT         GblObject_setAttribute        (SELF, GBL_OBJECT_ATTRIBUTE attrib, GblBool value)                  GBL_NOEXCEPT;
GBL_INLINE GblBool            GblObject_attribute           (CSELF, GBL_OBJECT_ATTRIBUTE attrib)                                GBL_NOEXCEPT;

GBL_API                       GblObject_get                 (CSELF, ...) GBL_NULL_TERMINATED                                    GBL_NOEXCEPT;
GBL_API                       GblObject_set                 (SELF, ...) GBL_NULL_TERMINATED                                     GBL_NOEXCEPT;

GBL_API                       GblObject_getValue            (CSELF, const char* pName, ...)                                     GBL_NOEXCEPT;
GBL_API                       GblObject_setValue            (SELF,  const char* pName, ...)                                     GBL_NOEXCEPT;

GBL_API                       GblObject_getVaList           (CSELF, va_list* pList)                                             GBL_NOEXCEPT;
GBL_API                       GblObject_setVaList           (SELF, va_list* pList)                                              GBL_NOEXCEPT;

GBL_API                       GblObject_getVariants         (CSELF,
                                                             GblUint propertyCount,
                                                             const char* pNames[],
                                                             GblVariant* pValues)                                               GBL_NOEXCEPT;
GBL_API                       GblObject_setVariants         (SELF,
                                                             GblUint propertyCount,
                                                             const char* pNames[],
                                                             const GblVariant* pValues)                                         GBL_NOEXCEPT;

GBL_INLINE const GblProperty* GblObject_propertyFindString  (CSELF, const char* pName)                                          GBL_NOEXCEPT;
GBL_INLINE const GblProperty* GblObject_propertyFindQuark   (CSELF, GblQuark name)                                              GBL_NOEXCEPT;
GBL_INLINE const GblProperty* GblObject_propertyNext        (CSELF, const GblProperty* pPrev, GblFlags mask)                    GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT         GblObject_propertyGet         (CSELF, const GblProperty* pProperty, GblVariant* pValue)           GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT         GblObject_propertySet         (SELF, const GblProperty* pProperty, const GblVariant* pValue)      GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT         GblObject_propertyGetQuark    (CSELF, GblQuark name, GblVariant* pValue)                          GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT         GblObject_propertySetQuark    (SELF,  GblQuark name, const GblVariant* pValue)                    GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT         GblObject_propertyGetString   (CSELF, const char* pName, GblVariant* pValue)                      GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT         GblObject_propertySetString   (SELF,  const char* pName, const GblVariant* pValue)                GBL_NOEXCEPT;

//GBL_INLINE GBL_RESULT         GblObject_propertyGetWithId   (CSELF, GblSize id, GblVariant* pValue)                             GBL_NOEXCEPT;
//GBL_INLINE GBL_RESULT         GblObject_propertySetWithId   (SELF, GblSize id, const GblVariant* pValue)                        GBL_NOEXCEPT;

//GBL_INLINE GblBool            GblObject_extFieldExists

//GBL_INLINE GBL_RESULT         GblObject_extFieldSetUserdata     (CSELF, GblQuark quark, void* pUd, GblArrayMapDestructFn pDtor)     GBL_NOEXCEPT;
//GBL_INLINE GBL_RESULT         GblObject_extFieldSetVariantCopy  (CSELF, GblQuark quark, GblVariant)
//remove me
GBL_EXPORT void*              GblObject_userdata            (CSELF)                                                             GBL_NOEXCEPT;
GBL_EXPORT void               GblObject_userdataSet         (SELF, void* pUserdata)                                             GBL_NOEXCEPT;

GBL_EXPORT GblObject*         GblObject_parent              (CSELF)                                                             GBL_NOEXCEPT;
GBL_EXPORT void               GblObject_parentSet           (SELF, GblObject* pParent)                                          GBL_NOEXCEPT;

GBL_EXPORT const char*        GblObject_name                (CSELF) GBL_NOEXCEPT;
GBL_EXPORT void               GblObject_nameSet             (SELF, const char* pName)                                           GBL_NOEXCEPT;

GBL_INLINE GblObject*         GblObject_ancestorFindByType  (CSELF, GblType ancestorType)                                       GBL_NOEXCEPT;
GBL_INLINE GblObject*         GblObject_ancestorFindByName  (CSELF, const char* pName)                                          GBL_NOEXCEPT;
GBL_EXPORT GblContext*        GblObject_contextFind         (SELF)                                                              GBL_NOEXCEPT;

GBL_EXPORT GblObject*         GblObject_childFirst          (CSELF)                                                             GBL_NOEXCEPT;
GBL_EXPORT void               GblObject_childAdd            (SELF, GblObject* pChild)                                           GBL_NOEXCEPT;
GBL_EXPORT GblBool            GblObject_childRemove         (SELF, GblObject* pChild)                                           GBL_NOEXCEPT;
GBL_EXPORT GblObject*         GblObject_childFindByType     (CSELF, GblType childType)                                          GBL_NOEXCEPT;
GBL_EXPORT GblObject*         GblObject_childFindByName     (CSELF, const char* pName)                                          GBL_NOEXCEPT;
GBL_EXPORT GblObject*         GblObject_childFindByIndex    (CSELF, GblSize index)                                              GBL_NOEXCEPT;
GBL_EXPORT GblSize            GblObject_childCount          (CSELF)                                                             GBL_NOEXCEPT;
//GBL_EXPORT GblBool          GblObject_childForEach        (CSELF, GblObjectChildIterFn pIterFn, void* pUserdata)              GBL_NOEXCEPT;

GBL_EXPORT GblObject*         GblObject_siblingNext         (CSELF)                                                             GBL_NOEXCEPT;
// half-assed
GBL_INLINE GblObject*         GblObject_siblingFindByType   (CSELF, GblType siblingType)                                        GBL_NOEXCEPT;
GBL_INLINE GblObject*         GblObject_siblingFindByName   (CSELF, const char* pName)                                          GBL_NOEXCEPT;
//GBL_INLINE GblObject*         GblObject_siblingFindByIndex  (CSELF, GblSize index) GBL_NOEXCEPT;
//GBL_INLINE Gblobject*         GblObject_siblingCount()

GBL_API                       GblObject_eventSend           (SELF, GblEvent* pEvent) GBL_NOEXCEPT;
GBL_API                       GblObject_eventNotify         (SELF, GblEvent* pEvent) GBL_NOEXCEPT;

GBL_API                       GblObject_eventFilterInstall  (SELF, GblIEventFilter* pFilter)                                    GBL_NOEXCEPT;
GBL_API                       GblObject_eventFilterUninstall(SELF, GblIEventFilter* pFilter)                                    GBL_NOEXCEPT;
GblIEventFilter*              GblObject_eventFilterAt       (CSELF, GblSize index)                                              GBL_NOEXCEPT;
GblSize                       GblObject_eventFilterCount    (CSELF)                                                             GBL_NOEXCEPT;


//GBL_API                 GblObject_load(SELF, GblIStructuredReader* pReader) GBL_NOEXCEPT;
//GBL_API                 GblObject_save(SELF, GblIStructuredWriter* pWriter) GBL_NOEXCEPT;

// ============ IMPL ===============

GBL_INLINE const GblProperty* GblObject_propertyFindString(CSELF, const char* pName) GBL_NOEXCEPT {
    return gblPropertyTableFind(GBL_INSTANCE_TYPEOF(pSelf), GblQuark_fromString(pName));
}
GBL_INLINE const GblProperty* GblObject_propertyFindQuark(CSELF, GblQuark name) GBL_NOEXCEPT {
    return gblPropertyTableFind(GBL_INSTANCE_TYPEOF(pSelf), name);
}
GBL_INLINE const GblProperty* GblObject_propertyNext(CSELF, const GblProperty* pPrev, GblFlags mask) GBL_NOEXCEPT {
    return gblPropertyTableNext(GBL_INSTANCE_TYPEOF(pSelf), pPrev, mask);
}
GBL_INLINE GBL_RESULT GblObject_propertySet(SELF, const GblProperty* pProperty, const GblVariant* pValue) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GblClass* pClass = GblClass_weakRefDefault(GblProperty_objectType(pProperty));
    GBL_API_VERIFY_EXPRESSION(pClass);
    GBL_API_CALL(GBL_OBJECT_CLASS(pClass)->pFnPropertySet(pSelf, GblProperty_id(pProperty), pValue, pProperty));
    GBL_API_END();
}
GBL_INLINE GBL_RESULT GblObject_propertyGet(CSELF, const GblProperty* pProperty, GblVariant* pValue) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GblClass* pClass = GblClass_weakRefDefault(GblProperty_objectType(pProperty));
    GBL_API_VERIFY_EXPRESSION(pClass);
    GBL_API_CALL(GBL_OBJECT_CLASS(pClass)->pFnPropertyGet(pSelf, GblProperty_id(pProperty), pValue, pProperty));
    GBL_API_END();
}
GBL_INLINE GBL_RESULT GblObject_propertyGetQuark(CSELF, GblQuark name, GblVariant* pValue) GBL_NOEXCEPT {
    return GblObject_propertyGet(pSelf, GblObject_propertyFindQuark(pSelf, name), pValue);
}
GBL_INLINE GBL_RESULT GblObject_propertySetQuark(SELF,  GblQuark name, const GblVariant* pValue) GBL_NOEXCEPT {
    return GblObject_propertySet(pSelf, GblObject_propertyFindQuark(pSelf, name), pValue);
}
GBL_INLINE GBL_RESULT GblObject_propertyGetString(CSELF, const char* pName, GblVariant* pValue) GBL_NOEXCEPT {
    return GblObject_propertyGetQuark(pSelf, GblQuark_fromString(pName), pValue);
}
GBL_INLINE GBL_RESULT GblObject_propertySetString(SELF,  const char* pName, const GblVariant* pValue) GBL_NOEXCEPT {
        return GblObject_propertySetQuark(pSelf, GblQuark_fromString(pName), pValue);
}

GBL_INLINE GblObject* GblObject_ancestorFindByType(CSELF, GblType ancestorType) GBL_NOEXCEPT {
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

GBL_INLINE GblObject* GblObject_ancestorFindByName(CSELF, const char* pName) GBL_NOEXCEPT {
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


GBL_INLINE GblObject* GblObject_siblingFindByType(CSELF, GblType siblingType) GBL_NOEXCEPT {
    GblObject* pObject = NULL;
    if(GblObject_parent(pSelf)) {
        GblObject_childFindByType(GblObject_parent(pSelf), siblingType);
    }
    return pObject;
}

GBL_INLINE GblObject* GblObject_siblingFindByName(CSELF, const char* pName) GBL_NOEXCEPT {
    GblObject* pObject = NULL;
    GblObject* pParent = GblObject_parent(pSelf);
    if(pParent) {
        pObject = GblObject_childFindByName(pParent, pName);
    }
    return pObject;
}

GBL_INLINE GBL_RESULT GblObject_setAttribute(SELF, GBL_OBJECT_ATTRIBUTE attrib, GblBool value) GBL_NOEXCEPT {
    GBL_UNUSED(value, pSelf);
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_ARG(attrib < GBL_OBJECT_ATTRIBUTE_COUNT);
    switch(attrib) {
    default:
        GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_ARG, "[GblObject]: Failed to set invalid attribute: %u", attrib);
        break;
    }
    GBL_API_END();
}

GBL_INLINE GblBool GblObject_attribute(CSELF, GBL_OBJECT_ATTRIBUTE attrib) GBL_NOEXCEPT {
    GblBool value = GBL_FALSE;
    GBL_API_BEGIN(NULL);
    switch(attrib) {
    case GBL_OBJECT_ATTRIBUTE_CONSTRUCTED_IN_PLACE:
        value = GBL_PRIV(pSelf->base).constructedInPlace;
        break;
    default:
        GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_ARG, "[GblObject]: Failed to get invalid attribute: %u", attrib);
        break;
    }
    GBL_API_END_BLOCK();
    return value;
}


GBL_DECLS_END

#undef CSELF
#undef SELF

#endif // GIMBAL_OBJECT_H

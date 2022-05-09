#ifndef GIMBAL_OBJECT_H
#define GIMBAL_OBJECT_H

#include "../core/gimbal_api_frame.h"
#include "../meta/gimbal_instance.h"
#include "../ifaces/gimbal_ivariant.h"
#include "../ifaces/gimbal_itable.h"
#include "../ifaces/gimbal_ievent_handler.h"
#include "../ifaces/gimbal_ievent_filter.h"
#include "gimbal_property.h"
#include "../types/gimbal_quark.h"
#include "../containers/gimbal_list.h"

#define GBL_OBJECT_TYPE                     (GBL_BUILTIN_TYPE(OBJECT))
#define GBL_OBJECT_PARENT_PREFIX            GBL_INVALID
#define GBL_OBJECT_STRUCT                   GblObject
#define GBL_OBJECT_CLASS_STRUCT             GblObjectClass

#define GBL_OBJECT(instance)                GBL_INSTANCE_CAST_PREFIX(instance, GBL_OBJECT)
#define GBL_OBJECT_CHECK(instance)          GBL_INSTANCE_CHECK_PREFIX(instance, GBL_OBJECT)
#define GBL_OBJECT_TRY(instance)            GBL_INSTANCE_TRY_PREFIX(instance, GBL_OBJECT)

#define GBL_OBJECT_CLASS(klass)             GBL_CLASS_CAST_PREFIX(klass, GBL_OBJECT)
#define GBL_OBJECT_CLASS_CHECK(klass)       GBL_CLASS_CHECK_PREFIX(klass, GBL_OBJECT)
#define GBL_OBJECT_CLASS_TRY(klass)         GBL_CLASS_TRY_PREFIX(klass, GBL_OBJECT)

#define GBL_OBJECT_GET_CLASS(instance)      GBL_INSTANCE_CAST_CLASS_PREFIX(instance, GBL_OBJECT)
#define GBL_OBJECT_TRY_CLASS(instance)      GBL_INSTANCE_TRY_CLASS_PREFIX(instance, GBL_OBJECT)

#define GBL_OBJECT_REF(object)              (GblObject_ref((GblObject*)object))
#define GBL_OBJECT_UNREF(object)            (GblObject_unref((GblObject*)object))

#define SELF                GblObject* pSelf
#define CSELF               const SELF

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblObject);
GBL_FORWARD_DECLARE_STRUCT(GblEvent);

GBL_FORWARD_DECLARE_STRUCT(GblSlot);
GBL_FORWARD_DECLARE_STRUCT(GblVector);
GBL_FORWARD_DECLARE_STRUCT(GblObjectExtraData_);

typedef struct GblObjectClass {
    GblClass                base;
    GblIVariantIFace        iVariantIFace;
    GblITableIFace          iTableIFace;
    GblIEventHandlerIFace   iEventHandlerIFace;
    GblIEventFilterIFace    iEventFilterIFace;

    GBL_RESULT (*pFnConstructor)        (SELF);
    GBL_RESULT (*pFnDestructor)         (SELF);
    GBL_RESULT (*pFnConstructed)        (SELF);

    GBL_RESULT (*pFnPropertyGet)        (CSELF, GblSize slot, GblVariant* pValue, const GblProperty* pProp);
    GBL_RESULT (*pFnPropertySet)        (SELF,  GblSize slot, const GblVariant* pValue, const GblProperty* pProp);
    GBL_RESULT (*pFnSlotCall)           (SELF,  GblSize slot, GblVariant* pRet, GblUint argc, GblVariant* pArgs, const GblSlot* pSlot);
    GBL_RESULT (*pFnPropertyNotify)     (SELF,  GblProperty* pProperty);    // signal
} GblObjectClass;

GBL_DECLARE_ENUM(GBL_OBJECT_ATTRIBUTE) {
    GBL_OBJECT_ATTRIBUTE_CONSTRUCTED_IN_PLACE,
    GBL_OBJECT_ATTRIBUTE_CLASS_OWNED,
    GBL_OBJECT_ATTRIBUTE_CLASS_CONSTRUCTED_IN_PLACE,
    GBL_OBJECT_ATTRIBUTE_COUNT
};

typedef struct GblObject {
    union {
        GblObjectClass*     pClass;
        GblInstance         base;
    };
    GblRefCount             refCounter; // should be atomic

    uint16_t                contextType                            : 1;
    uint16_t                classOwned                             : 1;
    uint16_t                classConstructedInPlace                : 1;
    uint16_t                constructedInPlace                     : 1;
    uint16_t                childEventsSend                        : 1;
    uint16_t                childEventsReceive                     : 1;
    uint16_t                parentITableIndexFallthrough           : 1;
    uint16_t                parentITableNewIndexFallthrough        : 1;

    GblObjectExtraData_*    pExtraData;
} GblObject;

GBL_EXPORT GblObject*         GblObject_new                 (GblType type, ...)                 GBL_NULL_TERMINATED             GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT         GblObject_newInPlace          (SELF, GblType type, ...)           GBL_NULL_TERMINATED             GBL_NOEXCEPT;
GBL_EXPORT GblObject*         GblObject_newWithClass        (GblObjectClass* pClass, ...)       GBL_NULL_TERMINATED             GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT         GblObject_newInPlaceWithClass (SELF, GblObjectClass* pClass, ...) GBL_NULL_TERMINATED             GBL_NOEXCEPT;

GBL_EXPORT GblObject*         GblObject_newVaList           (GblType type, va_list* pList)                                      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT         GblObject_newInPlaceVaList    (SELF, GblType type, va_list* pList)                                GBL_NOEXCEPT;
GBL_EXPORT GblObject*         GblObject_newVaListWithClass  (GblObjectClass* pClass, va_list* pList)                            GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT         GblObject_newInPlaceVaListWithClass(SELF, GblObjectClass* pClass, va_list* pList)                       GBL_NOEXCEPT;

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
GBL_EXPORT GBL_RESULT         GblObject_newInPlaceVariantsWithClass(SELF,
                                                                    GblObjectClass* pClass,
                                                                    GblUint propertyCount,
                                                                    const char* pNames[],
                                                                    const GblVariant* pValues)                                  GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT         GblObject_attributeSet        (SELF, GBL_OBJECT_ATTRIBUTE attrib, GblBool value)                  GBL_NOEXCEPT;
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

GBL_EXPORT GblObject*         GblObject_ref                 (GblObject* pObject)                                                GBL_NOEXCEPT;
GBL_EXPORT GblRefCount        GblObject_unref               (GblObject* pObject)                                                GBL_NOEXCEPT;
GBL_EXPORT GblRefCount        GblObject_refCount            (const GblObject* pObject)                                          GBL_NOEXCEPT;

GBL_INLINE const GblProperty* GblObject_propertyFindString  (CSELF, const char* pName)                                          GBL_NOEXCEPT;
GBL_INLINE const GblProperty* GblObject_propertyFindQuark   (CSELF, GblQuark name)                                              GBL_NOEXCEPT;
GBL_INLINE const GblProperty* GblObject_propertyNext        (CSELF, const GblProperty* pPrev, GBL_PROPERTY_FLAGS mask)          GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT         GblObject_propertyGet         (CSELF, const GblProperty* pProperty, GblVariant* pValue)           GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT         GblObject_propertySet         (SELF, const GblProperty* pProperty, const GblVariant* pValue)      GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT         GblObject_propertyGetQuark    (CSELF, GblQuark name, GblVariant* pValue)                          GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT         GblObject_propertySetQuark    (SELF,  GblQuark name, const GblVariant* pValue)                    GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT         GblObject_propertyGetString   (CSELF, const char* pName, GblVariant* pValue)                      GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT         GblObject_propertySetString   (SELF,  const char* pName, const GblVariant* pValue)                GBL_NOEXCEPT;

GBL_INLINE void*              GblObject_userdata            (CSELF)                                                             GBL_NOEXCEPT;
GBL_INLINE void               GblObject_userdataSet         (SELF, void* pUserdata)                                             GBL_NOEXCEPT;

GBL_INLINE GblObject*         GblObject_parent              (CSELF)                                                             GBL_NOEXCEPT;
GBL_INLINE void               GblObject_parentSet           (SELF, GblObject* pParent)                                          GBL_NOEXCEPT;

GBL_INLINE const char*        GblObject_name                (CSELF) GBL_NOEXCEPT;
GBL_INLINE void               GblObject_nameSet             (SELF, const char* pName)                                           GBL_NOEXCEPT;

GBL_INLINE GblObject*         GblObject_ancestorFindByType  (CSELF, GblType ancestorType)                                       GBL_NOEXCEPT;
GBL_INLINE GblObject*         GblObject_ancestorFindByName  (CSELF, const char* pName)                                          GBL_NOEXCEPT;
GBL_EXPORT GblContext*        GblObject_contextFind         (SELF)                                                              GBL_NOEXCEPT;

GBL_INLINE GblObject*         GblObject_childFirst          (CSELF)                                                             GBL_NOEXCEPT;
GBL_INLINE void               GblObject_childAdd            (SELF, GblObject* pChild)                                           GBL_NOEXCEPT;
GBL_INLINE GblBool            GblObject_childRemove         (SELF, GblObject* pChild)                                           GBL_NOEXCEPT;
GBL_INLINE GblObject*         GblObject_childFindByType     (CSELF, GblType childType)                                          GBL_NOEXCEPT;
GBL_INLINE GblObject*         GblObject_childFindByName     (CSELF, const char* pName)                                          GBL_NOEXCEPT;
GBL_INLINE GblObject*         GblObject_childFindByIndex    (CSELF, GblSize index)                                              GBL_NOEXCEPT;
GBL_INLINE GblSize            GblObject_childCount          (CSELF)                                                             GBL_NOEXCEPT;

GBL_INLINE GblObject*         GblObject_siblingNext         (CSELF)                                                             GBL_NOEXCEPT;
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







// ============ INLINEZ ===============

typedef struct GblObjectExtraData_ {
    struct GblObject*       pParent;
    struct GblObject*       pSiblingNext;
    struct GblObject*       pChildFirst;
    void*                   pUserdata;
    GblQuark                name;
    GblVector*              pEventFilters;
} GblObjectExtraData_;

GBL_INLINE GBL_RESULT GblObject_ensureExtraData_(SELF) GBL_NOEXCEPT {
    GBL_API_BEGIN(GblType_context(GBL_INSTANCE_TYPE(pSelf)));
    if(!pSelf->pExtraData) {
        pSelf->pExtraData = (GblObjectExtraData_*)GBL_API_MALLOC(sizeof(GblObjectExtraData_));
        memset(pSelf->pExtraData, 0, sizeof(GblObjectExtraData_));
    }
    GBL_API_END();
}

GBL_INLINE const GblProperty* GblObject_propertyFindString(CSELF, const char* pName) GBL_NOEXCEPT {
    return gblPropertyTableFind(GBL_INSTANCE_TYPE(pSelf), GblQuark_fromString(pName));
}
GBL_INLINE const GblProperty* GblObject_propertyFindQuark(CSELF, GblQuark name) GBL_NOEXCEPT {
    return gblPropertyTableFind(GBL_INSTANCE_TYPE(pSelf), name);
}
GBL_INLINE const GblProperty* GblObject_propertyNext(CSELF, const GblProperty* pPrev, GBL_PROPERTY_FLAGS mask) GBL_NOEXCEPT {
    return gblPropertyTableNext(GBL_INSTANCE_TYPE(pSelf), pPrev, mask);
}
GBL_INLINE GBL_RESULT GblObject_propertySet(SELF, const GblProperty* pProperty, const GblVariant* pValue) GBL_NOEXCEPT {
    GBL_API_BEGIN(GblType_context(GBL_INSTANCE_TYPE(pSelf)));
    GblClass* pClass = GblClass_peekFromType(GblProperty_objectType(pProperty));
    GBL_API_VERIFY_EXPRESSION(pClass);
    GBL_API_CALL(GBL_OBJECT_CLASS(pClass)->pFnPropertySet(pSelf, GblProperty_id(pProperty), pValue, pProperty));
    GBL_API_END();
}
GBL_INLINE GBL_RESULT GblObject_propertyGet(CSELF, const GblProperty* pProperty, GblVariant* pValue) GBL_NOEXCEPT {
    GBL_API_BEGIN(GblType_context(GBL_INSTANCE_TYPE(pSelf)));
    GblClass* pClass = GblClass_peekFromType(GblProperty_objectType(pProperty));
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

GBL_INLINE GblObject* GblObject_parent(CSELF) GBL_NOEXCEPT {
    return pSelf->pExtraData ? pSelf->pExtraData->pParent : NULL;
}

GBL_INLINE void GblObject_parentSet(SELF, GblObject* pParent) GBL_NOEXCEPT {
    GblObject* pOldParent = GblObject_parent(pSelf);
    if(pOldParent) {
        GblObject_childRemove(pOldParent, pSelf);
    }

    if(pParent) GblObject_childAdd(pParent, pSelf);
}

GBL_INLINE GblObject* GblObject_childFirst(CSELF) GBL_NOEXCEPT {
    return pSelf->pExtraData? pSelf->pExtraData->pChildFirst : NULL;
}
GBL_INLINE GblObject*  GblObject_siblingNext(CSELF) GBL_NOEXCEPT {
    return pSelf->pExtraData? pSelf->pExtraData->pSiblingNext : NULL;
}

GBL_INLINE GblObject* GblObject_ancestorFindByType(CSELF, GblType ancestorType) GBL_NOEXCEPT {
    GblObject* pAncestor = NULL;
    //GBL_API_BEGIN(GblType_context(GBL_INSTANCE_TYPE(pSelf))); {
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
    GBL_API_BEGIN(GblType_context(GBL_INSTANCE_TYPE(pSelf))); {
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


GBL_INLINE void GblObject_childAdd(SELF, GblObject* pChild) GBL_NOEXCEPT {
    GBL_API_BEGIN(GblType_context(GBL_INSTANCE_TYPE(pSelf)));
    GBL_API_VERIFY_POINTER(pChild);
    {
        GblObject* pNode = GblObject_childFirst(pSelf);
        if(!pNode) {
            GblObject_ensureExtraData_(pSelf);
            pSelf->pExtraData->pChildFirst = pChild;
        } else {
            GblObject* pSibling = NULL;
            while((pSibling = GblObject_siblingNext(pNode))) {
                GBL_API_VERIFY_EXPRESSION(pSibling != pChild);
                pNode = pSibling;
            };
            pNode->pExtraData->pSiblingNext = pChild;
        }
        GblObject_ensureExtraData_(pChild);
        pChild->pExtraData->pParent = pSelf;
        pChild->pExtraData->pSiblingNext = NULL;
    }
    GBL_API_END_BLOCK();
}

GBL_INLINE GblBool GblObject_childRemove(SELF, GblObject* pChild) GBL_NOEXCEPT {
    GblBool success = GBL_FALSE;
    GBL_API_BEGIN(GblType_context(GBL_INSTANCE_TYPE(pSelf)));
    GBL_API_VERIFY_POINTER(pChild);
    GBL_API_VERIFY_POINTER(pSelf->pExtraData && pSelf->pExtraData->pChildFirst);
    {
        GblObject* pNode = GblObject_childFirst(pSelf);
        if(pNode == pChild) {
            pSelf->pExtraData->pChildFirst = pChild->pExtraData->pSiblingNext;
            success = GBL_TRUE;
        } else {
            while(pNode->pExtraData->pSiblingNext) {
                if(pNode->pExtraData->pSiblingNext == pChild) {
                    pNode->pExtraData->pSiblingNext = pChild->pExtraData->pSiblingNext;
                    success = GBL_TRUE;
                    break;
                }
                pNode = GblObject_siblingNext(pNode);
            }
        }
        if(success) {
            pChild->pExtraData->pSiblingNext = NULL;
            pChild->pExtraData->pParent = NULL;
        }
    }
    GBL_API_END_BLOCK();
    return success;
}

GBL_INLINE GblObject* GblObject_childFindByType(CSELF, GblType childType) GBL_NOEXCEPT {
    GblObject* pChild = NULL;
    GBL_API_BEGIN(GblType_context(GBL_INSTANCE_TYPE(pSelf))); {
        for(GblObject* pIt = GblObject_childFirst(pSelf);
            pIt != NULL;
            pIt = GblObject_siblingNext(pIt))
        {
            if(GBL_INSTANCE_CHECK(pIt, childType)) {
                pChild = pIt;
                break;
            }
        }
    } GBL_API_END_BLOCK();
    return pChild;
}

GBL_INLINE GblObject* GblObject_childFindByName(CSELF, const char* pName) GBL_NOEXCEPT {
    GblObject* pChild = NULL;
    GBL_API_BEGIN(GblType_context(GBL_INSTANCE_TYPE(pSelf))); {
        for(GblObject* pIt = GblObject_childFirst(pSelf);
            pIt != NULL;
            pIt = GblObject_siblingNext(pIt))
        {
            const char* pNodeName = GblObject_name(pIt);
            if(pNodeName && strcmp(pNodeName, pName) == 0) {
                pChild = pIt;
                break;
            }
        }
    } GBL_API_END_BLOCK();
    return pChild;
}

GBL_INLINE GblObject* GblObject_childFindByIndex(CSELF, GblSize index) GBL_NOEXCEPT {
    GblObject* pChild = NULL;
    GBL_API_BEGIN(GblType_context(GBL_INSTANCE_TYPE(pSelf)));\
    GblSize count = 0;
    for(GblObject* pObj = GblObject_childFirst(GBL_OBJECT(pSelf));
        pObj != NULL;
        pObj = GblObject_siblingNext(GBL_OBJECT(pObj)))
    {
        if(count++ == index) {
            pChild = pObj;
            break;
        }
    }
    GBL_API_END_BLOCK();
    return pChild;
}

GBL_INLINE GblSize GblObject_childCount(CSELF) GBL_NOEXCEPT {
    GblSize count = 0;
    GBL_API_BEGIN(GblType_context(GBL_INSTANCE_TYPE(pSelf)));
    for(GblObject* pObj = GblObject_childFirst(GBL_OBJECT(pSelf));
        pObj != NULL;
        pObj = GblObject_siblingNext(GBL_OBJECT(pObj)))
    {
        ++count;
    }
    GBL_API_END_BLOCK();
    return count;
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


GBL_INLINE void GblObject_nameSet(SELF, const char* pName) GBL_NOEXCEPT {
    if(pName) {
        GblObject_ensureExtraData_(pSelf);
        pSelf->pExtraData->name = GblQuark_fromString(pName);
    } else if(pSelf->pExtraData) {
        pSelf->pExtraData->name = GBL_QUARK_INVALID;
    }
}
GBL_INLINE const char* GblObject_name(CSELF) GBL_NOEXCEPT {
    return pSelf->pExtraData? GblQuark_toString(pSelf->pExtraData->name) : NULL;
}

GBL_INLINE void* GblObject_userdata(CSELF) GBL_NOEXCEPT {
    return pSelf->pExtraData? pSelf->pExtraData->pUserdata : NULL;
}


GBL_INLINE void GblObject_userdataSet(SELF, void* pUserdata) GBL_NOEXCEPT {
    if(pUserdata) {
        GblObject_ensureExtraData_(pSelf);
        pSelf->pExtraData->pUserdata = pUserdata;
    } else if(pSelf->pExtraData) {
        pSelf->pExtraData->pUserdata = NULL;
    }
}

GBL_INLINE GBL_RESULT GblObject_attributeSet(SELF, GBL_OBJECT_ATTRIBUTE attrib, GblBool value) GBL_NOEXCEPT {
    GBL_API_BEGIN(GblType_context(GBL_INSTANCE_TYPE(pSelf)));
    GBL_API_VERIFY_ARG(attrib < GBL_OBJECT_ATTRIBUTE_COUNT);
    switch(attrib) {
    case GBL_OBJECT_ATTRIBUTE_CLASS_CONSTRUCTED_IN_PLACE:
        pSelf->classConstructedInPlace = value;
        break;
    case GBL_OBJECT_ATTRIBUTE_CLASS_OWNED:
        pSelf->classOwned = value;
        break;
    default:
        GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_ARG, "[GblObject]: Failed to set invalid attribute: %u", attrib);
        break;
    }
    GBL_API_END();
}

GBL_INLINE GblBool GblObject_attribute(CSELF, GBL_OBJECT_ATTRIBUTE attrib) GBL_NOEXCEPT {
    GblBool value = GBL_FALSE;
    GBL_API_BEGIN(GblType_context(GBL_INSTANCE_TYPE(pSelf)));
    switch(attrib) {
    case GBL_OBJECT_ATTRIBUTE_CONSTRUCTED_IN_PLACE:
        value = pSelf->constructedInPlace;
        break;
    case GBL_OBJECT_ATTRIBUTE_CLASS_CONSTRUCTED_IN_PLACE:
        value = pSelf->classConstructedInPlace;
        break;
    case GBL_OBJECT_ATTRIBUTE_CLASS_OWNED:
        value = pSelf->classOwned;
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

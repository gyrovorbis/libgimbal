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
#define GBL_OBJECT_TYPE                 (GBL_BUILTIN_TYPE(OBJECT))

#define GBL_OBJECT(instance)            (GBL_INSTANCE_CAST(instance, GBL_OBJECT_TYPE, GblObject))
#define GBL_OBJECT_CHECK(instance)      (GBL_INSTANCE_CHECK(instance, GBL_OBJECT_TYPE))
#define GBL_OBJECT_TRY(instance)        (GBL_INSTANCE_TRY(instance, GBL_OBJECT_TYPE, GblObject))

#define GBL_OBJECT_CLASS(klass)         (GBL_CLASS_CAST(klass, GBL_OBJECT_TYPE, GblObjectClass))
#define GBL_OBJECT_CLASS_CHECK(klass)   (GBL_CLASS_CHECK(klass, GBL_OBJECT_TYPE))
#define GBL_OBJECT_CLASS_TRY(klass)     (GBL_CLASS_TRY(klass, GBL_OBJECT_TYPE, GblObjectClass))

#define GBL_OBJECT_GET_CLASS(instance)  (GBL_INSTANCE_GET_CLASS(instance, GBL_OBJECT_TYPE, GblObjectClass))
#define GBL_OBJECT_TRY_CLASS(instance)  (GBL_INSTANCE_TRY_CLASS(instance, GBL_OBJECT_TYPE, GblObjectClass))

#define GBL_SELF GblObject* pSelf
#define GBL_CSELF const GBL_SELF

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblObject);
GBL_FORWARD_DECLARE_STRUCT(GblEvent);


GBL_CLASS_DERIVE(GblObjectClass, GblBoxClass,
                 GblITableIFace, GblIEventHandlerIFace, GblIEventFilterIFace)
    GBL_RESULT (*pFnConstructor)(GBL_SELF);
    GBL_RESULT (*pFnConstructed)(GBL_SELF);
    GBL_RESULT (*pFnPropertyGet)(GBL_CSELF, GblSize id, GblVariant* pValue, const GblProperty* pProp);
    GBL_RESULT (*pFnPropertySet)(GBL_SELF,  GblSize id, const GblVariant* pValue, const GblProperty* pProp);
GBL_CLASS_END

GBL_INSTANCE_DERIVE_EMPTY(GblObject, GblBox)

GBL_PROPERTIES(GblObject,
    (name,     GBL_GENERIC,  (ALL, CONSTRUCT),          GBL_POINTER_TYPE),
    (parent,   GBL_GENERIC,  (READ, WRITE, CONSTRUCT),  GBL_OBJECT_TYPE),
    (userdata, GBL_GENERIC,  (READ, WRITE, CONSTRUCT)),
    (refCount, GBL_GENERIC,  (READ),                    GBL_UINT16_TYPE)
)

GBL_EXPORT GblType     GblObject_type                (void)                              GBL_NOEXCEPT;

GBL_EXPORT GblObject*  GblObject_create              (GblType type, ...)                 GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblObject_construct           (GBL_SELF, GblType type, ...)           GBL_NOEXCEPT;
GBL_EXPORT GblObject*  GblObject_createWithClass     (GblObjectClass* pClass, ...)       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblObject_constructWithClass  (GBL_SELF, GblObjectClass* pClass, ...) GBL_NOEXCEPT;
GBL_EXPORT GblObject*  GblObject_createVaList        (GblType type, va_list* pList)      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblObject_constructVaList     (GBL_SELF,
                                                      GblType           type,
                                                      va_list*          pList)           GBL_NOEXCEPT;

GBL_EXPORT GblObject*  GblObject_createVaListWithClass
                                                     (GblObjectClass*   pClass,
                                                      va_list*          pList)           GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblObject_constructVaListWithClass
                                                     (GBL_SELF,
                                                      GblObjectClass*   pClass,
                                                      va_list*          pList)           GBL_NOEXCEPT;

GBL_EXPORT GblObject*  GblObject_createVariants      (GblType           type,
                                                      GblUint           propertyCount,
                                                      const char*       pNames[],
                                                      const GblVariant* pValues)         GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblObject_constructVariants   (GBL_SELF,
                                                      GblType           type,
                                                      GblUint           propertyCount,
                                                      const char*       pNames[],
                                                      const GblVariant* pValues)         GBL_NOEXCEPT;

GBL_EXPORT GblObject*  GblObject_createVariantsWithClass
                                                     (GblObjectClass*   pClass,
                                                      GblUint           propertyCount,
                                                      const char*       pNames[],
                                                      const GblVariant* pValues)         GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblObject_constructVariantsWithClass
                                                     (GBL_SELF,
                                                      GblObjectClass*   pClass,
                                                      GblUint           propertyCount,
                                                      const char*       pNames[],
                                                      const GblVariant* pValues)        GBL_NOEXCEPT;

GBL_EXPORT const char* GblObject_name                (GBL_CSELF)                            GBL_NOEXCEPT;
GBL_EXPORT void        GblObject_setName             (GBL_SELF, const char* pName)          GBL_NOEXCEPT;

GBL_EXPORT GblObject*  GblObject_parent              (GBL_CSELF)                            GBL_NOEXCEPT;
GBL_EXPORT void        GblObject_setParent           (GBL_SELF, GblObject* pParent)         GBL_NOEXCEPT;

GBL_EXPORT GblObject*  GblObject_childFirst          (GBL_CSELF)                            GBL_NOEXCEPT;
GBL_EXPORT GblSize     GblObject_childCount          (GBL_CSELF)                            GBL_NOEXCEPT;

GBL_EXPORT GblObject*  GblObject_siblingNext         (GBL_CSELF)                            GBL_NOEXCEPT;

GBL_EXPORT void        GblObject_addChild            (GBL_SELF, GblObject* pChild)          GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblObject_removeChild         (GBL_SELF, GblObject* pChild)          GBL_NOEXCEPT;

GBL_INLINE GblObject*  GblObject_findAncestorByType  (GBL_CSELF, GblType ancestorType)      GBL_NOEXCEPT;
GBL_INLINE GblObject*  GblObject_findAncestorByName  (GBL_CSELF, const char* pName)         GBL_NOEXCEPT;
GBL_EXPORT GblContext* GblObject_findContext         (GBL_SELF)                             GBL_NOEXCEPT;

GBL_EXPORT GblObject*  GblObject_findChildByType     (GBL_CSELF, GblType childType)         GBL_NOEXCEPT;
GBL_EXPORT GblObject*  GblObject_findChildByName     (GBL_CSELF, const char* pName)         GBL_NOEXCEPT;
GBL_EXPORT GblObject*  GblObject_findChildByIndex    (GBL_CSELF, GblSize index)             GBL_NOEXCEPT;

GBL_INLINE GblObject*  GblObject_findSiblingByType   (GBL_CSELF, GblType siblingType)       GBL_NOEXCEPT;
GBL_INLINE GblObject*  GblObject_findSiblingByName   (GBL_CSELF, const char* pName)         GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblObject_sendEvent           (GBL_SELF, GblEvent* pEvent)           GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblObject_notifyEvent         (GBL_SELF, GblEvent* pEvent)           GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblObject_installEventFilter  (GBL_SELF, GblIEventFilter* pFilter)   GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblObject_uninstallEventFilter(GBL_SELF, GblIEventFilter* pFilter)   GBL_NOEXCEPT;

GBL_EXPORT GblSize     GblObject_eventFilterCount    (GBL_CSELF)                            GBL_NOEXCEPT;
GBL_EXPORT GblIEventFilter*
                       GblObject_eventFilterAt       (GBL_CSELF, GblSize index)             GBL_NOEXCEPT;

//==============

GBL_EXPORT GBL_RESULT  GblObject_readProperty        (GBL_CSELF, const char* pName, ...)    GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblObject_writeProperty       (GBL_SELF, const char* pName, ...)     GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblObject_readPropertyVaList  (GBL_CSELF,
                                                      const char* pName,
                                                      va_list*    pVaList)              GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblObject_writePropertyVaList (GBL_SELF,
                                                      const char* pName,
                                                      va_list     pVaList)              GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblObject_readPropertyVariant (GBL_CSELF,
                                                      const char* pName,
                                                      GblVariant* pValue)               GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblObject_writePropertyVariant(GBL_SELF,
                                                      const char* pName,
                                                      GblVariant* pValue)               GBL_NOEXCEPT;

//================



GBL_API                       GblObject_get                 (GBL_CSELF, ...) GBL_NULL_TERMINATED                                    GBL_NOEXCEPT;
GBL_API                       GblObject_set                 (GBL_SELF, ...) GBL_NULL_TERMINATED                                     GBL_NOEXCEPT;

GBL_API                       GblObject_getValue            (GBL_CSELF, const char* pName, ...)                                     GBL_NOEXCEPT;
GBL_API                       GblObject_setValue            (GBL_SELF,  const char* pName, ...)                                     GBL_NOEXCEPT;

GBL_API                       GblObject_getVaList           (GBL_CSELF, va_list* pList)                                             GBL_NOEXCEPT;
GBL_API                       GblObject_setVaList           (GBL_SELF, va_list* pList)                                              GBL_NOEXCEPT;

GBL_API                       GblObject_getVariants         (GBL_CSELF,
                                                             GblUint propertyCount,
                                                             const char* pNames[],
                                                             GblVariant* pValues)                                               GBL_NOEXCEPT;
GBL_API                       GblObject_setVariants         (GBL_SELF,
                                                             GblUint propertyCount,
                                                             const char* pNames[],
                                                             const GblVariant* pValues)                                         GBL_NOEXCEPT;

GBL_INLINE const GblProperty* GblObject_propertyFindString  (GBL_CSELF, const char* pName)                                          GBL_NOEXCEPT;
GBL_INLINE const GblProperty* GblObject_propertyFindQuark   (GBL_CSELF, GblQuark name)                                              GBL_NOEXCEPT;
GBL_INLINE const GblProperty* GblObject_propertyNext        (GBL_CSELF, const GblProperty* pPrev, GblFlags mask)                    GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT         GblObject_propertyGet         (GBL_CSELF, const GblProperty* pProperty, GblVariant* pValue)           GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT         GblObject_propertySet         (GBL_SELF, const GblProperty* pProperty, const GblVariant* pValue)      GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT         GblObject_propertyGetQuark    (GBL_CSELF, GblQuark name, GblVariant* pValue)                          GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT         GblObject_propertySetQuark    (GBL_SELF,  GblQuark name, const GblVariant* pValue)                    GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT         GblObject_propertyGetString   (GBL_CSELF, const char* pName, GblVariant* pValue)                      GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT         GblObject_propertySetString   (GBL_SELF,  const char* pName, const GblVariant* pValue)                GBL_NOEXCEPT;

//GBL_INLINE GBL_RESULT         GblObject_propertyGetWithId   (GBL_CSELF, GblSize id, GblVariant* pValue)                             GBL_NOEXCEPT;
//GBL_INLINE GBL_RESULT         GblObject_propertySetWithId   (GBL_SELF, GblSize id, const GblVariant* pValue)                        GBL_NOEXCEPT;

//GBL_INLINE GblBool            GblObject_extFieldExists

//GBL_INLINE GBL_RESULT         GblObject_extFieldSetUserdata     (GBL_CSELF, GblQuark quark, void* pUd, GblArrayMapDestructFn pDtor)     GBL_NOEXCEPT;
//GBL_INLINE GBL_RESULT         GblObject_extFieldSetVariantCopy  (GBL_CSELF, GblQuark quark, GblVariant)
//remove me


// ============ IMPL ===============

GBL_INLINE const GblProperty* GblObject_propertyFindString(GBL_CSELF, const char* pName) GBL_NOEXCEPT {
    return GblProperty_find(GBL_INSTANCE_TYPEOF(pSelf), GblQuark_fromString(pName));
}
GBL_INLINE const GblProperty* GblObject_propertyFindQuark(GBL_CSELF, GblQuark name) GBL_NOEXCEPT {
    return GblProperty_find(GBL_INSTANCE_TYPEOF(pSelf), name);
}
GBL_INLINE const GblProperty* GblObject_propertyNext(GBL_CSELF, const GblProperty* pPrev, GblFlags mask) GBL_NOEXCEPT {
    return GblProperty_next(GBL_INSTANCE_TYPEOF(pSelf), pPrev, mask);
}
GBL_INLINE GBL_RESULT GblObject_propertySet(GBL_SELF, const GblProperty* pProperty, const GblVariant* pValue) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GblClass* pClass = GblClass_weakRefDefault(GblProperty_objectType(pProperty));
    GBL_API_VERIFY_EXPRESSION(pClass);
    GBL_API_CALL(GBL_OBJECT_CLASS(pClass)->pFnPropertySet(pSelf, pProperty->id, pValue, pProperty));
    GBL_API_END();
}
GBL_INLINE GBL_RESULT GblObject_propertyGet(GBL_CSELF, const GblProperty* pProperty, GblVariant* pValue) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GblClass* pClass = GblClass_weakRefDefault(GblProperty_objectType(pProperty));
    GBL_API_VERIFY_EXPRESSION(pClass);
    GBL_API_CALL(GBL_OBJECT_CLASS(pClass)->pFnPropertyGet(pSelf, pProperty->id, pValue, pProperty));
    GBL_API_END();
}
GBL_INLINE GBL_RESULT GblObject_propertyGetQuark(GBL_CSELF, GblQuark name, GblVariant* pValue) GBL_NOEXCEPT {
    return GblObject_propertyGet(pSelf, GblObject_propertyFindQuark(pSelf, name), pValue);
}
GBL_INLINE GBL_RESULT GblObject_propertySetQuark(GBL_SELF,  GblQuark name, const GblVariant* pValue) GBL_NOEXCEPT {
    return GblObject_propertySet(pSelf, GblObject_propertyFindQuark(pSelf, name), pValue);
}
GBL_INLINE GBL_RESULT GblObject_propertyGetString(GBL_CSELF, const char* pName, GblVariant* pValue) GBL_NOEXCEPT {
    return GblObject_propertyGetQuark(pSelf, GblQuark_fromString(pName), pValue);
}
GBL_INLINE GBL_RESULT GblObject_propertySetString(GBL_SELF,  const char* pName, const GblVariant* pValue) GBL_NOEXCEPT {
        return GblObject_propertySetQuark(pSelf, GblQuark_fromString(pName), pValue);
}

GBL_INLINE GblObject* GblObject_findAncestorByType(GBL_CSELF, GblType ancestorType) GBL_NOEXCEPT {
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

GBL_INLINE GblObject* GblObject_findAncestorByName(GBL_CSELF, const char* pName) GBL_NOEXCEPT {
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


GBL_INLINE GblObject* GblObject_findSiblingByType(GBL_CSELF, GblType siblingType) GBL_NOEXCEPT {
    GblObject* pObject = NULL;
    if(GblObject_parent(pSelf)) {
        GblObject_findChildByType(GblObject_parent(pSelf), siblingType);
    }
    return pObject;
}

GBL_INLINE GblObject* GblObject_findSiblingByName(GBL_CSELF, const char* pName) GBL_NOEXCEPT {
    GblObject* pObject = NULL;
    GblObject* pParent = GblObject_parent(pSelf);
    if(pParent) {
        pObject = GblObject_findChildByName(pParent, pName);
    }
    return pObject;
}


GBL_DECLS_END

#undef GBL_CSELF
#undef GBL_SELF

#endif // GIMBAL_OBJECT_H

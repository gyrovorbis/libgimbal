/*! \file
 *  \brief GblObject structure and related functions
 *  \ingroup metaBuiltinTypes
 */

#ifndef GIMBAL_OBJECT_H
#define GIMBAL_OBJECT_H

#include "gimbal_box.h"
#include "../ifaces/gimbal_itable.h"
#include "../ifaces/gimbal_ievent_handler.h"
#include "../ifaces/gimbal_ievent_filter.h"
#include "../properties/gimbal_property.h"

/*! \ingroup metaBuiltinTypes
 * \details  ::GblType UUID of a GblObject
 */
#define GBL_OBJECT_TYPE                             (GBL_TYPEOF(GblObject))
#define GBL_OBJECT(instance)                        (GBL_INSTANCE_CAST(instance, GblObject))
#define GBL_OBJECT_CLASS(klass)                     (GBL_CLASS_CAST(klass, GblObject))
#define GBL_OBJECT_GET_CLASS(instance)              (GBL_INSTANCE_GET_CLASS(instance, GblObject))

#define GBL_OBJECT_NEW(cType, ...)                  ((cType*)GblObject_create(GBL_TYPEOF(cType), __VA_ARGS__, NULL))
#define GBL_OBJECT_CONTRUCT(cType, instance, ...)   (GblObject_construct(instance, GBL_TYPEOF(cType), __VA_ARGS__, NULL))

#define GBL_SELF_TYPE GblObject

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblObject);
GBL_FORWARD_DECLARE_STRUCT(GblProperty);

GBL_CLASS_DERIVE(GblObject, GblBox,
                 GblITable, GblIEventHandler, GblIEventFilter)
    GBL_RESULT (*pFnConstructor)(GBL_SELF);
    GBL_RESULT (*pFnConstructed)(GBL_SELF);
    GBL_RESULT (*pFnProperty)   (GBL_CSELF, const GblProperty* pProp, GblVariant* pValue);
    GBL_RESULT (*pFnSetProperty)(GBL_SELF, const GblProperty* pProp, GblVariant* pValue);
GBL_CLASS_END

GBL_INSTANCE_DERIVE_EMPTY(GblObject, GblBox)

GBL_PROPERTIES(GblObject,
    (name,     GBL_GENERIC,  (READ, WRITE, LOAD, SAVE), GBL_POINTER_TYPE),
    (parent,   GBL_GENERIC,  (READ, WRITE),             GBL_OBJECT_TYPE),
    (userdata, GBL_GENERIC,  (READ, WRITE),             GBL_POINTER_TYPE),
    (refCount, GBL_GENERIC,  (READ),                    GBL_UINT16_TYPE)
)

GBL_EXPORT GblType     GblObject_type                       (void)                                             GBL_NOEXCEPT;
GBL_EXPORT GblObject*  GblObject_create                     (GblType type, ...)                                GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblObject_construct                  (GBL_SELF, GblType type, ...)                      GBL_NOEXCEPT;
GBL_EXPORT GblObject*  GblObject_createWithClass            (GblObjectClass* pClass, ...)                      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblObject_constructWithClass         (GBL_SELF, GblObjectClass* pClass, ...)            GBL_NOEXCEPT;
GBL_EXPORT GblObject*  GblObject_createVaList               (GblType type, va_list* pList)                     GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblObject_constructVaList            (GBL_SELF, GblType  type, va_list* pList)          GBL_NOEXCEPT;
GBL_EXPORT GblObject*  GblObject_createVaListWithClass      (GblObjectClass* pClass, va_list* pList)           GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblObject_constructVaListWithClass   (GBL_SELF,
                                                             GblObjectClass*  pClass,
                                                             va_list*         pList)                           GBL_NOEXCEPT;

GBL_EXPORT GblObject*  GblObject_createVariants             (GblType         type,
                                                             GblSize         propertyCount,
                                                             const char*     pNames[],
                                                             GblVariant*     pValues)                          GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblObject_constructVariants          (GBL_SELF,
                                                             GblType         type,
                                                             GblSize         propertyCount,
                                                             const char*     pNames[],
                                                             GblVariant*     pValues)                          GBL_NOEXCEPT;

GBL_EXPORT GblObject*  GblObject_createVariantsWithClass    (GblObjectClass* pClass,
                                                             GblSize         propertyCount,
                                                             const char*     pNames[],
                                                             GblVariant*     pValues)                          GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblObject_constructVariantsWithClass (GBL_SELF,
                                                             GblObjectClass* pClass,
                                                             GblSize         propertyCount,
                                                             const char*     pNames[],
                                                             GblVariant*     pValues)                          GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblObject_property                   (GBL_CSELF, const char* pName, ...)                GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblObject_propertyByQuark            (GBL_CSELF, GblQuark name, ...)                    GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblObject_properties                 (GBL_CSELF, ...) GBL_NULL_TERMINATED               GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblObject_propertyVaList             (GBL_CSELF, const char* pName, va_list* pList)     GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblObject_propertyVaListByQuark      (GBL_CSELF, GblQuark name, va_list* pList)         GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblObject_propertiesVaList           (GBL_CSELF, va_list* pList)                        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblObject_propertyVariant            (GBL_CSELF, const char* pName, GblVariant* pValue) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblObject_propertyVariantByQuark     (GBL_CSELF, GblQuark name, GblVariant* pValue)     GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblObject_propertiesVariants         (GBL_CSELF,
                                                             GblSize     propertyCount,
                                                             const char* pNames[],
                                                             GblVariant* pValue)                               GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblObject_setProperty                (GBL_SELF, const char* pName, ...)                 GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblObject_setPropertyByQuark         (GBL_SELF, GblQuark quark, ...)                    GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblObject_setProperties              (GBL_SELF, ...)  GBL_NULL_TERMINATED               GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblObject_setPropertyVaList          (GBL_SELF, const char* pName, va_list* pList)      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblObject_setPropertyVaListByQuark   (GBL_SELF, GblQuark name, va_list* pList)          GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblObject_setPropertiesVaList        (GBL_SELF, va_list* pList)                         GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblObject_setPropertyVariant         (GBL_SELF, const char* pName, GblVariant* pValue)  GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblObject_setPropertyVariantByQuark  (GBL_SELF, GblQuark name, GblVariant* pValue)      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblObject_setPropertiesVariants      (GBL_SELF,
                                                             GblSize     propertyCount,
                                                             const char* pNames[],
                                                             GblVariant* pValue)                               GBL_NOEXCEPT;

GBL_EXPORT const char* GblObject_name                       (GBL_CSELF)                                        GBL_NOEXCEPT;
GBL_EXPORT void        GblObject_setName                    (GBL_SELF, const char* pName)                      GBL_NOEXCEPT;

GBL_EXPORT GblSize     GblObject_depth                      (GBL_CSELF)                                        GBL_NOEXCEPT;

GBL_EXPORT GblObject*  GblObject_parent                     (GBL_CSELF)                                        GBL_NOEXCEPT;
GBL_EXPORT void        GblObject_setParent                  (GBL_SELF, GblObject* pParent)                     GBL_NOEXCEPT;

GBL_EXPORT GblObject*  GblObject_childFirst                 (GBL_CSELF)                                        GBL_NOEXCEPT;
GBL_EXPORT GblSize     GblObject_childCount                 (GBL_CSELF)                                        GBL_NOEXCEPT;

GBL_EXPORT GblObject*  GblObject_siblingNext                (GBL_CSELF)                                        GBL_NOEXCEPT;

GBL_EXPORT void        GblObject_addChild                   (GBL_SELF, GblObject* pChild)                      GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblObject_removeChild                (GBL_SELF, GblObject* pChild)                      GBL_NOEXCEPT;

GBL_EXPORT GblObject*  GblObject_findAncestorByType         (GBL_CSELF, GblType ancestorType)                  GBL_NOEXCEPT;
GBL_EXPORT GblObject*  GblObject_findAncestorByName         (GBL_CSELF, const char* pName)                     GBL_NOEXCEPT;
GBL_EXPORT GblObject*  GblObject_findAncestorByHeight       (GBL_CSELF, GblSize height)                        GBL_NOEXCEPT;

GBL_EXPORT GblObject*  GblObject_findBaseByDepth            (GBL_CSELF, GblSize depth)                         GBL_NOEXCEPT;
GBL_EXPORT GblContext* GblObject_findContext                (GBL_SELF)                                         GBL_NOEXCEPT;

GBL_EXPORT GblObject*  GblObject_findChildByType            (GBL_CSELF, GblType childType)                     GBL_NOEXCEPT;
GBL_EXPORT GblObject*  GblObject_findChildByName            (GBL_CSELF, const char* pName)                     GBL_NOEXCEPT;
GBL_EXPORT GblObject*  GblObject_findChildByIndex           (GBL_CSELF, GblSize index)                         GBL_NOEXCEPT;

GBL_EXPORT GblObject*  GblObject_findSiblingByType          (GBL_CSELF, GblType siblingType)                   GBL_NOEXCEPT;
GBL_EXPORT GblObject*  GblObject_findSiblingByName          (GBL_CSELF, const char* pName)                     GBL_NOEXCEPT;
GBL_EXPORT GblObject*  GblObject_findSiblingByIndex         (GBL_CSELF, GblSize index)                         GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblObject_sendEvent                  (GBL_SELF, GblEvent* pEvent)                       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblObject_notifyEvent                (GBL_SELF, GblEvent* pEvent)                       GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblObject_installEventFilter         (GBL_SELF, GblIEventFilter* pFilter)               GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblObject_uninstallEventFilter       (GBL_SELF, GblIEventFilter* pFilter)               GBL_NOEXCEPT;

GBL_EXPORT GblSize     GblObject_eventFilterCount           (GBL_CSELF)                                        GBL_NOEXCEPT;
GBL_EXPORT GblIEventFilter*
                       GblObject_eventFilterAt              (GBL_CSELF, GblSize index)                         GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_OBJECT_H

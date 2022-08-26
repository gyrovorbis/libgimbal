/*! \file
 *  \brief GblBox (reference-counted, opaque userdata), and related functions
 *  \ingroup metaBuiltinTypes
 */

#ifndef GIMBAL_BOX_H
#define GIMBAL_BOX_H

#include "gimbal_instance.h"
#include "../ifaces/gimbal_ivariant.h"
#include "../../containers/gimbal_array_map.h"

#define GBL_BOX_TYPE                 (GBL_BUILTIN_TYPE(BOX))    ///< GblType UUID of a GblBox

#define GBL_BOX(instance)            (GBL_INSTANCE_CAST(instance, GBL_BOX_TYPE, GblBox))
#define GBL_BOX_CLASS(GBL_KLASS)     (GBL_CLASS_CAST(GBL_KLASS, GBL_BOX_TYPE, GblBoxClass))
#define GBL_BOX_GET_CLASS(instance)  (GBL_INSTANCE_GET_CLASS(instance, GBL_BOX_TYPE, GblBoxClass))

#define GBL_BOX_REF(instance)        (GblBox_ref(GBL_BOX(instance)))
#define GBL_BOX_UNREF(instance)      (GblBox_unref(GBL_BOX(instance)))

#define GBL_KLASS   GblBoxClass* pSelf
#define GBL_CKLASS  const GBL_KLASS
#define GBL_SELF    GblBox* pSelf
#define GBL_CSELF   const GBL_SELF

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblBox);

/*! \class GblBoxClass
 *  \brief Minimally bindable GblClass with reference semantics and opaque userdata.
 *  \ingroup metaBuiltinTypes
 *  \sa GblClass, GblBox
 *  \extends GblClass
 *  \implements GblIVariantIFace
 */
GBL_CLASS_BASE(GblBox, GblIVariant)
    GBL_PRIVATE()
        GblArrayMap* pFields;
    GBL_PRIVATE_END

    GBL_RESULT (*pFnDestructor)(GBL_SELF);
GBL_CLASS_END

/*! \class GblBox
 *  \brief Minimally bindable GblInstance with reference semantics and opaque userdata.
 *  \ingroup metaBuiltinTypes
 *  \sa GblInstance, GblBoxClass
 *  \extends GblInstance
 *  \implements GblIVariant
 */
GBL_INSTANCE_BASE(GblBox)
    GBL_PRIVATE()
        GblArrayMap* pFields;
        GblRefCount  refCounter;
        uint16_t     contextType         : 1;
        uint16_t     constructedInPlace  : 1;
        uint16_t     derivedFlags        : 14;
    GBL_PRIVATE_END
GBL_INSTANCE_END

// ===== Class =====
GBL_EXPORT GblBoxClass* GblBoxClass_createFloatingExt   (GblType           derivedType,
                                                         GblSize           totalSize,
                                                         void*             pUserdata,
                                                         GblArrayMapDtorFn pFnUdDtor)   GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblBoxClass_constructFloatingExt(GBL_KLASS,
                                                         GblType           derivedType,
                                                         void*             pUserdata,
                                                         GblArrayMapDtorFn pFnUdDtor)   GBL_NOEXCEPT;

GBL_EXPORT void*        GblBoxClass_userdata            (GBL_CKLASS)                    GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblBoxClass_setUserdata         (GBL_KLASS, void* pUserdata)    GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblBoxClass_setUserDestructor   (GBL_KLASS,
                                                         GblArrayMapDtorFn pFnUdDtor)   GBL_NOEXCEPT;

GBL_EXPORT uintptr_t    GblBoxClass_getField            (GBL_CKLASS, GblQuark key)      GBL_NOEXCEPT;
GBL_EXPORT uintptr_t    GblBoxClass_takeField           (GBL_KLASS, GblQuark key)       GBL_NOEXCEPT;
GBL_EXPORT GblBool      GblBoxClass_clearField          (GBL_KLASS, GblQuark key)       GBL_NOEXCEPT;
GBL_EXPORT GblBool      GblBoxClass_hasField            (GBL_CKLASS, GblQuark key)      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblBoxClass_setField            (GBL_KLASS,
                                                         GblQuark          key,
                                                         uintptr_t         ud,
                                                         GblArrayMapDtorFn pFnDtor)     GBL_NOEXCEPT;

// ===== Instance =====
GBL_EXPORT GblBox*      GblBox_create                   (GblType derivedType)           GBL_NOEXCEPT;
GBL_EXPORT GblBox*      GblBox_createWithClass          (GblBoxClass* pClass)           GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblBox_construct                (GBL_SELF, GblType derivedType) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblBox_constructWithClass       (GBL_SELF, GblBoxClass* pClass) GBL_NOEXCEPT;

GBL_EXPORT GblBox*      GblBox_createExt                (GblType           derivedType,
                                                         GblSize           totalSize,
                                                         void*             pUserdata,
                                                         GblArrayMapDtorFn pFnUdDtor)   GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblBox_constructExt             (GBL_SELF,
                                                         GblType           derivedType,
                                                         void*             pUserdata,
                                                         GblArrayMapDtorFn pFnUdDtor)   GBL_NOEXCEPT;

GBL_EXPORT GblBox*      GblBox_createExtWithClass       (GblBoxClass*      pClass,
                                                         GblSize           totalSize,
                                                         void*             pUserdata,
                                                         GblArrayMapDtorFn pFnUdDtor)   GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblBox_constructExtWithClass    (GBL_SELF,
                                                         GblBoxClass*      pClass,
                                                         void*             pUserdata,
                                                         GblArrayMapDtorFn pFnUdDtor)   GBL_NOEXCEPT;

GBL_EXPORT GblBox*      GblBox_ref                      (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT GblRefCount  GblBox_unref                    (GBL_SELF)                      GBL_NOEXCEPT;
GBL_EXPORT GblRefCount  GblBox_refCount                 (GBL_CSELF)                     GBL_NOEXCEPT;

GBL_EXPORT void*        GblBox_userdata                 (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblBox_setUserdata              (GBL_SELF, void* pUserdata)     GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblBox_setUserDestructor        (GBL_SELF,
                                                         GblArrayMapDtorFn pFnUdDtor)   GBL_NOEXCEPT;

GBL_EXPORT uintptr_t    GblBox_getField                 (GBL_CSELF, GblQuark key)       GBL_NOEXCEPT;
GBL_EXPORT uintptr_t    GblBox_takeField                (GBL_SELF, GblQuark key)        GBL_NOEXCEPT;
GBL_EXPORT GblBool      GblBox_clearField               (GBL_SELF, GblQuark key)        GBL_NOEXCEPT;
GBL_EXPORT GblBool      GblBox_hasField                 (GBL_CSELF, GblQuark key)       GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblBox_setField                 (GBL_SELF,
                                                         GblQuark          key,
                                                         uintptr_t         ud,
                                                         GblArrayMapDtorFn pFnDtor)     GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_CSELF
#undef GBL_SELF
#undef GBL_CKLASS
#undef GBL_KLASS

#endif // GIMBAL_BOX_H

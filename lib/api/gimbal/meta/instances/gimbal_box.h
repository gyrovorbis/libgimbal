#ifndef GIMBAL_BOX_H
#define GIMBAL_BOX_H

#include "gimbal_instance.h"
#include "../ifaces/gimbal_ivariant.h"
#include "../../containers/gimbal_array_map.h"

#define GBL_BOX_TYPE                 (GBL_BUILTIN_TYPE(BOX))

#define GBL_BOX(instance)            (GBL_INSTANCE_CAST(instance, GBL_BOX_TYPE, GblBox))
#define GBL_BOX_CLASS(klass)         (GBL_CLASS_CAST(klass, GBL_BOX_TYPE, GblBoxClass))
#define GBL_BOX_GET_CLASS(instance)  (GBL_INSTANCE_GET_CLASS(instance, GBL_BOX_TYPE, GblBoxClass))

#define GBL_BOX_REF(instance)        (GblBox_ref(GBL_BOX(instance)))
#define GBL_BOX_UNREF(instance)      (GblBox_unref(GBL_BOX(instance)))

#define KLASS   GblBoxClass* pSelf
#define CKLASS  const KLASS
#define SELF    GblBox* pSelf
#define CSELF   const SELF

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblBox);

GBL_CLASS_DERIVE(GblBoxClass,
                 GblClass, GblIVariantIFace)
    GBL_PRIVATE()
        GblArrayMap* pFields;
        uint16_t     instanceDtor : 1;
        uint16_t     derivedFlags : 15;
    GBL_PRIVATE_END

    GBL_RESULT  (*pFnDestructor)(SELF);
GBL_CLASS_END

GBL_INSTANCE_DERIVE(GblBox, GblInstance, GblBoxClass)
    GBL_PRIVATE()
        GblArrayMap* pFields;
        GblRefCount  refCounter;
        uint16_t     contextType         : 1;
        uint16_t     constructedInPlace  : 1;
        uint16_t     instanceDtor        : 1;
        uint16_t     derivedFlags        : 13;
    GBL_PRIVATE_END
GBL_INSTANCE_END

// ===== Class =====
GBL_EXPORT GblBox*     GblBoxClass_createFloatingExt   (GblType           derivedType,
                                                        GblSize           totalSize,
                                                        void*             pUserdata,
                                                        GblArrayMapDtorFn pFnUdDtor,
                                                        ...)                        GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblBoxClass_constructFloatingExt(KLASS,
                                                        void*             pUserdata,
                                                        GblArrayMapDtorFn pFnDtor,
                                                        ...)                        GBL_NOEXCEPT;

GBL_EXPORT void*       GblBoxClass_userdata            (CKLASS)                     GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblBoxClass_setUserdata         (KLASS, void* pUserdata)     GBL_NOEXCEPT;

GBL_EXPORT uintptr_t   GblBoxClass_getField            (CKLASS, GblQuark key)       GBL_NOEXCEPT;
GBL_EXPORT uintptr_t   GblBoxClass_takeField           (KLASS, GblQuark key)        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblBoxClass_clearField          (KLASS, GblQuark key)        GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblBoxClass_hasField            (KLASS, GblQuark key)        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblBoxClass_setField            (KLASS,
                                                        GblQuark          key,
                                                        uintptr_t         ud,
                                                        GblArrayMapDtorFn pFnDtor)  GBL_NOEXCEPT;

// ===== Instance =====
GBL_EXPORT GblBox*     GblBox_create                   (GblType derivedType)        GBL_NOEXCEPT;
GBL_EXPORT GblBox*     GblBox_createWithClass          (GblBoxClass* pClass)        GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblBox_construct                (SELF, GblType derivedType)  GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblBox_constructWithClass       (SELF, GblBoxClass* pClass)  GBL_NOEXCEPT;

GBL_EXPORT GblBox*     GblBox_createExt                (GblType           derivedType,
                                                        GblSize           totalSize,
                                                        void*             pUserdata,
                                                        GblArrayMapDtorFn pFnUdDtor,
                                                        ...)                        GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblBox_constructExt             (GblType           derivedType,
                                                        void*             pUserdata,
                                                        GblArrayMapDtorFn pFnUdDtor,
                                                        ...)                        GBL_NOEXCEPT;

GBL_EXPORT GblBox*     GblBox_createExtWithClass       (GblBoxClass*      pClass,
                                                        GblSize           totalSize,
                                                        void*             pUserdata,
                                                        GblArrayMapDtorFn pFnUdDtor,
                                                        ...)                        GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblBox_constructExtWithClass    (GblBoxClass*      pClass,
                                                        void*             pUserdata,
                                                        GblArrayMapDtorFn pFnUdDtor,
                                                        ...)                        GBL_NOEXCEPT;

GBL_EXPORT GblBox*     GblBox_ref                      (SELF)                       GBL_NOEXCEPT;
GBL_EXPORT GblRefCount GblBox_unref                    (SELF)                       GBL_NOEXCEPT;
GBL_EXPORT GblRefCount GblBox_refCount                 (CSELF)                      GBL_NOEXCEPT;

GBL_EXPORT void*       GblBox_userdata                 (CSELF)                      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblBox_setUserdata              (SELF, void* pUserdata)      GBL_NOEXCEPT;

GBL_EXPORT uintptr_t   GblBox_getField                 (CSELF, GblQuark key)        GBL_NOEXCEPT;
GBL_EXPORT uintptr_t   GblBox_takeField                (SELF, GblQuark key)         GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblBox_clearField               (SELF, GblQuark key)         GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblBox_hasField                 (SELF, GblQuark key)         GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblBox_setField                 (SELF,
                                                        GblQuark          key,
                                                        uintptr_t         ud,
                                                        GblArrayMapDtorFn pFnDtor)  GBL_NOEXCEPT;

GBL_DECLS_END

#undef CSELF
#undef SELF
#undef CKLASS
#undef KLASS

#endif // GIMBAL_BOX_H
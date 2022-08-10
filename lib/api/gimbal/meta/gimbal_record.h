#ifndef GIMBAL_REF_COUNTED_H
#define GIMBAL_REF_COUNTED_H

#include "gimbal_instance.h"
#include "../ifaces/gimbal_ivariant.h"
#include "../containers/gimbal_array_map.h"

#define GBL_REF_COUNTED_TYPE                 (GBL_BUILTIN_TYPE(RECORD))

#define GBL_REF_COUNTED(instance)            (GBL_INSTANCE_CAST(instance, GBL_REF_COUNTED_TYPE, GblRefCounted))
#define GBL_REF_COUNTED_CLASS(klass)         (GBL_CLASS_CAST(klass, GBL_REF_COUNTED_TYPE, GblRefCountedClass))
#define GBL_REF_COUNTED_GET_CLASS(instance)  (GBL_INSTANCE_GET_CLASS(instance, GBL_REF_COUNTED_TYPE, GblRefCountedClass))

#define KLASS   GblRefCountedClass* pSelf
#define CKLASS  const KLASS
#define SELF    GblRefCounted* pSelf
#define CSELF   const SELF

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblRefCounted);

GBL_CLASS_DERIVE(GblRefCountedClass,
                 GblClass, GblIVariantIFace)
    GBL_PRIVATE()
        GblArrayMap* pFields;
    GBL_PRIVATE_END

    GBL_RESULT  (*pFnDestructor)(SELF);
GBL_CLASS_END

GBL_INSTANCE_DERIVE(GblRefCounted, GblInstance, GblRefCountedClass)
    GBL_PRIVATE()
        GblArrayMap* pFields;
        GblRefCount  refCounter;
        uint16_t     contextType         : 1;
        uint16_t     constructedInPlace  : 1;
        uint16_t     derivedFlags        : 14;
    GBL_PRIVATE_END
GBL_INSTANCE_END

// ===== Class =====
GBL_EXPORT GBL_RESULT  GblRefCountedClass_setField     (KLASS,
                                                    GblQuark          key,
                                                    uintptr_t         ud,
                                                    GblArrayMapDtorFn pFnDtor) GBL_NOEXCEPT;

GBL_EXPORT uintptr_t   GblRefCountedClass_getField     (CKLASS, GblQuark key)      GBL_NOEXCEPT;
GBL_EXPORT uintptr_t   GblRefCountedClass_takeField    (KLASS, GblQuark key)       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblRefCountedClass_clearField   (KLASS, GblQuark key)       GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblRefCountedClass_hasField     (KLASS, GblQuark key)       GBL_NOEXCEPT;

// ===== Instance =====
GBL_EXPORT GblRefCounted*  GblRefCounted_create            (GblType derivedType)       GBL_NOEXCEPT;
GBL_EXPORT GblRefCounted*  GblRefCounted_createWithClass   (GblRefCountedClass* pClass)    GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblRefCounted_construct         (SELF,
                                                    GblType derivedType)       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblRefCounted_constructWithClass(SELF,
                                                    GblRefCountedClass* pClass)    GBL_NOEXCEPT;

GBL_EXPORT GblRefCounted*  GblRefCounted_ref               (SELF)                      GBL_NOEXCEPT;
GBL_EXPORT GblRefCount GblRefCounted_unref             (SELF)                      GBL_NOEXCEPT;
GBL_EXPORT GblRefCount GblRefCounted_refCount          (CSELF)                     GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblRefCounted_setField          (SELF,
                                                    GblQuark          key,
                                                    uintptr_t         ud,
                                                    GblArrayMapDtorFn pFnDtor) GBL_NOEXCEPT;

GBL_EXPORT uintptr_t   GblRefCounted_getField          (CSELF, GblQuark key)       GBL_NOEXCEPT;
GBL_EXPORT uintptr_t   GblRefCounted_takeField         (SELF, GblQuark key)        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblRefCounted_clearField        (SELF, GblQuark key)        GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblRefCounted_hasField          (SELF, GblQuark key)        GBL_NOEXCEPT;

GBL_DECLS_END

#undef CSELF
#undef SELF
#undef CKLASS
#undef KLASS

#endif // GIMBAL_REF_COUNTED_H

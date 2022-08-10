#ifndef GIMBAL_REF_COUNTED_H
#define GIMBAL_REF_COUNTED_H

#include "gimbal_instance.h"
#include "../ifaces/gimbal_ivariant.h"
#include "../containers/gimbal_array_map.h"

#define GBL_RECORD_TYPE                 (GBL_BUILTIN_TYPE(RECORD))

#define GBL_RECORD(instance)            (GBL_INSTANCE_CAST(instance, GBL_RECORD_TYPE, GblRecord))
#define GBL_RECORD_CLASS(klass)         (GBL_CLASS_CAST(klass, GBL_RECORD_TYPE, GblRecordClass))
#define GBL_RECORD_GET_CLASS(instance)  (GBL_INSTANCE_GET_CLASS(instance, GBL_RECORD_TYPE, GblRecordClass))

#define KLASS   GblRecordClass* pSelf
#define CKLASS  const KLASS
#define SELF    GblRecord* pSelf
#define CSELF   const SELF

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblRecord);

GBL_CLASS_DERIVE(GblRecordClass,
                 GblClass, GblIVariantIFace)
    GBL_PRIVATE()
        GblArrayMap* pFields;
    GBL_PRIVATE_END

    GBL_RESULT  (*pFnDestructor)(SELF);
GBL_CLASS_END

GBL_INSTANCE_DERIVE(GblRecord, GblInstance, GblRecordClass)
    GBL_PRIVATE()
        GblArrayMap* pFields;
        GblRefCount  refCounter;
        uint16_t     contextType         : 1;
        uint16_t     constructedInPlace  : 1;
        uint16_t     derivedFlags        : 14;
    GBL_PRIVATE_END
GBL_INSTANCE_END

// ===== Class =====
GBL_EXPORT GBL_RESULT  GblRecordClass_setField     (KLASS,
                                                    GblQuark          key,
                                                    uintptr_t         ud,
                                                    GblArrayMapDtorFn pFnDtor) GBL_NOEXCEPT;

GBL_EXPORT uintptr_t   GblRecordClass_getField     (CKLASS, GblQuark key)      GBL_NOEXCEPT;
GBL_EXPORT uintptr_t   GblRecordClass_takeField    (KLASS, GblQuark key)       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblRecordClass_clearField   (KLASS, GblQuark key)       GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblRecordClass_hasField     (KLASS, GblQuark key)       GBL_NOEXCEPT;

// ===== Instance =====
GBL_EXPORT GblRecord*  GblRecord_create            (GblType derivedType)       GBL_NOEXCEPT;
GBL_EXPORT GblRecord*  GblRecord_createWithClass   (GblRecordClass* pClass)    GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblRecord_construct         (SELF,
                                                    GblType derivedType)       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblRecord_constructWithClass(SELF,
                                                    GblRecordClass* pClass)    GBL_NOEXCEPT;

GBL_EXPORT GblRecord*  GblRecord_ref               (SELF)                      GBL_NOEXCEPT;
GBL_EXPORT GblRefCount GblRecord_unref             (SELF)                      GBL_NOEXCEPT;
GBL_EXPORT GblRefCount GblRecord_refCount          (CSELF)                     GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblRecord_setField          (SELF,
                                                    GblQuark          key,
                                                    uintptr_t         ud,
                                                    GblArrayMapDtorFn pFnDtor) GBL_NOEXCEPT;

GBL_EXPORT uintptr_t   GblRecord_getField          (CSELF, GblQuark key)       GBL_NOEXCEPT;
GBL_EXPORT uintptr_t   GblRecord_takeField         (SELF, GblQuark key)        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblRecord_clearField        (SELF, GblQuark key)        GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblRecord_hasField          (SELF, GblQuark key)        GBL_NOEXCEPT;

GBL_DECLS_END

#undef CSELF
#undef SELF
#undef CKLASS
#undef KLASS

#endif // GIMBAL_REF_COUNTED_H

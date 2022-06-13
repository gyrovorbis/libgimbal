#ifndef GIMBAL_CLASS_H
#define GIMBAL_CLASS_H

#include "gimbal_type.h"

#define GBL_CLASS(klass)                    ((GblClass*)klass)
#define GBL_CLASS_TYPE(klass)               (GblType_fromClass(GBL_CLASS(klass)))
#define GBL_CLASS_SUPER(klass)              (GblClass_peek(GblType_parent(GBL_CLASS_TYPE(klass))))
#define GBL_CLASS_DEFAULT(klass)            (GblClass_peek(GBL_CLASS_TYPE(klass)))
#define GBL_CLASS_PRIVATE(klass, type)      (GblClass_private(GBL_CLASS(klass), type))

#define GBL_CLASS_CHECK(klass, toType)                      \
            (GblClass_check((GblClass*)klass, toType))
#define GBL_CLASS_CHECK_PREFIX(klass, typePrefix)           \
            (GBL_CLASS_CHECK(klass, typePrefix##_TYPE))

#define GBL_CLASS_CAST(klass, toType, cType)                \
            ((cType*)GblClass_cast((GblClass*)klass, toType))
#define GBL_CLASS_CAST_PREFIX(klass, typePrefix)            \
            (GBL_CLASS_CAST(klass, typePrefix##_TYPE, typePrefix##_CLASS_STRUCT))

#define GBL_CLASS_TRY(klass, toType, cType)                 \
            ((cType*)GblClass_try((GblClass*)klass, toType))
#define GBL_CLASS_TRY_PREFIX(klass, typePrefix)             \
            (GBL_CLASS_TRY(klass, typePrefix##_TYPE, typePrefix##_CLASS_STRUCT))

#define SELF    GblClass*           pSelf
#define CSELF   const GblClass*     pSelf

GBL_DECLS_BEGIN

typedef struct GblClass {
    uintptr_t           private_;
} GblClass;

// Returns an existing reference to the default class for the given type or lazily allocates a new one
GBL_EXPORT GblClass*    GblClass_ref                (GblType type)                  GBL_NOEXCEPT;
// Creates a new, unowned class for the given type on the heap
GBL_EXPORT GblClass*    GblClass_createFloating     (GblType type)                  GBL_NOEXCEPT;
// Placement constructs an unowned class for the given type
GBL_EXPORT GBL_RESULT   GblClass_constructFloating  (SELF, GblType type)            GBL_NOEXCEPT;

// Returns a pointer to the default class for the given type without incrementing refCount
GBL_EXPORT GblClass*    GblClass_peek               (GblType type)                  GBL_NOEXCEPT;
// Retrieves an existing reference hed by an instance wthout incrementing refCount
GBL_EXPORT GblClass*    GblClass_peekFromInstance   (const GblInstance* pInstance)  GBL_NOEXCEPT;
// Returns a reference to an existing class
GBL_INLINE GblClass*    GblClass_refFromClass       (SELF)                          GBL_NOEXCEPT;

// Release a reference to the default class
GBL_EXPORT GblRefCount  GblClass_unref              (SELF)                          GBL_NOEXCEPT;
// Delete heap-allocated class created with GblClass_createFloating()
GBL_EXPORT GBL_RESULT   GblClass_destroyFloating    (SELF)                          GBL_NOEXCEPT;
// Placement delete on class created in-place with GblClass_constructFloating
GBL_EXPORT GBL_RESULT   GblClass_destructFloating   (SELF)                          GBL_NOEXCEPT;

// Returns true if the given class can be casted to the given class type
GBL_EXPORT GblBool      GblClass_check              (CSELF, GblType toType)         GBL_NOEXCEPT;
// Casts the given class to a class of the given type, erroring out upon failure
GBL_EXPORT GblClass*    GblClass_cast               (SELF, GblType toType)          GBL_NOEXCEPT;
// Attempts to cast the given class to a class of the given type, returning NULL upon failure
GBL_EXPORT GblClass*    GblClass_try                (SELF, GblType toType)          GBL_NOEXCEPT;
// Retrieves the private data pointer associated with a given base type
GBL_EXPORT void*        GblClass_private            (CSELF, GblType type)           GBL_NOEXCEPT;

GBL_INLINE GblType      GblClass_type               (CSELF)                         GBL_NOEXCEPT;
GBL_INLINE const char*  GblClass_typeName           (CSELF)                         GBL_NOEXCEPT;
GBL_INLINE GblSize      GblClass_size               (CSELF)                         GBL_NOEXCEPT;
GBL_INLINE GblSize      GblClass_privateSize        (CSELF)                         GBL_NOEXCEPT;
GBL_INLINE GblSize      GblClass_totalSize          (CSELF)                         GBL_NOEXCEPT;
GBL_INLINE GblClass*    GblClass_super              (CSELF)                         GBL_NOEXCEPT;
GBL_INLINE GblClass*    GblClass_default            (CSELF)                         GBL_NOEXCEPT;
GBL_INLINE const char*  GblClass_name               (CSELF)                         GBL_NOEXCEPT;
GBL_INLINE GblBool      GblClass_isDefault          (CSELF)                         GBL_NOEXCEPT;
GBL_INLINE GblBool      GblClass_isOverridden       (CSELF)                         GBL_NOEXCEPT;
GBL_EXPORT GblBool      GblClass_isFloating         (CSELF)                         GBL_NOEXCEPT;
GBL_EXPORT GblBool      GblClass_isInterface        (CSELF)                         GBL_NOEXCEPT;
GBL_EXPORT GblBool      GblClass_isInterfaceImpl    (CSELF)                         GBL_NOEXCEPT;
GBL_EXPORT GblBool      GblClass_isOwned            (CSELF)                         GBL_NOEXCEPT;
GBL_EXPORT GblBool      Gblclass_isInPlace          (CSELF)                         GBL_NOEXCEPT;

// ========== IMPL ==========

GBL_INLINE GblBool GblClass_isDefault(CSELF) GBL_NOEXCEPT {
    return pSelf == GBL_CLASS_DEFAULT(pSelf);
}

GBL_INLINE GblType GblClass_type(CSELF) GBL_NOEXCEPT {
    return GBL_CLASS_TYPE(pSelf);
}

GBL_INLINE const char* GblClass_typeName(CSELF) GBL_NOEXCEPT {
    return pSelf? GblType_name(GBL_CLASS_TYPE(pSelf)) : "Invalid";
}

GBL_INLINE GblClass* GblClass_super(CSELF) GBL_NOEXCEPT {
    return GBL_CLASS_SUPER(pSelf);
}

GBL_INLINE GblClass* GblClass_default(CSELF) GBL_NOEXCEPT {
    return GBL_CLASS_DEFAULT(pSelf);
}

GBL_INLINE GblSize GblClass_size(CSELF) GBL_NOEXCEPT {
    GblSize size = 0;
    if(pSelf) {
        const GblTypeInfo* pInfo = GblType_info(GBL_CLASS_TYPE(pSelf));
        if(pInfo) {
            size = pInfo->classSize;
        }
    }
    return size;
}

GBL_INLINE const char* GblClass_name(CSELF) GBL_NOEXCEPT {
    return pSelf? GblType_name(GBL_CLASS_TYPE(pSelf)) : "Invalid";
}

GBL_INLINE GblClass* GblClass_refFromClass(GblClass* pClass) GBL_NOEXCEPT {
    return pClass? GblClass_ref(GBL_CLASS_TYPE(pClass)) : NULL;
}

GBL_DECLS_END

#undef SELF
#undef CSELF

#endif // GIMBAL_CLASS_H

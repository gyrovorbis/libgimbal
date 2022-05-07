#ifndef GIMBAL_CLASS_H
#define GIMBAL_CLASS_H

#include "gimbal_type.h"

#define GBL_CLASS(klass)                    ((GblClass*)klass)
#define GBL_CLASS_TYPE(klass)               (GblType_fromClass(GBL_CLASS(klass)))
#define GBL_CLASS_SUPER(klass)              (GblClass_peekFromType(GblType_parent(GBL_CLASS_TYPE(klass))))

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

#define GBL_CLASS_VCALL()

#define SELF    GblClass*           pSelf
#define CSELF   const GblClass*     pSelf

GBL_DECLS_BEGIN

GBL_DECLARE_OPAQUE(GblClassPrivate_);

typedef struct GblClass {
    GblClassPrivate_     pPrivate_;
} GblClass;

GBL_INLINE GblClass*    GblClass_ref                (SELF)                          GBL_NOEXCEPT;
GBL_EXPORT GblClass*    GblClass_refFromType        (GblType type)                  GBL_NOEXCEPT;
GBL_EXPORT GblClass*    GblClass_peekFromType       (GblType type)                  GBL_NOEXCEPT;
GBL_EXPORT GblClass*    GblClass_createFloating     (GblType type)                  GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblClass_constructFloating  (SELF, GblType type)            GBL_NOEXCEPT;
GBL_EXPORT GblClass*    GblClass_fromInstance       (const GblInstance* pInstance)  GBL_NOEXCEPT;

GBL_EXPORT GblRefCount  GblClass_unref              (SELF)                          GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblClass_destroyFloating    (SELF)                          GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblClass_destructFloating   (SELF)                          GBL_NOEXCEPT;

GBL_INLINE GblType      GblClass_type               (CSELF)                         GBL_NOEXCEPT;
GBL_INLINE const char*  GblClass_typeName           (CSELF)                         GBL_NOEXCEPT;
GBL_INLINE GblSize      GblClass_size               (CSELF)                         GBL_NOEXCEPT;
GBL_INLINE GblClass*    GblClass_super              (CSELF)                         GBL_NOEXCEPT;
GBL_INLINE const char*  GblClass_name               (CSELF)                         GBL_NOEXCEPT;
GBL_INLINE GblRefCount  GblClass_refCount           (CSELF)                         GBL_NOEXCEPT;
GBL_EXPORT GblRefCount  GblClass_refCountFromType   (GblType type)                  GBL_NOEXCEPT;
GBL_EXPORT GblBool      GblClass_isFloating         (CSELF)                         GBL_NOEXCEPT;
GBL_EXPORT GblBool      GblClass_isInterfaceImpl    (CSELF)                         GBL_NOEXCEPT;
GBL_EXPORT GblBool      GblClass_isOwned            (CSELF)                         GBL_NOEXCEPT;
GBL_EXPORT GblBool      Gblclass_isInPlace          (CSELF)                         GBL_NOEXCEPT;

GBL_EXPORT GblBool      GblClass_check              (CSELF, GblType toType)         GBL_NOEXCEPT;
GBL_EXPORT GblClass*    GblClass_cast               (SELF, GblType toType)          GBL_NOEXCEPT;
GBL_EXPORT GblClass*    GblClass_try                (SELF, GblType toType)          GBL_NOEXCEPT;

// ========== IMPL ==========

GBL_INLINE GblType GblClass_type(CSELF) GBL_NOEXCEPT {
    return GBL_CLASS_TYPE(pSelf);
}

GBL_INLINE const char* GblClass_typeName(CSELF) GBL_NOEXCEPT {
    return pSelf? GblType_name(GBL_CLASS_TYPE(pSelf)) : "Invalid";
}

GBL_INLINE GblClass* GblClass_super(CSELF) GBL_NOEXCEPT {
    return GBL_CLASS_SUPER(pSelf);
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

 GBL_INLINE GblClass* GblClass_ref(SELF) GBL_NOEXCEPT {
    return pSelf? GblClass_refFromType(GBL_CLASS_TYPE(pSelf)) : NULL;
}

 GBL_INLINE GblRefCount GblClass_refCount(const GblClass* pSelf) GBL_NOEXCEPT {
     return GblClass_refCountFromType(GBL_CLASS_TYPE(pSelf));
 }


GBL_DECLS_END

#undef SELF
#undef CSELF

#endif // GIMBAL_CLASS_H

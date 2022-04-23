#ifndef GIMBAL_CLASS_H
#define GIMBAL_CLASS_H

#include "gimbal_type.h"

#define GBL_CLASS(klass)                    ((GblClass*)klass)
#define GBL_CLASS_TYPE(klass)               (gblTypeFromClass(GBL_CLASS(klass)))
#define GBL_CLASS_SUPER(klass)              (gblTypeClassPeek(gblTypeParent(GBL_CLASS_TYPE(klass))))
#define GBL_CLASS_IS_FLOATING(klass)        (gblTypeClassIsFloating(GBL_CLASS(klass)))

#define SELF    GblClass*           pSelf
#define CSELF   const GblClass*     pSelf

GBL_DECLS_BEGIN

typedef struct GblClass {
    GblType     type;
} GblClass;


GBL_INLINE GBL_RESULT   GblClass_constructFloating  (SELF, GblType type);
GBL_INLINE GblClass*    GblClass_createFloating     (GblType type);
GBL_INLINE GBL_RESULT   GblClass_destructFloating   (SELF);
GBL_INLINE GBL_RESULT   GblClass_destroyFloating    (SELF);
GBL_INLINE GblType      GblClass_typeOf             (CSELF);
GBL_INLINE GblSize      GblClass_sizeOf             (CSELF);
GBL_INLINE GblSize      GblClass_alignOf            (CSELF);
GBL_INLINE uint16_t     GblClass_offsetOf           (CSELF);
GBL_INLINE GblClass*    GblClass_superOf            (CSELF);
GBL_INLINE const char*  GblClass_name               (CSELF);
GBL_INLINE GblRefCount  GblClass_refCount           (CSELF);
GBL_INLINE GblClass*    GblClass_ref                (SELF);
GBL_INLINE GblRefCount  GblClass_unref              (SELF);
GBL_INLINE GblBool      GblClass_isFloating         (CSELF);



// ====== INLINE IMPLEMENTATIONS =====

GBL_MAYBE_UNUSED GBL_INLINE GBL_RESULT GblClass_constructFloating(SELF, GblType type) {
    return gblTypeClassConstructFloating(type, pSelf);
}

GBL_MAYBE_UNUSED GBL_INLINE GblClass* GblClass_createFloating(GblType type) {
    return gblTypeClassCreateFloating(type);
}

GBL_MAYBE_UNUSED GBL_INLINE GBL_RESULT GblClass_destructFloating(SELF) {
    return gblTypeClassDestructFloating(pSelf);
}

GBL_MAYBE_UNUSED GBL_INLINE GBL_RESULT GblClass_destroyFloating(SELF) {
    return gblTypeClassDestroyFloating(pSelf);
}

GBL_MAYBE_UNUSED GBL_INLINE GblType      GblClass_typeOf     (CSELF) {
    return GBL_CLASS_TYPE(pSelf);
}
GBL_MAYBE_UNUSED GBL_INLINE GblClass*    GblClass_superOf   (CSELF) {
    return GBL_CLASS_SUPER(pSelf);
}
GBL_MAYBE_UNUSED GBL_INLINE GblSize      GblClass_sizeOf     (CSELF) {
    return pSelf? gblTypeInfo(pSelf->type)->classSize : 0;
}

GBL_MAYBE_UNUSED GBL_INLINE GblSize      GblClass_alignOf    (CSELF) {
    return pSelf? gblTypeInfo(pSelf->type)->classAlign : 0;
}

GBL_MAYBE_UNUSED GBL_INLINE const char*  GblClass_name       (CSELF) {
    return pSelf? gblTypeName(pSelf->type) : "Invalid";
}

GBL_MAYBE_UNUSED GBL_INLINE GblRefCount  GblClass_refCount   (CSELF) {
    return pSelf? gblTypeClassRefCount(pSelf->type) : 0;
}

GBL_MAYBE_UNUSED GBL_INLINE GblClass*    GblClass_ref        (SELF) {
    return pSelf? gblTypeClassRef(pSelf->type) : NULL;
}
GBL_MAYBE_UNUSED GBL_INLINE GblRefCount    GblClass_unref      (SELF) {
    return pSelf? gblTypeClassUnref(pSelf) : 0;
}
GBL_MAYBE_UNUSED GBL_INLINE GblBool GblClass_isFloating(CSELF) {
    return GBL_CLASS_IS_FLOATING(pSelf);
}

GBL_DECLS_END

#undef SELF
#undef CSELF

#endif // GIMBAL_CLASS_H

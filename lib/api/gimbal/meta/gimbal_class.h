#ifndef GIMBAL_CLASS_H
#define GIMBAL_CLASS_H

#include "gimbal_type.h"

GBL_DECLS_BEGIN

#define SELF    GblClass*           pSelf
#define CSELF   const GblClass*     pSelf

#define GBL_CLASS_VCALL_BODY(VFUNC, ...)                                \
    if(!pSelf)          return GBL_RESULT_ERROR_INVALID_ARG;            \
    if(!pSelf->VFUNC)   return GBL_RESULT_ERROR_INVALID_VIRTUAL_CALL;   \
    else                return pSelf->VFUNC(__VA_ARGS__)


typedef struct GblClass {
    GblType             typeId;
} GblClass;


GBL_INLINE GblType      GblClass_typeOf     (CSELF);
GBL_INLINE GblSize      GblClass_sizeOf     (CSELF);
GBL_INLINE GblSize      GblClass_alignOf    (CSELF);
GBL_INLINE uint16_t     GblClass_offsetOf   (CSELF);
GBL_INLINE GblClass*    GblClass_parentOf   (CSELF);
GBL_INLINE const char*  GblClass_name       (CSELF);
GBL_INLINE GblRefCount  GblClass_refCount   (CSELF);
GBL_INLINE GblClass*    GblClass_cast       (SELF,  GblType isAType);
GBL_INLINE GblClass*    GblClass_ref        (SELF);
GBL_INLINE GblRefCount  GblClass_unref      (SELF);

// ====== INLINE IMPLEMENTATIONS =====


GBL_MAYBE_UNUSED GBL_INLINE GblType      GblClass_typeOf     (CSELF) {
    return pSelf? pSelf->typeId : GBL_TYPE_INVALID;
}
GBL_MAYBE_UNUSED GBL_INLINE GblClass*    GblClass_parentOf   (CSELF) {
    return pSelf? gblTypeClassPeek(gblTypeParent(pSelf->typeId)) : NULL;
}
GBL_MAYBE_UNUSED GBL_INLINE GblSize      GblClass_sizeOf     (CSELF) {
    return pSelf? gblTypeInfo(pSelf->typeId)->classSize : 0;
}

GBL_MAYBE_UNUSED GBL_INLINE GblSize      GblClass_alignOf    (CSELF) {
    return pSelf? gblTypeInfo(pSelf->typeId)->classAlign : 0;
}

GBL_MAYBE_UNUSED GBL_INLINE const char*  GblClass_name       (CSELF) {
    return pSelf? gblTypeName(pSelf->typeId) : "Invalid";
}

GBL_MAYBE_UNUSED GBL_INLINE GblRefCount  GblClass_refCount   (CSELF) {
    return pSelf? gblTypeClassRefCount(pSelf->typeId) : 0;
}
GBL_MAYBE_UNUSED GBL_INLINE GblClass*    GblClass_cast       (SELF,  GblType isAType) {
    if(pSelf && gblTypeIsA(pSelf->typeId, isAType)) {
        return pSelf;
    } else {
        return NULL;
    }
}
GBL_MAYBE_UNUSED GBL_INLINE GblClass*    GblClass_ref        (SELF) {
    return pSelf? gblTypeClassRef(pSelf->typeId) : NULL;
}
GBL_MAYBE_UNUSED GBL_INLINE GblRefCount    GblClass_unref      (SELF) {
    return pSelf? gblTypeClassUnref(pSelf) : 0;
}

#undef SELF
#undef CSELF

GBL_DECLS_END

#endif // GIMBAL_CLASS_H

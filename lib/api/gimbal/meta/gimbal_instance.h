#ifndef GIMBAL_INSTANCE_H
#define GIMBAL_INSTANCE_H


#include "gimbal_class.h"

GBL_DECLS_BEGIN

#define SELF    GblInstance*       pSelf
#define CSELF   const GblInstance* pSelf

#if 0
GBL_INLINE GblType      GblInstance_typeOf      (CSELF);
GBL_INLINE GblSize      GblInstance_sizeOf      (CSELF);
GBL_INLINE GblSize      GblInstance_alignOf     (CSELF);
GBL_INLINE GblClass*    GblInstance_classOf     (CSELF);
GBL_INLINE GblClass*    GblInstance_parentOf    (CSELF);
GBL_INLINE GblInstance* GblInstance_cast        (SELF, GblType toType);







// ====== INLINEZ ======


GBL_INLINE GblType      GblInstance_typeOf      (CSELF) {
    return pSelf? GblClass_typeOf(pSelf->pClass) : GBL_TYPE_INVALID;
}
GBL_INLINE GblSize      GblInstance_sizeOf      (CSELF) {
    return pSelf? GblClass_sizeOf(pSelf->pClass) : 0;
}
GBL_INLINE GblSize      GblInstance_alignOf     (CSELF) {
    return pSelf? GblClass_alignOf(pSelf->pClass) : 0;
}
GBL_INLINE GblClass*    GblInstance_classOf     (CSELF) {
    return pSelf? pSelf->pClass : NULL;
}
GBL_INLINE GblClass*    GblInstance_parentOf     (CSELF) {
    return pSelf? GblClass_parentOf(pSelf->pClass) : NULL;
}
GBL_INLINE GblInstance* GblInstance_cast(SELF, GblType toType) {
    return pSelf && GblClass_cast(pSelf->pClass, toType) ? pSelf : NULL;
}

#endif
#undef SELF
#undef CSELF

GBL_DECLS_END

#endif // GIMBAL_INSTANCE_H

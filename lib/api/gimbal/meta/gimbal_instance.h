#ifndef GIMBAL_INSTANCE_H
#define GIMBAL_INSTANCE_H


#include "gimbal_class.h"

GBL_BEGIN_DECLS

#define SELF    GblInstance*       pSelf
#define CSELF   const GblInstance* pSelf

typedef struct GblInstance {
    GblClass*   pClass;
} GblInstance;


GBL_EXPORT GblType      GblInstance_typeOf      (CSELF);
GBL_EXPORT GblSize      GblInstance_sizeOf      (CSELF);
GBL_EXPORT GblSize      GblInstance_alignOf     (CSELF);
GBL_EXPORT GblClass*    GblInstance_classOf     (CSELF);
GBL_EXPORT GblClass*    GblInstance_parentOf    (CSELF);
GBL_EXPORT GblInstance* GblInstance_cast        (SELF, GblType toType);

GBL_API                 GblInstance_vMetaCall  (SELF,   GBL_META_CALL call, GblUint id, void** ppArgs);
GBL_API                 GblInstance_vConstruct (SELF);
GBL_API                 GblInstance_vDestruct  (SELF);
GBL_API                 GblInstance_vCopy      (SELF,   const GblInstance* pSrc);
GBL_API                 GblInstance_vMove      (SELF,   const GblInstance* pSrc);
GBL_API                 GblInstance_vCompare   (CSELF,  const GblVariant* pVariant);
GBL_API                 GblInstance_vConvert   (CSELF,  GblVariant* pVariant);
GBL_API                 GblInstance_vLoad      (SELF,   const void* pInstance, const GblString* pString);
GBL_API                 GblInstance_vSave      (CSELF,  const void* pInstance, GblString* pString);


GBL_EXPORT GblType      GblInstance_typeOf      (CSELF) {
    return pSelf? GblClass_typeOf(pSelf->pClass) : GBL_TYPE_INVALID;
}
GBL_EXPORT GblSize      GblInstance_sizeOf      (CSELF) {
    return pSelf? GblClass_sizeOf(pSelf->pClass) : 0;
}
GBL_EXPORT GblSize      GblInstance_alignOf     (CSELF) {
    return pSelf? GblClass_alignOf(pSelf->pClass) : 0;
}
GBL_EXPORT GblClass*    GblInstance_classOf     (CSELF) {
    return pSelf? pSelf->pClass : NULL;
}
GBL_EXPORT GblClass*    GblInstance_parentOf     (CSELF) {
    return pSelf? GblClass_parentOf(pSelf->pClass) : NULL;
}
GBL_EXPORT GblInstance* GblInstance_cast(SELF, GblType toType) {
    return pSelf && GblClass_cast(pSelf->pClass, toType) ? pSelf : NULL;
}

#undef SELF
#undef CSELF

GBL_END_DECLS

#endif // GIMBAL_INSTANCE_H

#ifndef GIMBAL_INTERFACE_H
#define GIMBAL_INTERFACE_H

#include "gimbal_class.h"

#define GBL_INTERFACE(klass)                GBL_TYPE_CAST_CLASS(klass, GBL_TYPE_INTERFACE, GblInterface)

#define SELF    GblInterface* pSelf
#define CSELF   const GblInterface* pSelf

GBL_DECLS_BEGIN

typedef struct GblInterface {
    GblClass    base;
    int16_t     offsetToTop;
} GblInterface;

GBL_INLINE int16_t              GblInterface_classOffset(CSELF);
GBL_INLINE GblClass*            GblInterface_outerClass(CSELF);

GBL_MAYBE_UNUSED GBL_INLINE
uint16_t GblInterface_offsetOf(CSELF) {
    return pSelf? pSelf->offsetToTop : 0;
}

GBL_MAYBE_UNUSED GBL_INLINE
GblClass* GblInterface_outerClass(CSELF) {
    return pSelf? (GblClass*)((uint8_t*)pSelf - pSelf->offsetToTop) : NULL;
}


GBL_DECLS_END

#undef CSELF
#undef SELF

#endif // GIMBAL_INTERFACE_H

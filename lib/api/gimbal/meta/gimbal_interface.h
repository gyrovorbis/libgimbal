#ifndef GIMBAL_INTERFACE_H
#define GIMBAL_INTERFACE_H

#include "gimbal_instance.h"

#define SELF    GblInterface* pSelf
#define CSELF   const GblInterface* pSelf

GBL_DECLS_BEGIN

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

#ifndef GIMBAL_INTERFACE_H
#define GIMBAL_INTERFACE_H

#include "gimbal_instance.h"

#define SELF    GblInterface* pSelf
#define CSELF   const GblInterface* pSelf

GBL_DECLS_BEGIN

typedef struct GblInterface {
    GblClass            base;
    int16_t             offsetToTop;
} GblInterface;


GBL_INLINE int16_t              GblInterface_classOffset(CSELF);
GBL_INLINE GblClass*            GblInterface_outerClass(CSELF);




GBL_MAYBE_UNUSED GBL_INLINE
uint16_t GblInterface_offsetOf(CSELF) {
    return pSelf? pSelf->offsetToTop : 0;
}

GBL_MAYBE_UNUSED GBL_INLINE
GblClass* GblInterface_outerClass(CSELF) {
    if(!pSelf)              return NULL;
    if(!pSelf->offsetToTop) return NULL;
    else
        return (GblClass*)((uint8_t*)pSelf + pSelf->offsetToTop);
}


#if 0


GBL_INLINE const GblInstance*   GblInterface_instance(CSELF);
GBL_INLINE const void*          GblInterface_vtable(CSELF);
GBL_INLINE GblBool              GblInterface_isValid(CSELF);




GBL_MAYBE_UNUSED GBL_INLINE
GblType GblInterface_instanceTypeOf(CSELF) {
    return pSelf? GblInstance_typeOf(GblInterface_instance(pSelf)) : GBL_TYPE_INVALID;
}

GBL_MAYBE_UNUSED GBL_INLINE
GblClass* GblInterface_instanceClassOf(CSELF) {
    return pSelf? GblInstance_classOf(GblInterface_instance(pSelf)) : NULL;
}

GBL_MAYBE_UNUSED GBL_INLINE
const GblInstance* GblInterface_instance(CSELF) {
    return pSelf? pSelf->pInstance : NULL;
}

GBL_MAYBE_UNUSED GBL_INLINE
const void* GblInterface_vtable(CSELF) {
    return pSelf? pSelf->pVTable : NULL;
}

GBL_MAYBE_UNUSED GBL_INLINE
GblBool GblInterface_isValid(CSELF) {
    if(!pSelf)          return GBL_FALSE;
    if(!pSelf->pVTable) return GBL_FALSE;
    //if(!pSelf->pInstance)   return GBL_FALSE;
    return GBL_TRUE;
}



typedef struct GblInterface {
    union {
        void*           pVTable;
        GblInstance     base;
    };
    GblInstance*        pInstance;
} GblInterface;



#endif


GBL_DECLS_END

#undef CSELF
#undef SELF

#endif // GIMBAL_INTERFACE_H

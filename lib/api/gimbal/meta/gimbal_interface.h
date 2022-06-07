#ifndef GIMBAL_INTERFACE_H
#define GIMBAL_INTERFACE_H

#include "gimbal_class.h"
#include "gimbal_instance.h"

#define GBL_INTERFACE_TYPE                      GBL_BUILTIN_TYPE(INTERFACE)
#define GBL_INTERFACE_CLASS_STRUCT              GblInterface
#define GBL_INTERFACE(klass)                    (GblInterface_tryFromClass(GBL_CLASS(klass)))
#define GBL_INTERFACE_OUTER_CLASS(klass)        (GblInterface_outerClass(GBL_CLASS(klass)))
#define GBL_INTERFACE_OUTER_MOST_CLASS(klass)   (GblInterface_outerMostClass(GBL_INTERFACE(klass)))

#define SELF    GblInterface* pSelf
#define CSELF   const GblInterface* pSelf

GBL_DECLS_BEGIN

typedef struct GblInterface {
    GblClass    base;
    int16_t     outerClassOffset;
} GblInterface;

GBL_INLINE GblInterface* GblInterface_peek              (GblClass* pClass, GblType ifaceType)   GBL_NOEXCEPT;
GBL_INLINE GblInterface* GblInterface_try               (GblClass* pClass)                      GBL_NOEXCEPT;

GBL_INLINE int16_t       GblInterface_outerClassOffset  (CSELF)                                 GBL_NOEXCEPT;
GBL_INLINE GblClass*     GblInterface_outerClass        (SELF)                                  GBL_NOEXCEPT;
GBL_INLINE GblClass*     GblInterface_outerMostClass    (SELF)                                  GBL_NOEXCEPT;

// ========== IMPL ==========

GBL_INLINE int16_t GblInterface_outerClassOffset(CSELF) GBL_NOEXCEPT {
    return pSelf? pSelf->outerClassOffset : 0;
}

GBL_INLINE GblClass* GblInterface_outerClass(SELF) GBL_NOEXCEPT {
    return pSelf? (GblClass*)((uintptr_t)pSelf + pSelf->outerClassOffset) : NULL;
}

GBL_INLINE GblClass* GblInterface_outerMostClass(SELF) GBL_NOEXCEPT {
    GblClass* pClass = GBL_CLASS(pSelf);
    while(GblClass_isInterfaceImpl(pClass)) {
        pClass = GblInterface_outerClass((GblInterface*)pClass);
    }
    return pClass;
}

GBL_INLINE GblInterface* GblInterface_try(GblClass* pClass) GBL_NOEXCEPT {
    return GblClass_isInterfaceImpl(pClass)? (GblInterface*)pClass : NULL;
}

GBL_INLINE GblInterface* GblInterface_peek(GblClass* pClass, GblType ifaceType) GBL_NOEXCEPT {
    return (GblInterface*)GblClass_try(pClass, ifaceType);
}


GBL_DECLS_END

#undef CSELF
#undef SELF

#endif // GIMBAL_INTERFACE_H

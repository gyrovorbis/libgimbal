#include <gimbal/meta/gimbal_interface.h>

GBL_EXPORT GblClass* GblInterface_outerClass(GblInterface* pSelf) GBL_NOEXCEPT {
    return pSelf? (GblClass*)((uintptr_t)pSelf + pSelf->outerClassOffset) : NULL;
}

GBL_EXPORT GblClass* GblInterface_outerMostClass(GblInterface* pSelf) GBL_NOEXCEPT {
    GblClass* pClass = GBL_CLASS(pSelf);
    while(GblClass_isInterfaceImpl(pClass)) {
        pClass = GblInterface_outerClass((GblInterface*)pClass);
    }
    return pClass;
}

GBL_EXPORT GblInterface* GblInterface_try(GblClass* pClass) GBL_NOEXCEPT {
    return GblClass_isInterfaceImpl(pClass)? (GblInterface*)pClass : NULL;
}

GBL_EXPORT GblInterface* GblInterface_peek(GblClass* pClass, GblType ifaceType) GBL_NOEXCEPT {
    return (GblInterface*)GblClass_try(pClass, ifaceType);
}

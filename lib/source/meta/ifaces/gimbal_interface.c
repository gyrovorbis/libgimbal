#include <gimbal/meta/ifaces/gimbal_interface.h>

GBL_EXPORT GblClass* GblInterface_outerClass(GblInterface* pSelf) {
    return pSelf? (GblClass*)((uintptr_t)pSelf + pSelf->outerClassOffset_) : NULL;
}

GBL_EXPORT GblClass* GblInterface_outerMostClass(GblInterface* pSelf){
    GblClass* pClass = GBL_CLASS(pSelf);
    while(GblClass_isInterfaceImpl(pClass)) {
        pClass = GblInterface_outerClass((GblInterface*)pClass);
    }
    return pClass;
}

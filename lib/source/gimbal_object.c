#include <gimbal/meta/gimbal_object.h>
#include <gimbal/types/gimbal_variant.h>



static GBL_RESULT GblObjectClass_constructor(GblObject* pSelf) {
    pSelf->refCounter = 1;
}

static GBL_RESULT GblObjectClass_destructor(GblObject* pSelf) {
    // remove from parent
    // remove children
}

GBL_API GblObjectClass_init(GblObjectClass* pClass, GblObjectClassData* pData, GblContext hCtx) GBL_NOEXCEPT {

}

GBL_API GblObjectClass_final(GblObjectClass* pClass, GblContext hCtx) GBL_NOEXCEPT {

}

GBL_API GblObjectClass_propertyInstall(GblObjectClass* pClass, const GblObjectPropertyInfo* pInfo) GBL_NOEXCEPT {

}

GBL_API GblObject_init(GblInstance* pInstance, GblContext hCtx) {
    GBL_API_BEGIN(hCtx); {
        GblObjectClass* pClass = GBL_OBJECT_GET_CLASS(pInstance);
        GBL_API_CALL(pClass->pFnConstructor(GBL_OBJECT(pInstance)));
    } GBL_API_END();
}


GBL_EXPORT GblObject* GblObject_new(GblType type, ...) GBL_NOEXCEPT {
    GblObject* pObject = NULL;
    va_list    varArgs;
    va_start(varArgs, type);
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_TYPE(type, GBL_TYPE_OBJECT);

    pObject = GBL_OBJECT(gblTypeInstanceCreate(type));
    GBL_API_VERIFY_POINTER(pObject);

#if 0
    // not gonna work, we have to specifically check for CONSTRUCTABLE flag
    GBL_API_CALL(GblObject_setVaList(pObject, varArgs));

 #else
    GblObjectClass* pClass = GBL_OBJECT_GET_CLASS(pObject);

    const char* pKey = NULL;
    while((pKey = va_arg(varArgs, const char*))) {
        GblObjectPropertyInfo* pProp = GblObjectClass_propertyFind(pClass, pKey);
        if(pProp && (pProp->flags & GBL_OBJECT_PROPERTY_FLAG_CONSTRUCT)) {
            GblVariant variant;
            GBL_API_CALL(GblVariant_constructValueCopyVaList(&variant, type, varArgs));
            // CAN BE MOVE SEMANTICS
            GBL_API_CALL(pClass->pFnPropertySet(pObject, pProp->slot, &variant, pProp));
        } else {
            // Can't recover if we don't know how to parse after...
            GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY,
                               "Attempted to set unknown property: %s", pKey);
        }

    }
#endif

    pClass->pFnConstructed(pObject);

    GBL_API_END_BLOCK();
    va_end(varArgs);
    return pObject;
}

GBL_EXPORT GblObject* GblObject_ref(GblObject* pObject) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pObject);
    if(pObject) {
       ++pObject->refCounter;
    }
    GBL_API_END_BLOCK();
    return pObject;
}

GBL_EXPORT GblRefCount GblObject_unref(GblObject* pObject) GBL_NOEXCEPT {
    GblRefCount count = 0;
    GBL_API_BEGIN(NULL);
    if(pObject) {
        GBL_API_VERIFY_EXPRESSION(pObject->refCounter);
        if(--pObject->refCounter == 0) {
            GblObjectClass* pClass = GBL_OBJECT_GET_CLASS(pObject);
            pClass->pFnDestructor(pObject);
            gblTypeInstanceDestroy(&pObject->base);
        } else {
            count = pObject->refCounter;
        }
    }
    GBL_API_END_BLOCK();
    return count;
}

GBL_EXPORT GblRefCount GblObject_refCount(const GblObject* pObject) GBL_NOEXCEPT {
    return pObject->refCounter;
}


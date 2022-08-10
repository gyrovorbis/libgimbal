#include <gimbal/meta/gimbal_record.h>
#include "gimbal_type_.h"

GBL_EXPORT GblRefCounted* GblRefCounted_create(GblType derivedType) {
    GblRefCounted* pRecord = NULL;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_TYPE(derivedType, GBL_REF_COUNTED_TYPE);
    pRecord = (GblRefCounted*)GblInstance_create(derivedType);
    GBL_API_END_BLOCK();
    return pRecord;
}

GBL_EXPORT GblRefCounted* GblRefCounted_createWithClass(GblRefCountedClass* pClass) {
    GblRefCounted* pRecord = NULL;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pClass);
    GBL_API_VERIFY_TYPE(GBL_CLASS_TYPEOF(pClass), GBL_REF_COUNTED_TYPE);
    pRecord = (GblRefCounted*)GblInstance_createWithClass((GblClass*)pClass);
    GBL_API_END_BLOCK();
    return pRecord;
}

GBL_EXPORT GBL_RESULT GblRefCounted_construct(GblRefCounted* pSelf, GblType derivedType) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_TYPE(derivedType, GBL_REF_COUNTED_TYPE);
    GBL_API_VERIFY_CALL(GblInstance_construct((GblInstance*)pSelf, derivedType));
    GBL_PRIV_REF(pSelf).constructedInPlace = 1;
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblRefCounted_constructWithClass(GblRefCounted* pSelf,
                                                   GblRefCountedClass* pClass)
{
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pClass);
    GBL_API_VERIFY_TYPE(GBL_CLASS_TYPEOF(pClass), GBL_REF_COUNTED_TYPE);
    GBL_API_VERIFY_CALL(GblInstance_constructWithClass((GblInstance*)pSelf,
                                                       (GblClass*)pClass));
    GBL_PRIV_REF(pSelf).constructedInPlace = 1;
    GBL_API_END();
}

GBL_EXPORT GblRefCounted* GblRefCounted_ref(GblRefCounted* pSelf) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);
    if(pSelf) {
       GBL_ATOMIC_INT16_INC(GBL_PRIV_REF(pSelf).refCounter);
    }
    GBL_API_END_BLOCK();
    return pSelf;
}

GBL_EXPORT GblRefCount GblRefCounted_unref(GblRefCounted* pSelf) {
    GblRefCount count = 0;
    GBL_API_BEGIN(NULL);
    if(pSelf) {
        GBL_API_VERIFY_EXPRESSION(GBL_PRIV_REF(pSelf).refCounter);
        if((count = GBL_ATOMIC_INT16_DEC(GBL_PRIV_REF(pSelf).refCounter)-1) == 0) {
            GblRefCountedClass* pClass = GBL_REF_COUNTED_GET_CLASS(pSelf);
            GBL_API_CALL(pClass->pFnDestructor(pSelf));

            if(!GBL_PRIV_REF(pSelf).constructedInPlace)
                GBL_API_CALL(GblInstance_destroy(&pSelf->base));
            else
                GBL_API_CALL(GblInstance_destruct(&pSelf->base));
        }
    }
    GBL_API_END_BLOCK();
    return count;
}

GBL_EXPORT GblRefCount GblRefCounted_refCount(const GblRefCounted* pSelf) {
    return GBL_ATOMIC_INT16_LOAD(GBL_PRIV_REF(pSelf).refCounter);
}

static GBL_RESULT GblRefCounted_init_(GblInstance* pInstance, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_ATOMIC_INT16_INIT(GBL_PRIV_REF((GblRefCounted*)pInstance).refCounter, 1);
    GBL_API_END();
}

static GBL_RESULT GblRefCounted_destructor_(GblRefCounted* pSelf) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_CALL(GblArrayMap_destroy(&GBL_PRIV_REF(pSelf).pFields));
    GBL_API_END();
}

static GBL_RESULT GblRefCountedClass_init_(GblClass* pClass, const void* pUd, GblContext* pCtx) {
    GBL_UNUSED(pUd);
    GBL_API_BEGIN(pCtx);
    GblRefCountedClass* pSelfClass = (GblRefCountedClass*)pClass;
    pSelfClass->pFnDestructor = GblRefCounted_destructor_;
    GBL_API_END();
}

static GBL_RESULT GblRefCountedClass_final_(GblClass* pClass, const void* pUd, GblContext* pCtx) {
    GBL_UNUSED(pUd);
    GBL_API_BEGIN(pCtx);
    GblRefCountedClass* pSelfClass = (GblRefCountedClass*)pClass;
    GBL_API_VERIFY_CALL(GblArrayMap_destroy(&GBL_PRIV_REF(pSelfClass).pFields));
    GBL_API_END();

}

GBL_EXPORT GBL_RESULT GblRefCounted_typeRegister_(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    static GblTypeInterfaceMapEntry ifaceMap[] = {
        {
            GBL_INVALID_TYPE,
            offsetof(GblRefCountedClass, GblIVariantIFaceImpl)
        }
    };

    static const GblTypeInfo typeInfo = {
        .classSize          = sizeof(GblRefCountedClass),
        .pFnClassInit       = GblRefCountedClass_init_,
        .pFnClassFinal      = GblRefCountedClass_final_,
        .instanceSize       = sizeof(GblRefCounted),
        .pFnInstanceInit    = GblRefCounted_init_,
        .interfaceCount     = 1,
        .pInterfaceMap      = ifaceMap
    };


    ifaceMap[0].interfaceType = GBL_IVARIANT_TYPE;

    GblType_registerBuiltin_(GBL_TYPE_BUILTIN_INDEX_RECORD,
                             GBL_INSTANCE_TYPE,
                             GblQuark_internStringStatic("Record"),
                             &typeInfo,
                             GBL_TYPE_FLAG_TYPEINFO_STATIC);

    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END();
}

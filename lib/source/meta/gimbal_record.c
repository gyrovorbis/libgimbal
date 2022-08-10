#include <gimbal/meta/gimbal_record.h>
#include "gimbal_type_.h"

GBL_EXPORT GblRecord* GblRecord_create(GblType derivedType) {
    GblRecord* pRecord = NULL;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_TYPE(derivedType, GBL_RECORD_TYPE);
    pRecord = (GblRecord*)GblInstance_create(derivedType);
    GBL_API_END_BLOCK();
    return pRecord;
}

GBL_EXPORT GblRecord* GblRecord_createWithClass(GblRecordClass* pClass) {
    GblRecord* pRecord = NULL;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pClass);
    GBL_API_VERIFY_TYPE(GBL_CLASS_TYPEOF(pClass), GBL_RECORD_TYPE);
    pRecord = (GblRecord*)GblInstance_createWithClass((GblClass*)pClass);
    GBL_API_END_BLOCK();
    return pRecord;
}

GBL_EXPORT GBL_RESULT GblRecord_construct(GblRecord* pSelf, GblType derivedType) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_TYPE(derivedType, GBL_RECORD_TYPE);
    GBL_API_VERIFY_CALL(GblInstance_construct((GblInstance*)pSelf, derivedType));
    GBL_PRIV_REF(pSelf).constructedInPlace = 1;
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblRecord_constructWithClass(GblRecord* pSelf,
                                                   GblRecordClass* pClass)
{
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pClass);
    GBL_API_VERIFY_TYPE(GBL_CLASS_TYPEOF(pClass), GBL_RECORD_TYPE);
    GBL_API_VERIFY_CALL(GblInstance_constructWithClass((GblInstance*)pSelf,
                                                       (GblClass*)pClass));
    GBL_PRIV_REF(pSelf).constructedInPlace = 1;
    GBL_API_END();
}

GBL_EXPORT GblRecord* GblRecord_ref(GblRecord* pSelf) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);
    if(pSelf) {
       GBL_ATOMIC_INT16_INC(GBL_PRIV_REF(pSelf).refCounter);
    }
    GBL_API_END_BLOCK();
    return pSelf;
}

GBL_EXPORT GblRefCount GblRecord_unref(GblRecord* pSelf) {
    GblRefCount count = 0;
    GBL_API_BEGIN(NULL);
    if(pSelf) {
        GBL_API_VERIFY_EXPRESSION(GBL_PRIV_REF(pSelf).refCounter);
        if((count = GBL_ATOMIC_INT16_DEC(GBL_PRIV_REF(pSelf).refCounter)-1) == 0) {
            GblRecordClass* pClass = GBL_RECORD_GET_CLASS(pSelf);
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

GBL_EXPORT GblRefCount GblRecord_refCount(const GblRecord* pSelf) {
    return GBL_ATOMIC_INT16_LOAD(GBL_PRIV_REF(pSelf).refCounter);
}

static GBL_RESULT GblRecord_init_(GblInstance* pInstance, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_ATOMIC_INT16_INIT(GBL_PRIV_REF((GblRecord*)pInstance).refCounter, 1);
    GBL_API_END();
}

static GBL_RESULT GblRecord_destructor_(GblRecord* pSelf) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_CALL(GblArrayMap_destroy(&GBL_PRIV_REF(pSelf).pFields));
    GBL_API_END();
}

static GBL_RESULT GblRecordClass_init_(GblClass* pClass, const void* pUd, GblContext* pCtx) {
    GBL_UNUSED(pUd);
    GBL_API_BEGIN(pCtx);
    GblRecordClass* pSelfClass = (GblRecordClass*)pClass;
    pSelfClass->pFnDestructor = GblRecord_destructor_;
    GBL_API_END();
}

static GBL_RESULT GblRecordClass_final_(GblClass* pClass, const void* pUd, GblContext* pCtx) {
    GBL_UNUSED(pUd);
    GBL_API_BEGIN(pCtx);
    GblRecordClass* pSelfClass = (GblRecordClass*)pClass;
    GBL_API_VERIFY_CALL(GblArrayMap_destroy(&GBL_PRIV_REF(pSelfClass).pFields));
    GBL_API_END();

}

GBL_EXPORT GBL_RESULT GblRecord_typeRegister_(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    static GblTypeInterfaceMapEntry ifaceMap[] = {
        {
            GBL_INVALID_TYPE,
            offsetof(GblRecordClass, GblIVariantIFaceImpl)
        }
    };

    static const GblTypeInfo typeInfo = {
        .classSize          = sizeof(GblRecordClass),
        .pFnClassInit       = GblRecordClass_init_,
        .pFnClassFinal      = GblRecordClass_final_,
        .instanceSize       = sizeof(GblRecord),
        .pFnInstanceInit    = GblRecord_init_,
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

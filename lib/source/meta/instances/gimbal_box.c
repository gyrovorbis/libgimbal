#include <gimbal/meta/instances/gimbal_box.h>
#include "../types/gimbal_type_.h"

#define GBL_BOX_FIELD_KEY_DESTRUCTOR   "_dtor"
#define GBL_BOX_FIELD_KEY_USERDATA     "_ud"

GblQuark fieldKeyDtor_  = GBL_QUARK_INVALID;
GblQuark fieldKeyUd_    = GBL_QUARK_INVALID;

GBL_EXPORT void* GblBox_userdata(const GblBox* pSelf) {
    return (void*)GblArrayMap_getValue(&GBL_PRIV_REF(pSelf).pFields, fieldKeyUd_);
}

GBL_EXPORT GBL_RESULT GblBox_setUserdata(GblBox* pSelf, void* pUserdata) {
    if(pUserdata) {
        return GblArrayMap_setUserdata(&GBL_PRIV_REF(pSelf).pFields,
                                       fieldKeyUd_,
                                       (uintptr_t)pUserdata,
                                       NULL);
    } else {
        return GblArrayMap_erase(&GBL_PRIV_REF(pSelf).pFields,
                                 fieldKeyUd_);
    }
}

GBL_EXPORT GBL_RESULT GblBox_setUserDestructor_(GblBox* pSelf, GblArrayMapDtorFn pFnDtor) {
    if(pFnDtor) {
        return GblArrayMap_setUserdata(&GBL_PRIV_REF(pSelf).pFields,
                                       fieldKeyDtor_,
                                       (uintptr_t)pSelf,
                                       pFnDtor);
    } else {
        return GblArrayMap_erase(&GBL_PRIV_REF(pSelf).pFields,
                                 fieldKeyDtor_);
    }
}

GBL_EXPORT GblBox* GblBox_create(GblType derivedType) {
    GblBox* pRecord = NULL;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_TYPE(derivedType, GBL_BOX_TYPE);
    pRecord = (GblBox*)GblInstance_create(derivedType);
    GBL_API_END_BLOCK();
    return pRecord;
}

GBL_EXPORT GblBox* GblBox_createWithClass(GblBoxClass* pClass) {
    GblBox* pRecord = NULL;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pClass);
    GBL_API_VERIFY_TYPE(GBL_CLASS_TYPEOF(pClass), GBL_BOX_TYPE);
    pRecord = (GblBox*)GblInstance_createWithClass((GblClass*)pClass);
    GBL_API_END_BLOCK();
    return pRecord;
}

GBL_EXPORT GBL_RESULT GblBox_construct(GblBox* pSelf, GblType derivedType) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_TYPE(derivedType, GBL_BOX_TYPE);
    GBL_API_VERIFY_CALL(GblInstance_construct((GblInstance*)pSelf, derivedType));
    GBL_PRIV_REF(pSelf).constructedInPlace = 1;
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblBox_constructWithClass(GblBox* pSelf,
                                                GblBoxClass* pClass)
{
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pClass);
    GBL_API_VERIFY_TYPE(GBL_CLASS_TYPEOF(pClass), GBL_BOX_TYPE);
    GBL_API_VERIFY_CALL(GblInstance_constructWithClass((GblInstance*)pSelf,
                                                       (GblClass*)pClass));
    GBL_PRIV_REF(pSelf).constructedInPlace = 1;
    GBL_API_END();
}

GBL_EXPORT GblBox* GblBox_createExt(GblType           derivedType,
                                    GblSize           totalSize,
                                    void*             pUserdata,
                                    GblArrayMapDtorFn pFnDtor,
                                    ...)
{
    GblBox* pBox = NULL;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_TYPE(derivedType, GBL_BOX_TYPE);
    GBL_API_VERIFY_ARG(totalSize >= sizeof(GblBox));
    pBox = GBL_API_MALLOC(totalSize, 0, GblType_name(derivedType));
    GBL_API_VERIFY_CALL(GblInstance_construct((GblInstance*)pBox, derivedType));
    if(pFnDtor) GBL_API_VERIFY_CALL(GblBox_setUserDestructor_(pBox, pFnDtor));
    if(pUserdata) GBL_API_VERIFY_CALL(GblBox_setUserdata(pBox, pUserdata));

    GBL_API_END_BLOCK();
    return pBox;
}


GBL_EXPORT GblBox* GblBox_ref(GblBox* pSelf) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);
    if(pSelf) {
       GBL_ATOMIC_INT16_INC(GBL_PRIV_REF(pSelf).refCounter);
    }
    GBL_API_END_BLOCK();
    return pSelf;
}

GBL_EXPORT GblRefCount GblBox_unref(GblBox* pSelf) {
    GblRefCount count = 0;
    GBL_API_BEGIN(NULL);
    if(pSelf) {
        GBL_API_VERIFY_EXPRESSION(GBL_PRIV_REF(pSelf).refCounter);
        if((count = GBL_ATOMIC_INT16_DEC(GBL_PRIV_REF(pSelf).refCounter)-1) == 0) {
            GblBoxClass* pClass = GBL_BOX_GET_CLASS(pSelf);
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

GBL_EXPORT GblRefCount GblBox_refCount(const GblBox* pSelf) {
    return GBL_ATOMIC_INT16_LOAD(GBL_PRIV_REF(pSelf).refCounter);
}

static GBL_RESULT GblBox_init_(GblInstance* pInstance, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_ATOMIC_INT16_INIT(GBL_PRIV_REF((GblBox*)pInstance).refCounter, 1);
    GBL_API_END();
}

static GBL_RESULT GblBox_destructor_(GblBox* pSelf) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_CALL(GblArrayMap_destroy(&GBL_PRIV_REF(pSelf).pFields));
    GBL_API_END();
}

static GBL_RESULT GblBoxClass_init_(GblClass* pClass, const void* pUd, GblContext* pCtx) {
    GBL_UNUSED(pUd);
    GBL_API_BEGIN(pCtx);
    fieldKeyDtor_   = GblQuark_fromStringStatic(GBL_BOX_FIELD_KEY_DESTRUCTOR);
    fieldKeyUd_     = GblQuark_fromStringStatic(GBL_BOX_FIELD_KEY_USERDATA);
    GblBoxClass* pSelfClass = (GblBoxClass*)pClass;
    pSelfClass->pFnDestructor = GblBox_destructor_;
    GBL_API_END();
}

static GBL_RESULT GblBoxClass_final_(GblClass* pClass, const void* pUd, GblContext* pCtx) {
    GBL_UNUSED(pUd);
    GBL_API_BEGIN(pCtx);
    GblBoxClass* pSelfClass = (GblBoxClass*)pClass;
    GBL_API_VERIFY_CALL(GblArrayMap_destroy(&GBL_PRIV_REF(pSelfClass).pFields));
    GBL_API_END();

}

GBL_EXPORT GBL_RESULT GblBox_typeRegister_(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    static GblTypeInterfaceMapEntry ifaceMap[] = {
        {
            GBL_INVALID_TYPE,
            offsetof(GblBoxClass, GblIVariantIFaceImpl)
        }
    };

    static const GblTypeInfo typeInfo = {
        .classSize          = sizeof(GblBoxClass),
        .pFnClassInit       = GblBoxClass_init_,
        .pFnClassFinal      = GblBoxClass_final_,
        .instanceSize       = sizeof(GblBox),
        .pFnInstanceInit    = GblBox_init_,
        .interfaceCount     = 1,
        .pInterfaceMap      = ifaceMap
    };


    ifaceMap[0].interfaceType = GBL_IVARIANT_TYPE;

    GblType_registerBuiltin_(GBL_TYPE_BUILTIN_INDEX_BOX,
                             GBL_INSTANCE_TYPE,
                             GblQuark_internStringStatic("GblBox"),
                             &typeInfo,
                             GBL_TYPE_FLAG_TYPEINFO_STATIC);

    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END();
}

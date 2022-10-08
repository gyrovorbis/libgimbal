#include <gimbal/meta/instances/gimbal_box.h>
#include "../types/gimbal_type_.h"

#define GBL_BOX_FIELD_KEY_DESTRUCTOR   "_dtor"
#define GBL_BOX_FIELD_KEY_USERDATA     "_ud"

GblQuark fieldKeyDtor_  = GBL_QUARK_INVALID;
GblQuark fieldKeyUd_    = GBL_QUARK_INVALID;

GBL_EXPORT GBL_RESULT GblBox_setUserDestructor(GblBox* pSelf, GblArrayMapDtorFn pFnDtor) {
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

GBL_EXPORT uintptr_t GblBox_getField(const GblBox* pSelf, GblQuark key) {
    return GblArrayMap_getValue(&GBL_PRIV_REF(pSelf).pFields, key);
}

GBL_EXPORT uintptr_t GblBox_takeField(GblBox* pSelf, GblQuark key) {
    uintptr_t value = 0;
    GblArrayMap_extractValue(&GBL_PRIV_REF(pSelf).pFields, key, &value);
    return value;
}

GBL_EXPORT GblBool GblBox_clearField(GblBox* pSelf, GblQuark key) {
    return GblArrayMap_erase(&GBL_PRIV_REF(pSelf).pFields, key);
}

GBL_EXPORT GblBool GblBox_hasField(const GblBox* pSelf, GblQuark key) {
    return GblArrayMap_contains(&GBL_PRIV_REF(pSelf).pFields, key);
}

GBL_EXPORT GBL_RESULT GblBox_setField(GblBox*           pSelf,
                                      GblQuark          key,
                                      uintptr_t         ud,
                                      GblArrayMapDtorFn pFnDtor)
{
    return GblArrayMap_setUserdata(&GBL_PRIV_REF(pSelf).pFields, key, ud, pFnDtor);
}

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

GBL_EXPORT GblBox* GblBox_create(GblType derivedType) {
    GblBox* pRecord = NULL;
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_TYPE(derivedType, GBL_BOX_TYPE);
    pRecord = (GblBox*)GblInstance_create(derivedType);
    GBL_CTX_END_BLOCK();
    return pRecord;
}

GBL_EXPORT GblBox* GblBox_createWithClass(GblBoxClass* pClass) {
    GblBox* pRecord = NULL;
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_POINTER(pClass);
    GBL_CTX_VERIFY_TYPE(GBL_CLASS_TYPEOF(pClass), GBL_BOX_TYPE);
    pRecord = (GblBox*)GblInstance_createWithClass((GblClass*)pClass);
    GBL_CTX_END_BLOCK();
    return pRecord;
}

GBL_EXPORT GBL_RESULT GblBox_construct(GblBox* pSelf, GblType derivedType) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_TYPE(derivedType, GBL_BOX_TYPE);
    GBL_CTX_VERIFY_CALL(GblInstance_construct((GblInstance*)pSelf, derivedType));
    GBL_PRIV_REF(pSelf).constructedInPlace = 1;
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblBox_constructWithClass(GblBox* pSelf,
                                                GblBoxClass* pClass)
{
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_POINTER(pClass);
    GBL_CTX_VERIFY_TYPE(GBL_CLASS_TYPEOF(pClass), GBL_BOX_TYPE);
    GBL_CTX_VERIFY_CALL(GblInstance_constructWithClass((GblInstance*)pSelf,
                                                       (GblClass*)pClass));
    GBL_PRIV_REF(pSelf).constructedInPlace = 1;
    GBL_CTX_END();
}

GBL_EXPORT GblBox* GblBox_createExt(GblType           derivedType,
                                    GblSize           totalSize,
                                    void*             pUserdata,
                                    GblArrayMapDtorFn pFnUdDtor)
{
    GblBox* pBox = NULL;
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_ARG(totalSize >= sizeof(GblBox));
    pBox = GBL_CTX_MALLOC(totalSize, 0, GblType_name(derivedType));
    GBL_CTX_CALL(GblBox_constructExt(pBox, derivedType, pUserdata, pFnUdDtor));
    GBL_PRIV_REF(pBox).constructedInPlace = 0;
    GBL_CTX_END_BLOCK();
    return pBox;
}

GBL_EXPORT GBL_RESULT GblBox_constructExt(GblBox*           pSelf,
                                          GblType           derivedType,
                                          void*             pUserdata,
                                          GblArrayMapDtorFn pFnUdDtor)
{
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_POINTER(pSelf);
    GBL_CTX_VERIFY_TYPE(derivedType, GBL_BOX_TYPE);
    GBL_CTX_VERIFY_CALL(GblInstance_construct((GblInstance*)pSelf, derivedType));
    if(pFnUdDtor) GBL_CTX_VERIFY_CALL(GblBox_setUserDestructor(pSelf, pFnUdDtor));
    if(pUserdata) GBL_CTX_VERIFY_CALL(GblBox_setUserdata(pSelf, pUserdata));
    GBL_PRIV_REF(pSelf).constructedInPlace = 1;
    GBL_CTX_END();
}

GBL_EXPORT GblBox* GblBox_createExtWithClass(GblBoxClass*      pClass,
                                             GblSize           totalSize,
                                             void*             pUserdata,
                                             GblArrayMapDtorFn pFnUdDtor)
{
    GblBox* pBox = NULL;
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_ARG(totalSize >= sizeof(GblBox));
    pBox = GBL_CTX_MALLOC(totalSize, 0, GblType_name(GBL_CLASS_TYPEOF(pClass)));
    GBL_CTX_CALL(GblBox_constructExtWithClass(pBox, pClass, pUserdata, pFnUdDtor));
    GBL_PRIV_REF(pBox).constructedInPlace = 0;
    GBL_CTX_END_BLOCK();
    return pBox;
}

GBL_EXPORT GBL_RESULT GblBox_constructExtWithClass(GblBox*           pSelf,
                                                   GblBoxClass*      pClass,
                                                   void*             pUserdata,
                                                   GblArrayMapDtorFn pFnUdDtor)
{
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_POINTER(pSelf && pClass);
    GBL_CTX_VERIFY_TYPE(GBL_CLASS_TYPEOF(pClass), GBL_BOX_TYPE);
    GBL_CTX_VERIFY_CALL(GblInstance_constructWithClass(GBL_INSTANCE(pSelf), GBL_CLASS(pClass)));
    if(pFnUdDtor) GBL_CTX_VERIFY_CALL(GblBox_setUserDestructor(pSelf, pFnUdDtor));
    if(pUserdata) GBL_CTX_VERIFY_CALL(GblBox_setUserdata(pSelf, pUserdata));
    GBL_PRIV_REF(pSelf).constructedInPlace = 1;
    GBL_CTX_END();
}


GBL_EXPORT GblBox* GblBox_ref(GblBox* pSelf) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_POINTER(pSelf);
    if(pSelf) {
       GBL_ATOMIC_INT16_INC(GBL_PRIV_REF(pSelf).refCounter);
    }
    GBL_CTX_END_BLOCK();
    return pSelf;
}

GBL_EXPORT GblRefCount GblBox_unref(GblBox* pSelf) {
    GblRefCount count = 0;
    GBL_CTX_BEGIN(NULL);
    if(pSelf) {
        GBL_CTX_VERIFY_EXPRESSION(GBL_PRIV_REF(pSelf).refCounter);
        if((count = GBL_ATOMIC_INT16_DEC(GBL_PRIV_REF(pSelf).refCounter)-1) == 0) {
            GblBoxClass* pClass = GBL_BOX_GET_CLASS(pSelf);
            GBL_CTX_CALL(pClass->pFnDestructor(pSelf));

            if(!GBL_PRIV_REF(pSelf).constructedInPlace)
                GBL_CTX_CALL(GblInstance_destroy(&pSelf->base));
            else
                GBL_CTX_CALL(GblInstance_destruct(&pSelf->base));
        }
    }
    GBL_CTX_END_BLOCK();
    return count;
}

GBL_EXPORT GblRefCount GblBox_refCount(const GblBox* pSelf) {
    return GBL_ATOMIC_INT16_LOAD(GBL_PRIV_REF(pSelf).refCounter);
}

static GBL_RESULT GblBox_init_(GblInstance* pInstance, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GBL_ATOMIC_INT16_INIT(GBL_PRIV_REF((GblBox*)pInstance).refCounter, 1);
    GBL_CTX_END();
}

static GBL_RESULT GblBox_destructor_(GblBox* pSelf) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblArrayMap_destroy(&GBL_PRIV_REF(pSelf).pFields));
    GBL_CTX_END();
}

GBL_EXPORT void* GblBoxClass_userdata(const GblBoxClass* pSelf) {
    return (void*)GblArrayMap_getValue(&GBL_PRIV_REF(pSelf).pFields, fieldKeyUd_);
}

GBL_EXPORT GBL_RESULT GblBoxClass_setUserDestructor(GblBoxClass* pSelf, GblArrayMapDtorFn pFnDtor) {
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

GBL_EXPORT GBL_RESULT GblBoxClass_setUserdata(GblBoxClass* pSelf, void* pUserdata) {
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

GBL_EXPORT uintptr_t GblBoxClass_getField(const GblBoxClass* pSelf, GblQuark key) {
    return GblArrayMap_getValue(&GBL_PRIV_REF(pSelf).pFields, key);
}

GBL_EXPORT uintptr_t GblBoxClass_takeField(GblBoxClass* pSelf, GblQuark key) {
    uintptr_t value = 0;
    GblArrayMap_extractValue(&GBL_PRIV_REF(pSelf).pFields, key, &value);
    return value;
}

GBL_EXPORT GblBool GblBoxClass_clearField(GblBoxClass* pSelf, GblQuark key) {
    return GblArrayMap_erase(&GBL_PRIV_REF(pSelf).pFields, key);
}

GBL_EXPORT GblBool GblBoxClass_hasField(const GblBoxClass* pSelf, GblQuark key) {
    return GblArrayMap_contains(&GBL_PRIV_REF(pSelf).pFields, key);
}

GBL_EXPORT GBL_RESULT GblBoxClass_setField(GblBoxClass*      pSelf,
                                           GblQuark          key,
                                           uintptr_t         ud,
                                           GblArrayMapDtorFn pFnDtor)
{
    return GblArrayMap_setUserdata(&GBL_PRIV_REF(pSelf).pFields, key, ud, pFnDtor);
}


GBL_EXPORT GblBoxClass* GblBoxClass_createFloatingExt(GblType           derivedType,
                                                      GblSize           totalSize,
                                                      void*             pUserdata,
                                                      GblArrayMapDtorFn pFnUdDtor)
{
    GblBoxClass* pClass = NULL;
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_TYPE(derivedType, GBL_BOX_TYPE);
    GBL_CTX_VERIFY_ARG(totalSize >= sizeof(GblBoxClass));
    pClass = GBL_CTX_MALLOC(totalSize, 0, GblType_name(derivedType));
    GBL_CTX_CALL(GblBoxClass_constructFloatingExt(pClass, derivedType, pUserdata, pFnUdDtor));
    GBL_CLASS_FLAG_CLEAR_(GBL_CLASS(pClass), GBL_CLASS_FLAG_IN_PLACE_);
    GBL_CTX_END_BLOCK();
    return pClass;
}

GBL_EXPORT GBL_RESULT GblBoxClass_constructFloatingExt(GblBoxClass*      pSelf,
                                                       GblType           derivedType,
                                                       void*             pUserdata,
                                                       GblArrayMapDtorFn pFnUdDtor)
{
    GBL_CTX_BEGIN(NULL);
    GblClass_constructFloating(GBL_CLASS(pSelf), derivedType);
    if(pUserdata) GBL_CTX_VERIFY_CALL(GblBoxClass_setUserdata(pSelf, pUserdata));
    if(pFnUdDtor) GBL_CTX_VERIFY_CALL(GblBoxClass_setUserDestructor(pSelf, pFnUdDtor));
    GBL_CTX_END();
}

static GBL_RESULT GblBoxClass_init_(GblClass* pClass, const void* pUd, GblContext* pCtx) {
    GBL_UNUSED(pUd);
    GBL_CTX_BEGIN(pCtx);
    fieldKeyDtor_   = GblQuark_fromStringStatic(GBL_BOX_FIELD_KEY_DESTRUCTOR);
    fieldKeyUd_     = GblQuark_fromStringStatic(GBL_BOX_FIELD_KEY_USERDATA);
    GblBoxClass* pSelfClass = (GblBoxClass*)pClass;
    pSelfClass->pFnDestructor = GblBox_destructor_;
    GBL_CTX_END();
}

static GBL_RESULT GblBoxClass_final_(GblClass* pClass, const void* pUd, GblContext* pCtx) {
    GBL_UNUSED(pUd);
    GBL_CTX_BEGIN(pCtx);
    GblBoxClass* pSelfClass = (GblBoxClass*)pClass;
    GBL_CTX_VERIFY_CALL(GblArrayMap_destroy(&GBL_PRIV_REF(pSelfClass).pFields));
    GBL_CTX_END();

}

GBL_EXPORT GblType GblBox_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static GblTypeInterfaceMapEntry ifaceMap[] = {
        {
            GBL_INVALID_TYPE,
            offsetof(GblBoxClass, GblIVariantImpl)
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

    if(type == GBL_INVALID_TYPE) GBL_UNLIKELY {
        GBL_CTX_BEGIN(NULL);

        ifaceMap[0].interfaceType = GBL_IVARIANT_TYPE;

        type = GblType_registerStatic(GblQuark_internStringStatic("GblBox"),
                                      GBL_INSTANCE_TYPE,
                                      &typeInfo,
                                      GBL_TYPE_FLAG_TYPEINFO_STATIC);

        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }
    return type;
}

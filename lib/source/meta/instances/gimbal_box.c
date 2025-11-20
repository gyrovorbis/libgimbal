#include <gimbal/meta/instances/gimbal_box.h>
#include <gimbal/meta/instances/gimbal_object.h>
#include <gimbal/meta/signals/gimbal_marshal.h>
#include "../types/gimbal_type_.h"

#define GBL_BOX_(self)                 GBL_PRIV_REF(GBL_BOX(self))
#define GBL_BOX_CLASS_(klass)          GBL_PRIV_REF(GBL_BOX_CLASS(klass))
#define GBL_BOX_FIELD_KEY_DESTRUCTOR   "_dtor"
#define GBL_BOX_FIELD_KEY_USERDATA     "_ud"

static GblQuark fieldKeyDtor_  = GBL_QUARK_INVALID;
static GblQuark fieldKeyUd_    = GBL_QUARK_INVALID;
static GblQuark userdataQuark_ = GBL_QUARK_INVALID;
static GblQuark refCountQuark_ = GBL_QUARK_INVALID;

// ============== GblBoxClass ====================
GBL_EXPORT GblBoxClass* (GblBoxClass_createFloating)(GblType           derivedType,
                                                     size_t            totalSize,
                                                     void*             pUserdata,
                                                     GblArrayMapDtorFn pFnUdDtor)
{
    GblBoxClass* pClass = NULL;
    GBL_CTX_BEGIN(NULL);

    GBL_CTX_VERIFY_TYPE(derivedType, GBL_BOX_TYPE);

    pClass = (GblBoxClass*)GblClass_createFloating(derivedType, totalSize);
    if(pUserdata) GBL_CTX_VERIFY_CALL(GblBoxClass_setUserdata(pClass, pUserdata));
    if(pFnUdDtor) GBL_CTX_VERIFY_CALL(GblBoxClass_setUserDestructor(pClass, pFnUdDtor));

    GBL_CTX_END_BLOCK();
    return pClass;
}

GBL_EXPORT GBL_RESULT (GblBoxClass_constructFloating)(GblBoxClass*      pSelf,
                                                      GblType           derivedType,
                                                      void*             pUserdata,
                                                      GblArrayMapDtorFn pFnUdDtor)
{
    GBL_CTX_BEGIN(NULL);

    GBL_CTX_VERIFY_TYPE(derivedType, GBL_BOX_TYPE);

    GblClass_constructFloating(GBL_CLASS(pSelf), derivedType);
    if(pUserdata) GBL_CTX_VERIFY_CALL(GblBoxClass_setUserdata(pSelf, pUserdata));
    if(pFnUdDtor) GBL_CTX_VERIFY_CALL(GblBoxClass_setUserDestructor(pSelf, pFnUdDtor));

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

GBL_EXPORT uintptr_t GblBoxClass_field(const GblBoxClass* pSelf, GblQuark key) {
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

// ============== GblBox ==================
GBL_EXPORT GblBox* (GblBox_create)(GblType           derivedType,
                                   size_t            size,
                                   void*             pUserdata,
                                   GblArrayMapDtorFn pFnUdDtor,
                                   GblBoxClass*      pClass)
{
    GblBox* pBox = NULL;
    GBL_CTX_BEGIN(NULL);

    GBL_CTX_VERIFY_TYPE(derivedType, GBL_BOX_TYPE);

    pBox = (GblBox*)GblInstance_create(derivedType,
                                       size,
                                       GBL_CLASS(pClass));

    if(pBox) {
        if(pUserdata)
            GBL_CTX_VERIFY_CALL(GblBox_setUserdata(pBox, pUserdata));
        if(pFnUdDtor)
            GBL_CTX_VERIFY_CALL(GblBox_setUserDestructor(pBox, pFnUdDtor));
    }

    GBL_CTX_END_BLOCK();
    return pBox;
}


GBL_EXPORT GBL_RESULT (GblBox_construct)(GblBox*           pSelf,
                                         GblType           derivedType,
                                         void*             pUserdata,
                                         GblArrayMapDtorFn pFnUdDtor,
                                         GblBoxClass*      pClass)
{
    GBL_CTX_BEGIN(NULL);

    GBL_CTX_VERIFY_TYPE(derivedType, GBL_BOX_TYPE);

    GBL_CTX_VERIFY_CALL(GblInstance_construct((GblInstance*)pSelf,
                                              derivedType,
                                              GBL_CLASS(pClass)));
    GBL_BOX_(pSelf).constructedInPlace = GBL_TRUE;

    if(pUserdata)
        GBL_CTX_VERIFY_CALL(GblBox_setUserdata(pSelf, pUserdata));
    if(pFnUdDtor)
        GBL_CTX_VERIFY_CALL(GblBox_setUserDestructor(pSelf, pFnUdDtor));

    GBL_CTX_END();
}


GBL_EXPORT GBL_RESULT GblBox_setUserDestructor(GblBox* pSelf, GblArrayMapDtorFn pFnDtor) {
    if(pFnDtor)
        return GblArrayMap_setUserdata(&GBL_BOX_(pSelf).pFields,
                                       fieldKeyDtor_,
                                       (uintptr_t)pSelf,
                                       pFnDtor);
    else
        return GblArrayMap_erase(&GBL_BOX_(pSelf).pFields, fieldKeyDtor_);
}

GBL_EXPORT uintptr_t GblBox_field(const GblBox* pSelf, GblQuark key) {
    return GblArrayMap_getValue(&GBL_BOX_(pSelf).pFields, key);
}

GBL_EXPORT uintptr_t GblBox_takeField(GblBox* pSelf, GblQuark key) {
    uintptr_t value = 0;
    GblArrayMap_extractValue(&GBL_BOX_(pSelf).pFields, key, &value);
    return value;
}

GBL_EXPORT GblBool GblBox_clearField(GblBox* pSelf, GblQuark key) {
    return GblArrayMap_erase(&GBL_BOX_(pSelf).pFields, key);
}

GBL_EXPORT GblBool GblBox_hasField(const GblBox* pSelf, GblQuark key) {
    return GblArrayMap_contains(&GBL_BOX_(pSelf).pFields, key);
}

GBL_EXPORT GBL_RESULT GblBox_setField(GblBox*           pSelf,
                                      GblQuark          key,
                                      uintptr_t         ud,
                                      GblArrayMapDtorFn pFnDtor) {
    return GblArrayMap_setUserdata(&GBL_BOX_(pSelf).pFields, key, ud, pFnDtor);
}

GBL_EXPORT void* GblBox_userdata(const GblBox* pSelf) {
    return (void*)GblArrayMap_getValue(&GBL_BOX_(pSelf).pFields, fieldKeyUd_);
}

GBL_RESULT GblBox_setUserdata_(GblBox* pSelf, void* pUserdata) {
    if(pUserdata)
        return GblArrayMap_setUserdata(&GBL_BOX_(pSelf).pFields,
                                       fieldKeyUd_,
                                       (uintptr_t)pUserdata,
                                       NULL);
    else
        return GblArrayMap_erase(&GBL_BOX_(pSelf).pFields, fieldKeyUd_);
}

GBL_EXPORT GBL_RESULT GblBox_setUserdata(GblBox* pSelf, void* pUserdata) {
    GblObject* pObject;
    GBL_RESULT result = GBL_RESULT_SUCCESS;

    if(pUserdata != GblBox_userdata(pSelf)) {
        result = GblBox_setUserdata_(pSelf, pUserdata);

        /* Disgusting down-cast, because GblObject adds a "userdata" property which
           must get notified of changes outside the property system. */
        if(pObject = GBL_AS(GblObject, pSelf))
            GblObject_emitPropertyChangeByQuark(pObject, userdataQuark_);
    }

    return result;
}

GBL_EXPORT GblBox* GblBox_ref(GblBox* pSelf) {
    GblObject* pObject;

    GBL_ASSERT(pSelf);
    GBL_BOX_(pSelf).refCounter++;

    /* Disgusting down-cast, because GblObject adds a "userdata" property which
       must get notified of changes outside the property system. */
    if((pObject = GBL_AS(GblObject, pSelf)))
        GblObject_emitPropertyChangeByQuark(pObject, refCountQuark_);

    return pSelf;
}

GBL_EXPORT GblRefCount GblBox_unref(GblBox* pSelf) {
    GblRefCount count = 0;
    GBL_RESULT result = GBL_RESULT_SUCCESS;

    // It's legal to call with a NULL pointer, just like free().
    if GBL_LIKELY(pSelf) {
        GblObject* pObject;

        // Debug-only sanity check.
        GBL_ASSERT(GBL_BOX_(pSelf).refCounter, "No references remaining to unref!");

        // Check if we're releasing the final reference.
        if(!(count = (GBL_BOX_(pSelf).refCounter--) - 1)) {
            // Mark the Box as being in the finalization stage.
            GBL_BOX_(pSelf).finalizing = GBL_TRUE;

            if((pObject = GBL_AS(GblObject, pSelf)))
                GblObject_emitPropertyChangeByQuark(pObject, refCountQuark_);

            // Emit "finalizing" signal for any attached weak references.
            GBL_EMIT(pSelf, "finalize");

            // Invoke virtual destructor, which better fuckin' chain up.
            result |= GBL_BOX_GET_CLASS(pSelf)->pFnDestructor(pSelf);

            // Destruct or destroy depending on how it was constructed.
            if GBL_UNLIKELY(GBL_BOX_(pSelf).constructedInPlace)
                result |= GblInstance_destruct(&pSelf->base);
            else
                result |= GblInstance_destroy(&pSelf->base);

            // \todo Raise an actual error.
            //GBL_ASSERT(GBL_RESULT_SUCCESS(result));
        } else if((pObject = GBL_AS(GblObject, pSelf)))
            GblObject_emitPropertyChangeByQuark(pObject, refCountQuark_);
    }

    return count;
}

GBL_EXPORT GblRefCount GblBox_refCount(const GblBox* pSelf) {
    return GBL_PRIV_REF(pSelf).refCounter;
}

GBL_EXPORT GblBool GblBox_isFinalizing(const GblBox* pSelf) {
    return GBL_PRIV_REF(pSelf).finalizing;
}

GBL_EXPORT GblBool GblBox_isDestructing(const GblBox* pSelf) {
    return GBL_PRIV_REF(pSelf).destructing;
}

static GBL_RESULT GblBox_IVariant_construct_(GblVariant* pVariant, size_t argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_UNUSED(argc);

    GBL_RESULT result = GBL_RESULT_SUCCESS;

    switch(op) {
    case GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT:
        pVariant->pBox = GblBox_create(pVariant->type);
        break;

    case GBL_IVARIANT_OP_FLAG_CONSTRUCT_COPY:
    case GBL_IVARIANT_OP_FLAG_CONSTRUCT_VALUE_COPY:
        pVariant->pBox = pArgs[0].pVoid? GBL_REF(pArgs[0].pVoid) : NULL;
        break;

    default:
        pVariant->pBox = pArgs[0].pVoid;
        if(op == GBL_IVARIANT_OP_FLAG_CONSTRUCT_MOVE)
            result = GblVariant_invalidate(&pArgs[0]);
    }

    return result;
}

static GBL_RESULT GblBox_IVariant_destruct_(GblVariant* pVariant) {
    GBL_UNREF(pVariant->pBox);

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GblBox_IVariant_compare_(const GblVariant* pVariant, const GblVariant* pOther, int* pResult) {
    *pResult = pVariant->pBox - pOther->pBox;

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GblBox_IVariant_get_(GblVariant* pSelf, size_t argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_UNUSED(argc);

    GBL_RESULT result = GBL_RESULT_SUCCESS;

    // Assign pointer-to-pointer to point to this GblBox.
    *(GblBox**)pArgs[0].pVoid = pSelf->pBox;

    // Take a new reference if we're copying into the pointer.
    if(op == GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY)
        GBL_REF(pSelf->pBox);
    // Invalidate old reference if we're moving into the pointer.
    else if(op == GBL_IVARIANT_OP_FLAG_GET_VALUE_MOVE)
        result = GblVariant_invalidate(pSelf);

    return result;
}

static GBL_RESULT GblBox_IVariant_set_(GblVariant* pSelf, size_t argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_UNUSED(argc);

    GBL_RESULT result = GBL_RESULT_SUCCESS;

    // Release current reference.
    GBL_UNREF(pSelf->pBox);
    // Assign to new reference.
    pSelf->pBox = pArgs[0].pVoid;

    // Take a new reference if we're copying.
    if(op == GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY || op == GBL_IVARIANT_OP_FLAG_SET_COPY)
        GBL_REF(pSelf->pBox);
    // Invalidate the old reference if we're moving.
    else if(op == GBL_IVARIANT_OP_FLAG_SET_MOVE || op == GBL_IVARIANT_OP_FLAG_SET_VALUE_MOVE)
        result = GblVariant_invalidate(&pArgs[0]);

    return result;
}

static GBL_RESULT GblBox_destructor_(GblBox* pSelf) {
    GBL_PRIV_REF(pSelf).destructing = GBL_TRUE;
    return GblArrayMap_destroy(&GBL_PRIV_REF(pSelf).pFields);
}

static GBL_RESULT GblBox_init_(GblInstance* pInstance) {
    GBL_PRIV_REF(GBL_BOX(pInstance)).refCounter = 1;

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GblBoxClass_init_(GblClass* pClass, const void* pUd) {
    GBL_UNUSED(pUd);

    GBL_RESULT result = GBL_RESULT_SUCCESS;

    if GBL_UNLIKELY(!GblType_classRefCount(GBL_BOX_TYPE)) {
        fieldKeyDtor_  = GblQuark_fromStatic(GBL_BOX_FIELD_KEY_DESTRUCTOR);
        fieldKeyUd_    = GblQuark_fromStatic(GBL_BOX_FIELD_KEY_USERDATA);
        userdataQuark_ = GblQuark_fromStatic("userdata");
        refCountQuark_ = GblQuark_fromStatic("refCount");

        result = GblSignal_install(GBL_BOX_TYPE,
                                   "finalize",
                                   GblMarshal_CClosure_VOID__INSTANCE,
                                   0);
    }

    static const GblIVariantVTable iVariantVTable = {
        .supportedOps = GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT    |
                        GBL_IVARIANT_OP_FLAG_CONSTRUCT_COPY       |
                        GBL_IVARIANT_OP_FLAG_CONSTRUCT_MOVE       |
                        GBL_IVARIANT_OP_FLAG_CONSTRUCT_VALUE_COPY |
                        GBL_IVARIANT_OP_FLAG_CONSTRUCT_VALUE_MOVE |
                        GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY       |
                        GBL_IVARIANT_OP_FLAG_SET_COPY             |
                        GBL_IVARIANT_OP_FLAG_SET_VALUE_MOVE       |
                        GBL_IVARIANT_OP_FLAG_SET_MOVE             |
                        GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY       |
                        GBL_IVARIANT_OP_FLAG_GET_VALUE_MOVE       |
                        GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK,
        .pGetValueFmt = "p",
        .pSetValueFmt = "p",
        .pFnConstruct = GblBox_IVariant_construct_,
        .pFnDestruct  = GblBox_IVariant_destruct_,
        .pFnCompare   = GblBox_IVariant_compare_,
        .pFnGet       = GblBox_IVariant_get_,
        .pFnSet       = GblBox_IVariant_set_,
    };

    GBL_IVARIANT_CLASS(pClass)->pVTable       = &iVariantVTable;
    GBL_BOX_CLASS(pClass)     ->pFnDestructor = GblBox_destructor_;

    return result;
}

static GBL_RESULT GblBoxClass_final_(GblClass* pClass, const void* pUd) {
    GBL_UNUSED(pUd);

    if GBL_UNLIKELY(!GblType_classRefCount(GBL_BOX_TYPE))
        GblSignal_uninstall(GBL_BOX_TYPE, "finalize");

    return GblArrayMap_destroy(&GBL_BOX_CLASS_(pClass).pFields);
}

GBL_EXPORT GblType GblBox_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static GblInterfaceImpl ifaceMap[] = {
        {
            GBL_INVALID_TYPE,
            offsetof(GblBoxClass, GblIVariantImpl)
        }
    };

    static const GblTypeInfo typeInfo = {
        .classSize       = sizeof(GblBoxClass),
        .pFnClassInit    = GblBoxClass_init_,
        .pFnClassFinal   = GblBoxClass_final_,
        .instanceSize    = sizeof(GblBox),
        .pFnInstanceInit = GblBox_init_,
        .interfaceCount  = 1,
        .pInterfaceImpls = ifaceMap
    };

    if GBL_UNLIKELY(type == GBL_INVALID_TYPE) {
        ifaceMap[0].interfaceType = GBL_IVARIANT_TYPE;

        type = GblType_register(GblQuark_internStatic("GblBox"),
                                GBL_INSTANCE_TYPE,
                                &typeInfo,
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}

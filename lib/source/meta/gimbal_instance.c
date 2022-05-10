#include <gimbal/meta/gimbal_instance.h>
#include "gimbal_type_.h"


GBL_RESULT typeInstanceConstructValidate_(GblType type) {
    GblMetaClass* pMeta = GBL_META_CLASS_(type);
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_ARG(type != GBL_INVALID_TYPE,
                       "[GblType] Instance Construct: Invalid type!");
    GBL_API_VERIFY(GBL_TYPE_IS_INSTANTIABLE(type),
                   GBL_RESULT_ERROR_INVALID_TYPE,
                   "Cannot instantiate NON INSTANTIABLE type!");
    GBL_API_VERIFY(!GBL_TYPE_IS_ABSTRACT(type),
                   GBL_RESULT_ERROR_INVALID_TYPE,
                   "Cannot instantiate ABSTRACT type!");
    GBL_API_VERIFY(pMeta->info.instanceSize,
                   GBL_RESULT_ERROR_INVALID_ARG,
                   "Cannot instantiate type with instance of size 0!");
    GBL_API_END();
}


GBL_EXPORT GBL_RESULT GblInstance_construct_(GblType type, GblInstance* pInstance, GblClass* pClass) {
    GblMetaClass*   pMeta = GBL_META_CLASS_(type);
    GBL_API_BEGIN(pCtx_);
    GBL_API_CALL(typeInstanceConstructValidate_(type));
    GBL_API_PUSH_VERBOSE("[GblType] Instance Construct: type %s", GblType_name(GBL_TYPE_(pMeta)));

    // Zero Initialize
    memset(pInstance, 0, pMeta->info.instanceSize);

    // Set Class
    if(!pClass) pClass = GblClass_refFromType(type);
    else GblClass_refFromType(type);
    GBL_API_VERIFY_EXPRESSION(pClass, "Failed to retrieve class reference!");
    pInstance->pClass = pClass;

    GBL_API_VERBOSE("Calling constructor chain.");
    GBL_API_PUSH();

    for(uint8_t idx = 0; idx <= pMeta->depth; ++idx) {
        GblMetaClass* pIter = GBL_META_CLASS_(GblType_base((GblType)pMeta, idx));
        if(pIter->info.pFnInstanceInit) {
            GBL_API_VERBOSE("Calling instance initializers: [%s]", GblType_name(GBL_TYPE_(pIter)));
            GBL_API_CALL(pIter->info.pFnInstanceInit(pInstance, pCtx_));
        } else {
            GBL_API_VERBOSE("No instance ctor: [%s]", GblType_name(GBL_TYPE_(pIter)));
        }
    }

    GBL_API_POP(1);

#ifdef GBL_TYPE_DEBUG
    {
        GblRefCount count = 0;
        atomic_fetch_add(&pMeta->instanceRefCount, 1);
        GBL_API_DEBUG("Instance reference count: %u", count+1);
    }
#endif

    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblInstance_construct(GblInstance* pSelf, GblType type) {
    GBL_API_BEGIN(pCtx_);
    GBL_API_CALL(GblInstance_construct_(type, pSelf, NULL));
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblInstance_constructWithClass(GblInstance* pSelf, GblClass* pClass) {
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_POINTER(pClass);
    GBL_API_CALL(GblInstance_construct_(GBL_CLASS_TYPE(pClass), pSelf, pClass));
    GBL_API_END();
}

GBL_EXPORT GblInstance* GblInstance_create_(GblType type, GblClass* pClass) {
    GblInstance* pInstance  = NULL;
    GblMetaClass* pMeta     = GBL_META_CLASS_(type);
    GBL_API_BEGIN(pCtx_);
    GBL_API_CALL(typeInstanceConstructValidate_(type));
    GBL_API_PUSH_VERBOSE("[GblType] Instance Create: type %s", GblType_name(GBL_TYPE_(pMeta)));

    GBL_API_DEBUG("Allocating %u bytes.", pMeta->info.instanceSize);

    GBL_API_VERIFY(GBL_TYPE_IS_INSTANTIABLE(type) && !GBL_TYPE_IS_ABSTRACT(type),
                   GBL_RESULT_ERROR_INVALID_TYPE,
                   "Cannot instantiate non-instantiable or abstract types!");

    pInstance = GBL_API_MALLOC(gblAlignedAllocSize(pMeta->info.instanceSize),
                               GBL_ALIGNOF(GBL_MAX_ALIGN_T),
                               GblType_name(GBL_TYPE_(pMeta)));

    GBL_API_CALL(GblInstance_construct_(type, pInstance, pClass));

    GBL_API_POP(1);
    GBL_API_END_BLOCK();
    return pInstance;
}

GBL_EXPORT GblInstance* GblInstance_create(GblType type) {
    GblInstance* pInstance = NULL;
    GBL_API_BEGIN(pCtx_);
    pInstance = GblInstance_create_(type, NULL);
    GBL_API_END_BLOCK();
    return pInstance;
}

GBL_EXPORT GblInstance* GblInstance_createWithClass(GblClass* pClass) {
    GblInstance* pInstance = NULL;
    GBL_API_BEGIN(pCtx_);
    pInstance = GblInstance_create_(GBL_CLASS_TYPE(pClass), pClass);
    GBL_API_END_BLOCK();
    return pInstance;
}

GBL_EXPORT GblRefCount GblInstance_destruct(GblInstance* pSelf) {
    GblRefCount     refCount    = 0;
    GblClass*       pClass      = GBL_INSTANCE_CLASS(pSelf);
    const GblFlags  flags       = GBL_CLASS_FLAGS_(GBL_CLASS_TYPE(pClass));
    GBL_API_BEGIN(pCtx_);
    if(GblClass_isFloating(pClass)) {
        if(flags & GBL_CLASS_FLAG_OWNED_) {
            if(flags & GBL_CLASS_FLAG_IN_PLACE_)
                GblClass_destructFloating(pClass);
            else
                GblClass_destroyFloating(pClass);
        }
    } else {
        refCount = GblClass_unref(GblClass_fromInstance(pSelf));
    }
    GBL_API_END_BLOCK();
    return refCount;
}

GBL_EXPORT GblRefCount GblInstance_destroy(GblInstance* pSelf) {
    GblRefCount refCount = 0;
    GBL_API_BLOCK(pCtx_, {
       if(!pSelf) GBL_API_DONE();
       else {
            GBL_API_PUSH_VERBOSE("[GblType] Instance Destroy: %s",
                                 GblInstance_typeName(pSelf));

            refCount = GblInstance_destruct(pSelf);
            GBL_API_FREE(pSelf);

            GBL_API_POP(1);

        }
    });
    return refCount;
}

GBL_EXPORT GBL_RESULT GblInstance_classSwizzle(GblInstance* pSelf, GblClass* pClass) {
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_POINTER(pClass);
    GblClass* pClassOld = GBL_INSTANCE_CLASS(pSelf);

    // If we're replacing an existing class
    if(pClassOld) {
        // ensure that we're the same or a derived type
        GBL_API_VERIFY(GblClass_check(pClass, GBL_CLASS_TYPE(pClassOld)),
                       GBL_RESULT_ERROR_INVALID_TYPE,
                       "[GblType] Cannot swizzle from class type [%s] "
                       "to unrelated class type [%s].",
                       GblClass_typeName(pClassOld),
                       GblClass_typeName(pClass));

        // destroy or unreferencec
        const GblFlags classFlags = GBL_CLASS_FLAGS_(pClassOld);
        if(classFlags & GBL_CLASS_FLAG_OWNED_) {
            if(classFlags & GBL_CLASS_FLAG_IN_PLACE_)
                GBL_API_CALL(GblClass_destructFloating(pClassOld));
            else
                GBL_API_CALL(GblClass_destroyFloating(pClassOld));
        }
    }

    // set new class
    pSelf->pClass = pClass;

    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblInstance_classSink(GblInstance* pSelf)  {
    GblClass* pClass = GBL_INSTANCE_CLASS(pSelf);
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_POINTER(pClass);

    GBL_API_VERIFY(GblClass_isFloating(pClass),
                   GBL_RESULT_ERROR_INVALID_OPERATION,
                   "[GblInstance]: Cannot sink non-floating class of type: %s",
                   GblClass_typeName(pClass));

    GBL_CLASS_FLAG_SET_(pClass, GBL_CLASS_FLAG_OWNED_);
    GBL_API_END();
}

GBL_EXPORT GblBool GblInstance_check(const GblInstance* pSelf, GblType type) {
    GblBool result = GBL_FALSE;
    GBL_API_BEGIN(pCtx_);
    if(!(!pSelf && type == GBL_INVALID_TYPE)) {
        GBL_API_VERIFY_POINTER(pSelf);
        result = GblClass_check(pSelf->pClass, type);
    }
    GBL_API_END_BLOCK();
    return result;
}

#ifdef GBL_TYPE_DEBUG
GBL_EXPORT GblRefCount GblInstance_refCountFromType(GblType type) {
    GblRefCount refCount      = 0;
    GblMetaClass* pMeta = GBL_META_CLASS_(type);
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_ARG(pMeta != NULL);
    refCount = atomic_load(&pMeta->instanceRefCount);
    GBL_API_END_BLOCK();
    return refCount;
}
#endif


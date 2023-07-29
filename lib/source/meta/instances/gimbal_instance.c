#include <gimbal/meta/instances/gimbal_instance.h>
#include "../types/gimbal_type_.h"

GBL_EXPORT void* GblInstance_basePtr_(const GblInstance* pInstance) {
    GblMetaClass* pMeta = GBL_META_CLASS_(GBL_TYPEOF(pInstance));
    return pMeta? (void*)((uint8_t*)pInstance + pMeta->instancePrivateOffset) : NULL;
}

// not doing any type validation here...
GBL_EXPORT void* GblInstance_private(const GblInstance* pInstance, GblType base) {
    GblMetaClass* pMeta = GBL_META_CLASS_(base);
    return pMeta && pMeta->instancePrivateOffset?
                (void*)((uint8_t*)pInstance + pMeta->instancePrivateOffset) :
                NULL;
}

// not doing any type validation here...
GBL_EXPORT GblInstance* GblInstance_public(const void* pPrivate, GblType base) {
    GblMetaClass* pMeta = GBL_META_CLASS_(base);
    return pMeta && pMeta->instancePrivateOffset?
                (GblInstance*)((uint8_t*)pPrivate - pMeta->instancePrivateOffset) :
                NULL;
}

GBL_RESULT typeInstanceConstructValidate_(GblType type, GblClass* pClass, GblBool inPlace) {
    GblMetaClass* pMeta = GBL_META_CLASS_(type);
    GBL_CTX_BEGIN(pCtx_);
    GBL_CTX_VERIFY_TYPE(type);
    GBL_CTX_VERIFY(GBL_TYPE_INSTANTIABLE_CHECK(type),
                   GBL_RESULT_ERROR_INVALID_TYPE,
                   "Cannot instantiate NON INSTANTIABLE type!");
    GBL_CTX_VERIFY(!GBL_TYPE_ABSTRACT_CHECK(type),
                  GBL_RESULT_ERROR_INVALID_TYPE,
                   "Cannot instantiate ABSTRACT type!");
    GBL_CTX_VERIFY(pMeta->pInfo->instanceSize,
                   GBL_RESULT_ERROR_INVALID_ARG,
                   "Cannot instantiate type with instance of size 0!");
    GBL_CTX_VERIFY(!(inPlace && pMeta->instancePrivateOffset),
                   GBL_RESULT_ERROR_INVALID_OPERATION,
                   "Cannot PLACEMENT CONSTRUCT an instantiable that has PRIVATE DATA!");
    GBL_CTX_VERIFY(!pClass || type == GblClass_typeOf(pClass),
                   GBL_RESULT_ERROR_TYPE_MISMATCH,
                   "Cannot instantiate type with class of a different type!");
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblInstance_init_(GblType type, GblInstance* pInstance, GblClass* pClass) {
    GblMetaClass*   pMeta = GBL_META_CLASS_(type);
    GBL_CTX_BEGIN(pCtx_);
    GBL_CTX_PUSH_VERBOSE("[GblType] Instance Init: type %s", GblType_name(type));

    // Zero Initialize
    memset(pInstance, 0, pMeta->pInfo->instanceSize);
    if(pMeta->pInfo->instancePrivateSize) {
        memset((void*)(((uintptr_t)pInstance) + pMeta->instancePrivateOffset),
               0,
               pMeta->pInfo->instancePrivateSize);
    }

    // Set Class
    if(!pClass) pClass = GblClass_refDefault(type);
    else GblClass_refDefault(type);
    GBL_CTX_VERIFY_EXPRESSION(pClass, "Failed to retrieve class reference!");
    pInstance->pClass = pClass;

    GBL_CTX_VERBOSE("Calling initializer chain.");
    GBL_CTX_PUSH();

    for(uint8_t idx = 0; idx <= pMeta->depth; ++idx) {
        GblMetaClass* pIter = GBL_META_CLASS_(GblType_base((GblType)pMeta, idx));
        if(pIter->pInfo->pFnInstanceInit) {
            GBL_CTX_VERBOSE("Calling instance initializers: [%s]", GblType_name(GBL_TYPE_(pIter)));
            GBL_CTX_CALL(pIter->pInfo->pFnInstanceInit(pInstance));
        } else {
            GBL_CTX_VERBOSE("No instance initializer: [%s]", GblType_name(GBL_TYPE_(pIter)));
        }
    }

    GBL_CTX_POP(1);

//#ifdef GBL_TYPE_DEBUG
    {
        int16_t count = GBL_ATOMIC_INT16_INC(pMeta->instanceRefCount);
        GBL_CTX_DEBUG("Instance RefCount: %u", count+1);
    }
//#endif

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT (GblInstance_construct)(GblInstance* pSelf, GblType type, GblClass* pClass) {
    GBL_CTX_BEGIN(pCtx_);
    GBL_CTX_VERIFY_CALL(typeInstanceConstructValidate_(type, pClass, GBL_TRUE));
    GBL_CTX_VERIFY_POINTER(pSelf);
    GBL_CTX_PUSH_VERBOSE("[GblType] Instance Construct: type %s", GblType_name(type));
    GBL_CTX_VERIFY_CALL(GblInstance_init_(type, pSelf, pClass));
    GBL_CTX_END();
}

GBL_EXPORT GblInstance* (GblInstance_create)(GblType type, size_t size, GblClass* pClass) {
    GblInstance*  pInstance = NULL;
    GblMetaClass* pMeta     = GBL_META_CLASS_(type);

    GBL_CTX_BEGIN(pCtx_);

    GBL_CTX_PUSH_VERBOSE("[GblType] Instance Create: type %s", GblType_name(type));
    GBL_CTX_VERIFY_CALL(typeInstanceConstructValidate_(type, pClass, GBL_FALSE));

    if(!size) size = pMeta->pInfo->instanceSize;
    GBL_CTX_VERIFY(size >= pMeta->pInfo->instanceSize,
                   GBL_RESULT_ERROR_INVALID_ARG,
                   "Attempt to allocate [%s] with insufficient size [given: %zu, required: %zu]",
                   GblType_name(type),
                   size,
                   pMeta->pInfo->instanceSize);

    GBL_CTX_DEBUG("Allocating %zu bytes.", size + (-pMeta->instancePrivateOffset));

    uint8_t* pBase =
        GBL_CTX_MALLOC(gblAlignedAllocSizeDefault(size + (-pMeta->instancePrivateOffset)),
                       GBL_ALIGNOF(GBL_MAX_ALIGN_T),
                       GblType_name(type));

    // initialize just the private portion here, as the public will be initialized later
    if(pMeta->instancePrivateOffset != 0)
        memset(pBase, 0, -pMeta->instancePrivateOffset);

    pBase -= pMeta->instancePrivateOffset;

    pInstance = GBL_INSTANCE(pBase);
    GBL_CTX_VERIFY_CALL(GblInstance_init_(type, pInstance, pClass));

    GBL_CTX_POP(1);
    GBL_CTX_END_BLOCK();
    return pInstance;
}

GBL_EXPORT GBL_RESULT GblInstance_classRelease_(GblInstance* pSelf) {
    GblClass*       pClass      = GBL_INSTANCE_GET_CLASS(pSelf);
    const GblFlags  flags       = GBL_CLASS_FLAGS_(pClass);
    GblType         type        = GBL_CLASS_TYPEOF(pClass);
    GBL_CTX_BEGIN(pCtx_);

    // maybe release class's reference
    if(flags & GBL_CLASS_FLAG_OWNED_) {
        if(flags & GBL_CLASS_FLAG_IN_PLACE_)
            GBL_CTX_CALL(GblClass_destructFloating(pClass));
        else
            GBL_CTX_CALL(GblClass_destroyFloating(pClass));
    }

    // release instance's reference
    GblClass_unrefDefault(GblClass_weakRefDefault(type));

    GBL_CTX_END();
}

GBL_EXPORT GblRefCount GblInstance_destruct(GblInstance* pSelf) {
    GblRefCount     refCount    = 0;
    GBL_CTX_BEGIN(pCtx_);
    if(pSelf) {
        GblMetaClass* pMeta = GBL_META_CLASS_(GBL_TYPEOF(pSelf));
        GBL_CTX_CALL(GblSignal_removeInstance_(pSelf));
        refCount = GBL_ATOMIC_INT16_DEC(pMeta->instanceRefCount) - 1;
        GBL_CTX_VERIFY_CALL(GblInstance_classRelease_(pSelf));
    }
    GBL_CTX_END_BLOCK();
    return refCount;
}

GBL_EXPORT GblRefCount GblInstance_destroy(GblInstance* pSelf) {
    GblRefCount refCount = 0;
    GBL_CTX_BEGIN(pCtx_); {
       if(pSelf) {
            GBL_CTX_PUSH_VERBOSE("[GblType] Instance Destroy: %s",
                                 GblType_name(GblInstance_typeOf(pSelf)));
            void* pBase = GblInstance_basePtr_(pSelf);
            refCount = GblInstance_destruct(pSelf);
            GBL_CTX_VERIFY_LAST_RECORD();
            GBL_CTX_FREE(pBase);
            GBL_CTX_POP(1);
        }
    }
    GBL_CTX_END_BLOCK();
    return refCount;
}

GBL_EXPORT GBL_RESULT GblInstance_swizzleClass(GblInstance* pSelf, GblClass* pClass) {
    GBL_CTX_BEGIN(pCtx_);
    GBL_CTX_VERIFY_POINTER(pSelf);
    GBL_CTX_VERIFY_POINTER(pClass);
    GblClass* pClassOld = GBL_INSTANCE_GET_CLASS(pSelf);

    // If we're replacing an existing class
    if(pClassOld) {
        // ensure that we're the same or a derived type
        GBL_CTX_VERIFY(GblClass_check(pClass, GBL_CLASS_TYPEOF(pClassOld)),
                       GBL_RESULT_ERROR_TYPE_MISMATCH,
                       "[GblType] Cannot swizzle from class type [%s] "
                       "to unrelated class type [%s].",
                       GblType_name(GblClass_typeOf(pClassOld)),
                       GblType_name(GblClass_typeOf(pClass)));

        // destroy or unreference
        GBL_CTX_CALL(GblInstance_classRelease_(pSelf));
    }

    // set new class
    pSelf->pClass = pClass;

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblInstance_sinkClass(GblInstance* pSelf)  {
    GblClass* pClass = GBL_INSTANCE_GET_CLASS(pSelf);
    GBL_CTX_BEGIN(pCtx_);
    GBL_CTX_VERIFY_POINTER(pClass);

    GBL_CTX_VERIFY(GblClass_isFloating(pClass),
                   GBL_RESULT_ERROR_INVALID_OPERATION,
                   "[GblInstance]: Cannot sink non-floating class of type: %s",
                   GblType_name(GblClass_typeOf(pClass)));

    GBL_CLASS_FLAG_SET_(pClass, GBL_CLASS_FLAG_OWNED_);
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblInstance_floatClass(GblInstance* pSelf) {
    GblClass* pClass = GBL_INSTANCE_GET_CLASS(pSelf);
    GBL_CTX_BEGIN(pCtx_);
    GBL_CTX_VERIFY_POINTER(pClass);

    GBL_CTX_VERIFY(GblClass_isOwned(pClass),
                   GBL_RESULT_ERROR_INVALID_OPERATION,
                   "[GblInstance]: Cannot float unowned class of type: %s",
                   GblType_name(GblClass_typeOf(pClass)));

    GBL_CLASS_FLAG_CLEAR_(pClass, GBL_CLASS_FLAG_OWNED_);
    GBL_CTX_END();
}

GBL_EXPORT GblBool GblInstance_check(const GblInstance* pSelf, GblType type) {
    GblBool result = GBL_FALSE;
    if(!(!pSelf && type == GBL_INVALID_TYPE)) {
        if(!pSelf) {
            GBL_CTX_BEGIN(pCtx_);
            GBL_CTX_VERIFY_POINTER(NULL);
            GBL_CTX_END_BLOCK();
        } else {
            result = GblClass_check(pSelf->pClass, type);
        }
    } else result = GBL_TRUE;
    return result;
}

GBL_EXPORT GblInstance* GblInstance_convert_(GblInstance* pSelf, GblType type, GblBool check) GBL_NOEXCEPT {
    GblInstance* pOutInstance = NULL;

    const GblBool invalidPtr = (!pSelf && type != GBL_INVALID_TYPE);
    const GblBool typeMismatch = (!invalidPtr && !(GblInstance_check(pSelf, type)));

    if(invalidPtr || typeMismatch) {
        if(check) {
            GBL_CTX_BEGIN(NULL);
            GBL_CTX_VERIFY_POINTER(!invalidPtr,
                                   "Failed to cast NULL instance to %s.",
                                   GblType_name(type));
            GBL_CTX_VERIFY(!typeMismatch, GBL_RESULT_ERROR_TYPE_MISMATCH,
                           "Failed to cast instance from %s to %s.",
                           GblType_name(GblInstance_typeOf(pSelf)),
                           GblType_name(type));
            GBL_CTX_END_BLOCK();

        }
    } else if(pSelf) {
        pOutInstance = pSelf;
    }

    return pOutInstance;
}

GBL_EXPORT GblType GblInstance_typeOf(const GblInstance* pSelf) GBL_NOEXCEPT {
    return pSelf? GblClass_typeOf(pSelf->pClass) : GBL_INVALID_TYPE;
}

GBL_EXPORT size_t GblInstance_size(const GblInstance* pSelf) GBL_NOEXCEPT {
    size_t  size = 0;
    if(pSelf) {
        const GblTypeInfo* pInfo = GblType_info(GBL_TYPEOF(pSelf));
        if(pInfo) size = pInfo->instanceSize;
    }
    return size;
}

GBL_EXPORT size_t GblInstance_privateSize(const GblInstance* pSelf) GBL_NOEXCEPT {
    size_t  size = 0;
    if(pSelf) {
        const GblTypeInfo* pInfo = GblType_info(GBL_TYPEOF(pSelf));
        if(pInfo) size = pInfo->instancePrivateSize;
    }
    return size;
}

GBL_EXPORT size_t GblInstance_totalSize(const GblInstance* pSelf) GBL_NOEXCEPT {
    GblMetaClass* pMeta = GBL_META_CLASS_(GBL_TYPEOF(pSelf));
    return pMeta? (pMeta->pInfo->instanceSize - pMeta->instancePrivateOffset) : 0;
}

GBL_EXPORT GblClass* GblInstance_class(const GblInstance* pSelf) GBL_NOEXCEPT {
    return pSelf? pSelf->pClass : NULL;
}

GBL_EXPORT GblInstance* GblInstance_as(GblInstance* pSelf, GblType type) GBL_NOEXCEPT {
    return GblInstance_convert_(pSelf, type, GBL_FALSE);
}

GBL_EXPORT  GblInstance* GblInstance_cast(GblInstance* pSelf, GblType type) GBL_NOEXCEPT {
    return GblInstance_convert_(pSelf, type, GBL_TRUE);
}


#include <gimbal/meta/gimbal_class.h>
#include <gimbal/meta/gimbal_instance.h>
#include <gimbal/meta/gimbal_interface.h>
#include "gimbal/core/gimbal_api_frame.h"
#include "gimbal_type_.h"

GBL_INLINE void* GblClass_basePtr_(const GblClass* pClass) {
    GblMetaClass* pMeta = GBL_META_CLASS_(GBL_CLASS_TYPE(pClass));
    return pMeta? (void*)((uint8_t*)pClass + pMeta->classPrivateOffset) : NULL;
}

GBL_EXPORT GblSize GblClass_totalSize(const GblClass* pClass) {
    GblMetaClass* pMeta = GBL_META_CLASS_(GBL_CLASS_TYPE(pClass));
    return pMeta? (pMeta->pInfo->classSize - pMeta->classPrivateOffset) : 0;
}

GBL_EXPORT GblSize GblClass_privateSize(const GblClass* pClass) {
    GblMetaClass* pMeta = GBL_META_CLASS_(GBL_CLASS_TYPE(pClass));
    return pMeta? pMeta->pInfo->classPrivateSize  : 0;
}

GBL_EXPORT void* GblClass_private(const GblClass* pClass, GblType type) {
    GblMetaClass* pMeta = GBL_META_CLASS_(type);
    return pMeta && pMeta->pInfo->classPrivateSize?
                (void*)((uint8_t*)pClass + pMeta->classPrivateOffset) :
                NULL;
}

GBL_EXPORT GblClass* GblClass_peekFromInstance(const GblInstance* pInstance) {
    GblClass* pClass = NULL;
    GBL_API_BEGIN(GblType_contextDefault());
    if(pInstance) {
        pClass = pInstance->pClass;
    }
    GBL_API_END_BLOCK();
    return pClass;
}

GBL_EXPORT GblBool GblClass_isOverridden(const GblClass* pSelf) {
    GblBool     result = GBL_FALSE;
    GblClass*   pDefault;
    if(pSelf && pSelf != (pDefault = GblClass_default(pSelf))) {
        if(memcmp(GblClass_basePtr_(pSelf),
                  GblClass_basePtr_(pDefault),
                  GblClass_totalSize(pSelf)) != 0)
            result = GBL_TRUE;
    }
    return result;
}

GBL_EXPORT GblBool GblClass_isFloating(const GblClass* pSelf) {
    GblBool floating = GBL_FALSE;
    if(pSelf) {
       GblMetaClass* pMeta = GBL_CLASS_META_CLASS_(pSelf);
       if(!pMeta->pClass || pMeta->pClass != pSelf)
           if(!GblClass_isOwned(pSelf))
               floating = GBL_TRUE;
    }
    return floating;
}

GBL_EXPORT GblBool GblClass_isInterface(const GblClass* pSelf) {
    return GBL_TYPE_IS_INTERFACED(GBL_CLASS_TYPE(pSelf));
}

GBL_EXPORT GblBool GblClass_isInterfaceImpl(const GblClass* pSelf) {
    return pSelf? (GblBool)(GBL_CLASS_FLAGS_(pSelf) & GBL_CLASS_FLAG_IFACE_IMPL_) : GBL_FALSE;
}

GBL_EXPORT GblBool GblClass_isOwned(const GblClass* pSelf) {
    return pSelf? (GblBool)(GBL_CLASS_FLAGS_(pSelf) & GBL_CLASS_FLAG_OWNED_) : GBL_FALSE;
}

GBL_EXPORT GblBool GblClass_isInPlace(const GblClass* pSelf) {
    return pSelf? (GblBool)(GBL_CLASS_FLAGS_(pSelf) & GBL_CLASS_FLAG_IN_PLACE_) : GBL_FALSE;
}

static GBL_RESULT GbClass_construct_(GblClass* pClass, GblMetaClass* pMeta, GblFlags classFlags);

static GBL_RESULT GblClass_constructInterface_(GblInterface* pClass, GblMetaClass* pMeta, int16_t offset) {
    GBL_API_BEGIN(pCtx_);
    GBL_API_PUSH_VERBOSE("InterfaceClass::construct(%s)", GblType_name(GBL_TYPE_(pMeta)));

    GBL_API_VERBOSE("offset: %d", offset);
    pClass->outerClassOffset = offset;
    GBL_API_PUSH_VERBOSE("Default Class Ref");
    void* pInterfaceClass = GblClass_ref((GblType)pMeta);
    GBL_API_POP(1);
    GBL_API_VERIFY_EXPRESSION(pInterfaceClass);
    GBL_API_CALL(GbClass_construct_(&pClass->base, pMeta, GBL_CLASS_FLAG_IFACE_IMPL_));
    GBL_API_POP(1);
    GBL_API_END();
}

/*
 * Constructor initializes immediate data (typeId) then itereates over all bases,
 * initializing them from top to bottom. Each base has its interfaces initialized
 * before the rest of its class.
 */
static GBL_RESULT GbClass_construct_(GblClass* pClass, GblMetaClass* pMeta, GblFlags classFlags) {
    GBL_API_BEGIN(pCtx_);
    GBL_API_PUSH_VERBOSE("Class::construct(%s)", GblType_name(GBL_TYPE_(pMeta)));
    GBL_API_VERBOSE("Type: %p", pMeta);

    // Zero initiailze class - NOPE OVERWRITING IFACE IF WE DO THAT!
    //memset(pClass, 0, pMeta->pInfo->classSize);

    //IMMEDIATELY initialize its type!!!
    /*
    GBL_API_VERIFY_EXPRESSION(pClass == pMeta->pClass || (classFlags & GBL_CLASS_FLAG_FLOATING_),
                              "Floating class detected but not flagged!");
    */
    GBL_CLASS_PRIVATE_SET_(pClass, pMeta, classFlags);

    if(pMeta->pParent) {
        //GBL_API_PUSH_VERBOSE("Adding reference to parent class: ", pMeta->pParent->pName);
        //ensure construction of parent class (this will recurse)
        GblClass_ref((GblType)pMeta->pParent);
        //GBL_API_POP(1);
    } else {
        GBL_API_VERBOSE("Parent Class Ref: None");
    }

    for(uint8_t idx = 0; idx <= pMeta->depth; ++idx) {
        GblMetaClass* pIter = GBL_META_CLASS_(GblType_base((GblType)pMeta, idx));
        GBL_API_PUSH_VERBOSE("Class[%u]: %s", idx, GblType_name(GBL_TYPE_(pIter)));

        //GBL_API_PUSH_VERBOSE("Constructing Interfaces");
        if(!pIter->pInfo->interfaceCount) {
            //GBL_API_VERBOSE("Interfaces: None");
        } else {
            for(GblSize i = 0; i < pIter->pInfo->interfaceCount; ++i) {
                const GblTypeInterfaceMapEntry* pIEntry = &pIter->pInfo->pInterfaceMap[i];
                GblMetaClass* pIMeta = (GblMetaClass*)pIEntry->interfaceType;
                GBL_API_VERIFY_EXPRESSION(pIMeta);
                GBL_API_PUSH_VERBOSE("Interface[%u]: %s", i, GblType_name(GBL_TYPE_(pIMeta)));
                GblInterface* pIClass = (GblInterface*)((char*)pClass + pIEntry->classOffset);
                GBL_API_CALL(GblClass_constructInterface_(pIClass, pIMeta, -pIEntry->classOffset));
                GBL_API_POP(1);
            }
        }
        //GBL_API_POP(1);

        if(pIter->pInfo->pFnClassInit) {
            //GBL_API_VERBOSE("ClassInit(%s)", GblType_name(GBL_TYPE_(pIter)));
            GBL_API_CALL(pIter->pInfo->pFnClassInit(pClass, pIter->pInfo->pClassData, pCtx_));
        } else {
            //GBL_API_VERBOSE("Ctor: NULL", GblType_name(GBL_TYPE_(pIter)));
        }

        GBL_API_POP(1);
    }

   // GBL_API_POP(1);
    GBL_API_POP(1);
    GBL_API_END();
}


static GblClass* GblClass_create_(GblMetaClass* pMeta, GblBool floating) {
    GblClass* pFloatingClass = NULL;
    GblClass** ppClass       = &pFloatingClass;
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_ARG(pMeta);
    //GBL_API_PUSH_VERBOSE("Class::create(%s)", GblType_name(GBL_TYPE_(pMeta)));

    if(!floating) {
        GBL_API_VERIFY_EXPRESSION(!GBL_ATOMIC_INT16_LOAD(pMeta->refCount),
                                  "Already have a reference to an invalid class object!");

        ppClass = &pMeta->pClass;
    }

    //Allocate a new class structure if one isn't already available

    if(!*ppClass) {
        GBL_API_VERBOSE("Allocating separate class structure!");
        uint8_t* pBase = GBL_API_MALLOC(gblAlignedAllocSize(pMeta->pInfo->classSize -
                                                            pMeta->classPrivateOffset),
                                        GBL_ALIGNOF(GBL_MAX_ALIGN_T),
                                        GblType_name(GBL_TYPE_(pMeta)));
        if(pMeta->classPrivateOffset != 0)
            memset(pBase, 0, -pMeta->classPrivateOffset);

        pBase -= pMeta->classPrivateOffset;
        pFloatingClass = GBL_CLASS(pBase);

    //Class must've been allocated with Meta class. Use existing data.
    } else {
       //GBL_API_VERBOSE("Using existing inline class allocation.");
    }

    // Call constructor
    GBL_API_CALL(GbClass_construct_(*ppClass, pMeta, 0));

    //GBL_API_POP(1);
    GBL_API_END_BLOCK();
    return *ppClass;
}

GBL_EXPORT GblClass* GblClass_ref(GblType type) GBL_NOEXCEPT {
    GblClass* pClass    = NULL;
    GblMetaClass* pMeta = GBL_META_CLASS_(type);
    GBL_API_BEGIN(GblType_contextDefault());
    GBL_API_VERIFY_ARG(type != GBL_INVALID_TYPE);

    GBL_API_VERIFY(pMeta->pInfo->classSize != 0,
                   GBL_RESULT_UNIMPLEMENTED,
                   "[GblType] Attempt to reference a class of size 0!");

    //GBL_API_PUSH_VERBOSE("Class::reference(%s)", GblType_name(GBL_TYPE_(pMeta)));
    GBL_TYPE_ENSURE_INITIALIZED_();

    // Return existing reference to class data
    if(pMeta->pClass && GBL_CLASS_TYPE(pMeta->pClass) != GBL_INVALID_TYPE) {
        GBL_API_VERIFY_EXPRESSION(GBL_ATOMIC_INT16_LOAD(pMeta->refCount) ||
                                  (pMeta->flags & GBL_TYPE_FLAG_CLASS_PINNED),
                                  "No references to an initialized unpinned class!?");
        //GBL_API_VERBOSE("Using existing class data");
        pClass = pMeta->pClass;

    // Create a new class structure
    } else {
        pClass = GblClass_create_(pMeta, GBL_FALSE);
        GBL_API_VERIFY_EXPRESSION(pClass && pClass == pMeta->pClass, "Failed to create class!");
    }

    // Either way, we're returning a new reference, add refcount
    GblRefCount oldCount = GBL_ATOMIC_INT16_INC(pMeta->refCount);
    if(oldCount)
        GBL_API_VERBOSE("++[%s].refCount: %u", GblType_name(GBL_TYPE_(pMeta)), oldCount+1);

    //GBL_API_POP(1);
    GBL_API_END_BLOCK();
    return pClass;
}

GBL_EXPORT GblClass* GblClass_peek(GblType type) GBL_NOEXCEPT {
    GblClass*       pClass  = NULL;
    GblMetaClass*   pMeta   = GBL_META_CLASS_(type);
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_ARG(type != GBL_INVALID_TYPE);
    GBL_API_VERIFY(pMeta->pClass, GBL_RESULT_ERROR_INVALID_OPERATION,
                  "[GblType] gblClassPeek(%s): no class!", GblType_name(type));
    GBL_API_VERIFY(pMeta->refCount, GBL_RESULT_ERROR_INVALID_OPERATION,
                   "Cannot peek into uninitialized class: %s", GblType_name(type));
    pClass = pMeta->pClass;

    GBL_API_END_BLOCK();
    return pClass;
}

GBL_EXPORT GBL_RESULT GblClass_verifyFloatingConstruction(GblType type, GblBool inPlace) {
    GBL_API_BEGIN(pCtx_);
    GblMetaClass* pMeta = GBL_META_CLASS_(type);

    GBL_API_VERIFY(!(inPlace && pMeta->classPrivateOffset != 0),
                   GBL_RESULT_ERROR_INVALID_OPERATION,
                   "Cannot construct a class IN PLACE which contains PRIVATE DATA!");
    GBL_API_END();
}


GBL_EXPORT GBL_RESULT GblClass_constructFloating(GblClass* pSelf, GblType type) GBL_NOEXCEPT {
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_ARG(type != GBL_INVALID_TYPE);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_CALL(GblClass_verifyFloatingConstruction(type, GBL_TRUE));
    GblClass_ref(type); // have to reference actual type
    GBL_API_CALL(GbClass_construct_(pSelf,
                                        GBL_META_CLASS_(type),
                                        GBL_CLASS_FLAG_IN_PLACE_));
    GBL_API_END();
}

GBL_EXPORT GblClass* GblClass_createFloating(GblType type) GBL_NOEXCEPT {
    GblClass* pClass = NULL;
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_ARG(type != GBL_INVALID_TYPE);
    GBL_API_CALL(GblClass_verifyFloatingConstruction(type, GBL_FALSE));
    GblClass_ref(type);
    pClass = GblClass_create_(GBL_META_CLASS_(type), GBL_TRUE);
    GBL_API_END_BLOCK();
    return pClass;
}


static GBL_EXPORT GBL_RESULT GblClass_destruct_(GblClass* pClass) {
    GblMetaClass* pMeta = GBL_META_CLASS_(GBL_CLASS_TYPE(pClass));
    GBL_API_BEGIN(pCtx_);
    GBL_API_DEBUG("Destroying %s class!", GblType_name(GBL_TYPE_(pMeta)));
    GBL_API_PUSH();
    GblMetaClass* pIter = pMeta;
    // walk up the destructor chain
    GBL_API_PUSH_VERBOSE("Walking class destructors.");

    // iterate from derived to base class
    do {
        // call finalizer if class provides one
        if(pIter->pInfo->pFnClassFinal) {
            GBL_API_DEBUG("Calling class dtor: [%s]", GblType_name(GBL_TYPE_(pIter)));
            GBL_API_CALL(pIter->pInfo->pFnClassFinal(pMeta->pClass, pIter->pInfo->pClassData, pCtx_));
        } else {
            GBL_API_DEBUG("No class dtor: [%s]", GblType_name(GBL_TYPE_(pIter)));
        }

        // iterate over all interfaces
        for(GblSize i = 0; i < pIter->pInfo->interfaceCount; ++i) {
            GblInterface* pInterface = (GblInterface*)((const char*)pClass +
                                                       pIter->pInfo->pInterfaceMap[i].classOffset);
            GblClass* pDefaultIFaceClass = GblClass_peek(GBL_CLASS_TYPE(pInterface));

            // destruct interface implementation
            GblClass_destruct_(GBL_CLASS(pInterface));

            // unreference default interface class
            GblClass_unref(pDefaultIFaceClass);
        }
        pIter = pIter->pParent;
    } while(pIter);
    GBL_API_POP(1);

    // Store parent class
    GblClass* pParentClass = pMeta->pParent? pMeta->pParent->pClass : NULL;

    //Release reference to parent class
    if(pParentClass) GblClass_unref(pParentClass);

    // clear the type so it's not looking initialized
    GBL_CLASS_PRIVATE_SET_(pClass, GBL_INVALID_TYPE, 0);

    GBL_API_POP(1);
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblClass_destructFloating(GblClass* pSelf) {
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_POINTER(pSelf);
    const GblType type = GBL_CLASS_TYPE(pSelf);
    GBL_API_CALL(GblClass_destruct_(pSelf));
    GblClass_unref(GblClass_peek(type));
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblClass_destroyFloating(GblClass* pSelf) {
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_POINTER(pSelf);
    void* pBase = GblClass_basePtr_(pSelf);
    GBL_API_CALL(GblClass_destructFloating(pSelf));
    GBL_API_FREE(pBase);
    GBL_API_END();
}

GBL_EXPORT GblRefCount GblClass_unref(GblClass* pSelf) GBL_NOEXCEPT {
    GblRefCount refCount    = 0;
    GblMetaClass* pMeta     = NULL;
    GBL_API_BEGIN(pCtx_);
    if(!pSelf) GBL_API_DONE(); //valid to Unref NULL pointer

    GBL_API_VERIFY(GBL_CLASS_TYPE(pSelf) != GBL_INVALID_TYPE,
                   GBL_RESULT_ERROR_INTERNAL,
                   "Class::unreference(): The specified class has an invalid ID!");

    pMeta = GBL_META_CLASS_(GBL_CLASS_TYPE(pSelf));

    GBL_API_PUSH_VERBOSE("Class::unreference(%s): %u",
                         GblClass_typeName(pSelf),
                         pMeta->refCount);

    GBL_API_VERIFY(GBL_ATOMIC_INT16_LOAD(pMeta->refCount) != 0,
                   GBL_RESULT_ERROR_INTERNAL,
                   "The refcount for the given class was already at 0!");

    refCount = GBL_ATOMIC_INT16_DEC(pMeta->refCount);

    if(refCount-1) {
        GBL_API_VERBOSE("--[%s].refCount: %u", GblType_name(GBL_TYPE_(pMeta)), refCount-1);
    } else {
        GblRefCount instanceRefCount = 0;
        if((instanceRefCount = GBL_ATOMIC_INT16_LOAD(pMeta->instanceRefCount)))
            GBL_API_WARN("0 class references with remaining instance references: %u", instanceRefCount);

        if(pMeta->flags & GBL_TYPE_FLAG_CLASS_PINNED)
            GBL_API_VERBOSE("Class::unreference(%s): 0 - Preserving pinned class",
                            GblClass_typeName(pSelf));
        else
            GBL_API_VERIFY_CALL(GblClass_destruct_(pSelf));

    }

    GBL_API_POP(1);
    GBL_API_END_BLOCK();
    return refCount-1;
}


static GblInterface* GblClass_peekInterface_(GblClass* pClass, GblType ifaceType) {
    GblInterface* pIClass       = NULL;
    GblMetaClass* pIFaceMeta    = GBL_META_CLASS_(ifaceType);
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_POINTER(pClass);
    GBL_API_VERIFY_EXPRESSION(GBL_CLASS_TYPE(pClass) != GBL_INVALID_TYPE);
    GBL_API_VERIFY_ARG(ifaceType != GBL_INVALID_TYPE);
    GBL_API_VERIFY_ARG(GBL_TYPE_IS_INTERFACED(ifaceType));
    {
        GblMetaClass* pMeta = GBL_META_CLASS_(GBL_CLASS_TYPE(pClass));
        while(pMeta) {
            for(unsigned i = 0; i < pMeta->pInfo->interfaceCount; ++i) {
                GblInterface* pCurIClass = (GblInterface*)((uintptr_t)pClass + pMeta->pInfo->pInterfaceMap[i].classOffset);
                GBL_API_VERIFY_EXPRESSION(GBL_CLASS_FLAG_TEST_(GBL_CLASS(pCurIClass), GBL_CLASS_FLAG_IFACE_IMPL_));
                GBL_API_VERIFY_EXPRESSION(GBL_META_CLASS_(pMeta->pInfo->pInterfaceMap[i].interfaceType)
                                          == GBL_META_CLASS_(GBL_CLASS_TYPE(pCurIClass)));
                GBL_API_VERIFY_EXPRESSION(GBL_CLASS_TYPE(pCurIClass) != GBL_INVALID_TYPE);
                if(GBL_META_CLASS_(GBL_CLASS_TYPE(pCurIClass)) == pIFaceMeta) {
                    pIClass = pCurIClass;
                    break;
                } else {
                    pCurIClass = GblClass_peekInterface_(GBL_CLASS(pCurIClass), ifaceType);
                    if(pCurIClass) {
                        GBL_API_VERIFY_EXPRESSION(GBL_META_CLASS_(GBL_CLASS_TYPE(pCurIClass)) == pIFaceMeta);
                        pIClass = pCurIClass;
                        break;
                    }
                }
            }
            pMeta = pMeta->pParent;
        }
    }
    GBL_API_END_BLOCK();
    return pIClass;
}


static GblClass* GblClass_cast_(GblClass* pClass, GblType toType, GblBool check) {
    GblClass* pClassStart = pClass;
    GblClass* pToClass = NULL;
    GBL_API_BEGIN(pCtx_);

    // Casting a NULL pointer or to an invalid type returns NULL
    if(pClass && toType != GBL_INVALID_TYPE) GBL_LIKELY {
        //early exit when class is already of desired type
        if(GBL_CLASS_META_CLASS_(pClass) == GBL_META_CLASS_(toType)) GBL_LIKELY {
            pToClass = pClass;
        } else {
            GblBool toInterface = GBL_FALSE;
            if(GBL_TYPE_IS_INTERFACED(toType)) toInterface = GBL_TRUE;

            // If current node is an interface class, attempt to "back out" to root class node
            while(GBL_TYPE_IS_INTERFACED(GBL_CLASS_TYPE(pClass))) {
                // Move class pointer to interface's concrete class
                pClass = GblInterface_outerClass((GblInterface*)pClass);
                GBL_API_VERIFY_EXPRESSION(pClass);
            }

            GblMetaClass* pMeta = GBL_CLASS_META_CLASS_(pClass);
            GBL_API_VERIFY_EXPRESSION(pMeta);

            // iterate from derived to base class, breadth-first searching
            do {
                //check current class
                if(pMeta == GBL_META_CLASS_(toType)) {
                    pToClass = pClass;
                    break;
                }
                pMeta = pMeta->pParent;
            } while(pMeta);

            if(!pToClass && toInterface) {
                GblInterface* pInterface = GblClass_peekInterface_(pClass, toType);
                if(pInterface) {
                    pToClass = GBL_CLASS(pInterface);
                }
            }
        }

    }
    GBL_API_END_BLOCK();
    if(check && !pToClass) {
        if(toType == GBL_INVALID_TYPE) {
            GBL_API_RECORD_SET(GBL_RESULT_ERROR_TYPE_MISMATCH,
                               "Attempted to cast from type %s to GBL_INVALID_TYPE!",
                               GblClass_typeName(pClassStart));
        } else {
            GBL_API_RECORD_SET(GBL_RESULT_ERROR_TYPE_MISMATCH,
                               "Failed to cast from type %s to %s!",
                               GblClass_typeName(pClassStart), GblType_name(toType));
        }
    }
    return pToClass;
}

GBL_EXPORT GblBool GblClass_check(const GblClass* pSelf, GblType toType) {
    GblBool result = GBL_FALSE;
    GBL_API_BEGIN(pCtx_);
    if(!(!pSelf && toType == GBL_INVALID_TYPE)) {
        GBL_API_VERIFY_POINTER(pSelf);
        result = GblType_check(GBL_CLASS_TYPE(pSelf), toType);
    }
    GBL_API_END_BLOCK();
    return result;
}

GBL_EXPORT GblClass* GblClass_cast(GblClass* pSelf, GblType toType) GBL_NOEXCEPT {
    return GblClass_cast_(pSelf, toType, GBL_TRUE);
}
GBL_EXPORT GblClass* GblClass_try(GblClass* pSelf, GblType toType) GBL_NOEXCEPT {
    return GblClass_cast_(pSelf, toType, GBL_FALSE);
}





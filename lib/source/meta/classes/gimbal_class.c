#include <gimbal/meta/classes/gimbal_class.h>
#include <gimbal/meta/instances/gimbal_instance.h>
#include <gimbal/meta/ifaces/gimbal_interface.h>
#include "gimbal/core/gimbal_ctx.h"
#include "../types/gimbal_type_.h"

GBL_INLINE void* GblClass_basePtr_(const GblClass* pClass) {
    GblMetaClass* pMeta = GBL_META_CLASS_(GBL_CLASS_TYPEOF(pClass));
    return pMeta? (void*)((uint8_t*)pClass + pMeta->classPrivateOffset) : NULL;
}

GBL_EXPORT size_t  GblClass_totalSize(const GblClass* pClass) {
    GblMetaClass* pMeta = GBL_META_CLASS_(GBL_CLASS_TYPEOF(pClass));
    return pMeta? (pMeta->pInfo->classSize - pMeta->classPrivateOffset) : 0;
}

GBL_EXPORT size_t  GblClass_privateSize(const GblClass* pClass) {
    GblMetaClass* pMeta = GBL_META_CLASS_(GBL_CLASS_TYPEOF(pClass));
    return pMeta? pMeta->pInfo->classPrivateSize  : 0;
}

GBL_EXPORT void* GblClass_private(const GblClass* pClass, GblType type) {
    void* pData = NULL;
    const GblType classType = GBL_CLASS_TYPEOF(pClass);

    if((!pClass && type != GBL_INVALID_TYPE) || (pClass && (type != classType && !GblType_derives(classType, type)))) {
        GBL_CTX_BEGIN(NULL);
        GBL_CTX_VERIFY(GBL_FALSE, GBL_RESULT_ERROR_TYPE_MISMATCH);
        GBL_CTX_END_BLOCK();
    } else if(pClass) {

        GblMetaClass* pMeta = GBL_META_CLASS_(type);
        pData =  pMeta && pMeta->pInfo->classPrivateSize?
                    (void*)((uint8_t*)pClass + pMeta->classPrivateOffset) :
                    NULL;

    }
    return pData;
}

GBL_EXPORT GblClass* GblClass_public(const void* pPrivate, GblType type) {
    GblClass* pClass = NULL;

    if(pPrivate) {
        GblMetaClass* pMeta = GBL_META_CLASS_(type);

        pClass = pMeta && pMeta->pInfo->classPrivateSize?
                        (GblClass*)((uint8_t*)pPrivate - pMeta->classPrivateOffset) :
                        NULL;

    } else if(type != GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);
        GBL_CTX_VERIFY(GBL_FALSE, GBL_RESULT_ERROR_INVALID_TYPE);
        GBL_CTX_END_BLOCK();
    }
    return pClass;
}

GBL_EXPORT GblBool GblClass_isOverridden(const GblClass* pSelf) {
    GblBool     result = GBL_FALSE;
    GblClass*   pDefault;

    if(pSelf && pSelf != (pDefault = GblClass_default(pSelf))) {
        const size_t  cmpSize = GblClass_size(pSelf) - sizeof(GblClass);
        /* we have to move past the class's private_ field, because it has
         * the encoded FLAGS which have bits for floating/owned, etc. */
        if(cmpSize && memcmp((uint8_t*)GblClass_basePtr_(pSelf)    + sizeof(GblClass),
                             (uint8_t*)GblClass_basePtr_(pDefault) + sizeof(GblClass),
                             cmpSize) != 0)
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
    return GBL_TYPE_INTERFACED_CHECK(GBL_CLASS_TYPEOF(pSelf));
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
    GBL_CTX_BEGIN(pCtx_);
    GBL_CTX_PUSH_VERBOSE("InterfaceClass::construct(%s)", GblType_name(GBL_TYPE_(pMeta)));

    GBL_CTX_VERBOSE("offset: %d", offset);
    pClass->outerClassOffset_ = offset;
    GBL_CTX_PUSH_VERBOSE("Default Class Ref");
    void* pInterfaceClass = GblClass_refDefault((GblType)pMeta);
    GBL_CTX_POP(1);
    GBL_CTX_VERIFY_EXPRESSION(pInterfaceClass);
    GBL_CTX_CALL(GbClass_construct_(&pClass->base, pMeta, GBL_CLASS_FLAG_IFACE_IMPL_));
    GBL_CTX_POP(1);
    GBL_CTX_END();
}

/*
 * Constructor initializes immediate data (typeId) then itereates over all bases,
 * initializing them from top to bottom. Each base has its interfaces initialized
 * before the rest of its class.
 */
static GBL_RESULT GbClass_construct_(GblClass* pClass, GblMetaClass* pMeta, GblFlags classFlags) {
    GBL_CTX_BEGIN(pCtx_);
    GBL_CTX_PUSH_VERBOSE("Class::construct(%s)", GblType_name(GBL_TYPE_(pMeta)));
    GBL_CTX_VERBOSE("Type: %p", pMeta);
    GBL_CTX_VERBOSE("Flags: %x", classFlags);

    // Zero initialize class - NOPE OVERWRITING IFACE IF WE DO THAT!
    if(!(classFlags & GBL_CLASS_FLAG_IFACE_IMPL_))
        memset(pClass, 0, pMeta->pInfo->classSize);
    else
        memset((void*)((uintptr_t)pClass + sizeof(GblInterface)), 0, pMeta->pInfo->classSize-sizeof(GblInterface));

    //IMMEDIATELY initialize its type!!!
    /*
    GBL_CTX_VERIFY_EXPRESSION(pClass == pMeta->pClass || (classFlags & GBL_CLASS_FLAG_FLOATING_),
                              "Floating class detected but not flagged!");
    */
    GBL_CLASS_PRIVATE_SET_(pClass, pMeta, classFlags);

    if(pMeta->pParent) {
        //GBL_CTX_PUSH_VERBOSE("Adding reference to parent class: ", pMeta->pParent->pName);
        //ensure construction of parent class (this will recurse)
        GblClass_refDefault((GblType)pMeta->pParent);
        //GBL_CTX_POP(1);
    } else {
        GBL_CTX_VERBOSE("Parent Class Ref: None");
    }

    for(uint8_t idx = 0; idx <= pMeta->depth; ++idx) {
        GblMetaClass* pIter = GBL_META_CLASS_(GblType_base((GblType)pMeta, idx));
        GBL_CTX_PUSH_VERBOSE("Class[%u]: %s", idx, GblType_name(GBL_TYPE_(pIter)));

        //GBL_CTX_PUSH_VERBOSE("Constructing Interfaces");
        if(!pIter->pInfo->interfaceCount) {
            //GBL_CTX_VERBOSE("Interfaces: None");
        } else {
            for(size_t  i = 0; i < pIter->pInfo->interfaceCount; ++i) {
                const GblTypeInterfaceMapEntry* pIEntry = &pIter->pInfo->pInterfaceMap[i];
                GblMetaClass* pIMeta = (GblMetaClass*)pIEntry->interfaceType;
                GBL_CTX_VERIFY_EXPRESSION(pIMeta);
                GBL_CTX_PUSH_VERBOSE("Interface[%u]: %s", i, GblType_name(GBL_TYPE_(pIMeta)));
                GblInterface* pIClass = (GblInterface*)((char*)pClass + pIEntry->classOffset);
                GBL_CTX_CALL(GblClass_constructInterface_(pIClass, pIMeta, -pIEntry->classOffset));
                GBL_CTX_POP(1);
            }
        }
        //GBL_CTX_POP(1);

        if(pIter->pInfo->pFnClassInit) {
            //GBL_CTX_VERBOSE("ClassInit(%s)", GblType_name(GBL_TYPE_(pIter)));
            GBL_CTX_CALL(pIter->pInfo->pFnClassInit(pClass, pIter->pInfo->pClassData, pCtx_));
        } else {
            //GBL_CTX_VERBOSE("Ctor: NULL", GblType_name(GBL_TYPE_(pIter)));
        }

        GBL_CTX_POP(1);
    }

   // GBL_CTX_POP(1);
    GBL_CTX_POP(1);
    GBL_CTX_END();
}


static GblClass* GblClass_create_(GblMetaClass* pMeta, GblBool floating) {
    GblClass*  pFloatingClass = NULL;
    GblClass** ppClass        = &pFloatingClass;
    GBL_CTX_BEGIN(pCtx_);
    GBL_CTX_VERIFY_ARG(pMeta);
    //GBL_CTX_PUSH_VERBOSE("Class::create(%s)", GblType_name(GBL_TYPE_(pMeta)));

    if(!floating) {
        GBL_CTX_VERIFY_EXPRESSION(!GBL_ATOMIC_INT16_LOAD(pMeta->refCount),
                                  "Already have a reference to an invalid class object!");

        ppClass = &pMeta->pClass;
    }

    //Allocate a new class structure if one isn't already available

    if(!*ppClass) {
        GBL_CTX_VERBOSE("Allocating separate class structure!");
        uint8_t* pBase = GBL_CTX_MALLOC(gblAlignedAllocSizeDefault(pMeta->pInfo->classSize -
                                                            pMeta->classPrivateOffset),
                                        GBL_ALIGNOF(GBL_MAX_ALIGN_T),
                                        GblType_name(GBL_TYPE_(pMeta)));
        if(pMeta->classPrivateOffset != 0)
            memset(pBase, 0, -pMeta->classPrivateOffset);

        pBase -= pMeta->classPrivateOffset;
        pFloatingClass = GBL_CLASS(pBase);

    //Class must've been allocated with Meta class. Use existing data.
    } else {
       //GBL_CTX_VERBOSE("Using existing inline class allocation.");
    }

    // Call constructor
    GBL_CTX_CALL(GbClass_construct_(*ppClass, pMeta, 0));

    //GBL_CTX_POP(1);
    GBL_CTX_END_BLOCK();
    return *ppClass;
}

GBL_EXPORT GblClass* GblClass_refDefault(GblType type) GBL_NOEXCEPT {
    GblClass* pClass    = NULL;
    GblMetaClass* pMeta = GBL_META_CLASS_(type);
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_TYPE(type);

    GBL_CTX_VERIFY(pMeta->pInfo->classSize != 0,
                   GBL_RESULT_UNIMPLEMENTED,
                   "[GblType] Attempt to reference class [%s] of size 0!",
                   GblType_name(type));

    //GBL_CTX_PUSH_VERBOSE("Class::reference(%s)", GblType_name(GBL_TYPE_(pMeta)));
    GBL_TYPE_ENSURE_INITIALIZED_();

    if(!GBL_ATOMIC_INT16_LOAD(pMeta->refCount))
       GBL_CTX_VERIFY_CALL(GblType_refresh_(type));

    // Return existing reference to class data
    if(pMeta->pClass && GBL_CLASS_TYPEOF(pMeta->pClass) != GBL_INVALID_TYPE) {
        GBL_CTX_VERIFY_EXPRESSION(GBL_ATOMIC_INT16_LOAD(pMeta->refCount) ||
                                  (pMeta->flags & GBL_TYPE_FLAG_CLASS_PINNED),
                                  "No references to an initialized unpinned class!?");
        //GBL_CTX_VERBOSE("Using existing class data");
        pClass = pMeta->pClass;

    // Create a new class structure
    } else {
        pClass = GblClass_create_(pMeta, GBL_FALSE);
        GBL_CTX_VERIFY_EXPRESSION(pClass && pClass == pMeta->pClass, "Failed to create class!");
    }

    // Either way, we're returning a new reference, add refcount
    GblRefCount oldCount = GBL_ATOMIC_INT16_INC(pMeta->refCount);
    if(oldCount)
        GBL_CTX_VERBOSE("++[%s].refCount: %u", GblType_name(GBL_TYPE_(pMeta)), oldCount+1);

    //GBL_CTX_POP(1);
    GBL_CTX_END_BLOCK();
    return pClass;
}

GBL_EXPORT GblClass* GblClass_weakRefDefault(GblType type) GBL_NOEXCEPT {
    GblClass*       pClass  = NULL;
    GblMetaClass*   pMeta   = GBL_META_CLASS_(type);
    GBL_CTX_BEGIN(pCtx_);
    GBL_CTX_VERIFY_TYPE(type);
    GBL_CTX_VERIFY(pMeta->pClass, GBL_RESULT_ERROR_INVALID_OPERATION,
                  "[GblType] GblClass_weakRefDefault(%s): no class!", GblType_name(type));
    GBL_CTX_VERIFY(GBL_CLASS_TYPEOF_(pMeta->pClass) != GBL_INVALID_TYPE,
                   GBL_RESULT_ERROR_INVALID_OPERATION,
                   "Cannot weakly reference an invalid class.");
    GBL_CTX_VERIFY(pMeta->refCount, GBL_RESULT_ERROR_INVALID_OPERATION,
                   "Cannot weakly reference an uninitialized class: %s", GblType_name(type));
    pClass = pMeta->pClass;

    GBL_CTX_END_BLOCK();
    return pClass;
}

GBL_EXPORT GBL_RESULT GblClass_verifyFloatingConstruction(GblType type, GblBool inPlace) {
    GBL_CTX_BEGIN(pCtx_);
    GblMetaClass* pMeta = GBL_META_CLASS_(type);

    GBL_CTX_VERIFY(!(inPlace && pMeta->classPrivateOffset != 0),
                   GBL_RESULT_ERROR_INVALID_OPERATION,
                   "Cannot construct a class IN PLACE which contains PRIVATE DATA!");
    GBL_CTX_END();
}


GBL_EXPORT GBL_RESULT GblClass_constructFloating(GblClass* pSelf, GblType type) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(pCtx_);
    GBL_CTX_VERIFY_TYPE(type);
    GBL_CTX_VERIFY_POINTER(pSelf);
    GBL_CTX_CALL(GblClass_verifyFloatingConstruction(type, GBL_TRUE));
    GblClass_refDefault(type); // have to reference actual type
    GBL_CTX_CALL(GbClass_construct_(pSelf,
                                    GBL_META_CLASS_(type),
                                    GBL_CLASS_FLAG_IN_PLACE_));
    GBL_CTX_END();
}

GBL_EXPORT GblClass* GblClass_createFloating(GblType type) GBL_NOEXCEPT {
    GblClass* pClass = NULL;
    GBL_CTX_BEGIN(pCtx_);
    GBL_CTX_VERIFY_TYPE(type);
    GBL_CTX_CALL(GblClass_verifyFloatingConstruction(type, GBL_FALSE));
    GblClass_refDefault(type);
    pClass = GblClass_create_(GBL_META_CLASS_(type), GBL_TRUE);
    GBL_CTX_END_BLOCK();
    return pClass;
}

static GBL_EXPORT GBL_RESULT GblClass_destruct_(GblClass* pClass) {
    GblMetaClass* pMeta = GBL_META_CLASS_(GBL_CLASS_TYPEOF(pClass));
    GBL_CTX_BEGIN(pCtx_);
    GBL_CTX_DEBUG("Destroying %s class!", GblType_name(GBL_TYPE_(pMeta)));
    GBL_CTX_PUSH();
    GblMetaClass* pIter = pMeta;
    // walk up the destructor chain
    GBL_CTX_PUSH_VERBOSE("Walking class destructors.");

    // iterate from derived to base class
    do {
        // call finalizer if class provides one
        if(pIter->pInfo->pFnClassFinal) {
            GBL_CTX_DEBUG("Calling class dtor: [%s]", GblType_name(GBL_TYPE_(pIter)));
            GBL_CTX_CALL(pIter->pInfo->pFnClassFinal(pClass, pIter->pInfo->pClassData, pCtx_));
        } else {
            GBL_CTX_DEBUG("No class dtor: [%s]", GblType_name(GBL_TYPE_(pIter)));
        }

        // iterate over all interfaces
        for(size_t  i = 0; i < pIter->pInfo->interfaceCount; ++i) {
            GblInterface* pInterface = (GblInterface*)((const char*)pClass +
                                                       pIter->pInfo->pInterfaceMap[i].classOffset);
            GblClass* pDefaultIFaceClass = GblClass_weakRefDefault(GBL_CLASS_TYPEOF(pInterface));

            // destruct interface implementation
            GblClass_destruct_(GBL_CLASS(pInterface));

            // unreference default interface class
            GblClass_unrefDefault(pDefaultIFaceClass);
        }
        pIter = pIter->pParent;
    } while(pIter);
    GBL_CTX_POP(1);

    // Store parent class
    GblClass* pParentClass = pMeta->pParent? pMeta->pParent->pClass : NULL;

    //Release reference to parent class
    if(pParentClass) GblClass_unrefDefault(pParentClass);

    // clear the type so it's not looking initialized
    GBL_CLASS_PRIVATE_SET_(pClass, GBL_INVALID_TYPE, 0);

    GBL_CTX_POP(1);
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblClass_destructFloating_(GblClass* pSelf) {
    GBL_CTX_BEGIN(pCtx_);
    const GblType type = GBL_CLASS_TYPEOF(pSelf);
    GBL_CTX_VERIFY(!GblClass_isDefault(pSelf),
                   GBL_RESULT_ERROR_INVALID_OPERATION,
                   "Attempting to manually destruct default class: [%s]",
                   GblType_name(GBL_CLASS_TYPEOF(pSelf)));
    GBL_CTX_CALL(GblClass_destruct_(pSelf));
    GblClass_unrefDefault(GblClass_weakRefDefault(type));
    GBL_CTX_END();
}


GBL_EXPORT GBL_RESULT GblClass_destructFloating(GblClass* pSelf) {
    GBL_CTX_BEGIN(pCtx_);
    if(pSelf) {
        GBL_CTX_VERIFY(GblClass_isInPlace(pSelf),
                       GBL_RESULT_ERROR_INVALID_OPERATION,
                       "Attempting to DESTRUCT dynamically CREATED class: [%s]",
                       GblType_name(GBL_CLASS_TYPEOF(pSelf)));
        GBL_CTX_VERIFY_CALL(GblClass_destructFloating_(pSelf));
    }
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblClass_destroyFloating(GblClass* pSelf) {
    GBL_CTX_BEGIN(pCtx_);
    if(pSelf) {
        GBL_CTX_VERIFY(!GblClass_isInPlace(pSelf),
                       GBL_RESULT_ERROR_INVALID_OPERATION,
                       "Attempng to DESTROY in-place CONSTRUCTED class: [%s]",
                       GblType_name(GBL_CLASS_TYPEOF(pSelf)));
        void* pBase = GblClass_basePtr_(pSelf);
        GBL_CTX_VERIFY_CALL(GblClass_destructFloating_(pSelf));
        GBL_CTX_FREE(pBase);
    }
    GBL_CTX_END();
}

GBL_EXPORT GblRefCount GblClass_unrefDefault(GblClass* pSelf) GBL_NOEXCEPT {
    GblRefCount refCount    = 0;
    GblMetaClass* pMeta     = NULL;
    GBL_CTX_BEGIN(pCtx_);
    if(!pSelf) GBL_CTX_DONE(); //valid to Unref NULL pointer

    GBL_CTX_VERIFY(GBL_CLASS_TYPEOF(pSelf) != GBL_INVALID_TYPE,
                   GBL_RESULT_ERROR_INTERNAL,
                   "Class::unreference(): The specified class has an invalid ID!");

    pMeta = GBL_META_CLASS_(GBL_CLASS_TYPEOF(pSelf));
    refCount = GBL_ATOMIC_INT16_LOAD(pMeta->refCount);

    //GBL_CTX_PUSH_VERBOSE("Class::unreference(%s): %u",
    //                     GblType_name(GblClass_typeOf(pSelf)),
    //                     refCount - 1);

    GBL_CTX_VERIFY(GblClass_isDefault(pSelf),
                   GBL_RESULT_ERROR_INVALID_OPERATION,
                   "Cannot unreference a non-default class!");

    GBL_CTX_VERIFY(refCount != 0,
                   GBL_RESULT_ERROR_INTERNAL,
                   "The refcount for the given class was already at 0!");

    refCount = GBL_ATOMIC_INT16_DEC(pMeta->refCount) - 1;

    if(refCount) {
        ;//GBL_CTX_VERBOSE("--[%s].refCount: %u", GblType_name(GBL_TYPE_(pMeta)), refCount);
    } else {
        GblRefCount instanceRefCount = 0;
        if((instanceRefCount = GBL_ATOMIC_INT16_LOAD(pMeta->instanceRefCount)))
            GBL_CTX_WARN("0 class references with remaining instance references: %u", instanceRefCount);

        if(pMeta->flags & GBL_TYPE_FLAG_CLASS_PINNED)
            //GBL_CTX_VERBOSE("Class::unreference(%s): 0 - Preserving pinned class",
            //                GblType_name(GblClass_typeOf(pSelf)));
            ;
        else
            GBL_CTX_VERIFY_CALL(GblClass_destruct_(pSelf));
    }

    //GBL_CTX_POP(1);
    GBL_CTX_END_BLOCK();
    return refCount;
}

// Cast a class to the given type, with optional error checking and validation (recurses for interfaces)
static GblClass* GblClass_cast_(GblClass* pClass, GblType toType, GblBool check, GblBool recursing) {
    GblClass* pClassStart = pClass;
    GblClass* pToClass    = NULL;

    // Casting a NULL pointer or to an invalid type returns NULL
    if(pClass && toType != GBL_INVALID_TYPE) GBL_LIKELY {
        //early exit when class is already of desired type
        if(GBL_CLASS_META_CLASS_(pClass) == GBL_META_CLASS_(toType)) GBL_LIKELY {
            pToClass = pClass;
        } else {
            GblBool toInterface = GBL_FALSE;
            if(GBL_TYPE_INTERFACED_CHECK(toType)) toInterface = GBL_TRUE;

            // If current node is an interface class, attempt to "back out" to root class node
            // BUT ONLY IF WE AREN'T TRYING TO CHECK AN INTERFACE RECURSIVELY
            if(!recursing) {
                while(GBL_TYPE_INTERFACED_CHECK(GBL_CLASS_TYPEOF(pClass))) {
                    // Move class pointer to interface's concrete class
                    pClass = GblInterface_outerClass((GblInterface*)pClass);
                    GBL_ASSERT(pClass);
                }
            }

            GblMetaClass* pMeta = GBL_CLASS_META_CLASS_(pClass);
            GBL_ASSERT(pMeta);

            // iterate from derived to base class, breadth-first searching
            do {
                //check current class
                if(pMeta == GBL_META_CLASS_(toType)) {
                    pToClass = pClass;
                    break;
                } else if(toInterface) {
                    for(unsigned i = 0; i < pMeta->pInfo->interfaceCount; ++i) {

                        GblMetaClass* pIFaceMeta = GBL_META_CLASS_(toType);
                        GblInterface* pCurIClass = (GblInterface*)((uintptr_t)pClass +
                                                    pMeta->pInfo->pInterfaceMap[i].classOffset);

                        /* Assume an interface is invalid, because it has yet to be constructed,
                           so skip using it for further consideration. */
                        if(GBL_CLASS_TYPEOF(pCurIClass) == GBL_INVALID_TYPE)
                            continue;

                        GBL_ASSERT(GBL_CLASS_FLAG_TEST_(GBL_CLASS(pCurIClass), GBL_CLASS_FLAG_IFACE_IMPL_));
                        GBL_ASSERT(GBL_META_CLASS_(pMeta->pInfo->pInterfaceMap[i].interfaceType)
                                                    == GBL_META_CLASS_(GBL_CLASS_TYPEOF(pCurIClass)));
                        GBL_ASSERT(GBL_CLASS_TYPEOF(pCurIClass) != GBL_INVALID_TYPE);

                        if(GBL_META_CLASS_(GBL_CLASS_TYPEOF(pCurIClass)) == pIFaceMeta) {
                            pToClass = GBL_CLASS(pCurIClass);
                            break;
                        } else {
                            pCurIClass = (GblInterface*)GblClass_cast_(GBL_CLASS(pCurIClass), toType, GBL_FALSE, GBL_TRUE);
                            if(pCurIClass) {
                                GBL_ASSERT(GblType_check(GBL_CLASS_TYPEOF(pCurIClass), toType)); //SLOOOW, but good check for Debug
                                pToClass = GBL_CLASS(pCurIClass);
                                break;
                            }
                        }
                    }
                }
                pMeta = pMeta->pParent;
            } while(!pToClass && pMeta);
        }
    }
    if(check && !pToClass) {
        GBL_CTX_BEGIN(pCtx_);
        if(toType == GBL_INVALID_TYPE) {
            if(pClass) {
                GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_TYPE_MISMATCH,
                                   "Attempted to cast from type %s to GBL_INVALID_TYPE!",
                                   GblType_name(GblClass_typeOf(pClassStart)));
            }
        } else {
            GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_TYPE_MISMATCH,
                               "Failed to cast from type %s to %s!",
                               GblType_name(GblClass_typeOf(pClassStart)), GblType_name(toType));
        }
        GBL_CTX_END_BLOCK();
    }
    return pToClass;
}

GBL_EXPORT GblBool GblClass_check(const GblClass* pSelf, GblType toType) {
    GblBool result = GBL_FALSE;

    if(!pSelf && toType == GBL_INVALID_TYPE) {
        result = GBL_TRUE;
    } else if(!pSelf) {
        GBL_CTX_BEGIN(pCtx_);
        GBL_CTX_VERIFY_POINTER(pSelf);
        GBL_CTX_END_BLOCK();
    } else {
        result = GblType_check(GBL_CLASS_TYPEOF(pSelf), toType);
    }
    return result;
}

GBL_EXPORT GblClass* GblClass_cast(GblClass* pSelf, GblType toType) GBL_NOEXCEPT {
    return GblClass_cast_(pSelf, toType, GBL_TRUE, GBL_FALSE);
}
GBL_EXPORT GblClass* GblClass_try(GblClass* pSelf, GblType toType) GBL_NOEXCEPT {
    return GblClass_cast_(pSelf, toType, GBL_FALSE, GBL_FALSE);
}


GBL_EXPORT GblBool GblClass_isDefault(const GblClass* pSelf) GBL_NOEXCEPT {
    return pSelf && pSelf == GBL_CLASS_DEFAULT(pSelf);
}

GBL_EXPORT GblType GblClass_typeOf(const GblClass* pSelf) GBL_NOEXCEPT {
    GblType type = GBL_INVALID_TYPE;
    if(pSelf) {
        type = GBL_CLASS_TYPEOF_(pSelf);
    }
    return type;
}

GBL_EXPORT GblClass* GblClass_super(const GblClass* pSelf) GBL_NOEXCEPT {
    GblClass* pClass = NULL;
    if(pSelf) {
        const GblType parent = GblType_parent(GBL_CLASS_TYPEOF(pSelf));
        if(GblType_verify(parent)) {
            pClass = GblClass_weakRefDefault(parent);
        }
    }
    return pClass;
}

GBL_EXPORT GblClass* GblClass_default(const GblClass* pSelf) GBL_NOEXCEPT {
    return pSelf? GblClass_weakRefDefault(GBL_CLASS_TYPEOF(pSelf)) : GBL_NULL;
}

GBL_EXPORT size_t  GblClass_size(const GblClass* pSelf) GBL_NOEXCEPT {
    size_t  size = 0;
    if(pSelf) {
        const GblTypeInfo* pInfo = GblType_info(GBL_CLASS_TYPEOF(pSelf));
        if(pInfo) {
            size = pInfo->classSize;
        }
    }
    return size;
}



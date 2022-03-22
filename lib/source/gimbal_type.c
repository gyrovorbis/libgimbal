#include <gimbal/meta/gimbal_type.h>
#include <gimbal/meta/gimbal_class.h>
#include <gimbal/meta/gimbal_instance.h>
#include <gimbal/meta/gimbal_interface.h>
#include <gimbal/containers/gimbal_hashset.h>
#include <gimbal/algorithms/gimbal_hash.h>
#include <gimbal/algorithms/gimbal_sort.h>
#include <gimbal/containers/gimbal_vector.h>
#include <stdatomic.h>
#include <threads.h>

#define GBL_TYPE_REGISTRY_HASH_MAP_CAPACITY_DEFAULT 32
#define GBL_META_TYPE_NAME_SIZE_MAX                 1024

typedef atomic_uint_fast16_t            GblRefCounter;

// === STATIC DATA ====

typedef struct GblMetaClass {
    struct GblMetaClass*    pParent;
    const char*             pName;
    GblRefCounter           refCount;
#ifdef GBL_TYPE_DEBUG
    GblRefCounter           instanceRefCount;
#endif
    GblTypeInfo             info;
    GblFlags                flags; // whether pClass is static and allocated with class or lazily allocated and dynamic
    void*                   pModule; //module
    GblClass*               pClass;
    uint8_t                 ifaceCount;
    GblTypeInterfaceEntry   ifaces[];
} GblMetaClass;

static  GblContext  pCtx_       = NULL;
static  once_flag   initOnce_   = ONCE_FLAG_INIT;
static  mtx_t       typeRegMtx_;
static  GblHashSet  typeRegistry_;

static int metaClassIFaceEntryComparator_(const void* pA, const void* pB) {
    const GblTypeInterfaceEntry* pIFaceA = pA;
    const GblTypeInterfaceEntry* pIfaceB = pB;
    return pIFaceA->interfaceType - pIfaceB->interfaceType;
}

static GblHash metaClassHasher_(const GblHashSet* pMap, const void* pItem) {
    GBL_UNUSED(pMap);
    const GblMetaClass** pMeta = (const GblMetaClass**)pItem;
    return gblHashMurmur((*pMeta)->pName, strnlen((*pMeta)->pName, GBL_META_TYPE_NAME_SIZE_MAX));
}

static const void* metaClassKey_(const GblHashSet* pMap, const void* pEntry) {
    GBL_UNUSED(pMap);
    GblMetaClass* pClass = *(GblMetaClass**)pEntry;
    return pClass->pName;
}

static int metaClassComparator_(const GblHashSet* pMap, const void* pA, const void* pB) {
    GBL_UNUSED(pMap);
    const char *pCharA = (const char*)pA, *pCharB = (const char*)pB;
    return strncmp(pCharA, pCharB, GBL_META_TYPE_NAME_SIZE_MAX);
}

static void metaClassElementFree_(const GblHashSet* pMap, void* item) {
    GBL_UNUSED(pMap);
    GBL_API_BEGIN(pCtx_);
    GblMetaClass** ppMetaClass = (GblMetaClass**)item;
    GBL_API_FREE(*ppMetaClass);
    GBL_API_END_BLOCK();
}

static void gblTypeInit_(void) {
    GBL_API_BEGIN(pCtx_);
    mtx_init(&typeRegMtx_, mtx_plain);
    GBL_API_PUSH_VERBOSE("[GblType]: Initializing.");
    mtx_lock(&typeRegMtx_);
    GBL_API_RESULT() = GblHashSet_construct(&typeRegistry_,
                                sizeof(GblMetaClass*),
                                metaClassHasher_,
                                metaClassComparator_,
                                metaClassElementFree_,
                                metaClassKey_,
                                NULL,
                                GBL_TYPE_REGISTRY_HASH_MAP_CAPACITY_DEFAULT,
                                pCtx_);
    mtx_unlock(&typeRegMtx_);
    GBL_API_POP(1);
    GBL_API_END_BLOCK();
}

static GBL_RESULT gblTypeFinalize_(void) {
    GBL_API_BEGIN(pCtx_);
    GBL_API_PUSH_VERBOSE("[GblType]: Finalizing");
    mtx_lock(&typeRegMtx_);

    GblHashSet_destruct(&typeRegistry_);

    GBL_API_POP(1);
    GBL_API_END_BLOCK();
    mtx_unlock(&typeRegMtx_);
    return GBL_API_RESULT();
}

// === PUBLIC API ====

GBL_API gblTypeReinit(GblContext hCtx) {
    GBL_API_BEGIN(pCtx_);
    {
        GBL_API_PUSH_VERBOSE("[GblType]: Reinititalizing.");
        call_once(&initOnce_, gblTypeInit_);
        GBL_API_CALL(gblTypeFinalize_());
        pCtx_ = hCtx;
    }
    {
        GBL_API_BEGIN(pCtx_);
        gblTypeInit_();
        GBL_API_END();
    }
}


GBL_EXPORT GblType gblTypeFind(const char* pTypeName) {
    GblType foundType = GBL_TYPE_INVALID;
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_POINTER(pTypeName);
    call_once(&initOnce_, gblTypeInit_);

    if(strnlen(pTypeName, GBL_META_TYPE_NAME_SIZE_MAX+1) > GBL_META_TYPE_NAME_SIZE_MAX)
        GBL_API_WARN("Typename is too large and will be truncated: %s", pTypeName);

    mtx_lock(&typeRegMtx_);
    foundType = (GblType)*(GblMetaClass**)GblHashSet_get(&typeRegistry_, pTypeName);
    mtx_unlock(&typeRegMtx_);

    GBL_API_END_BLOCK();
    return foundType;
}

GBL_EXPORT GblSize gblTypeCount(void) {
    GblSize count = 0;
    GBL_API_BEGIN(pCtx_);
    call_once(&initOnce_, gblTypeInit_);
    mtx_lock(&typeRegMtx_);
    count = GblHashSet_size(&typeRegistry_);
    mtx_unlock(&typeRegMtx_);
    GBL_API_END_BLOCK();
    return count;
}

GBL_EXPORT const char* gblTypeName(GblType type) {
    const char*     pName   = NULL;
    GblMetaClass*   pMeta   = (GblMetaClass*)type;
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_ARG(type != GBL_TYPE_INVALID);
    pName = pMeta->pName;
    GBL_API_END_BLOCK();
    return pName;
}

GBL_EXPORT GblType gblTypeParent(GblType type) {
    GblType parent      = GBL_TYPE_INVALID;
    GblMetaClass* pMeta = (GblMetaClass*)type;
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_ARG(pMeta != NULL);
    parent = (GblType)pMeta->pParent;
    GBL_API_END_BLOCK();
    return parent;
}

GBL_EXPORT GblType gblTypeBase(GblType type) {
    GblType base      = GBL_TYPE_INVALID;
    GblMetaClass* pMeta = (GblMetaClass*)type;
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_ARG(pMeta != NULL);
    while(pMeta->pParent) pMeta = pMeta->pParent;
    base = (GblType)pMeta;
    GBL_API_END_BLOCK();
    return base;
}

GBL_EXPORT GblTypeFlags gblTypeFlags(GblType type) {
    GblTypeFlags flags      = 0;
    GblMetaClass* pMeta = (GblMetaClass*)type;
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_ARG(pMeta != NULL);
    flags = pMeta->flags;
    GBL_API_END_BLOCK();
    return flags;
}

GBL_EXPORT const GblTypeInfo*   gblTypeInfo(GblType type) {
    const GblTypeInfo* pInfo      = 0;
    GblMetaClass* pMeta = (GblMetaClass*)type;
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_ARG(pMeta != NULL);
    pInfo = &pMeta->info;
    GBL_API_END_BLOCK();
    return pInfo;
}

GBL_EXPORT GblRefCount gblTypeClassRefCount(GblType type) {
    GblRefCount refCount      = 0;
    GblMetaClass* pMeta = (GblMetaClass*)type;
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_ARG(pMeta != NULL);
    refCount = atomic_load(&pMeta->refCount);
    GBL_API_END_BLOCK();
    return refCount;
}

#ifdef GBL_TYPE_DEBUG
GBL_EXPORT GblRefCount gblInstanceRefCount(GblType type) {
    GblRefCount refCount      = 0;
    GblMetaClass* pMeta = (GblMetaClass*)type;
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_ARG(pMeta != NULL);
    refCount = atomic_load(&pMeta->instanceRefCount);
    GBL_API_END_BLOCK();
    return refCount;
}
#endif

GBL_EXPORT GblBool gblTypeIsA(GblType derived, GblType base) {
    GblBool result      = 0;
    GblMetaClass* pDerived = (GblMetaClass*)derived;
    GblMetaClass* pBase    = (GblMetaClass*)base;
    GblMetaClass* pIter     = pDerived;
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_ARG(derived != GBL_TYPE_INVALID);
    GBL_API_VERIFY_ARG(base != GBL_TYPE_INVALID);
    // Walk the type hierarchy (STOP IGNORING INTERFACES)!
    while((pIter = pIter->pParent)) {
        if(pIter == pBase) {
            result = GBL_TRUE;
            break;
        }
    }
    GBL_API_END_BLOCK();
    return result;
}

// ==== MAKE SURE TO ITERATE OVER PARENTS AND CHECK THAT CLASS/INSTANCE SIZE IS VALID =====
GBL_EXPORT GblType gblTypeRegister(GblType                      parent,
                                   const char*                  pName,
                                   const GblTypeInfo*           pInfo,
                                   GblFlags                     flags,
                                   const GblTypeInterfaceEntry* pIFaces[])
{
    GblBool hasMutex = GBL_FALSE;
    GblType newType = GBL_TYPE_INVALID;
    GblMetaClass* pParent = (GblMetaClass*)parent;
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_POINTER(pName);
    GBL_API_VERIFY_POINTER(pInfo);

    GBL_API_PUSH_VERBOSE("[GblType] Register: Type %s derived from %s", pName, pParent->pName);

    GBL_API_VERIFY_ARG(gblTypeFind(pName) == GBL_TYPE_INVALID,
                        "Existing entry for the given type name was found!");

    GBL_API_VERIFY_ARG(pInfo->classSize >= pParent->info.classSize,
                        "Class size [%u] must be >= to parent's class size [%u]!",
                        pInfo->classSize, pParent->info.classSize);

    GBL_API_VERIFY_ARG(pInfo->classAlign >= pParent->info.classAlign,
                          "Class alignment [%u] must be >= to parent's class alignment [%u]!",
                          pInfo->classAlign, pParent->info.classAlign);

    GBL_API_VERIFY_ARG(pInfo->instanceSize >= pParent->info.instanceSize,
                          "Instance size [%u]must be >= to parent's instance size [%u]!",
                          pInfo->instanceSize, pParent->info.instanceSize);

    GBL_API_VERIFY_ARG(pInfo->instanceAlign >= pParent->info.instanceAlign,
                          "Instance alignment [%u] must be >= to parent's instance alignment [%u]!",
                          pInfo->instanceAlign, pParent->info.instanceAlign);

    call_once(&initOnce_, gblTypeInit_);
    {
        GblSize ifaceCount = 0;
        if(pIFaces) {
            const GblTypeInterfaceEntry* pIfaceEntry;
            while((pIfaceEntry = pIFaces[ifaceCount])) {
                ++ifaceCount;
            }
        }

        GblSize metaSize = sizeof(GblMetaClass);
        const GblSize nameOffset = metaSize;
        const GblSize classOffset = metaSize += strlen(pName)+1;
        const GblSize ifaceOffset = metaSize += pInfo->classSize;
        metaSize += sizeof(GblTypeInterfaceEntry) * ifaceCount;

        GblMetaClass* pMeta = GBL_API_MALLOC(metaSize, pInfo->classAlign, pName);

        memset(pMeta, 0, metaSize);
        memcpy(&pMeta->info, pInfo, sizeof(GblTypeInfo));
        atomic_init(&pMeta->refCount, 0);
#ifdef GBL_TYPE_DEBUG
        atomic_init(&pMeta->instanceRefCount, 0);
#endif
        strcpy((char*)(pMeta + nameOffset), pName);
        pMeta->flags        = flags;
        pMeta->pParent      = (struct GblMetaClass*)parent;
        pMeta->pClass       = (GblClass*)(uintptr_t)(pMeta + classOffset);
        pMeta->ifaceCount   = ifaceCount;
        for(uint8_t i = 0; i < ifaceCount; ++i) {
            memcpy((uint8_t*)(pMeta + ifaceOffset + i*sizeof(GblTypeInterfaceEntry)), pIFaces[i], sizeof(GblTypeInterfaceEntry));
        }

        gblSortQuick(pMeta->ifaces, pMeta->ifaceCount, sizeof(GblTypeInterfaceEntry), metaClassIFaceEntryComparator_);

        mtx_lock(&typeRegMtx_);
        hasMutex = GBL_TRUE;
        GblMetaClass* pOldData = GblHashSet_set(&typeRegistry_,
                                   &pMeta);

        GBL_API_VERIFY(!pOldData, GBL_RESULT_ERROR_INVALID_ARG,
                       "A previous metatype entry named %s existed already!", pName);

        newType = (GblType)*(GblMetaClass**)GblHashSet_get(&typeRegistry_, pName);
    }
    GBL_API_POP(1);
    GBL_API_END_BLOCK();
    if(hasMutex) mtx_unlock(&typeRegMtx_);
    return newType;
}

GBL_EXPORT void gblTypeUnregister(GblType type) {
    GblMetaClass* pMeta = (GblMetaClass*)type;
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_ARG(type != GBL_TYPE_INVALID);
    GBL_API_PUSH_VERBOSE("[GblType] Unregister: %s", pMeta->pName);
    call_once(&initOnce_, gblTypeInit_);
    {
        GblRefCount refCount = gblTypeClassRefCount(type);
        if(refCount) {
            GBL_API_WARN("Attempting to unregister type with active class references: %u", refCount);
        }

#ifdef GBL_TYPE_DEBUG
        refCount = gblTypeInstanceRefCount(type);
        if(refCount) {
            GBL_API_WARN("Attempting to unregister type with active class references: %u", refCount);
        }
#endif

        GblMetaClass* pRet  = NULL;
        mtx_lock(&typeRegMtx_);
        pRet = GblHashSet_extract(&typeRegistry_, pMeta);
        mtx_unlock(&typeRegMtx_);
        GBL_API_RECORD_SET(GBL_API_RESULT());
        GBL_API_VERIFY(pRet, GBL_RESULT_ERROR_INVALID_ARG,
                       "Meta Type entry for the specified type was not found! [%s]",
                       pRet->pName);
        GBL_API_VERIFY(pRet == pMeta, GBL_RESULT_ERROR_INTERNAL,
                       "Wtf, the two types don't even match! [%s vs %s]",
                       pMeta->pName, pRet->pName);

        GBL_API_FREE(pRet);

    }
    GBL_API_POP(1);
    GBL_API_END_BLOCK();
}

static GBL_RESULT gblTypeClassConstruct_(GblClass* pClass, GblMetaClass* pMeta);

static GBL_RESULT gblTypeInterfaceClassConstruct_(GblInterfaceClass* pClass, GblMetaClass* pMeta, int16_t offset) {
    GBL_API_BEGIN(pCtx_);
    GBL_API_PUSH_VERBOSE("[GblType] InterfaceClass::construct(%s)", pMeta->pName);
    GBL_API_VERBOSE("offset: %d", offset);
    pClass->offsetToTop = offset;
    GBL_API_CALL(gblTypeClassConstruct_(&pClass->base, pMeta));
    GBL_API_POP(1);
    GBL_API_END();
}

/*
 * Constructor initializes immediate data (typeId) then itereates over all bases,
 * initializing them from top to bottom. Each base has its interfaces initialized
 * before the rest of its class.
 */
static GBL_RESULT gblTypeClassConstruct_(GblClass* pClass, GblMetaClass* pMeta) {
    GBL_API_BEGIN(pCtx_);
    GBL_API_PUSH_VERBOSE("[GblType] Class::construct(%s)", pMeta->pName);
    GBL_API_VERBOSE("TypeId: %u", (GblType)pMeta);

    //IMMEDIATELY initialize its type!!!
    pClass->typeId = (GblType)pMeta;

    if(pMeta->pParent) {
        GBL_API_PUSH_VERBOSE("Adding reference to parent class: ", pMeta->pParent->pName);
        //ensure construction of parent class (this will recurse)
        gblTypeClassRef((GblType)pMeta->pParent);
        GBL_API_POP(1);
    } else {
        GBL_API_VERBOSE("No parent class to add reference to!");
    }

    // Create a structure to hold all base types
    const GblSize vecStackSize = sizeof(GblVector) + 20*sizeof(GblMetaClass*);
    GblVector* pBases = GBL_ALLOCA(vecStackSize);
    GBL_API_CALL(gblVectorConstruct(pBases, GBL_API_CONTEXT(), sizeof(GblMetaClass*), vecStackSize));

    GBL_API_DEBUG("Populating base class initializer chain.");

    // Populate structure by traversing hierarchy
    GblMetaClass* pIter = pMeta;
    while(pIter) {
        GBL_API_CALL(gblVectorPushBack(pBases, pIter));
        pIter = pIter->pParent;
    }

    GBL_API_DEBUG("Calling initializer chain.");
    GBL_API_PUSH();

    // Iterate backwards over the structure calling ctors from base to derived order
    GblSize count = 0;
    GBL_API_CALL(gblVectorSize(pBases, &count));
    GBL_API_VERIFY(count, GBL_RESULT_ERROR_INTERNAL);
    for(GblSize idx = count-1; idx >= 0; --idx) {
        GBL_API_CALL(gblVectorAt(pBases, idx, (void**)&pIter));
        GBL_API_PUSH_VERBOSE("[%u]: %s", count+1 - idx, pIter->pName);

        GBL_API_PUSH_VERBOSE("Calling Interface Constructors");
        for(int i = 0; i < pIter->ifaceCount; ++i) {
            const GblMetaClass* pIMeta = (const GblMetaClass*)pIter->ifaces[i].interfaceType;
            const GblTypeInterfaceEntry* pIEntry = &pIter->ifaces[i];
            GBL_API_VERIFY_EXPRESSION(pIMeta);
            GBL_API_PUSH_VERBOSE("[%u]: %s", i, pIMeta->pName);
            GblInterfaceClass* pIClass = (GblInterfaceClass*)((char*)pClass + pIEntry->classOffset);
            GBL_API_CALL(gblTypeInterfaceClassConstruct_(pIClass, pMeta, -pIEntry->classOffset));
            GBL_API_POP(1);
        }
        GBL_API_POP(1);

        if(pIter->info.pFnClassInit) {
            GBL_API_CALL(pIter->info.pFnClassInit(pClass, pMeta->info.pClassData));
        } else {
            GBL_API_DEBUG("No class ctor: [%s]", pIter->pName);
        }

        GBL_API_POP(1);
    }

    GBL_API_POP(1);

    // Free the class heirarchy structure
    GBL_API_CALL(gblVectorDestruct(pBases));

    GBL_API_POP(1);
    GBL_API_END();
}

static GblClass* gblTypeClassCreate_(GblMetaClass* pMeta) {
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_ARG(pMeta);
    GBL_API_PUSH_VERBOSE("[GblType] Class::create(%s)", pMeta->pName);
    GBL_API_VERIFY_EXPRESSION(!atomic_load(&pMeta->refCount),
                              "Already have a reference to an invalid class object!");

    //Allocate a new class structure if one isn't already available
    if(!pMeta->pClass) {
        GBL_API_VERBOSE("Allocating separate class structure!");
        pMeta->pClass = GBL_API_MALLOC(pMeta->info.classSize, pMeta->info.classAlign, pMeta->pName);
    //Class must've been allocated with Meta class. Use existing data.
    } else {
       GBL_API_VERBOSE("Using existing inline class allocation.");
    }

    // Call constructor
    GBL_API_CALL(gblTypeClassConstruct_(pMeta->pClass, pMeta));

    GBL_API_POP(1);
    GBL_API_END_BLOCK();
    return pMeta->pClass;
}


GBL_EXPORT GblClass* gblTypeClassRef(GblType typeId) {
    GblClass* pClass    = NULL;
    GblMetaClass* pMeta = (GblMetaClass*)typeId;
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_ARG(typeId != GBL_TYPE_INVALID);
    GBL_API_VERIFY(pMeta->info.classSize != 0,
                   GBL_RESULT_UNIMPLEMENTED,
                   "[GblType] Attempt to reference a class of size 0!");

    GBL_API_PUSH_VERBOSE("[GblType] Referencing class for type: %s", pMeta->pName);
    call_once(&initOnce_, gblTypeInit_);

    // Return existing reference to class data
    if(pMeta->pClass && pMeta->pClass->typeId != GBL_TYPE_INVALID) {
        GBL_API_VERIFY_EXPRESSION(atomic_load(&pMeta->refCount),
                                  "No references to an initialized class!?");
        GBL_API_DEBUG("Using existing class data");
        pClass = pMeta->pClass;

    // Create a new class structure
    } else {
        pClass = gblTypeClassCreate_(pMeta);
        GBL_API_VERIFY_EXPRESSION(pClass && pClass == pMeta->pClass, "Failed to create class!");
    }

    // Either way, we're returning a new reference, add refcount
    GblRefCount oldCount = atomic_fetch_add(&pMeta->refCount, 1);
    GBL_API_DEBUG("Incrementing RefCount: "GBL_SIZE_FMT, oldCount+1);

    GBL_API_POP(1);
    GBL_API_END_BLOCK();
    return pClass;
}

GBL_EXPORT GblClass* gblTypeClassPeek(GblType typeId) {
    GblClass* pClass    = NULL;
    GblMetaClass* pMeta = (GblMetaClass*)typeId;
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_ARG(typeId != GBL_TYPE_INVALID);
    pClass = pMeta->pClass;
    GBL_API_END_BLOCK();
    return pClass;
}

GBL_EXPORT GblRefCount gblTypeClassUnref(GblClass* pClass) {
    GblRefCount refCount    = 0;
    GblMetaClass* pMeta     = NULL;
    GBL_API_BEGIN(pCtx_);
    if(!pClass) GBL_API_DONE(); //valid to Unref NULL pointer

    GBL_API_PUSH_VERBOSE("[GblType] Unreferencing class for type: %s", pMeta->pName);

    GBL_API_VERIFY(pClass->typeId != GBL_INDEX_INVALID,
                   GBL_RESULT_ERROR_INTERNAL,
                   "The specified class has an invalid ID!");

    pMeta = (GblMetaClass*)pClass->typeId;
    GBL_API_VERIFY(atomic_load(&pMeta->refCount) != 0,
                   GBL_RESULT_ERROR_INTERNAL,
                   "The refcount for the given class was already at 0!");

    refCount = atomic_fetch_sub(&pMeta->refCount, 1);

    if(refCount) {
        GBL_API_DEBUG("Decrementing refCount: %u", refCount-1);
    } else {
        GBL_API_DEBUG("Destroying %s class!", pMeta->pName);
        GBL_API_PUSH();

#ifdef GBL_TYPE_DEBUG
        {
            GblRefCount instanceRefCount = 0;
            instanceRefCount = atomic_load(&pMeta->instanceRefCount);
        GBL_API_VERIFY_EXPRESSION(instanceRefCount > 0,
                                  "Cannot destroy class when there are still %u instances using it!", instanceRefCount)
        }
#endif

        GblMetaClass* pIter = pMeta;
        // walk up the destructor chain
        GBL_API_DEBUG("Walking class destructors.");
        do {
            if(pIter->info.pFnClassFinalize) {
                GBL_API_DEBUG("Calling class dtor: [%s]", pIter->pName);
                GBL_API_CALL(pIter->info.pFnClassFinalize(pMeta->pClass, pMeta->info.pClassData));
            } else {
                GBL_API_DEBUG("No class dtor: [%s]", pIter->pName);
            }
            pIter = pIter->pParent;
        } while(pIter);
        GBL_API_POP(1);

        // Store parent pointer then deallocate
        GblClass* pParentClass = pMeta->pParent? pMeta->pParent->pClass : NULL;
        GBL_API_FREE(pMeta->pClass);

        //Release reference to parent class;
        if(pParentClass) gblTypeClassUnref(pParentClass);

        GBL_API_POP(1);
    }

    GBL_API_POP(1);
    GBL_API_END_BLOCK();
    return refCount;
}

GBL_EXPORT GblInstance* gblTypeInstanceCreate(GblType type) {
    GblInstance* pInstance = NULL;
    GblMetaClass* pMeta     = (GblMetaClass*)type;
    GblClass* pClass        = NULL;
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_ARG(type != GBL_TYPE_INVALID, "[GblType] Instance Create: Invalid type!");
    GBL_API_PUSH_VERBOSE("[GblType] Instance Create: type %s", pMeta->pName);
    GBL_API_VERIFY(pMeta->info.instanceSize,
                   GBL_RESULT_ERROR_INVALID_ARG,
                   "Cannot instantiate type with instance of size 0!");

    pClass = gblTypeClassRef(type);
    GBL_API_VERIFY_EXPRESSION(pClass, "Failed to retrieve class reference!");

    GBL_API_DEBUG("Allocating %u bytes.", pMeta->info.instanceSize);

    pInstance = GBL_API_MALLOC(pMeta->info.instanceSize, pMeta->info.instanceAlign, pMeta->pName);
    pInstance->pClass = pClass;

    // Create a structure to hold all base types
    const GblSize vecStackSize = sizeof(GblVector) + 20*sizeof(GblMetaClass*);
    GblVector* pBases = GBL_ALLOCA(vecStackSize);
    GBL_API_CALL(gblVectorConstruct(pBases, GBL_API_CONTEXT(), sizeof(GblMetaClass*), vecStackSize));

    GBL_API_DEBUG("Populating constructor chain.");

    // Populate structure by traversing hierarchy
    GblMetaClass* pIter = pMeta;
    while(pIter) {
        GBL_API_CALL(gblVectorPushBack(pBases, pIter));
        pIter = pIter->pParent;
    }

    GBL_API_DEBUG("Calling constructor chain.");
    GBL_API_PUSH();

    // Iterate backwards over the structure calling ctors from base to derived order
    GblSize count = 0;
    GBL_API_CALL(gblVectorSize(pBases, &count));
    GBL_API_VERIFY(count, GBL_RESULT_ERROR_INTERNAL);
    for(GblSize idx = count-1; idx >= 0; --idx) {
        GBL_API_CALL(gblVectorAt(pBases, idx, (void**)&pIter));
        if(pIter->info.pFnInstanceInit) {
            GBL_API_DEBUG("Calling instance initializers: [%s]", pIter->pName);
            GBL_API_CALL(pIter->info.pFnInstanceInit(pInstance));
        } else {
            GBL_API_DEBUG("No instance ctor: [%s]", pIter->pName);
        }
    }

    GBL_API_POP(1);

    // Free the class heirarchy structure
    GBL_API_CALL(gblVectorDestruct(pBases));

#ifdef GBL_TYPE_DEBUG
    {
        GblRefCount count = 0;
        atomic_fetch_add(&pMeta->instanceRefCount, 1);
        GBL_API_DEBUG("Instance reference count: %u", count+1);
    }
#endif

    GBL_API_POP(1);
    GBL_API_END_BLOCK();
    return pInstance;
}

GBL_EXPORT GblRefCount gblTypeInstanceDestroy(GblInstance* pInstance) {
    GblRefCount refCount = 0;
    GBL_API_BEGIN(pCtx_);
    if(!pInstance) GBL_API_DONE(); //valid to destroy NULL pointer
    else {
        GblClass* pClass = GblInstance_classOf(pInstance);
        GBL_API_VERIFY_EXPRESSION(pClass);
        GBL_API_PUSH_VERBOSE("[GblType] Instance Destroy: %s", GblClass_name(pClass));
#if 0 // instance does not have a virtual destructor!
        GblClass* pIter = pClass;
        // walk up the destructor chain
        GBL_API_DEBUG("Walking instance destructors.");
        do {
        //    if(pIter->pFnDestruct) {
             //   GBL_API_DEBUG("Calling instance dtor: [%s]", GblClass_name(pIter));
          //      GBL_API_CALL(pIter->pFnDestruct(pInstance));

          //  } else {
                GBL_API_DEBUG("No instance dtor: [%s]", GblClass_name(pIter));
           // }
            pIter = GblClass_parentOf(pIter);
        } while(pIter);
        GBL_API_POP(1);
#endif

#ifdef GBL_TYPE_DEBUG
        GblMetaClass* pMeta = (GblMetaClass*)GblClass_type(pClass);
        GblRefCount refCount = atomic_fetch_sub(&pMeta->instanceRefCount, 1);
        GBL_API_DEBUG("Instance reference count: %u", count-1);
#endif

        GBL_API_DEBUG("Releasing class reference.");
        refCount = gblTypeClassUnref(pClass);

        GBL_API_FREE(pInstance);

        GBL_API_POP(1);
    }
    GBL_API_END_BLOCK();
    return refCount;
}

//should be able to make this a binary search!
GBL_EXPORT GblInterfaceClass* gblTypeInterfaceClassPeek(GblClass* pClass, GblType ifaceType) {
    GblInterfaceClass* pIClass = NULL;
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_POINTER(pClass);
    GBL_API_VERIFY_EXPRESSION(pClass->typeId != GBL_TYPE_INVALID);
    GBL_API_VERIFY_ARG(ifaceType != GBL_TYPE_INVALID);
    {
        GblMetaClass* pMeta = (GblMetaClass*)pClass->typeId;
        while(pMeta) {
            for(unsigned i = 0; i < pMeta->ifaceCount; ++i) {
                GblInterfaceClass* pCurIClass = (GblInterfaceClass*)((char*)pClass + pMeta->ifaces[i].classOffset);
                GBL_API_VERIFY_EXPRESSION(pMeta->ifaces[i].interfaceType == pCurIClass->base.typeId);
                GBL_API_VERIFY_EXPRESSION(pCurIClass->base.typeId != GBL_TYPE_INVALID);
                if(pCurIClass->base.typeId == ifaceType) {
                    pIClass = pCurIClass;
                    break;
                } else {
                    pCurIClass = gblTypeInterfaceClassPeek(&pCurIClass->base, ifaceType);
                    if(pCurIClass) {
                        GBL_API_VERIFY_EXPRESSION(pCurIClass->base.typeId == ifaceType);
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

GBL_EXPORT GblInterfaceClass* gblTypeInterfaceClassPeekParent(GblClass* pClass, GblType ifaceType) {
    GblInterfaceClass* pIClass = NULL;
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_POINTER(pClass);
    GBL_API_VERIFY_EXPRESSION(pClass->typeId != GBL_TYPE_INVALID);
    GBL_API_VERIFY_ARG(ifaceType != GBL_TYPE_INVALID);
    {
        GblType parentType = gblTypeParent(GblClass_typeOf(pClass));
        GBL_API_VERIFY_EXPRESSION(parentType != GBL_TYPE_INVALID);
        GblClass* pParentClass = gblTypeClassPeek(parentType);
        if(pParentClass) {
            pIClass = gblTypeInterfaceClassPeek(pParentClass, ifaceType);
        }
    }
    GBL_API_END_BLOCK();
    return pIClass;
}

//maybe make this gracefully fall through to using the default interface implementation?!
GBL_EXPORT GblInterface* gblTypeInterfaceInit(GblInterface* pSelf, GblType ifaceType, GblInstance* pInstance) {
    GblInterface* pIface = NULL;
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_POINTER(pSelf);
    pSelf->pVTable      = NULL;
    pSelf->pInstance    = NULL;
    GBL_API_VERIFY_ARG(ifaceType != GBL_TYPE_INVALID);
    GBL_API_VERIFY_POINTER(pInstance);
    {
        GblClass* pClass = GblInstance_classOf(pInstance);
        GBL_API_VERIFY_EXPRESSION(pClass);
        GblInterfaceClass* pIClass = gblTypeInterfaceClassPeek(pClass, ifaceType);
        if(pIClass) {
            GBL_API_VERIFY_EXPRESSION(pIClass->base.typeId == ifaceType);
            pSelf->pVTable      = pIClass;
            pSelf->pInstance    = pInstance;
            pIface              = pSelf;
        }
    }
    GBL_API_END_BLOCK();
    return pIface;
}

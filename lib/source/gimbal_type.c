#include <gimbal/meta/gimbal_type.h>
#include <gimbal/meta/gimbal_class.h>
#include <gimbal/meta/gimbal_instance.h>
#include <gimbal/containers/gimbal_hashmap.h>
#include <stdatomic.h>
#include <threads.h>

#define GBL_TYPE_REGISTRY_HASH_MAP_CAPACITY_DEFAULT 32
#define GBL_META_TYPE_NAME_SIZE_MAX                 100

typedef atomic_uint_fast16_t            GblRefCounter;

// === STATIC DATA ====

typedef struct GblMetaType_ {
    char                    name[100];
    GblRefCounter           refCount;
#ifdef GBL_TYPE_DEBUG
    GblRefCounter           instanceRefCount;
#endif
    GblTypeInfo             info;
    GblFlags                flags;
    struct GblMetaType_*    pParent;
    void*                   pModule; //module
    GblClass*               pClass;
} GblMetaType_;

static  once_flag   initOnce_ = ONCE_FLAG_INIT;
static  mtx_t       typeRegMtx_;
static  GblHashMap  typeRegistry_;

static uint64_t metaTypeHasher_(const void* pItem, uint64_t seed0, uint64_t seed1) {
    const GblMetaType_* pMeta = pItem;
    uint64_t            hash = 0;
    GBL_API_BEGIN(NULL);
    GBL_API_CALL(gblHashMapMurmur(pMeta->name, GBL_META_TYPE_NAME_SIZE_MAX, seed0, seed1, &hash));
    GBL_API_END_BLOCK();
    return hash;
}

static int metaTypeComparator_(const void* pA, const void* pB, void* pUdata) {
    GBL_UNUSED(pUdata);
    return strncmp(pA, pB, GBL_TYPE_REGISTRY_HASH_MAP_CAPACITY_DEFAULT);
}

static void metaTypeDtor_(void* pItem) {
    const GblMetaType_* pMeta = pItem;
    GBL_API_BEGIN(NULL);
    GBL_API_FREE(pMeta->pClass);
    GBL_API_END_BLOCK();
}

static void gblTypeInit_(void) {
    GBL_API_BEGIN(NULL);
    mtx_init(&typeRegMtx_, mtx_plain);
    GBL_API_PUSH_VERBOSE("[GblType]: Initializing.");
    mtx_lock(&typeRegMtx_);
    GBL_API_CALL(gblHashMapInit(&typeRegistry_,
                                GBL_API_CONTEXT(),
                                sizeof(GblMetaType_),
                                GBL_TYPE_REGISTRY_HASH_MAP_CAPACITY_DEFAULT,
                                metaTypeHasher_,
                                metaTypeComparator_,
                                metaTypeDtor_));
    mtx_unlock(&typeRegMtx_);
    GBL_API_POP(1);
    GBL_API_END_BLOCK();
}

// === PUBLIC API ====


GBL_EXPORT GblType gblTypeFind(const char* pTypeName) {
    GblType foundType = GBL_TYPE_INVALID;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pTypeName);
    call_once(&initOnce_, gblTypeInit_);

    if(strnlen(pTypeName, GBL_META_TYPE_NAME_SIZE_MAX+1) > GBL_META_TYPE_NAME_SIZE_MAX)
        GBL_API_WARN("Typename is too large and will be truncated: %s", pTypeName);

    mtx_lock(&typeRegMtx_);
    GBL_API_CALL(gblHashMapGet(&typeRegistry_, pTypeName, (void**)&foundType));
    mtx_unlock(&typeRegMtx_);

    GBL_API_END_BLOCK();
    return foundType;
}

GBL_EXPORT GblSize gblTypeCount(void) {
    GblSize count = 0;
    GBL_API_BEGIN(NULL);
    call_once(&initOnce_, gblTypeInit_);
    mtx_lock(&typeRegMtx_);
    GBL_API_CALL(gblHashMapCount(&typeRegistry_, &count));
    mtx_unlock(&typeRegMtx_);
    GBL_API_END_BLOCK();
    return count;
}

GBL_EXPORT const char* gblTypeName(GblType type) {
    const char*     pName   = NULL;
    GblMetaType_*   pMeta   = (GblMetaType_*)type;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_ARG(type != GBL_TYPE_INVALID);
    pName = pMeta->name;
    GBL_API_END_BLOCK();
    return pName;
}

GBL_EXPORT GblType gblTypeParent(GblType type) {
    GblType parent      = GBL_TYPE_INVALID;
    GblMetaType_* pMeta = (GblMetaType_*)type;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_ARG(pMeta != NULL);
    parent = (GblType)pMeta->pParent;
    GBL_API_END_BLOCK();
    return parent;
}

GBL_EXPORT GblType gblTypeBase(GblType type) {
    GblType base      = GBL_TYPE_INVALID;
    GblMetaType_* pMeta = (GblMetaType_*)type;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_ARG(pMeta != NULL);
    while(pMeta->pParent) pMeta = pMeta->pParent;
    base = (GblType)pMeta;
    GBL_API_END_BLOCK();
    return base;
}

GBL_EXPORT GblTypeFlags gblTypeFlags(GblType type) {
    GblTypeFlags flags      = 0;
    GblMetaType_* pMeta = (GblMetaType_*)type;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_ARG(pMeta != NULL);
    flags = pMeta->flags;
    GBL_API_END_BLOCK();
    return flags;
}

GBL_EXPORT const GblTypeInfo*   gblTypeInfo(GblType type) {
    const GblTypeInfo* pInfo      = 0;
    GblMetaType_* pMeta = (GblMetaType_*)type;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_ARG(pMeta != NULL);
    pInfo = &pMeta->info;
    GBL_API_END_BLOCK();
    return pInfo;
}

GBL_EXPORT GblRefCount gblTypeClassRefCount(GblType type) {
    GblRefCount refCount      = 0;
    GblMetaType_* pMeta = (GblMetaType_*)type;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_ARG(pMeta != NULL);
    refCount = atomic_load(&pMeta->refCount);
    GBL_API_END_BLOCK();
    return refCount;
}

#ifdef GBL_TYPE_DEBUG
GBL_EXPORT GblRefCount gblInstanceRefCount(GblType type) {
    GblRefCount refCount      = 0;
    GblMetaType_* pMeta = (GblMetaType_*)type;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_ARG(pMeta != NULL);
    refCount = atomic_load(&pMeta->instanceRefCount);
    GBL_API_END_BLOCK();
    return refCount;
}
#endif

GBL_EXPORT GblBool gblTypeIsA(GblType derived, GblType base) {
    GblBool result      = 0;
    GblMetaType_* pDerived = (GblMetaType_*)derived;
    GblMetaType_* pBase    = (GblMetaType_*)base;
    GblMetaType_* pIter     = pDerived;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_ARG(derived != GBL_TYPE_INVALID);
    GBL_API_VERIFY_ARG(base != GBL_TYPE_INVALID);
    // Walk the type hierarchy
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
GBL_EXPORT GblType gblTypeRegister(GblType              parent,
                                   const char*          pName,
                                   const GblTypeInfo*   pInfo,
                                   GblFlags             flags)
{
    GblType newType = GBL_TYPE_INVALID;
    GblMetaType_* pParent = (GblMetaType_*)parent;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pName);
    GBL_API_VERIFY_POINTER(pInfo);
#if 0
    GBL_API_VERIFY(parent != GBL_TYPE_INVALID,
                   GBL_RESULT_ERROR_TYPE_MISMATCH,
                   "[GblType] Register: Invalid parent type!");
#endif
    GBL_API_PUSH_VERBOSE("[GblType] Register: Type %s derived from %s", pName, pParent->name);

    GBL_API_VERIFY(gblTypeFind(pName) == GBL_TYPE_INVALID,
                   GBL_RESULT_ERROR_INVALID_ARG,
                   "Existing entry for the given type name was found!");

    GBL_API_VERIFY_EXPRESSION(pInfo->classSize >= pParent->info.classSize,
                              "Class size [%u] must be >= to parent's class size [%u]!",
                              pInfo->classSize, pParent->info.classSize);

    GBL_API_VERIFY_EXPRESSION(pInfo->classAlign >= pParent->info.classAlign,
                              "Class alignment [%u] must be >= to parent's class alignment [%u]!",
                              pInfo->classAlign, pParent->info.classAlign);

    GBL_API_VERIFY_EXPRESSION(pInfo->instanceSize >= pParent->info.instanceSize,
                              "Instance size [%u]must be >= to parent's instance size [%u]!",
                              pInfo->instanceSize, pParent->info.instanceSize);

    GBL_API_VERIFY_EXPRESSION(pInfo->instanceAlign >= pParent->info.instanceAlign,
                              "Instance alignment [%u] must be >= to parent's instance alignment [%u]!",
                              pInfo->instanceAlign, pParent->info.instanceAlign);
    call_once(&initOnce_, gblTypeInit_);
    {
        GblMetaType_ metaType;
        memset(&metaType, 0, sizeof(GblMetaType_));
        memcpy(&metaType.info, pInfo, sizeof(GblTypeInfo));
        atomic_init(&metaType.refCount, 0);
#ifdef GBL_TYPE_DEBUG
        atomic_init(&metaType.instanceRefCount, 0);
#endif
        strncpy(metaType.name, pName, GBL_META_TYPE_NAME_SIZE_MAX);
        metaType.flags = flags;
        metaType.pParent = (struct GblMetaType_*)parent;

        if(strnlen(pName, GBL_META_TYPE_NAME_SIZE_MAX+1) > GBL_META_TYPE_NAME_SIZE_MAX) {
            GBL_API_WARN("Typename is too large and has been truncated: %s",
                         metaType.name);
        }

        void* pOldData = NULL;

        mtx_lock(&typeRegMtx_);
        GBL_API_CALL(gblHashMapSet(&typeRegistry_,
                                   &metaType,
                                   &pOldData));

        GBL_API_VERIFY(!pOldData, GBL_RESULT_ERROR_INVALID_ARG,
                       "A previous metatype entry named %s existed already!", pName);

        GBL_API_VERIFY_HASH_MAP(&typeRegistry_);

        GBL_API_CALL(gblHashMapGet(&typeRegistry_, pName, (void**)&newType));
        mtx_unlock(&typeRegMtx_);
    }
    GBL_API_POP(1);
    GBL_API_END_BLOCK();
    return newType;
}

GBL_EXPORT void gblTypeUnregister(GblType type) {
    GblMetaType_* pMeta = (GblMetaType_*)type;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_ARG(type != GBL_TYPE_INVALID);
    GBL_API_PUSH_VERBOSE("[GblType] Unregister: %s", pMeta->name);
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

        GblMetaType_* pRet  = NULL;
        mtx_lock(&typeRegMtx_);
        GBL_API_CALL(gblHashMapDelete(&typeRegistry_, pMeta, (void**)&pRet));
        mtx_unlock(&typeRegMtx_);
        GBL_API_VERIFY(pRet, GBL_RESULT_ERROR_INVALID_ARG,
                       "Meta Type entry for the specified type was not found! [%s]",
                       pRet->name);
        GBL_API_VERIFY(pRet == pMeta, GBL_RESULT_ERROR_INTERNAL,
                       "Wtf, the two types don't even match! [%s vs %s]",
                       pMeta->name, pRet->name);

    }
    GBL_API_POP(1);
    GBL_API_END_BLOCK();
}


GBL_EXPORT GblClass* gblTypeClassRef(GblType typeId) {
    GblClass* pClass    = NULL;
    GblMetaType_* pMeta = (GblMetaType_*)typeId;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_ARG(typeId != GBL_TYPE_INVALID);
    GBL_API_VERIFY(pMeta->info.classSize != 0,
                   GBL_RESULT_UNIMPLEMENTED,
                   "[GblType] Attempt to reference a class of size 0!");

    GBL_API_PUSH_VERBOSE("[GblType] Referencing class for type: %s", pMeta->name);
    call_once(&initOnce_, gblTypeInit_);

    if(pMeta->pClass) {
        GblRefCount oldCount = atomic_fetch_add(&pMeta->refCount, 1);
        GBL_API_DEBUG("Incrementing RefCount: "GBL_SIZE_FMT, oldCount+1);
        pClass = pMeta->pClass;
    } else {
        GBL_API_PUSH_VERBOSE("Allocating new instance: ["GBL_SIZE_FMT" bytes]", pMeta->info.classSize);

        // Allocate the class
        pClass = GBL_API_MALLOC(pMeta->info.classSize, pMeta->info.classAlign, pMeta->name);
        memset(pClass, 0, pMeta->info.classSize);

        // Attach class to MetaType
        pMeta->pClass = pClass;

        //IMMEDIATELY initialize its type!!!
        pClass->typeId = typeId;

        //ensure construction of parent class (this will recurse)
        if(pMeta->pParent) gblTypeClassRef((GblType)pMeta->pParent);

        // Create a structure to hold all base types
        const GblSize vecStackSize = sizeof(GblVector) + 20*sizeof(GblMetaType_*);
        GblVector* pBases = GBL_ALLOCA(vecStackSize);
        GBL_API_CALL(gblVectorConstruct(pBases, GBL_API_CONTEXT(), sizeof(GblMetaType_*), vecStackSize));

        GBL_API_DEBUG("Populating constructor chain.");

        // Populate structure by traversing hierarchy
        GblMetaType_* pIter = pMeta;
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
            if(pIter->info.pFnClassConstruct) {
                GBL_API_DEBUG("Calling class ctor: [%s]", pIter->name);
                GBL_API_CALL(pIter->info.pFnClassConstruct(pClass, pMeta->info.pClassData));

            } else {
                GBL_API_DEBUG("No class ctor: [%s]", pIter->name);
            }
        }

        GBL_API_POP(1);

        // Free the class heirarchy structure
        GBL_API_CALL(gblVectorDestruct(pBases));

        GBL_API_POP(1);
    }

    GBL_API_POP(1);
    GBL_API_END_BLOCK();
    return pClass;
}

GBL_EXPORT GblClass* gblTypeClassPeek(GblType typeId) {
    GblClass* pClass    = NULL;
    GblMetaType_* pMeta = (GblMetaType_*)typeId;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_ARG(typeId != GBL_TYPE_INVALID);
    pClass = pMeta->pClass;
    GBL_API_END_BLOCK();
    return pClass;
}

GBL_EXPORT GblRefCount gblTypeClassUnref(GblClass* pClass) {
    GblRefCount refCount    = 0;
    GblMetaType_* pMeta     = NULL;
    GBL_API_BEGIN(NULL);
    if(!pClass) GBL_API_DONE(); //valid to Unref NULL pointer

    GBL_API_PUSH_VERBOSE("[GblType] Unreferencing class for type: %s", pMeta->name);

    GBL_API_VERIFY(pClass->typeId != GBL_INDEX_INVALID,
                   GBL_RESULT_ERROR_INTERNAL,
                   "The specified class has an invalid ID!");

    pMeta = (GblMetaType_*)pClass->typeId;
    GBL_API_VERIFY(atomic_load(&pMeta->refCount) != 0,
                   GBL_RESULT_ERROR_INTERNAL,
                   "The refcount for the given class was already at 0!");

    refCount = atomic_fetch_sub(&pMeta->refCount, 1);

    if(refCount) {
        GBL_API_DEBUG("Decrementing refCount: %u", refCount-1);
    } else {
        GBL_API_DEBUG("Destroying %s class!", pMeta->name);
        GBL_API_PUSH();

#ifdef GBL_TYPE_DEBUG
        {
            GblRefCount instanceRefCount = 0;
            instanceRefCount = atomic_load(&pMeta->instanceRefCount);
        GBL_API_VERIFY_EXPRESSION(instanceRefCount > 0,
                                  "Cannot destroy class when there are still %u instances using it!", instanceRefCount)
        }
#endif

        GblMetaType_* pIter = pMeta;
        // walk up the destructor chain
        GBL_API_DEBUG("Walking class destructors.");
        do {
            if(pIter->info.pFnClassDestruct) {
                GBL_API_DEBUG("Calling class dtor: [%s]", pIter->name);
                GBL_API_CALL(pIter->info.pFnClassDestruct(pMeta->pClass, pMeta->info.pClassData));
            } else {
                GBL_API_DEBUG("No class dtor: [%s]", pIter->name);
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
    GblMetaType_* pMeta     = (GblMetaType_*)type;
    GblClass* pClass        = NULL;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_ARG(type != GBL_TYPE_INVALID, "[GblType] Instance Create: Invalid type!");
    GBL_API_PUSH_VERBOSE("[GblType] Instance Create: type %s", pMeta->name);
    GBL_API_VERIFY(pMeta->info.instanceSize,
                   GBL_RESULT_ERROR_INVALID_ARG,
                   "Cannot instantiate type with instance of size 0!");

    pClass = gblTypeClassRef(type);
    GBL_API_VERIFY(pClass, GBL_RESULT_ERROR_INTERNAL, "Failed to retrieve class reference!");

    GBL_API_DEBUG("Allocating %u bytes.", pMeta->info.instanceSize);

    pInstance = GBL_API_MALLOC(pMeta->info.instanceSize, pMeta->info.instanceAlign, pMeta->name);
    pInstance->pClass = pClass;

    // Create a structure to hold all base types
    const GblSize vecStackSize = sizeof(GblVector) + 20*sizeof(GblMetaType_*);
    GblVector* pBases = GBL_ALLOCA(vecStackSize);
    GBL_API_CALL(gblVectorConstruct(pBases, GBL_API_CONTEXT(), sizeof(GblMetaType_*), vecStackSize));

    GBL_API_DEBUG("Populating constructor chain.");

    // Populate structure by traversing hierarchy
    GblMetaType_* pIter = pMeta;
    GblClass* pClassIter = NULL;
    while(pIter) {
        pClassIter = pIter->pClass;
        GBL_API_VERIFY_EXPRESSION(pClassIter);
        GBL_API_CALL(gblVectorPushBack(pBases, pClassIter));
        pIter = pIter->pParent;
    }

    GBL_API_DEBUG("Calling constructor chain.");
    GBL_API_PUSH();

    // Iterate backwards over the structure calling ctors from base to derived order
    GblSize count = 0;
    GBL_API_CALL(gblVectorSize(pBases, &count));
    GBL_API_VERIFY(count, GBL_RESULT_ERROR_INTERNAL);
    for(GblSize idx = count-1; idx >= 0; --idx) {
        GBL_API_CALL(gblVectorAt(pBases, idx, (void**)&pClassIter));
        if(pClassIter->pFnConstruct) {
            GBL_API_DEBUG("Calling instance ctor: [%s]", GblClass_name(pClassIter));
            GBL_API_CALL(pClassIter->pFnConstruct(pInstance));
        } else {
            GBL_API_DEBUG("No instance ctor: [%s]", GblClass_name(pClassIter));
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
    GBL_API_BEGIN(NULL);
    if(!pInstance) GBL_API_DONE(); //valid to destroy NULL pointer
    else {
        GblClass* pClass = GblInstance_classOf(pInstance);
        GBL_API_VERIFY_EXPRESSION(pClass);
        GBL_API_PUSH_VERBOSE("[GblType] Instance Destroy: %s", GblClass_name(pClass));

        GblClass* pIter = pClass;
        // walk up the destructor chain
        GBL_API_DEBUG("Walking instance destructors.");
        do {
            if(pIter->pFnDestruct) {
                GBL_API_DEBUG("Calling instance dtor: [%s]", GblClass_name(pIter));
                GBL_API_CALL(pIter->pFnDestruct(pInstance));

            } else {
                GBL_API_DEBUG("No instance dtor: [%s]", GblClass_name(pIter));
            }
            pIter = GblClass_parentOf(pIter);
        } while(pIter);
        GBL_API_POP(1);

#ifdef GBL_TYPE_DEBUG
        GblMetaType_* pMeta = (GblMetaType_*)GblClass_type(pClass);
        GblRefCount refCount = atomic_fetch_sub(&pMeta->instanceRefCount, 1);
        GBL_API_DEBUG("Instance reference count: %u", count-1);
#endif

        GBL_API_DEBUG("Releasing class reference.");
        GBL_API_CALL(gblTypeClassUnref(pClass));

        GBL_API_FREE(pInstance);

        GBL_API_POP(1);
    }
    GBL_API_END_BLOCK();
    return refCount;
}

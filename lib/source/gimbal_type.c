#include <gimbal/meta/gimbal_type.h>
#include <gimbal/meta/gimbal_class.h>
#include <gimbal/meta/gimbal_instance.h>
#include <gimbal/meta/gimbal_interface.h>
#include <gimbal/containers/gimbal_hashset.h>
#include <gimbal/algorithms/gimbal_hash.h>
#include <gimbal/algorithms/gimbal_sort.h>
#include <gimbal/containers/gimbal_vector.h>
#include <gimbal/meta/gimbal_ivariant.h>
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
} GblMetaClass;



static  GblContext  pCtx_           = NULL;
static  once_flag   initOnce_       = ONCE_FLAG_INIT;
static  GblBool     initialized_    = GBL_FALSE;
static  mtx_t       typeRegMtx_;
static  GblHashSet  typeRegistry_;
static struct {
    GblVector       vector;
    uint8_t         stackBuffer[sizeof(GblType)*GBL_TYPE_BUILTIN_COUNT];
}                   typeBuiltins_;

static int metaClassIFaceEntryComparator_(const void* pA, const void* pB) {
    const GblTypeInterfaceMapEntry* pIFaceA = pA;
    const GblTypeInterfaceMapEntry* pIfaceB = pB;
    return pIFaceA->interfaceType - pIfaceB->interfaceType;
}

static GblHash metaClassHasher_(const GblHashSet* pMap, const void* pItem) {
    GBL_UNUSED(pMap);
    const GblMetaClass** pMeta = (const GblMetaClass**)pItem;
    return gblHashMurmur((*pMeta)->pName, strnlen((*pMeta)->pName, GBL_META_TYPE_NAME_SIZE_MAX));
}

static GblBool metaClassComparator_(const GblHashSet* pMap, const void* pA, const void* pB) {
    GBL_UNUSED(pMap);
    const GblMetaClass** ppRhs = (const GblMetaClass**)pA, **ppLhs = (const GblMetaClass**)pB;
    return strncmp((*ppRhs)->pName, (*ppLhs)->pName, GBL_META_TYPE_NAME_SIZE_MAX) == 0;
}

static void metaClassElementFree_(const GblHashSet* pMap, void* item) {
    GBL_UNUSED(pMap);
    GBL_API_BEGIN(pCtx_);
    GblMetaClass** ppMetaClass = (GblMetaClass**)item;
    GBL_API_FREE(*ppMetaClass);
    GBL_API_END_BLOCK();
}


static GblType typeRegister_(GblType parent,
                            const char* pName,
                            const GblTypeInfo* pInfo,
                            GblFlags flags)
{
    GblBool hasMutex = GBL_FALSE;
    GblType newType = GBL_TYPE_INVALID;
    GblMetaClass* pParent = (GblMetaClass*)parent;
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_POINTER(pName);
    GBL_API_VERIFY_POINTER(pInfo);

    GBL_API_PUSH_VERBOSE("[GblType] Register Type: %s", pName);

    GBL_API_PUSH_VERBOSE("Type Info");
    GBL_API_VERBOSE("%-20s: %-100.100s", "Parent Type", gblTypeName(parent));

    GBL_API_PUSH_VERBOSE("flags");
    GBL_API_VERBOSE("%-20s: %-100u", "CLASSED",         (flags & GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED));
    GBL_API_VERBOSE("%-20s: %-100u", "INSTANTIABLE",    (flags & GBL_TYPE_FUNDAMENTAL_FLAG_INSTANTIABLE));
    GBL_API_VERBOSE("%-20s: %-100u", "DERIVABLE",       (flags & GBL_TYPE_FUNDAMENTAL_FLAG_DERIVABLE));
    GBL_API_VERBOSE("%-20s: %-100u", "DEEP_DERIVABLE",  (flags & GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE));
    GBL_API_VERBOSE("%-20s: %-100u", "ABSTRACT",        (flags & GBL_TYPE_FLAG_ABSTRACT));
    GBL_API_VERBOSE("%-20s: %-100u", "FINAL",           (flags & GBL_TYPE_FLAG_FINAL));
    GBL_API_POP(1);

    GBL_API_PUSH_VERBOSE("Class Info");
    GBL_API_VERBOSE("%-20s: %-100u", "size",            pInfo->classSize);
    GBL_API_VERBOSE("%-20s: %-100u", "alignment",       pInfo->classAlign);
    GBL_API_VERBOSE("%-20s: %-100p", "data",            pInfo->pClassData);
    GBL_API_VERBOSE("%-20s: %-100p", "initializer",     pInfo->pFnClassInit);
    GBL_API_VERBOSE("%-20s: %-100p", "finalizer",       pInfo->pFnClassFinalize);
    GBL_API_POP(1);

    if(pInfo->interfaceCount) {
        GBL_API_PUSH_VERBOSE("Interface Map");
        for(GblSize i = 0; i < pInfo->interfaceCount; ++i) {
            GBL_API_VERBOSE("%u: [Type: %s, Class Offset: %u]",
                            i,
                            gblTypeName(pInfo->pInterfaceMap[i].interfaceType),
                            pInfo->pInterfaceMap[i].classOffset);
        }
        GBL_API_POP(1);
    }

    GBL_API_PUSH_VERBOSE("Instance Info");
    GBL_API_VERBOSE("%-20s: %-100u", "size",            pInfo->instanceSize);
    GBL_API_VERBOSE("%-20s: %-100u", "alignment",       pInfo->instanceAlign);
    GBL_API_VERBOSE("%-20s: %-100p", "initializer",     pInfo->pFnInstanceInit);

    GBL_API_POP(2);

    if(pParent) { // not a fundamental type

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

        GBL_API_VERIFY_ARG(!(flags & GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED),
                           "Invalid use of CLASSED type flag on non-fundamental type!");

        GBL_API_VERIFY_ARG(!(flags & GBL_TYPE_FUNDAMENTAL_FLAG_INSTANTIABLE),
                           "Invalid use of INSTANTIABLE type flag on non-fundamental type!");

        GBL_API_VERIFY_ARG(!(flags & GBL_TYPE_FUNDAMENTAL_FLAG_DERIVABLE),
                           "Invalid use of DERIVABLE type flag on non-fundamental type!");

        GBL_API_VERIFY_ARG(!(flags & GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE),
                           "Invalid use of DEEP_DERIVABLE type flag on non-fundamental type!");

    } else {    //assumed to be a fundamental type
        //verify args here

    }

    {
        GblSize metaSize = sizeof(GblMetaClass);
        const GblSize ifaceOffset = metaSize;
        const GblSize nameOffset = metaSize += sizeof(GblTypeInterfaceMapEntry) * pInfo->interfaceCount;
        const GblSize classOffset = metaSize += strlen(pName)+1;
        metaSize += pInfo->classSize;

        GblMetaClass* pMeta = GBL_API_MALLOC(metaSize);

        memset(pMeta, 0, metaSize);
        memcpy(&pMeta->info, pInfo, sizeof(GblTypeInfo));
        atomic_init(&pMeta->refCount, 0);
#ifdef GBL_TYPE_DEBUG
        atomic_init(&pMeta->instanceRefCount, 0);
#endif
        strcpy(((char*)pMeta + nameOffset), pName);
        pMeta->flags                = flags;
        pMeta->pParent              = (struct GblMetaClass*)parent;
        pMeta->pName                = ((const char*)pMeta + nameOffset);
        pMeta->pClass               = pInfo->classSize?
                                            (GblClass*)((char*)pMeta + classOffset) : NULL;
        pMeta->info.pInterfaceMap   = pInfo->interfaceCount?
                                            (GblTypeInterfaceMapEntry*)((char*)pMeta + ifaceOffset) : NULL;
        memcpy((GblTypeInterfaceMapEntry*)pMeta->info.pInterfaceMap, pInfo->pInterfaceMap, sizeof(GblTypeInterfaceMapEntry)*pInfo->interfaceCount);

        gblSortQuick((GblTypeInterfaceMapEntry*)pMeta->info.pInterfaceMap, pMeta->info.interfaceCount, sizeof(GblTypeInterfaceMapEntry), metaClassIFaceEntryComparator_);

        mtx_lock(&typeRegMtx_);
        hasMutex = GBL_TRUE;

        const GblMetaClass* pOldData = GblHashSet_set(&typeRegistry_,
                                                      &pMeta);

        newType = (GblType)pMeta;

        GBL_API_VERIFY(!pOldData, GBL_RESULT_ERROR_INVALID_ARG,
                       "A previous metatype entry named %s existed already!", pName);

    }
    GBL_API_POP(1);
    GBL_API_END_BLOCK();
    if(hasMutex) mtx_unlock(&typeRegMtx_);
    return newType;

}

static GBL_RESULT registerBuiltinType_(GblSize                          expectedIndex,
                                       GblType                          parentType,
                                       const char*                      pName,
                                       const GblTypeInfo*               pTypeInfo,
                                       GblTypeFlags                     flags)
{
    GblType type = GBL_TYPE_INVALID;
    GblSize size = 0;
    GBL_API_BEGIN(pCtx_);
    GBL_API_PUSH_VERBOSE("[GblType] Registering Builtin Type: %s", pName);
    type = typeRegister_(parentType, pName, pTypeInfo, flags);
    GBL_API_CALL(gblVectorPushBack(&typeBuiltins_.vector, &type));
    GBL_API_CALL(gblVectorSize(&typeBuiltins_.vector, &size));
    GBL_API_VERIFY_EXPRESSION(size == expectedIndex+1,
                              "Failed to obtain expected index! "
                              "[expected: %u, actual: %u]", expectedIndex, size-1);
    GBL_API_POP(1);
    GBL_API_END();
}

typedef struct GblPrimitiveClass {
    GblClass        base;
    GblIVariant     iVariant;
} GblPrimitiveClass;

GBL_RESULT GblPrimitiveClass_init(GblPrimitiveClass* pClass, GblIVariant* pIVariant) {
    GBL_API_BEGIN(pCtx_);
    memcpy(&pClass->iVariant.flags,
           &pIVariant->flags,
           sizeof(GblIVariant) - offsetof(GblIVariant, flags));
    GBL_API_END();
}

static GBL_RESULT registerBuiltinTypes_(void) {
    GBL_API_BEGIN(pCtx_);
    GBL_API_PUSH_VERBOSE("[GblType] Registering Builtin Types");
    GBL_API_CALL(registerBuiltinType_(0,
                                      GBL_TYPE_INVALID,
                                      "Interface",
                                      &((const GblTypeInfo) {
                                          .classSize    = sizeof(GblInterface),
                                          .classAlign   = GBL_ALIGNOF(GblInterface),
                                      }),
                                      GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED | GBL_TYPE_FUNDAMENTAL_FLAG_DERIVABLE));
    GBL_API_CALL(registerBuiltinType_(1,
                                      GBL_TYPE_INTERFACE,
                                      "IVariant",
                                      &((const GblTypeInfo) {
                                          .classSize    = sizeof(GblIVariant),
                                          .classAlign   = GBL_ALIGNOF(GblIVariant),
                                      }),
                                      0));
    GBL_API_CALL(registerBuiltinType_(2,
                                      GBL_TYPE_INVALID,
                                      "nil",
                                      &((const GblTypeInfo) {
                                          .pFnClassInit = (GblTypeClassInitFn)GblPrimitiveClass_init,
                                          .classSize    = sizeof(GblPrimitiveClass),
                                          .classAlign   = GBL_ALIGNOF(GblPrimitiveClass),
                                          .pClassData   = &((const GblIVariant) {

                                          })
                                      }),
                                      0));
    GBL_API_POP(1);
    GBL_API_END();
}

static void gblTypeInit_(void) {
    GBL_API_BEGIN(pCtx_);

    GBL_API_PUSH_VERBOSE("[GblType]: Initializing.");
    GBL_API_VERIFY(!initialized_, GBL_RESULT_ERROR_INVALID_OPERATION, "Already initialized!");
    mtx_init(&typeRegMtx_, mtx_plain | mtx_recursive);
    mtx_lock(&typeRegMtx_);
    GBL_API_CALL(GblHashSet_construct(&typeRegistry_,
                                      sizeof(GblMetaClass*),
                                      metaClassHasher_,
                                      metaClassComparator_,
                                      metaClassElementFree_,
                                      GBL_TYPE_REGISTRY_HASH_MAP_CAPACITY_DEFAULT,
                                      pCtx_));
    GBL_API_CALL(gblVectorConstruct(&typeBuiltins_.vector,
                                    pCtx_,
                                    sizeof(GblMetaClass*),
                                    sizeof(typeBuiltins_)));
    GBL_API_CALL(registerBuiltinTypes_());
    GBL_API_POP(1);
    initialized_ = GBL_TRUE;
    GBL_API_END_BLOCK();
    mtx_unlock(&typeRegMtx_);
}

static GBL_RESULT gblTypeFinalize_(void) {
    GBL_API_BEGIN(pCtx_);
    GBL_API_PUSH_VERBOSE("[GblType]: Finalizing");
    mtx_lock(&typeRegMtx_);

    GblHashSet_destruct(&typeRegistry_);
    GBL_API_CALL(gblVectorDestruct(&typeBuiltins_.vector));

    GBL_API_POP(1);
    initialized_ = GBL_FALSE;
    GBL_API_END_BLOCK();
    mtx_unlock(&typeRegMtx_);
    return GBL_API_RESULT();
}

// === PUBLIC API ====

GBL_API gblTypeReinit(GblContext hCtx) {
    GBL_API_BEGIN(hCtx);
    if(!initialized_) {
        GBL_API_PUSH_VERBOSE("[GblType]: Reinititalizing.");
        pCtx_ = hCtx;
        call_once(&initOnce_, gblTypeInit_);
    } else {
        GBL_API_BEGIN(hCtx);
        GBL_API_CALL(gblTypeFinalize_());
        pCtx_ = hCtx;
        gblTypeInit_();
    }
    GBL_API_END();
}

GBL_EXPORT GblType gblTypeBuiltin(GblUint index) {
    GblType type = GBL_TYPE_INVALID;
    GBL_API_BEGIN(pCtx_);
    {
        GblType* pType = NULL;
        GBL_API_CALL(gblVectorAt(&typeBuiltins_.vector, index, (void**)&pType));
        type = *pType;
    }
    GBL_API_END_BLOCK();
    return type;
}

GBL_EXPORT GblType gblTypeFind(const char* pTypeName) {
    GblType foundType = GBL_TYPE_INVALID;
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_POINTER(pTypeName);
    call_once(&initOnce_, gblTypeInit_);

    GblMetaClass* pTempClass = GBL_ALLOCA(sizeof(GblMetaClass));
    pTempClass->pName = pTypeName;

    if(strnlen(pTypeName, GBL_META_TYPE_NAME_SIZE_MAX+1) > GBL_META_TYPE_NAME_SIZE_MAX)
        GBL_API_WARN("Typename is too large and will be truncated: %s", pTypeName);

    mtx_lock(&typeRegMtx_);
    GblMetaClass** ppReturnValue = (GblMetaClass**)GblHashSet_get(&typeRegistry_, &pTempClass);
    if(ppReturnValue) {
        foundType = (GblType)*ppReturnValue;
    }
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

GBL_EXPORT GblBool gblTypeValid(GblType type) {
    return type != GBL_TYPE_INVALID;
}

GBL_EXPORT const char* gblTypeName(GblType type) {
    const char*     pName   = "Invalid";
    GblMetaClass*   pMeta   = (GblMetaClass*)type;
    GBL_API_BEGIN(pCtx_);
    if(pMeta) {
        pName = pMeta->pName;
    }
    GBL_API_END_BLOCK();
    return pName;
}

GBL_EXPORT GblType gblTypeParent(GblType type) {
    GblType parent      = GBL_TYPE_INVALID;
    GblMetaClass* pMeta = (GblMetaClass*)type;
    GBL_API_BEGIN(pCtx_);
    if(pMeta) {
        parent = (GblType)pMeta->pParent;
    }
    GBL_API_END_BLOCK();
    return parent;
}

GBL_EXPORT GblType gblTypeFundamental(GblType type) {
    GblType base      = GBL_TYPE_INVALID;
    GblMetaClass* pMeta = (GblMetaClass*)type;
    GBL_API_BEGIN(pCtx_);
    if(pMeta) {
        while(pMeta->pParent) pMeta = pMeta->pParent;
        base = (GblType)pMeta;
    }
    GBL_API_END_BLOCK();
    return base;
}

GBL_EXPORT GblTypeFlags gblTypeFlags(GblType type) {
    GblTypeFlags flags      = 0;
    GblMetaClass* pMeta = (GblMetaClass*)type;
    GBL_API_BEGIN(pCtx_);
    if(pMeta) flags = pMeta->flags;
    GBL_API_END_BLOCK();
    return flags;
}

GBL_EXPORT const GblTypeInfo*   gblTypeInfo(GblType type) {
    const GblTypeInfo* pInfo      = NULL;
    GblMetaClass* pMeta = (GblMetaClass*)type;
    GBL_API_BEGIN(pCtx_);
    if(pMeta) pInfo = &pMeta->info;
    GBL_API_END_BLOCK();
    return pInfo;
}

GBL_EXPORT GblRefCount gblTypeClassRefCount(GblType type) {
    GblRefCount refCount      = 0;
    GblMetaClass* pMeta = (GblMetaClass*)type;
    GBL_API_BEGIN(pCtx_);
    if(pMeta) refCount = atomic_load(&pMeta->refCount);
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


static GblBool typeIsA_(GblType derived, GblType base, GblBool ifaceChecks) {
    GblBool result      = 0;
    GblMetaClass* pDerived = (GblMetaClass*)derived;
    GblMetaClass* pBase    = (GblMetaClass*)base;
    GblMetaClass* pIter     = pDerived;
    GBL_API_BEGIN(pCtx_);
    if(derived == GBL_TYPE_INVALID && base == GBL_TYPE_INVALID) {
        result = 1;
    } else {

        while(pIter) {
            // check if current class level is base
            if(pIter == pBase) {
                result = GBL_TRUE;
                break;
            } else if(ifaceChecks) {
                // recurse over interfaces checking
                for(GblSize i = 0; i < pIter->info.interfaceCount; ++i) {
                    if(typeIsA_(pIter->info.pInterfaceMap[i].interfaceType, base, ifaceChecks)) {
                        result = GBL_TRUE;
                        GBL_API_DONE();
                    }
                }
            }
            pIter = pIter->pParent;
        }
    }
    GBL_API_END_BLOCK();
    return result;
}

GBL_EXPORT GblBool gblTypeIsA(GblType derived, GblType base) {
    return typeIsA_(derived, base, GBL_TRUE);
}

GBL_EXPORT GblBool gblTypeIsFundamentallyA(GblType derived, GblType base) {
    return typeIsA_(derived, base, GBL_FALSE);
}


// ==== MAKE SURE TO ITERATE OVER PARENTS AND CHECK THAT CLASS/INSTANCE SIZE IS VALID =====
GBL_EXPORT GblType gblTypeRegisterStatic(GblType                     parent,
                                        const char*                  pName,
                                        const GblTypeInfo*           pInfo,
                                        GblFlags                     flags)
{
    GblType newType = GBL_TYPE_INVALID;
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_POINTER(pName);
    GBL_API_VERIFY_POINTER(pInfo);

    GBL_API_PUSH_VERBOSE("[GblType] Register Static Type: %s", pName);

    GBL_API_VERIFY_ARG(gblTypeFind(pName) == GBL_TYPE_INVALID,
                        "Existing entry for the given type name was found!");
    call_once(&initOnce_, gblTypeInit_);

    newType = typeRegister_(parent, pName, pInfo, flags);

    GBL_API_END_BLOCK();
    return newType;
}

GBL_EXPORT GBL_RESULT gblTypeUnregister(GblType type) {
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
        const GblBool success = GblHashSet_erase(&typeRegistry_, &pMeta);
        mtx_unlock(&typeRegMtx_);
        GBL_API_VERIFY(success, GBL_RESULT_ERROR_INVALID_TYPE, "Failed to remove the type from the registry HashSet!");
    }
    GBL_API_POP(1);
    GBL_API_END();
}

static GBL_RESULT gblTypeClassConstruct_(GblClass* pClass, GblMetaClass* pMeta);

static GBL_RESULT gblTypeInterfaceClassConstruct_(GblInterface* pClass, GblMetaClass* pMeta, int16_t offset) {
    GBL_API_BEGIN(pCtx_);
    GBL_API_PUSH_VERBOSE("[GblType] InterfaceClass::construct(%s)", pMeta->pName);
    GBL_API_VERBOSE("offset: %d", offset);
    pClass->offsetToTop = offset;
    void* pInterfaceClass = gblTypeClassRef((GblType)pMeta);
    GBL_API_VERIFY_EXPRESSION(pInterfaceClass);
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
        GBL_API_CALL(gblVectorPushBack(pBases, &pIter));
        pIter = pIter->pParent;
    }

    GBL_API_DEBUG("Calling initializer chain.");
    GBL_API_PUSH();

    // Iterate backwards over the structure calling ctors from base to derived order
    GblSize count = 0;
    GBL_API_CALL(gblVectorSize(pBases, &count));
    GBL_API_VERIFY(count, GBL_RESULT_ERROR_INTERNAL);
    for(int idx = count-1; idx >= 0; --idx) {
        GblMetaClass** ppIter = NULL;
        GBL_API_CALL(gblVectorAt(pBases, idx, (void**)&ppIter));
        pIter = *ppIter;
        GBL_API_PUSH_VERBOSE("[%u]: %s", count - idx, pIter->pName);

        GBL_API_PUSH_VERBOSE("Calling Interface Constructors");
        for(GblSize i = 0; i < pIter->info.interfaceCount; ++i) {
            const GblTypeInterfaceMapEntry* pIEntry = &pIter->info.pInterfaceMap[i];
            const GblMetaClass* pIMeta = (const GblMetaClass*)pIEntry->interfaceType;
            GBL_API_VERIFY_EXPRESSION(pIMeta);
            GBL_API_PUSH_VERBOSE("[%u]: %s", i, pIMeta->pName);
            GblInterface* pIClass = (GblInterface*)((char*)pClass + pIEntry->classOffset);
            GBL_API_CALL(gblTypeInterfaceClassConstruct_(pIClass, pIMeta, -pIEntry->classOffset));
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
    GBL_API_DEBUG("Incrementing RefCount: %"GBL_SIZE_FMT, oldCount+1);

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

    GBL_API_PUSH_VERBOSE("[GblType] Unreferencing class for type: %s", gblTypeName(pClass->typeId));

    GBL_API_VERIFY(pClass->typeId != GBL_TYPE_INVALID,
                   GBL_RESULT_ERROR_INTERNAL,
                   "The specified class has an invalid ID!");

    pMeta = (GblMetaClass*)pClass->typeId;
    GBL_API_VERIFY(atomic_load(&pMeta->refCount) != 0,
                   GBL_RESULT_ERROR_INTERNAL,
                   "The refcount for the given class was already at 0!");

    refCount = atomic_fetch_sub(&pMeta->refCount, 1);

    if(refCount-1) {
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
        GBL_API_PUSH_VERBOSE("Walking class destructors.");
        do {
            if(pIter->info.pFnClassFinalize) {
                GBL_API_DEBUG("Calling class dtor: [%s]", pIter->pName);
                GBL_API_CALL(pIter->info.pFnClassFinalize(pMeta->pClass, pMeta->info.pClassData));
            } else {
                GBL_API_DEBUG("No class dtor: [%s]", pIter->pName);
            }

            for(GblSize i = 0; i < pIter->info.interfaceCount; ++i) {
                gblTypeClassUnref((GblClass*)((const char*)pClass + pIter->info.pInterfaceMap[i].classOffset));
            }
            pIter = pIter->pParent;
        } while(pIter);
        GBL_API_POP(1);

        // Store parent pointer then deallocate
        GblClass* pParentClass = pMeta->pParent? pMeta->pParent->pClass : NULL;
       // GBL_API_FREE(pMeta->pClass);

        //Release reference to parent class;
        if(pParentClass) gblTypeClassUnref(pParentClass);

        // clear the type so it's not looking initialized
        pMeta->pClass->typeId = GBL_TYPE_INVALID;

        GBL_API_POP(1);
    }

    GBL_API_POP(1);
    GBL_API_END_BLOCK();
    return refCount;
}

GBL_EXPORT GBL_RESULT gblTypeInstanceConstruct(GblType type, GblInstance* pInstance) {
    GblMetaClass*   pMeta = (GblMetaClass*)type;
    GblClass*       pClass = NULL;
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_ARG(type != GBL_TYPE_INVALID, "[GblType] Instance Construct: Invalid type!");
    GBL_API_PUSH_VERBOSE("[GblType] Instance Construct: type %s", pMeta->pName);

    pClass = gblTypeClassRef(type);
    GBL_API_VERIFY_EXPRESSION(pClass, "Failed to retrieve class reference!");
    pInstance->pClass = pClass;

    // Create a structure to hold all base types
    const GblSize vecStackSize = sizeof(GblVector) + 20*sizeof(GblMetaClass*);
    GblVector* pBases = GBL_ALLOCA(vecStackSize);
    GBL_API_CALL(gblVectorConstruct(pBases, GBL_API_CONTEXT(), sizeof(GblMetaClass*), vecStackSize));

    GBL_API_DEBUG("Populating constructor chain.");

    // Populate structure by traversing hierarchy
    GblMetaClass* pIter = pMeta;
    while(pIter) {
        GBL_API_CALL(gblVectorPushBack(pBases, &pIter));
        pIter = pIter->pParent;
    }

    GBL_API_DEBUG("Calling constructor chain.");
    GBL_API_PUSH();

    // Iterate backwards over the structure calling ctors from base to derived order
    GblSize count = 0;
    GBL_API_CALL(gblVectorSize(pBases, &count));
    GBL_API_VERIFY(count, GBL_RESULT_ERROR_INTERNAL);
    for(int idx = count-1; idx >= 0; --idx) {
        GblMetaClass** ppIter = NULL;
        GBL_API_CALL(gblVectorAt(pBases, idx, (void**)&ppIter));
        pIter = *ppIter;
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

    GBL_API_END();
}

GBL_EXPORT GblInstance* gblTypeInstanceCreate(GblType type) {
    GblInstance* pInstance = NULL;
    GblMetaClass* pMeta     = (GblMetaClass*)type;
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_ARG(type != GBL_TYPE_INVALID, "[GblType] Instance Create: Invalid type!");
    GBL_API_PUSH_VERBOSE("[GblType] Instance Create: type %s", pMeta->pName);
    GBL_API_VERIFY(pMeta->info.instanceSize,
                   GBL_RESULT_ERROR_INVALID_ARG,
                   "Cannot instantiate type with instance of size 0!");

    GBL_API_DEBUG("Allocating %u bytes.", pMeta->info.instanceSize);

    pInstance = GBL_API_MALLOC(pMeta->info.instanceSize, pMeta->info.instanceAlign, pMeta->pName);

    GBL_API_CALL(gblTypeInstanceConstruct(type, pInstance));

    GBL_API_POP(1);
    GBL_API_END_BLOCK();
    return pInstance;
}

GBL_EXPORT GblRefCount gblTypeInstanceDestruct(GblInstance* pInstance) {
    GblRefCount refCount = 0;
    GBL_API_BEGIN(pCtx_);
    refCount = gblTypeClassUnref(GblInstance_classOf(pInstance));
    GBL_API_END_BLOCK();
    return refCount;
}

GBL_EXPORT GblRefCount gblTypeInstanceDestroy(GblInstance* pInstance) {
    GblRefCount refCount = 0;
    GBL_API_BLOCK(pCtx_, {
       if(!pInstance) GBL_API_DONE();
       else {
            GBL_API_PUSH_VERBOSE("[GblType] Instance Destroy: %s",
                                 GblClass_name(GblInstance_classOf(pInstance)));

            refCount = gblTypeInstanceDestruct(pInstance);
            GBL_API_FREE(pInstance);

            GBL_API_POP(1);

        }
    });
    return refCount;
}

GBL_EXPORT GblBool gblTypeClassIsA(GblClass* pClass, GblType type) {
    GblBool result = GBL_FALSE;
    GBL_API_BEGIN(pCtx_);
    if(!(!pClass && type == GBL_TYPE_INVALID)) {
        GBL_API_VERIFY_POINTER(pClass);
        result = gblTypeIsA(pClass->typeId, type);
    }
    GBL_API_END_BLOCK();
    return result;
}

static GblClass* typeClassCast_(GblClass* pClass, GblType toType, GblBool check) {
    GblClass* pClassStart = pClass;
    GblClass* pToClass = NULL;
    GBL_API_BEGIN(pCtx_);

    // Casting a NULL pointer or to an invalid type returns NULL
    if(pClass && toType != GBL_TYPE_INVALID) GBL_LIKELY {
        //early exit
        if(pClass->typeId == toType) GBL_LIKELY {
            pToClass = pClass;
        } else {

            // If current node is an interface class, attempt to "back out" to root class node
            while(gblTypeFundamental(pClass->typeId) == GBL_TYPE_INTERFACE) {
                // Move class pointer to interface's concrete class
                pClass = GblInterface_outerClass((GblInterface*)pClass);
                GBL_API_VERIFY_POINTER(pClass);
            }

            GblMetaClass* pMeta = (GblMetaClass*)pClass->typeId;
            GBL_API_VERIFY_POINTER(pMeta);

            // itatere from derived to base class, breadth-first searching
            do {
                //check current class
                if((GblType)pMeta == toType) {
                    pToClass = pClass;
                    break;
                // check current class's interfaces
                } else {
                    GblInterface* pInterface = gblTypeClassInterfacePeek(pClass, toType);
                    if(pInterface) {
                        pToClass = &pInterface->base;
                        break;
                    }
                }
                pMeta = pMeta->pParent;
            } while(pMeta);
        }

    }
    GBL_API_END_BLOCK();
    if(check && !pToClass) {
        if(toType == GBL_TYPE_INVALID) {
            GBL_API_WARN("Attempted to cast from type %s to TYPE_INVALID!",
                         GblClass_name(pClassStart));
        } else {
            GBL_API_WARN("Failed to cast from type %s to %s!",
                         GblClass_name(pClassStart), gblTypeName(toType));
        }
    }
    return pToClass;
}

GBL_EXPORT GblClass* gblTypeClassCast(GblClass* pClass, GblType toType) {
    return typeClassCast_(pClass, toType, GBL_FALSE);
}

GBL_EXPORT GblClass* gblTypeClassCastCheck(GblClass* pClass, GblType toType) {
    return typeClassCast_(pClass, toType, GBL_TRUE);
}

GBL_EXPORT GblType gblTypeClassType(GblClass* pClass) {
    GblType type = GBL_TYPE_INVALID;
    GBL_API_BEGIN(pCtx_);
    if(pClass) {
        type = pClass->typeId;
    }
    GBL_API_END_BLOCK();
    return type;
}


//should be able to make this a binary search!
GBL_EXPORT GblInterface* gblTypeClassInterfacePeek(GblClass* pClass, GblType ifaceType) {
    GblInterface* pIClass = NULL;
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_POINTER(pClass);
    GBL_API_VERIFY_EXPRESSION(pClass->typeId != GBL_TYPE_INVALID);
    GBL_API_VERIFY_ARG(ifaceType != GBL_TYPE_INVALID);
    {
        GblMetaClass* pMeta = (GblMetaClass*)pClass->typeId;
        while(pMeta) {
            for(unsigned i = 0; i < pMeta->info.interfaceCount; ++i) {
                GblInterface* pCurIClass = (GblInterface*)((char*)pClass + pMeta->info.pInterfaceMap[i].classOffset);
                GBL_API_VERIFY_EXPRESSION(pMeta->info.pInterfaceMap[i].interfaceType == pCurIClass->base.typeId);
                GBL_API_VERIFY_EXPRESSION(pCurIClass->base.typeId != GBL_TYPE_INVALID);
                if(pCurIClass->base.typeId == ifaceType) {
                    pIClass = pCurIClass;
                    break;
                } else {
                    pCurIClass = gblTypeClassInterfacePeek(&pCurIClass->base, ifaceType);
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

GBL_EXPORT GblInterface* gblTypeClassInterfacePeekParent(GblClass* pClass, GblType ifaceType) {
    GblInterface* pIClass = NULL;
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_POINTER(pClass);
    GBL_API_VERIFY_EXPRESSION(pClass->typeId != GBL_TYPE_INVALID);
    GBL_API_VERIFY_ARG(ifaceType != GBL_TYPE_INVALID);
    {
        GblType parentType = gblTypeParent(GblClass_typeOf(pClass));
        GBL_API_VERIFY_EXPRESSION(parentType != GBL_TYPE_INVALID);
        GblClass* pParentClass = gblTypeClassPeek(parentType);
        if(pParentClass) {
            pIClass = gblTypeClassInterfacePeek(pParentClass, ifaceType);
        }
    }
    GBL_API_END_BLOCK();
    return pIClass;
}


GBL_EXPORT GblBool gblTypeInstanceIsA(GblInstance* pInstance, GblType type) {
    GblBool result = GBL_FALSE;
    GBL_API_BEGIN(pCtx_);
    if(!(!pInstance && type == GBL_TYPE_INVALID)) {
        GBL_API_VERIFY_POINTER(pInstance);
        result = gblTypeClassIsA(pInstance->pClass, type);
    }
    GBL_API_END_BLOCK();
    return result;
}

GBL_EXPORT GblInstance* typeInstanceCast_(GblInstance* pInstance, GblType type, GblBool check) {
    GblInstance* pOutInstance = NULL;
    GBL_API_BEGIN(pCtx_);
    if(!(!pInstance && type == GBL_TYPE_INVALID)) {
        GBL_API_VERIFY_POINTER(pInstance);
        if(gblTypeInstanceIsA(pInstance, type)) {
            pOutInstance = pInstance;
        } else if(check) {
            GBL_API_WARN("Failed to cast instance from %s to %s.",
                         GblClass_name(GblInstance_classOf(pInstance)),
                         gblTypeName(type));
        }
    }
    GBL_API_END_BLOCK();
    return pOutInstance;
}


GBL_EXPORT GblInstance* gblTypeInstanceCast(GblInstance* pInstance, GblType type) {
    return typeInstanceCast_(pInstance, type, GBL_FALSE);
}

GBL_EXPORT GblInstance* gblTypeInstanceCastCheck(GblInstance* pInstance, GblType type) {
    return typeInstanceCast_(pInstance, type, GBL_TRUE);
}


static GblClass* typeInstanceClassCast_(GblInstance* pInstance, GblType type, GblBool check) {
    GblClass* pToClass = NULL;
    GBL_API_BEGIN(pCtx_);
    if(!(!pInstance && type == GBL_TYPE_INVALID)) {
        GBL_API_VERIFY_POINTER(pInstance);
        pToClass = typeClassCast_(pInstance->pClass, type, check);
    }
    GBL_API_END_BLOCK();
    return pToClass;
}

GBL_EXPORT GblClass* gblTypeInstanceClassCast(GblInstance* pInstance, GblType type) {
    return typeInstanceClassCast_(pInstance, type, GBL_FALSE);
}

GBL_EXPORT GblClass* gblTypeInstanceClassCastCheck(GblInstance* pInstance, GblType type) {
    return typeInstanceClassCast_(pInstance, type, GBL_TRUE);
}

GBL_EXPORT GblClass* gblTypeInstanceClass(GblInstance* pInstance) {
    GblClass* pClass = NULL;
    GBL_API_BEGIN(pCtx_);
    if(pInstance) {
        pClass = pInstance->pClass;
    }
    GBL_API_END_BLOCK();
    return pClass;
}

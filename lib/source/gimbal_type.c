#include <gimbal/meta/gimbal_type.h>
#include <gimbal/meta/gimbal_class.h>
#include <gimbal/meta/gimbal_instance.h>
#include <gimbal/meta/gimbal_interface.h>
#include <gimbal/containers/gimbal_hash_set.h>
#include <gimbal/algorithms/gimbal_hash.h>
#include <gimbal/algorithms/gimbal_sort.h>
#include <gimbal/containers/gimbal_vector.h>
#include <gimbal/meta/gimbal_ivariant.h>
#include <stdatomic.h>
#include <pthread.h>

#define GBL_TYPE_REGISTRY_HASH_MAP_CAPACITY_DEFAULT 32
#define GBL_META_TYPE_NAME_SIZE_MAX                 1024
#define GBL_TYPE_BASES_VECTOR_STACK_COUNT           20
#define GBL_TYPE_ENSURE_INITIALIZED_()  \
    if(!initializing_) pthread_once(&initOnce_, gblTypeInit_)
#define GBL_META_CLASS(type) ((GblMetaClass*)type)

typedef atomic_uint_fast16_t GblRefCounter;

extern GBL_RESULT GblIVariant_typeRegister_     (GblContext hCtx);
extern GBL_RESULT gblValueTypesRegister_        (GblContext hCtx);
extern GBL_RESULT GblITable_typeRegister_       (GblContext hCtx);
extern GBL_RESULT GblIEventHandler_typeRegister_(GblContext hCtx);
extern GBL_RESULT GblIEventFilter_typeRegister_ (GblContext hCtx);
extern GBL_RESULT GblObject_typeRegister_       (GblContext hCtx);

// === STATIC DATA ====

typedef struct GblMetaClass {
    struct GblMetaClass*    pParent;
    const char*             pName;
    GblRefCounter           refCount;
#ifdef GBL_TYPE_DEBUG
    GblRefCounter           instanceRefCount;
#endif
    GblTypeInfo             info;
    GblFlags                flags;
    GblIPlugin*             pPlugin;
    GblClass*               pClass;
    uint8_t                 depth;
    struct GblMetaClass**   pBases;
} GblMetaClass;

static GblContext               pCtx_           = NULL;
static pthread_once_t           initOnce_       = PTHREAD_ONCE_INIT;
static GblBool                  initialized_    = GBL_FALSE;
static GBL_THREAD_LOCAL GblBool initializing_   = GBL_FALSE;
static pthread_mutex_t          typeRegMtx_;
static GblHashSet               typeRegistry_;
static struct {
    GblVector   vector;
    uint8_t     stackBuffer[sizeof(GblType)*GBL_TYPE_BUILTIN_COUNT];
}               typeBuiltins_;

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

static GblClass* typeInterfaceOuterClass_(GblInterface* pInterface) {
    if(!pInterface)              return NULL;
    if(!pInterface->offsetToTop) return NULL;
    else
        return (GblClass*)((uint8_t*)pInterface + pInterface->offsetToTop);
}

GBL_EXPORT GblInterface* typeClassInterfacePeek_(GblClass* pClass, GblType ifaceType) {
    GblInterface* pIClass = NULL;
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_POINTER(pClass);
    GBL_API_VERIFY_EXPRESSION(pClass->type != GBL_TYPE_INVALID);
    GBL_API_VERIFY_ARG(ifaceType != GBL_TYPE_INVALID);
    GBL_API_VERIFY_ARG(GBL_TYPE_IS_INTERFACE(ifaceType));
    {
        GblMetaClass* pMeta = (GblMetaClass*)pClass->type;
        while(pMeta) {
            for(unsigned i = 0; i < pMeta->info.interfaceCount; ++i) {
                GblInterface* pCurIClass = (GblInterface*)((char*)pClass + pMeta->info.pInterfaceMap[i].classOffset);
                GBL_API_VERIFY_EXPRESSION(pMeta->info.pInterfaceMap[i].interfaceType == pCurIClass->base.type);
                GBL_API_VERIFY_EXPRESSION(pCurIClass->base.type != GBL_TYPE_INVALID);
                if(pCurIClass->base.type == ifaceType) {
                    pIClass = pCurIClass;
                    break;
                } else {
                    pCurIClass = typeClassInterfacePeek_(&pCurIClass->base, ifaceType);
                    if(pCurIClass) {
                        GBL_API_VERIFY_EXPRESSION(pCurIClass->base.type == ifaceType);
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
    GBL_API_VERBOSE("%-20s: %-100u", "CLASSED",         (flags & GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED)? 1 : 0);
    GBL_API_VERBOSE("%-20s: %-100u", "INSTANTIABLE",    (flags & GBL_TYPE_FUNDAMENTAL_FLAG_INSTANTIABLE)? 1 : 0);
    GBL_API_VERBOSE("%-20s: %-100u", "DERIVABLE",       (flags & GBL_TYPE_FUNDAMENTAL_FLAG_DERIVABLE)? 1 : 0);
    GBL_API_VERBOSE("%-20s: %-100u", "DEEP_DERIVABLE",  (flags & GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE)? 1 : 0);
    GBL_API_VERBOSE("%-20s: %-100u", "BUILTIN",         (flags & GBL_TYPE_FLAG_BUILTIN)? 1 : 0);
    GBL_API_VERBOSE("%-20s: %-100u", "ABSTRACT",        (flags & GBL_TYPE_FLAG_ABSTRACT)? 1 : 0);
    GBL_API_VERBOSE("%-20s: %-100u", "FINAL",           (flags & GBL_TYPE_FLAG_FINAL)? 1 : 0);
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

    GblFlags fundamentalFlags = 0;
    if(pParent && !(flags & GBL_TYPE_FUNDAMENTAL_FLAGS_MASK)) { // not a fundamental type
        GblMetaClass* pFundamental = (GblMetaClass*)gblTypeFundamental((GblType)pParent);
        fundamentalFlags = pFundamental? pFundamental->flags : 0;

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

        if(!(fundamentalFlags & GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE)) {
            GBL_API_VERIFY(fundamentalFlags & GBL_TYPE_FUNDAMENTAL_FLAG_DERIVABLE, GBL_RESULT_ERROR_INVALID_TYPE,
                           "Cannot derive from NON DERIVABLE type!");

            GBL_API_VERIFY(gblTypeFundamental((GblType)pParent), GBL_RESULT_ERROR_INVALID_TYPE,
                           "Cannot derive from DERIVABLE type more than once!");
        }


    } else {    //assumed to be a fundamental type
        GBL_API_VERBOSE("Fundamental Type!");

        if(flags & GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE) flags |= GBL_TYPE_FUNDAMENTAL_FLAG_DERIVABLE;
        if(flags & GBL_TYPE_FUNDAMENTAL_FLAG_INSTANTIABLE)   flags |= GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED;

        fundamentalFlags = flags;
    }

    if(fundamentalFlags & GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED) {
        GBL_API_VERIFY(pInfo->classSize > 0, GBL_RESULT_ERROR_INVALID_TYPE,
                       "Cannot register a CLASSED type with a class size of 0!");
    } else {
        GBL_API_VERIFY(pInfo->classSize == 0, GBL_RESULT_ERROR_INVALID_TYPE,
                       "Cannot register a NON CLASSED type with a class size > 0!");
    }

    if(fundamentalFlags & GBL_TYPE_FUNDAMENTAL_FLAG_INSTANTIABLE) {
        GBL_API_VERIFY(pInfo->instanceSize > 0, GBL_RESULT_ERROR_INVALID_TYPE,
                       "Cannot register an INSTANTIABLE type with an instance size of 0!");
        GBL_API_VERIFY(fundamentalFlags & GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED, GBL_RESULT_ERROR_INVALID_TYPE,
                       "Cannot register an INSTANTIABLE type without it being CLASSED!");
    } else {
        GBL_API_VERIFY(pInfo->instanceSize == 0, GBL_RESULT_ERROR_INVALID_TYPE,
                       "Cannot register a NON INSTANTIABLE type with an instance size > 0!");
    }

    GBL_API_VERIFY(!(pParent && (pParent->flags & GBL_TYPE_FLAG_FINAL)), GBL_RESULT_ERROR_INVALID_TYPE,
                   "Cannot derive from a FINAL type!");


    {
        uint8_t baseCount = 0;
        GblMetaClass* pParentIt = pParent;
        while(pParentIt) {
            ++baseCount;
            pParentIt = pParentIt->pParent;
        }

        GblSize metaSize = sizeof(GblMetaClass);
        const GblSize ifaceOffset = metaSize;
        const GblSize nameOffset = metaSize += sizeof(GblTypeInterfaceMapEntry) * pInfo->interfaceCount;
        const GblSize classOffset = metaSize += strlen(pName)+1;
        const GblSize basesOffset = metaSize += pInfo->classSize;
        metaSize += baseCount * sizeof(GblMetaType*);

        GblMetaClass* pMeta = GBL_API_MALLOC(metaSize);

        memset(pMeta, 0, metaSize);
        memcpy(&pMeta->info, pInfo, sizeof(GblTypeInfo));
        atomic_init(&pMeta->refCount, 0);
#ifdef GBL_TYPE_DEBUG
        atomic_init(&pMeta->instanceRefCount, 0);
#endif
        strcpy(((char*)pMeta + nameOffset), pName);
        pMeta->flags                = flags;
        pMeta->depth                = baseCount;
        pMeta->pParent              = (struct GblMetaClass*)parent;
        pMeta->pName                = ((const char*)pMeta + nameOffset);
        pMeta->pClass               = pInfo->classSize?
                                            (GblClass*)((char*)pMeta + classOffset) : NULL;
        pMeta->info.pInterfaceMap   = pInfo->interfaceCount?
                                            (GblTypeInterfaceMapEntry*)((char*)pMeta + ifaceOffset) : NULL;
        pMeta->pBases               = baseCount?
                                            (GblMetaClass**)((char*)pMeta + basesOffset) : NULL;

        memcpy((GblTypeInterfaceMapEntry*)pMeta->info.pInterfaceMap, pInfo->pInterfaceMap, sizeof(GblTypeInterfaceMapEntry)*pInfo->interfaceCount);
        //gblSortQuick((GblTypeInterfaceMapEntry*)pMeta->info.pInterfaceMap, pMeta->info.interfaceCount, sizeof(GblTypeInterfaceMapEntry), metaClassIFaceEntryComparator_);

        pParentIt = pParent;
        baseCount = 0;
        while(pParentIt) {
            pMeta->pBases[pMeta->depth-(++baseCount)] = pParentIt;
            pParentIt = pParentIt->pParent;
        }
        GBL_API_VERIFY_EXPRESSION(baseCount == pMeta->depth);

        pthread_mutex_lock(&typeRegMtx_);
        hasMutex = GBL_TRUE;

        const GblMetaClass* pOldData = GblHashSet_set(&typeRegistry_,
                                                      &pMeta);

        GBL_API_VERIFY(!pOldData, GBL_RESULT_ERROR_INVALID_ARG,
                       "A previous metatype entry named %s existed already!", pName);

        newType = (GblType)pMeta;
        if(flags & GBL_TYPE_FLAG_BUILTIN) {
             GBL_API_CALL(gblVectorPushBack(&typeBuiltins_.vector, &newType));
        }

    }
    GBL_API_POP(1);
    GBL_API_END_BLOCK();
    if(hasMutex) pthread_mutex_unlock(&typeRegMtx_);
    return newType;
}

static GBL_RESULT gblTypeRegisterBuiltins_(void) {
    GBL_API_BEGIN(pCtx_);
    gblTypeRegisterBuiltin(0,
              GBL_TYPE_INVALID,
              "Interface",
              &((const GblTypeInfo) {
                  .classSize    = sizeof(GblInterface),
                  .classAlign   = GBL_ALIGNOF(GblInterface),
              }),
              GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED         |
              GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE  |
              GBL_TYPE_FLAG_ABSTRACT);
    GBL_API_CALL(GblIVariant_typeRegister_(pCtx_));
    GBL_API_CALL(gblValueTypesRegister_(pCtx_));
    GBL_API_CALL(GblITable_typeRegister_(pCtx_));
    GBL_API_CALL(GblIEventHandler_typeRegister_(pCtx_));
    GBL_API_CALL(GblIEventFilter_typeRegister_(pCtx_));
    GBL_API_CALL(GblObject_typeRegister_(pCtx_));
    GBL_API_END();
}

static void gblTypeInit_(void) {
    GBL_API_BEGIN(pCtx_);

    GBL_API_PUSH_VERBOSE("[GblType]: Initializing.");
    GBL_API_VERIFY(!initialized_, GBL_RESULT_ERROR_INVALID_OPERATION, "Already initialized!");
    initializing_ = GBL_TRUE;
    pthread_mutexattr_t mutexAttr;
    pthread_mutexattr_settype(&mutexAttr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&typeRegMtx_, &mutexAttr);
    pthread_mutex_lock(&typeRegMtx_);
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
    GBL_API_CALL(gblTypeRegisterBuiltins_());
    GBL_API_POP(1);
    initialized_    = GBL_TRUE;
    initializing_   = GBL_FALSE;
    GBL_API_END_BLOCK();
    pthread_mutex_unlock(&typeRegMtx_);
}

GblType gblTypeRegisterBuiltin(GblSize            expectedIndex,
                               GblType            parentType,
                               const char*        pName,
                               const GblTypeInfo* pTypeInfo,
                               GblTypeFlags       flags)
{
    GblType type = GBL_TYPE_INVALID;
    GblSize size = 0;
    GBL_API_BEGIN(pCtx_);
    GBL_TYPE_ENSURE_INITIALIZED_();
    GBL_API_PUSH_VERBOSE("[GblType] Registering Builtin Type: %s", pName);
    flags |= GBL_TYPE_FLAG_BUILTIN;
    type = typeRegister_(parentType, pName, pTypeInfo, flags);
    GBL_API_CALL(gblVectorSize(&typeBuiltins_.vector, &size));
    GBL_API_VERIFY_EXPRESSION(size == expectedIndex+1,
                              "Failed to obtain expected index! "
                              "[expected: %u, actual: %u]", expectedIndex, size-1);
    // create initial/static instance of class if builtin
    {
        const GblTypeInfo* pInfo = gblTypeInfo(type);
        GBL_API_VERIFY_POINTER(pInfo);
        if(pInfo->classSize) {
            //gblTypeClassRef(type);
        }
    }
    GBL_API_POP(1);
    GBL_API_END_BLOCK();
    return type;
}

static GBL_RESULT gblTypeFinalize_(void) {
    GblBool hasMutex = GBL_FALSE;
    GBL_API_BEGIN(pCtx_);
    GBL_API_PUSH_VERBOSE("[GblType]: Finalizing");
    pthread_mutex_lock(&typeRegMtx_);
    hasMutex = GBL_TRUE;

    GblHashSet_destruct(&typeRegistry_);
    GBL_API_CALL(gblVectorDestruct(&typeBuiltins_.vector));

    GBL_API_POP(1);
    initialized_ = GBL_FALSE;
    GBL_API_END_BLOCK();
    if(hasMutex) {
        pthread_mutex_unlock(&typeRegMtx_);
        if(!initialized_) pthread_mutex_destroy(&typeRegMtx_);
    }
    return GBL_API_RESULT();
}

// === PUBLIC API ====

GBL_API gblTypeReinit(GblContext hCtx) {
    GBL_API_BEGIN(NULL);
    if(!initialized_) {
        GBL_API_PUSH_VERBOSE("[GblType]: Reinititalizing.");
        pCtx_ = hCtx;
        GBL_TYPE_ENSURE_INITIALIZED_();
    } else {
        GBL_API_CALL(gblTypeFinalize_());
        pCtx_ = hCtx;
        gblTypeInit_();
    }
    GBL_API_END();
}

GBL_EXPORT GblType gblTypeBuiltin(GblUint index) {
    GblType type = GBL_TYPE_INVALID;
    GBL_TYPE_ENSURE_INITIALIZED_();
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
    GBL_TYPE_ENSURE_INITIALIZED_();

    GblMetaClass* pTempClass = GBL_ALLOCA(sizeof(GblMetaClass));
    pTempClass->pName = pTypeName;

    if(strnlen(pTypeName, GBL_META_TYPE_NAME_SIZE_MAX+1) > GBL_META_TYPE_NAME_SIZE_MAX)
        GBL_API_WARN("Typename is too large and will be truncated: %s", pTypeName);

    pthread_mutex_lock(&typeRegMtx_);
    GblMetaClass** ppReturnValue = (GblMetaClass**)GblHashSet_get(&typeRegistry_, &pTempClass);
    if(ppReturnValue) {
        foundType = (GblType)*ppReturnValue;
    }
    pthread_mutex_unlock(&typeRegMtx_);

    GBL_API_END_BLOCK();
    return foundType;
}

GBL_EXPORT GblSize gblTypeCount(void) {
    GblSize count = 0;
    GBL_API_BEGIN(pCtx_);
    GBL_TYPE_ENSURE_INITIALIZED_();
    pthread_mutex_lock(&typeRegMtx_);
    count = GblHashSet_size(&typeRegistry_);
    pthread_mutex_unlock(&typeRegMtx_);
    GBL_API_END_BLOCK();
    return count;
}

GBL_EXPORT GblBool gblTypeIsValid(GblType type) {
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
    while(pMeta) {
        if(!pMeta->pParent || pMeta->flags & GBL_TYPE_FUNDAMENTAL_FLAGS_MASK) {
            base = (GblType)pMeta;
            break;
        }
        pMeta = pMeta->pParent;
    }
    GBL_API_END_BLOCK();
    return base;
}

GBL_EXPORT GblType gblTypeBase(GblType type, GblUint depth) {
    GblType base = GBL_TYPE_INVALID;
    GblMetaClass* pMeta =  GBL_META_CLASS(type);
    GBL_API_BEGIN(pCtx_);
    if(pMeta) {
        if(depth < pMeta->depth) {
            base = (GblType)pMeta->pBases[depth];
        } else if(depth == pMeta->depth) {
            base = (GblType)pMeta;
        }
    }
    GBL_API_END_BLOCK();
    return base;
}

GBL_EXPORT GblType gblTypeAncestor(GblType type, GblUint level) {
    GblType ancestor = GBL_TYPE_INVALID;
    GblMetaClass* pMeta =  GBL_META_CLASS(type);
    GBL_API_BEGIN(pCtx_);
    if(pMeta) {
        if(level == 0) {
            ancestor = (GblType)pMeta;
        } else if(level <= pMeta->depth) {
            ancestor = (GblType)pMeta->pBases[pMeta->depth-level];
        }
    }
    GBL_API_END_BLOCK();
    return ancestor;
}

GBL_EXPORT GblUint gblTypeDepth(GblType type) {
    GblUint depth = 0;
    GBL_API_BEGIN(pCtx_);
    GblMetaClass* pMeta = (GblMetaClass*)type;
    if(pMeta) {
        depth = pMeta->depth;
    }
    GBL_API_END_BLOCK();
    return depth;
}

GBL_EXPORT GblBool gblTypeFlagsTest(GblType type, GblFlags mask) {
    GblBool result = GBL_FALSE;
    GBL_API_BEGIN(pCtx_); {
        GblMetaClass* pMeta = (GblMetaClass*)type;
        if(pMeta) {
            GblFlags typeMask = (mask & GBL_TYPE_FLAGS_MASK & pMeta->flags);
            GblFlags fundamentalMask = (mask & GBL_TYPE_FUNDAMENTAL_FLAGS_MASK);
            if(fundamentalMask) {
                GblMetaClass* pFundamental = (GblMetaClass*)gblTypeFundamental(type);
                if(pFundamental) {
                    fundamentalMask &= pFundamental->flags;
                } else {
                    fundamentalMask = 0; // FLAGS FOR INVALID TYPE
                }
            }
            result = ((typeMask | fundamentalMask) != 0);
        }
    } GBL_API_END_BLOCK();
    return result;
}

GBL_EXPORT const GblTypeInfo* gblTypeInfo(GblType type) {
    const GblTypeInfo* pInfo      = NULL;
    GblMetaClass* pMeta = (GblMetaClass*)type;
    GBL_API_BEGIN(pCtx_);
    if(pMeta) pInfo = &pMeta->info;
    GBL_API_END_BLOCK();
    return pInfo;
}

GBL_EXPORT GblIPlugin* gblTypePlugin(GblType type) {
    return type == GBL_TYPE_INVALID? NULL : GBL_META_CLASS(type)->pPlugin;
}

GBL_EXPORT GblContext gblTypeContext(GblType type) {
    GBL_UNUSED(type);
    return pCtx_;
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


static GblBool typeIsA_(GblType derived, GblType base, GblBool classChecks, GblBool ifaceChecks) {
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
                result = classChecks? GBL_TRUE : GBL_FALSE;
                break;
            } else if(ifaceChecks) {
                // recurse over interfaces checking
                for(GblSize i = 0; i < pIter->info.interfaceCount; ++i) {
                    if(typeIsA_(pIter->info.pInterfaceMap[i].interfaceType, base, classChecks, ifaceChecks)) {
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

GBL_EXPORT GblBool gblTypeIsA(GblType type, GblType other) {
    return typeIsA_(type, other, GBL_TRUE, GBL_TRUE);
}

GBL_EXPORT GblBool gblTypeIsOrInherits(GblType derived, GblType base) {
    return typeIsA_(derived, base, GBL_TRUE, GBL_FALSE);
}

GBL_EXPORT GblBool gblTypeMapsInterface(GblType concrete, GblType iface) {
    return typeIsA_(concrete, iface, GBL_FALSE, GBL_TRUE);
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

    GBL_API_VERIFY_ARG(!(flags & GBL_TYPE_FLAG_BUILTIN),
                       "Cannot register a builtin type as a static type!");

    GBL_TYPE_ENSURE_INITIALIZED_();

    newType = typeRegister_(parent, pName, pInfo, flags);

    GBL_API_END_BLOCK();
    return newType;
}

GBL_EXPORT GBL_RESULT gblTypeUnregister(GblType type) {
    GblMetaClass* pMeta = (GblMetaClass*)type;
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_ARG(type != GBL_TYPE_INVALID);
    GBL_API_PUSH_VERBOSE("[GblType] Unregister: %s", pMeta->pName);
    GBL_TYPE_ENSURE_INITIALIZED_();
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
        pthread_mutex_lock(&typeRegMtx_);
        const GblBool success = GblHashSet_erase(&typeRegistry_, &pMeta);
        pthread_mutex_unlock(&typeRegMtx_);
        GBL_API_VERIFY(success, GBL_RESULT_ERROR_INVALID_TYPE, "Failed to remove the type from the registry HashSet!");
    }
    GBL_API_POP(1);
    GBL_API_END();
}

static GBL_RESULT gblTypeClassConstruct_(GblClass* pClass, GblMetaClass* pMeta);

static GBL_RESULT gblTypeInterfaceClassConstruct_(GblInterface* pClass, GblMetaClass* pMeta, int16_t offset) {
    GBL_API_BEGIN(pCtx_);
    GBL_API_PUSH_VERBOSE("InterfaceClass::construct(%s)", pMeta->pName);

    GBL_API_VERBOSE("offset: %d", offset);
    pClass->offsetToTop = offset;
    GBL_API_PUSH_VERBOSE("Default Class Ref");
    void* pInterfaceClass = gblTypeClassRef((GblType)pMeta);
    GBL_API_POP(1);
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
    GBL_API_PUSH_VERBOSE("Class::construct(%s)", pMeta->pName);
    GBL_API_VERBOSE("Type: %p", pMeta);

    // Zero initiailze class - NOPE OVERWRITING IFACE IF WE DO THAT!
    //memset(pClass, 0, pMeta->info.classSize);

    //IMMEDIATELY initialize its type!!!
    pClass->type = (GblType)pMeta;

    if(pMeta->pParent) {
        //GBL_API_PUSH_VERBOSE("Adding reference to parent class: ", pMeta->pParent->pName);
        //ensure construction of parent class (this will recurse)
        gblTypeClassRef((GblType)pMeta->pParent);
        //GBL_API_POP(1);
    } else {
        GBL_API_VERBOSE("Parent Class Ref: None");
    }

    for(uint8_t idx = 0; idx <= pMeta->depth; ++idx) {
        GblMetaClass* pIter = GBL_META_CLASS(gblTypeBase((GblType)pMeta, idx));
        GBL_API_PUSH_VERBOSE("Class[%u]: %s", idx, pIter->pName);

        //GBL_API_PUSH_VERBOSE("Constructing Interfaces");
        if(!pIter->info.interfaceCount) {
            //GBL_API_VERBOSE("Interfaces: None");
        } else {
            for(GblSize i = 0; i < pIter->info.interfaceCount; ++i) {
                const GblTypeInterfaceMapEntry* pIEntry = &pIter->info.pInterfaceMap[i];
                GblMetaClass* pIMeta = (GblMetaClass*)pIEntry->interfaceType;
                GBL_API_VERIFY_EXPRESSION(pIMeta);
                GBL_API_PUSH_VERBOSE("Interface[%u]: %s", i, pIMeta->pName);
                GblInterface* pIClass = (GblInterface*)((char*)pClass + pIEntry->classOffset);
                GBL_API_CALL(gblTypeInterfaceClassConstruct_(pIClass, pIMeta, -pIEntry->classOffset));
                GBL_API_POP(1);
            }
        }
        //GBL_API_POP(1);

        if(pIter->info.pFnClassInit) {
            //GBL_API_VERBOSE("ClassInit(%s)", pIter->pName);
            GBL_API_CALL(pIter->info.pFnClassInit(pClass, pMeta->info.pClassData, pCtx_));
        } else {
            //GBL_API_VERBOSE("Ctor: NULL", pIter->pName);
        }

        GBL_API_POP(1);
    }

   // GBL_API_POP(1);
    GBL_API_POP(1);
    GBL_API_END();
}

static GblClass* gblTypeClassCreate_(GblMetaClass* pMeta) {
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_ARG(pMeta);
    //GBL_API_PUSH_VERBOSE("Class::create(%s)", pMeta->pName);
    GBL_API_VERIFY_EXPRESSION(!atomic_load(&pMeta->refCount),
                              "Already have a reference to an invalid class object!");

    //Allocate a new class structure if one isn't already available
    if(!pMeta->pClass) {
        GBL_API_VERBOSE("Allocating separate class structure!");
        pMeta->pClass = GBL_API_MALLOC(pMeta->info.classSize, pMeta->info.classAlign, pMeta->pName);
    //Class must've been allocated with Meta class. Use existing data.
    } else {
       //GBL_API_VERBOSE("Using existing inline class allocation.");
    }

    // Call constructor
    GBL_API_CALL(gblTypeClassConstruct_(pMeta->pClass, pMeta));

    //GBL_API_POP(1);
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

    //GBL_API_PUSH_VERBOSE("Class::reference(%s)", pMeta->pName);
    GBL_TYPE_ENSURE_INITIALIZED_();

    // Return existing reference to class data
    if(pMeta->pClass && pMeta->pClass->type != GBL_TYPE_INVALID) {
        GBL_API_VERIFY_EXPRESSION(atomic_load(&pMeta->refCount),
                                  "No references to an initialized class!?");
        //GBL_API_VERBOSE("Using existing class data");
        pClass = pMeta->pClass;

    // Create a new class structure
    } else {
        pClass = gblTypeClassCreate_(pMeta);
        GBL_API_VERIFY_EXPRESSION(pClass && pClass == pMeta->pClass, "Failed to create class!");
    }

    // Either way, we're returning a new reference, add refcount
    GblRefCount oldCount = atomic_fetch_add(&pMeta->refCount, 1);
    GBL_API_VERBOSE("++[%s].refCount: %"GBL_SIZE_FMT, pMeta->pName, oldCount+1);

    //GBL_API_POP(1);
    GBL_API_END_BLOCK();
    return pClass;
}

GBL_EXPORT GblClass* gblTypeClassPeek(GblType typeId) {
    GblClass*       pClass  = NULL;
    GblMetaClass*   pMeta   = (GblMetaClass*)typeId;
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_ARG(typeId != GBL_TYPE_INVALID);
    GBL_API_VERIFY(pMeta->pClass, GBL_RESULT_ERROR_INVALID_OPERATION,
                  "[GblType] gblClassPeek(%s): no class!", gblTypeName(typeId));
    GBL_API_VERIFY(pMeta->refCount, GBL_RESULT_ERROR_INVALID_OPERATION,
                   "Cannot peek into uninitialized class: %s", gblTypeName(typeId));
    pClass = pMeta->pClass;

    GBL_API_END_BLOCK();
    return pClass;

}

static GBL_EXPORT GBL_RESULT gblTypeClassDestruct_(GblClass* pClass) {
    GblMetaClass* pMeta = GBL_META_CLASS(GBL_CLASS_TYPE(pClass));
    GBL_API_BEGIN(pCtx_);
    GBL_API_DEBUG("Destroying %s class!", pMeta->pName);
    GBL_API_PUSH();
    GblMetaClass* pIter = pMeta;
    // walk up the destructor chain
    GBL_API_PUSH_VERBOSE("Walking class destructors.");
    do {
        if(pIter->info.pFnClassFinalize) {
            GBL_API_DEBUG("Calling class dtor: [%s]", pIter->pName);
            GBL_API_CALL(pIter->info.pFnClassFinalize(pMeta->pClass, pMeta->info.pClassData, pCtx_));
        } else {
            GBL_API_DEBUG("No class dtor: [%s]", pIter->pName);
        }

        for(GblSize i = 0; i < pIter->info.interfaceCount; ++i) {
            GblInterface* pInterface = (GblInterface*)((const char*)pClass + pIter->info.pInterfaceMap[i].classOffset);
            GblClass* pDefaultIFaceClass = gblTypeClassPeek(GBL_CLASS_TYPE(pInterface));
            gblTypeClassUnref(pDefaultIFaceClass);

            gblTypeClassDestruct_(GBL_CLASS(pInterface));
            //gblTypeClassUnref(GBL_CLASS(pInterface));
            // unreference default interface class too!
            //gblTypeClassUnref(gblTypeClassPeek(GBL_CLASS_TYPE(pInterface)));
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
    pClass->type = GBL_TYPE_INVALID;

    GBL_API_POP(1);
    GBL_API_END();
}

GBL_EXPORT GblRefCount gblTypeClassUnref(GblClass* pClass) {
    GblRefCount refCount    = 0;
    GblMetaClass* pMeta     = NULL;
    GBL_API_BEGIN(pCtx_);
    if(!pClass) GBL_API_DONE(); //valid to Unref NULL pointer

    GBL_API_VERIFY(pClass->type != GBL_TYPE_INVALID,
                   GBL_RESULT_ERROR_INTERNAL,
                   "Class::unreference(): The specified class has an invalid ID!");

    pMeta = (GblMetaClass*)pClass->type;

    GBL_API_PUSH_VERBOSE("Class::unreference(%s): %u", gblTypeName(pClass->type), pMeta->refCount);

    GBL_API_VERIFY(atomic_load(&pMeta->refCount) != 0,
                   GBL_RESULT_ERROR_INTERNAL,
                   "The refcount for the given class was already at 0!");

    refCount = atomic_fetch_sub(&pMeta->refCount, 1);

    if(refCount-1) {
        GBL_API_VERBOSE("--[%s].refCount: %u", pMeta->pName, refCount-1);
    } else {
#ifdef GBL_TYPE_DEBUG
        {
            GblRefCount instanceRefCount = 0;
            instanceRefCount = atomic_load(&pMeta->instanceRefCount);
        GBL_API_VERIFY_EXPRESSION(instanceRefCount > 0,
                                  "Cannot destroy class when there are still %u instances using it!", instanceRefCount)
        }
#endif

        GBL_API_CALL(gblTypeClassDestruct_(pClass));
    }

    GBL_API_POP(1);
    GBL_API_END_BLOCK();
    return refCount;
}

GBL_RESULT typeInstanceConstructValidate_(GblType type) {
    GblMetaClass* pMeta = (GblMetaClass*)type;
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_ARG(type != GBL_TYPE_INVALID, "[GblType] Instance Construct: Invalid type!");
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


GBL_EXPORT GBL_RESULT gblTypeInstanceConstruct(GblType type, GblInstance* pInstance) {
    GblMetaClass*   pMeta = (GblMetaClass*)type;
    GblClass*       pClass = NULL;
    GBL_API_BEGIN(pCtx_);
    GBL_API_CALL(typeInstanceConstructValidate_(type));
    GBL_API_PUSH_VERBOSE("[GblType] Instance Construct: type %s", pMeta->pName);

    // Zero Initialize
    memset(pInstance, 0, pMeta->info.instanceSize);

    // Set Class
    pClass = gblTypeClassRef(type);
    GBL_API_VERIFY_EXPRESSION(pClass, "Failed to retrieve class reference!");
    pInstance->pClass = pClass;

    GBL_API_DEBUG("Calling constructor chain.");
    GBL_API_PUSH();

    for(uint8_t idx = 0; idx <= pMeta->depth; ++idx) {
        GblMetaClass* pIter = GBL_META_CLASS(gblTypeBase((GblType)pMeta, idx));
        if(pIter->info.pFnInstanceInit) {
            GBL_API_DEBUG("Calling instance initializers: [%s]", pIter->pName);
            GBL_API_CALL(pIter->info.pFnInstanceInit(pInstance, pCtx_));
        } else {
            GBL_API_DEBUG("No instance ctor: [%s]", pIter->pName);
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

GBL_EXPORT GblInstance* gblTypeInstanceCreate(GblType type) {
    GblInstance* pInstance = NULL;
    GblMetaClass* pMeta     = (GblMetaClass*)type;
    GBL_API_BEGIN(pCtx_);
    GBL_API_CALL(typeInstanceConstructValidate_(type));
    GBL_API_PUSH_VERBOSE("[GblType] Instance Create: type %s", pMeta->pName);

    GBL_API_DEBUG("Allocating %u bytes.", pMeta->info.instanceSize);

    GBL_API_VERIFY(GBL_TYPE_IS_INSTANTIABLE(type) && !GBL_TYPE_IS_ABSTRACT(type),
                   GBL_RESULT_ERROR_INVALID_TYPE,
                   "Cannot instantiate non-instantiable or abstract types!");

    pInstance = GBL_API_MALLOC(pMeta->info.instanceSize, pMeta->info.instanceAlign, pMeta->pName);

    GBL_API_CALL(gblTypeInstanceConstruct(type, pInstance));

    GBL_API_POP(1);
    GBL_API_END_BLOCK();
    return pInstance;
}

GBL_EXPORT GblRefCount gblTypeInstanceDestruct(GblInstance* pInstance) {
    GblRefCount refCount = 0;
    GBL_API_BEGIN(pCtx_);
    refCount = gblTypeClassUnref(gblTypeClassFromInstance(pInstance));
    GBL_API_END_BLOCK();
    return refCount;
}

GBL_EXPORT GblRefCount gblTypeInstanceDestroy(GblInstance* pInstance) {
    GblRefCount refCount = 0;
    GBL_API_BLOCK(pCtx_, {
       if(!pInstance) GBL_API_DONE();
       else {
            GBL_API_PUSH_VERBOSE("[GblType] Instance Destroy: %s",
                                 gblTypeName(gblTypeClassFromInstance(pInstance)->type));

            refCount = gblTypeInstanceDestruct(pInstance);
            GBL_API_FREE(pInstance);

            GBL_API_POP(1);

        }
    });
    return refCount;
}

GBL_EXPORT GblBool gblTypeClassIsA(const GblClass* pClass, GblType type) {
    GblBool result = GBL_FALSE;
    GBL_API_BEGIN(pCtx_);
    if(!(!pClass && type == GBL_TYPE_INVALID)) {
        GBL_API_VERIFY_POINTER(pClass);
        result = gblTypeIsA(pClass->type, type);
    }
    GBL_API_END_BLOCK();
    return result;
}

static GblClass* typeClassCast_(const GblClass* pClass, GblType toType, GblBool check) {
    GblClass* pClassStart = pClass;
    GblClass* pToClass = NULL;
    GBL_API_BEGIN(pCtx_);

    // Casting a NULL pointer or to an invalid type returns NULL
    if(pClass && toType != GBL_TYPE_INVALID) GBL_LIKELY {
        //early exit
        if(pClass->type == toType) GBL_LIKELY {
            pToClass = pClass;
        } else {
            GblBool toInterface = GBL_FALSE;
            if(GBL_TYPE_IS_INTERFACE(toType)) toInterface = GBL_TRUE;

            // If current node is an interface class, attempt to "back out" to root class node
            while(gblTypeFundamental(pClass->type) == GBL_TYPE_INTERFACE) {
                // Move class pointer to interface's concrete class
                pClass = typeInterfaceOuterClass_((GblInterface*)pClass);
                GBL_API_VERIFY_POINTER(pClass);
            }

            GblMetaClass* pMeta = (GblMetaClass*)pClass->type;
            GBL_API_VERIFY_POINTER(pMeta);

            // iterate from derived to base class, breadth-first searching
            do {
                //check current class
                if((GblType)pMeta == toType) {
                    pToClass = pClass;
                    break;
                // check current class's interfaces
                }
                pMeta = pMeta->pParent;
            } while(pMeta);

            if(!pToClass && toInterface) {
                GblInterface* pInterface = typeClassInterfacePeek_(pClass, toType);
                if(pInterface) {
                    pToClass = &pInterface->base;
                }
            }
        }

    }
    GBL_API_END_BLOCK();
    if(check && !pToClass) {
        if(toType == GBL_TYPE_INVALID) {
            GBL_API_WARN("Attempted to cast from type %s to TYPE_INVALID!",
                         gblTypeName(pClassStart->type));
        } else {
            GBL_API_WARN("Failed to cast from type %s to %s!",
                         gblTypeName(pClassStart->type), gblTypeName(toType));
        }
    }
    return pToClass;
}

GBL_EXPORT GblClass* gblTypeClassTryCast(const GblClass* pClass, GblType toType) {
    return typeClassCast_(pClass, toType, GBL_FALSE);
}

GBL_EXPORT GblClass* gblTypeClassCast(const GblClass* pClass, GblType toType) {
    return typeClassCast_(pClass, toType, GBL_TRUE);
}

GBL_EXPORT GblType gblTypeFromClass(const GblClass* pClass) {
    GblType type = GBL_TYPE_INVALID;
    GBL_API_BEGIN(pCtx_);
    if(pClass) {
        type = pClass->type;
    }
    GBL_API_END_BLOCK();
    return type;
}

GBL_EXPORT GblBool gblTypeInstanceIsA(const GblInstance* pInstance, GblType type) {
    GblBool result = GBL_FALSE;
    GBL_API_BEGIN(pCtx_);
    if(!(!pInstance && type == GBL_TYPE_INVALID)) {
        GBL_API_VERIFY_POINTER(pInstance);
        result = gblTypeClassIsA(pInstance->pClass, type);
    }
    GBL_API_END_BLOCK();
    return result;
}

GBL_EXPORT GblInstance* typeInstanceCast_(const GblInstance* pInstance, GblType type, GblBool check) {
    GblInstance* pOutInstance = NULL;
    GBL_API_BEGIN(pCtx_);
    if(!(!pInstance && type == GBL_TYPE_INVALID)) {
        GBL_API_VERIFY_POINTER(pInstance);
        if(gblTypeInstanceIsA(pInstance, type)) {
            pOutInstance = pInstance;
        } else if(check) {
            GBL_API_WARN("Failed to cast instance from %s to %s.",
                         gblTypeName(gblTypeClassFromInstance(pInstance)->type),
                         gblTypeName(type));
        }
    }
    GBL_API_END_BLOCK();
    return pOutInstance;
}


GBL_EXPORT GblInstance* gblTypeInstanceTryCast(const GblInstance* pInstance, GblType type) {
    return typeInstanceCast_(pInstance, type, GBL_FALSE);
}

GBL_EXPORT GblInstance* gblTypeInstanceCast(const GblInstance* pInstance, GblType type) {
    return typeInstanceCast_(pInstance, type, GBL_TRUE);
}

GBL_EXPORT GblClass* gblTypeClassFromInstance(const GblInstance* pInstance) {
    GblClass* pClass = NULL;
    GBL_API_BEGIN(pCtx_);
    if(pInstance) {
        pClass = pInstance->pClass;
    }
    GBL_API_END_BLOCK();
    return pClass;
}

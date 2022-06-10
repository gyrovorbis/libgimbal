#include "gimbal_type_.h"
#include <gimbal/meta/gimbal_class.h>
#include <gimbal/meta/gimbal_instance.h>
#include <gimbal/meta/gimbal_interface.h>
#include <gimbal/algorithms/gimbal_hash.h>
#include <gimbal/algorithms/gimbal_sort.h>
#include <gimbal/ifaces/gimbal_ivariant.h>

GblContext*              pCtx_           = NULL;
once_flag                initOnce_       = ONCE_FLAG_INIT;
GblBool                  inittedOnce_    = GBL_FALSE;
GblBool                  initialized_    = GBL_FALSE;
GBL_THREAD_LOCAL GblBool initializing_   = GBL_FALSE;
mtx_t                    typeRegMtx_;
GblHashSet               typeRegistry_;
struct TypeBuiltins_     typeBuiltins_;


// === STATIC DATA ====

static GblSize           initialTypeBuiltinCount_ = GBL_TYPE_BUILTIN_COUNT;
static GblSize           initialTypeTotalCount_   = GBL_TYPE_REGISTRY_HASH_MAP_CAPACITY_DEFAULT_;


GBL_MAYBE_UNUSED static int metaClassIFaceEntryComparator_(const void* pA, const void* pB) {
    const GblTypeInterfaceMapEntry* pIFaceA = pA;
    const GblTypeInterfaceMapEntry* pIfaceB = pB;
    return pIFaceA->interfaceType - pIfaceB->interfaceType;
}

static GblHash metaClassHasher_(const GblHashSet* pMap, const void* pItem) {
    GBL_UNUSED(pMap);
    const GblMetaClass** pMeta = (const GblMetaClass**)pItem;
    return gblHashMurmur(&(*pMeta)->name, sizeof(GblQuark));
}

static GblBool metaClassComparator_(const GblHashSet* pMap, const void* pA, const void* pB) {
    GBL_UNUSED(pMap);
    const GblMetaClass** ppRhs = (const GblMetaClass**)pA, **ppLhs = (const GblMetaClass**)pB;
    return (*ppRhs)->name == (*ppLhs)->name;
}

static void metaClassElementFree_(const GblHashSet* pMap, void* item) {
    GBL_UNUSED(pMap);
    GBL_API_BEGIN(pCtx_);
    GblMetaClass** ppMetaClass = (GblMetaClass**)item;
    GBL_API_FREE(*ppMetaClass);
    GBL_API_END_BLOCK();
}

GBL_EXPORT GblBool GblType_conforms_(GblType type, GblType dependent, GblBool verify) {
    GblBool conforms = GBL_FALSE;
    GBL_API_BEGIN(pCtx_);
    if(dependent != GBL_INVALID_TYPE && type != GBL_INVALID_TYPE) {
        if(GblType_flagsTest(dependent, GBL_TYPE_FUNDAMENTAL_FLAG_DEPENDENT)) {
            for(GblInt a = (GblInt)GblType_depth(dependent); a >= 0; --a) {
                GblType ancestor = GblType_ancestor(dependent, a);
                GblMetaClass* pMeta = GBL_META_CLASS_(ancestor);
                for(GblSize p = 0; p < pMeta->info.dependencyCount; ++p) {
                    if(GblType_check(type, pMeta->info.pDependencies[p]) ||
                            GblType_conforms_(type, pMeta->info.pDependencies[p], verify))
                    {
                        continue;
                    } else if(verify) {
                        GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_TYPE,
                                           "Type [%s] failed to conform to type [%s] "
                                           "due to not satisfying dependency: [%s]",
                                           GblType_name(type),
                                           GblType_name(dependent),
                                           GblType_name(pMeta->info.pDependencies[p]));
                    } else GBL_API_DONE(); //if even one check fails, you're done.
                }
            }
            conforms = GBL_TRUE; // oh shit, you made it to the end!
        }
    }
    GBL_API_END_BLOCK();
    return conforms;
}

static GBL_RESULT typeLog_(GblType parent,
                        const GblTypeInfo* pInfo,
                        GblFlags flags)
{
    GBL_API_BEGIN(pCtx_);
    GBL_API_PUSH_VERBOSE("Type Info");
    GBL_API_VERBOSE("%-20s: %-100.100s", "Parent Type", GblType_name(parent));


    GBL_API_PUSH_VERBOSE("flags");
    GBL_API_VERBOSE("%-20s: %-100u", "DEPENDENT",           (flags & GBL_TYPE_FUNDAMENTAL_FLAG_DEPENDENT)? 1 : 0);
    GBL_API_VERBOSE("%-20s: %-100u", "CLASSED",             (flags & GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED)? 1 : 0);
    GBL_API_VERBOSE("%-20s: %-100u", "INSTANTIABLE",        (flags & GBL_TYPE_FUNDAMENTAL_FLAG_INSTANTIABLE)? 1 : 0);
    GBL_API_VERBOSE("%-20s: %-100u", "DERIVABLE",           (flags & GBL_TYPE_FUNDAMENTAL_FLAG_DERIVABLE)? 1 : 0);
    GBL_API_VERBOSE("%-20s: %-100u", "DEEP_DERIVABLE",      (flags & GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE)? 1 : 0);
    GBL_API_VERBOSE("%-20s: %-100u", "UNMAPPABLE",          (flags & GBL_TYPE_FLAG_UNMAPPABLE)? 1 : 0);
    GBL_API_VERBOSE("%-20s: %-100u", "BUILTIN",             (flags & GBL_TYPE_FLAG_BUILTIN)? 1 : 0);
    GBL_API_VERBOSE("%-20s: %-100u", "ABSTRACT",            (flags & GBL_TYPE_FLAG_ABSTRACT)? 1 : 0);
    GBL_API_VERBOSE("%-20s: %-100u", "FINAL",               (flags & GBL_TYPE_FLAG_FINAL)? 1 : 0);
    GBL_API_POP(1);

    GBL_API_PUSH_VERBOSE("Class Info");
    GBL_API_VERBOSE("%-20s: %-100u", "size",            pInfo->classSize);
    GBL_API_VERBOSE("%-20s: %-100u", "privateSize",     pInfo->classPrivateSize);
    GBL_API_VERBOSE("%-20s: %-100p", "data",            pInfo->pClassData);
    GBL_API_VERBOSE("%-20s: %-100p", "initializer",     pInfo->pFnClassInit);
    GBL_API_VERBOSE("%-20s: %-100p", "finalizer",       pInfo->pFnClassFinal);
    GBL_API_POP(1);


    GBL_API_PUSH_VERBOSE("Instance Info");
    GBL_API_VERBOSE("%-20s: %-100u", "size",            pInfo->instanceSize);
    GBL_API_VERBOSE("%-20s: %-100u", "privateSize",     pInfo->instancePrivateSize);
    GBL_API_VERBOSE("%-20s: %-100p", "initializer",     pInfo->pFnInstanceInit);
    GBL_API_POP(1);

    if(pInfo->interfaceCount) {
        GBL_API_PUSH_VERBOSE("Interface Map");
        for(GblSize i = 0; i < pInfo->interfaceCount; ++i) {
            GBL_API_VERBOSE("%u: [Type: %s, Class Offset: %u]",
                            i,
                            GblType_name(pInfo->pInterfaceMap[i].interfaceType),
                            pInfo->pInterfaceMap[i].classOffset);
        }
        GBL_API_POP(1);
    }

    if(pInfo->dependencyCount) {
        GBL_API_PUSH_VERBOSE("Dependencies");
        for(GblSize p = 0; p < pInfo->dependencyCount; ++p) {
            GBL_API_VERBOSE("%u: [Type: %s]",
                            p,
                            GblType_name(pInfo->pDependencies[p]));

        }
        GBL_API_POP(1);
    }
    GBL_API_POP(1);
    GBL_API_END();
}

static GblFlags typeFlagsWithImplied_(GblType parent,
                               const GblTypeInfo* pInfo,
                               GblFlags flags)
{
    GblMetaClass* pParentMeta = GBL_META_CLASS_(parent);
    if(!pParentMeta && pInfo->dependencyCount)              flags |= GBL_TYPE_FUNDAMENTAL_FLAG_DEPENDENT;
    if(!pParentMeta && pInfo->classSize)                    flags |= GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED;
    if(!pParentMeta && pInfo->instanceSize)                 flags |= GBL_TYPE_FUNDAMENTAL_FLAG_INSTANTIABLE;
    if(flags & GBL_TYPE_FUNDAMENTAL_FLAG_INTERFACED) {
                                                            flags |= GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED;
                                                            flags |= GBL_TYPE_FUNDAMENTAL_FLAG_DEPENDENT;
    }
    if(flags & GBL_TYPE_FUNDAMENTAL_FLAG_INSTANTIABLE)      flags |= GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED;
    if(flags & GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE)    flags |= GBL_TYPE_FUNDAMENTAL_FLAG_DERIVABLE;
    return flags;
}

static GBL_RESULT typeValidate_(GblType parent,
                         const GblTypeInfo* pInfo,
                         GblFlags flags)
{
    GblMetaClass* pParentMeta = GBL_META_CLASS_(parent);
    GBL_API_BEGIN(pCtx_);

    GblFlags fundFlags = (flags & GBL_TYPE_FUNDAMENTAL_FLAGS_MASK);
    GBL_API_VERIFY(!(pParentMeta && fundFlags),
                   GBL_RESULT_ERROR_INVALID_TYPE,
                   "Cannot use FUNDAMENTAL FLAGS on DERIVED type!");

    if(pParentMeta) {
        const GblType fundamentalType = GblType_fundamental(parent);
        fundFlags |= GBL_META_CLASS_(fundamentalType)->flags;
    }
    fundFlags |= flags;

    GBL_API_VERIFY(!(pParentMeta && !(fundFlags & GBL_TYPE_FUNDAMENTAL_FLAG_DERIVABLE)),
                   GBL_RESULT_ERROR_INVALID_TYPE,
                   "Cannot DERIVE from a NON-DERIVABLE type!");

    GBL_API_VERIFY(!(pParentMeta && GblType_depth(parent) > 0 &&
                     !(fundFlags & GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE)),
                     GBL_RESULT_ERROR_INVALID_TYPE,
                     "Cannot REDERIVE from a non DEEP DERIVABLE type!");

    if(fundFlags & GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED) {
        GBL_API_VERIFY(pInfo->classSize >= sizeof(GblClass), GBL_RESULT_ERROR_INVALID_TYPE,
                       "Cannot register a CLASSED type with a class size < sizeof(GblClass): [%u]!",
                       sizeof(GblClass));

        GBL_API_VERIFY(!(pInfo->pInterfaceMap && !pInfo->interfaceCount),
                       GBL_RESULT_ERROR_INVALID_TYPE,
                       "Passed non-NULL interface map with an interface count of 0!");
    } else {
        GBL_API_VERIFY(pInfo->classSize == 0, GBL_RESULT_ERROR_INVALID_TYPE,
                       "Cannot register a NON CLASSED type with a class size > 0!");
    }

    if(fundFlags & GBL_TYPE_FUNDAMENTAL_FLAG_INTERFACED) {
        GBL_API_VERIFY(!(fundFlags & GBL_TYPE_FUNDAMENTAL_FLAG_INSTANTIABLE),
                       GBL_RESULT_ERROR_INVALID_TYPE,
                       "INTERFACED types cannot be INSTANTIABLE!");

        GBL_API_VERIFY(pInfo->classSize >= sizeof(GblInterface), GBL_RESULT_ERROR_INVALID_TYPE,
                       "Cannot register an INTERFACED type with a class size < sizeof(GblInterface): [%u]!",
                       sizeof(GblClass));
    }

    if(fundFlags & GBL_TYPE_FUNDAMENTAL_FLAG_INSTANTIABLE) {
        GBL_API_VERIFY(pInfo->instanceSize >= sizeof(GblInstance), GBL_RESULT_ERROR_INVALID_TYPE,
                       "Cannot register an INSTANTIABLE type with an instance size < sizeof(GblInstance): [%u]",
                       sizeof(GblInstance));
        GBL_API_VERIFY(fundFlags & GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED, GBL_RESULT_ERROR_INVALID_TYPE,
                       "Cannot register an INSTANTIABLE type without it being CLASSED!");
    } else {
        GBL_API_VERIFY(pInfo->instanceSize == 0, GBL_RESULT_ERROR_INVALID_TYPE,
                       "Cannot register a NON INSTANTIABLE type with an instance size > 0!");
    }

    GBL_API_VERIFY(!(pParentMeta && (pParentMeta->flags & GBL_TYPE_FLAG_FINAL)), GBL_RESULT_ERROR_INVALID_TYPE,
                   "Cannot derive from a FINAL type!");

    if(pInfo->dependencyCount) {
        GBL_API_VERIFY(fundFlags & GBL_TYPE_FUNDAMENTAL_FLAG_DEPENDENT,
                       GBL_RESULT_ERROR_INVALID_TYPE,
                       "PREREQUESITES are only supported for DEPENDENT types!");

        GblSize instantiableCount = 0;
        for(GblSize p = 0; p < pInfo->dependencyCount; ++p) {
            if(GBL_TYPE_IS_INSTANTIABLE(pInfo->pDependencies[p]))
                ++instantiableCount;
            else GBL_API_VERIFY(GBL_TYPE_IS_INTERFACED(pInfo->pDependencies[p]) ||
                                GBL_TYPE_IS_DEPENDENT(pInfo->pDependencies[p]),
                                GBL_RESULT_ERROR_INVALID_TYPE,
                                "DEPENDENCIES can only be INTERFACED or DEPENDENT types!");
        }
        GBL_API_VERIFY(instantiableCount <= 1, GBL_RESULT_ERROR_INVALID_TYPE,
                       "Cannot have multiple INSTANTIABLE DEPENDENCY types: [%u]",
                       instantiableCount);
    }

    if(pInfo->interfaceCount) {
        GBL_API_VERIFY(pInfo->pInterfaceMap,
                       GBL_RESULT_ERROR_INVALID_TYPE,
                       "NULL pointer passed as interface map!");

        GBL_API_VERIFY((fundFlags & GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED),
                       GBL_RESULT_ERROR_INVALID_TYPE,
                       "Cannot specify an INTERFACE MAP on NON-CLASSED fundamental type!");

        // Interface pass 1: validate individual entries
        for(GblSize i = 0; i < pInfo->interfaceCount; ++i) {
            const GblTypeInterfaceMapEntry* pEntry = &pInfo->pInterfaceMap[i];
            const GblType ifaceType = pEntry->interfaceType;

            GBL_API_VERIFY(GblType_verify(ifaceType),
                           GBL_RESULT_ERROR_INVALID_TYPE,
                           "INTERFACE[%u]: Invalid type!",
                           i);

            GBL_API_VERIFY(GblType_flagsTest(ifaceType, GBL_TYPE_FUNDAMENTAL_FLAG_INTERFACED),
                           GBL_RESULT_ERROR_INVALID_TYPE,
                           "INTERFACE[%u: %s]: Not an INTERFACED fundamental type!",
                           i, GblType_name(ifaceType));

            GblType commonType = GBL_INVALID_TYPE;
            GBL_API_VERIFY((commonType = GblType_common(parent, ifaceType)) == GBL_INVALID_TYPE,
                           GBL_RESULT_ERROR_INVALID_TYPE,
                           "INTERFACE[%u: %s]: Forms AMBIGUOUS TYPE[%s] with base class [%s]!",
                           i, GblType_name(ifaceType), GblType_name(commonType), GblType_name(parent));

            GblMetaClass* pIMeta = GBL_META_CLASS_(ifaceType);

            GBL_API_VERIFY(pEntry->classOffset + pIMeta->info.classSize <= pInfo->classSize,
                           GBL_RESULT_ERROR_INVALID_TYPE,
                           "INTERFACE[%u: %s]: mapped BEYOND class boundary [%u > %u]!",
                           i, GblType_name(ifaceType), pEntry->classOffset + pIMeta->info.classSize, pInfo->classSize);


            if(pParentMeta) {
                GBL_API_VERIFY(pEntry->classOffset >= pParentMeta->info.classSize,
                               GBL_RESULT_ERROR_INVALID_TYPE,
                               "INTERFACE[%u: %s]: offset [%u] maps WITHIN base class [<%u]!",
                               i, GblType_name(ifaceType), pIMeta->info.classSize, pParentMeta->info.classSize);
            }
        }

        // Interfae pass 2: validate relative to each other
        for(GblSize i = 0; i < pInfo->interfaceCount; ++i) {
            const GblTypeInterfaceMapEntry* pEntry = &pInfo->pInterfaceMap[i];
            const GblType ifaceType = pEntry->interfaceType;
            GblMetaClass* pIMeta = GBL_META_CLASS_(ifaceType);
            for(GblSize ii = i+1; ii < pInfo->interfaceCount; ++ii) {
                const GblTypeInterfaceMapEntry* pOtherEntry = &pInfo->pInterfaceMap[ii];
                const GblType otherIfaceType = pOtherEntry->interfaceType;

                GblType common = GBL_INVALID_TYPE;


                GBL_API_VERIFY((common = GblType_common(ifaceType, otherIfaceType)) == GBL_INVALID_TYPE,
                               GBL_RESULT_ERROR_INVALID_TYPE,
                               "INTERFACE[%s] forms AMBIGUOUS TYPE[%s] with INTERFACE[%s]",
                               GblType_name(ifaceType), GblType_name(common), GblType_name(otherIfaceType));

                GBL_API_VERIFY(pEntry->classOffset + pIMeta->info.classSize <= pOtherEntry->classOffset,
                               GBL_RESULT_ERROR_INVALID_TYPE,
                               "INTERFACE[%u: %s] data OVERLAPS with INTERFACE[%u: %s]",
                               i, GblType_name(ifaceType), ii, GblType_name(otherIfaceType));


            }
        }

        for(GblSize i = 0; i < pInfo->interfaceCount; ++i) {
            const GblTypeInterfaceMapEntry* pEntry = &pInfo->pInterfaceMap[i];
            const GblType ifaceType = pEntry->interfaceType;
            GblMetaClass* pIMeta = GBL_META_CLASS_(ifaceType);

            for(GblSize p = 0; p < pIMeta->info.dependencyCount; ++p) {
                GblType prereqType = pIMeta->info.pDependencies[p];

                GblBool satisfied = GBL_FALSE;
                if(GblType_conforms_(parent, prereqType, GBL_TRUE)) {
                    continue;
                }

                for(GblSize ii = 0; ii < i; ++ii) {
                    GblType prevIFaceType = pInfo->pInterfaceMap[ii].interfaceType;
                    if(GblType_check(prevIFaceType, prereqType)) {
                        satisfied = GBL_TRUE;
                        break;
                    }

                    for(GblSize pp = 0; pp < p; ++pp) {
                        GblType prevPrereqType = pIMeta->info.pDependencies[pp];
                        if(GblType_check(prevPrereqType, prereqType)) {
                            satisfied = GBL_TRUE;
                            break;
                        }
                    }
                }

                GBL_API_VERIFY(satisfied, GBL_RESULT_ERROR_INVALID_TYPE,
                               "Failed to satisfy DEPENDENCY type [%s] on INTERFACE ENTRY[%u] (%s)",
                               GblType_name(prereqType),
                               i,
                               GblType_name(ifaceType));
            }
        }
    }

    GBL_API_END();

}

static GblType typeRegister_(GblType parent,
                            const char* pName,
                            const GblTypeInfo* pInfo,
                            GblFlags flags)
{
    GblBool hasMutex = GBL_FALSE;
    GblType newType = GBL_INVALID_TYPE;
    GblMetaClass* pParent = GBL_META_CLASS_(parent);
    static const GblTypeInfo defaultTypeInfo = {
        .pFnClassInit = NULL,
        .pFnClassFinal = NULL,
        .classSize = 0,
        .classPrivateSize = 0,
        .pClassData = NULL,
        .interfaceCount = 0,
        .pInterfaceMap = NULL,
        .dependencyCount = 0,
        .pDependencies = NULL,
        .pFnInstanceInit = NULL,
        .instanceSize = 0,
        .instancePrivateSize = 0
    };
    pInfo = pInfo? pInfo : &defaultTypeInfo;
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_POINTER(pName);
    GBL_API_VERIFY_POINTER(pInfo);

    GBL_API_PUSH_VERBOSE("[GblType] Register Type: %s", pName);

    GBL_API_VERIFY(GblType_fromName(pName) == GBL_INVALID_TYPE,
                   GBL_RESULT_ERROR_INVALID_TYPE,
                   "An existing entry was found with the same type name!");
    GBL_API_CALL(typeLog_(parent, pInfo, flags));
    flags = typeFlagsWithImplied_(parent, pInfo, flags);
    GBL_API_VERIFY_CALL(typeValidate_(parent, pInfo, flags));


    {
        uint8_t baseCount = 0;
        GblMetaClass* pParentIt = pParent;
        while(pParentIt) {
            ++baseCount;
            pParentIt = pParentIt->pParent;
        }

        GblSize metaSize = sizeof(GblMetaClass);
        const GblSize ifaceOffset   = metaSize;
        const GblSize prereqOffset  = metaSize += sizeof(GblTypeInterfaceMapEntry) * pInfo->interfaceCount;
        const GblSize classOffset   = metaSize += sizeof(GblType)                  * pInfo->dependencyCount;
        const GblSize basesOffset   = metaSize += pInfo->classSize;
        metaSize += baseCount * sizeof(GblMetaClass*);

        const GblSize alignRem = (metaSize % GBL_META_CLASS_ALIGNMENT_);
        if(alignRem)
            metaSize += (GBL_META_CLASS_ALIGNMENT_ - (alignRem));

        GBL_API_PUSH_VERBOSE("MetaClass Info");
        GBL_API_VERBOSE("%-20s: %-100u", "size", metaSize);
        GBL_API_VERBOSE("%-20s: %-100u", "depth", baseCount);
        GBL_API_POP(2);

        GblMetaClass* pMeta = GBL_API_MALLOC(metaSize,
                                             GBL_META_CLASS_ALIGNMENT_,
                                             pName);

        GBL_API_VERIFY_EXPRESSION(!((uint64_t)pMeta & GBL_CLASS_FLAGS_BIT_MASK_),
                                  "malloc returned a misaligned pointer!");

        memset(pMeta, 0, metaSize);
        memcpy(&pMeta->info, pInfo, sizeof(GblTypeInfo));
        GBL_ATOMIC_INT16_INIT(pMeta->refCount, 0);
        GBL_ATOMIC_INT16_INIT(pMeta->instanceRefCount, 0);
#ifdef GBL_TYPE_DEBUG
        GBL_ATOMIC_INT16_INIT(&pMeta->instanceRefCount, 0);
#endif
        pMeta->flags                = flags;
        pMeta->depth                = baseCount;
        pMeta->classPrivateOffset   = !pParent? 0 : pParent->classPrivateOffset - pInfo->classPrivateSize;
        pMeta->instancePrivateOffset= !pParent? 0 : pParent->instancePrivateOffset - pInfo->instancePrivateSize;
        pMeta->pParent              = (struct GblMetaClass*)parent;
        pMeta->name                 = GblQuark_fromString(pName);
        pMeta->pClass               = pInfo->classSize?
                                            (GblClass*)((char*)pMeta + classOffset) : NULL;
        pMeta->info.pInterfaceMap   = pInfo->interfaceCount?
                                            (GblTypeInterfaceMapEntry*)((char*)pMeta + ifaceOffset) : NULL;
        pMeta->info.pDependencies  = pInfo->dependencyCount?
                                            (GblType*)((char*)pMeta + prereqOffset) : NULL;
        pMeta->pBases               = baseCount?
                                            (GblMetaClass**)((char*)pMeta + basesOffset) : NULL;

        memcpy((GblTypeInterfaceMapEntry*)pMeta->info.pInterfaceMap,
               pInfo->pInterfaceMap,
               sizeof(GblTypeInterfaceMapEntry) * pInfo->interfaceCount);
        //gblSortQuick((GblTypeInterfaceMapEntry*)pMeta->info.pInterfaceMap, pMeta->info.interfaceCount, sizeof(GblTypeInterfaceMapEntry), metaClassIFaceEntryComparator_);

        memcpy((GblType*)pMeta->info.pDependencies,
               pInfo->pDependencies,
               sizeof(GblType*) * pInfo->dependencyCount);

        pParentIt = pParent;
        baseCount = 0;
        while(pParentIt) {
            pMeta->pBases[pMeta->depth-(++baseCount)] = pParentIt;
            pParentIt = pParentIt->pParent;
        }
        GBL_API_VERIFY_EXPRESSION(baseCount == pMeta->depth);

        mtx_lock(&typeRegMtx_);
        hasMutex = GBL_TRUE;

        const GblMetaClass* pOldData = GblHashSet_set(&typeRegistry_,
                                                      &pMeta);

        GBL_API_VERIFY(!pOldData, GBL_RESULT_ERROR_INVALID_ARG,
                       "A previous metatype entry named %s existed already!", pName);

        newType = (GblType)pMeta;
        if(flags & GBL_TYPE_FLAG_BUILTIN) {
             GBL_API_CALL(GblVector_pushBack(&typeBuiltins_.vector, &newType));
        }

    }
    GBL_API_END_BLOCK();
    if(hasMutex) mtx_unlock(&typeRegMtx_);
    return newType;
}

static GBL_RESULT GblType_registerBuiltins_(void) {
    GBL_API_BEGIN(pCtx_);
    GblType_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_PROTOCOL,
                            GBL_INVALID_TYPE,
                            GblQuark_internStringStatic("Protocol"),
                            NULL,
                            GBL_TYPE_FUNDAMENTAL_FLAG_DEPENDENT |
                            GBL_TYPE_FUNDAMENTAL_FLAG_DERIVABLE);
    GblType_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_CLASS,
                            GBL_INVALID_TYPE,
                            GblQuark_internStringStatic("Class"),
                            &((const GblTypeInfo) {
                                .classSize    = sizeof(GblClass)
                            }),
                            GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED           |
                            GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE);
    GblType_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_INTERFACE,
                            GBL_INVALID_TYPE,
                            GblQuark_internStringStatic("Interface"),
                            &((const GblTypeInfo) {
                                .classSize    = sizeof(GblInterface)
                            }),
                            GBL_TYPE_FUNDAMENTAL_FLAG_DEPENDENT         |
                            GBL_TYPE_FUNDAMENTAL_FLAG_INTERFACED        |
                            GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE    |
                            GBL_TYPE_FLAG_ABSTRACT                      |
                            GBL_TYPE_FLAG_UNMAPPABLE);
    GblType_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_INSTANCE,
                            GBL_INVALID_TYPE,
                            GblQuark_internStringStatic("Instance"),
                            &((const GblTypeInfo) {
                                .classSize    = sizeof(GblClass),
                                .instanceSize = sizeof(GblInstance)
                            }),
                            GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE    |
                            GBL_TYPE_FUNDAMENTAL_FLAG_INSTANTIABLE);
    GBL_API_CALL(GblIVariant_typeRegister_(pCtx_));
    GBL_API_CALL(gblValueTypesRegister_(pCtx_));
    GBL_API_CALL(GblITable_typeRegister_(pCtx_));
    GBL_API_CALL(GblIEventHandler_typeRegister_(pCtx_));
    GBL_API_CALL(GblIEventFilter_typeRegister_(pCtx_));
    GBL_API_CALL(GblObject_typeRegister_(pCtx_));
    GBL_API_CALL(GblIAllocator_typeRegister_(pCtx_));
    GBL_API_CALL(GblILogger_typeRegister_(pCtx_));
    GBL_API_CALL(GblContext_typeRegister_(pCtx_));
    GBL_API_CALL(GblEvent_typeRegister_(pCtx_));
    GBL_API_CALL(GblIPlugin_typeRegister_(pCtx_));
    GBL_API_END();
}

void GblType_init_(void) {
    initializing_ = GBL_TRUE;
    GBL_API_BEGIN(pCtx_);
    GBL_API_PUSH_VERBOSE("[GblType]: Initializing.");
    GBL_API_VERIFY(!initialized_, GBL_RESULT_ERROR_INVALID_OPERATION, "Already initialized!");
    if(!inittedOnce_) mtx_init(&typeRegMtx_, mtx_recursive);
    mtx_lock(&typeRegMtx_);
    GBL_API_CALL(GblHashSet_construct(&typeRegistry_,
                                      sizeof(GblMetaClass*),
                                      metaClassHasher_,
                                      metaClassComparator_,
                                      metaClassElementFree_,
                                      initialTypeTotalCount_,
                                      pCtx_));
    GBL_API_CALL(GblVector_construct(&typeBuiltins_.vector,
                                    sizeof(GblMetaClass*),
                                    0,
                                    NULL,
                                    sizeof(typeBuiltins_),
                                    pCtx_));
    GBL_API_CALL(GblVariant_init_(pCtx_));
    GBL_API_CALL(GblType_registerBuiltins_());
    GBL_API_POP(1);
    initialized_    = GBL_TRUE;
    initializing_   = GBL_FALSE;
    inittedOnce_    = GBL_TRUE;
    GBL_API_END_BLOCK();
    mtx_unlock(&typeRegMtx_);
}

// === PUBLIC API ====

GblType GblType_registerBuiltin(GblSize           expectedIndex,
                               GblType            parentType,
                               const char*        pName,
                               const GblTypeInfo* pTypeInfo,
                               GblTypeFlags       flags)
{
    GblType type = GBL_INVALID_TYPE;
    GblSize size = 0;
    GBL_API_BEGIN(pCtx_);
    GBL_TYPE_ENSURE_INITIALIZED_();
    GBL_API_PUSH_VERBOSE("[GblType] Registering Builtin Type: %s", pName);
    flags |= GBL_TYPE_FLAG_BUILTIN;
    type = typeRegister_(parentType, pName, pTypeInfo, flags);
    size = GblVector_size(&typeBuiltins_.vector);
    GBL_API_VERIFY_EXPRESSION(size == expectedIndex+1,
                              "Failed to obtain expected index! "
                              "[expected: %u, actual: %u]", expectedIndex, size-1);
    // create initial/static instance of class if builtin (so that class isn't dynamically created/destroyed)
    {
        const GblTypeInfo* pInfo = GblType_info(type);
        GBL_API_VERIFY_POINTER(pInfo);
        if(pInfo->classSize) {
            //GblClass_ref(type);
        }
    }
    GBL_API_POP(1);
    GBL_API_END_BLOCK();
    return type;
}

GBL_EXPORT GBL_RESULT GblType_final(void) {
    GblBool hasMutex = GBL_FALSE;
    GBL_API_BEGIN(pCtx_);
    GBL_API_PUSH_VERBOSE("[GblType]: Finalizing");
    mtx_lock(&typeRegMtx_);
    hasMutex = GBL_TRUE;

    //iterate over types and cleanup classes
    GBL_API_CALL(GblVariant_final_(pCtx_));
    GBL_API_CALL(GblVector_clear(&typeBuiltins_.vector));
    GBL_API_CALL(GblVector_destruct(&typeBuiltins_.vector));
    GBL_API_CALL(GblHashSet_destruct(&typeRegistry_));

    GBL_API_POP(1);
    initialized_ = GBL_FALSE;
    GBL_API_END_BLOCK();
    if(hasMutex) {
        mtx_unlock(&typeRegMtx_);
        //if(!initialized_) mtx_destroy(&typeRegMtx_); keep to support reinit
    }
    return GBL_API_RESULT();
}

GBL_API GblType_init(GblContext*    pCtx,
                     GblSize        typeBuiltinInitialCount,
                     GblSize        typeTotalInitialCount) {
    if(typeBuiltinInitialCount) initialTypeBuiltinCount_    = typeBuiltinInitialCount;
    if(typeTotalInitialCount)   initialTypeTotalCount_      = typeTotalInitialCount;
    GBL_API_BEGIN(NULL);
    if(!initialized_) {
       // GBL_API_PUSH_VERBOSE("[GblType]: Inititalizing.");
        pCtx_ = pCtx;
        GBL_TYPE_ENSURE_INITIALIZED_();
    } else {
        GBL_API_PUSH_VERBOSE("[GblType]: ReInititalizing.");
        GBL_API_CALL(GblType_final());
        pCtx_ = pCtx;
        GblType_init_();
    }
    GBL_API_END();
}

GBL_EXPORT GblType GblType_fromBuiltinIndex(GblUint index) {
    GblType type = GBL_INVALID_TYPE;
    GBL_TYPE_ENSURE_INITIALIZED_();
    //GBL_API_BEGIN(pCtx_);
    {
        GblType* pType = NULL;
        GblSize registeredCount = GblVector_size(&typeBuiltins_.vector);
        if(index < registeredCount) {
            pType = GblVector_at(&typeBuiltins_.vector, index);
            if(pType)
                type = *pType;
        }
    }
    //GBL_API_END_BLOCK();
    return type;
}

GBL_EXPORT GblType GblType_fromNameQuark(GblQuark name) {
    GblType foundType = GBL_INVALID_TYPE;
    GBL_API_BEGIN(pCtx_);
    if(name == GBL_QUARK_INVALID) GBL_API_DONE();
    GBL_TYPE_ENSURE_INITIALIZED_();

    GblMetaClass* pTempClass = GBL_ALLOCA(sizeof(GblMetaClass));
    pTempClass->name = name;

    mtx_lock(&typeRegMtx_);
    GblMetaClass** ppReturnValue = (GblMetaClass**)GblHashSet_get(&typeRegistry_, &pTempClass);
    if(ppReturnValue) {
        foundType = (GblType)*ppReturnValue;
    }
    mtx_unlock(&typeRegMtx_);

    GBL_API_END_BLOCK();
    return foundType;
}

GBL_EXPORT GblType GblType_fromName(const char* pTypeName) {
    return GblType_fromNameQuark(GblQuark_tryString(pTypeName));
}

GBL_EXPORT GblSize GblType_registeredCount(void) {
    GblSize count = 0;
    GBL_API_BEGIN(pCtx_);
    GBL_TYPE_ENSURE_INITIALIZED_();
    mtx_lock(&typeRegMtx_);
    count = GblHashSet_size(&typeRegistry_);
    mtx_unlock(&typeRegMtx_);
    GBL_API_END_BLOCK();
    return count;
}

GBL_EXPORT GblSize GblType_builtinCount(void) {
    GblSize count = 0;
    GBL_API_BEGIN(pCtx_);
    GBL_TYPE_ENSURE_INITIALIZED_();
    mtx_lock(&typeRegMtx_);
    count = GblVector_size(&typeBuiltins_.vector);
    mtx_unlock(&typeRegMtx_);
    GBL_API_END_BLOCK();
    return count;
}

GBL_EXPORT GblType GblType_nextRegistered(GblType prevType) {
    GblType next = GBL_INVALID_TYPE;
    GBL_API_BEGIN(pCtx_);

    GblBool useNext = (prevType == GBL_INVALID_TYPE);
    for(GblSize p = 0; p < GblHashSet_bucketCount(&typeRegistry_); ++p) {
        GblMetaClass** ppMeta = GblHashSet_probe(&typeRegistry_, p);
        if(ppMeta) {
            GblType type = GBL_TYPE_(*ppMeta);
            if(useNext) {
                next = type;
                break;
            } else if(type == prevType) {
                useNext = GBL_TRUE;
            }
        }
    }
    GBL_API_END_BLOCK();
    return next;
}

GBL_EXPORT GblQuark GblType_nameQuark(GblType type) {
    GblQuark name         = GBL_QUARK_INVALID;
    GblMetaClass* pMeta   = GBL_META_CLASS_(type);
    GBL_API_BEGIN(pCtx_);
    if(pMeta) {
        name = pMeta->name;
    }
    GBL_API_END_BLOCK();
    return name;
}

GBL_EXPORT const char* GblType_name(GblType type) {
    return type != GBL_INVALID_TYPE?
                GblQuark_toString(GblType_nameQuark(type)) :
                "Invalid";
}

GBL_EXPORT GblType GblType_parent(GblType type) {
    GblType parent      = GBL_INVALID_TYPE;
    GblMetaClass* pMeta = GBL_META_CLASS_(type);
    GBL_API_BEGIN(pCtx_);
    if(pMeta) {
        parent = (GblType)pMeta->pParent;
    }
    GBL_API_END_BLOCK();
    return parent;
}

GBL_EXPORT GblType GblType_fundamental(GblType type) {
    GblType base      = GBL_INVALID_TYPE;
    GblMetaClass* pMeta = GBL_META_CLASS_(type);
    GBL_API_BEGIN(pCtx_);
    if(pMeta) {
        if(!pMeta->pParent) base = GBL_TYPE_(pMeta);
        else base = GBL_TYPE_(pMeta->pBases[0]);
    }
    GBL_API_END_BLOCK();
    return base;
} 

GBL_EXPORT GblType GblType_base(GblType type, GblUint depth) {
    GblType base = GBL_INVALID_TYPE;
    GblMetaClass* pMeta =  GBL_META_CLASS_(type);
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

GBL_EXPORT GblType GblType_ancestor(GblType type, GblUint level) {
    GblType ancestor = GBL_INVALID_TYPE;
    GblMetaClass* pMeta =  GBL_META_CLASS_(type);
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

GBL_EXPORT GblUint GblType_depth(GblType type) {
    GblUint depth = 0;
    GBL_API_BEGIN(pCtx_);
    GblMetaClass* pMeta = GBL_META_CLASS_(type);
    if(pMeta) {
        depth = pMeta->depth;
    }
    GBL_API_END_BLOCK();
    return depth;
}

GBL_EXPORT GblBool GblType_flagsTest(GblType type, GblFlags mask) {
    GblBool result = GBL_FALSE;
    //GBL_API_BEGIN(pCtx_);
    {
        GblMetaClass* pMeta = GBL_META_CLASS_(type);
        if(pMeta) {
            GblFlags typeMask = (mask & GBL_TYPE_FLAGS_MASK & pMeta->flags);
            GblFlags fundamentalMask = (mask & GBL_TYPE_FUNDAMENTAL_FLAGS_MASK);
            if(fundamentalMask) {
                GblMetaClass* pFundamental = (GblMetaClass*)GblType_fundamental(type);
                if(pFundamental) {
                    fundamentalMask &= pFundamental->flags;
                } else {
                    fundamentalMask = 0; // FLAGS FOR INVALID TYPE
                }
            }
            result = ((typeMask | fundamentalMask) != 0);
        }
    } //GBL_API_END_BLOCK();
    return result;
}

GBL_EXPORT const GblTypeInfo* GblType_info(GblType type) {
    const GblTypeInfo* pInfo      = NULL;
    GblMetaClass* pMeta = GBL_META_CLASS_(type);
    GBL_API_BEGIN(pCtx_);
    if(pMeta) pInfo = &pMeta->info;
    GBL_API_END_BLOCK();
    return pInfo;
}

GBL_EXPORT GblIPlugin* GblType_plugin(GblType type) {
    return type == GBL_INVALID_TYPE? NULL : GBL_META_CLASS_(type)->pPlugin;
}

GBL_EXPORT GblContext* GblType_contextDefault(void) {
    return pCtx_;
}

static GblBool GblType_typeIsA_(GblType derived, GblType base, GblBool classChecks, GblBool ifaceChecks, GblBool castChecks) {
    GblBool         result   = GBL_FALSE;
    GblMetaClass*   pDerived = GBL_META_CLASS_(derived);
    GblMetaClass*   pBase    = GBL_META_CLASS_(base);
    GblMetaClass*   pIter    = pDerived;

    GBL_API_BEGIN(pCtx_);
    if(derived == GBL_INVALID_TYPE && base == GBL_INVALID_TYPE) {
        result = GBL_TRUE;
    } else if(derived != GBL_INVALID_TYPE && base != GBL_INVALID_TYPE) {
        GBL_ASSERT(GblType_verify(derived));
        GBL_ASSERT(GblType_verify(base));

        // Are we even checking for an interface type!?
        if(!GBL_TYPE_IS_INTERFACED(base) || (pBase->flags & GBL_TYPE_FLAG_UNMAPPABLE))
            ifaceChecks = GBL_FALSE;

        while(pIter) {
            if(pIter == pBase) {    // check if current class level is base
                result = classChecks? GBL_TRUE : GBL_FALSE;
                break;
            } else if(ifaceChecks) {
                // recurse over interfaces checking
                for(GblSize i = 0; i < pIter->info.interfaceCount; ++i) {
                    if(GblType_typeIsA_(pIter->info.pInterfaceMap[i].interfaceType,
                                        base,
                                        GBL_TRUE,
                                        ifaceChecks,
                                        castChecks)) {
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

GBL_EXPORT GblBool GblType_verify(GblType type) {
    GblBool valid = GBL_FALSE;
    if(type != GBL_INVALID_TYPE) {
        const GblQuark nameQuark        = GblType_nameQuark(type);
        const GblType registeredType    = GblType_fromNameQuark(nameQuark);
        if(registeredType != GBL_INVALID_TYPE) {
            valid = GBL_TRUE;
        }
    }
    return valid;
}

GBL_EXPORT GblBool GblType_check(GblType type, GblType other) {
    return GblType_typeIsA_(type, other, GBL_TRUE, GBL_TRUE, GBL_TRUE);
}

// subtyping is inclusive
GBL_EXPORT GblBool GblType_derives(GblType derived, GblType base) {
    if(derived == base) return GBL_FALSE;
    return GblType_typeIsA_(derived, base, GBL_TRUE, GBL_FALSE, GBL_FALSE);
}

GBL_EXPORT GblBool GblType_maps(GblType concrete, GblType iface) {
    if(concrete == GBL_INVALID_TYPE || iface == GBL_INVALID_TYPE) return GBL_FALSE;
    return GblType_typeIsA_(concrete, iface, GBL_FALSE, GBL_TRUE, GBL_FALSE);
}

GBL_EXPORT GblBool GblType_implements(GblType type, GblType superType) {
    return type == superType || GblType_derives(type, superType) || GblType_maps(type, superType);
}

GBL_EXPORT GblType GblType_common(GblType type, GblType other) {
    if(type == GBL_INVALID_TYPE || other == GBL_INVALID_TYPE)
        return GBL_INVALID_TYPE;

    GblBool ifaced = GblType_flagsTest(other, GBL_TYPE_FUNDAMENTAL_FLAG_INTERFACED);

    for(GblUint b = 0; b <= GblType_depth(other); ++b) {
        const GblType otherBase = GblType_base(other, b);
        const GblMetaClass* pOtherBase = GBL_META_CLASS_(otherBase);
        if(GblType_implements(type, otherBase)) {
            if(ifaced && GblType_flagsTest(otherBase, GBL_TYPE_FLAG_UNMAPPABLE))
                return GBL_INVALID_TYPE;
            else
                return otherBase;
        } else if(!(pOtherBase->flags & GBL_TYPE_FLAG_UNMAPPABLE)){
            for(GblSize i = 0; i < pOtherBase->info.interfaceCount; ++i) {
                const GblType otherIface = pOtherBase->info.pInterfaceMap[i].interfaceType;
                const GblType common = GblType_common(type, otherIface);
                if(common != GBL_INVALID_TYPE) {
                    return common;
                }
            }
        }
    }
    return GBL_INVALID_TYPE;
}


GBL_EXPORT GblBool GblType_depends(GblType dependent, GblType dependency) {
    GblBool result = GBL_FALSE;
    GBL_API_BEGIN(pCtx_);
    if(dependent != GBL_INVALID_TYPE && dependency != GBL_INVALID_TYPE) {
        if(GblType_flagsTest(dependent, GBL_TYPE_FUNDAMENTAL_FLAG_DEPENDENT)) {
            for(GblInt a = (GblInt)GblType_depth(dependent); a >= 0; --a) {
                GblType ancestor = GblType_ancestor(dependent, a);
                GblMetaClass* pMeta = GBL_META_CLASS_(ancestor);
                for(GblSize p = 0; p < pMeta->info.dependencyCount; ++p) {
                    if(pMeta->info.pDependencies[p] == dependency ||
                            GblType_depends(pMeta->info.pDependencies[p], dependency))
                    {
                        result = GBL_TRUE;
                        GBL_API_DONE();
                    }
                }
            }
        }
    }
    GBL_API_END_BLOCK();
    return result;
}


GBL_EXPORT GblBool GblType_conforms(GblType type, GblType dependent) {
    return GblType_conforms_(type, dependent, GBL_FALSE);
}

// ==== MAKE SURE TO ITERATE OVER PARENTS AND CHECK THAT CLASS/INSTANCE SIZE IS VALID =====
GBL_EXPORT GblType GblType_registerStatic(GblType                    parent,
                                        const char*                  pName,
                                        const GblTypeInfo*           pInfo,
                                        GblFlags                     flags)
{
    GblType newType = GBL_INVALID_TYPE;
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_POINTER(pName);
    //GBL_API_VERIFY_POINTER(pInfo);

    GBL_API_PUSH_VERBOSE("[GblType] Register Static Type: %s", pName);

    GBL_API_VERIFY_ARG(GblType_fromName(pName) == GBL_INVALID_TYPE,
                        "Existing entry for the given type name was found!");

    GBL_API_VERIFY_ARG(!(flags & GBL_TYPE_FLAG_BUILTIN),
                       "Cannot register a builtin type as a static type!");

    GBL_TYPE_ENSURE_INITIALIZED_();

    newType = typeRegister_(parent, pName, pInfo, flags);
    GBL_API_VERIFY_LAST_RECORD();

    GBL_API_END_BLOCK();
    return newType;
}

GBL_EXPORT GBL_RESULT GblType_unregister(GblType type) {
    GblMetaClass* pMeta = GBL_META_CLASS_(type);
    GBL_API_BEGIN(pCtx_);
    GBL_API_VERIFY_ARG(type != GBL_INVALID_TYPE);
    GBL_API_PUSH_VERBOSE("[GblType] Unregister: %s", GblType_name(type));
    GBL_TYPE_ENSURE_INITIALIZED_();
    {
        GblRefCount refCount = GblType_classRefCount(type);
        if(refCount) {
            GBL_API_WARN("Attempting to unregister type with active class references: %u", refCount);
        }

        refCount = GblType_instanceRefCount(type);
        if(refCount) {
            GBL_API_WARN("Attempting to unregister type with active class references: %u", refCount);
        }

        mtx_lock(&typeRegMtx_);
        const GblBool success = GblHashSet_erase(&typeRegistry_, &pMeta);
        mtx_unlock(&typeRegMtx_);
        GBL_API_VERIFY(success, GBL_RESULT_ERROR_INVALID_TYPE, "Failed to remove the type from the registry HashSet!");
    }
    GBL_API_POP(1);
    GBL_API_END();
}

GBL_EXPORT GblType GblType_fromClass(const GblClass* pClass) {
    GblType type = GBL_INVALID_TYPE;
    //GBL_API_BEGIN(pCtx_);
    if(pClass) {
        type = GBL_CLASS_TYPE_(pClass);
    }
    //GBL_API_END_BLOCK();
    return type;
}

GBL_EXPORT GblRefCount GblType_classRefCount(GblType type) {
    GblRefCount refCount      = 0;
    GblMetaClass* pMeta = GBL_META_CLASS_(type);
    GBL_API_BEGIN(pCtx_);
    if(pMeta) refCount = GBL_ATOMIC_INT16_LOAD(pMeta->refCount);
    GBL_API_END_BLOCK();
    return refCount;
}

GBL_EXPORT GblRefCount GblType_instanceRefCount(GblType type) {
    GblRefCount refCount      = 0;
    if(type != GBL_INVALID_TYPE) {
        GblMetaClass* pMeta = GBL_META_CLASS_(type);
        GBL_API_BEGIN(pCtx_);
        GBL_API_VERIFY_ARG(pMeta != NULL);
        refCount = GBL_ATOMIC_INT16_LOAD(pMeta->instanceRefCount);
        GBL_API_END_BLOCK();
    }
    return refCount;
}

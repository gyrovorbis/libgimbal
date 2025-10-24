#include "gimbal_type_.h"
#include <gimbal/meta/classes/gimbal_class.h>
#include <gimbal/meta/instances/gimbal_instance.h>
#include <gimbal/meta/ifaces/gimbal_interface.h>
#include <gimbal/algorithms/gimbal_hash.h>
#include <gimbal/algorithms/gimbal_sort.h>
#include <gimbal/meta/ifaces/gimbal_ivariant.h>
#include <gimbal/meta/ifaces/gimbal_iplugin.h>
#include <gimbal/containers/gimbal_array_map.h>

GblContext*              pCtx_           = NULL;
once_flag                initOnce_       = ONCE_FLAG_INIT;
GblBool                  inittedOnce_    = GBL_FALSE;
GblBool                  initialized_    = GBL_FALSE;
GBL_THREAD_LOCAL GblBool initializing_   = GBL_FALSE;
mtx_t                    typeRegMtx_;
GblHashSet               typeRegistry_;
struct TypeBuiltins_     typeBuiltins_;


GBL_MAYBE_UNUSED static int metaClassIFaceEntryComparator_(const void* pA, const void* pB) {
    const GblInterfaceImpl* pIFaceA = pA;
    const GblInterfaceImpl* pIfaceB = pB;
    return pIFaceA->interfaceType - pIfaceB->interfaceType;
}

static GblHash metaClassHasher_(const GblHashSet* pMap, const void* pItem) {
    GBL_UNUSED(pMap);
    const GblMetaClass** pMeta = (const GblMetaClass**)pItem;
    return gblHash(&(*pMeta)->name, sizeof(GblQuark));
}

static GblBool metaClassComparator_(const GblHashSet* pMap, const void* pA, const void* pB) {
    GBL_UNUSED(pMap);
    const GblMetaClass** ppRhs = (const GblMetaClass**)pA, **ppLhs = (const GblMetaClass**)pB;
    return (*ppRhs)->name == (*ppLhs)->name;
}

static void metaClassElementFree_(const GblHashSet* pMap, void* item) {
    GBL_UNUSED(pMap);
    GBL_CTX_BEGIN(pCtx_);
    GblMetaClass** ppMetaClass = (GblMetaClass**)item;
    GBL_CTX_FREE(*ppMetaClass);
    GBL_CTX_END_BLOCK();
}

GblInterface* GblType_extension_(GblType type, GblType ifaceType) {
    GblMetaClass* pMeta = GBL_META_CLASS_(type);
    GblInterface* pExt = NULL;

    if(pMeta) {
        pExt = (GblInterface*)GblArrayMap_getValue(&pMeta->pExtensions,
                                                   ifaceType);
    }

    return pExt;
}

static GblIPlugin* GblType_plugin(GblType type) {
    return type == GBL_INVALID_TYPE? NULL : (GblIPlugin*)GblType_extension_(type, GBL_IPLUGIN_TYPE);
}

static GblBool GblType_conforms_(GblType type, GblType dependent, GblBool verify) {
    GblBool conforms = GBL_FALSE;
    GBL_CTX_BEGIN(pCtx_);
    if(dependent != GBL_INVALID_TYPE && type != GBL_INVALID_TYPE) {
        GblBool classConforms   = GBL_TRUE;
        if(GblType_flags(dependent) & GBL_TYPE_ROOT_FLAG_CLASSED) {
            classConforms = GblType_check(type, dependent);
        }

        if(classConforms && (GblType_flags(dependent) & GBL_TYPE_ROOT_FLAG_DEPENDENT)) {
            for(int a = (int)GblType_depth(dependent); a >= 0; --a) {
                GblType ancestor = GblType_ancestor(dependent, a);
                GblMetaClass* pMeta = GBL_META_CLASS_(ancestor);
                for(size_t  p = 0; p < pMeta->pInfo->dependencyCount; ++p) {
                    if(GblType_check(type, pMeta->pInfo->pDependencies[p]) ||
                            GblType_conforms_(type, pMeta->pInfo->pDependencies[p], verify))
                    {
                        continue;
                    } else if(verify) {
                        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_TYPE,
                                           "Type [%s] failed to conform to type [%s] "
                                           "due to not satisfying dependency: [%s]",
                                           GblType_name(type),
                                           GblType_name(dependent),
                                           GblType_name(pMeta->pInfo->pDependencies[p]));
                    } else GBL_CTX_DONE(); //if even one check fails, you're done.
                }
            }
        }

        conforms = classConforms; // oh shit, you made it to the end!

    } else if(dependent == GBL_INVALID_TYPE && type == GBL_INVALID_TYPE) {
        conforms = GBL_TRUE;
    }
    GBL_CTX_END_BLOCK();
    return conforms;
}

static GBL_RESULT typeLog_(GblType parent,
                        const GblTypeInfo* pInfo,
                        GblFlags flags)
{
    GBL_CTX_BEGIN(pCtx_);
    GBL_CTX_PUSH_VERBOSE("Type Info");
    GBL_CTX_VERBOSE("%-20s: %-100.100s", "Parent Type", GblType_name(parent));


    GBL_CTX_PUSH_VERBOSE("flags");
    GBL_CTX_VERBOSE("%-20s: %-100u", "DEPENDENT",       (flags & GBL_TYPE_ROOT_FLAG_DEPENDENT)? 1 : 0);
    GBL_CTX_VERBOSE("%-20s: %-100u", "CLASSED",         (flags & GBL_TYPE_ROOT_FLAG_CLASSED)? 1 : 0);
    GBL_CTX_VERBOSE("%-20s: %-100u", "INSTANTIABLE",    (flags & GBL_TYPE_ROOT_FLAG_INSTANTIABLE)? 1 : 0);
    GBL_CTX_VERBOSE("%-20s: %-100u", "DERIVABLE",       (flags & GBL_TYPE_ROOT_FLAG_DERIVABLE)? 1 : 0);
    GBL_CTX_VERBOSE("%-20s: %-100u", "DEEP_DERIVABLE",  (flags & GBL_TYPE_ROOT_FLAG_DEEP_DERIVABLE)? 1 : 0);
    GBL_CTX_VERBOSE("%-20s: %-100u", "TYPEINFO_STATIC", (flags & GBL_TYPE_FLAG_TYPEINFO_STATIC)? 1 : 0);
    GBL_CTX_VERBOSE("%-20s: %-100u", "CLASS_PINNED",    (flags & GBL_TYPE_FLAG_CLASS_PINNED)? 1 : 0);
    GBL_CTX_VERBOSE("%-20s: %-100u", "CLASS_PREINIT",   (flags & GBL_TYPE_FLAG_CLASS_PREINIT)? 1 : 0);
    GBL_CTX_VERBOSE("%-20s: %-100u", "UNMAPPABLE",      (flags & GBL_TYPE_FLAG_UNMAPPABLE)? 1 : 0);
    GBL_CTX_VERBOSE("%-20s: %-100u", "BUILTIN",         (flags & GBL_TYPE_FLAG_BUILTIN)? 1 : 0);
    GBL_CTX_VERBOSE("%-20s: %-100u", "ABSTRACT",        (flags & GBL_TYPE_FLAG_ABSTRACT)? 1 : 0);
    GBL_CTX_VERBOSE("%-20s: %-100u", "FINAL",           (flags & GBL_TYPE_FLAG_FINAL)? 1 : 0);
    GBL_CTX_POP(1);

    GBL_CTX_PUSH_VERBOSE("Class Info");
    GBL_CTX_VERBOSE("%-20s: %-100u", "size",            pInfo->classSize);
    GBL_CTX_VERBOSE("%-20s: %-100u", "privateSize",     pInfo->classPrivateSize);
    GBL_CTX_VERBOSE("%-20s: %-100p", "data",            pInfo->pClassData);
    GBL_CTX_VERBOSE("%-20s: %-100p", "initializer",     pInfo->pFnClassInit);
    GBL_CTX_VERBOSE("%-20s: %-100p", "finalizer",       pInfo->pFnClassFinal);
    GBL_CTX_POP(1);


    GBL_CTX_PUSH_VERBOSE("Instance Info");
    GBL_CTX_VERBOSE("%-20s: %-100u", "size",            pInfo->instanceSize);
    GBL_CTX_VERBOSE("%-20s: %-100u", "privateSize",     pInfo->instancePrivateSize);
    GBL_CTX_VERBOSE("%-20s: %-100p", "initializer",     pInfo->pFnInstanceInit);
    GBL_CTX_POP(1);

    if(pInfo->interfaceCount) {
        GBL_CTX_PUSH_VERBOSE("Interface Map");
        for(size_t  i = 0; i < pInfo->interfaceCount; ++i) {
            GBL_CTX_VERBOSE("%u: [Type: %s, Class Offset: %u]",
                            i,
                            GblType_name(pInfo->pInterfaceImpls[i].interfaceType),
                            pInfo->pInterfaceImpls[i].classOffset);
        }
        GBL_CTX_POP(1);
    }

    if(pInfo->dependencyCount) {
        GBL_CTX_PUSH_VERBOSE("Dependencies");
        for(size_t  p = 0; p < pInfo->dependencyCount; ++p) {
            GBL_CTX_VERBOSE("%u: [Type: %s]",
                            p,
                            GblType_name(pInfo->pDependencies[p]));

        }
        GBL_CTX_POP(1);
    }
    GBL_CTX_POP(1);
    GBL_CTX_END();
}

static GblFlags typeFlagsWithImplied_(GblType parent,
                                      const GblTypeInfo* pInfo,
                                      GblFlags flags)
{
    GblMetaClass* pParentMeta = GBL_META_CLASS_(parent);
    if(!pParentMeta && pInfo->dependencyCount)              flags |= GBL_TYPE_ROOT_FLAG_DEPENDENT;
    if(!pParentMeta && pInfo->classSize)                    flags |= GBL_TYPE_ROOT_FLAG_CLASSED;
    if(!pParentMeta && pInfo->instanceSize)                 flags |= GBL_TYPE_ROOT_FLAG_INSTANTIABLE;
    if(flags & GBL_TYPE_FLAG_CLASS_PREINIT)                 flags |= GBL_TYPE_FLAG_CLASS_PINNED;
    if(flags & GBL_TYPE_ROOT_FLAG_INTERFACED) {
                                                            flags |= GBL_TYPE_ROOT_FLAG_CLASSED;
                                                            flags |= GBL_TYPE_ROOT_FLAG_DEPENDENT;
    }
    if(flags & GBL_TYPE_ROOT_FLAG_INSTANTIABLE)             flags |= GBL_TYPE_ROOT_FLAG_CLASSED;
    if(flags & GBL_TYPE_ROOT_FLAG_DEEP_DERIVABLE)           flags |= GBL_TYPE_ROOT_FLAG_DERIVABLE;
    return flags;
}

static GBL_RESULT typeValidate_(GblType parent,
                                const GblTypeInfo* pInfo,
                                GblFlags flags)
{
    GblMetaClass* pParentMeta = GBL_META_CLASS_(parent);
    GBL_CTX_BEGIN(pCtx_);

    GblFlags fundFlags = (flags & GBL_TYPE_ROOT_FLAGS_MASK);
    GBL_CTX_VERIFY(!(pParentMeta && fundFlags),
                   GBL_RESULT_ERROR_INVALID_TYPE,
                   "Cannot use FUNDAMENTAL FLAGS on DERIVED type!");

    if(pParentMeta) {
        const GblType fundamentalType = GblType_root(parent);
        fundFlags |= GBL_META_CLASS_(fundamentalType)->flags;
    }
    fundFlags |= flags;

    GBL_CTX_VERIFY(!(pParentMeta && !(fundFlags & GBL_TYPE_ROOT_FLAG_DERIVABLE)),
                   GBL_RESULT_ERROR_INVALID_TYPE,
                   "Cannot DERIVE from a NON-DERIVABLE type!");

    GBL_CTX_VERIFY(!(pParentMeta && GblType_depth(parent) > 0 &&
                     !(fundFlags & GBL_TYPE_ROOT_FLAG_DEEP_DERIVABLE)),
                     GBL_RESULT_ERROR_INVALID_TYPE,
                     "Cannot REDERIVE from a non DEEP DERIVABLE type!");

    GBL_CTX_VERIFY(!(!(fundFlags & GBL_TYPE_ROOT_FLAG_CLASSED) &&
                     (fundFlags & GBL_TYPE_FLAG_CLASS_PINNED)),
                   GBL_RESULT_ERROR_INVALID_TYPE,
                   "Cannot pin an NON CLASSED type!");

    if(fundFlags & GBL_TYPE_ROOT_FLAG_CLASSED) {
        GBL_CTX_VERIFY(pInfo->classSize >= sizeof(GblClass), GBL_RESULT_ERROR_INVALID_TYPE,
                       "Cannot register a CLASSED type with a class size < sizeof(GblClass): [%u]!",
                       sizeof(GblClass));

        GBL_CTX_VERIFY(!(pInfo->pInterfaceImpls && !pInfo->interfaceCount),
                       GBL_RESULT_ERROR_INVALID_TYPE,
                       "Passed non-NULL interface map with an interface count of 0!");
    } else {
        GBL_CTX_VERIFY(pInfo->classSize == 0, GBL_RESULT_ERROR_INVALID_TYPE,
                       "Cannot register a NON CLASSED type with a class size > 0!");
    }

    if(fundFlags & GBL_TYPE_ROOT_FLAG_INTERFACED) {
        GBL_CTX_VERIFY(!(fundFlags & GBL_TYPE_ROOT_FLAG_INSTANTIABLE),
                       GBL_RESULT_ERROR_INVALID_TYPE,
                       "INTERFACED types cannot be INSTANTIABLE!");

        GBL_CTX_VERIFY(pInfo->classSize >= sizeof(GblInterface), GBL_RESULT_ERROR_INVALID_TYPE,
                       "Cannot register an INTERFACED type with a class size < sizeof(GblInterface): [%u]!",
                       sizeof(GblClass));
    }

    if(fundFlags & GBL_TYPE_ROOT_FLAG_INSTANTIABLE) {
        GBL_CTX_VERIFY(pInfo->instanceSize >= sizeof(GblInstance), GBL_RESULT_ERROR_INVALID_TYPE,
                       "Cannot register an INSTANTIABLE type with an instance size < sizeof(GblInstance): [%u]",
                       sizeof(GblInstance));
        GBL_CTX_VERIFY(fundFlags & GBL_TYPE_ROOT_FLAG_CLASSED, GBL_RESULT_ERROR_INVALID_TYPE,
                       "Cannot register an INSTANTIABLE type without it being CLASSED!");
    } else {
        GBL_CTX_VERIFY(pInfo->instanceSize == 0, GBL_RESULT_ERROR_INVALID_TYPE,
                       "Cannot register a NON INSTANTIABLE type with an instance size > 0!");
    }

    GBL_CTX_VERIFY(!(pParentMeta && (pParentMeta->flags & GBL_TYPE_FLAG_FINAL)), GBL_RESULT_ERROR_INVALID_TYPE,
                   "Cannot derive from a FINAL type!");

    if(pInfo->dependencyCount) {
        GBL_CTX_VERIFY(fundFlags & GBL_TYPE_ROOT_FLAG_DEPENDENT,
                       GBL_RESULT_ERROR_INVALID_TYPE,
                       "PREREQUESITES are only supported for DEPENDENT types!");

        size_t  primaryCount = 0;
        for(size_t  p = 0; p < pInfo->dependencyCount; ++p) {
            // have to check whether other dependencies have concrete type
            // also allowed to specify additional specificity on concrete type
            if(!(GblType_flags(pInfo->pDependencies[p]) & GBL_TYPE_ROOT_FLAG_DEPENDENT))
                ++primaryCount;
            else GBL_CTX_VERIFY(GBL_TYPE_DEPENDENT_CHECK(pInfo->pDependencies[p]),
                                GBL_RESULT_ERROR_INVALID_TYPE,
                                "Secondary DEPENDENCIES can only be DEPENDENT types!");
        }
        GBL_CTX_VERIFY(primaryCount <= 1, GBL_RESULT_ERROR_INVALID_TYPE,
                       "Cannot have multiple primary NON-DEPENDENT DEPENDENCY types: [%u]",
                       primaryCount);
    }

    if(pInfo->interfaceCount) {
        GBL_CTX_VERIFY(pInfo->pInterfaceImpls,
                       GBL_RESULT_ERROR_INVALID_TYPE,
                       "NULL pointer passed as interface map!");

        GBL_CTX_VERIFY((fundFlags & GBL_TYPE_ROOT_FLAG_CLASSED),
                       GBL_RESULT_ERROR_INVALID_TYPE,
                       "Cannot specify an INTERFACE MAP on NON-CLASSED fundamental type!");

        // Interface pass 1: validate individual entries
        for(size_t  i = 0; i < pInfo->interfaceCount; ++i) {
            const GblInterfaceImpl* pEntry = &pInfo->pInterfaceImpls[i];
            const GblType ifaceType = pEntry->interfaceType;

            GBL_CTX_VERIFY(GblType_verify(ifaceType),
                           GBL_RESULT_ERROR_INVALID_TYPE,
                           "INTERFACE[%u]: Invalid type!",
                           i);

            GBL_CTX_VERIFY(GblType_flags(ifaceType) & GBL_TYPE_ROOT_FLAG_INTERFACED,
                           GBL_RESULT_ERROR_INVALID_TYPE,
                           "INTERFACE[%u: %s]: Not an INTERFACED fundamental type!",
                           i, GblType_name(ifaceType));

            GblType commonType = GBL_INVALID_TYPE;
            GBL_CTX_VERIFY((commonType = GblType_common(parent, ifaceType)) == GBL_INVALID_TYPE,
                           GBL_RESULT_ERROR_INVALID_TYPE,
                           "INTERFACE[%u: %s]: Forms AMBIGUOUS TYPE[%s] with base class [%s]!",
                           i, GblType_name(ifaceType), GblType_name(commonType), GblType_name(parent));

            GblMetaClass* pIMeta = GBL_META_CLASS_(ifaceType);

            GBL_CTX_VERIFY(pEntry->classOffset + pIMeta->pInfo->classSize <= pInfo->classSize,
                           GBL_RESULT_ERROR_INVALID_TYPE,
                           "INTERFACE[%u: %s]: mapped BEYOND class boundary [%u > %u]!",
                           i, GblType_name(ifaceType), pEntry->classOffset + pIMeta->pInfo->classSize, pInfo->classSize);


            if(pParentMeta) {
                GBL_CTX_VERIFY(pEntry->classOffset >= pParentMeta->pInfo->classSize,
                               GBL_RESULT_ERROR_INVALID_TYPE,
                               "INTERFACE[%u: %s]: offset [%u] maps WITHIN base class [<%u]!",
                               i, GblType_name(ifaceType), pIMeta->pInfo->classSize, pParentMeta->pInfo->classSize);
            }
        }

        // Interfae pass 2: validate relative to each other
        for(size_t  i = 0; i < pInfo->interfaceCount; ++i) {
            const GblInterfaceImpl* pEntry = &pInfo->pInterfaceImpls[i];
            const GblType ifaceType = pEntry->interfaceType;
            GblMetaClass* pIMeta = GBL_META_CLASS_(ifaceType);
            for(size_t  ii = i+1; ii < pInfo->interfaceCount; ++ii) {
                const GblInterfaceImpl* pOtherEntry = &pInfo->pInterfaceImpls[ii];
                const GblType otherIfaceType = pOtherEntry->interfaceType;

                GblType common = GBL_INVALID_TYPE;


                GBL_CTX_VERIFY((common = GblType_common(ifaceType, otherIfaceType)) == GBL_INVALID_TYPE,
                               GBL_RESULT_ERROR_INVALID_TYPE,
                               "INTERFACE[%s] forms AMBIGUOUS TYPE[%s] with INTERFACE[%s]",
                               GblType_name(ifaceType), GblType_name(common), GblType_name(otherIfaceType));

                GBL_CTX_VERIFY(pEntry->classOffset + pIMeta->pInfo->classSize <= pOtherEntry->classOffset,
                               GBL_RESULT_ERROR_INVALID_TYPE,
                               "INTERFACE[%u: %s] data OVERLAPS with INTERFACE[%u: %s]",
                               i, GblType_name(ifaceType), ii, GblType_name(otherIfaceType));
            }
        }

        for(size_t  i = 0; i < pInfo->interfaceCount; ++i) {
            const GblInterfaceImpl* pEntry = &pInfo->pInterfaceImpls[i];
            const GblType ifaceType = pEntry->interfaceType;
            GblMetaClass* pIMeta = GBL_META_CLASS_(ifaceType);

            for(size_t  p = 0; p < pIMeta->pInfo->dependencyCount; ++p) {
                GblType prereqType = pIMeta->pInfo->pDependencies[p];

                GblBool satisfied = GBL_FALSE;
                if(GblType_conforms_(parent, prereqType, GBL_TRUE)) {
                    continue;
                }

                for(size_t  ii = 0; ii < i; ++ii) {
                    GblType prevIFaceType = pInfo->pInterfaceImpls[ii].interfaceType;
                    if(GblType_check(prevIFaceType, prereqType)) {
                        satisfied = GBL_TRUE;
                        break;
                    }

                    for(size_t  pp = 0; pp < p; ++pp) {
                        GblType prevPrereqType = pIMeta->pInfo->pDependencies[pp];
                        if(GblType_check(prevPrereqType, prereqType)) {
                            satisfied = GBL_TRUE;
                            break;
                        }
                    }
                }

                GBL_CTX_VERIFY(satisfied, GBL_RESULT_ERROR_INVALID_TYPE,
                               "Failed to satisfy DEPENDENCY type [%s] on INTERFACE ENTRY[%u] (%s)",
                               GblType_name(prereqType),
                               i,
                               GblType_name(ifaceType));
            }
        }
    }

    GBL_CTX_END();

}

static GBL_RESULT GblType_updateTypeInfoClassChunk_(GblMetaClass* pMeta,
                                                   const GblTypeInfo* pInfo);


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
        .pInterfaceImpls = NULL,
        .dependencyCount = 0,
        .pDependencies = NULL,
        .pFnInstanceInit = NULL,
        .instanceSize = 0,
        .instancePrivateSize = 0
    };
    pInfo = pInfo? pInfo : &defaultTypeInfo;
    GBL_CTX_BEGIN(pCtx_);
    GBL_CTX_VERIFY_POINTER(pName);
    GBL_CTX_VERIFY_POINTER(pInfo);

    GBL_CTX_PUSH_VERBOSE("[GblType] Register Type: %s", pName);

    GBL_CTX_VERIFY(GblType_find(pName) == GBL_INVALID_TYPE,
                   GBL_RESULT_ERROR_INVALID_TYPE,
                   "An existing entry was found with the same type name!");
    GBL_CTX_CALL(typeLog_(parent, pInfo, flags));
    flags = typeFlagsWithImplied_(parent, pInfo, flags);
    GBL_CTX_VERIFY_CALL(typeValidate_(parent, pInfo, flags));


    {
        uint8_t baseCount = 0;
        GblMetaClass* pParentIt = pParent;
        while(pParentIt) {
            ++baseCount;
            pParentIt = pParentIt->pParent;
        }

        const size_t  classPrivateOffset  =  ((!pParent? 0 : pParent->classPrivateOffset) - pInfo->classPrivateSize);

        size_t  metaSize = sizeof(GblMetaClass) + baseCount * sizeof(GblMetaClass*);

        metaSize = gblAlignedAllocSize(metaSize, GBL_META_CLASS_ALIGNMENT_);

        GBL_CTX_PUSH_VERBOSE("MetaClass Info");
        GBL_CTX_VERBOSE("%-20s: %-100u", "total size",      metaSize);
        GBL_CTX_VERBOSE("%-20s: %-100u", "metaclass size",  sizeof(GblMetaClass));
        GBL_CTX_POP(2);

        GblMetaClass* pMeta = GBL_CTX_MALLOC(metaSize,
                                             GBL_META_CLASS_ALIGNMENT_,
                                             pName);

        GBL_CTX_VERIFY_EXPRESSION(!((uintptr_t)pMeta & GBL_CLASS_FLAGS_BIT_MASK_),
                                  "malloc returned a misaligned pointer!");

        memset(pMeta, 0, metaSize);

        atomic_store(&pMeta->refCount, 0);
        atomic_store(&pMeta->instanceRefCount, 0);
        pMeta->flags                = flags;
        if(parent != GBL_INVALID_TYPE)
            pMeta->flags |= (GBL_META_CLASS_(GblType_root(parent))->flags & GBL_TYPE_ROOT_FLAGS_MASK);
        pMeta->depth                = baseCount;
        pMeta->classPrivateOffset   = classPrivateOffset;
        pMeta->instancePrivateOffset= (!pParent? 0 : pParent->instancePrivateOffset) - pInfo->instancePrivateSize;
        pMeta->pParent              = GBL_META_CLASS_(parent);
        pMeta->name                 = GblQuark_fromString(pName);

        pParentIt = pParent;
        baseCount = 0;
        if(pParent) {
            GblNaryTree_addChildFront(&pParent->treeNode, &pMeta->treeNode);

            while(pParentIt) {
                pMeta->pBases[pMeta->depth-(++baseCount)] = pParentIt;
                pParentIt = pParentIt->pParent;
            }
        }
        GBL_CTX_VERIFY_EXPRESSION(baseCount == pMeta->depth);

        if(flags & GBL_TYPE_FLAG_TYPEINFO_STATIC) {
             pMeta->pInfo = pInfo;

        } else {

            GBL_CTX_VERIFY_EXPRESSION(GblType_updateTypeInfoClassChunk_(pMeta, pInfo));
        }

        mtx_lock(&typeRegMtx_);
        hasMutex = GBL_TRUE;

        const GblMetaClass* pOldData = GblHashSet_set(&typeRegistry_,
                                                      &pMeta);

        GBL_CTX_VERIFY(!pOldData, GBL_RESULT_ERROR_INVALID_ARG,
                       "A previous metatype entry named %s existed already!", pName);

        newType = (GblType)pMeta;
        if(flags & GBL_TYPE_FLAG_BUILTIN) {
             GBL_CTX_CALL(GblArrayList_pushBack(&typeBuiltins_.vector, &newType));
        }

        if(flags & GBL_TYPE_FLAG_CLASS_PREINIT) {
            GblClass* pClass = GblClass_refDefault(newType);
            GBL_CTX_VERIFY(pClass,
                           GBL_RESULT_ERROR_INVALID_CLASS,
                           "Class immediate construction failed!");
        }

    }
    GBL_CTX_END_BLOCK();
    if(hasMutex) mtx_unlock(&typeRegMtx_);
    return newType;
}

extern GBL_RESULT GblType_refresh_(GblType type) {
    GBL_CTX_BEGIN(pCtx_);
    if(type != GBL_INVALID_TYPE) {
        GblMetaClass* pMeta   = GBL_META_CLASS_(type);
        GblIPlugin* pPlugin = NULL;//GblType_plugin(type);

        if(!pMeta->pClass || pPlugin) {

            // not sure if needed
            //GBL_CTX_VERIFY_CALL(GblType_refresh_(GBL_TYPE_(pMeta->pParent)));

            GblTypeInfo info;
            if(pPlugin) {
                GblIPluginClass* pIPluginIFace = GBL_IPLUGIN_GET_CLASS(pPlugin);
                pIPluginIFace->pFnTypeInfo(pPlugin, type, &info);
                GBL_CTX_VERIFY_CALL(GblType_updateTypeInfoClassChunk_(pMeta, &info));
            } else {
                GblType_updateTypeInfoClassChunk_(pMeta, pMeta->pInfo);
            }
        }
    }

    GBL_CTX_END();
}

static GBL_RESULT GblType_freeTypeInfoClassChunk_(GblMetaClass* pMeta) {
    GBL_CTX_BEGIN(pCtx_);
    // Determine where the pointer to the previous chunk is and free it.
    if(pMeta->pInfo && !(pMeta->flags & GBL_TYPE_FLAG_TYPEINFO_STATIC)) {
        GBL_CTX_FREE((void*)pMeta->pInfo);
        pMeta->pInfo = NULL;
        pMeta->pClass = NULL;
    } else if(pMeta->pClass) {
        GBL_CTX_FREE((uint8_t*)pMeta->pClass + pMeta->classPrivateOffset);
        pMeta->pClass = NULL;
    }
    GBL_CTX_END();
}

/*  DATA CHUNK LAYOUT
 *
 *  1) TypeInfo Struct   ----++
 *  2) Interface Table        | TypeInfo subchunk
 *  3) Dependency Table  -----+
 *  4) Private class data  ---+  Class subchunk
 *  5) Public class data   ---+
 */
// NOT THREAD SAFE AT ALL YET
static GBL_RESULT GblType_updateTypeInfoClassChunk_(GblMetaClass* pMeta,
                                                   const GblTypeInfo* pInfo)
{
    GBL_CTX_BEGIN(pCtx_);

    GBL_CTX_VERBOSE("[GblType] Refreshing TypeInfo/Class Data Chunk: [%s]", GblQuark_toString(pMeta->name));

    GblType_freeTypeInfoClassChunk_(pMeta);

    // Calculate sizes and offsets for chunk and subchunks contained within it
    size_t  chunkSize               = 0;
    size_t  ifaceOffset             = 0;
    size_t  prereqOffset            = 0;

    // Only include typeinfo within the chunk if it isn't statically allocated
    if(!(pMeta->flags & GBL_TYPE_FLAG_TYPEINFO_STATIC)) {
        ifaceOffset  =  chunkSize += sizeof(GblTypeInfo);
        prereqOffset =  chunkSize += sizeof(GblInterfaceImpl) * pInfo->interfaceCount;
                        chunkSize += sizeof(GblType)                  * pInfo->dependencyCount;
    }

    // Class subchunk is private struct data followed by public struct data
    const size_t  classPublicOffset = chunkSize += -pMeta->classPrivateOffset;
    const size_t  classTotalSize    = pInfo->classSize + -(pMeta->classPrivateOffset);
    chunkSize += classTotalSize;

    // Log stats so we can optimize
    GBL_CTX_PUSH_VERBOSE("TypeInfoClass Chunk Info");
    GBL_CTX_VERBOSE("%-20s: %-100u", "total size",      chunkSize);
    GBL_CTX_VERBOSE("%-20s: %-100u", "TypeInfo size",   sizeof(GblTypeInfo));
    GBL_CTX_VERBOSE("%-20s: %-100u", "Interface size",  sizeof(GblInterfaceImpl) * pInfo->interfaceCount);
    GBL_CTX_VERBOSE("%-20s: %-100u", "Class size",      classTotalSize);
    GBL_CTX_POP(1);

    // Allocate and set chunk data
    uint8_t* pChunk = GBL_CTX_MALLOC(chunkSize);
    memset(pChunk, 0, chunkSize);

    // Typeinfo subchunk is only in when non-static
    if(!(pMeta->flags & GBL_TYPE_FLAG_TYPEINFO_STATIC)) {

        // Copy outer typeinfo struct
        pMeta->pInfo    = (const GblTypeInfo*)pChunk;
        memcpy((void*)pMeta->pInfo, pInfo, sizeof(GblTypeInfo));

        // copy interface tables
        if(pInfo->interfaceCount) {
            ((GblTypeInfo*)pMeta->pInfo)->pInterfaceImpls  =  (GblInterfaceImpl*)((char*)pChunk + ifaceOffset);

            memcpy((GblInterfaceImpl*)pMeta->pInfo->pInterfaceImpls,
                   pInfo->pInterfaceImpls,
                   sizeof(GblInterfaceImpl) * pInfo->interfaceCount);
        }

        // copy dependency tables
        if(pInfo->dependencyCount) {
            ((GblTypeInfo*)pMeta->pInfo)->pDependencies  = (GblType*)((char*)pChunk + prereqOffset);

            memcpy((GblType*)pMeta->pInfo->pDependencies,
                   pInfo->pDependencies,
                   sizeof(GblType*) * pInfo->dependencyCount);
        }
    }

    // set metaclass's class pointer to class subchunk
    pMeta->pClass   = classTotalSize > 0? (GblClass*)(uint8_t*)(pChunk + classPublicOffset) : NULL;
    GBL_CTX_END();
}

static GBL_RESULT GblType_registerBuiltins_(void) {
    GBL_CTX_BEGIN(pCtx_);

    const static GblTypeInfo protocolInfo = { 0 };

    GblType_registerBuiltin_(GBL_TYPE_BUILTIN_INDEX_PROTOCOL,
                             GBL_INVALID_TYPE,
                             GblQuark_internStatic("Protocol"),
                             &protocolInfo,
                             GBL_TYPE_ROOT_FLAG_DEPENDENT |
                             GBL_TYPE_ROOT_FLAG_DERIVABLE |
                             GBL_TYPE_FLAG_TYPEINFO_STATIC);
    GBL_CTX_VERIFY_LAST_RECORD();

    const static GblTypeInfo classInfo = {
        .classSize    = sizeof(GblClass)
    };

    GblType_registerBuiltin_(GBL_TYPE_BUILTIN_INDEX_STATIC_CLASS,
                             GBL_INVALID_TYPE,
                             GblQuark_internStatic("StaticClass"),
                             &classInfo,
                             GBL_TYPE_ROOT_FLAG_CLASSED         |
                             GBL_TYPE_ROOT_FLAG_DEEP_DERIVABLE  |
                             GBL_TYPE_FLAG_TYPEINFO_STATIC      |
                             GBL_TYPE_FLAG_CLASS_PREINIT);
    GBL_CTX_VERIFY_LAST_RECORD();

    const static GblTypeInfo ifaceInfo = {
        .classSize    = sizeof(GblInterface)
    };

    GblType_registerBuiltin_(GBL_TYPE_BUILTIN_INDEX_INTERFACE,
                            GBL_INVALID_TYPE,
                            GblQuark_internStatic("Interface"),
                            &ifaceInfo,
                            GBL_TYPE_ROOT_FLAG_DEPENDENT        |
                            GBL_TYPE_ROOT_FLAG_INTERFACED       |
                            GBL_TYPE_ROOT_FLAG_DEEP_DERIVABLE   |
                            GBL_TYPE_FLAG_ABSTRACT              |
                            GBL_TYPE_FLAG_UNMAPPABLE            |
                            GBL_TYPE_FLAG_TYPEINFO_STATIC       |
                            GBL_TYPE_FLAG_CLASS_PREINIT);
    GBL_CTX_VERIFY_LAST_RECORD();

    const static GblTypeInfo instanceInfo = {
        .classSize    = sizeof(GblClass),
        .instanceSize = sizeof(GblInstance)
    };

    GblType_registerBuiltin_(GBL_TYPE_BUILTIN_INDEX_INSTANCE,
                             GBL_INVALID_TYPE,
                             GblQuark_internStatic("Instance"),
                             &instanceInfo,
                             GBL_TYPE_ROOT_FLAG_DEEP_DERIVABLE  |
                             GBL_TYPE_ROOT_FLAG_INSTANTIABLE    |
                             GBL_TYPE_FLAG_TYPEINFO_STATIC      |
                             GBL_TYPE_FLAG_CLASS_PREINIT);
    GBL_CTX_VERIFY_LAST_RECORD();

    GBL_CTX_CALL(GblIVariant_typeRegister_(pCtx_));
    GBL_CTX_CALL(GblPrimitive_valueTypesRegister_(pCtx_));
    GBL_CTX_CALL(GblPointer_typeRegister_(pCtx_));

    GBL_CTX_END();
}

static void GblType_final_(void) {
    GblBool hasMutex = GBL_FALSE;
    GBL_CTX_BEGIN(pCtx_);
    GBL_CTX_PUSH_VERBOSE("[GblType]: Finalizing");
    mtx_lock(&typeRegMtx_);
    hasMutex = GBL_TRUE;

    //iterate over types and cleanup classes
    GBL_CTX_CALL(GblThread_final_());
    GBL_CTX_CALL(GblModule_final_());
    GBL_CTX_CALL(GblSignal_final_(pCtx_));
    GBL_CTX_CALL(GblProperty_final_(pCtx_));
    GBL_CTX_CALL(GblVariant_final_(pCtx_));
    GBL_CTX_CALL(GblArrayList_clear(&typeBuiltins_.vector));
    GBL_CTX_CALL(GblArrayList_destruct(&typeBuiltins_.vector));
    GBL_CTX_CALL(GblHashSet_destruct(&typeRegistry_));

    GBL_CTX_POP(1);
    initialized_ = GBL_FALSE;
    GBL_CTX_END_BLOCK();
    if(hasMutex) {
        mtx_unlock(&typeRegMtx_);
        //if(!initialized_) mtx_destroy(&typeRegMtx_); keep to support reinit
    }
}

void GblType_init_(void) {
    initializing_ = GBL_TRUE;
    GBL_CTX_BEGIN(pCtx_);
    GBL_CTX_PUSH_VERBOSE("[GblType]: Initializing.");
    GBL_CTX_VERIFY(!initialized_, GBL_RESULT_ERROR_INVALID_OPERATION, "Already initialized!");
    if(!inittedOnce_) mtx_init(&typeRegMtx_, mtx_recursive);
    mtx_lock(&typeRegMtx_);

    GBL_CTX_VERIFY_CALL(GblHashSet_construct(&typeRegistry_,
                                             sizeof(GblMetaClass*),
                                             metaClassHasher_,
                                             metaClassComparator_,
                                             metaClassElementFree_,
                                             GBL_TYPE_REGISTRY_HASH_MAP_CAPACITY_DEFAULT_,
                                             pCtx_));

    GBL_CTX_VERIFY_CALL(GblArrayList_construct(&typeBuiltins_.vector,
                                            sizeof(GblMetaClass*),
                                            0,
                                            NULL,
                                            sizeof(typeBuiltins_),
                                            GBL_FALSE,
                                            pCtx_));

    GBL_CTX_CALL(GblProperty_init_(pCtx_));
    GBL_CTX_CALL(GblSignal_init_(pCtx_));
    GBL_CTX_CALL(GblVariant_init_(pCtx_));
    GBL_CTX_CALL(GblType_registerBuiltins_());

    GBL_CTX_POP(1);
    initialized_    = GBL_TRUE;
    initializing_   = GBL_FALSE;
    inittedOnce_    = GBL_TRUE;
    atexit(GblType_final_);
    GBL_CTX_END_BLOCK();
    mtx_unlock(&typeRegMtx_);
}

// === PUBLIC API ====

GblType GblType_registerBuiltin_(size_t             expectedIndex,
                                 GblType            parentType,
                                 const char*        pName,
                                 const GblTypeInfo* pTypeInfo,
                                 GblTypeFlags       flags)
{
    GblType type = GBL_INVALID_TYPE;
    size_t  size = 0;
    GBL_CTX_BEGIN(pCtx_);
    GBL_TYPE_ENSURE_INITIALIZED_();
    GBL_CTX_PUSH_VERBOSE("[GblType] Registering Builtin Type: %s", pName);


    flags |= GBL_TYPE_FLAG_BUILTIN;
    type = typeRegister_(parentType, pName, pTypeInfo, flags);
    size = GblArrayList_size(&typeBuiltins_.vector);
    GBL_CTX_VERIFY_EXPRESSION(size == expectedIndex+1,
                              "Failed to obtain expected index! "
                              "[expected: %u, actual: %u]", expectedIndex, size-1);
    // create initial/static instance of class if builtin (so that class isn't dynamically created/destroyed)
    {
        const GblTypeInfo* pInfo = GblType_info(type);
        GBL_CTX_VERIFY_POINTER(pInfo);
        if(pInfo->classSize) {
            //GblClass_refDefault(type);
        }
    }
    GBL_CTX_POP(1);
    GBL_CTX_END_BLOCK();
    return type;
}

GBL_EXPORT GblType GblType_fromBuiltinIndex(size_t index) {
    GblType type = GBL_INVALID_TYPE;
    GBL_TYPE_ENSURE_INITIALIZED_();
    {
        GblType* pType = GblArrayList_at(&typeBuiltins_.vector, index);
        if(pType) type = *pType;
    }
    return type;
}

GBL_EXPORT GblType GblType_findQuark(GblQuark name) {
    GblType foundType = GBL_INVALID_TYPE;
    GBL_CTX_BEGIN(pCtx_);
    if(name == GBL_QUARK_INVALID) GBL_CTX_DONE();
    GBL_TYPE_ENSURE_INITIALIZED_();

    GblMetaClass* pTempClass = GBL_ALLOCA(sizeof(GblMetaClass));
    pTempClass->name = name;

    mtx_lock(&typeRegMtx_);
    GblMetaClass** ppReturnValue = (GblMetaClass**)GblHashSet_get(&typeRegistry_, &pTempClass);
    if(ppReturnValue) {
        foundType = (GblType)*ppReturnValue;
    }
    mtx_unlock(&typeRegMtx_);

    GBL_CTX_END_BLOCK();
    return foundType;
}

GBL_EXPORT GblType GblType_find(const char* pTypeName) {
    return GblType_findQuark(GblQuark_tryString(pTypeName));
}

GBL_EXPORT size_t  GblType_count(void) {
    size_t  count = 0;
    GBL_CTX_BEGIN(pCtx_);
    GBL_TYPE_ENSURE_INITIALIZED_();
    mtx_lock(&typeRegMtx_);
    count = GblHashSet_size(&typeRegistry_);
    mtx_unlock(&typeRegMtx_);
    GBL_CTX_END_BLOCK();
    return count;
}

GBL_EXPORT size_t  GblType_builtinCount(void) {
    size_t  count = 0;
    GBL_CTX_BEGIN(pCtx_);
    GBL_TYPE_ENSURE_INITIALIZED_();
    mtx_lock(&typeRegMtx_);
    count = GblArrayList_size(&typeBuiltins_.vector);
    mtx_unlock(&typeRegMtx_);
    GBL_CTX_END_BLOCK();
    return count;
}

GBL_EXPORT GblType GblType_next(GblType prevType) {
    GblType next = GBL_INVALID_TYPE;
    GBL_CTX_BEGIN(pCtx_);

    GblBool useNext = (prevType == GBL_INVALID_TYPE);
    for(size_t  p = 0; p < GblHashSet_bucketCount(&typeRegistry_); ++p) {
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
    GBL_CTX_END_BLOCK();
    return next;
}

GBL_EXPORT GblQuark GblType_nameQuark(GblType type) {
    GblQuark name         = GBL_QUARK_INVALID;
    GblMetaClass* pMeta   = GBL_META_CLASS_(type);
    if(pMeta) {
        name = pMeta->name;
    }
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
    GBL_CTX_BEGIN(pCtx_);
    if(pMeta) {
        parent = GBL_TYPE_(pMeta->pParent);
    }
    GBL_CTX_END_BLOCK();
    return parent;
}

GBL_EXPORT GblType GblType_root(GblType type) {
    GblType base        = GBL_INVALID_TYPE;
    GblMetaClass* pMeta = GBL_META_CLASS_(type);
    if(pMeta) {
        if(!pMeta->pParent) base = GBL_TYPE_(pMeta);
        else base = GBL_TYPE_(pMeta->pBases[0]);
    }
    return base;
} 

GBL_EXPORT GblType GblType_base(GblType type, size_t depth) {
    GblType base        = GBL_INVALID_TYPE;
    GblMetaClass* pMeta = GBL_META_CLASS_(type);

    if(pMeta) {
        if(depth < pMeta->depth) {
            base = (GblType)pMeta->pBases[depth];
        } else if(depth == pMeta->depth) {
            base = (GblType)pMeta;
        }
    }

    return base;
}

GBL_EXPORT GblType GblType_ancestor(GblType type, size_t level) {
    GblType ancestor = GBL_INVALID_TYPE;
    GblMetaClass* pMeta =  GBL_META_CLASS_(type);
    GBL_CTX_BEGIN(pCtx_);
    if(pMeta) {
        if(level == 0) {
            ancestor = (GblType)pMeta;
        } else if(level <= pMeta->depth) {
            ancestor = (GblType)pMeta->pBases[pMeta->depth-level];
        }
    }
    GBL_CTX_END_BLOCK();
    return ancestor;
}

GBL_EXPORT size_t  GblType_depth(GblType type) {
    size_t  depth = 0;
    GBL_CTX_BEGIN(pCtx_);
    GblMetaClass* pMeta = GBL_META_CLASS_(type);
    if(pMeta) {
        depth = pMeta->depth;
    }
    GBL_CTX_END_BLOCK();
    return depth;
}

GBL_EXPORT GblFlags GblType_flags(GblType type) {
    GblMetaClass* pMeta = GBL_META_CLASS_(type);

    if(pMeta)
        return pMeta->flags;

    return 0;
}

GBL_EXPORT const GblTypeInfo* GblType_info(GblType type) {
    GblMetaClass* pMeta = GBL_META_CLASS_(type);

    if(pMeta)
        return pMeta->pInfo;

    return NULL;
}

// If this ever needs to be optimized, make it non-recursive
static GblBool GblType_typeIsA_(GblType derived, GblType base, GblBool classChecks, GblBool ifaceChecks, GblBool castChecks) {
    GblBool         result   = GBL_FALSE;
    GblMetaClass*   pDerived = GBL_META_CLASS_(derived);
    GblMetaClass*   pBase    = GBL_META_CLASS_(base);
    GblMetaClass*   pIter    = pDerived;

    if(derived == GBL_INVALID_TYPE && base == GBL_INVALID_TYPE) {
        result = GBL_TRUE;
    } else if(derived != GBL_INVALID_TYPE && base != GBL_INVALID_TYPE) {
        GBL_ASSERT(GblType_verify(derived));
        GBL_ASSERT(GblType_verify(base));

        // Are we even checking for an interface type!?
        if(!GBL_TYPE_INTERFACED_CHECK(base) || (pBase->flags & GBL_TYPE_FLAG_UNMAPPABLE))
            ifaceChecks = GBL_FALSE;

        while(pIter) {
            if(pIter == pBase) {    // check if current class level is base
                result = classChecks? GBL_TRUE : GBL_FALSE;
                break;
            } else if(ifaceChecks) {
                // recurse over interfaces checking
                for(size_t i = 0; i < pIter->pInfo->interfaceCount; ++i) {
                    if(GblType_typeIsA_(pIter->pInfo->pInterfaceImpls[i].interfaceType,
                                        base,
                                        GBL_TRUE,
                                        ifaceChecks,
                                        castChecks)) {
                        result = GBL_TRUE;
                        goto done;
                    }
                }
            }
            pIter = pIter->pParent;
        }
    }
    done:
    return result;
}

GBL_EXPORT GblBool GblType_verify(GblType type) {
    GblBool valid = GBL_FALSE;
    if(type != GBL_INVALID_TYPE) {
        const GblQuark nameQuark     = GblType_nameQuark(type);
        const GblType registeredType = GblType_findQuark(nameQuark);
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

    GblBool ifaced = !!(GblType_flags(other) & GBL_TYPE_ROOT_FLAG_INTERFACED);

    for(size_t  b = 0; b <= GblType_depth(other); ++b) {
        const GblType otherBase = GblType_base(other, b);
        const GblMetaClass* pOtherBase = GBL_META_CLASS_(otherBase);
        if(GblType_implements(type, otherBase)) {
            if(ifaced && (GblType_flags(otherBase) & GBL_TYPE_FLAG_UNMAPPABLE))
                return GBL_INVALID_TYPE;
            else
                return otherBase;
        } else if(!(pOtherBase->flags & GBL_TYPE_FLAG_UNMAPPABLE)) {
            for(size_t  i = 0; i < pOtherBase->pInfo->interfaceCount; ++i) {
                const GblType otherIface = pOtherBase->pInfo->pInterfaceImpls[i].interfaceType;
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
    GBL_CTX_BEGIN(pCtx_);
    if(dependent != GBL_INVALID_TYPE && dependency != GBL_INVALID_TYPE) {
        if(GblType_flags(dependent) & GBL_TYPE_ROOT_FLAG_DEPENDENT) {
            for(int a = (int)GblType_depth(dependent); a >= 0; --a) {
                GblType ancestor = GblType_ancestor(dependent, a);
                GblMetaClass* pMeta = GBL_META_CLASS_(ancestor);
                for(size_t  p = 0; p < pMeta->pInfo->dependencyCount; ++p) {
                    if(pMeta->pInfo->pDependencies[p] == dependency ||
                            GblType_depends(pMeta->pInfo->pDependencies[p], dependency))
                    {
                        result = GBL_TRUE;
                        GBL_CTX_DONE();
                    }
                }
            }
        }
    }
    GBL_CTX_END_BLOCK();
    return result;
}


GBL_EXPORT GblBool GblType_conforms(GblType type, GblType dependent) {
    return GblType_conforms_(type, dependent, GBL_FALSE);
}

// ==== MAKE SURE TO ITERATE OVER PARENTS AND CHECK THAT CLASS/INSTANCE SIZE IS VALID =====
GBL_EXPORT GblType GblType_register(const char*                  pName,
                                    GblType                      parent,
                                    const GblTypeInfo*           pInfo,
                                    GblFlags                     flags)
{
    GblType newType = GBL_INVALID_TYPE;
    GBL_CTX_BEGIN(pCtx_);
    GBL_CTX_VERIFY_POINTER(pName);
    //GBL_CTX_VERIFY_POINTER(pInfo);

    GBL_CTX_PUSH_VERBOSE("[GblType] Register Static Type: %s", pName);

    GBL_CTX_VERIFY_ARG(GblType_find(pName) == GBL_INVALID_TYPE,
                        "Existing entry for the given type name was found!");

    GBL_CTX_VERIFY_ARG(!(flags & GBL_TYPE_FLAG_BUILTIN),
                       "Cannot register a builtin type as a static type!");

    GBL_TYPE_ENSURE_INITIALIZED_();

    newType = typeRegister_(parent, pName, pInfo, flags);
    GBL_CTX_VERIFY_LAST_RECORD();

    GBL_CTX_END_BLOCK();
    return newType;
}

GBL_EXPORT GBL_RESULT GblType_unregister(GblType type) {
    GblMetaClass* pMeta = GBL_META_CLASS_(type);
    GBL_CTX_BEGIN(pCtx_);
    GBL_CTX_VERIFY_ARG(type != GBL_INVALID_TYPE);
    GBL_CTX_PUSH_VERBOSE("[GblType] Unregister: %s", GblType_name(type));
    GBL_TYPE_ENSURE_INITIALIZED_();
    {
        GblRefCount refCount = GblType_classRefCount(type);
        if(refCount) {
            GBL_CTX_WARN("Attempting to unregister type with active class references: %u", refCount);
        }

        refCount = GblType_instanceCount(type);
        if(refCount) {
            GBL_CTX_WARN("Attempting to unregister type with active class references: %u", refCount);
        }

        GblNaryTree_disconnect(&pMeta->treeNode);

        GBL_CTX_VERIFY_CALL(GblType_freeTypeInfoClassChunk_(pMeta));
        mtx_lock(&typeRegMtx_);

        const GblBool success = GblHashSet_erase(&typeRegistry_, &pMeta);
        //GblHashSet_shrinkToFit(&typeRegistry_);
        mtx_unlock(&typeRegMtx_);
        GBL_CTX_VERIFY(success, GBL_RESULT_ERROR_INVALID_TYPE, "Failed to remove the type from the registry HashSet!");
    }
    GBL_CTX_POP(1);
    GBL_CTX_END();
}

GBL_EXPORT GblRefCount GblType_classRefCount(GblType type) {
    GblRefCount refCount      = 0;
    GblMetaClass* pMeta = GBL_META_CLASS_(type);
    GBL_CTX_BEGIN(pCtx_);
    if(pMeta) refCount = atomic_load(&pMeta->refCount);
    GBL_CTX_END_BLOCK();
    return refCount;
}

GBL_EXPORT GblRefCount GblType_instanceCount(GblType type) {
    GblRefCount refCount      = 0;
    if(type != GBL_INVALID_TYPE) {
        GblMetaClass* pMeta = GBL_META_CLASS_(type);
        GBL_CTX_BEGIN(pCtx_);
        GBL_CTX_VERIFY_ARG(pMeta != NULL);
        refCount = atomic_load(&pMeta->instanceRefCount);
        GBL_CTX_END_BLOCK();
    }
    return refCount;
}

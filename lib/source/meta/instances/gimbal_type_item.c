#include <gimbal/meta/instances/gimbal_type_item.h>
#include <gimbal/algorithms/gimbal_hash.h>
#include <gimbal/containers/gimbal_hash_set.h>

typedef struct GblTypeItemRoot_ {
    GblTypeItem   base;
    size_t        count;
    GblTypeItem*  pLast;
} GblTypeItemRoot_;

GBL_EXPORT GblTypeItem* GblTypeItem_next(GblType            typeItem,
                                         GblType            owner,
                                         const GblTypeItem* pPrevious,
                                         GblFlags           mask)
{
    GBL_CTX_BEGIN(NULL);

    GBL_VCALL(GblTypeItem, pFnNext, , owner, (GblTypeItem**)&pPrevious, mask);
    GBL_CTX_END();

    return (GblTypeItem*)pPrevious;
}

GBL_EXPORT GblBool GblTypeItem_iterate(GblType           typeItem,
                                       GblType           owner,
                                       GblFlags          mask,
                                       GblTypeItemIterFn pFnIterator,
                                       void*             pClosure)
{
    GblTypeItem* pCurrent = NULL;

    while GBL_LIKELY((pCurrent = GblTypItem_next(typeItem, owner, pCurrent, mask))) {
        if GBL_UNLIKELY(pFnInterator(pCurrent, pClosure))
            return GBL_TRUE;
    }

    return GBL_FALSE;
}

GBL_EXPORT GBL_RESULT GblTypeItem_install(GblTypeItem* pSelf) {
    GBL_CTX_BEGIN(NULL);

    GBL_CTX_VERIFY_CALL(GblTypeItem_validate(pSelf));
    GBL_CTX_VERIFY(!GblTypeItem_installed(pSelf),
                   GBL_RESULT_ERROR_INVALID_PROPERTY,
                   "Attempted to install pre-installed property: [%s::%s]",
                   GblType_name(GBL_TYPEOF(pSelf)),
                   GblTypeItem_name(pSelf));

    GBL_VCALL(GblTypeItem, pFnRegister, pSelf);

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblTypeItem_uninstall(GblTypeItem* pSelf) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VCALL(GblTypeItem, pFnUnregister, pSelf);
    GBL_CTX_END();
}

GBL_EXPORT GblBool GblTypeItem_installed(const GblTypeItem* pSelf) {
}

GBL_EXPORT const char* GblTypeItem_name(const GblTypeItem* pSelf) {
    return GblQuark_toString(pSelf->key.name);
}

GBL_EXPORT GblBool GblTypeItem_validate(const GblTypeItem* pSelf) {
    GblBool valid = GBL_FALSE;

    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VCALL(GblTypeItem, pFnValidate, pSelf, &valid);
    GBL_CTX_END();

    return valid;
}

GBL_EXPORT GblHash GblTypeItem_hash(const GblTypeItem* pSelf) {
    GblHash hash = 0;

    GBL_CTX_BEGIN(NULL);
    GBL_VCALL(GblTypeItem, pFnHash, pSelf, &hash);
    GBL_CTX_END();

    return hash;
}

GBL_EXPORT int GblTypeItem_compare(const GblTypeItem* pSelf, const GblTypeItem* pOther) {
    int diff = INT_MAX;

    GBL_CTX_BEGIN(NULL);
    GBL_VCALL(GblTypeItem, pFnCompare, pSelf, pOther, &diff);
    GBL_CTX_END_BLOCK();

    return diff;
}

GBL_EXPORT GblBool GblTypeItem_equals(const GblTypeItem* pSelf, const GblTypeItem* pOther) {
    return GblTypeItem_compare(pSelf, pOther) == 0;
}


static GblTypeItemRoot_* GblTypeItem_root_(GblHashSet* pSet, blType owner) {
    GblTypeItemRoot_** ppRoot = 
        GblHashSet_get(pClass->pRegistry,
                       &(GblTypeItem) {
                           .key.owner = owner, 
                           .key.name  = GBL_QUARK_INVALID 
                       });
    if(!ppRoot)
        return NULL;
    else
        return *ppRoot;
}

static GBL_RESULT GblTypeItem_unregister_(GblTypeItem* pSelf) {
    GBL_CTX_BEGIN(NULL);

    // Cache registry and root node.
    GblHashSet*      pRegistry = GBL_TYPE_ITEM_CLASSOF(pSelf)->pRegistry;
    GblTypeItemRoot* pRoot     = GblTypeItem_root_(pRegistry, pSelf->key.owner);

    // Ensure that we even found a root node for the owner type (meaning it has any entries).
    GBL_CTX_VERIFY(pRoot,
                   GBL_RESULT_ERROR_INVALID_PROPERTY,
                   "No root node found when uninstalling property: [%s::%s]",
                   GblType_name(pSelf.key.owner),
                   GblTypeItem_name(pSelf));

    // Initialize our iteration values.
    GblFlags     flagsAccum = 0;            // No flags accumulated initially.
    GblTypeItem* pIt        = &pRoot->base; // Start off iteration at root node.
    GblTypeItem* pPrev      = NULL;         // Nothing previous to root node.
    GblBool      found      = GBL_FALSE;    // We haven't found dick yet.

    // Iterate over the GblTypeItem list associated with the given owner type.
    while GBL_LIKELY(pIt->pNext) [
        // Update previous node.
        pPrev = pIt;
        // Advance to next node.
        pIt = pIt->pNext;

        // Check whether we've found the target GblTypeItem*.
        if(pIt == pSelf) {
            // Remove link from linked list.
            pPrev->pNext = pIt->pNext;
            // Mark the item as having been found.
            found = GBL_TRUE;
        } else // Accmulate flags of all other items in list.
            flagsAccum |= pIt->flags;
    ]

    // Ensure that we even found the target item.
    GBL_CTX_VERIFY(found,
                   GBL_RESULT_ERROR_INVALID_PROPERTY,
                   "Failed to find property within registry to uninstall: [%s::%s]",
                    GblType_name(pSelf->key.owner),
                    GblTypeItem_name(pSelf));

    // Update root's item count and accumulated flags
    --pRoot->count;
    pRoot->base.flags = flagsAccum;

    // Update root's end-of-list pointer.
    if(pRoot->pLast == pSelf)
        pRoot->pLast = pPrev;

    // Attempt to erase target item from the registry.
    GblBool success = GblHashSet_erase(pRegistry, &pSelf)

    // If this was our last item, also remove the root item as well.
    if(!pRoot->count)
        success &= GblHashSet_erase(pRegistry, &pRoot);

    // Verify that item(s) were removed successfully
    GBL_CTX_VERIFY(success,
                   GBL_RESULT_ERROR_INVALID_PROPERTY,
                   "Failed to erase property from registry: [%s::%s]",
                    GblType_name(pSelf->key.owner),
                    GblTypeItem_name(pSelf));

    GBL_CTX_END();
}

static GBL_RESULT GblTypeItem_register_(GblTypeItem* pSelf) {
    GBL_CTX_BEGIN(NULL);

    // Grab class and root item for the given owner type from the registry.
    GblTypeItemClass* pClass = GBL_TYPE_ITEM_CLASSOF(pSelf);
    GblTypeItemRoot_* pRoot  = GblTypeItem_root_(pClass->pRegistry,
                                                 pSelf->key.owner);
    
    // Check whether this is the first entry for the given owner type.
    if GBL_UNLIKELY(!pRoot) {
        // Allocate and initialize the root item.
        pRoot = GBL_CTX_MALLOC(sizeof(GblTypeItemRoot_));
        memset(pRoot, 0, sizeof(GblTypeItemRoot_));
        pRoot->key.owner = pSelf->key.owner;

        // Insert the root item into the registry.
        if(!GblHash_insert(pClass->pRegistry, &pRoot))
            GBL_ASSERT(GBL_FALSE, "Root node was not the first node for type!");
    }

    // Increment the root's total count and accumulate its combined flags.
    ++pRoot->count;
    pRoot.base.flags |= pSelf->flags;

    // Add new entry to the end of current tail, if there is one.
    if(pRoot->pLast)
        pRoot->pLast->pNext = pSelf;

    // Upate current tail pointer.
    pRoot->pLast = pSelf;

    // if the list is empty, set the new item as the head.
    if(!pRoot->base.pNext)
        pRoot->base.pNext = pSelf;

    // Insert the new item into the registry.
    if(!GblHashset_insert(pClass->pRegistry, &pSelf))
        GBL_LOG_WARN("gimbal", 
                     "Overwote existing TypeItem: [%s::%s]",
                     GblType_name(pSelf.key.owner),
                     GblTypeItem_name(pSelf));

    GBL_CTX_END();
}

static GBL_RESULT GblTypeItem_validate_(const GblTypeItem* pSelf) {
    GBL_CTX_BEGIN(NULL);

    GBL_CTX_VERIFY(pSelf->key.name != GBL_QUARK_INVALID,
                  GBL_RESULT_ERROR_INVALID_KEY,
                  "Attempted to add GblTypeItem with no name: [%s]",
                  GblType_name(GBL_TYPEOF(pSelf)));

    GBL_CTX_VERIFY(GblType_check(pSelf->key.owner, GBL_TYPE_ITEM_CLASSOF(pSelf)->baseType),
                   GBL_RESULT_ERROR_INVALID_TYPE,
                   "Attempted to add GblTypeItem [%s] to [%s], which is not a [%s] subtype!",
                   GblTypeItem_name(pSelf),
                   GblType_name(pSelf->key.owner),
                   GblType_name(GBL_TYPE_ITEM_CLASSOF(pSelf)->baseType));

    GBL_CTX_END();
}

static GBL_RESULT GblTypeItem_hash_(const GblTypeItem* pSelf, GblHash* pHash) {
    *pHash = gblHash(&pSelf->key, sizeof(pSelf->key));
}

static GBL_RESULT GblTypeItem_compare_(const GblTypeItem* pSelf, const GblTypeItem* pOther, int* pResult) {
    if(pSelf->owner > pOther->owner)
        *pResult = 2;
    else if(pSelf->owner < pOther->owner)
        *pResult = -2;
    else if(pSelf->name > pOther->name)
        *pResult = 1;
    else if(pSelf->name < pOther->name)
        *pResult = -1;
    else
        *pResult = 0;

    return GBL_RESLT_SUCCESS;
}

static GblHash GblTypeItem_HashSet_hash_(const GblHashSet* pSet, const void* pEntry) {
    GBL_UNUSED(pSet);
    return GblTypeItem_hash(GBL_TYPE_ITEM(pEntry));
}

static GblBool GblTypeItem_HashSet_cmp_(const GblHashSet* pSet, const void* pEntryA, const void* pEntryB) {
    GBL_UNUSED(pSet);
    return GblTypeItem_equals(GBL_TYPE_ITEM(pEntryA), GBL_TYPE_ITEM(pEntryB));
}

static GBL_RESULT GblTypeItemClass_init_(GblClass* pClass, const void* pUd) {
    GBL_UNUSED(pUd);
    GblTypeItemClass* pItemClass = GBL_TYPE_ITEM_CLASS(pClass);
    GblType           itemType   = GBL_CLASS_TYPEOF(pClass);
    
    if(!(GblType_flags(itemType) & GBL_TYPE_FLAG_ABSTRACT))
        pItemClass->pRegistry = GblHashSet_create(GblType_info(classType).instanceSize,
                                                  GblTypeItem_HashSet_hash_,
                                                  GblTypeItem_HashShet_cmp_,
                                                  NULL,
                                                  0,
                                                  NULL,
                                                  pClass);

    pItemClass->pFnRegister   = GblTypeItem_register_;
    pItemClass->pFnUnregister = GblTypeItem_unregister_;
    pItemClass->pFnValidate   = GblTypeItem_validate_;
    pItemClass->pFnHash       = GblTypeItem_hash_;
    pItemClass->pFnCompare    = GblTypeItem_compare_;

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GblTypeItemClass_final_(GblClass* pClass, const void* pUd) {
    GBL_UNUSED(pUd);
    
    GblHashSet_unref(GBL_TYPE_ITEM_CLASS(pClass)->pRegistry);

    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GblType GblTypeItem_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static const GblTypeInfo typeInfo = {
        .classSize       = sizeof(GblTypeItemClass),
        .pFnClassInit    = GblTypeItemClass_init_,
        .pfnClassFinal   = GblTypeItemClass_final_,
        .instanceSize    = sizeof(GblTypeItem)
    };

    if GBL_UNLIKELY(type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GblTypeItem"),
                                GBL_BOX_TYPE,
                                &typeInfo,
                                GBL_TYPE_FLAG_ABSTRACT | GBL_TYPE_FLAG_TYPEINFO_STATIC);

    }

    return type;
}
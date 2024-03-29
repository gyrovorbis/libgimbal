// IMPLEMENTATION BASED ON BTREE.C:
// Copyright 2020 Joshua J Baker. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.

#include <gimbal/containers/gimbal_tree_set.h>
#include <gimbal/core/gimbal_ctx.h>

#define GBL_TREE_SET_SPARE_COUNT_(p)         (sizeof(p->pSpares) / sizeof(void*))
#define GBL_TREE_SET_MIN_COUNT_(max)         (max * 40/100)


static GBL_RESULT grow_group_(GblTreeSet* pSelf, GblTreeSetGroup* pGroup) {
    GBL_CTX_BEGIN(pSelf->pCtx);

    const size_t  capacity = pGroup->capacity ? pGroup->capacity*2 : 1;
    GblTreeSetNode** ppNodes = GBL_CTX_MALLOC(sizeof(GblTreeSetNode*)*capacity);

    memcpy(ppNodes, pGroup->ppNodes, pGroup->length*sizeof(GblTreeSetNode*));
    GBL_CTX_FREE(pGroup->ppNodes);
    pGroup->ppNodes = ppNodes;
    pGroup->capacity = capacity;
    GBL_CTX_END();
}


static GBL_RESULT takeaway_(GblTreeSet* pSelf, GblTreeSetNode* pNode) {
    const size_t  MAXLEN = 32;
    GblTreeSetGroup* pGroup = NULL;
    GBL_CTX_BEGIN(pSelf->pCtx);
    if(pNode->leaf) {
        pGroup = &pSelf->pool.leaves;
    } else {
        pGroup = &pSelf->pool.branches;
    }
    if(pGroup->length == MAXLEN) {
        GBL_CTX_FREE(pNode);
    } else {
        if (pGroup->length == pGroup->capacity) {
            GBL_CTX_CALL(grow_group_(pSelf, pGroup));
        }
        pGroup->ppNodes[pGroup->length++] = pNode;
    }
    GBL_CTX_END();
}

static GblTreeSetNode *node_new_(GblTreeSet* pSelf, GblBool leaf) {
    GblTreeSetNode* pNode = NULL;
    GBL_CTX_BEGIN(pSelf->pCtx);
    size_t  size = sizeof(GblTreeSetNode);
    if (!leaf) {
        size += sizeof(GblTreeSetNode*) * pSelf->maxCount;
    }
    size_t  entryOffset = size;
    size += pSelf->entrySize * (pSelf->maxCount-1);
    pNode = GBL_CTX_MALLOC(size);
    pNode->leaf = leaf;
    pNode->entryCount = 0;
    pNode->pEntries = (uint8_t*)pNode + entryOffset;
    GBL_CTX_END_BLOCK();
    return pNode;
}

// fill_pool fills the node pool prior to inserting items. This ensures there
// is enough memory before we begin doing to things like splits and tree
// rebalancing. There needs to be at least one available leaf and N branches
// where N is equal to the height of the tree.
static GBL_RESULT fill_pool_(GblTreeSet* pSelf) {
    GBL_CTX_BEGIN(pSelf->pCtx);
    if(!pSelf->pool.leaves.length) {
        if(!pSelf->pool.leaves.capacity) {
            GBL_CTX_CALL(grow_group_(pSelf, &pSelf->pool.leaves));
        }
        GblTreeSetNode* pLeaf = node_new_(pSelf, GBL_TRUE);
        pSelf->pool.leaves.ppNodes[pSelf->pool.leaves.length++] = pLeaf;
    }
    while(pSelf->pool.branches.length < pSelf->height) {
        if (pSelf->pool.branches.length == pSelf->pool.branches.capacity) {
            GBL_CTX_CALL(grow_group_(pSelf, &pSelf->pool.branches));
        }
        GblTreeSetNode* pBranch = node_new_(pSelf, GBL_FALSE);
        pSelf->pool.branches.ppNodes[pSelf->pool.branches.length++] = pBranch;
    }
    GBL_CTX_END();
}

static GBL_RESULT node_free_(GblTreeSet* pSelf, GblTreeSetNode* pNode) {
    GBL_CTX_BEGIN(pSelf->pCtx);
    if(!pNode->leaf) {
        for(uint16_t e = 0; e < pNode->entryCount; ++e) {
            node_free_(pSelf, pNode->pChildren[e]);
        }
        node_free_(pSelf, pNode->pChildren[pNode->entryCount]);
    }
    if(pSelf->pFnDestruct) {
        for(uint16_t e = 0; e < pNode->entryCount; ++e) {
            uint8_t* pEntry = (uint8_t*)pNode->pEntries + e*pSelf->entrySize;
            pSelf->pFnDestruct(pSelf, pEntry);
        }
    }
    GBL_CTX_FREE(pNode);
    GBL_CTX_END();
}

static GBL_RESULT release_pool_(GblTreeSet* pSelf) {
    GBL_CTX_BEGIN(pSelf->pCtx);
    for (size_t  i = 0; i < pSelf->pool.leaves.length; ++i) {
        GBL_CTX_FREE(pSelf->pool.leaves.ppNodes[i]);
    }
    GBL_CTX_FREE(pSelf->pool.leaves.ppNodes);
    for (size_t  i = 0; i < pSelf->pool.branches.length; ++i) {
        GBL_CTX_FREE(pSelf->pool.branches.ppNodes[i]);
    }
    GBL_CTX_FREE(pSelf->pool.branches.ppNodes);
    memset(&pSelf->pool, 0, sizeof(GblTreeSetPool));
    GBL_CTX_END();
}

GBL_INLINE void reset_load_fields_(GblTreeSet* pSelf) {
    pSelf->pLastEntry = NULL;
    pSelf->pLastNode= NULL;
}

GBL_INLINE GblTreeSetNode* gimme_node_(GblTreeSet* pSelf, GblTreeSetGroup* pGroup) {
    GBL_CTX_BEGIN(pSelf->pCtx);
    GBL_CTX_VERIFY_EXPRESSION(pGroup->length != 0, "[GblTreeSet]: Out of nodes!");
    GBL_CTX_END_BLOCK();
    return pGroup->ppNodes[--pGroup->length];
}

GBL_INLINE GblTreeSetNode* gimme_leaf_(GblTreeSet* pSelf) {
    return gimme_node_(pSelf, &pSelf->pool.leaves);
}

GBL_INLINE GblTreeSetNode* gimme_branch_(GblTreeSet* pSelf) {
    return gimme_node_(pSelf, &pSelf->pool.branches);
}

GBL_INLINE void* get_item_at_(GblTreeSetNode* pNode, size_t  entrySize, size_t  index) {
    return (uint8_t*)pNode->pEntries + entrySize*index;
}

GBL_INLINE void set_item_at_(GblTreeSetNode* pNode, size_t  entrySize, size_t  index, const void* pEntry) {
    memcpy(get_item_at_(pNode, entrySize, index), pEntry, entrySize);
}


GBL_INLINE void copy_item_into_(GblTreeSetNode* pNode, size_t  entrySize, size_t  index,
                           void* pInto)
{
    memcpy(pInto, get_item_at_(pNode, entrySize, index), entrySize);
}

GBL_INLINE void copy_item_(GblTreeSetNode* pNodeA, size_t  entrySize, size_t  indexA,
                                   GblTreeSetNode* pNodeB, size_t  indexB)
{
    memcpy(get_item_at_(pNodeA, entrySize, indexA),
           get_item_at_(pNodeB, entrySize, indexB), entrySize);
}

GBL_INLINE void swap_item_at_(GblTreeSetNode* pNode, size_t  entrySize, size_t  index,
                         const void* pEntry, void* pInto)
{
    void *pPtr = get_item_at_(pNode, entrySize, index);
    memcpy(pInto, pPtr, entrySize);
    memcpy(pPtr, pEntry, entrySize);
}

static int node_find_(const GblTreeSet* pSelf, GblTreeSetNode* pNode, const void* pKey,
                     GblBool* pFound, uint64_t* pHint, int depth)
{
    int low = 0;
    int high = pNode->entryCount - 1;
    if(pHint && depth < 8) {
        int index = ((uint8_t*)pHint)[depth];
        if(index > 0) {
            if(index > pNode->entryCount-1) {
                index = pNode->entryCount - 1;
            }
            void* pItem = get_item_at_(pNode, pSelf->entrySize, (size_t )index);
            const int cmp = pSelf->pFnCompare(pSelf, pKey, pItem);
            if(cmp == 0) {
                *pFound = GBL_TRUE;
                return index;
            }
            if (cmp > 0) {
                low = index + 1;
            } else {
                high = index - 1;
            }
        }
    }
    int index;
    while (low <= high) {
        const int mid = (low + high) / 2;
        void* pItem = get_item_at_(pNode, pSelf->entrySize, (size_t )mid);
        const int cmp = pSelf->pFnCompare(pSelf, pKey, pItem);
        if(cmp == 0) {
            *pFound = GBL_TRUE;
            index = mid;
            goto done;
        }
        if(cmp < 0) {
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }
    *pFound = GBL_FALSE;
    index = low;
done:
    if (pHint && depth < 8) {
        ((uint8_t*)pHint)[depth] = (uint8_t)index;
    }
    return index;
}

static void node_split_(GblTreeSet* pSelf, GblTreeSetNode* pNode,
                       GblTreeSetNode** ppRight, void** ppMedian, GblBool leanLeft)
{
    int mid;
    if(leanLeft) {
        // Split so the left node has as many items as possible, leaving the
        // new right with the minimum items. This makes more space available to
        // the right node for sequential inserts and bulk loading.
        mid = (int)(pSelf->maxCount - 1 - pSelf->minCount);
        const int mdif = (pNode->entryCount-(mid+1))-(int)pSelf->minCount;
        if(mdif < 0) {
            mid += mdif;
        }
    } else {
        // split so that both left and right have the same number of items.
        mid = (int)(pSelf->maxCount-1)/2;
    }
    *ppMedian   = get_item_at_(pNode, pSelf->entrySize, (size_t)mid);
    *ppRight    = pNode->leaf ? gimme_leaf_(pSelf) : gimme_branch_(pSelf);
    (*ppRight)->leaf = pNode->leaf;
    (*ppRight)->entryCount = pNode->entryCount - ((uint16_t)mid+1);
    memmove((*ppRight)->pEntries,
            (uint8_t*)pNode->pEntries + (int)pSelf->entrySize*(mid+1),
            (size_t)(*ppRight)->entryCount * pSelf->entrySize);
    if(!pNode->leaf) {
        for(int e = 0; e <= (*ppRight)->entryCount; ++e) {
            (*ppRight)->pChildren[e] = pNode->pChildren[mid+1+e];
        }
    }
    pNode->entryCount = (uint16_t)mid;
}

static void node_join_(GblTreeSetNode* pLeft, GblTreeSetNode* pRight, size_t  entrySize) {
    memcpy((uint8_t*)pLeft->pEntries + entrySize*(size_t)pLeft->entryCount,
           pRight->pEntries,
           (size_t)pRight->entryCount * entrySize);
    if(!pLeft->leaf) {
        memcpy(&pLeft->pChildren[pLeft->entryCount],
               &pRight->pChildren[0],
               (size_t)(pRight->entryCount+1)*sizeof(GblTreeSetNode*));
    }
    pLeft->entryCount += pRight->entryCount;
}


GBL_INLINE void node_shift_right_(GblTreeSetNode* pNode, size_t  entrySize, size_t  index) {
    memmove((uint8_t*)pNode->pEntries + entrySize*(index+1),
            (uint8_t*)pNode->pEntries + entrySize*index,
            ((size_t)pNode->entryCount-index)*entrySize);

    if (!pNode->leaf) {
        memmove(&pNode->pChildren[index+1],
                &pNode->pChildren[index],
                ((size_t)pNode->entryCount - index+1)*sizeof(GblTreeSetNode*));
    }
    ++pNode->entryCount;
}

GBL_INLINE void node_shift_left_(GblTreeSetNode* pNode, size_t  entrySize, size_t index,
                            GblBool forMerge)
{
    memmove((uint8_t*)pNode->pEntries + entrySize*index,
            (uint8_t*)pNode->pEntries + entrySize*(index+1),
            ((size_t)pNode->entryCount-index)*entrySize);
    if (!pNode->leaf) {
        if (forMerge) {
            ++index;
        }
        memmove(&pNode->pChildren[index],
                &pNode->pChildren[index+1],
                ((size_t)pNode->entryCount-index+1)*sizeof(GblTreeSetNode*));
    }
    --pNode->entryCount;
}

GBL_INLINE GblBool node_set_(GblTreeSet* pSelf, GblTreeSetNode* pNode, const void* pEntry,
                     GblBool leanLeft, uint64_t* pHint, int depth)
{
    GblBool found = GBL_FALSE;
    int i = node_find_(pSelf, pNode, pEntry, &found, pHint, depth);
    if(found) {
        swap_item_at_(pNode, pSelf->entrySize, (size_t )i, pEntry, pSelf->pSpares[0]);
        return GBL_TRUE;
    }
    else if(pNode->leaf) {
        node_shift_right_(pNode, pSelf->entrySize, (size_t )i);
        set_item_at_(pNode, pSelf->entrySize, (size_t )i, pEntry);
        return GBL_FALSE;
    }
    else if(node_set_(pSelf, pNode->pChildren[i], pEntry, leanLeft, pHint, depth+1)) {
        return GBL_TRUE;
    }
    else if((size_t )pNode->pChildren[i]->entryCount == (pSelf->maxCount-1)) {
        void*           pMedian = NULL;
        GblTreeSetNode* pRight  = NULL;
        node_split_(pSelf, pNode->pChildren[i], &pRight, &pMedian, leanLeft);
        node_shift_right_(pNode, pSelf->entrySize, (size_t )i);
        set_item_at_(pNode, pSelf->entrySize, (size_t )i, pMedian);
        pNode->pChildren[i+1] = pRight;
    }
    return GBL_FALSE;
}

static void* btree_set_x_(GblTreeSet* pSelf, const void* pEntry, GblBool leanLeft, uint64_t *pHint)
{
    void* pRetValue = NULL;
    GBL_CTX_BEGIN(pSelf->pCtx); {
        reset_load_fields_(pSelf);

        GBL_CTX_VERIFY_POINTER(pEntry);
        GBL_CTX_CALL(fill_pool_(pSelf));

        if(!pSelf->pRoot) {
            pSelf->pRoot = gimme_leaf_(pSelf);
            set_item_at_(pSelf->pRoot, pSelf->entrySize, 0, pEntry);
            pSelf->pRoot->entryCount = 1;
            pSelf->count++;
            pSelf->height++;
            pRetValue = NULL;
            GBL_CTX_DONE();
        }
        if (node_set_(pSelf, pSelf->pRoot, pEntry, leanLeft, pHint, 0)) {
            pRetValue = pSelf->pSpares[0];
            GBL_CTX_DONE();
        }
        ++pSelf->count;
        if ((size_t )pSelf->pRoot->entryCount == (pSelf->maxCount-1)) {
            void *pOldRoot          = pSelf->pRoot;
            GblTreeSetNode *pRight  = NULL;
            void *pMedian           = NULL;
            node_split_(pSelf, pOldRoot, &pRight, &pMedian, leanLeft);
            pSelf->pRoot = gimme_branch_(pSelf);
            pSelf->pRoot->pChildren[0] = pOldRoot;
            set_item_at_(pSelf->pRoot, pSelf->entrySize, 0, pMedian);
            pSelf->pRoot->pChildren[1] = pRight;
            pSelf->pRoot->entryCount = 1;
            ++pSelf->height;
        }
    } GBL_CTX_END_BLOCK();
    return pRetValue;
}

// btree_get_hint is the same as btree_get except that an optional "hint" can
// be provided which may make the operation quicker when done as a batch or
// in a userspace context.
void *btree_get_hint_(const GblTreeSet* pSelf, const void* pKey, uint64_t* pHint) {
    GblTreeSetNode* pNode = pSelf->pRoot;
    if(!pNode) {
        return NULL;
    }
    for(int depth = 0; ; ++depth) {
        GblBool found = GBL_FALSE;
        const int i = node_find_(pSelf, pNode, pKey, &found, pHint, depth);
        if(found) {
            return get_item_at_(pNode, pSelf->entrySize, (size_t )i);
        }
        if(pNode->leaf) {
            return NULL;
        }
        pNode = pNode->pChildren[i];
    }
}


GBL_EXPORT GBL_RESULT GblTreeSet_construct_7(GblTreeSet* pSelf,
                               size_t                    entrySize,
                               GblTreeSetCompareFn  pFnCompare,
                               GblTreeSetDestructFn pFnDestruct,
                               size_t                    maxEntries,
                               GblContext*               pCtx,
                               void*                     pUserdata) GBL_NOEXCEPT
{
    GBL_CTX_BEGIN(pCtx);

    if (maxEntries == 0) {
        maxEntries = 256;
    } else {
        if (maxEntries % 2 == 1) maxEntries--;
        if (maxEntries < 4)      maxEntries = 4;
        if (maxEntries > 4096)   maxEntries = 4096;
    }
    GBL_CTX_VERIFY_ARG(entrySize != 0);
    GBL_CTX_VERIFY_POINTER(pFnCompare);
    memset(pSelf, 0, sizeof(GblTreeSet));

    const int spareCount = GBL_TREE_SET_SPARE_COUNT_(pSelf);
    for(int s = 0; s < spareCount; ++s) {
        pSelf->pSpares[s] = GBL_CTX_MALLOC(entrySize);
    }

   pSelf->pCtx          = pCtx;
   pSelf->pFnCompare    = pFnCompare;
   pSelf->pFnDestruct   = pFnDestruct;
   pSelf->maxCount      = maxEntries;
   pSelf->minCount      = GBL_TREE_SET_MIN_COUNT_(maxEntries);
   pSelf->entrySize     = entrySize;
   pSelf->pUserdata     = pUserdata;

   GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblTreeSet_construct_6(GblTreeSet* pSelf,
                               size_t                    entrySize,
                               GblTreeSetCompareFn  pFnCompare,
                               GblTreeSetDestructFn pFnDestruct,
                               size_t                    maxEntries,
                               GblContext*               pCtx) GBL_NOEXCEPT
{
    return GblTreeSet_construct_7(pSelf,
                                  entrySize,
                                  pFnCompare,
                                  pFnDestruct,
                                  maxEntries,
                                  pCtx,
                                  NULL);
}


GBL_EXPORT GBL_RESULT GblTreeSet_construct_5(GblTreeSet* pSelf,
                               size_t                    entrySize,
                               GblTreeSetCompareFn  pFnCompare,
                               GblTreeSetDestructFn pFnDestruct,
                               size_t                    maxEntries) GBL_NOEXCEPT
{
    return GblTreeSet_construct_6(pSelf,
                                  entrySize,
                                  pFnCompare,
                                  pFnDestruct,
                                  maxEntries,
                                  NULL);
}

GBL_EXPORT GBL_RESULT GblTreeSet_construct_4(GblTreeSet* pSelf,
                               size_t                    entrySize,
                               GblTreeSetCompareFn  pFnCompare,
                               GblTreeSetDestructFn pFnDestruct) GBL_NOEXCEPT
{
    return GblTreeSet_construct_5(pSelf,
                                  entrySize,
                                  pFnCompare,
                                  pFnDestruct,
                                  0);
}

GBL_EXPORT GBL_RESULT GblTreeSet_construct_3(GblTreeSet* pSelf,
                               size_t                    entrySize,
                               GblTreeSetCompareFn  pFnCompare) GBL_NOEXCEPT
{
    return GblTreeSet_construct_4(pSelf,
                                  entrySize,
                                  pFnCompare,
                                  NULL);
}


GBL_EXPORT void GblTreeSet_clear(GblTreeSet* pSelf) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(pSelf->pCtx);
    if(pSelf->pRoot) {
        GBL_CTX_CALL(node_free_(pSelf, pSelf->pRoot));
    }
    GBL_CTX_CALL(release_pool_(pSelf));
    pSelf->pRoot = NULL;
    pSelf->count = 0;
    reset_load_fields_(pSelf);
    GBL_CTX_END_BLOCK();
}

GBL_EXPORT GBL_RESULT GblTreeSet_destruct(GblTreeSet* pSelf) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(pSelf->pCtx);
    //GBL_CTX_FREE(pSelf->pRoot);

    GblTreeSet_clear(pSelf);

    const int spareCount = GBL_TREE_SET_SPARE_COUNT_(pSelf);
    for (int s = 0; s < spareCount; ++s) {
        GBL_CTX_FREE(pSelf->pSpares[s]);
    }
    GBL_CTX_END();
}

GBL_EXPORT void* GblTreeSet_set(GblTreeSet* pSelf, const void* pEntry) GBL_NOEXCEPT {
    return GblTreeSet_setHint(pSelf, pEntry, NULL);
}

GBL_EXPORT void* GblTreeSet_setHint(GblTreeSet* pSelf, const void* pEntry, uint64_t* pHint) GBL_NOEXCEPT {
    void* pExisting = NULL;
    GBL_CTX_BEGIN(pSelf->pCtx);
    GBL_CTX_VERIFY_POINTER(pEntry);
    pExisting = btree_set_x_(pSelf, pEntry, GBL_FALSE, pHint);
    GBL_CTX_END_BLOCK();
    return pExisting;
}

GBL_EXPORT void* GblTreeSet_get(const GblTreeSet* pSelf, const void* pKey) GBL_NOEXCEPT {
    return GblTreeSet_getHint(pSelf, pKey, NULL);
}

GBL_EXPORT void* GblTreeSet_getHint(const GblTreeSet* pSelf, const void* pKey, uint64_t* pHint) GBL_NOEXCEPT {
    void* pEntry = NULL;
    GBL_CTX_BEGIN(pSelf->pCtx);
    GBL_CTX_VERIFY_POINTER(pKey);
    pEntry = btree_get_hint_(pSelf, pKey, pHint);
    GBL_CTX_END_BLOCK();
    return pEntry;
}

GBL_EXPORT void* GblTreeSet_at(const GblTreeSet* pSelf, const void* pKey) GBL_NOEXCEPT {
    return GblTreeSet_atHint(pSelf, pKey, NULL);
}

GBL_EXPORT void* GblTreeSet_atHint(const GblTreeSet* pSelf, const void* pKey, uint64_t* pHint) GBL_NOEXCEPT {
    void* pEntry = NULL;
    GBL_CTX_BEGIN(pSelf->pCtx);
    GBL_CTX_VERIFY_POINTER(pKey);
    pEntry = GblTreeSet_getHint(pSelf, pKey, pHint);
    GBL_CTX_VERIFY(pEntry,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_END_BLOCK();
    return pEntry;
}

GBL_EXPORT GblBool GblTreeSet_contains(const GblTreeSet* pSelf, const void* pKey) GBL_NOEXCEPT {
    return GblTreeSet_get(pSelf, pKey) != NULL;
}

GBL_EXPORT size_t  GblTreeSet_count(const GblTreeSet* pSelf, const void* pKey) GBL_NOEXCEPT {
    return GblTreeSet_contains(pSelf, pKey)? 1 : 0;
}


enum delact {
    DELKEY, POPFRONT, POPBACK, POPMAX,
};

static GblBool node_delete_(GblTreeSet* pSelf, GblTreeSetNode* pNode, enum delact act,
                        size_t  index, const void* pKey, void* pPrev, uint64_t* pHint, int depth)
{
    int i = 0;
    GblBool found = GBL_FALSE;
    switch(act) {
    case POPMAX:
        i = pNode->entryCount-1;
        found = GBL_TRUE;
        break;
    case POPFRONT:
        i = 0;
        found = pNode->leaf;
        break;
    case POPBACK:
        if (!pNode->leaf) {
            i = pNode->entryCount;
            found = GBL_FALSE;
        } else {
            i = pNode->entryCount-1;
            found = GBL_TRUE;
        }
        break;
    case DELKEY:
        i = node_find_(pSelf, pNode, pKey, &found, pHint, depth);
        break;
    }
    if(pNode->leaf) {
        if(found) {
            // item was found in leaf, copy its contents and delete it.
            copy_item_into_(pNode, pSelf->entrySize, (size_t )i, pPrev);
            node_shift_left_(pNode, pSelf->entrySize, (size_t )i, GBL_FALSE);
            return GBL_TRUE;
        }
        return GBL_FALSE;
    }
    // branch
    GblBool deleted = GBL_FALSE;
    if(found) {
        if(act == POPMAX) {
            // popping off the max item into into its parent branch to maintain
            // a balanced tree.
            ++i;
            node_delete_(pSelf, pNode->pChildren[i], POPMAX, 0, NULL,  pPrev, pHint,
                        depth+1);
            deleted = GBL_TRUE;
        } else {
            // item was found in branch, copy its contents, delete it, and
            // begin popping off the max items in child nodes.
            copy_item_into_(pNode, pSelf->entrySize, (size_t )i, pPrev);
            node_delete_(pSelf, pNode->pChildren[i], POPMAX, 0, NULL,
                        pSelf->pSpares[2], pHint, depth+1);
            set_item_at_(pNode, pSelf->entrySize, (size_t )i, pSelf->pSpares[2]);
            deleted = GBL_TRUE;
        }
    } else {
        // item was not found in this branch, keep searching.
        deleted = node_delete_(pSelf, pNode->pChildren[i], act, index, pKey, pPrev,
                              pHint, depth+1);
    }
    if (!deleted) {
        return GBL_FALSE;
    }

    if ((size_t )pNode->pChildren[i]->entryCount >= pSelf->minCount) {
        return GBL_TRUE;
    }

    if (i == pNode->entryCount) {
        --i;
    }

    GblTreeSetNode* pLeft   = pNode->pChildren[i];
    GblTreeSetNode* pRight  = pNode->pChildren[i+1];

    if((size_t )(pLeft->entryCount + pRight->entryCount + 1) < (pSelf->maxCount - 1)) {
        // merge left + item + right
        copy_item_(pLeft, pSelf->entrySize, (size_t )pLeft->entryCount, pNode, (size_t )i);
        ++pLeft->entryCount;
        node_join_(pLeft, pRight, pSelf->entrySize);
        takeaway_(pSelf, pRight);
        node_shift_left_(pNode, pSelf->entrySize, (size_t )i, GBL_TRUE);
    } else if (pLeft->entryCount > pRight->entryCount) {
        // move left -> right
        node_shift_right_(pRight, pSelf->entrySize, 0);
        copy_item_(pRight, pSelf->entrySize, 0, pNode, (size_t )i);
        if(!pLeft->leaf) {
            pRight->pChildren[0] = pLeft->pChildren[pLeft->entryCount];
        }
        copy_item_(pNode, pSelf->entrySize, (size_t )i, pLeft, (size_t )(pLeft->entryCount - 1));
        if (!pLeft->leaf) {
            pLeft->pChildren[pLeft->entryCount] = NULL;
        }
        --pLeft->entryCount;
    } else {
        // move right -> left
        copy_item_(pLeft, pSelf->entrySize, (size_t )pLeft->entryCount, pNode, (size_t )i);
        if (!pLeft->leaf) {
            pLeft->pChildren[pLeft->entryCount+1] = pRight->pChildren[0];
        }
        ++pLeft->entryCount;
        copy_item_(pNode, pSelf->entrySize, (size_t )i, pRight, 0);
        node_shift_left_(pRight, pSelf->entrySize, 0, GBL_FALSE);
    }
    return deleted;
}


static void* delete_x_(GblTreeSet* pSelf, enum delact act, size_t  index,
                      const void* pKey, uint64_t* pHint)
{
    reset_load_fields_(pSelf);

    if(!pSelf->pRoot) {
        return NULL;
    }
    GblBool deleted = node_delete_(pSelf, pSelf->pRoot, act, index, pKey,
                               pSelf->pSpares[0], pHint, 0);
    if(!deleted) {
        return NULL;
    }
    if(pSelf->pRoot->entryCount == 0) {
        GblTreeSetNode* pOldRoot = pSelf->pRoot;
        if(!pSelf->pRoot->leaf) {
            pSelf->pRoot = pSelf->pRoot->pChildren[0];
        } else {
            pSelf->pRoot = NULL;
        }
        takeaway_(pSelf, pOldRoot);
        --pSelf->height;
    }
    --pSelf->count;
    return pSelf->pSpares[0];
}


GBL_EXPORT void* GblTreeSet_extract(GblTreeSet* pSelf,  const void* pKey) GBL_NOEXCEPT {
    void* pEntry = NULL;
    GBL_CTX_BEGIN(pSelf->pCtx);
    GBL_CTX_VERIFY_POINTER(pKey);
    pEntry = delete_x_(pSelf, DELKEY, 0, pKey, NULL);
    GBL_CTX_END_BLOCK();
    return pEntry;
}

GBL_EXPORT GblBool GblTreeSet_erase(GblTreeSet* pSelf, const void* pKey) GBL_NOEXCEPT {
    void*   pEntry = NULL;
    GBL_CTX_BEGIN(pSelf->pCtx);
    pEntry = GblTreeSet_extract(pSelf, pKey);
    if(pEntry) {
        if(pSelf->pFnDestruct) {
            pSelf->pFnDestruct(pSelf, pEntry);
        }
    }
    GBL_CTX_END_BLOCK();
    return pEntry != NULL;
}

GBL_EXPORT size_t  GblTreeSet_size(const GblTreeSet* pSelf) {
    return pSelf->count;
}

GBL_EXPORT size_t  GblTreeSet_height(const GblTreeSet* pSelf) {
    return pSelf->height;
}

GBL_EXPORT GblContext* GblTreeSet_context(const GblTreeSet* pSelf) {
    return pSelf->pCtx;
}

GBL_EXPORT GblBool GblTreeSet_empty(const GblTreeSet* pSelf) {
    return pSelf->count ? GBL_FALSE : GBL_TRUE;
}

GBL_EXPORT void* GblTreeSet_userdata(const GblTreeSet* pSelf) {
    return pSelf->pUserdata;
}


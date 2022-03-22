#ifndef GIMBAL_BTREE_H
#define GIMBAL_BTREE_H

#include <btree.c/btree.h>
#include <time.h>
#include "gimbal_container.h"
#include "../gimbal_api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GblBTree {
    GblContext      hCtx;
    struct btree*   pImpl_;
} GblBTree;


GBL_INLINE void* gblBTreeAlloc_(size_t bytes) {
    GBL_API_BEGIN(NULL);
    return GBL_API_MALLOC(bytes);
}

GBL_INLINE void* gblBTreeRealloc_(void* pData, size_t bytes) {
    GBL_API_BEGIN(NULL);
    return GBL_API_REALLOC(pData, bytes);
}

GBL_INLINE void gblBTreeFree_(void* pData) {
    GBL_API_BEGIN(NULL);
    GBL_API_FREE(pData);
}


GBL_INLINE GBL_RESULT gblBTreeOom(GblBTree* pTree, GblBool* pResult) {
    GBL_ASSERT(pTree);
    GBL_API_BEGIN(pTree->hCtx);
    GBL_API_VERIFY_POINTER(pResult);
    btree_oom(pTree->pImpl_);
    GBL_API_END();
}

#define GBL_API_VERIFY_BTREE(btree)                                 \
    GBL_STMT_START {                                                \
        GblBool oom = GBL_FALSE;                                    \
        GBL_API_CALL(gblBTreeOom(btree, &oom));                     \
        GBL_API_VERIFY(!oom, GBL_RESULT_ERROR_MEM_ALLOC,            \
                       "Btree just returned out of memory!!");      \
    } GBL_STMT_END

GBL_INLINE GBL_RESULT gblBtreeInit(GblBTree* pTree, GblContext hCtx, GblSize elSize, GblSize maxItems,
                                   int (*pFnCompare)(const void *a, const void *b, void *udata))
{
    GBL_API_BEGIN(hCtx);
    GBL_API_POINTER_VERIFY(pTree);
    memset(pTree, 0, sizeof(GblBTree));
    pTree->hCtx = hCtx;

    pTree->pImpl_ = btree_new_with_allocator(&gblBTreeAlloc_, &gblBTreeRealloc_, &gblBTreeFree_,
                                             elSize, maxItems, pFnCompare);
    GBL_API_VERIFY_POINTER(pTree->pImpl_);
    GBL_API_VERIFY_BTREE(pTree);
    GBL_API_END();
}

GBL_INLINE GBL_RESULT gblBTreeFinalize(GblBTree* pTree) {
    GBL_ASSERT(pTree);
    GBL_API_BEGIN(pTree->hCtx);
    btree_free(pTree);
    GBL_API_END();
}

GBL_INLINE GBL_RESULT gblBTreeHeight(GblBTree* pTree, GblSize* pHeight) {
    GBL_ASSERT(pTree);
    GBL_API_BEGIN(pTree->hCtx);
    GBL_API_VERIFY_POINTER(pHeight);
    *pHeight = btree_height(pTree->pImpl_);
    GBL_API_END();
}

GBL_INLINE GBL_RESULT gblBTreeCount(GblBTree* pTree, GblSize* pCount) {
    GBL_ASSERT(pTree);
    GBL_API_BEGIN(pTree->hCtx);
    GBL_API_VERIFY_POINTER(pCount);
    *pCount = btree_count(pTree->pImpl_);
    GBL_API_END();
}

GBL_INLINE GBL_RESULT gblBTreeSet(GblBTree* pTree, void* pItem, void** ppResult) {
    GBL_ASSERT(pTree);
    GBL_API_BEGIN(pTree->hCtx);
    GBL_API_VERIFY_POINTER(pItem);
    GBL_API_VERIFY_POINTER(ppResult);
    *ppResult = btree_set(pTree->pImpl_, pItem);
    GBL_API_VERIFY_BTREE(pTree);
    GBL_API_END();
}

GBL_INLINE GBL_RESULT gblBTreeGet(GblBTree* pTree, void* pKey, void** ppResult) {
    GBL_ASSERT(pTree);
    GBL_API_BEGIN(pTree->hCtx);
    GBL_API_VERIFY_POINTER(pKey);
    GBL_API_VERIFY_POINTER(ppResult);
    *ppResult = btree_get(pTree->pImpl_, pKey);
    GBL_API_END();
}

GBL_INLINE GBL_RESULT gblBTreeDelete(GblBTree* pTree, void* pKey, void** ppResult) {
    GBL_ASSERT(pTree);
    GBL_API_BEGIN(pTree->hCtx);
    GBL_API_VERIFY_POINTER(pKey);
    GBL_API_VERIFY_POINTER(ppResult);
    *ppResult = btree_delete(pTree->pImpl_, pKey);
    GBL_API_END();
}

GBL_INLINE GBL_RESULT gblBTreePopMin(GblBTree* pTree, void** ppResult) {
    GBL_ASSERT(pTree);
    GBL_API_BEGIN(pTree->hCtx);
    GBL_API_VERIFY_POINTER(ppResult);
    *ppResult = btree_pop_min(pTree->pImpl_);
    GBL_API_END();
}

GBL_INLINE GBL_RESULT gblBTreePopMax(GblBTree* pTree, void** ppResult) {
    GBL_ASSERT(pTree);
    GBL_API_BEGIN(pTree->hCtx);
    GBL_API_VERIFY_POINTER(ppResult);
    *ppResult = btree_pop_max(pTree->pImpl_);
    GBL_API_END();
}

GBL_INLINE GBL_RESULT gblBTreeMin(GblBTree* pTree, void** ppResult) {
    GBL_ASSERT(pTree);
    GBL_API_BEGIN(pTree->hCtx);
    GBL_API_VERIFY_POINTER(ppResult);
    *ppResult = btree_min(pTree->pImpl_);
    GBL_API_END();
}

GBL_INLINE GBL_RESULT gblBTreeMax(GblBTree* pTree, void** ppResult) {
    GBL_ASSERT(pTree);
    GBL_API_BEGIN(pTree->hCtx);
    GBL_API_VERIFY_POINTER(ppResult);
    *ppResult = btree_max(pTree->pImpl_);
    GBL_API_END();
}

GBL_INLINE GBL_RESULT gblBTreeLoad(GblBTree* pTree, void* pItem, void** ppResult) {
    GBL_ASSERT(pTree);
    GBL_API_BEGIN(pTree->hCtx);
    GBL_API_VERIFY_POINTER(pItem);
    GBL_API_VERIFY_POINTER(ppResult);
    *ppResult = btree_load(pTree->pImpl_, pItem);
    GBL_API_VERIFY_BTREE(pTree);
    GBL_API_END();
}

GBL_INLINE GBL_RESULT gblBTreeAcend(GblBTree* pTree, void* pPivot, bool (*pFnIter)(const void *item, void *udata), void *pUdata, GblBool* pResult) {
    GBL_ASSERT(pTree);
    GBL_API_BEGIN(pTree->hCtx);
    GBL_API_VERIFY_POINTER(pPivot);
    GBL_API_VERIFY_POINTER(pResult);
    *pResult = btree_ascend(pTree->pImpl_, pPivot, pFnIter, pUdata);
    GBL_API_END();
}

GBL_INLINE GBL_RESULT gblBTreeDescend(GblBTree* pTree, void* pPivot, bool (*pFnIter)(const void *item, void *udata), void *pUdata, GblBool* pResult) {
    GBL_ASSERT(pTree);
    GBL_API_BEGIN(pTree->hCtx);
    GBL_API_VERIFY_POINTER(pPivot);
    GBL_API_VERIFY_POINTER(pResult);
    *pResult = btree_descend(pTree->pImpl_, pPivot, pFnIter, pUdata);
    GBL_API_END();
}

// btree_action is a return value for the iter callback function that is
// passed to the btree_action_* functions.
enum btree_action {
    BTREE_STOP,     // Stop the iteration
    BTREE_NONE,     // Make no change and continue iterating
    BTREE_DELETE,   // Delete item item and continue iterating
    BTREE_UPDATE,   // Update the item and continue iterating.
};


GBL_INLINE GBL_RESULT gblBTreeActionAcend(GblBTree* pTree, void* pPivot, bool (*pFnIter)(const void *item, void *udata), void *pUdata, GblBool* pResult) {
    GBL_ASSERT(pTree);
    GBL_API_BEGIN(pTree->hCtx);
    GBL_API_VERIFY_POINTER(pPivot);
    GBL_API_VERIFY_POINTER(pResult);
    *pResult = btree_action_ascend(pTree->pImpl_, pPivot, pFnIter, pUdata);
    GBL_API_END();
}

GBL_INLINE GBL_RESULT gblBTreeActionDescend(GblBTree* pTree, void* pPivot, bool (*pFnIter)(const void *item, void *udata), void *pUdata, GblBool* pResult) {
    GBL_ASSERT(pTree);
    GBL_API_BEGIN(pTree->hCtx);
    GBL_API_VERIFY_POINTER(pPivot);
    GBL_API_VERIFY_POINTER(pResult);
    *pResult = btree_action_descend(pTree->pImpl_, pPivot, pFnIter, pUdata);
    GBL_API_END();
}

// functions that support hints

GBL_INLINE GBL_RESULT gblBTreeSetHint(GblBTree* pTree, void* pItem, uint64_t* pHint, void** ppResult) {
    GBL_ASSERT(pTree);
    GBL_API_BEGIN(pTree->hCtx);
    GBL_API_VERIFY_POINTER(pItem);
    GBL_API_VERIFY_POINTER(pHint);
    GBL_API_VERIFY_POINTER(ppResult);
    *ppResult = btree_set_hint(pTree->pImpl_, pItem, pHint);
    GBL_API_VERIFY_BTREE(pTree);
    GBL_API_END();
}

GBL_INLINE GBL_RESULT gblBTreeDeleteHint(GblBTree* pTree, void* pKey, uint64_t* pHint, void** ppResult) {
    GBL_ASSERT(pTree);
    GBL_API_BEGIN(pTree->hCtx);
    GBL_API_VERIFY_POINTER(pItem);
    GBL_API_VERIFY_POINTER(pHint);
    GBL_API_VERIFY_POINTER(ppResult);
    *ppResult = btree_delete_hint(pTree->pImpl_, pKey, pHint);
    GBL_API_END();
}


GBL_INLINE GBL_RESULT gblBTreeAcendHint(GblBTree* pTree, void* pPivot, bool (*pFnIter)(const void *item, void *udata), void *pUdata, uint64_t* pHint, GblBool* pResult) {
    GBL_ASSERT(pTree);
    GBL_API_BEGIN(pTree->hCtx);
    GBL_API_VERIFY_POINTER(pPivot);
    GBL_API_VERIFY_POINTER(pResult);
    GBL_API_VERIFY_POINTER(pHint);
    *pResult = btree_ascend_hint(pTree->pImpl_, pPivot, pFnIter, pUdata, pHint);
    GBL_API_VERIFY_BTREE(pTree);
    GBL_API_END();
}

GBL_INLINE GBL_RESULT gblBTreeDescendHint(GblBTree* pTree, void* pPivot, bool (*pFnIter)(const void *item, void *udata), void *pUdata, uint64_t* pHint, GblBool* pResult) {
    GBL_ASSERT(pTree);
    GBL_API_BEGIN(pTree->hCtx);
    GBL_API_VERIFY_POINTER(pPivot);
    GBL_API_VERIFY_POINTER(pResult);
    GBL_API_VERIFY_POINTER(pHint);
    *pResult = btree_descend_hint(pTree->pImpl_, pPivot, pFnIter, pUdata, pHint);
    GBL_API_VERIFY_BTREE(pTree);
    GBL_API_END();
}

GBL_INLINE GBL_RESULT gblBTreeActionAcendHint(GblBTree* pTree, void* pPivot, bool (*pFnIter)(const void *item, void *udata), void *pUdata, uint64_t* pHint, GblBool* pResult) {
    GBL_ASSERT(pTree);
    GBL_API_BEGIN(pTree->hCtx);
    GBL_API_VERIFY_POINTER(pPivot);
    GBL_API_VERIFY_POINTER(pResult);
    GBL_API_VERIFY_POINTER(pHint);
    *pResult = btree_action_ascend_hint(pTree->pImpl_, pPivot, pFnIter, pUdata, pHint);
    GBL_API_VERIFY_BTREE(pTree);
    GBL_API_END();
}

GBL_INLINE GBL_RESULT gblBTreeActionDescendHint(GblBTree* pTree, void* pPivot, bool (*pFnIter)(const void *item, void *udata), void *pUdata, uint64_t* pHint, GblBool* pResult) {
    GBL_ASSERT(pTree);
    GBL_API_BEGIN(pTree->hCtx);
    GBL_API_VERIFY_POINTER(pPivot);
    GBL_API_VERIFY_POINTER(pResult);
    GBL_API_VERIFY_POINTER(pHint);
    *pResult = btree_action_descend_hint(pTree->pImpl_, pPivot, pFnIter, pUdata, pHint);
    GBL_API_VERIFY_BTREE(pTree);
    GBL_API_END();
}



#ifdef __cplusplus
}
#endif

#endif // GIMBAL_BTREE_H

#include <gimbal/allocators/gimbal_pool_allocator.h>
#include <gimbal/algorithms/gimbal_numeric.h>
#include <gimbal/core/gimbal_ctx.h>

GBL_EXPORT GBL_RESULT (GblPoolAllocator_construct)(GblPoolAllocator*      pSelf,
                                                   size_t                 entrySize,
                                                   size_t                 entriesPerPage,
                                                   size_t                 entryAlign,
                                                   GblArenaAllocatorPage* pInitialPage,
                                                   GblContext*            pCtx)
{
    GBL_CTX_BEGIN(pCtx);

    GBL_CTX_VERIFY_CALL(GblArenaAllocator_construct(&pSelf->arena,
                                                    gblAlignedAllocSize(entrySize, entryAlign) * entriesPerPage,
                                                    entryAlign,
                                                    pInitialPage,
                                                    pCtx));
    GblLinkedList_init(&pSelf->freeList);
    pSelf->entrySize    = entrySize;
    pSelf->entryAlign   = entryAlign;
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblPoolAllocator_destruct(GblPoolAllocator* pSelf) {
    return GblArenaAllocator_destruct(&pSelf->arena);
}

GBL_EXPORT void* GblPoolAllocator_new(GblPoolAllocator* pSelf) {
    GblLinkedListNode* pPtr = GblLinkedList_popFront(&pSelf->freeList);
    if(!pPtr) {
        pPtr = GblArenaAllocator_alloc(&pSelf->arena,
                                       pSelf->entrySize,
                                       pSelf->entryAlign);
    }
    if(pPtr) ++pSelf->activeEntries;
    return pPtr;
}

GBL_EXPORT GBL_RESULT GblPoolAllocator_delete(GblPoolAllocator* pSelf, void* pEntry) {
    GblLinkedList_pushFront(&pSelf->freeList, (GblLinkedListNode*)pEntry);
    --pSelf->activeEntries;
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT size_t  GblPoolAllocator_freeListSize(const GblPoolAllocator* pSelf) {
    return GblLinkedList_count(&pSelf->freeList);
}

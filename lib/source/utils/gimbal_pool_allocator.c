#include <gimbal/utils/gimbal_pool_allocator.h>
#include <gimbal/algorithms/gimbal_numeric.h>
#include <gimbal/core/gimbal_api_frame.h>

GBL_EXPORT GBL_RESULT (GblPoolAllocator_construct)(GblPoolAllocator*      pSelf,
                                                   GblSize                entrySize,
                                                   GblSize                entriesPerPage,
                                                   GblSize                entryAlign,
                                                   GblArenaAllocatorPage* pInitialPage,
                                                   GblContext*            pCtx)
{
    GBL_API_BEGIN(pCtx);

    GBL_API_VERIFY_CALL(GblArenaAllocator_construct(&pSelf->arena,
                                                    gblAlignedAllocSize(entrySize, entryAlign) * entriesPerPage,
                                                    entryAlign,
                                                    pInitialPage,
                                                    pCtx));
    GblLinkedList_init(&pSelf->freeList);
    pSelf->entrySize    = entrySize;
    pSelf->entryAlign   = entryAlign;
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblPoolAllocator_destruct(GblPoolAllocator* pSelf) {
    return GblArenaAllocator_destruct(&pSelf->arena);
}

GBL_EXPORT void* GblPoolAllocator_new(GblPoolAllocator* pSelf) {
    GblLinkedListNode* pPtr = GblLinkedList_popFront(&pSelf->freeList);
    if(!pPtr) {
        pPtr = GblArenaAllocator_allocAligned(&pSelf->arena,
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

GBL_EXPORT GblSize GblPoolAllocator_freeListSize(const GblPoolAllocator* pSelf) {
    return GblLinkedList_count(&pSelf->freeList);
}

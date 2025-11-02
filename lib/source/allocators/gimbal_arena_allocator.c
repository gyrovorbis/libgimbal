#include <gimbal/allocators/gimbal_arena_allocator.h>
#include <gimbal/core/gimbal_ctx.h>
#include <gimbal/meta/ifaces/gimbal_ilogger.h>

#define GBL_ARENA_PAGE_(node)  GBL_LINKED_LIST_ENTRY(node, GblArenaAllocatorPage, listNode)

GBL_EXPORT GBL_RESULT (GblArenaAllocator_construct)(GblArenaAllocator*     pSelf,
                                                    size_t                 pageSize,
                                                    size_t                 pageAlign,
                                                    GblArenaAllocatorPage* pInitialPage,
                                                    GblContext*            pCtx)
{
    GBL_CTX_BEGIN(pCtx);
    //GblInstance_construct(GBL_INSTANCE(&pSelf), GBL_ARENA_ALLOCATOR_TYPE);
    memset(pSelf, 0, sizeof(GblArenaAllocator));
    pSelf->pCtx = pCtx;
    GblLinkedList_init(&pSelf->listNode);
    if(pInitialPage)
        GblLinkedList_pushFront(&pSelf->listNode, &pInitialPage->listNode);
    pSelf->pageSize = pageSize;
    pSelf->pageAlign = pageAlign < GBL_ALIGNOF(GBL_MAX_ALIGN_T)? GBL_ALIGNOF(GBL_MAX_ALIGN_T) : pageAlign;
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblArenaAllocator_destruct(GblArenaAllocator* pSelf) {
    return GblArenaAllocator_freeAll(pSelf);
}

static GblArenaAllocatorPage* GblArenaAllocator_allocPage_(GblContext* pCtx, size_t  size, size_t  align) {
    GblArenaAllocatorPage* pPage = NULL;
    GBL_CTX_BEGIN(pCtx);
    const size_t  actualSize = gblAlignedAllocSize(sizeof(GblArenaAllocatorPage) + size-1, align);
    pPage = GBL_CTX_MALLOC(actualSize, align);
    GblLinkedList_init(&pPage->listNode);
    pPage->capacity = size;
    pPage->used = 0;
    pPage->staticAlloc = GBL_FALSE;
    GBL_CTX_END_BLOCK();
    return pPage;
}

GBL_EXPORT void* GblArenaAllocator_alloc(GblArenaAllocator* pSelf, size_t  size, size_t alignment) {
    void* pData = NULL;
    if(size) {

        GblArenaAllocatorPage* pPage = GBL_ARENA_PAGE_(GblLinkedList_front(&pSelf->listNode));

        if(alignment == 0) alignment = GBL_ALIGNOF(GBL_MAX_ALIGN_T);
        const size_t  actualSize = gblAlignedAllocSize(size, alignment);

        if(!pPage || (pPage->capacity - pPage->used) < actualSize) {
            if(actualSize <= pSelf->pageSize) {
                GblArenaAllocatorPage* pNewPage = GblArenaAllocator_allocPage_(pSelf->pCtx,
                                                                               pSelf->pageSize,
                                                                               pSelf->pageAlign);
                GblLinkedList_pushFront(&pSelf->listNode, &pNewPage->listNode);
            } else {
                GBL_CTX_BEGIN(pSelf->pCtx);
                GBL_CTX_VERIFY(actualSize <= pSelf->pageSize,
                               GBL_RESULT_ERROR_OVERFLOW,
                               "Cannot allocate chunk of size %u from pages of size %u",
                               actualSize, pSelf->pageSize);
                GBL_CTX_END_BLOCK();
                goto end;
            }
        }

        pData = &pSelf->pActivePage->bytes[pSelf->pActivePage->used];
        pSelf->pActivePage->used += actualSize;
        ++pSelf->allocCount;

        const uintptr_t rem = (uintptr_t)pData % alignment;
        if(rem) {
            uintptr_t advancePointer = (alignment - rem);
            pData = (void*)(((uintptr_t)pData) + advancePointer);
        }
    }
end:
    return pData;
}

GBL_EXPORT GBL_RESULT GblArenaAllocator_freeAll(GblArenaAllocator* pSelf) {
    GBL_CTX_BEGIN(pSelf->pCtx);

    GblLinkedListNode staticList;
    GblLinkedList_init(&staticList);

    GblLinkedListNode tempNode;
    // iterate over all pages, freeing the heap ones, accumulating + clearing the static ones
    for(GblLinkedListNode* pNode = pSelf->listNode.pNext;
        pNode != &pSelf->listNode;
        pNode = pNode->pNext)
    {
        GblArenaAllocatorPage* pPage = GBL_ARENA_PAGE_(pNode);
        if(!pPage->staticAlloc) {
            tempNode = *pNode;
            GBL_CTX_FREE(pPage);
            pNode = &tempNode;
        } else {
            pPage->used = 0;
            tempNode = *pNode;
            GblLinkedList_pushBack(&staticList, pNode);
            pNode = &tempNode;
        }
    }

    GblLinkedList_init(&pSelf->listNode);
    GblLinkedList_joinFront(&pSelf->listNode, &staticList);
    pSelf->allocCount   = 0;

    GBL_CTX_END();
}

GBL_EXPORT size_t  GblArenaAllocator_pageCount(const GblArenaAllocator* pSelf) {
    return GblLinkedList_count(&pSelf->listNode);
}

GBL_EXPORT size_t  GblArenaAllocator_bytesUsed(const GblArenaAllocator* pSelf) {
    size_t  bytes = 0;
    for(GblLinkedListNode* pIt = pSelf->listNode.pNext;
        pIt != &pSelf->listNode;
        pIt = pIt->pNext)
    {
        bytes += GBL_ARENA_PAGE_(pIt)->used;
    }
    return bytes;
}

GBL_EXPORT size_t  GblArenaAllocator_totalCapacity(const GblArenaAllocator* pSelf) {
    size_t  bytes = 0;
    for(GblLinkedListNode* pIt = pSelf->listNode.pNext;
        pIt != &pSelf->listNode;
        pIt = pIt->pNext)
    {
        bytes += GBL_ARENA_PAGE_(pIt)->capacity;
    }
    return bytes;
}

GBL_EXPORT float GblArenaAllocator_utilization(const GblArenaAllocator* pSelf) {
    size_t  capacity = 0;
    size_t  used = 0;
    for(GblLinkedListNode* pIt = pSelf->listNode.pNext;
        pIt != &pSelf->listNode;
        pIt = pIt->pNext)
    {
        capacity += GBL_ARENA_PAGE_(pIt)->capacity;
        used += GBL_ARENA_PAGE_(pIt)->used;
    }
    return (capacity != 0)? (float)used/(float)capacity : 0.0f;
}

GBL_EXPORT size_t  GblArenaAllocator_fragmentedBytes(const GblArenaAllocator* pSelf) {
    size_t bytes = 0;
    for(GblLinkedListNode* pIt = pSelf->listNode.pNext;
        pIt != &pSelf->listNode;
        pIt = pIt->pNext)
    {
        if(GBL_ARENA_PAGE_(pIt) == pSelf->pActivePage) continue;
        bytes += GBL_ARENA_PAGE_(pIt)->capacity - GBL_ARENA_PAGE_(pIt)->used;
    }
    return bytes;
}

GBL_EXPORT size_t  GblArenaAllocator_bytesAvailable(const GblArenaAllocator* pSelf) {
    size_t bytes = 0;
    if(pSelf->pActivePage) {
        bytes = pSelf->pActivePage->capacity - pSelf->pActivePage->used;
    }
    return bytes;
}

GBL_EXPORT void GblArenaAllocator_saveState(const GblArenaAllocator* pSelf, GblArenaAllocatorState* pState) {
    pState->pActivePage = pSelf->pActivePage;
    pState->bytesUsed   = pSelf->pActivePage->used;
}

GBL_EXPORT GBL_RESULT GblArenaAllocator_loadState(GblArenaAllocator* pSelf, const GblArenaAllocatorState* pState) {
    GBL_CTX_BEGIN(pSelf->pCtx);
    while(pSelf->pActivePage != pState->pActivePage) {
        GblArenaAllocatorPage* pPage = GBL_ARENA_PAGE_(GblLinkedList_popFront(&pSelf->listNode));
        GBL_CTX_FREE(pPage);
    }
    pSelf->pActivePage->used = pState->bytesUsed;
    GBL_CTX_END();
}

static GBL_RESULT GblArenaAllocator_IAllocator_alloc_(GblIAllocator*       pIAllocator,
                                           const GblStackFrame* pFrame,
                                           size_t               size,
                                           size_t               align,
                                           const char*          pDbgStr,
                                           void**               ppData)
{
    GblArenaAllocator* pSelf = GBL_ARENA_ALLOCATOR(pIAllocator);

    *ppData = GblArenaAllocator_alloc(pSelf, size, align);

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GblArenaAllocator_IAllocator_realloc_(GblIAllocator*       pIAllocator,
                                                        const GblStackFrame* pFrame,
                                                        void*                pData,
                                                        size_t               newSize,
                                                        size_t               newAlign,
                                                        void**               ppNewData)
{
    GblArenaAllocator* pSelf = GBL_ARENA_ALLOCATOR(pIAllocator);

    *ppNewData = GblArenaAllocator_alloc(pSelf, newSize, newAlign);

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GblArenaAllocator_IAllocator_free_(GblIAllocator*       pIAllocator,
                                                     const GblStackFrame* pFrame,
                                                     void*                pData)
{
    return GBL_RESULT_UNIMPLEMENTED;
}

static GBL_RESULT GblArenaAllocatorClass_init_(GblClass* pClass, const void* pUd) {
    GBL_IALLOCATOR_CLASS(pClass)->pFnAlloc   = GblArenaAllocator_IAllocator_alloc_;
    GBL_IALLOCATOR_CLASS(pClass)->pFnRealloc = GblArenaAllocator_IAllocator_realloc_;
    GBL_IALLOCATOR_CLASS(pClass)->pFnFree    = GblArenaAllocator_IAllocator_free_;
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GblType GblArenaAllocator_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static GblInterfaceImpl ifaceEntries[] = {
        {
            .classOffset = offsetof(GblArenaAllocatorClass, GblIAllocatorImpl)
        }
    };

    const static GblTypeInfo info = {
        .pFnClassInit   = GblArenaAllocatorClass_init_,
        .classSize      = sizeof(GblArenaAllocatorClass),
        .instanceSize   = sizeof(GblArenaAllocator),
        .interfaceCount = 1,
        .pInterfaceImpls  = ifaceEntries
    };

    if GBL_UNLIKELY(type == GBL_INVALID_TYPE) {
        ifaceEntries[0].interfaceType = GBL_IALLOCATOR_TYPE;

        type = GblType_register(GblQuark_internStatic("GblArenaAllocator"),
                                GBL_OBJECT_TYPE,
                                &info,
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}


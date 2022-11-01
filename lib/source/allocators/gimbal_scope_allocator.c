#include <gimbal/allocators/gimbal_scope_allocator.h>
#include <gimbal/core/gimbal_ctx.h>

#define GBL_SCOPE_ALLOCATOR_DTOR_NODE_ENTRY_(listNode)  GBL_LINKED_LIST_ENTRY(listNode, GblScopeAllocatorDtorEntry_, node)

typedef struct GblScopeAllocatorDtorEntry_ {
    union {
        struct GblScopeAllocatorDtorEntry* pNext;
        GblLinkedListNode                  node;
    };
    GblScopeAllocatorDtorFn                pFnDtor;
    void*                                  pData;
} GblScopeAllocatorDtorEntry_;

GBL_EXPORT GblScopeAllocator* GblScopeAllocator_create(GblArenaAllocator* pArena) {
    GblScopeAllocator* pAllocator = NULL;
    GBL_CTX_BEGIN(pArena->pCtx);
    pAllocator = GBL_CTX_MALLOC(sizeof(GblScopeAllocator), 0, "Scope Allocator");
    GBL_CTX_VERIFY_CALL(GblScopeAllocator_construct(pAllocator, pArena));
    GBL_CTX_END_BLOCK();
    return pAllocator;
}

GBL_EXPORT GBL_RESULT GblScopeAllocator_construct(GblScopeAllocator*  pSelf,
                                                   GblArenaAllocator* pArena)
{
    pSelf->pArena = pArena;
    GblArenaAllocator_saveState(pArena, &pSelf->rewindState);
    GblLinkedList_init(&pSelf->dtorList);
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT GblScopeAllocator_destroy(GblScopeAllocator* pSelf) {
    GBL_CTX_BEGIN(pSelf->pArena->pCtx);
    GBL_CTX_VERIFY_CALL(GblScopeAllocator_destruct(pSelf));
    GBL_CTX_FREE(pSelf);
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblScopeAllocator_destruct(GblScopeAllocator* pSelf) {
    GBL_CTX_BEGIN(pSelf->pArena->pCtx);

    for(GblLinkedListNode* pIt = pSelf->dtorList.pNext;
        pIt != &pSelf->dtorList;
        pIt = pIt->pNext)
    {
        GblScopeAllocatorDtorEntry_* pEntry = GBL_SCOPE_ALLOCATOR_DTOR_NODE_ENTRY_(pIt);
        GBL_CTX_CALL(pEntry->pFnDtor(pEntry->pData));
    }

    GBL_CTX_VERIFY_CALL(GblArenaAllocator_loadState(pSelf->pArena,
                                                    &pSelf->rewindState));
    GBL_CTX_END();
}

GBL_EXPORT void* GblScopeAllocator_alloc(GblScopeAllocator*      pSelf,
                                         GblSize                 size,
                                         GblSize                 align,
                                         GblScopeAllocatorDtorFn pFnDtor)
{
    void* pPtr = GblArenaAllocator_allocAligned(pSelf->pArena,
                                                size,
                                                align);

    if(pPtr && pFnDtor) {
        GblScopeAllocator_pushDtor(pSelf, pFnDtor, pPtr);
    }

    return pPtr;
}


GBL_EXPORT GBL_RESULT GblScopeAllocator_pushDtor(GblScopeAllocator*      pSelf,
                                                 GblScopeAllocatorDtorFn pFnDtor,
                                                 void*                   pData)
{
    GblScopeAllocatorDtorEntry_* pEntry = GblArenaAllocator_alloc(pSelf->pArena,
                                                                  sizeof(GblScopeAllocatorDtorEntry_));
    if(pEntry) {
        pEntry->pFnDtor = pFnDtor;
        pEntry->pData   = pData;
        GblLinkedList_init(&pEntry->node);
        GblLinkedList_pushFront(&pSelf->dtorList, &pEntry->node);
        return GBL_RESULT_SUCCESS;
    } else {
        return GBL_RESULT_ERROR_MEM_ALLOC;
    }
}


GBL_EXPORT GblSize GblScopeAllocator_dtorCount(const GblScopeAllocator* pSelf) {
    return GblLinkedList_count(&pSelf->dtorList);
}


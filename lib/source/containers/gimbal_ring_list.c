#include <gimbal/containers/gimbal_ring_list.h>
#include <gimbal/core/gimbal_ctx.h>
#include <gimbal/allocators/gimbal_pool_allocator.h>
#include <gimbal/meta/ifaces/gimbal_ilogger.h>
#include <stdarg.h>

#define GBL_RING_LIST_(node) GBL_DOUBLY_LINKED_LIST_ENTRY(node, GblRingList, listNode)

#define GBL_RING_LIST_POOL_ALLOCATOR

#ifdef GBL_RING_LIST_POOL_ALLOCATOR
#  define GBL_RING_LIST_NEW_()           (GblPoolAllocator_new(&allocPool_))
#  define GBL_RING_LIST_DELETE_(ptr)     (GblPoolAllocator_delete(&allocPool_, ptr))
#else
#  define GBL_RING_LIST_NEW_()           GBL_CTX_NEW(GblRingList)
#  define GBL_RING_LIST_DELETE_(ptr)     GBL_CTX_FREE(ptr)
#endif

static GblPoolAllocator allocPool_;

void* GblRingList_new_(void) {
    return GBL_RING_LIST_NEW_();
}

void GblRingList_delete_(void* pPtr) {
    GBL_RING_LIST_DELETE_(pPtr);
}

GBL_EXPORT GBL_RESULT GblRingList_pushBackVaList(GblRingList* pSelf, va_list* pList) {
    GBL_CTX_BEGIN(NULL);
    void* pArg = NULL;

    GblRingList* pEntry = NULL;
    while((pArg = va_arg(*pList, void*))) {
        pEntry = GBL_RING_LIST_NEW_();
        pEntry->pData = pArg;
        GblDoublyLinkedList_pushBack(&pSelf->listNode,
                                     &pEntry->listNode);
        ++pSelf->size;
    }

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblRingList_pushFrontVaList(GblRingList* pSelf, va_list* pList) {
    GBL_CTX_BEGIN(NULL);
    void* pArg = NULL;

    GblSize idx = 0;
    while((pArg = va_arg(*pList, void*))) {
        GblRingList* pEntry = GBL_RING_LIST_NEW_();
        pEntry->pData = pArg;
        GblDoublyLinkedList_insert(&pSelf->listNode,
                                   idx++,
                                   &pEntry->listNode);
        ++pSelf->size;
    }

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblRingList_insertVaList(GblRingList* pSelf, intptr_t index, va_list* pList) {
    GBL_CTX_BEGIN(NULL);

    if(index == 0) {
        GBL_CTX_VERIFY_CALL(GblRingList_pushFrontVaList(pSelf, pList));
    } else if(index == (intmax_t)pSelf->size) {
        GBL_CTX_VERIFY_CALL(GblRingList_pushBackVaList(pSelf, pList));
    } else if(index == -((intmax_t)pSelf->size + 1)) {
        GBL_CTX_VERIFY_CALL(GblRingList_pushFrontVaList(pSelf, pList));
    } else {
        void* pArg;
        GblRingList* pEntry = GBL_RING_LIST_(GblDoublyLinkedList_at(&pSelf->listNode, index));
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_VERIFY(pEntry, GBL_RESULT_ERROR_OUT_OF_RANGE);

        if(index > 0) {
            while((pArg = va_arg(*pList, void*))) {
                GblRingList* pNewEntry = GBL_RING_LIST_NEW_();
                GblDoublyLinkedList_init(&pNewEntry->listNode);
                GblDoublyLinkedList_insertBefore(&pEntry->listNode, &pNewEntry->listNode);
                pNewEntry->pData = pArg;
                ++pSelf->size;
            }

        } else {
            while((pArg = va_arg(*pList, void*))) {
                GblRingList* pNewEntry = GBL_RING_LIST_NEW_();
                GblDoublyLinkedList_init(&pNewEntry->listNode);
                GblDoublyLinkedList_insertAfter(&pEntry->listNode, &pNewEntry->listNode);
                pNewEntry->pData = pArg;
                ++pSelf->size;
                pEntry = pNewEntry;
            }
        }
    }

    GBL_CTX_END();
}

GBL_EXPORT GblRingList* GblRingList_createEmpty(void) {
    GblRingList* pList = NULL;
    GBL_CTX_BEGIN(NULL);

    // top-level entry point, ensure pool is created!
    if(!allocPool_.entrySize) {
        GBL_CTX_VERIFY_CALL(GblPoolAllocator_construct(&allocPool_,
                                                       sizeof(GblRingList),
                                                       20));
    }

    pList = GBL_RING_LIST_NEW_();
    GblDoublyLinkedList_init(&pList->listNode);
    pList->size = 0;
    GBL_CTX_END_BLOCK();
    return pList;
}

GBL_EXPORT GblRingList* (GblRingList_create)(void* pData, ...) {
    GblRingList* pList = NULL;
    va_list varArgs;
    va_start(varArgs, pData);
    GBL_CTX_BEGIN(NULL);

    pList = GblRingList_createEmpty();

    GBL_CTX_VERIFY_CALL(GblRingList_pushBack(pList, pData));
    GBL_CTX_VERIFY_CALL(GblRingList_pushBackVaList(pList, &varArgs));

    GBL_CTX_END_BLOCK();
    va_end(varArgs);
    return pList;
}

GBL_EXPORT GblRingList* (GblRingList_copy)(const GblRingList* pSelf,
                                           GblRingListCopyFn  pFnCopy,
                                           void*              pCapture)
{
    GblRingList* pList = NULL;
    GBL_CTX_BEGIN(NULL);

    pList = GblRingList_createEmpty();
    GBL_CTX_VERIFY_LAST_RECORD();

    if(!pFnCopy) {
        for(GblRingList* pIt = pSelf->ringNode.pNext;
            pIt != pSelf;
            pIt = pIt->ringNode.pNext)
        {
            GBL_CTX_VERIFY_CALL(GblRingList_pushBack(pList, pIt->pData));
        }
    } else {
        for(GblRingList* pIt = pSelf->ringNode.pNext;
            pIt != pSelf;
            pIt = pIt->ringNode.pNext)
        {
            GBL_CTX_VERIFY_CALL(GblRingList_pushBack(pList, pFnCopy(pIt->pData, pCapture)));
        }
    }

    GBL_CTX_END_BLOCK();
    return pList;
}

GBL_EXPORT GBL_RESULT (GblRingList_destroy)(GblRingList*      pSelf,
                                            GblRingListDtorFn pFnDtor,
                                            void*             pCapture)
{
    GBL_CTX_BEGIN(NULL);

    GblRingList* pNext = NULL;
    for(GblRingList* pIt = pSelf->ringNode.pNext;
        pIt != pSelf;
        pIt = pNext)
    {
        pNext = pIt->ringNode.pNext;
        if(pFnDtor) GBL_CTX_VERIFY_CALL(pFnDtor(pIt->pData, pCapture));
        GBL_RING_LIST_DELETE_(pIt);
    }

    GBL_RING_LIST_DELETE_(pSelf);

    GBL_CTX_END();
}


GBL_EXPORT GblSize GblRingList_size(const GblRingList* pSelf) {
    return GblDoublyLinkedList_count(&pSelf->listNode);
}

GBL_EXPORT GblBool GblRingList_empty(const GblRingList* pSelf) {
    return GblDoublyLinkedList_empty(&pSelf->listNode);
}

GBL_EXPORT void* GblRingList_front(const GblRingList* pSelf) {
    GblRingList* pEntry = GBL_RING_LIST_(GblDoublyLinkedList_front(&pSelf->listNode));
    return pEntry? pEntry->pData : NULL;
}

GBL_EXPORT void* GblRingList_back(const GblRingList* pSelf) {
    GblRingList* pEntry = GBL_RING_LIST_(GblDoublyLinkedList_back(&pSelf->listNode));
    return pEntry? pEntry->pData : NULL;
}

GBL_EXPORT void* GblRingList_at(const GblRingList* pSelf, intptr_t index) {
    GblDoublyLinkedListNode* pNode = GblDoublyLinkedList_at(&pSelf->listNode, index);
    return pNode? GBL_RING_LIST_(pNode)->pData : NULL;
}

GBL_EXPORT GBL_RESULT (GblRingList_pushBack)(GblRingList* pSelf, ...) {
    va_list varArgs;
    va_start(varArgs, pSelf);

    GBL_RESULT result = GblRingList_pushBackVaList(pSelf, &varArgs);

    va_end(varArgs);
    return result;
}

GBL_EXPORT GBL_RESULT (GblRingList_pushFront)(GblRingList* pSelf, ...) {
    va_list varArgs;
    va_start(varArgs, pSelf);

    GBL_RESULT result = GblRingList_pushFrontVaList(pSelf, &varArgs);

    va_end(varArgs);
    return result;
}

GBL_EXPORT GBL_RESULT (GblRingList_insert)(GblRingList* pSelf, intptr_t index, ...) {
    va_list varArgs;
    va_start(varArgs, index);

    const GBL_RESULT result = GblRingList_insertVaList(pSelf, index, &varArgs);

    va_end(varArgs);
    return result;
}

GBL_EXPORT void* GblRingList_replace(GblRingList* pSelf, intptr_t index, void* pData) {
    void* pRetVal = NULL;
    GblRingList* pEntry = GBL_RING_LIST_(GblDoublyLinkedList_at(&pSelf->listNode, index));
    if(pEntry) {
        pRetVal = pEntry->pData;
        pEntry->pData = pData;
    }
    return pRetVal;
}

GBL_EXPORT GBL_RESULT GblRingList_insertSorted(GblRingList* pSelf, void* pData, GblRingListCmpFn pFnCmp, void* pCl) {
    GBL_CTX_BEGIN(NULL);
    GblRingList* pEntry = GBL_RING_LIST_NEW_();
    pEntry->pData = pData;

    if(!pFnCmp) {
        // do straight pointer/value comparison without a comparator
        for(GblRingList* pIt = pSelf->ringNode.pNext;
            pIt != pSelf;
            pIt = pIt->ringNode.pNext)
        {
            if(pIt->pData > pData) {
                GblDoublyLinkedList_insertBefore(&pIt->listNode, &pEntry->listNode);
                ++pSelf->size;
                GBL_CTX_DONE();
            }
        }
    } else {
        // use the comparator for comparing the two values
        for(GblRingList* pIt = pSelf->ringNode.pNext;
            pIt != pSelf;
            pIt = pIt->ringNode.pNext)
        {
            if(pFnCmp(pIt->pData, pData, pCl) > 0) {
                GblDoublyLinkedList_insertBefore(&pIt->listNode, &pEntry->listNode);
                ++pSelf->size;
                GBL_CTX_DONE();
            }
        }
    }

    // It's either the biggest value or the only value, either way, it's at the end
    GblDoublyLinkedList_pushBack(&pSelf->listNode, &pEntry->listNode);
    ++pSelf->size;

    GBL_CTX_END();
}

GBL_EXPORT GblBool (GblRingList_splice)(GblRingList* pSelf, GblRingList* pOther, int32_t index) {
    const GblBool result = GblDoublyLinkedList_join(&pSelf->listNode, index, &pOther->listNode);
    pSelf->size = GblDoublyLinkedList_count(&pSelf->listNode);
    return result;
}

GBL_EXPORT void* (GblRingList_popBack)(GblRingList* pSelf, GblSize count) {
    GBL_CTX_BEGIN(NULL);
    void* pData = NULL;
    for(GblSize i = 0; i < count; ++i) {
        GblRingList* pEntry = GBL_RING_LIST_(GblDoublyLinkedList_popBack(&pSelf->listNode));
        if(pEntry) pData = pEntry->pData;
        GBL_RING_LIST_DELETE_(pEntry);
        --pSelf->size;
    }
    GBL_CTX_END_BLOCK();
    return pData;
}

GBL_EXPORT void* (GblRingList_popFront)(GblRingList* pSelf, GblSize count) {
    GBL_CTX_BEGIN(NULL);
    void* pData = NULL;
    for(GblSize i = 0; i < count; ++i) {
        GblRingList* pEntry = GBL_RING_LIST_(GblDoublyLinkedList_popFront(&pSelf->listNode));
        if(pEntry) pData = pEntry->pData;
        GBL_RING_LIST_DELETE_(pEntry);
        --pSelf->size;
    }
    GBL_CTX_END_BLOCK();
    return pData;
}

GBL_EXPORT void* (GblRingList_remove)(GblRingList* pSelf, intptr_t index, GblSize count) {
    void* pData = NULL;
    GBL_CTX_BEGIN(NULL);

    if(index >= 0)
        GBL_CTX_VERIFY(index + count <= pSelf->size,
                       GBL_RESULT_ERROR_OUT_OF_RANGE);
    else
        GBL_CTX_VERIFY(labs(index) + count <= pSelf->size+1,
                       GBL_RESULT_ERROR_OUT_OF_RANGE);

    GblRingList* pNode = GBL_RING_LIST_(GblDoublyLinkedList_at(&pSelf->listNode, index));
    GblRingList* pNext = NULL;
    while(pNode && count && pSelf->size) {
        pData = pNode->pData;
        pNext = (index >= 0)? pNode->ringNode.pNext : pNode->ringNode.pPrev;
        GblDoublyLinkedList_remove(&pNode->listNode);
        GBL_RING_LIST_DELETE_(pNode);
        --count;
        --pSelf->size;
        pNode = pNext;
    }

    GBL_CTX_END_BLOCK();
    return pData;
}


GBL_EXPORT GBL_RESULT GblRingList_clear(GblRingList* pSelf) {
    GBL_CTX_BEGIN(NULL);

    GblRingList* pNext = NULL;
    for(GblRingList* pIt = pSelf->ringNode.pNext;
        pIt != pSelf;
        pIt = pNext)
    {
        pNext = pIt->ringNode.pNext;
        GBL_RING_LIST_DELETE_(pIt);
    }

    GblDoublyLinkedList_init(&pSelf->listNode);
    pSelf->size = 0;

    GBL_CTX_END();
}

static int (GblRingList_sortDefault_)(const void* pV1, const void* pV2, void* pClosure) {
    GBL_UNUSED(pClosure);
    return (uintptr_t)GBL_DOUBLY_LINKED_LIST_ENTRY(pV1, GblRingList, listNode)->pData -
           (uintptr_t)GBL_DOUBLY_LINKED_LIST_ENTRY(pV2, GblRingList, listNode)->pData;
}

typedef struct GblRingListSortAdapterClosure_ {
    GblRingListCmpFn pFnCmp;
    void*            pClosure;
} GblRingListSortAdapterClosure_;

static int GblRingList_sortAdapter_(const void* pV1, const void* pV2, void* pClosure) {
    GblRingListSortAdapterClosure_* pAdapter = pClosure;
    return pAdapter->pFnCmp(GBL_DOUBLY_LINKED_LIST_ENTRY(pV1, GblRingList, listNode)->pData,
                            GBL_DOUBLY_LINKED_LIST_ENTRY(pV2, GblRingList, listNode)->pData,
                            pAdapter->pClosure);
}

GBL_EXPORT void (GblRingList_sort)(GblRingList* pSelf, GblRingListCmpFn pCmpFn, void* pCl) {
    if(!pCmpFn) {
        return GblDoublyLinkedList_mergeSort(&pSelf->listNode, (GblRingList_sortDefault_), NULL);
    } else {
        return GblDoublyLinkedList_mergeSort(&pSelf->listNode,
                                             GblRingList_sortAdapter_,
                                             &(GblRingListSortAdapterClosure_) {
                                                .pFnCmp = pCmpFn,
                                                .pClosure = pCl
                                             });
    }

}

GBL_EXPORT void GblRingList_rotate(GblRingList* pSelf, intptr_t n) {
    GblDoublyLinkedList_rotate(&pSelf->listNode, n);
}

GBL_EXPORT void GblRingList_reverse(GblRingList* pSelf) {
    GblDoublyLinkedList_reverse(&pSelf->listNode);
}

GBL_EXPORT GblBool (GblRingList_foreach)(GblRingList* pSelf, GblRingListIterFn pFnIt, void* pCl) {
    for(GblRingList* pIt = pSelf->ringNode.pNext;
        pIt != pSelf;
        pIt = pIt->ringNode.pNext)
    {
        if(pFnIt(pIt->pData, pCl)) return GBL_TRUE;
    }
    return GBL_FALSE;
}

GBL_EXPORT GblSize GblRingList_find(const GblRingList* pSelf,
                                    const void*        pVal,
                                    GblRingListCmpFn   pFnCmp,
                                    void*              pCl)
{
    GblSize index = 0;
    for(GblRingList* pIt = pSelf->ringNode.pNext;
        pIt != pSelf;
        pIt = pIt->ringNode.pNext)
    {
        if(pFnCmp) {
            if(pFnCmp(pIt->pData, pVal, pCl) == 0) return index;
        } else if(pIt->pData == pVal) return index;
        ++index;
    }
    return GBL_RING_LIST_NPOS;
}

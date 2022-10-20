/*! \file
 *  \brief GblDoublyLinkedListNode structure and related functions
 *  \ingroup containers
 *
 *  \copydoc GblDoublyLinkedListNode
 */

#ifndef GIMBAL_DOUBLY_LINKED_LIST_HPP
#define GIMBAL_DOUBLY_LINKED_LIST_HPP

#include "gimbal_linked_list.h"

#define GBL_SELF_TYPE GblDoublyLinkedListNode

#define GBL_DOUBLY_LINKED_LIST_NPOS                             GBL_NPOS
#define GBL_DOUBLY_LINKED_LIST_NODE_INITIALIZER()               { .pNext = NULL, .pPrev = NULL }
#define GBL_DOUBLY_LINKED_LIST_NODE(name)                       GblDoublyLinkedListNode name = { .pNext = &name, .pPrev = &name }
#define GBL_DOUBLY_LINKED_LIST_ENTRY(node, structure, field)    GBL_CONTAINER_OF(node, structure, field)

GBL_DECLS_BEGIN

typedef GblLinkedListCmpFn GblDoublyLinkedListCmpFn;

/*! \brief Intrustive doubly linked list structure with vector-style API.
 *
 *  GblDoublyLinkedListNode is the low-level API around managing manually
 *  allocated, intrusive linked list structures with the list nodes
 *  embedded within their containing structures.
 *
 *  Operation                         |Time Complexity
 *  ----------------------------------|------------------
 *  iteration                         | O(N)
 *  insertion/removal (middle)        | O(N)
 *  insertion/removal (front or back) | O(1)
 *  access (front or back)            | O(1)
 *  random access (middle)            | O(N)
 *
 *  \note For more a more user-friendly, high-level, abstract,
 *  non-intrusive circularly linked list structure which can be
 *  filled with arbitrary data and whose allocations are managed
 *  automatically, see the GblRingList API, built upon this one.
 *  \ingroup containers
 *  \sa GblRingList
 */
typedef struct GblDoublyLinkedListNode {                // Size (32-bit / 64-bit)
    union {
        struct GblDoublyLinkedListNode* pNext;          // 4/8      bytes
        GblLinkedListNode               singleNode;
    };
    struct GblDoublyLinkedListNode*     pPrev;          // 4/8      bytes
} GblDoublyLinkedListNode;                              // 8/16     total

GBL_INLINE void           GblDoublyLinkedList_init         (GBL_SELF)                                     GBL_NOEXCEPT;
GBL_INLINE void           GblDoublyLinkedList_move         (GBL_SELF, GBL_SELF_TYPE* pHead)               GBL_NOEXCEPT;

GBL_INLINE GblBool        GblDoublyLinkedList_empty        (GBL_CSELF)                                    GBL_NOEXCEPT;
GBL_INLINE GblSize        GblDoublyLinkedList_count        (GBL_CSELF)                                    GBL_NOEXCEPT;

GBL_INLINE GBL_SELF_TYPE* GblDoublyLinkedList_at           (GBL_CSELF, intptr_t index)                    GBL_NOEXCEPT;
GBL_INLINE GBL_SELF_TYPE* GblDoublyLinkedList_front        (GBL_CSELF)                                    GBL_NOEXCEPT;
GBL_INLINE GBL_SELF_TYPE* GblDoublyLinkedList_back         (GBL_CSELF)                                    GBL_NOEXCEPT;
GBL_INLINE GblBool        GblDoublyLinkedList_contains     (GBL_CSELF, GBL_SELF_TYPE* pNode)              GBL_NOEXCEPT;
GBL_INLINE GblSize        GblDoublyLinkedList_find         (GBL_CSELF, GBL_SELF_TYPE* pNode)              GBL_NOEXCEPT;
GBL_INLINE GBL_SELF_TYPE* GblDoublyLinkedList_middle       (GBL_CSELF)                                    GBL_NOEXCEPT;
GBL_INLINE GBL_SELF_TYPE* GblDoublyLinkedList_beforeMiddle (GBL_CSELF)                                    GBL_NOEXCEPT;

GBL_INLINE void           GblDoublyLinkedList_pushBack     (GBL_SELF, GBL_SELF_TYPE* pNode)               GBL_NOEXCEPT;
GBL_INLINE void           GblDoublyLinkedList_pushFront    (GBL_SELF, GBL_SELF_TYPE* pNode)               GBL_NOEXCEPT;
GBL_INLINE void           GblDoublyLinkedList_moveBack     (GBL_SELF, GBL_SELF_TYPE* pNode)               GBL_NOEXCEPT;
GBL_INLINE void           GblDoublyLinkedList_moveFront    (GBL_SELF, GBL_SELF_TYPE* pNode)               GBL_NOEXCEPT;
GBL_INLINE GblBool        GblDoublyLinkedList_join         (GBL_SELF, intptr_t idx, GBL_SELF_TYPE* pList) GBL_NOEXCEPT;
GBL_INLINE void           GblDoublyLinkedList_joinBack     (GBL_SELF, GBL_SELF_TYPE* pList)               GBL_NOEXCEPT;
GBL_INLINE void           GblDoublyLinkedList_joinFront    (GBL_SELF, GBL_SELF_TYPE* pList)               GBL_NOEXCEPT;
GBL_INLINE void           GblDoublyLinkedList_joinSorted   (GBL_SELF,
                                                            GBL_SELF_TYPE*           pList,
                                                            GblDoublyLinkedListCmpFn pCmpFn,
                                                            void*                    pClosure)            GBL_NOEXCEPT;

GBL_INLINE GblBool        GblDoublyLinkedList_insert       (GBL_SELF, intptr_t idx, GBL_SELF_TYPE* pNode) GBL_NOEXCEPT;
GBL_INLINE void           GblDoublyLinkedList_insertBefore (GBL_SELF_TYPE* pNode1, GBL_SELF_TYPE* pNode2) GBL_NOEXCEPT;
GBL_INLINE void           GblDoublyLinkedList_insertAfter  (GBL_SELF_TYPE* pNode1, GBL_SELF_TYPE* pNode2) GBL_NOEXCEPT;

GBL_INLINE GBL_SELF_TYPE* GblDoublyLinkedList_popBack      (GBL_SELF)                                     GBL_NOEXCEPT;
GBL_INLINE GBL_SELF_TYPE* GblDoublyLinkedList_popFront     (GBL_SELF)                                     GBL_NOEXCEPT;

GBL_INLINE GblBool        GblDoublyLinkedList_swap         (GBL_SELF_TYPE* pNode1, GBL_SELF_TYPE* pNode2) GBL_NOEXCEPT;
GBL_INLINE void           GblDoublyLinkedList_remove       (GBL_SELF_TYPE* pNode)                         GBL_NOEXCEPT;
GBL_INLINE GBL_SELF_TYPE* GblDoublyLinkedList_removeBefore (GBL_SELF_TYPE* pNode)                         GBL_NOEXCEPT;
GBL_INLINE GBL_SELF_TYPE* GblDoublyLinkedList_removeAfter  (GBL_SELF_TYPE* pNode)                         GBL_NOEXCEPT;
GBL_INLINE GblBool        GblDoublyLinkedList_replace      (GBL_SELF_TYPE* pNode1, GBL_SELF_TYPE* pNode2) GBL_NOEXCEPT;
GBL_INLINE void           GblDoublyLinkedList_splitAfter   (GBL_SELF,
                                                            GBL_SELF_TYPE* pHead2,
                                                            GBL_SELF_TYPE* pAfter)                        GBL_NOEXCEPT;

GBL_INLINE GBL_SELF_TYPE* GblDoublyLinkedList_erase        (GBL_SELF, intptr_t index)                     GBL_NOEXCEPT;
GBL_INLINE void           GblDoublyLinkedList_clear        (GBL_SELF)                                     GBL_NOEXCEPT;

GBL_INLINE void           GblDoublyLinkedList_mergeSort    (GBL_SELF,
                                                            GblDoublyLinkedListCmpFn pCmpFn,
                                                            void* pClosure)                               GBL_NOEXCEPT;

GBL_INLINE void           GblDoublyLinkedList_rotate       (GBL_SELF, intptr_t n)                         GBL_NOEXCEPT;
GBL_INLINE void           GblDoublyLinkedList_reverse      (GBL_SELF)                                     GBL_NOEXCEPT;

// ========== IMPL ==========

GBL_INLINE void GblDoublyLinkedList_init(GBL_SELF) GBL_NOEXCEPT {
    pSelf->pNext = pSelf->pPrev = pSelf;
}

GBL_INLINE void GblDoublyLinkedList_move(GBL_SELF, GBL_SELF_TYPE* pHead) GBL_NOEXCEPT {
    if(!GblDoublyLinkedList_empty(pHead)) {
        pHead->pPrev->pNext = pSelf;
        pSelf->pPrev        = pHead->pPrev;
        pHead->pNext->pPrev = pSelf;
        pSelf->pNext        = pHead->pNext;
        pHead->pNext        = pHead;
        pHead->pPrev        = pHead;
    }
}

GBL_INLINE void GblDoublyLinkedList_pushBack(GBL_SELF, GblDoublyLinkedListNode* pNode) GBL_NOEXCEPT {
    GblDoublyLinkedListNode* pLast = pSelf->pPrev;
    pLast->pNext = pNode;
    pNode->pPrev = pLast;
    pNode->pNext = pSelf;
    pSelf->pPrev = pNode;
}

GBL_INLINE void GblDoublyLinkedList_pushFront(GBL_SELF, GblDoublyLinkedListNode* pNode) GBL_NOEXCEPT {
    pNode->pNext = pSelf->pNext;
    pSelf->pNext->pPrev = pNode;
    pNode->pPrev = pSelf;
    pSelf->pNext = pNode;
}

GBL_INLINE GblSize GblDoublyLinkedList_find(GBL_CSELF, GblDoublyLinkedListNode* pNode) GBL_NOEXCEPT {
    return GblLinkedList_find(&pSelf->singleNode, &pNode->singleNode);
}

GBL_INLINE GblBool GblDoublyLinkedList_contains(GBL_CSELF, GblDoublyLinkedListNode* pNode) GBL_NOEXCEPT {
    return GblLinkedList_contains(&pSelf->singleNode, &pNode->singleNode);
}

GBL_INLINE GblBool GblDoublyLinkedList_empty(GBL_CSELF) GBL_NOEXCEPT {
    return GblLinkedList_empty(&pSelf->singleNode) && (pSelf->pPrev == pSelf || pSelf->pPrev == NULL);
}

GBL_INLINE GblSize GblDoublyLinkedList_count(GBL_CSELF) GBL_NOEXCEPT {
    return GblLinkedList_count(&pSelf->singleNode);
}

GBL_INLINE GblDoublyLinkedListNode* GblDoublyLinkedList_at(GBL_CSELF, intptr_t index) GBL_NOEXCEPT {
    if(index >= 0) return (GblDoublyLinkedListNode*)GblLinkedList_at(&pSelf->singleNode, index);
    else {
        intptr_t count = index;
        for(GblDoublyLinkedListNode* pIt = pSelf->pPrev;
            pIt != pSelf;
            pIt = pIt->pPrev)
        {
            if(++count == 0) return pIt;
        }
        return GBL_NULL;
    }
}

GBL_INLINE GblDoublyLinkedListNode* GblDoublyLinkedList_front(GBL_CSELF) GBL_NOEXCEPT {
    return (GblDoublyLinkedListNode*)GblLinkedList_front(&pSelf->singleNode);
}

GBL_INLINE GblDoublyLinkedListNode* GblDoublyLinkedList_back(GBL_CSELF) GBL_NOEXCEPT {
    return pSelf->pPrev != pSelf? pSelf->pPrev : NULL;
}

GBL_INLINE GBL_SELF_TYPE* GblDoublyLinkedList_middle(GBL_CSELF) GBL_NOEXCEPT {
    return (GBL_SELF_TYPE*)GblLinkedList_middle(&pSelf->singleNode);
}

GBL_INLINE GBL_SELF_TYPE* GblDoublyLinkedList_beforeMiddle(GBL_CSELF) GBL_NOEXCEPT {
    return (GBL_SELF_TYPE*)GblLinkedList_beforeMiddle(&pSelf->singleNode);
}

GBL_INLINE void GblDoublyLinkedList_remove(GblDoublyLinkedListNode* pNode) GBL_NOEXCEPT {
    pNode->pPrev->pNext = pNode->pNext;
    pNode->pNext->pPrev = pNode->pPrev;
    pNode->pNext        = pNode;
    pNode->pPrev        = pNode;
}

GBL_EXPORT GblBool GblDoublyLinkedList_replace(GblDoublyLinkedListNode* pExisting, GblDoublyLinkedListNode* pReplacement) GBL_NOEXCEPT {
    pReplacement->pPrev->pNext  = pReplacement->pNext;
    pReplacement->pNext->pPrev  = pReplacement->pPrev;

    pExisting->pPrev->pNext     = pReplacement;
    pExisting->pNext->pPrev     = pReplacement;

    pReplacement->pPrev         = pExisting->pPrev;
    pReplacement->pNext         = pExisting->pNext;

    pExisting->pPrev            = pExisting;
    pExisting->pNext            = pExisting;
    return GBL_TRUE;
}

GBL_INLINE void GblDoublyLinkedList_splitAfter(GBL_SELF,
                                               GBL_SELF_TYPE* pNewHead,
                                               GBL_SELF_TYPE* pAfter) GBL_NOEXCEPT
{
    GblDoublyLinkedListNode* pBack = GblDoublyLinkedList_back(pSelf);
    GBL_ASSERT(pBack != pAfter);
    pBack->pNext = pNewHead;
    pAfter->pNext->pPrev = pNewHead;
    pNewHead->pNext = pAfter->pNext;
    pNewHead->pPrev = pBack;
    pAfter->pNext = pSelf;
    pSelf->pPrev = pAfter;
}


GBL_EXPORT GblBool GblDoublyLinkedList_swap(GblDoublyLinkedListNode* pExisting, GblDoublyLinkedListNode* pReplacement) GBL_NOEXCEPT {
    pExisting->pPrev->pNext     = pReplacement;
    pExisting->pNext->pPrev     = pReplacement;
    pReplacement->pNext->pPrev  = pExisting;
    pReplacement->pPrev->pNext  = pExisting;

    GblDoublyLinkedListNode* pNode = pReplacement->pNext;
    pReplacement->pNext = pExisting->pNext;
    pExisting->pNext = pNode;
    pNode = pReplacement->pPrev;
    pReplacement->pPrev = pExisting->pPrev;
    pExisting->pPrev = pNode;
    return GBL_TRUE;
}

GBL_INLINE void GblDoublyLinkedList_moveBack(GBL_SELF, GblDoublyLinkedListNode* pNode) GBL_NOEXCEPT {
    pNode->pPrev->pNext = pNode->pNext;
    pNode->pNext->pPrev = pNode->pPrev;

    GblDoublyLinkedList_pushBack(pSelf, pNode);
}

GBL_INLINE void GblDoublyLinkedList_moveFront(GBL_SELF, GblDoublyLinkedListNode* pNode) GBL_NOEXCEPT {
    pNode->pPrev->pNext = pNode->pNext;
    pNode->pNext->pPrev = pNode->pPrev;

    GblDoublyLinkedList_pushFront(pSelf, pNode);
}

GBL_INLINE GBL_SELF_TYPE* GblDoublyLinkedList_erase(GBL_SELF, intptr_t index) GBL_NOEXCEPT {
    GblDoublyLinkedListNode* pNode = GblDoublyLinkedList_at(pSelf, index);
    if(pNode) {
        GblDoublyLinkedList_remove(pNode);
        return pNode;
    } else return GBL_NULL;
}

GBL_INLINE void GblDoublyLinkedList_clear(GBL_SELF) GBL_NOEXCEPT {
    GblDoublyLinkedListNode* pPrev = pSelf;
    GblDoublyLinkedListNode* pIt = pSelf->pNext;

    do {
        pPrev->pNext = pPrev;
        pPrev->pPrev = pPrev;
        pPrev = pIt;
        pIt = pIt->pNext;

    } while(pIt != pSelf);
}

GBL_INLINE void GblDoublyLinkedList_joinBack(GBL_SELF, GblDoublyLinkedListNode* pList) GBL_NOEXCEPT {
    if(!GblDoublyLinkedList_empty(pList)) {
        pSelf->pPrev->pNext = pList->pNext;
        pList->pNext->pPrev = pSelf->pPrev;
        pList->pPrev->pNext = pSelf;
        pSelf->pPrev        = pList->pPrev;
        pList->pPrev        = pList;
        pList->pNext        = pList;
    }
}

GBL_INLINE void GblDoublyLinkedList_joinFront(GBL_SELF, GblDoublyLinkedListNode* pList) GBL_NOEXCEPT {
    if(!GblDoublyLinkedList_empty(pList)) {
        pSelf->pNext->pPrev = pList->pPrev;
        pList->pPrev->pNext = pSelf->pNext;
        pSelf->pNext        = pList->pNext;
        pList->pNext->pPrev = pSelf;
        pList->pNext        = pList;
        pList->pPrev        = pList;
    }
}

GBL_INLINE void GblDoublyLinkedList_joinSorted(GBL_SELF,
                                               GBL_SELF_TYPE* pList,
                                               GblDoublyLinkedListCmpFn pCmpFn,
                                               void* pClosure) GBL_NOEXCEPT
{
    GBL_DOUBLY_LINKED_LIST_NODE(dstList);
    GBL_SELF_TYPE*  pSrc1 = pSelf->pNext;
    GBL_SELF_TYPE*  pSrc2 = pList->pNext;

    while(1) {

        if(pSrc1 == pSelf) {
            GblDoublyLinkedList_joinBack(&dstList, pList);
            break;
        } else if(pSrc2 == pList) {
            GblDoublyLinkedList_joinBack(&dstList, pSelf);
            break;
        }

        if(pCmpFn(pSrc1, pSrc2, pClosure) < 0) {
            GBL_SELF_TYPE* pTemp = pSrc1;
            pSrc1 = pSrc1->pNext;
            GblDoublyLinkedList_remove(pTemp);
            GblDoublyLinkedList_pushBack(&dstList, pTemp);
            continue;
        } else {
            GBL_SELF_TYPE* pTemp = pSrc2;
            pSrc2 = pSrc2->pNext;
            GblDoublyLinkedList_remove(pTemp);
            GblDoublyLinkedList_pushBack(&dstList, pTemp);
            continue;
        }
    }

    GBL_ASSERT(GblDoublyLinkedList_empty(pSelf));
    GBL_ASSERT(GblDoublyLinkedList_empty(pList));

    GblDoublyLinkedList_move(pSelf, &dstList);
}


GBL_INLINE GblDoublyLinkedListNode* GblDoublyLinkedList_popBack(GBL_SELF) GBL_NOEXCEPT {
    GblDoublyLinkedListNode* pBack = GblDoublyLinkedList_back(pSelf);
    if(pBack) {
        pBack->pPrev->pNext = pBack->pNext;
        pBack->pNext->pPrev = pBack->pPrev;
    }
    return pBack;
}

GBL_INLINE GblDoublyLinkedListNode* GblDoublyLinkedList_popFront(GBL_SELF) GBL_NOEXCEPT {
    GblDoublyLinkedListNode* pFront = GblDoublyLinkedList_front(pSelf);
    if(pFront) {
        pFront->pPrev->pNext = pFront->pNext;
        pFront->pNext->pPrev = pFront->pPrev;
    }
    return pFront;
}

GBL_INLINE void GblDoublyLinkedList_reverse(GBL_SELF) GBL_NOEXCEPT {
    GblDoublyLinkedListNode* pIt = pSelf;
    do {
        GblDoublyLinkedListNode* pTemp = pIt->pNext;
        pIt->pNext = pIt->pPrev;
        pIt->pPrev = pTemp;
        pIt = pTemp;
    } while(pIt != pSelf);
}

GBL_INLINE void GblDoublyLinkedList_rotate(GBL_SELF, intptr_t n) GBL_NOEXCEPT {
    if(n != 0) {
        GblDoublyLinkedListNode* pPivot = GblDoublyLinkedList_at(pSelf, n);
        if(pPivot) {
            GblDoublyLinkedList_remove(pSelf);
            GblDoublyLinkedList_insertBefore(pPivot, pSelf);
        }
    }
}

GBL_INLINE GblBool GblDoublyLinkedList_insert(GBL_SELF, intptr_t idx, GBL_SELF_TYPE* pNode) GBL_NOEXCEPT {
    if(idx >= 0) {
        intptr_t count = 0;
        for(GblDoublyLinkedListNode* pIt = pSelf->pNext;
            pIt != pSelf;
            pIt = pIt->pNext)
        {
            //inserting in the middle
            if(count++ == idx) {
                GblDoublyLinkedList_insertBefore(pIt, pNode);
                return GBL_TRUE;
            }
        }
        // inserting at the end
        if(count == idx) {
            GblDoublyLinkedList_pushBack(pSelf, pNode);
            return GBL_TRUE;
        }
    } else {
        intptr_t count = idx;
        for(GblDoublyLinkedListNode* pIt = pSelf->pPrev;
            pIt != pSelf;
            pIt = pIt->pPrev)
        {
            // inserting in middle
            if(++count == 0) {
                GblDoublyLinkedList_insertAfter(pIt, pNode);
                return GBL_TRUE;
            }
        }
        //inserting at front
        if(count+1 == 0) {
            GblDoublyLinkedList_pushFront(pSelf, pNode);
            return GBL_TRUE;
        }

    }
    return GBL_FALSE;
}

GBL_INLINE void GblDoublyLinkedList_insertBefore(GBL_SELF_TYPE* pNode1, GBL_SELF_TYPE* pNode2) GBL_NOEXCEPT {
    pNode2->pPrev = pNode1->pPrev;
    pNode2->pPrev->pNext = pNode2;
    pNode1->pPrev = pNode2;
    pNode2->pNext = pNode1;
}

GBL_INLINE void GblDoublyLinkedList_insertAfter(GBL_SELF_TYPE* pNode1, GBL_SELF_TYPE* pNode2) GBL_NOEXCEPT {
    pNode2->pNext = pNode1->pNext;
    pNode2->pNext->pPrev = pNode2;
    pNode1->pNext = pNode2;
    pNode2->pPrev = pNode1;
}

GBL_INLINE GBL_SELF_TYPE* GblDoublyLinkedList_removeBefore(GBL_SELF_TYPE* pNode) GBL_NOEXCEPT {
    GblDoublyLinkedListNode* pPrev = pNode->pPrev;
    GblDoublyLinkedList_remove(pPrev);
    return pPrev;
}

GBL_INLINE GBL_SELF_TYPE* GblDoublyLinkedList_removeAfter(GBL_SELF_TYPE* pNode) GBL_NOEXCEPT {
    GblDoublyLinkedListNode* pNext = pNode->pNext;
    GblDoublyLinkedList_remove(pNext);
    return pNext;
}

GBL_INLINE GblBool GblDoublyLinkedList_join (GBL_SELF, intptr_t idx, GBL_SELF_TYPE* pList) GBL_NOEXCEPT {
    GblBool success = GBL_FALSE;
    GblDoublyLinkedListNode* pNode = GblDoublyLinkedList_at(pSelf, idx);

    if(!GblDoublyLinkedList_empty(pList)) {
        if(pNode) {
            if(idx >= 0) {
                pNode->pPrev->pNext = pList->pNext;
                pList->pNext->pPrev = pNode->pPrev;
                pNode->pPrev        = pList->pPrev;
                pList->pPrev->pNext = pNode;
            } else {
                pNode->pNext->pPrev = pList->pPrev;
                pList->pPrev->pNext = pNode->pNext;
                pNode->pNext        = pList->pNext;
                pList->pNext->pPrev = pNode;
            }
        } else {
            const intptr_t count = GblDoublyLinkedList_count(pSelf);
            if(idx == count) GblDoublyLinkedList_joinBack(pSelf, pList);
            else if(idx == -(count+1)) GblDoublyLinkedList_joinFront(pSelf, pList);
        }

        GblDoublyLinkedList_init(pList);
        success = GBL_TRUE;
    }

    return success;
}

GBL_INLINE void GblDoublyLinkedList_mergeSort(GBL_SELF, GblDoublyLinkedListCmpFn pCmpFn, void* pClosure) GBL_NOEXCEPT {
    if(GblDoublyLinkedList_count(pSelf) < 2) {
        return;
    }

    GBL_DOUBLY_LINKED_LIST_NODE(halfList);

    GblDoublyLinkedList_splitAfter(pSelf, &halfList, GblDoublyLinkedList_beforeMiddle(pSelf));

    GblDoublyLinkedList_mergeSort(pSelf, pCmpFn, pClosure);
    GblDoublyLinkedList_mergeSort(&halfList, pCmpFn, pClosure);

    GblDoublyLinkedList_joinSorted(pSelf, &halfList, pCmpFn, pClosure);
}

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_DOUBLY_LINKED_LIST_HPP

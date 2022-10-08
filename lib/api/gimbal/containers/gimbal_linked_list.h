/*! \file
 *  \brief GblLinkedListNode structure and related functions
 *  \ingroup containers
 *  \todo
 *    GblLinkedList_insert(): insert into middle by index
 *    GblLinkedList_insertAfter(): no list head required
 */

#ifndef GIMBAL_LINKED_LIST_H
#define GIMBAL_LINKED_LIST_H

#include "../core/gimbal_typedefs.h"

#define GBL_LINKED_LIST_NPOS                            GBL_NPOS
#define GBL_LINKED_LIST_NODE_INITIALIZER()              { .pNext = NULL }
#define GBL_LINKED_LIST_NODE(name)                      GblLinkedListNode name = { .pNext = &name }
#define GBL_LINKED_LIST_ENTRY(node, structure, field)   GBL_CONTAINER_OF(node, structure, field)

#define GBL_SELF_TYPE GblLinkedListNode

typedef int (*GblLinkedListCmpFn)(const void* pA, const void* pb, void* pClosure);

GBL_DECLS_BEGIN

/*! \brief Intrustive singly linked list structure with vector-style API
 *  \ingroup containers
 */
typedef struct GblLinkedListNode {
    struct GblLinkedListNode* pNext;
} GblLinkedListNode;

GBL_INLINE void           GblLinkedList_init         (GBL_SELF)                                               GBL_NOEXCEPT;
GBL_INLINE void           GblLinkedList_move         (GBL_SELF, GBL_SELF_TYPE* pHead)                         GBL_NOEXCEPT;

GBL_INLINE GblBool        GblLinkedList_empty        (GBL_CSELF)                                              GBL_NOEXCEPT;
GBL_INLINE GblSize        GblLinkedList_count        (GBL_CSELF)                                              GBL_NOEXCEPT;

GBL_INLINE GBL_SELF_TYPE* GblLinkedList_at           (GBL_CSELF, GblSize index)                               GBL_NOEXCEPT;
GBL_INLINE GBL_SELF_TYPE* GblLinkedList_front        (GBL_CSELF)                                              GBL_NOEXCEPT;
GBL_INLINE GBL_SELF_TYPE* GblLinkedList_back         (GBL_CSELF)                                              GBL_NOEXCEPT;
GBL_INLINE GblBool        GblLinkedList_contains     (GBL_CSELF, GBL_SELF_TYPE* pNode)                        GBL_NOEXCEPT;
GBL_INLINE GblSize        GblLinkedList_find         (GBL_CSELF, GBL_SELF_TYPE* PNode)                        GBL_NOEXCEPT;
GBL_INLINE GBL_SELF_TYPE* GblLinkedList_middle       (GBL_CSELF)                                              GBL_NOEXCEPT;
GBL_INLINE GBL_SELF_TYPE* GblLinkedList_beforeMiddle (GBL_CSELF)                                              GBL_NOEXCEPT;

GBL_INLINE void           GblLinkedList_pushBack     (GBL_SELF, GBL_SELF_TYPE* pNode)                         GBL_NOEXCEPT;
GBL_INLINE void           GblLinkedList_pushFront    (GBL_SELF, GBL_SELF_TYPE* pNode)                         GBL_NOEXCEPT;
GBL_INLINE void           GblLinkedList_moveBack     (GBL_SELF, GBL_SELF_TYPE* pNode)                         GBL_NOEXCEPT;
GBL_INLINE void           GblLinkedList_moveFront    (GBL_SELF, GBL_SELF_TYPE* pNode)                         GBL_NOEXCEPT;
GBL_INLINE void           GblLinkedList_joinBack     (GBL_SELF, GBL_SELF_TYPE* pList)                         GBL_NOEXCEPT;
GBL_INLINE void           GblLinkedList_joinFront    (GBL_SELF, GBL_SELF_TYPE* pList)                         GBL_NOEXCEPT;
GBL_INLINE void           GblLinkedList_joinSorted   (GBL_SELF,
                                                      GBL_SELF_TYPE*     pList,
                                                      GblLinkedListCmpFn pCmpFn,
                                                      void*              pCl)                                 GBL_NOEXCEPT;

GBL_INLINE GblBool        GblLinkedList_insert       (GBL_SELF, GBL_SELF_TYPE* pNode, GblSize index)          GBL_NOEXCEPT;
GBL_INLINE void           GblLinkedList_insertAfter  (GBL_SELF_TYPE* pNode1, GBL_SELF_TYPE* pNode2)           GBL_NOEXCEPT;

GBL_INLINE GBL_SELF_TYPE* GblLinkedList_popBack      (GBL_SELF)                                               GBL_NOEXCEPT;
GBL_INLINE GBL_SELF_TYPE* GblLinkedList_popFront     (GBL_SELF)                                               GBL_NOEXCEPT;

GBL_EXPORT GblBool        GblLinkedList_swap         (GBL_SELF, GBL_SELF_TYPE* pNode1, GBL_SELF_TYPE* pNode2) GBL_NOEXCEPT;
GBL_INLINE GblBool        GblLinkedList_remove       (GBL_SELF, GBL_SELF_TYPE* pNode)                         GBL_NOEXCEPT;
GBL_EXPORT GblBool        GblLinkedList_replace      (GBL_SELF, GBL_SELF_TYPE* pNode1, GBL_SELF_TYPE* pNode2) GBL_NOEXCEPT;
GBL_INLINE void           GblLinkedList_splitAfter   (GBL_SELF, GBL_SELF_TYPE* pHead2, GBL_SELF_TYPE* pAfter) GBL_NOEXCEPT;

GBL_INLINE GBL_SELF_TYPE* GblLinkedList_erase        (GBL_SELF, GblSize index)                                GBL_NOEXCEPT;
GBL_INLINE void           GblLinkedList_clear        (GBL_SELF)                                               GBL_NOEXCEPT;

GBL_INLINE void           GblLinkedList_mergeSort    (GBL_SELF, GblLinkedListCmpFn pCmpFn, void* pClosure)    GBL_NOEXCEPT;

GBL_INLINE void           GblLinkedList_reverse      (GBL_SELF)                                               GBL_NOEXCEPT;

// ========== IMPL ==========

GBL_INLINE void GblLinkedList_init(GBL_SELF) GBL_NOEXCEPT {
    pSelf->pNext = pSelf;
}

GBL_INLINE void GblLinkedList_move(GBL_SELF, GBL_SELF_TYPE* pHead) GBL_NOEXCEPT {
    GBL_SELF_TYPE* pLast = GblLinkedList_back(pHead);
    if(pLast) {
        pSelf->pNext = pHead->pNext;
        pLast->pNext = pSelf;
        pHead->pNext = pHead;
    }
}

GBL_INLINE void GblLinkedList_pushBack(GBL_SELF, GblLinkedListNode* pNode) GBL_NOEXCEPT {
    GblLinkedListNode* pPrevIt = pSelf;
    for(GblLinkedListNode* pIt = pSelf->pNext;
        pIt != pSelf;
        pIt = pIt->pNext)
    {
        pPrevIt = pIt;
    }
    pPrevIt->pNext = pNode;
    pNode->pNext = pSelf;
}

GBL_INLINE void GblLinkedList_pushFront(GBL_SELF, GblLinkedListNode* pNode) GBL_NOEXCEPT {
    pNode->pNext = pSelf->pNext;
    pSelf->pNext = pNode;
}

GBL_INLINE GblBool GblLinkedList_remove(GBL_SELF, GblLinkedListNode* pNode) GBL_NOEXCEPT {
    GblBool found = GBL_FALSE;
    GblLinkedListNode* pPrevIt = pSelf;
    for(GblLinkedListNode* pIt = pSelf->pNext;
        pIt != pSelf;
        pIt = pIt->pNext)
    {
        if(pIt == pNode) {
            pPrevIt->pNext  = pNode->pNext;
            pNode->pNext    = pNode;
            found           = GBL_TRUE;
            break;
        }
        pPrevIt = pIt;
    }
    return found;
}

GBL_INLINE GBL_SELF_TYPE* GblLinkedList_erase(GBL_SELF, GblSize index) GBL_NOEXCEPT {
    GblSize count = 0;
    GblLinkedListNode* pPrevIt = pSelf;
    for(GblLinkedListNode* pIt = pSelf->pNext;
        pIt != pSelf;
        pIt = pIt->pNext)
    {
        if(count++ == index) {
            pPrevIt->pNext  = pIt->pNext;
            pIt->pNext      = pIt;
            return pIt;
        }
        pPrevIt = pIt;
    }
    return GBL_NULL;
}


GBL_INLINE GblSize GblLinkedList_find(GBL_CSELF, GblLinkedListNode* pNode) GBL_NOEXCEPT {
    GblSize index = GBL_LINKED_LIST_NPOS;
    GblSize count = 0;
    for(GblLinkedListNode* pIt = pSelf->pNext;
        pIt != pSelf;
        pIt = pIt->pNext)
    {
        if(pIt == pNode) {
            index = count;
            break;
        }
        ++count;
    }
    return index;
}

GBL_INLINE GblBool GblLinkedList_contains(GBL_CSELF, GblLinkedListNode* pNode) GBL_NOEXCEPT {
    return GblLinkedList_find(pSelf, pNode) != GBL_LINKED_LIST_NPOS;
}

GBL_INLINE GblBool GblLinkedList_empty(GBL_CSELF) GBL_NOEXCEPT {
    return pSelf->pNext == pSelf || pSelf->pNext == GBL_NULL;
}

GBL_INLINE GblSize GblLinkedList_count(GBL_CSELF) GBL_NOEXCEPT {
    GblSize count = 0;
    for(GblLinkedListNode* pIt = pSelf->pNext;
        pIt != pSelf;
        pIt = pIt->pNext)
    {
        ++count;
    }
    return count;
}

GBL_INLINE GblLinkedListNode* GblLinkedList_at(GBL_CSELF, GblSize index) GBL_NOEXCEPT {
    GblLinkedListNode* pNode = GBL_NULL;
    GblSize count = 0;
    for(GblLinkedListNode* pIt = pSelf->pNext;
        pIt != pSelf;
        pIt = pIt->pNext)
    {
        if(count++ == index) {
            pNode = pIt;
            break;
        }
    }
    return pNode;
}

GBL_INLINE GblLinkedListNode* GblLinkedList_front(GBL_CSELF) GBL_NOEXCEPT {
    return (pSelf->pNext != pSelf)? pSelf->pNext : GBL_NULL;
}

GBL_INLINE GblLinkedListNode* GblLinkedList_back(GBL_CSELF) GBL_NOEXCEPT {
    GblLinkedListNode* pPrevIt = (GblLinkedListNode*)pSelf;
    for(GblLinkedListNode* pIt = pSelf->pNext;
        pIt != pSelf;
        pIt = pIt->pNext)
    {
        pPrevIt = pIt;
    }
    return (pPrevIt != pSelf)? pPrevIt : GBL_NULL;
}

GBL_INLINE GblLinkedListNode* GblLinkedList_beforeMiddle(GBL_CSELF) GBL_NOEXCEPT {
    GblLinkedListNode* pFast = pSelf->pNext;
    GblLinkedListNode* pSlow = pSelf->pNext;
    //GblLinkedListNode* pPrevSlow = pSlow;

    while(pFast->pNext != pSelf &&
          pFast->pNext->pNext != pSelf)
    {
        pFast = pFast->pNext->pNext;
      //  pPrevSlow = pSlow;
        pSlow = pSlow->pNext;
    }

    //return pPrevSlow != pSelf? pPrevSlow : GBL_NULL;
    return pSlow != pSelf? pSlow : GBL_NULL;
}

GBL_INLINE GblLinkedListNode* GblLinkedList_middle(GBL_CSELF) GBL_NOEXCEPT {
    GblLinkedListNode* pBefore = GblLinkedList_beforeMiddle(pSelf);
    return (pBefore && pBefore->pNext != pSelf)? pBefore->pNext : GBL_NULL;
}

GBL_INLINE void GblLinkedList_join_(GblLinkedListNode* pNewList,
                                    GblLinkedListNode* pPrev,
                                    GblLinkedListNode* pNext)
{
    GblLinkedListNode* pFirst   = pNewList->pNext;
    GblLinkedListNode *pLast    = GblLinkedList_back(pNewList);
    if(!pLast) pLast = pNewList;

    pPrev->pNext = pFirst;
    pLast->pNext = pNext;
}

GBL_INLINE void GblLinkedList_splitAfter(GBL_SELF, GBL_SELF_TYPE* pNewHead, GBL_SELF_TYPE* pAfter) GBL_NOEXCEPT {
    GblLinkedListNode* pBack = GblLinkedList_back(pSelf);
    pBack->pNext = pNewHead;
    pNewHead->pNext = pAfter->pNext;
    pAfter->pNext = pSelf;
}

GBL_INLINE void GblLinkedList_joinFront(GBL_SELF, GblLinkedListNode* pList) GBL_NOEXCEPT {
    if(!GblLinkedList_empty(pList)) {
        GblLinkedList_join_(pList, pSelf, pSelf->pNext);
        GblLinkedList_init(pList);
    }
}


GBL_INLINE void GblLinkedList_joinBack(GBL_SELF, GblLinkedListNode* pList) GBL_NOEXCEPT {
    if(!GblLinkedList_empty(pList)) {
        GblLinkedListNode* pBack = GblLinkedList_back(pSelf);
        if(!pBack) pBack = pSelf;
        GblLinkedList_join_(pList, pBack, pSelf);
        GblLinkedList_init(pList);
    }
}

GBL_INLINE void GblLinkedList_joinSorted(GBL_SELF,
                                         GBL_SELF_TYPE*     pList,
                                         GblLinkedListCmpFn pCmpFn,
                                         void*              pCl) GBL_NOEXCEPT {
    GBL_LINKED_LIST_NODE(dst);
    GblLinkedListNode** ppDst = &dst.pNext;
    GblLinkedListNode*  pSrc1 = pSelf->pNext;
    GblLinkedListNode*  pSrc2 = pList->pNext;

    while(1) {
        if(pSrc1 == pSelf) {
            *ppDst = &dst;
            pSelf->pNext = pSelf;
            GblLinkedList_joinBack(&dst, pList);
            break;
        } else if(pSrc2 == pList) {
            *ppDst = &dst;
            GblLinkedList_joinBack(&dst, pSelf);
            break;
        }

        if(pCmpFn(pSrc1, pSrc2, pCl) < 0) {
            pSelf->pNext = pSrc1->pNext;
            *ppDst = pSrc1;
            pSrc1 = *(ppDst = &(pSrc1->pNext));
            continue;
        } else {
            pList->pNext = pSrc2->pNext;
            *ppDst = pSrc2;
            pSrc2 = *(ppDst = &(pSrc2->pNext));
            continue;
        }
    }

    GblLinkedList_move(pSelf, &dst);
}

GBL_INLINE void GblLinkedList_mergeSort(GBL_SELF, GblLinkedListCmpFn pCmpFn, void* pClosure) GBL_NOEXCEPT {
    if(GblLinkedList_count(pSelf) < 2) {
        return;
    }

    GBL_LINKED_LIST_NODE(halfList);

    GblLinkedList_splitAfter(pSelf, &halfList, GblLinkedList_beforeMiddle(pSelf));

    GblLinkedList_mergeSort(pSelf, pCmpFn, pClosure);
    GblLinkedList_mergeSort(&halfList, pCmpFn, pClosure);

    GblLinkedList_joinSorted(pSelf, &halfList, pCmpFn, pClosure);
}

GBL_INLINE void GblLinkedList_moveBack(GBL_SELF, GblLinkedListNode* pNode) GBL_NOEXCEPT {
    for(GblLinkedListNode* pIt = pNode->pNext;
        pIt != pNode;
        pIt = pIt->pNext)
    {
        if(pIt->pNext == pNode) {
            pIt->pNext = pNode->pNext;
            break;
        }
    }
    GblLinkedList_pushBack(pSelf, pNode);
}

GBL_INLINE void GblLinkedList_moveFront(GBL_SELF, GblLinkedListNode* pNode) GBL_NOEXCEPT {
    for(GblLinkedListNode* pIt = pNode->pNext;
        pIt != pNode;
        pIt = pIt->pNext)
    {
        if(pIt->pNext == pNode) {
            pIt->pNext = pNode->pNext;
            break;
        }
    }
    GblLinkedList_pushFront(pSelf, pNode);
}

GBL_INLINE void GblLinkedList_clear(GBL_SELF) GBL_NOEXCEPT {
    GblLinkedListNode* pPrev = pSelf;
    GblLinkedListNode* pIt = pSelf->pNext;

    do {
        pPrev->pNext = pPrev;
        pPrev = pIt;
        pIt = pIt->pNext;
    } while(pIt != pSelf);
}

GBL_INLINE GblLinkedListNode* GblLinkedList_popFront(GBL_SELF) GBL_NOEXCEPT {
    GblLinkedListNode* pFront = GblLinkedList_front(pSelf);
    if(pFront) pSelf->pNext = pFront->pNext;
    return pFront;
}

GBL_INLINE GblLinkedListNode* GblLinkedList_popBack(GBL_SELF) GBL_NOEXCEPT {
    GblLinkedListNode* pBack = GBL_NULL;
    GblLinkedListNode* pPrev = pSelf;
    for(GblLinkedListNode* pIt = pSelf->pNext;
        pIt != pSelf;
        pIt = pIt->pNext)
    {
        if(pIt->pNext == pSelf) {
            pBack = pIt;
            pPrev->pNext = pSelf;
            break;
        }
        pPrev = pIt;
    }

    return pBack;
}

GBL_INLINE void GblLinkedList_reverse(GBL_SELF) GBL_NOEXCEPT {
    GblLinkedListNode* pPrev = pSelf;
    GblLinkedListNode* pNext = pSelf;
    for(GblLinkedListNode* pIt = pSelf->pNext;
        pIt != pSelf;
        pIt = pNext)
    {
        pNext = pIt->pNext;
        pIt->pNext = pPrev;
        pPrev = pIt;
    }
    pSelf->pNext = pPrev;
}

GBL_INLINE void GblLinkedList_insertAfter(GBL_SELF_TYPE* pNode1, GBL_SELF_TYPE* pNode2) GBL_NOEXCEPT {
    pNode2->pNext = pNode1->pNext;
    pNode1->pNext = pNode2;
}

GBL_INLINE GblBool GblLinkedList_insert(GBL_SELF, GBL_SELF_TYPE* pNode, GblSize index) GBL_NOEXCEPT {
    GblSize count = 0;
    GblLinkedListNode* pPrev = pSelf;
    for(GblLinkedListNode* pIt = pSelf->pNext;
        pIt != pSelf;
        pIt = pIt->pNext)
    {
        //inserting in the middle
        if(count++ == index) {
            pNode->pNext = pPrev->pNext;
            pPrev->pNext = pNode;
            return GBL_TRUE;
        }
        pPrev = pIt;
    }
    // inserting at the end
    if(count == index) {
        GblLinkedList_pushBack(pSelf, pNode);
        return GBL_TRUE;
    }
    return GBL_FALSE;
}

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_LINKED_LIST_H

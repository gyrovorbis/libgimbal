/*! \file
 *  \brief GblLinkedListNode structure and related functions
 *  \ingroup containers
 */

#ifndef GIMBAL_LINKED_LIST_H
#define GIMBAL_LINKED_LIST_H

#include "../core/gimbal_typedefs.h"

#define SELF    GblLinkedListNode*  pSelf
#define CSELF   const SELF

#define GBL_LINKED_LIST_NPOS                            GBL_NPOS
#define GBL_LINKED_LIST_NODE_INITIALIZER()              { .pNext = NULL }
#define GBL_LINKED_LIST_NODE(name)                      GblLinkedListNode name = { .pNext = &name }
#define GBL_LINKED_LIST_ENTRY(node, structure, field)   GBL_CONTAINER_OF(node, structure, field)

GBL_DECLS_BEGIN

/*! \brief Intrustive singly linked list structure with vector-style API
 *  \ingroup containers
 */
typedef struct GblLinkedListNode {
    struct GblLinkedListNode* pNext;
} GblLinkedListNode;

GBL_INLINE void                 GblLinkedList_init          (SELF)                              GBL_NOEXCEPT;
GBL_INLINE void                 GblLinkedList_pushBack      (SELF, GblLinkedListNode* pNode)    GBL_NOEXCEPT;
GBL_INLINE void                 GblLinkedList_pushFront     (SELF, GblLinkedListNode* pNode)    GBL_NOEXCEPT;
GBL_INLINE void                 GblLinkedList_moveBack      (SELF, GblLinkedListNode* pNode)    GBL_NOEXCEPT;
GBL_INLINE void                 GblLinkedList_moveFront     (SELF, GblLinkedListNode* pNode)    GBL_NOEXCEPT;
GBL_INLINE void                 GblLinkedList_joinBack      (SELF, GblLinkedListNode* pList)    GBL_NOEXCEPT;
GBL_INLINE void                 GblLinkedList_joinFront     (SELF, GblLinkedListNode* pList)    GBL_NOEXCEPT;

GBL_INLINE GblLinkedListNode*   GblLinkedList_popBack       (SELF)                              GBL_NOEXCEPT;
GBL_INLINE GblLinkedListNode*   GblLinkedList_popFront      (SELF)                              GBL_NOEXCEPT;

GBL_EXPORT GblBool              GblLinkedList_swap          (SELF,
                                                             GblLinkedListNode* pNode1,
                                                             GblLinkedListNode* pNode2)         GBL_NOEXCEPT;

GBL_INLINE GblBool              GblLinkedList_remove        (SELF, GblLinkedListNode* pNode)    GBL_NOEXCEPT;

GBL_EXPORT GblBool              GblLinkedList_replace       (SELF,
                                                             GblLinkedListNode* pExisting,
                                                             GblLinkedListNode* pReplacement)   GBL_NOEXCEPT;

GBL_EXPORT GblBool              GblLinkedList_erase         (SELF, GblSize index)               GBL_NOEXCEPT;
GBL_INLINE void                 GblLinkedList_clear         (SELF)                              GBL_NOEXCEPT;

GBL_INLINE GblBool              GblLinkedList_empty         (CSELF)                             GBL_NOEXCEPT;
GBL_INLINE GblSize              GblLinkedList_count         (CSELF)                             GBL_NOEXCEPT;
GBL_INLINE GblLinkedListNode*   GblLinkedList_at            (CSELF, GblSize index)              GBL_NOEXCEPT;
GBL_INLINE GblLinkedListNode*   GblLinkedList_front         (CSELF)                             GBL_NOEXCEPT;
GBL_INLINE GblLinkedListNode*   GblLinkedList_back          (CSELF)                             GBL_NOEXCEPT;
GBL_INLINE GblBool              GblLinkedList_contains      (CSELF, GblLinkedListNode* pNode)   GBL_NOEXCEPT;
GBL_INLINE GblSize              GblLinkedList_find          (CSELF, GblLinkedListNode* PNode)   GBL_NOEXCEPT;

GBL_INLINE void                 GblLinkedList_reverse       (SELF)                              GBL_NOEXCEPT;

// ========== IMPL ==========

GBL_INLINE void GblLinkedList_init(SELF) GBL_NOEXCEPT {
    pSelf->pNext = pSelf;
}

GBL_INLINE void GblLinkedList_pushBack(SELF, GblLinkedListNode* pNode) GBL_NOEXCEPT {
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

GBL_INLINE void GblLinkedList_pushFront(SELF, GblLinkedListNode* pNode) GBL_NOEXCEPT {
    pNode->pNext = pSelf->pNext;
    pSelf->pNext = pNode;
}

GBL_INLINE GblBool GblLinkedList_remove(SELF, GblLinkedListNode* pNode) GBL_NOEXCEPT {
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


GBL_INLINE GblSize GblLinkedList_find(CSELF, GblLinkedListNode* pNode) GBL_NOEXCEPT {
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

GBL_INLINE GblBool GblLinkedList_contains(CSELF, GblLinkedListNode* pNode) GBL_NOEXCEPT {
    return GblLinkedList_find(pSelf, pNode) != GBL_LINKED_LIST_NPOS;
}

GBL_INLINE GblBool GblLinkedList_empty(CSELF) GBL_NOEXCEPT {
    return pSelf->pNext == pSelf || pSelf->pNext == GBL_NULL;
}

GBL_INLINE GblSize GblLinkedList_count(CSELF) GBL_NOEXCEPT {
    GblSize count = 0;
    for(GblLinkedListNode* pIt = pSelf->pNext;
        pIt != pSelf;
        pIt = pIt->pNext)
    {
        ++count;
    }
    return count;
}

GBL_INLINE GblLinkedListNode* GblLinkedList_at(CSELF, GblSize index) GBL_NOEXCEPT {
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

GBL_INLINE GblLinkedListNode* GblLinkedList_front(CSELF) GBL_NOEXCEPT {
    return (pSelf->pNext != pSelf)? pSelf->pNext : GBL_NULL;
}

GBL_INLINE GblLinkedListNode* GblLinkedList_back(CSELF) GBL_NOEXCEPT {
    GblLinkedListNode* pPrevIt = (GblLinkedListNode*)pSelf;
    for(GblLinkedListNode* pIt = pSelf->pNext;
        pIt != pSelf;
        pIt = pIt->pNext)
    {
        pPrevIt = pIt;
    }
    return (pPrevIt != pSelf)? pPrevIt : GBL_NULL;
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

GBL_INLINE void GblLinkedList_joinFront(SELF, GblLinkedListNode* pList) GBL_NOEXCEPT {
    if(!GblLinkedList_empty(pList)) {
        GblLinkedList_join_(pList, pSelf, pSelf->pNext);
        GblLinkedList_init(pList);
    }
}


GBL_INLINE void GblLinkedList_joinBack(SELF, GblLinkedListNode* pList) GBL_NOEXCEPT {
    if(!GblLinkedList_empty(pList)) {
        GblLinkedListNode* pBack = GblLinkedList_back(pSelf);
        if(!pBack) pBack = pSelf;
        GblLinkedList_join_(pList, pBack, pSelf);
        GblLinkedList_init(pList);
    }
}

GBL_INLINE void GblLinkedList_moveBack(SELF, GblLinkedListNode* pNode) GBL_NOEXCEPT {
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

GBL_INLINE void GblLinkedList_moveFront(SELF, GblLinkedListNode* pNode) GBL_NOEXCEPT {
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

GBL_INLINE void GblLinkedList_clear(SELF) GBL_NOEXCEPT {
    GblLinkedListNode* pPrev = pSelf;
    GblLinkedListNode* pIt = pSelf->pNext;

    do {
        pPrev->pNext = pPrev;
        pPrev = pIt;
        pIt = pIt->pNext;
    } while(pIt != pSelf);
}

GBL_INLINE GblLinkedListNode* GblLinkedList_popFront(SELF) GBL_NOEXCEPT {
    GblLinkedListNode* pFront = GblLinkedList_front(pSelf);
    if(pFront) pSelf->pNext = pFront->pNext;
    return pFront;
}

GBL_INLINE GblLinkedListNode* GblLinkedList_popBack(SELF) GBL_NOEXCEPT {
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

GBL_INLINE void GblLinkedList_reverse(SELF) GBL_NOEXCEPT {
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


GBL_DECLS_END

#undef CSELF
#undef SELF

#endif // GIMBAL_LINKED_LIST_H

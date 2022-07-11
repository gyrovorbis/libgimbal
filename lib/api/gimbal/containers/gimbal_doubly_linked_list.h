/*! \file
 *  \brief GblDoublyLinkedListNode structure and related functions
 *  \ingroup containers
 */

#ifndef GIMBAL_DOUBLY_LINKED_LIST_HPP
#define GIMBAL_DOUBLY_LINKED_LIST_HPP

#include "gimbal_linked_list.h"

#define SELF    GblDoublyLinkedListNode* pSelf
#define CSELF   const SELF

#define GBL_DOUBLY_LINKED_LIST_NPOS                             GBL_NPOS
#define GBL_DOUBLY_LINKED_LIST_NODE_INITIALIZER()               { .pNext = NULL, .pPrev = NULL }
#define GBL_DOUBLY_LINKED_LIST_NODE(name)                       GblDoublyLinkedListNode name = { .pNext = &name, .pPrev = &name }
#define GBL_DOUBLY_LINKED_LIST_ENTRY(node, structure, field)    GBL_CONTAINER_OF(node, structure, field)

GBL_DECLS_BEGIN

/*! \brief Intrustive doubly linked list structure with vector-style API
 *  \ingroup containers
 */
typedef struct GblDoublyLinkedListNode {

    union {
        struct GblDoublyLinkedListNode* pNext;
        GblLinkedListNode               singleNode;
    };

    struct GblDoublyLinkedListNode*     pPrev;
} GblDoublyLinkedListNode;

GBL_INLINE void     GblDoublyLinkedList_init        (SELF)                                  GBL_NOEXCEPT;

GBL_INLINE void     GblDoublyLinkedList_pushBack    (SELF,
                                                     GblDoublyLinkedListNode* pNode)        GBL_NOEXCEPT;

GBL_INLINE void     GblDoublyLinkedList_pushFront   (SELF,
                                                     GblDoublyLinkedListNode* pNode)        GBL_NOEXCEPT;

GBL_INLINE void     GblDoublyLinkedList_moveBack    (SELF,
                                                     GblDoublyLinkedListNode* pnode)        GBL_NOEXCEPT;

GBL_INLINE void     GblDoublyLinkedList_moveFront   (SELF,
                                                     GblDoublyLinkedListNode* pNode)        GBL_NOEXCEPT;

GBL_INLINE void     GblDoublyLinkedList_joinBack    (SELF,
                                                     GblDoublyLinkedListNode* pList)        GBL_NOEXCEPT;

GBL_INLINE void     GblDoublyLinkedList_joinFront   (SELF,
                                                     GblDoublyLinkedListNode* pList)        GBL_NOEXCEPT;

GBL_INLINE GblDoublyLinkedListNode*
                    GblDoublyLinkedList_popBack     (SELF)                                  GBL_NOEXCEPT;

GBL_INLINE GblDoublyLinkedListNode*
                    GblDoublyLinkedList_popFront    (SELF)                                  GBL_NOEXCEPT;

GBL_INLINE GblBool  GblDoublyLinkedList_empty       (CSELF)                                 GBL_NOEXCEPT;

GBL_INLINE GblSize  GblDoublyLinkedList_count       (CSELF)                                 GBL_NOEXCEPT;

GBL_INLINE GblDoublyLinkedListNode*
                    GblDoublyLinkedList_at          (CSELF, GblSize index)                  GBL_NOEXCEPT;

GBL_INLINE GblDoublyLinkedListNode*
                    GblDoublyLinkedList_front       (CSELF)                                 GBL_NOEXCEPT;

GBL_INLINE GblDoublyLinkedListNode*
                    GblDoublyLinkedList_back        (CSELF)                                 GBL_NOEXCEPT;

GBL_INLINE GblBool  GblDoublyLinkedList_contains    (CSELF,
                                                     GblDoublyLinkedListNode* pNode)        GBL_NOEXCEPT;

GBL_INLINE GblSize  GblDoublyLinkedList_find        (CSELF,
                                                     GblDoublyLinkedListNode* pNode)         GBL_NOEXCEPT;

GBL_INLINE GblBool  GblDoublyLinkedList_swap        (SELF,
                                                     GblDoublyLinkedListNode* pNode1,
                                                     GblDoublyLinkedListNode* pNode2)       GBL_NOEXCEPT;

GBL_INLINE void    GblDoublyLinkedList_remove      (CSELF,
                                                     GblDoublyLinkedListNode* pNode)        GBL_NOEXCEPT;

GBL_INLINE GblBool  GblDoublyLinkedList_replace     (SELF,
                                                     GblDoublyLinkedListNode* pExisting,
                                                     GblDoublyLinkedListNode* pReplacement) GBL_NOEXCEPT;

GBL_EXPORT GblBool  GblDoublyLinkedList_erase       (SELF, GblSize index)                   GBL_NOEXCEPT;

GBL_INLINE void     GblDoublyLinkedList_clear       (SELF)                                  GBL_NOEXCEPT;


// ========== IMPL ==========

GBL_INLINE void GblDoublyLinkedList_init(SELF) GBL_NOEXCEPT {
    pSelf->pNext = pSelf->pPrev = pSelf;
}

GBL_INLINE void GblDoublyLinkedList_pushBack(SELF, GblDoublyLinkedListNode* pNode) GBL_NOEXCEPT {
    GblDoublyLinkedListNode* pLast = pSelf->pPrev;
    pLast->pNext = pNode;
    pNode->pPrev = pLast;
    pNode->pNext = pSelf;
    pSelf->pPrev = pNode;
}

GBL_INLINE void GblDoublyLinkedList_pushFront(SELF, GblDoublyLinkedListNode* pNode) GBL_NOEXCEPT {
    pNode->pNext = pSelf->pNext;
    pSelf->pNext->pPrev = pNode;
    pNode->pPrev = pSelf;
    pSelf->pNext = pNode;
}

GBL_INLINE GblSize GblDoublyLinkedList_find(CSELF, GblDoublyLinkedListNode* pNode) GBL_NOEXCEPT {
    return GblLinkedList_find(&pSelf->singleNode, &pNode->singleNode);
}

GBL_INLINE GblBool GblDoublyLinkedList_contains(CSELF, GblDoublyLinkedListNode* pNode) GBL_NOEXCEPT {
    return GblLinkedList_contains(&pSelf->singleNode, &pNode->singleNode);
}

GBL_INLINE GblBool GblDoublyLinkedList_empty(CSELF) GBL_NOEXCEPT {
    return GblLinkedList_empty(&pSelf->singleNode) && (pSelf->pPrev == pSelf || pSelf->pPrev == NULL);
}

GBL_INLINE GblSize GblDoublyLinkedList_count(CSELF) GBL_NOEXCEPT {
    return GblLinkedList_count(&pSelf->singleNode);
}

GBL_INLINE GblDoublyLinkedListNode* GblDoublyLinkedList_at(CSELF, GblSize index) GBL_NOEXCEPT {
    return (GblDoublyLinkedListNode*)GblLinkedList_at(&pSelf->singleNode, index);
}

GBL_INLINE GblDoublyLinkedListNode* GblDoublyLinkedList_front(CSELF) GBL_NOEXCEPT {
    return (GblDoublyLinkedListNode*)GblLinkedList_front(&pSelf->singleNode);
}

GBL_INLINE GblDoublyLinkedListNode* GblDoublyLinkedList_back(CSELF) GBL_NOEXCEPT {
    return pSelf->pPrev != pSelf? pSelf->pPrev : NULL;
}

GBL_INLINE void GblDoublyLinkedList_remove(CSELF, GblDoublyLinkedListNode* pNode) GBL_NOEXCEPT {
    pNode->pPrev->pNext = pNode->pNext;
    pNode->pNext->pPrev = pNode->pPrev;
    pNode->pNext        = pNode;
    pNode->pPrev        = pNode;
}

GBL_EXPORT GblBool  GblDoublyLinkedList_replace(SELF, GblDoublyLinkedListNode* pExisting, GblDoublyLinkedListNode* pReplacement) GBL_NOEXCEPT {
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

GBL_EXPORT GblBool GblDoublyLinkedList_swap(SELF, GblDoublyLinkedListNode* pExisting, GblDoublyLinkedListNode* pReplacement) GBL_NOEXCEPT {
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

GBL_INLINE void GblDoublyLinkedList_moveBack(SELF, GblDoublyLinkedListNode* pNode) GBL_NOEXCEPT {
    pNode->pPrev->pNext = pNode->pNext;
    pNode->pNext->pPrev = pNode->pPrev;

    GblDoublyLinkedList_pushBack(pSelf, pNode);
}

GBL_INLINE void GblDoublyLinkedList_moveFront(SELF, GblDoublyLinkedListNode* pNode) GBL_NOEXCEPT {
    pNode->pPrev->pNext = pNode->pNext;
    pNode->pNext->pPrev = pNode->pPrev;

    GblDoublyLinkedList_pushFront(pSelf, pNode);
}

GBL_EXPORT GblBool GblDoublyLinkedList_erase(SELF, GblSize index) GBL_NOEXCEPT {
    GblDoublyLinkedListNode* pNode = GblDoublyLinkedList_at(pSelf, index);
    if(pNode) {
        GblDoublyLinkedList_remove(pSelf, pNode);
        return GBL_TRUE;
    } else return GBL_FALSE;
}

GBL_INLINE void GblDoublyLinkedList_clear(SELF) GBL_NOEXCEPT {
    GblDoublyLinkedListNode* pPrev = pSelf;
    GblDoublyLinkedListNode* pIt = pSelf->pNext;

    do {
        pPrev->pNext = pPrev;
        pPrev->pPrev = pPrev;
        pPrev = pIt;
        pIt = pIt->pNext;

    } while(pIt != pSelf);
}

GBL_INLINE void GblDoublyLinkedList_joinBack(SELF, GblDoublyLinkedListNode* pList) GBL_NOEXCEPT {
    if(!GblDoublyLinkedList_empty(pList)) {
        pSelf->pPrev->pNext = pList->pNext;
        pList->pNext->pPrev = pSelf->pPrev;
        pList->pPrev->pNext = pSelf;
        pSelf->pPrev        = pList->pPrev;
        pList->pPrev        = pList;
        pList->pNext        = pList;
    }
}

GBL_INLINE void GblDoublyLinkedList_joinFront(SELF, GblDoublyLinkedListNode* pList) GBL_NOEXCEPT {
    if(!GblDoublyLinkedList_empty(pList)) {
        pSelf->pNext->pPrev = pList->pPrev;
        pList->pPrev->pNext = pSelf->pNext;
        pSelf->pNext        = pList->pNext;
        pList->pNext->pPrev = pSelf;
        pList->pNext        = pList;
        pList->pPrev        = pList;
    }
}


GBL_INLINE GblDoublyLinkedListNode* GblDoublyLinkedList_popBack(SELF) GBL_NOEXCEPT {
    GblDoublyLinkedListNode* pBack = GblDoublyLinkedList_back(pSelf);
    if(pBack) {
        pBack->pPrev->pNext = pBack->pNext;
        pBack->pNext->pPrev = pBack->pPrev;
    }
    return pBack;
}

GBL_INLINE GblDoublyLinkedListNode* GblDoublyLinkedList_popFront(SELF) GBL_NOEXCEPT {
    GblDoublyLinkedListNode* pFront = GblDoublyLinkedList_front(pSelf);
    if(pFront) {
        pFront->pPrev->pNext = pFront->pNext;
        pFront->pNext->pPrev = pFront->pPrev;
    }
    return pFront;
}



GBL_DECLS_END

#undef CSELF
#undef SELF

#endif // GIMBAL_DOUBLY_LINKED_LIST_HPP

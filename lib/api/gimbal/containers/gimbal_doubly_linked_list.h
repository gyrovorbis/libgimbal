/*! \file
 *  \brief GblDoublyLinkedListNode structure and related functions
 *  \ingroup containers
 *  \copydoc GblDoublyLinkedListNode
 *  \todo
 *      - epic BSD-style for loop macros
 *
 *  \author Falco Girgis
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

GBL_EXPORT void           GblDoublyLinkedList_init         (GBL_SELF)                                     GBL_NOEXCEPT;
GBL_EXPORT void           GblDoublyLinkedList_move         (GBL_SELF, GBL_SELF_TYPE* pHead)               GBL_NOEXCEPT;

GBL_EXPORT GblBool        GblDoublyLinkedList_empty        (GBL_CSELF)                                    GBL_NOEXCEPT;
GBL_EXPORT size_t         GblDoublyLinkedList_count        (GBL_CSELF)                                    GBL_NOEXCEPT;

GBL_EXPORT GBL_SELF_TYPE* GblDoublyLinkedList_at           (GBL_CSELF, intptr_t index)                    GBL_NOEXCEPT;
GBL_EXPORT GBL_SELF_TYPE* GblDoublyLinkedList_front        (GBL_CSELF)                                    GBL_NOEXCEPT;
GBL_EXPORT GBL_SELF_TYPE* GblDoublyLinkedList_back         (GBL_CSELF)                                    GBL_NOEXCEPT;
GBL_EXPORT GblBool        GblDoublyLinkedList_contains     (GBL_CSELF, GBL_SELF_TYPE* pNode)              GBL_NOEXCEPT;
GBL_EXPORT size_t         GblDoublyLinkedList_find         (GBL_CSELF, GBL_SELF_TYPE* pNode)              GBL_NOEXCEPT;
GBL_EXPORT GBL_SELF_TYPE* GblDoublyLinkedList_middle       (GBL_CSELF)                                    GBL_NOEXCEPT;
GBL_EXPORT GBL_SELF_TYPE* GblDoublyLinkedList_beforeMiddle (GBL_CSELF)                                    GBL_NOEXCEPT;

GBL_EXPORT void           GblDoublyLinkedList_pushBack     (GBL_SELF, GBL_SELF_TYPE* pNode)               GBL_NOEXCEPT;
GBL_EXPORT void           GblDoublyLinkedList_pushFront    (GBL_SELF, GBL_SELF_TYPE* pNode)               GBL_NOEXCEPT;
GBL_EXPORT void           GblDoublyLinkedList_moveBack     (GBL_SELF, GBL_SELF_TYPE* pNode)               GBL_NOEXCEPT;
GBL_EXPORT void           GblDoublyLinkedList_moveFront    (GBL_SELF, GBL_SELF_TYPE* pNode)               GBL_NOEXCEPT;
GBL_EXPORT GblBool        GblDoublyLinkedList_join         (GBL_SELF, intptr_t idx, GBL_SELF_TYPE* pList) GBL_NOEXCEPT;
GBL_EXPORT void           GblDoublyLinkedList_joinBack     (GBL_SELF, GBL_SELF_TYPE* pList)               GBL_NOEXCEPT;
GBL_EXPORT void           GblDoublyLinkedList_joinFront    (GBL_SELF, GBL_SELF_TYPE* pList)               GBL_NOEXCEPT;
GBL_EXPORT void           GblDoublyLinkedList_joinSorted   (GBL_SELF,
                                                            GBL_SELF_TYPE*           pList,
                                                            GblDoublyLinkedListCmpFn pCmpFn,
                                                            void*                    pClosure)            GBL_NOEXCEPT;

GBL_EXPORT GblBool        GblDoublyLinkedList_insert       (GBL_SELF, intptr_t idx, GBL_SELF_TYPE* pNode) GBL_NOEXCEPT;
GBL_EXPORT void           GblDoublyLinkedList_insertBefore (GBL_SELF_TYPE* pNode1, GBL_SELF_TYPE* pNode2) GBL_NOEXCEPT;
GBL_EXPORT void           GblDoublyLinkedList_insertAfter  (GBL_SELF_TYPE* pNode1, GBL_SELF_TYPE* pNode2) GBL_NOEXCEPT;

GBL_EXPORT GBL_SELF_TYPE* GblDoublyLinkedList_popBack      (GBL_SELF)                                     GBL_NOEXCEPT;
GBL_EXPORT GBL_SELF_TYPE* GblDoublyLinkedList_popFront     (GBL_SELF)                                     GBL_NOEXCEPT;

GBL_EXPORT GblBool        GblDoublyLinkedList_swap         (GBL_SELF_TYPE* pNode1, GBL_SELF_TYPE* pNode2) GBL_NOEXCEPT;
GBL_EXPORT void           GblDoublyLinkedList_remove       (GBL_SELF_TYPE* pNode)                         GBL_NOEXCEPT;
GBL_EXPORT GBL_SELF_TYPE* GblDoublyLinkedList_removeBefore (GBL_SELF_TYPE* pNode)                         GBL_NOEXCEPT;
GBL_EXPORT GBL_SELF_TYPE* GblDoublyLinkedList_removeAfter  (GBL_SELF_TYPE* pNode)                         GBL_NOEXCEPT;
GBL_EXPORT GblBool        GblDoublyLinkedList_replace      (GBL_SELF_TYPE* pNode1, GBL_SELF_TYPE* pNode2) GBL_NOEXCEPT;
GBL_EXPORT void           GblDoublyLinkedList_splitAfter   (GBL_SELF,
                                                            GBL_SELF_TYPE* pHead2,
                                                            GBL_SELF_TYPE* pAfter)                        GBL_NOEXCEPT;

GBL_EXPORT GBL_SELF_TYPE* GblDoublyLinkedList_erase        (GBL_SELF, intptr_t index)                     GBL_NOEXCEPT;
GBL_EXPORT void           GblDoublyLinkedList_clear        (GBL_SELF)                                     GBL_NOEXCEPT;

GBL_EXPORT void           GblDoublyLinkedList_mergeSort    (GBL_SELF,
                                                            GblDoublyLinkedListCmpFn pCmpFn,
                                                            void* pClosure)                               GBL_NOEXCEPT;

GBL_EXPORT void           GblDoublyLinkedList_rotate       (GBL_SELF, intptr_t n)                         GBL_NOEXCEPT;
GBL_EXPORT void           GblDoublyLinkedList_reverse      (GBL_SELF)                                     GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_DOUBLY_LINKED_LIST_HPP

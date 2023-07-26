/*! \file
 *  \brief GblLinkedListNode structure and related functions
 *  \ingroup containers
 *  \todo
 *    - GblLinkedList_insert(): insert into middle by index
 *    - GblLinkedList_insertAfter(): no list head required
 *    - Slick KOS/BSD-style macro for() looping over nodes
 *
 *  \author Falco Girgis
 */

#ifndef GIMBAL_LINKED_LIST_H
#define GIMBAL_LINKED_LIST_H

#include "../core/gimbal_decls.h"

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

GBL_EXPORT void           GblLinkedList_init         (GBL_SELF)                                               GBL_NOEXCEPT;
GBL_EXPORT void           GblLinkedList_move         (GBL_SELF, GBL_SELF_TYPE* pHead)                         GBL_NOEXCEPT;

GBL_EXPORT GblBool        GblLinkedList_empty        (GBL_CSELF)                                              GBL_NOEXCEPT;
GBL_EXPORT size_t         GblLinkedList_count        (GBL_CSELF)                                              GBL_NOEXCEPT;

GBL_EXPORT GBL_SELF_TYPE* GblLinkedList_at           (GBL_CSELF, size_t  index)                               GBL_NOEXCEPT;
GBL_EXPORT GBL_SELF_TYPE* GblLinkedList_front        (GBL_CSELF)                                              GBL_NOEXCEPT;
GBL_EXPORT GBL_SELF_TYPE* GblLinkedList_back         (GBL_CSELF)                                              GBL_NOEXCEPT;
GBL_EXPORT GblBool        GblLinkedList_contains     (GBL_CSELF, GBL_SELF_TYPE* pNode)                        GBL_NOEXCEPT;
GBL_EXPORT size_t         GblLinkedList_find         (GBL_CSELF, GBL_SELF_TYPE* PNode)                        GBL_NOEXCEPT;
GBL_EXPORT GBL_SELF_TYPE* GblLinkedList_middle       (GBL_CSELF)                                              GBL_NOEXCEPT;
GBL_EXPORT GBL_SELF_TYPE* GblLinkedList_beforeMiddle (GBL_CSELF)                                              GBL_NOEXCEPT;

GBL_EXPORT void           GblLinkedList_pushBack     (GBL_SELF, GBL_SELF_TYPE* pNode)                         GBL_NOEXCEPT;
GBL_EXPORT void           GblLinkedList_pushFront    (GBL_SELF, GBL_SELF_TYPE* pNode)                         GBL_NOEXCEPT;
GBL_EXPORT void           GblLinkedList_moveBack     (GBL_SELF, GBL_SELF_TYPE* pNode)                         GBL_NOEXCEPT;
GBL_EXPORT void           GblLinkedList_moveFront    (GBL_SELF, GBL_SELF_TYPE* pNode)                         GBL_NOEXCEPT;
GBL_EXPORT void           GblLinkedList_joinBack     (GBL_SELF, GBL_SELF_TYPE* pList)                         GBL_NOEXCEPT;
GBL_EXPORT void           GblLinkedList_joinFront    (GBL_SELF, GBL_SELF_TYPE* pList)                         GBL_NOEXCEPT;
GBL_EXPORT void           GblLinkedList_joinSorted   (GBL_SELF,
                                                      GBL_SELF_TYPE*     pList,
                                                      GblLinkedListCmpFn pCmpFn,
                                                      void*              pCl)                                 GBL_NOEXCEPT;

GBL_EXPORT GblBool        GblLinkedList_insert       (GBL_SELF, GBL_SELF_TYPE* pNode, size_t  index)          GBL_NOEXCEPT;
GBL_EXPORT void           GblLinkedList_insertAfter  (GBL_SELF_TYPE* pNode1, GBL_SELF_TYPE* pNode2)           GBL_NOEXCEPT;

GBL_EXPORT GBL_SELF_TYPE* GblLinkedList_popBack      (GBL_SELF)                                               GBL_NOEXCEPT;
GBL_EXPORT GBL_SELF_TYPE* GblLinkedList_popFront     (GBL_SELF)                                               GBL_NOEXCEPT;

GBL_EXPORT GblBool        GblLinkedList_swap         (GBL_SELF, GBL_SELF_TYPE* pNode1, GBL_SELF_TYPE* pNode2) GBL_NOEXCEPT;
GBL_EXPORT GblBool        GblLinkedList_remove       (GBL_SELF, GBL_SELF_TYPE* pNode)                         GBL_NOEXCEPT;
GBL_EXPORT GblBool        GblLinkedList_replace      (GBL_SELF, GBL_SELF_TYPE* pNode1, GBL_SELF_TYPE* pNode2) GBL_NOEXCEPT;
GBL_EXPORT void           GblLinkedList_splitAfter   (GBL_SELF, GBL_SELF_TYPE* pHead2, GBL_SELF_TYPE* pAfter) GBL_NOEXCEPT;

GBL_EXPORT GBL_SELF_TYPE* GblLinkedList_erase        (GBL_SELF, size_t  index)                                GBL_NOEXCEPT;
GBL_EXPORT void           GblLinkedList_clear        (GBL_SELF)                                               GBL_NOEXCEPT;

GBL_EXPORT void           GblLinkedList_mergeSort    (GBL_SELF, GblLinkedListCmpFn pCmpFn, void* pClosure)    GBL_NOEXCEPT;

GBL_EXPORT void           GblLinkedList_reverse      (GBL_SELF)                                               GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_LINKED_LIST_H

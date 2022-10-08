/*! \file
 *  \brief GblRingList structure and related functions
 *  \ingroup containers
 *  \todo
 *    GblRingList_sort()
 */

#ifndef GIMBAL_RING_LIST_H
#define GIMBAL_RING_LIST_H

#include "gimbal_doubly_linked_list.h"

#define GBL_RING_LIST_NPOS  GBL_NPOS
#define GBL_SELF_TYPE       GblRingList

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblRingList);

typedef GblBool    (*GblRingListIterFn)(void* pValue, void* pClosure);
typedef int        (*GblRingListCmpFn) (const void* pVal1, const void* pVal2, void* pClosure);
typedef void*      (*GblRingListCopyFn)(const void* pValue, void* pClosure);
typedef GBL_RESULT (*GblRingListDtorFn)(void* pValue, void* pClosure);

/*! \brief Non-intrusive circularly doubly linked list with C++-style STL API
 *
 *  GblRingList is a non-intrusive circular doubly-linked
 *  list with an API mirroring C++'s std::vector, or that of
 *  an abstract List from other languges. All values are stored
 *  in the form of void* pointers.
 *
 *  Each entry within the list is maintained as a simple
 *  GblDoublyLinkedListNode connecting it to the prevous and next
 *  entries along with a void* as a data payload (with the list
 *  head storing its size in that union field instead). While
 *  these nodes are directly accessible, a higher-level API
 *  is presented, which manages allocations and node relationships
 *  internally.
 *
 *  The GblRingList API has been tweaked for convenience within
 *  the C programming language. This is why many of the methods
 *  are provided as overridden macros implementing default values
 *  and function overloading for doing things such as inserting
 *  or removing multiple entries at once.
 *
 *  Operation                         |Time Complexity
 *  ----------------------------------|------------------
 *  iteration                         | O(N)
 *  insertion/removal (middle)        | O(N)
 *  insertion/removal (front or back) | O(1)
 *  access (front or back)            | O(1)
 *  random access (middle)            | O(N)
 *
 *  \note
 *  As it's non-intrusive, every list entry must dynamically
 *  allocate a new node when a value is added to the list,
 *  which would normally result in lots of small, disjoint
 *  heap allocations, causing fragmentation. However, this
 *  is not the case with GblRingList. LibGimbal uses an
 *  arena allocator internally to efficiently allocate
 *  each node from contiguous blocks of data, also
 *  maintaining a free list to reuse nodes as they become
 *  available.
 *  \sa GblDoublyLinkedListNode
 *  \ingroup containers
 */
typedef struct GblRingList {
    union {                                 // Size (32-bit / 64-bit)
        GblDoublyLinkedListNode listNode;
        struct {
            struct GblRingList* pNext;      // 4/8      bytes
            struct GblRingList* pPrev;      // 4/8      bytes
        } ringNode;
    };
    union {
        uintptr_t size;                     // 4/8      bytes
        void*     pData;
    };
} GblRingList;                              // 12/24    total

// === Regular Methods ====
GBL_EXPORT GblRingList* GblRingList_createEmpty  (void)                                           GBL_NOEXCEPT;
GBL_EXPORT GblRingList* GblRingList_create       (void* pData, ...)                               GBL_NOEXCEPT;
GBL_EXPORT GblRingList* GblRingList_copy         (GBL_CSELF, GblRingListCopyFn pFnCpy, void* pCl) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblRingList_destroy      (GBL_SELF, GblRingListDtorFn pFnDtor, void* pCl) GBL_NOEXCEPT;

GBL_EXPORT GblSize      GblRingList_size         (GBL_CSELF)                                      GBL_NOEXCEPT;
GBL_EXPORT GblBool      GblRingList_empty        (GBL_CSELF)                                      GBL_NOEXCEPT;

GBL_EXPORT void*        GblRingList_front        (GBL_CSELF)                                      GBL_NOEXCEPT;
GBL_EXPORT void*        GblRingList_back         (GBL_CSELF)                                      GBL_NOEXCEPT;
GBL_EXPORT void*        GblRingList_at           (GBL_CSELF, intptr_t index)                      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblRingList_pushBack     (GBL_SELF, ...)                                  GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblRingList_pushFront    (GBL_SELF, ...)                                  GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblRingList_insert       (GBL_SELF, intptr_t index, ...)                  GBL_NOEXCEPT;
GBL_EXPORT void*        GblRingList_replace      (GBL_SELF, intptr_t index, void* pData)          GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblRingList_insertSorted (GBL_SELF,
                                                  void*            pData,
                                                  GblRingListCmpFn pFnCmp,
                                                  void*            pCl)                           GBL_NOEXCEPT;
GBL_EXPORT GblBool      GblRingList_join         (GBL_SELF,
                                                  intptr_t         index,
                                                  GblRingList*     pOther)                        GBL_NOEXCEPT;

GBL_EXPORT void*        GblRingList_popBack      (GBL_SELF, GblSize count)                        GBL_NOEXCEPT;
GBL_EXPORT void*        GblRingList_popFront     (GBL_SELF, GblSize count)                        GBL_NOEXCEPT;
GBL_EXPORT void*        GblRingList_remove       (GBL_SELF, intptr_t index, GblSize count)        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblRingList_clear        (GBL_SELF)                                       GBL_NOEXCEPT;

GBL_EXPORT void         GblRingList_sort         (GBL_SELF, GblRingListCmpFn pFnCmp, void* pCl)   GBL_NOEXCEPT;
GBL_EXPORT void         GblRingList_rotate       (GBL_SELF, intptr_t n)                           GBL_NOEXCEPT;
GBL_EXPORT void         GblRingList_reverse      (GBL_SELF)                                       GBL_NOEXCEPT;
GBL_EXPORT GblBool      GblRingList_foreach      (GBL_SELF, GblRingListIterFn pFnIt, void* pCl)   GBL_NOEXCEPT;
GBL_EXPORT GblSize      GblRingList_find         (GBL_CSELF,
                                                  const void*      pVal,
                                                  GblRingListCmpFn pFnCmp,
                                                  void*            pCl)                           GBL_NOEXCEPT;

// ==== Macro Overrides (for default arguments) ====
#define GblRingList_create(...)                 (GblRingList_create)(__VA_ARGS__, GBL_NULL)
#define GblRingList_copy(...)                   GblRingList_copyDefault_(__VA_ARGS__)
#define GblRingList_destroy(...)                GblRingList_destroyDefault_(__VA_ARGS__)
#define GblRingList_pushBack(self, ...)         (GblRingList_pushBack)(self, __VA_ARGS__, GBL_NULL)
#define GblRingList_pushFront(self, ...)        (GblRingList_pushFront)(self, __VA_ARGS__, GBL_NULL)
#define GblRingList_insert(self, ...)           (GblRingList_insert)(self, __VA_ARGS__, GBL_NULL)
#define GblRingList_insertSorted(...)           GblRingList_insertSortedDefault_(__VA_ARGS__)
#define GblRingList_popBack(...)                GblRingList_popBackDefault_(__VA_ARGS__)
#define GblRingList_popFront(...)               GblRingList_popFrontDefault_(__VA_ARGS__)
#define GblRingList_remove(...)                 GblRingList_removeDefault_(__VA_ARGS__)
#define GblRingList_sort(...)                   GblRingList_sortDefault_(__VA_ARGS__)
#define GblRingList_foreach(...)                GblRingList_foreachDefault_(__VA_ARGS__)
#define GblRingList_find(...)                   GblRingList_findDefault_(__VA_ARGS__)

// ===== IMPL =====

/// \cond
#define GblRingList_copyDefault_(...)  \
    GblRingList_copyDefault__(__VA_ARGS__, GBL_NULL, GBL_NULL)
#define GblRingList_copyDefault__(list, cpFn, cl, ...) \
    (GblRingList_copy)(list, cpFn, cl)

#define GblRingList_destroyDefault_(...) \
    GblRingList_destroyDefault__(__VA_ARGS__, GBL_NULL, GBL_NULL)
#define GblRingList_destroyDefault__(list, dtor, cl, ...)  \
    (GblRingList_destroy)(list, dtor, cl)

#define GblRingList_insertSortedDefault_(...) \
    GblRingList_insertSortedDefault__(__VA_ARGS__, GBL_NULL, GBL_NULL)
#define GblRingList_insertSortedDefault__(list, data, cmp, cl, ...) \
    (GblRingList_insertSorted)(list, data, cmp, cl)

#define GblRingList_popBackDefault_(...) \
    GblRingList_popBackDefault__(__VA_ARGS__, 1)
#define GblRingList_popBackDefault__(list, count, ...) \
    (GblRingList_popBack)(list, count)

#define GblRingList_popFrontDefault_(...) \
    GblRingList_popFrontDefault__(__VA_ARGS__, 1)
#define GblRingList_popFrontDefault__(list, count, ...) \
    (GblRingList_popFront)(list, count)

#define GblRingList_removeDefault_(...) \
    GblRingList_removeDefault__(__VA_ARGS__, 1)
#define GblRingList_removeDefault__(list, idx, count, ...) \
    (GblRingList_remove)(list, idx, count)

#define GblRingList_sortDefault_(...) \
    GblRingList_sortDefault__(__VA_ARGS__, GBL_NULL, GBL_NULL)
#define GblRingList_sortDefault__(list, cmp, cl, ...) \
    (GblRingList_sort)(list, cmp, cl)

#define GblRingList_foreachDefault_(...) \
    GblRingList_foreachDefault__(__VA_ARGS__, GBL_NULL)
#define GblRingList_foreachDefault__(list, it, cl, ...) \
    (GblRingList_foreach)(list, it, cl)

#define GblRingList_findDefault_(...) \
    GblRingList_findDefault__(__VA_ARGS__, GBL_NULL, GBL_NULL)
#define GblRingList_findDefault__(list, val, cmp, cl, ...) \
    (GblRingList_find)(list, val, cmp, cl)
/// \endcond

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_RING_LIST_H

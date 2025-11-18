/*! \file
 *  \brief GblRingList structure and related functions
 *  \ingroup containers
 *
 *  This file provides the GblRingList container structure and its public API
 *  for operating upon it.
 *
 *  \author    2023, 2025 Falco Girgis
 *  \copyright MIT License
 */
#ifndef GIMBAL_RING_LIST_H
#define GIMBAL_RING_LIST_H

#include <stdarg.h>

#include "../core/gimbal_result.h"
#include <gimbal/meta/classes/gimbal_opaque.h>
#include <gimbal/meta/ifaces/gimbal_itable_variant.h>
#include "gimbal_doubly_linked_list.h"

//! size_t type denoting an invalid array index or position within a GblRingList
#define GBL_RING_LIST_NPOS  GBL_NPOS

#define GBL_SELF_TYPE       GblRingList

/*! \name Type System
 *  \brief Type UUID and cast macros
 *  @{
 */
#define GBL_RING_LIST_TYPE         (GBL_TYPEID(GblRingList))             //!< Type UUID for GblRingList
#define GBL_RING_LIST_CLASS(klass) (GBL_CLASS_CAST(GblRingList, klass))  //!< Casts a GblClass to a GblRingListClass
//! @}

/*! Iterates over every entry within a GblRingList, setting the \p item variable to the current entry's value.
    Optionally takes in a \p type parameter to specify the type of the \p item variable, which defaults to void*.
*/
#define GblRingList_foreach(/*list, item, type*/...)                    GblRingList_foreachDefault_(__VA_ARGS__)

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblRingList);

/*! \struct     GblRingListClass
 *  \extends    GblOpaqueClass
 *  \implements GblITableVariantClass
 *  \brief      GblClass structure for GblRingList
 *
 *  GblRingListClass is the class structure for the GblRingList container.
 *  It also provides an inner GblType that defaults to GBL_POINTER_TYPE,
 *  used for its GblITableVariant implementation.
 */
GBL_CLASS_DERIVE(GblRingList, GblOpaque, GblITableVariant)
    GblType innerType;
GBL_CLASS_END

/*! \name  User-Operator Callbacks
 *  \brief Typedefs for applying various user-supplied functions over a ring list.
 *  @{
*/
//! Iterator callback function which gets called over every entry within a GblRingList, being passed back its stored value and an arbitrary closure, returning true should iteration cease early.
typedef GblBool    (*GblRingListIterFn)(void* pValue, void* pClosure);
//! Comparator callback for comparing the values of two different entries within a GblRingList, also getting passed back the closure that was passed to the API call. 0 should be returned if equal.
typedef int        (*GblRingListCmpFn) (const void* pVal1, const void* pVal2, void* pClosure);
//! Returns a copy of the value of each entry within a GblRingList, also getting passed back the closure that was passed to the initial API call.
typedef void*      (*GblRingListCopyFn)(const void* pValue, void* pClosure);
//! Destructs the value of each entry within a GblRingList, also getting passed back the closure that was passed to the iniital API call, and returning a GBL_RESULT for the status.
typedef GBL_RESULT (*GblRingListDtorFn)(void* pValue, void* pClosure);
//! @}

/*! \brief Non-intrusive circularly doubly linked list with C++-style STL API
 *  \ingroup containers
 *
 *  GblRingList is a non-intrusive circular doubly-linked
 *  list with an API mirroring C++'s std::vector, or that of
 *  an abstract List from other languges, except for the list
 *  can also be indexed from back-to-front with negative indices.
 *
 *  All values are stored in the form of void* pointers.
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
 *  Each node should be 12 bytes large on 32-bit and 24-bytes
 *  large on 64-bit architectures.
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
 *
 *  \sa GblDoublyLinkedListNode, GblRingList
 */
typedef struct GblRingList {
    union {                                 //!< Unionization of linked list node pointers, so they can be referred to generically or strongly-typed.
        GblDoublyLinkedListNode listNode;   //!< Linked list node as a generic GblDoublyLinkedListNode type.
        struct {
            struct GblRingList* pNext;      //!< Points to next node within the list (or wraps back to first node, if list tail).
            struct GblRingList* pPrev;      //!< Points to previous node within the list (or wraps back to last, if list head)
        } ringNode;                         //!< Linked list node as strongly-typed GblRingList pointers.
    };
    union {                                 //!< Unionization between payload data types for the list head and list nodes.
        uintptr_t size;                     //!< List head contains the total number of nodes within the list.
        void*     pData;                    //!< List entries contain void* pointers to arbitrary data payloads.
    };
} GblRingList;

GblType GblRingList_type(void);

/*! \name  Lifetime
 *  \brief Methods for creating, acquring, and releasing a list.
 *  @{
 */
//! Creates a new, empty GblRingList of size 0 and returns a reference to it.
GBL_EXPORT GblRingList* GblRingList_createEmpty (void)                           GBL_NOEXCEPT;
//! Creates a new GblRingList, given an initial value and an optional list of additional values, returning a reference to the list.
GBL_EXPORT GblRingList* GblRingList_create      (void* pData, ...)               GBL_NOEXCEPT;
//! Unreferences an existing GblRingList, destroying it if this was the last reference, using an optional, user-provided per-entry destructor which gets passed back the given optional closure data.
GBL_EXPORT GblRefCount  GblRingList_unref       (GBL_CSELF,
                                                 GblRingListDtorFn pFnDtor/*=NULL*/,
                                                 void*             pCl/*=NULL*/) GBL_NOEXCEPT;
//! Returns a new reference to an existing GblRingList, incrementing its reference counter by 1.
GBL_EXPORT GblRingList* GblRingList_ref         (GBL_CSELF)                      GBL_NOEXCEPT;
//! Creates a copy of the given GblRingList, optionally deep-copying each entry by using the user-provided copy function which optionally gets passed back a pointer to arbitrary closure data.
GBL_EXPORT GblRingList* GblRingList_copy        (GBL_CSELF,
                                                 GblRingListCopyFn pFnCpy/*=NULL*/,
                                                 void*             pCl/*=NULL*/) GBL_NOEXCEPT;
//! @}

/*! \name  Properties
 *  \brief Methods for querying the properties of a list.
 *  @{
 */
//! Returns the number of active references to the given GblRingList.
GBL_EXPORT GblRefCount GblRingList_refCount (GBL_CSELF) GBL_NOEXCEPT;
//! Returns the number of entries held by the given GblRingList.
GBL_EXPORT size_t      GblRingList_size     (GBL_CSELF) GBL_NOEXCEPT;
//! Returns true if the given GblRingList has no entries held within it.
GBL_EXPORT GblBool     GblRingList_empty    (GBL_CSELF) GBL_NOEXCEPT;
//! @}

/*! \name  Retrieval
 *  \brief Methods for retrieving entries within a list.
 *  @{
 */
//! Returns a pointer to the value held within the first entry of the list, or NULL if the list is empty.
GBL_EXPORT void*  GblRingList_front (GBL_CSELF)                     GBL_NOEXCEPT;
//! Returns a pointer to the value held within the last entry of the list, or NULL if the list is empty.
GBL_EXPORT void*  GblRingList_back  (GBL_CSELF)                     GBL_NOEXCEPT;
//! Linearly traverses the list to the given \p index, returning a pointer to the value held within that entry, or NULL if the entry is invalid. NOTE that negative indices start at the last entry and enumerate backwards.
GBL_EXPORT void*  GblRingList_at    (GBL_CSELF, intptr_t index)     GBL_NOEXCEPT;
//! Searches the list for an entry with the given value, or for an entry which matches the value based on the optional comparison callback, which takes an optional closure pointer. The index of the matching entry or `GBL_RING_LIST_NPOS` is returned.
GBL_EXPORT size_t GblRingList_find  (GBL_CSELF,
                                     const void*      pVal,
                                     GblRingListCmpFn pFnCmp/*=NULL*/,
                                     void*            pCl/*=NULL*/) GBL_NOEXCEPT;

//! @}

/*! \name  Insertion
 *  \brief Methods for adding entries into the list.
 * @{
 */
//! Adds a comma-separated list of values to the end of the given GblRingList.
GBL_EXPORT GBL_RESULT GblRingList_pushBack        (GBL_SELF, ...)                 GBL_NOEXCEPT;
//! Equivalent to GblRingList_pushBack(), except taking a va_list*, rather than a variadic argument list.
GBL_EXPORT GBL_RESULT GblRingList_pushBackVaList  (GBL_SELF, va_list* pList)      GBL_NOEXCEPT;
//! Adds a comma-separated list of values to the front of the given GblRingList.
GBL_EXPORT GBL_RESULT GblRingList_pushFront       (GBL_SELF, ...)                 GBL_NOEXCEPT;
//! Equivalent to GblRingList_pushFront(), except taking a va_list*, rather than a variadic argument list.
GBL_EXPORT GBL_RESULT GblRingList_pushFrontVaList (GBL_SELF, va_list* plist)      GBL_NOEXCEPT;
//! Inserts the comma-separated list of values into the given GblRingList, so that the first value is at the position of the given index.
GBL_EXPORT GBL_RESULT GblRingList_insert          (GBL_SELF, intptr_t index, ...) GBL_NOEXCEPT;
//! Equivalent to GblRingList_insert(), except taking a va_list* rather than a variadic argument list.
GBL_EXPORT GBL_RESULT GblRingList_insertVaList    (GBL_SELF,
                                                   intptr_t index,
                                                   va_list* pList)                GBL_NOEXCEPT;
//! Inserts a value into a sorted list, optionally taking a comparator for sort direction and an optional closure which gets passed back.
GBL_EXPORT GBL_RESULT GblRingList_insertSorted    (GBL_SELF,
                                                   void*            pData,
                                                   GblRingListCmpFn pFnCmp/*=NULL*/,
                                                   void*            pCl/*=NULL*/) GBL_NOEXCEPT;
//! Joins \p pOther into the given GblRingList such that its first entry is positioned at the given \p index, returning GBL_TRUE if it succeeded. Do NOT forget to still call GblRingList_unref() on the now empty \p pOther list.
GBL_EXPORT GblBool    GblRingList_splice          (GBL_SELF,
                                                   GblRingList* pOther,
                                                   int32_t      index)            GBL_NOEXCEPT;
//! Replaces the entry within the given GblRingList located at \p pIndex with the \p pData value, returning its old value (or NULL upon failure).
GBL_EXPORT void*      GblRingList_replace         (GBL_SELF,
                                                   intptr_t index,
                                                   void*    pData)                GBL_NOEXCEPT;
//! @}

/*! \name  Removing
 *  \brief Methods for removing entries from a list.
 *  @{
 */
//! Removes the last entry from the given list, returning its previously held value, or NULL if the list was empty.
GBL_EXPORT void*      GblRingList_popBack  (GBL_SELF, size_t count) GBL_NOEXCEPT;
//! Removes the first entry from the given list, returning its previously held value, or NULL if the list was empty.
GBL_EXPORT void*      GblRingList_popFront (GBL_SELF, size_t count) GBL_NOEXCEPT;
//! Removes \p count items from the given GblRingList, starting at the given \p index, returning the value previously held by the last item which was removed.
GBL_EXPORT void*      GblRingList_remove   (GBL_SELF,
                                            intptr_t index,
                                            size_t   count/*=1*/)   GBL_NOEXCEPT;
//! Returns the value which was held by the given GblRingList node within the given list, also removing it from the list.
GBL_EXPORT void*      GblRingList_extract  (GBL_SELF,
                                            GblRingList* pNode)     GBL_NOEXCEPT;
//! Erases all entries from the GblRingList, returning its size back to 0.
GBL_EXPORT GBL_RESULT GblRingList_clear    (GBL_SELF)               GBL_NOEXCEPT;
//! @}

/*! \name  Operations
 *  \brief Other operations which can be performed on a list.
 *   @{
 */
//! Sorts the given GblRingList, optionally using a user-specified comparator which can optionally take an closure data pointer.
GBL_EXPORT void    GblRingList_sort    (GBL_SELF,
                                        GblRingListCmpFn pFnCmp/*=NULL*/,
                                        void*            pCl/*=NULL*/)  GBL_NOEXCEPT;
//! Rotates the positions of all entries within the given GblRingList, such that they ahve been offset by \p n, where positives rotate right and negatives rotate left.
GBL_EXPORT void    GblRingList_rotate  (GBL_SELF, intptr_t n)           GBL_NOEXCEPT;
//! Reverses the order of the entire GblRingList, such that the tail becomes the head entry, and all values are now in opposite order.
GBL_EXPORT void    GblRingList_reverse (GBL_SELF)                       GBL_NOEXCEPT;
//! Iterates over every entry within the given GblRingList, passing their values to the given iterator function, wich may optionally take a closure data pointer. Iteration ends early when the iterator returns GBL_TRUE.
GBL_EXPORT GblBool GblRingList_iterate (GBL_SELF,
                                        GblRingListIterFn pFnIt,
                                        void*             pCl/*=NULL*/) GBL_NOEXCEPT;
//! @}

GBL_DECLS_END

//  ========= BEWARE YOUNG GRUGS, THERE BE COMPLEXITY DEMONS AND EVIL SHIT DEYOND HERE! =========

// ==== Macro Overrides (for default arguments) ====
#define GblRingList_create(...)                 (GblRingList_create)(__VA_ARGS__, GBL_NULL)
#define GblRingList_copy(...)                   GblRingList_copyDefault_(__VA_ARGS__)
#define GblRingList_unref(...)                  GblRingList_unrefDefault_(__VA_ARGS__)
#define GblRingList_pushBack(self, ...)         (GblRingList_pushBack)(self, __VA_ARGS__, GBL_NULL)
#define GblRingList_pushFront(self, ...)        (GblRingList_pushFront)(self, __VA_ARGS__, GBL_NULL)
#define GblRingList_insert(self, ...)           (GblRingList_insert)(self, __VA_ARGS__, GBL_NULL)
#define GblRingList_insertSorted(...)           GblRingList_insertSortedDefault_(__VA_ARGS__)
#define GblRingList_splice(...)                 GblRingList_spliceDefault_(__VA_ARGS__)
#define GblRingList_popBack(...)                GblRingList_popBackDefault_(__VA_ARGS__)
#define GblRingList_popFront(...)               GblRingList_popFrontDefault_(__VA_ARGS__)
#define GblRingList_remove(...)                 GblRingList_removeDefault_(__VA_ARGS__)
#define GblRingList_sort(...)                   GblRingList_sortDefault_(__VA_ARGS__)
#define GblRingList_iterate(...)                GblRingList_iterateDefault_(__VA_ARGS__)
#define GblRingList_find(...)                   GblRingList_findDefault_(__VA_ARGS__)

// ===== IMPL =====

//! \cond
#define GblRingList_copyDefault_(...)  \
    GblRingList_copyDefault__(__VA_ARGS__, GBL_NULL, GBL_NULL)
#define GblRingList_copyDefault__(list, cpFn, cl, ...) \
    (GblRingList_copy)(list, cpFn, cl)

#define GblRingList_unrefDefault_(...) \
    GblRingList_unrefDefault__(__VA_ARGS__, GBL_NULL, GBL_NULL)
#define GblRingList_unrefDefault__(list, dtor, cl, ...)  \
    (GblRingList_unref)(list, dtor, cl)

#define GblRingList_insertSortedDefault_(...) \
    GblRingList_insertSortedDefault__(__VA_ARGS__, GBL_NULL, GBL_NULL)
#define GblRingList_insertSortedDefault__(list, data, cmp, cl, ...) \
    (GblRingList_insertSorted)(list, data, cmp, cl)

#define GblRingList_spliceDefault_(...) \
    GblRingList_spliceDefault__(__VA_ARGS__, -1)
#define GblRingList_spliceDefault__(list1, list2, index, ...) \
    (GblRingList_splice)(list1, list2, index)

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
    GblRingList_foreachDefault__(__VA_ARGS__, void*)
#define GblRingList_foreachDefault__(list, item, type, ...) \
    GblRingList_foreachImpl_(list, item, type)
#define GblRingList_foreachImpl_(list, item, type)                          \
    GblRingList* GBL_APPEND_LINE(pNode) = list->ringNode.pNext;             \
    for(type item = (type)GBL_APPEND_LINE(pNode)->pData;                    \
        GBL_APPEND_LINE(pNode) != list;                                     \
        GBL_APPEND_LINE(pNode) = GBL_APPEND_LINE(pNode)->ringNode.pNext,    \
        item = (type)GBL_APPEND_LINE(pNode)->pData)                         \

#define GblRingList_iterateDefault_(...) \
    GblRingList_iterateDefault__(__VA_ARGS__, GBL_NULL)
#define GblRingList_iterateDefault__(list, it, cl, ...) \
    (GblRingList_iterate)(list, it, cl)

#define GblRingList_findDefault_(...) \
    GblRingList_findDefault__(__VA_ARGS__, GBL_NULL, GBL_NULL)
#define GblRingList_findDefault__(list, val, cmp, cl, ...) \
    (GblRingList_find)(list, val, cmp, cl)
//! \endcond

#undef GBL_SELF_TYPE

#endif // GIMBAL_RING_LIST_H

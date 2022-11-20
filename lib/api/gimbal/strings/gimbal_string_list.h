/*! \file
 *  \brief GblStringList structure and related functions
 *  \ingroup strings
 *  \copydoc GblStringList
 */

#ifndef GIMBAL_STRING_LIST_H
#define GIMBAL_STRING_LIST_H

#include "../containers/gimbal_ring_list.h"
#include "gimbal_string_ref.h"

#define GBL_STRING_LIST_NPOS GBL_RING_LIST_NPOS
#define GBL_SELF_TYPE        GblStringList

GBL_DECLS_BEGIN

typedef GblBool (*GblStringListIterFn)(GblStringRef* pRef, void* pClosure);

/*! \brief List of strings with array-like API
 *
 *  GblStringList is a dynamically resizable list of immutable
 *  strings. It is implemented as a GblRingBuffer where each entry
 *  is a GblStringRef.
 *
 *  \warning Since each entry is a GblStringRef, it should not be directly
 *  modified, as this will modify the value of every other reference to the
 *  same string data. If you must modify an entry, use GblStringList_replaceStr
 *  to replace an existing entry with a new value.
 *
 *  \note Since GblStringList is backed by GblRingList, it gains some
 *  interesting properties. The list can be rotated, reversed, and
 *  indexed from the opposite direction using negative values. Operations
 *  such as middle insertion/deletion and joining with another list do
 *  not require dynamically resizing or moving list entries.
 *
 *  \note Since each entry within the list is a GblStringRef, operations
 *  such as creating sublists, filtering, copying, and any operation that
 *  creates a new list from an existing list are optimized to not actually
 *  copy the values of each individual string. Instead, each derived list
 *  shares a reference to the same memory location.
 *
 *  \note Since each entry within the list is a GblStringRef, there is a
 *  duplicate API provided for when inserting entries into the list which
 *  are already GblStringRefs. Favor the methods ending with "Ref" for
 *  when you already have a reference type, and this will ellide copying.
 *
 *  \ingroup strings
 *  \sa GblStringRef, GblRingBuffer
 */
typedef GblRingList GblStringList;

// ===== Public API =====
GBL_INLINE GblStringList* GblStringList_createEmpty     (void)                                           GBL_NOEXCEPT;
GBL_EXPORT GblStringList* GblStringList_create          (const char* pFirst, ...)                        GBL_NOEXCEPT;
GBL_EXPORT GblStringList* GblStringList_createWithRefs  (GblStringRef* pFirst, ...)                      GBL_NOEXCEPT;
GBL_EXPORT GblStringList* GblStringList_createSplit     (const char* pStr, const char* pDelim)           GBL_NOEXCEPT;
GBL_EXPORT GblStringList* GblStringList_createFromArray (const char** ppStr, GblSize size)               GBL_NOEXCEPT;

GBL_EXPORT GblStringList* GblStringList_createSubList   (const GblStringList* pOther,
                                                         intptr_t             startIndex,
                                                         GblSize              count)                     GBL_NOEXCEPT;

GBL_EXPORT GblStringList* GblStringList_createFilter    (const GblStringList* pOther,
                                                         const char*          pPattern)                  GBL_NOEXCEPT;

GBL_EXPORT GblStringList* GblStringList_copy            (const GblStringList* pOther)                    GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT     GblStringList_destroy         (GBL_SELF)                                       GBL_NOEXCEPT;

GBL_EXPORT GblBool        GblStringList_equals          (GBL_CSELF,
                                                         const GblStringList* pOther,
                                                         GblBool              matchCase)                 GBL_NOEXCEPT;

GBL_INLINE GblSize        GblStringList_size            (GBL_CSELF)                                      GBL_NOEXCEPT;
GBL_INLINE GblBool        GblStringList_empty           (GBL_CSELF)                                      GBL_NOEXCEPT;

GBL_INLINE GblStringRef*  GblStringList_front           (GBL_CSELF)                                      GBL_NOEXCEPT;
GBL_INLINE GblStringRef*  GblStringList_back            (GBL_CSELF)                                      GBL_NOEXCEPT;
GBL_INLINE GblStringRef*  GblStringList_at              (GBL_CSELF, intptr_t index)                      GBL_NOEXCEPT;

GBL_EXPORT GblSize        GblStringList_find            (GBL_CSELF,
                                                         const char* pStr,
                                                         GblBool     matchCase,
                                                         GblSize     offset)                             GBL_NOEXCEPT;

GBL_EXPORT GblSize        GblStringList_rfind           (GBL_CSELF,
                                                         const char* pStr,
                                                         GblBool     matchCase,
                                                         GblSize     offset)                             GBL_NOEXCEPT;

GBL_INLINE GblBool        GblStringList_contains        (GBL_CSELF, const char* pStr, GblBool matchCase) GBL_NOEXCEPT;
GBL_EXPORT GblSize        GblStringList_count           (GBL_CSELF, const char* pStr, GblBool matchCase) GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT     GblStringList_set             (GBL_SELF, intptr_t index, const char* pData)    GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT     GblStringList_setRef          (GBL_SELF, intptr_t index, GblStringRef* pRef)   GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT     GblStringList_pushBack        (GBL_SELF, ...)                                  GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT     GblStringList_pushBackRefs    (GBL_SELF, ...)                                  GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT     GblStringList_pushFront       (GBL_SELF, ...)                                  GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT     GblStringList_pushFrontRefs   (GBL_SELF, ...)                                  GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT     GblStringList_insert          (GBL_SELF, intptr_t index, ...)                  GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT     GblStringList_insertRefs      (GBL_SELF, intptr_t index, ...)                  GBL_NOEXCEPT;

GBL_EXPORT GblSize        GblStringList_replace         (GBL_SELF,
                                                         const char* pOld,
                                                         const char* pNew,
                                                         GblBool     matchCase,
                                                         GblSize     limit)                              GBL_NOEXCEPT;

GBL_EXPORT GblSize        GblStringList_replaceWithRef  (GBL_SELF,
                                                         const char*   pOld,
                                                         GblStringRef* pNew,
                                                         GblBool       matchCase,
                                                         GblSize       limit)                            GBL_NOEXCEPT;

GBL_INLINE GblBool        GblStringList_splice          (GBL_SELF,
                                                         GblStringList* pOther,
                                                         intptr_t       index)                           GBL_NOEXCEPT;

GBL_EXPORT GblStringRef*  GblStringList_join            (GBL_CSELF,
                                                         const char* pSeparator)                         GBL_NOEXCEPT;

GBL_INLINE GblStringRef*  GblStringList_popBack         (GBL_SELF)                                       GBL_NOEXCEPT;
GBL_INLINE GblStringRef*  GblStringList_popFront        (GBL_SELF)                                       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT     GblStringList_erase           (GBL_SELF, intptr_t index, GblSize count)        GBL_NOEXCEPT;
GBL_EXPORT GblSize        GblStringList_remove          (GBL_SELF, const char* pStr, GblBool matchCase)  GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT     GblStringList_deduplicate     (GBL_SELF, GblBool matchCase)                    GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT     GblStringList_clear           (GBL_SELF)                                       GBL_NOEXCEPT;
GBL_EXPORT void           GblStringList_sort            (GBL_SELF, GblBool descending)                   GBL_NOEXCEPT;
GBL_INLINE void           GblStringList_rotate          (GBL_SELF, intptr_t n)                           GBL_NOEXCEPT;
GBL_INLINE void           GblStringList_reverse         (GBL_SELF)                                       GBL_NOEXCEPT;
GBL_INLINE GblBool        GblStringList_foreach         (GBL_SELF, GblStringListIterFn pFnIt, void* pCl) GBL_NOEXCEPT;

// Convenience macro overloads with default arguments
#define                   GblStringList_create(...)             (GblStringList_create)(__VA_ARGS__, GBL_NULL)
#define                   GblStringList_createWithRefs(...)     (GblStringList_createWithRefs)(__VA_ARGS__, GBL_NULL)
#define                   GblStringList_createFromArray(...)    GblStringList_createFromArrayDefault_(__VA_ARGS__)
#define                   GblStringList_equals(...)             GblStringList_equalsDefault_(__VA_ARGS__)
#define                   GblStringList_find(...)               GblStringList_findDefault_(__VA_ARGS__)
#define                   GblStringList_rfind(...)              GblStringList_rfindDefault_(__VA_ARGS__)
#define                   GblStringList_contains(...)           GblStringList_containsDefault_(__VA_ARGS__)
#define                   GblStringList_count(...)              GblStringList_countDefault_(__VA_ARGS__)
#define                   GblStringList_pushBack(...)           (GblStringList_pushBack)(__VA_ARGS__, GBL_NULL)
#define                   GblStringList_pushBackRefs(...)       (GblStringList_pushBackRefs)(__VA_ARGS__, GBL_NULL)
#define                   GblStringList_pushFront(...)          (GblStringList_pushFront)(__VA_ARGS__, GBL_NULL)
#define                   GblStringList_pushFrontRefs(...)      (GblStringList_pushFrontRefs)(__VA_ARGS__, GBL_NULL)
#define                   GblStringList_insert(...)             (GblStringList_insert)(__VA_ARGS__, GBL_NULL)
#define                   GblStringList_insertRefs(...)         (GblStringList_insertRefs)(__VA_ARGS__, GBL_NULL)
#define                   GblStringList_replace(...)            GblStringList_replaceDefault_(__VA_ARGS__)
#define                   GblStringList_replaceWithRef(...)     GblStringList_replaceWithRefDefault_(__VA_ARGS__)
#define                   GblStringList_splice(...)             GblStringList_spliceDefault_(__VA_ARGS__)
#define                   GblStringList_erase(...)              GblStringList_eraseDefault_(__VA_ARGS__)
#define                   GblStringList_remove(...)             GblStringList_removeDefault_(__VA_ARGS__)
#define                   GblStringList_deduplicate(...)        GblStringList_deduplicateDefault_(__VA_ARGS__)
#define                   GblStringList_sort(...)               GblStringList_sortDefault_(__VA_ARGS__)
#define                   GblStringList_foreach(...)            GblStringList_foreachDefault_(__VA_ARGS__)

// ===== Private Implementation =====
///\cond
#define GblStringList_createFromArrayDefault_(...) \
    GblStringList_createFromArrayDefault__(__VA_ARGS__, GBL_STRING_LIST_NPOS)
#define GblStringList_createFromArrayDefault__(array, len, ...) \
    (GblStringList_createFromArray)(array, len)

#define GblStringList_equalsDefault_(...) \
    GblStringList_equalsDefault__(__VA_ARGS__, GBL_TRUE)
#define GblStringList_equalsDefault__(list1, list2, match, ...) \
    (GblStringList_equals(list1, list2, match))

#define GblStringList_findDefault_(...) \
    GBL_VA_OVERLOAD_CALL_ARGC(GblStringList_findDefault, __VA_ARGS__)
#define GblStringList_findDefault_2(list, str) \
    GblStringList_findDefault_3(list, str, GBL_TRUE)
#define GblStringList_findDefault_3(list, str, match) \
    GblStringList_findDefault_4(list, str, match, 0)
#define GblStringList_findDefault_4(list, str, match, offset) \
    (GblStringList_find)(list, str, match, offset)

#define GblStringList_rfindDefault_(...) \
    GBL_VA_OVERLOAD_CALL_ARGC(GblStringList_rfindDefault, __VA_ARGS__)
#define GblStringList_rfindDefault_2(list, str) \
    GblStringList_rfindDefault_3(list, str, GBL_TRUE)
#define GblStringList_rfindDefault_3(list, str, match) \
    GblStringList_rfindDefault_4(list, str, match, GBL_STRING_LIST_NPOS)
#define GblStringList_rfindDefault_4(list, str, match, offset) \
    (GblStringList_rfind)(list, str, match, offset)

#define GblStringList_containsDefault_(...) \
    GblStringList_containsDefault__(__VA_ARGS__, GBL_TRUE)
#define GblStringList_containsDefault__(list, str, match, ...) \
    (GblStringList_contains)(list, str, match)

#define GblStringList_countDefault_(...) \
    GblStringList_countDefault__(__VA_ARGS__, GBL_TRUE)
#define GblStringList_countDefault__(list, str, match, ...) \
    (GblStringList_count)(list, str, match)

#define GblStringList_replaceDefault_(...) \
    GBL_VA_OVERLOAD_CALL_ARGC(GblStringList_replaceDefault, __VA_ARGS__)
#define GblStringList_replaceDefault_3(list, old, repl) \
    GblStringList_replaceDefault_4(list, old, repl, GBL_TRUE)
#define GblStringList_replaceDefault_4(list, old, repl, match) \
    GblStringList_replaceDefault_5(list, old, repl, match, 0)
#define GblStringList_replaceDefault_5(list, old, repl, match, offset) \
    (GblStringList_replace)(list, old, repl, match, offset)

#define GblStringList_replaceWithRefDefault_(...) \
    GBL_VA_OVERLOAD_CALL_ARGC(GblStringList_replaceWithRefDefault, __VA_ARGS__)
#define GblStringList_replaceWithRefDefault_3(list, old, repl) \
    GblStringList_replaceWithRefDefault_4(list, old, repl, GBL_TRUE)
#define GblStringList_replaceWithRefDefault_4(list, old, repl, match) \
    GblStringList_replaceWithRefDefault_5(list, old, repl, match, 0)
#define GblStringList_replaceWithRefDefault_5(list, old, repl, match, offset) \
    (GblStringList_replaceWithRef)(list, old, repl, match, offset)

#define GblStringList_spliceDefault_(...) \
    GblStringList_spliceDefault__(__VA_ARGS__, -1)
#define GblStringList_spliceDefault__(list, other, index, ...) \
    (GblStringList_splice)(list, other, index)

#define GblStringList_eraseDefault_(...) \
    GblStringList_eraseDefault__(__VA_ARGS__, 1)
#define GblStringList_eraseDefault__(list, index, count, ...) \
    (GblStringList_erase)(list, index, count)

#define GblStringList_removeDefault_(...) \
    GblStringList_removeDefault__(__VA_ARGS__, GBL_TRUE)
#define GblStringList_removeDefault__(list, str, match, ...) \
    (GblStringList_remove)(list, str, match)

#define GblStringList_deduplicateDefault_(...) \
    GblStringList_deduplicateDefault__(__VA_ARGS__, GBL_TRUE)
#define GblStringList_deduplicateDefault__(list, match, ...) \
    (GblStringList_deduplicate)(list, match)

#define GblStringList_sortDefault_(...) \
    GblStringList_sortDefault__(__VA_ARGS__, GBL_TRUE)
#define GblStringList_sortDefault__(list, asc, ...) \
    (GblStringList_sort)(list, asc)

#define GblStringList_foreachDefault_(...) \
    GblStringList_foreachDefault__(__VA_ARGS__, GBL_NULL)
#define GblStringList_foreachDefault__(list, iter, cl, ...) \
    (GblStringList_foreach)(list, iter, cl)
///\endcond

GBL_INLINE GblStringList* GblStringList_createEmpty(void) GBL_NOEXCEPT {
    return GblRingList_createEmpty();
}

GBL_INLINE GblSize GblStringList_size(GBL_CSELF) GBL_NOEXCEPT {
    return GblRingList_size(pSelf);
}

GBL_INLINE GblBool GblStringList_empty(GBL_CSELF) GBL_NOEXCEPT {
    return GblRingList_empty(pSelf);
}

GBL_INLINE GblStringRef* GblStringList_front(GBL_CSELF) GBL_NOEXCEPT {
    return (GblStringRef*)GblRingList_front(pSelf);
}

GBL_INLINE GblStringRef* GblStringList_back(GBL_CSELF) GBL_NOEXCEPT {
    return (GblStringRef*)GblRingList_back(pSelf);
}

GBL_INLINE GblStringRef* GblStringList_at(GBL_CSELF, intptr_t index) GBL_NOEXCEPT {
    return (GblStringRef*)GblRingList_at(pSelf, index);
}

GBL_INLINE GblBool (GblStringList_contains)(GBL_CSELF, const char* pStr, GblBool matchCase) GBL_NOEXCEPT {
    return GblStringList_find(pSelf, pStr, matchCase) != GBL_STRING_LIST_NPOS? GBL_TRUE : GBL_FALSE;
}

GBL_INLINE GblBool (GblStringList_splice)(GBL_SELF, GblStringList* pOther, intptr_t index) GBL_NOEXCEPT {
    return GblRingList_splice(pSelf, pOther, index);
}

GBL_INLINE GblStringRef* GblStringList_popBack(GBL_SELF) GBL_NOEXCEPT {
    return (GblStringRef*)GblRingList_popBack(pSelf);
}

GBL_INLINE GblStringRef* GblStringList_popFront(GBL_SELF) GBL_NOEXCEPT {
    return (GblStringRef*)GblRingList_popFront(pSelf);
}

GBL_INLINE void GblStringList_rotate(GBL_SELF, intptr_t n) GBL_NOEXCEPT {
    GblRingList_rotate(pSelf, n);
}

GBL_INLINE void GblStringList_reverse(GBL_SELF) GBL_NOEXCEPT {
    GblRingList_reverse(pSelf);
}

GBL_INLINE GblBool (GblStringList_foreach)(GBL_SELF, GblStringListIterFn pFnIt, void* pCl) GBL_NOEXCEPT {
    return GblRingList_foreach(pSelf, (GblRingListIterFn)pFnIt, pCl);
}


GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_STRING_LIST_H

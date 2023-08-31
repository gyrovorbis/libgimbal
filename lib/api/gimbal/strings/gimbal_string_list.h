/*! \file
 *  \brief GblStringList structure and related functions
 *  \ingroup strings
 *
 *  This file contains the definition of GblStringList
 *  and its related API. A GblStringList is a container
 *  for conveniently working with multiple separate
 *  strings.
 *
 *  \sa GblStringList
 *
 *  \todo
 *      - Add pattern matching API
 *      - replaceInStrings
 *      - GblStringList_createSplitPattern()
 *      - test any "view" method
 *      - make splice take a range
 *      - compare/equals strings/arrays/views
 *      - pushBack/pushFront arrays
 *      - insert arrays
 *
 *  \author     2023 Falco Girgis
 *  \copyright  MIT License
 */
#ifndef GIMBAL_STRING_LIST_H
#define GIMBAL_STRING_LIST_H

#include "../containers/gimbal_ring_list.h"
#include "gimbal_string_ref.h"

//! Represents the last position or an invalid index in a GblStringList
#define GBL_STRING_LIST_NPOS GBL_RING_LIST_NPOS

#define GBL_SELF_TYPE GblStringList

GBL_DECLS_BEGIN

/*! Iterator function used with GblStringList_foreach()

    Callback used when iterating over a GblStringList, getting passed
    a reference to each GblStringRef within the list, along with an
    optional closure/capture value that can be passed in.

    Returning GBL_TRUE will cause iteration to cease early.
*/
typedef GblBool (*GblStringListIterFn)(GblStringRef* pRef, void* pClosure);

/*! List of strings with array-like API
 *  \ingroup strings
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
 *  not require dynamically resizing or moving list entries. It also
 *  becomes a reference type with reference semantics.
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
 *  \sa GblStringRef, GblRingBuffer
 */
typedef GblRingList GblStringList;

/*! \name Lifetime Management
 *  \brief Methods for creating, referencing, and unreferencing lists
 *  @{
 */
//! Creates and returns a new, empty GblStringList reference with a size of 0
GBL_EXPORT GblStringList* GblStringList_createEmpty       (void)                                GBL_NOEXCEPT;
//! Creates and returns a new GblStringList reference, populating it with a list of C strings (auto NULL-terminated)
GBL_EXPORT GblStringList* GblStringList_create            (const char* pFirst, .../*, NULL*/)   GBL_NOEXCEPT;
//! Equivalent to GblStringList_create(), except for strings N+1 are all passed via a va_list pointer
GBL_EXPORT GblStringList* GblStringList_createVa          (const char* pFirst, va_list* pVa)    GBL_NOEXCEPT;
//! Equivalent to GblStringList_create(), except for the strings are provided as GblStringRefs (which will be referenced again)
GBL_EXPORT GblStringList* GblStringList_createWithRefs    (GblStringRef* pFirst, .../*, NULL*/) GBL_NOEXCEPT;
//! Equivalent to GblStringList_createWithRefs(), except for strings N+1 are all passed via a va_list pointer
GBL_EXPORT GblStringList* GblStringList_createWithRefsVa  (GblStringRef* pFirst, va_list* pVa)  GBL_NOEXCEPT;
//! Creates and returns a new GblStringList reference, populating it with the (auto) NULL-terminated list of (string, length) pairs
GBL_EXPORT GblStringList* GblStringList_createWithViews   (const char* pFirst,
                                                           size_t      firstLen,
                                                           ...
                                                           /*, NULL*/)                          GBL_NOEXCEPT;
//! Equivalent to GblStringList_createWithViews(), except for views N+1 are all passed via a va_list pointer
GBL_EXPORT GblStringList* GblStringList_createWithViewsVa (const char* pFirst,
                                                           size_t      firstLen,
                                                           va_list*    pVa)                     GBL_NOEXCEPT;
//! Creates and returns a new GblStringList, populated by an array of const char* pointers, with either a size or NULL terminator (default)
GBL_EXPORT GblStringList* GblStringList_createWithArray   (const char** ppStr,
                                                           size_t       size/*=NPOS*/)          GBL_NOEXCEPT;
//! Creates and returns a new GblStringList, populating it with \p pStr split up by every occurence of \p pDelim
GBL_EXPORT GblStringList* GblStringList_createSplit       (const char* pStr,
                                                           const char* pDelim,
                                                           size_t      strLen/*=0*/,
                                                           size_t      delimLen/*=0*/)          GBL_NOEXCEPT;
//! Deep copies an existing GblStringList, creating a duplicated list populated by references to the values held by \p pSrc
GBL_EXPORT GblStringList* GblStringList_createCopy        (const GblStringList* pSrc,
                                                           intptr_t             startIdx/*=0*/,
                                                           size_t               count/*=NPOS*/) GBL_NOEXCEPT;
//! Creates and returns a new GblStringList, populated only by those strings from \p pSrc which match the given pattern, \p pPat
GBL_EXPORT GblStringList* GblStringList_createFilter      (const GblStringList* pSrc,
                                                           const char*          pPat)           GBL_NOEXCEPT;
//! Returns a reference to an existing GblStringList, incrementing its reference count and returning it back
GBL_EXPORT GblStringList* GblStringList_ref               (GBL_CSELF)                           GBL_NOEXCEPT;
//! Releases a reference to the given GblStringList, decrementing and returning its remaining refCount, destroying it upon hitting 0
GBL_EXPORT GblRefCount    GblStringList_unref             (GBL_SELF)                            GBL_NOEXCEPT;
//! @}

/*! \name  Operators
 *  \brief Methods implementing basic operations
 *  @{
 */
//! Returns the lexicographical difference between the two lists, optionally doing so case insensitively
GBL_EXPORT int     GblStringList_compare       (GBL_CSELF,
                                                const GblStringList* pOther,
                                                GblBool              matchCase/*=GBL_TRUE*/) GBL_NOEXCEPT;
//! Returns GBL_TRUE if the given list is lexicographically equal to the \p pOther list, optionally ignoring case
GBL_EXPORT GblBool GblStringList_equals        (GBL_CSELF,
                                                const GblStringList* pOther,
                                                GblBool              matchCase/*=GBL_TRUE*/) GBL_NOEXCEPT;
//! Equivalent to GblStringList_compare(), except that an (auto) NULL-terminated list of C strings is the other value being compared
GBL_EXPORT int     GblStringList_compareStrs   (GBL_CSELF,
                                                int matchCase,
                                                ...
                                                /*,NULL*/)                                   GBL_NOEXCEPT;
//! Equivalent to GblStringList_equals(), except that an (auto) NULL-terminated list of C strings is the value being compared
GBL_EXPORT GblBool GblStringList_equalsStrs    (GBL_CSELF,
                                                int matchCase,
                                                ...
                                                /*,NULL*/)                                   GBL_NOEXCEPT;
//! Equivalent to GblStringList_compareStrs(), except the NULL-terminated list of C strings is passed through a va_list pointer
GBL_EXPORT int     GblStringList_compareStrsVa (GBL_CSELF,
                                                int      matchCase,
                                                va_list* pVa);
//! Equivalent to GblStringList_equalsStrs(), except the NULL-terminated list of C strings is passed through a va_list pointer
GBL_EXPORT GblBool GblStringList_equalsStrsVa  (GBL_CSELF,
                                                int      matchCase,
                                                va_list* pVa);

//! @}

/*! \name Properties
 *  \brief Methods for accessing properties and derived data
 *  @{
 */
//! Returns the number of active references to the given GblStringList
GBL_EXPORT GblRefCount   GblStringList_refCount (GBL_CSELF)                 GBL_NOEXCEPT;
//! Returns the number of string elements contained by the the given GblStringList
GBL_EXPORT size_t        GblStringList_size     (GBL_CSELF)                 GBL_NOEXCEPT;
//! Returns whether the given GblStringList contains any string elements or not
GBL_EXPORT GblBool       GblStringList_empty    (GBL_CSELF)                 GBL_NOEXCEPT;
//! Returns the first string (element 0) contained within the given GblStringList or NULL if it's empty
GBL_EXPORT GblStringRef* GblStringList_front    (GBL_CSELF)                 GBL_NOEXCEPT;
//! Returns the last string (element -1) contained within the given GblStringList or NULL if it's empty
GBL_EXPORT GblStringRef* GblStringList_back     (GBL_CSELF)                 GBL_NOEXCEPT;
//! Returns the string contained at the given signed\p index within the GblStringList or NULL if invalid
GBL_EXPORT GblStringRef* GblStringList_at       (GBL_CSELF, intptr_t index) GBL_NOEXCEPT;
//! @}

/*! \name Searching
 *  \brief Methods for finding, counting, and replacing strings
 *  @{
 */
//! Searches the list for \p pStr, starting at \p offset, returning its index if found, or GBL_RING_LIST_NPOS otherwise
GBL_EXPORT size_t  GblStringList_find     (GBL_CSELF,
                                           const char* pStr,
                                           GblBool     matchCase/*=GBL_TRUE*/,
                                           size_t      offset/*=0*/,
                                           size_t      strLen/*=0*/) GBL_NOEXCEPT;
//! This function is equivalent to GblStringList_find(), except that it searches from the last to first element
GBL_EXPORT size_t  GblStringList_rfind    (GBL_CSELF,
                                           const char* pStr,
                                           GblBool     matchCase/*=GBL_TRUE*/,
                                           size_t      offset/*=GBL_STRING_LIST_NPOS*/,
                                           size_t      strLen/*=0*/) GBL_NOEXCEPT;
//! Searches the given list for the first instance of \p pStr, returning its index or GBL_RING_LIST_NPOS if it wasn't found
GBL_EXPORT GblBool GblStringList_contains (GBL_CSELF,
                                           const char* pStr,
                                           GblBool     matchCase/*=GBL_TRUE*/,
                                           size_t      strLen/*=0*/) GBL_NOEXCEPT;
//! Searches the given list for the number of occurences of \p pStr, optionally doing case insensitive comparisons
GBL_EXPORT size_t  GblStringList_count    (GBL_CSELF,
                                           const char* pStr,
                                           GblBool     matchCase/*=GBL_TRUE*/,
                                           size_t      strLen/*=0*/) GBL_NOEXCEPT;
//! @}

/*! \name  Assigning
 *  \brief Methods for setting elements
 *  @{
 */
//! Sets the string contained at the given signed \p index to \p pStr, returning a result code
GBL_EXPORT GBL_RESULT GblStringList_set    (GBL_SELF,
                                            intptr_t    index,
                                            const char* pStr,
                                            size_t      strLen/*=0*/) GBL_NOEXCEPT;
//! Sets the string located at the given signed \p index to a new reference to \p pRef, returning a result code
GBL_EXPORT GBL_RESULT GblStringList_setRef (GBL_SELF,
                                            intptr_t      index,
                                            GblStringRef* pRef)       GBL_NOEXCEPT;
//! @}

/*! \name Adding
 *  \brief Methods for appending and inserting strings
 *  @{
 */
//! Appends the given (auto) NULL-terminated list of C strings to the back of the list, returning a status code
GBL_EXPORT GBL_RESULT GblStringList_pushBack         (GBL_SELF, .../*, NULL*/)   GBL_NOEXCEPT;
//! Equivalent to GblStringList_pushBack(), except the C strings are passed through a va_list pointer
GBL_EXPORT GBL_RESULT GblStringList_pushBackVa       (GBL_SELF, va_list* pVa)    GBL_NOEXCEPT;
//! Appends <i>new references</i> to a list of (auto) NULL-terminated GblStringRefs to the end of the given list, returning a status code
GBL_EXPORT GBL_RESULT GblStringList_pushBackRefs     (GBL_SELF, .../*, NULL*/)   GBL_NOEXCEPT;
//! Equivalent to GblStringList_pushBackRefs(), except the GblStringRefs are passed through a va_list pointer
GBL_EXPORT GBL_RESULT GblStringList_pushBackRefsVa   (GBL_SELF, va_list* pVa)    GBL_NOEXCEPT;
//! Appends the given (auto) NULL-terminated list of (string, length) pairs to the back of the list, returning a status code
GBL_EXPORT GBL_RESULT GblStringList_pushBackViews    (GBL_SELF,
                                                      /* const char* pStr, */
                                                      /* size_t      strLen,*/
                                                      ...
                                                      /*, NULL*/)                GBL_NOEXCEPT;
//! Equivalent to GblStringList_pushBackViews(), except the (string, length) pairs are passed through a va_list pointer
GBL_EXPORT GBL_RESULT GblStringList_pushBackViewsVa  (GBL_SELF, va_list* pVa)    GBL_NOEXCEPT;
//! Appends the given optionally NULL-terminated array of C strings to the back of the given list, returning a status code
GBL_EXPORT GBL_RESULT GblStringList_pushBackArray    (GBL_SELF,
                                                      const char** ppStrArray,
                                                      size_t       len/*=NPOS*/) GBL_NOEXCEPT;

//! Prepends the given (auto) NULL-terminated list of C strings to the front of the list, returning a status code
GBL_EXPORT GBL_RESULT GblStringList_pushFront        (GBL_SELF, .../*, NULL*/)   GBL_NOEXCEPT;
//! Equivalent to GblStringList_pushFront(), except the C strings are passed through a va_list pointer
GBL_EXPORT GBL_RESULT GblStringList_pushFrontVa      (GBL_SELF, va_list* pVa)    GBL_NOEXCEPT;
//! Prepends <i>new references</i> to a list of (auto) NULL-terminated GblStringRefs to the front of the given list, returning a status code
GBL_EXPORT GBL_RESULT GblStringList_pushFrontRefs    (GBL_SELF, .../*, NULL*/)   GBL_NOEXCEPT;
//! Equivalent to GblStringList_pushFrontRefs(), except the GblStringRefs are passed through a va_list pointer
GBL_EXPORT GBL_RESULT GblStringList_pushFrontRefsVa  (GBL_SELF, va_list* pVa)    GBL_NOEXCEPT;
//! Prepends the given (auto) NULL-terminated list of (string, length) pairs to the front of the list, returning a status code
GBL_EXPORT GBL_RESULT GblStringList_pushFrontViews   (GBL_SELF,
                                                      /* const char* pStr, */
                                                      /* size_t      strLen,*/
                                                      ...
                                                      /*, NULL*/)                GBL_NOEXCEPT;
//! Equivalent to GblStringList_pushFrontViews(), except the (string, length) pairs are passed through a va_list pointer
GBL_EXPORT GBL_RESULT GblStringList_pushFrontViewsVa (GBL_SELF, va_list* pVa)    GBL_NOEXCEPT;
//! Prepends the given optionally NULL-terminated array of C strings to the front of the given list, returning a status code
GBL_EXPORT GBL_RESULT GblStringList_pushFrontArray   (GBL_SELF,
                                                      const char** ppStrArray,
                                                      size_t       len/*=NPOS*/) GBL_NOEXCEPT;

//! Inserts a(n auto) NULL-terminated list of C strings to the signed \p index of the list, returning a status code
GBL_EXPORT GBL_RESULT GblStringList_insert           (GBL_SELF,
                                                      intptr_t index,
                                                      ...
                                                      /*, NULL*/)                GBL_NOEXCEPT;
//! Equivalent to GblStringList_insert(), except the list of C strings is passed through a va_list pointer
GBL_EXPORT GBL_RESULT GblStringList_insertVa         (GBL_SELF,
                                                      intptr_t index,
                                                      va_list* pVa)              GBL_NOEXCEPT;
//! Inserts <i>new references</i> to a(n auto) NULL-terminated list of GblStringRefs to \p index, returning a status code
GBL_EXPORT GBL_RESULT GblStringList_insertRefs       (GBL_SELF,
                                                      intptr_t index,
                                                      ...
                                                      /*, NULL*/)                GBL_NOEXCEPT;
//! Equivalent to GblStringList_insertRefs(), except the list of GblStringRefs is passed through a va_list pointer
GBL_EXPORT GBL_RESULT GblStringList_insertRefsVa     (GBL_SELF,
                                                      intptr_t index,
                                                      va_list* pVa)              GBL_NOEXCEPT;
//! Inserts the given (auto) NULL-terminated list of (string, length) pairs into \p index in the list, returning a status code
GBL_EXPORT GBL_RESULT GblStringList_insertViews      (GBL_SELF,
                                                      intptr_t      index,
                                                      /*const char* pStr, */
                                                      /* size_t     strLen, */
                                                      ...
                                                      /*, NULL*/)                GBL_NOEXCEPT;
//! Equivalent to GblStringList_insertViews(), except the list of pairs is provided through a va_list pointer
GBL_EXPORT GBL_RESULT GblStringList_insertViewsVa    (GBL_SELF,
                                                      intptr_t index,
                                                      va_list* pVa)              GBL_NOEXCEPT;
//! Inserts the given optionally NULL-terminated list of C strings at into \p index in the list, returning a status code
GBL_EXPORT GBL_RESULT GblStringList_insertArray      (GBL_SELF,
                                                      intptr_t index,
                                                      const char** ppStrArray,
                                                      size_t       len/*=NPOS*/) GBL_NOEXCEPT;
//! @}

/*! \name Removing
 *  \brief Methods for popping and erasing strings
 *  @{
 */
//! Pops a reference to the last string in the list off of the end, returning it or NULL if empty
GBL_EXPORT GblStringRef* GblStringList_popBack     (GBL_SELF)                       GBL_NOEXCEPT;
//! Pops a reference to the first string in the list off the front, returning it or NULL if empty
GBL_EXPORT GblStringRef* GblStringList_popFront    (GBL_SELF)                       GBL_NOEXCEPT;
//! Erases \p count (default 1) entries from the list, starting at \p index, returning a status code
GBL_EXPORT GBL_RESULT    GblStringList_erase       (GBL_SELF,
                                                    intptr_t index,
                                                    size_t   count/*=1*/)           GBL_NOEXCEPT;
//! Removes all instances of \p pStr, optionally ignoring case, from the given list, returning the number removed
GBL_EXPORT size_t        GblStringList_remove      (GBL_SELF,
                                                    const char* pStr,
                                                    GblBool     matchCase/*=GBL_TRUE*/,
                                                    size_t      strLen/*=0*/)       GBL_NOEXCEPT;
//! Extracts the given node from the given list, returning its internally contained GblStringRef
GBL_EXPORT GblStringRef* GblStringList_extract     (GBL_SELF,
                                                    GblStringList* pNode)           GBL_NOEXCEPT;
//! Removes all duplicate copies of any strings contained within the given list, returning a status code
GBL_EXPORT GBL_RESULT    GblStringList_deduplicate (GBL_SELF,
                                                    GblBool matchCase/*=GBL_TRUE*/) GBL_NOEXCEPT;
//! Clears the given list, removing all string elements it contains, and returning a status code
GBL_EXPORT GBL_RESULT    GblStringList_clear       (GBL_SELF)                       GBL_NOEXCEPT;
//! @}

/*! \name  Miscellaneous
 *  \brief Methods for replacing, sorting, and other utilities
 *  @{
 */
//! Replaces \p limit instances of \p pOld with \p pNew found within the list, returning the number found
GBL_EXPORT size_t        GblStringList_replace         (GBL_SELF,
                                                        const char* pOld,
                                                        const char* pNew,
                                                        GblBool     matchCase/*=TRUE*/,
                                                        size_t      limit/*=0*/,
                                                        size_t      oldLen/*=0*/,
                                                        size_t      newLen/*=0*/)    GBL_NOEXCEPT;
//! Equivalent to GblStringList_replace(), except the replacement is a <i>new reference</i> to the given GblStringRef
GBL_EXPORT size_t        GblStringList_replaceWithRef  (GBL_SELF,
                                                        const char*   pOld,
                                                        GblStringRef* pNew,
                                                        GblBool       matchCase/*=TRUE*/,
                                                        size_t        limit/*=0*/,
                                                        size_t        oldLen/*=0*/)  GBL_NOEXCEPT;
//! Splices the contents of \p pOther into \p pSelf at \p index, returning a result code
GBL_EXPORT GBL_RESULT    GblStringList_splice          (GBL_SELF,
                                                        GblStringList* pOther,
                                                        intptr_t       index/*=-1*/) GBL_NOEXCEPT;
//! Joins together all strings within the list into a single string, separated by \p pSeparator, returning a new reference
GBL_EXPORT GblStringRef* GblStringList_join            (GBL_CSELF,
                                                        const char* pSeparator,
                                                        size_t      sepLen/*=0*/)    GBL_NOEXCEPT;
//! Sorts the given list alphabetically in ascending order (by default) or descending order
GBL_EXPORT void          GblStringList_sort            (GBL_SELF,
                                                        GblBool descending/*=TRUE*/) GBL_NOEXCEPT;
//! Rotates entries of the given list either forward or backwards, wrapping them back around as necessary
GBL_EXPORT void          GblStringList_rotate          (GBL_SELF, intptr_t n)        GBL_NOEXCEPT;
//! Reverses the order of all entries within the given list, so that the old front is back and vice versa
GBL_EXPORT void          GblStringList_reverse         (GBL_SELF)                    GBL_NOEXCEPT;
//! Iterates over every string within the list, passing it and \p pCl (optional) to \p pFnInit, see \sa GblStringListIterFn
GBL_EXPORT GblBool       GblStringList_foreach         (GBL_SELF,
                                                        GblStringListIterFn pFnIt,
                                                        void* pCl/*=NULL*/)          GBL_NOEXCEPT;
//! @}

GBL_DECLS_END

///\cond
#define GblStringList_create(...) \
    (GblStringList_create)(__VA_ARGS__, GBL_NULL)

#define GblStringList_createWithRefs(...) \
    (GblStringList_createWithRefs)(__VA_ARGS__, GBL_NULL)

#define GblStringList_createWithViews(first, ...) \
    ((GblStringList_createWithViews)(first, __VA_ARGS__, NULL))

#define GblStringList_createSplit(...) \
    GblStringList_createSplitDefault_(__VA_ARGS__)
#define GblStringList_createSplitDefault_(...) \
    GblStringList_createSplitDefault__(__VA_ARGS__, 0, 0)
#define GblStringList_createSplitDefault__(str, del, strLen, delLen, ...) \
    ((GblStringList_createSplit)(str, del, strLen, delLen))

#define GblStringList_createWithArray(...) \
    GblStringList_createWithArrayDefault_(__VA_ARGS__)
#define GblStringList_createWithArrayDefault_(...) \
    GblStringList_createWithArrayDefault__(__VA_ARGS__, GBL_STRING_LIST_NPOS)
#define GblStringList_createWithArrayDefault__(array, len, ...) \
    (GblStringList_createWithArray)(array, len)

#define GblStringList_createCopy(...) \
    GBL_VA_OVERLOAD_CALL_ARGC(GblStringList_createCopyDefault, __VA_ARGS__)
#define GblStringList_createCopyDefault_1(src) \
    GblStringList_createCopyDefault_2(src, 0)
#define GblStringList_createCopyDefault_2(src, index) \
    GblStringList_createCopyDefault_3(src, index, GBL_STRING_LIST_NPOS)
#define GblStringList_createCopyDefault_3(src, index, count) \
    ((GblStringList_createCopy)(src, index, count))

#define GblStringList_pushBack(...) \
    (GblStringList_pushBack)(__VA_ARGS__, GBL_NULL)

#define GblStringList_pushBackRefs(...) \
    (GblStringList_pushBackRefs)(__VA_ARGS__, GBL_NULL)

#define GblStringList_pushBackViews(...) \
    (GblStringList_pushBackViews)(__VA_ARGS__, GBL_NULL)

#define GblStringList_pushBackArray(...) \
    GblStringList_pushBackArrayDefault_(__VA_ARGS__)
#define GblStringList_pushBackArrayDefault_(...) \
    GblStringList_pushBackArrayDefault__(__VA_ARGS__, GBL_STRING_LIST_NPOS)
#define GblStringList_pushBackArrayDefault__(list, array, size, ...) \
    ((GblStringList_pushBackArray)(list, array, size))

#define GblStringList_pushFront(...) \
    (GblStringList_pushFront)(__VA_ARGS__, GBL_NULL)

#define GblStringList_pushFrontRefs(...) \
    (GblStringList_pushFrontRefs)(__VA_ARGS__, GBL_NULL)

#define GblStringList_pushFrontViews(...) \
    (GblStringList_pushFrontViews)(__VA_ARGS__, GBL_NULL)

#define GblStringList_pushFrontArray(...) \
    GblStringList_pushFrontArrayDefault_(__VA_ARGS__)
#define GblStringList_pushFrontArrayDefault_(...) \
    GblStringList_pushFrontArrayDefault__(__VA_ARGS__, GBL_STRING_LIST_NPOS)
#define GblStringList_pushFrontArrayDefault__(list, array, size, ...) \
    ((GblStringList_pushFrontArray(list, array, size)))

#define GblStringList_insert(...) \
    (GblStringList_insert)(__VA_ARGS__, GBL_NULL)

#define GblStringList_insertRefs(...) \
    (GblStringList_insertRefs)(__VA_ARGS__, GBL_NULL)

#define GblStringList_insertViews(...) \
    (GblStringList_insertViews)(__VA_ARGS__, GBL_NULL)

#define GblStringList_insertArray(...) \
    GblStringList_insertArrayDefault_(__VA_ARGS__)
#define GblStringList_insertArrayDefault_(...) \
    GblStringList_insertArrayDefault__(__VA_ARGS__, GBL_STRING_LIST_NPOS)
#define GblStringList_insertArrayDefault__(list, idx, array, size, ...) \
    ((GblStringList_insertArray(list, idx, array, size)))

#define GblStringList_compare(...) \
    GblStringList_compareDefault_(__VA_ARGS__)
#define GblStringList_compareDefault_(...) \
    GblStringList_compareDefault__(__VA_ARGS__, GBL_TRUE)
#define GblStringList_compareDefault__(list1, list2, match, ...) \
    ((GblStringList_compare)(list1, list2, match))

#define GblStringList_equals(...) \
    GblStringList_equalsDefault_(__VA_ARGS__)
#define GblStringList_equalsDefault_(...) \
    GblStringList_equalsDefault__(__VA_ARGS__, GBL_TRUE)
#define GblStringList_equalsDefault__(list1, list2, match, ...) \
    ((GblStringList_equals)(list1, list2, match))

#define GblStringList_compareStrs(...) \
    ((GblStringList_compareStrs)(__VA_ARGS__, GBL_NULL))

#define GblStringList_equalsStrs(...) \
    ((GblStringList_equalsStrs)(__VA_ARGS__, GBL_NULL))

#define GblStringList_find(...) \
    GBL_VA_OVERLOAD_CALL_ARGC(GblStringList_findDefault, __VA_ARGS__)
#define GblStringList_findDefault_2(list, str) \
    GblStringList_findDefault_3(list, str, GBL_TRUE)
#define GblStringList_findDefault_3(list, str, match) \
    GblStringList_findDefault_4(list, str, match, 0)
#define GblStringList_findDefault_4(list, str, match, offset) \
    GblStringList_findDefault_5(list, str, match, offset, 0)
#define GblStringList_findDefault_5(list, str, match, offset, len) \
    ((GblStringList_find)(list, str, match, offset, len))

#define GblStringList_rfind(...) \
    GBL_VA_OVERLOAD_CALL_ARGC(GblStringList_rfindDefault, __VA_ARGS__)
#define GblStringList_rfindDefault_2(list, str) \
    GblStringList_rfindDefault_3(list, str, GBL_TRUE)
#define GblStringList_rfindDefault_3(list, str, match) \
    GblStringList_rfindDefault_4(list, str, match, GBL_STRING_LIST_NPOS)
#define GblStringList_rfindDefault_4(list, str, match, offset) \
    GblStringList_rfindDefault_5(list, str, match, offset, 0)
#define GblStringList_rfindDefault_5(list, str, match, offset, len) \
    (GblStringList_rfind)(list, str, match, offset, len)

#define GblStringList_contains(...) \
    GBL_VA_OVERLOAD_CALL_ARGC(GblStringList_containsDefault, __VA_ARGS__)
#define GblStringList_containsDefault_2(list, str) \
    GblStringList_containsDefault_3(list, str, GBL_TRUE)
#define GblStringList_containsDefault_3(list, str, match) \
    GblStringList_containsDefault_4(list, str, match, 0)
#define GblStringList_containsDefault_4(list, str, match, len) \
    ((GblStringList_contains)(list, str, match, len))

#define GblStringList_count(...) \
    GBL_VA_OVERLOAD_CALL_ARGC(GblStringList_countDefault, __VA_ARGS__)
#define GblStringList_countDefault_2(list, str) \
    GblStringList_countDefault_3(list, str, GBL_TRUE)
#define GblStringList_countDefault_3(list, str, match) \
    GblStringList_countDefault_4(list, str, match, 0)
#define GblStringList_countDefault_4(list, str, match, len) \
    ((GblStringList_count)(list, str, match, len))

#define GblStringList_set(...) \
    GblStringList_setDefault_(__VA_ARGS__)
#define GblStringList_setDefault_(...) \
    GblStringList_setDefault__(__VA_ARGS__, 0)
#define GblStringList_setDefault__(list, idx, str, len, ...) \
    ((GblStringList_set)(list, idx, str, len))

#define GblStringList_replace(...) \
    GBL_VA_OVERLOAD_CALL_ARGC(GblStringList_replaceDefault, __VA_ARGS__)
#define GblStringList_replaceDefault_3(list, old, repl) \
    GblStringList_replaceDefault_4(list, old, repl, GBL_TRUE)
#define GblStringList_replaceDefault_4(list, old, repl, match) \
    GblStringList_replaceDefault_5(list, old, repl, match, 0)
#define GblStringList_replaceDefault_5(list, old, repl, match, limit) \
    GblStringList_replaceDefault_6(list, old, repl, match, limit, 0)
#define GblStringList_replaceDefault_6(list, old, repl, match, limit, oldLen) \
    GblStringList_replaceDefault_7(list, old, repl, match, limit, oldLen, 0)
#define GblStringList_replaceDefault_7(list, old, repl, match, limit, oldLen, newLen) \
    ((GblStringList_replace)(list, old, repl, match, limit, oldLen, newLen))

#define GblStringList_replaceWithRef(...) \
    GblStringList_replaceWithRefDefault_(__VA_ARGS__)
#define GblStringList_replaceWithRefDefault_(...) \
    GBL_VA_OVERLOAD_CALL_ARGC(GblStringList_replaceWithRefDefault, __VA_ARGS__)
#define GblStringList_replaceWithRefDefault_3(list, old, repl) \
    GblStringList_replaceWithRefDefault_4(list, old, repl, GBL_TRUE)
#define GblStringList_replaceWithRefDefault_4(list, old, repl, match) \
    GblStringList_replaceWithRefDefault_5(list, old, repl, match, 0)
#define GblStringList_replaceWithRefDefault_5(list, old, repl, match, limit) \
    GblStringList_replaceWithRefDefault_6(list, old, repl, match, limit, 0)
#define GblStringList_replaceWithRefDefault_6(list, old, repl, match, limit, oldLen) \
    (GblStringList_replaceWithRef)(list, old, repl, match, limit, oldLen)

#define GblStringList_splice(...) \
    GblStringList_spliceDefault_(__VA_ARGS__)
#define GblStringList_spliceDefault_(...) \
    GblStringList_spliceDefault__(__VA_ARGS__, -1)
#define GblStringList_spliceDefault__(list, other, index, ...) \
    (GblStringList_splice)(list, other, index)

#define GblStringList_join(...) \
    GblStringList_joinDefault_(__VA_ARGS__)
#define GblStringList_joinDefault_(...) \
    GblStringList_joinDefault__(__VA_ARGS__, 0)
#define GblStringList_joinDefault__(list, sep, len, ...) \
    ((GblStringList_join)(list, sep, len))

#define GblStringList_erase(...) \
    GblStringList_eraseDefault_(__VA_ARGS__)
#define GblStringList_eraseDefault_(...) \
    GblStringList_eraseDefault__(__VA_ARGS__, 1)
#define GblStringList_eraseDefault__(list, index, count, ...) \
    (GblStringList_erase)(list, index, count)

#define GblStringList_remove(...) \
    GBL_VA_OVERLOAD_CALL_ARGC(GblStringList_removeDefault, __VA_ARGS__)
#define GblStringList_removeDefault_2(list, str) \
    GblStringList_removeDefault_3(list, str, GBL_TRUE)
#define GblStringList_removeDefault_3(list, str, match) \
    GblStringList_removeDefault_4(list, str, match, 0)
#define GblStringList_removeDefault_4(list, str, match, len) \
    ((GblStringList_remove)(list, str, match, len))

#define GblStringList_deduplicate(...) \
    GblStringList_deduplicateDefault_(__VA_ARGS__)
#define GblStringList_deduplicateDefault_(...) \
    GblStringList_deduplicateDefault__(__VA_ARGS__, GBL_TRUE)
#define GblStringList_deduplicateDefault__(list, match, ...) \
    (GblStringList_deduplicate)(list, match)

#define GblStringList_sort(...) \
    GblStringList_sortDefault_(__VA_ARGS__)
#define GblStringList_sortDefault_(...) \
    GblStringList_sortDefault__(__VA_ARGS__, GBL_TRUE)
#define GblStringList_sortDefault__(list, asc, ...) \
    (GblStringList_sort)(list, asc)

#define GblStringList_foreach(...) \
    GblStringList_foreachDefault_(__VA_ARGS__)
#define GblStringList_foreachDefault_(...) \
    GblStringList_foreachDefault__(__VA_ARGS__, GBL_NULL)
#define GblStringList_foreachDefault__(list, iter, cl, ...) \
    (GblStringList_foreach)(list, iter, cl)
///\endcond

#undef GBL_SELF_TYPE

#endif // GIMBAL_STRING_LIST_H

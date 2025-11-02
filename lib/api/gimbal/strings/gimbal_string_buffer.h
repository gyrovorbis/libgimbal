/*! \file
 *  \brief   GblStringBuffer mutable string builder API
 *  \ingroup strings
 *
 *  This file contains the structure and C API around the
 *  GblStringBuffer data type, a mutable container that
 *  specializes in building and constructing strings,
 *  equivalent to a C++/Rust string or a C#/Java string
 *  builder.
 *
 *  \todo
 *      - GblStringBuffer_swap(...)
 *      - GblStringBuffer_match()
 *      - GblStringBuffer_replaceMatch()
 *      - GblStringBuffer_removeMatch()
 *      - GblStringBuffer_appendUint64()
 *      - GblStringBuffer_appendInt64()
 *
 *  \author     2023 Falco Girgis
 *  \copyright  MIT License
 */

#ifndef GIMBAL_STRING_BUFFER_H
#define GIMBAL_STRING_BUFFER_H

#include "../containers/gimbal_array_list.h"
#include "gimbal_string_view.h"
#include "gimbal_string_ref.h"

#include <stdarg.h>

#define GBL_SELF_TYPE GblStringBuffer

GBL_DECLS_BEGIN

/*! \brief Mutable string type optimized for building and writing
 *
 *  GblStringBuffer is a type of string which is optimized for efficient
 *  piece-wise construction, appending, and writing. It's equivalent to
 *  a "String Builder" type in other languages, such as C# and  Java.
 *
 *  It is typically used temporarily to construct a string, afterwards
 *  it is usually converted to another type for storage, such as a
 *  GblStringRef or the internal buffer is taken away from it to be stored
 *  elsewhere.
 *
 *  Internally it is implemented similarly to a C++-vector, with both a
 *  size and a capacity, growing when needed, but not immediatley shrinking.
 *
 *  \note
 *  The API around GblStringBuffer is based around modifying strings. For
 *  read-only operations on them, such as searching or match counting, use
 *  GblStringBuffer_view() and the GblStringView API.
 *
 *  \note
 *  GblStringBuffer supports being created with additional trailing storage,
 *  allowing it to be over-allocated with malloc() or GBL_ALLOCA(). This means
 *  it will not create a separate heap allocation until necessary, and will
 *  instead useits trailing allocation region as its internal buffer. This can
 *  be very efficient when building temporary strings.
 *
 *  \ingroup strings
 *  \sa      GblStringRef, GblStringView, GblQuark
 */
typedef struct GblStringBuffer {
    GblArrayList data;  //!< Internal storage buffer
} GblStringBuffer;

/*! \name  Lifetime Management
 *  \brief Methods for consruction, destruction, moving, etc
 *  \relatesalso GblStringBuffer
 *  @{
 */
//! Constructs the given GblStringBuffer struct with any given initial values (or defaults), returning a result code
GBL_EXPORT GBL_RESULT GblStringBuffer_construct (GBL_SELF,
                                                 const char* pString/*=NULL*/,
                                                 size_t      length/*=0*/,
                                                 size_t      structSize/*=DEFAULT*/,
                                                 GblContext* pCtx/*=NULL*/)                     GBL_NOEXCEPT;
//! Destructs the given GblStringBuffer structure, releasing any allocated resources and returning a result code
GBL_EXPORT GBL_RESULT GblStringBuffer_destruct  (GBL_SELF)                                      GBL_NOEXCEPT;
//! Transfers ownership of a given C string to the given buffer, assigning its value and updating its capacity
GBL_EXPORT GBL_RESULT GblStringBuffer_acquire   (GBL_SELF, char* pData, size_t capacity)        GBL_NOEXCEPT;
//! Takes ownership of the given GblStringBuffer's internal buffer, also returning its capacity and a result code
GBL_EXPORT GBL_RESULT GblStringBuffer_release   (GBL_SELF, char**  ppStrPtr, size_t* pCapacity) GBL_NOEXCEPT;
//! @}

/*! \name  Properties
 *  \brief Methods for querying or retrieving associated data
 *  \relatesalso GblStringBuffer
 *  @{
 */
//! Returns a mutable pointer to the internal data buffer of the given GblStringBuffer
GBL_EXPORT char*       GblStringBuffer_data        (GBL_SELF)  GBL_NOEXCEPT;
//! Returns the string length of the string held by the given GblStringBuffer
GBL_EXPORT size_t      GblStringBuffer_length      (GBL_CSELF) GBL_NOEXCEPT;
//! Returns the actual size of the internal data buffer held by the given GblStringBuffer
GBL_EXPORT size_t      GblStringBuffer_capacity    (GBL_CSELF) GBL_NOEXCEPT;
//! Returns the number of bytes the given GblStringBuffer can store before falling back to the heap
GBL_EXPORT size_t      GblStringBuffer_stackBytes  (GBL_CSELF) GBL_NOEXCEPT;
//! Returns a mutable pointer to the extra stack storage region, if present
GBL_EXPORT char*       GblStringBuffer_stackBuffer (GBL_CSELF) GBL_NOEXCEPT;
//! Returns a pointer to the context the GblStringBuffer was created with
GBL_EXPORT GblContext* GblStringBuffer_context     (GBL_CSELF) GBL_NOEXCEPT;
//! Returns GBL_TRUE if the given GblStringBuffer is empty, otherwise returns GBL_FALSE
GBL_EXPORT GblBool     GblStringBuffer_empty       (GBL_CSELF) GBL_NOEXCEPT;
//! Returns GBL_TRUE if the given GblStringBuffer holds a valid C string (with NULL terminator)
GBL_EXPORT GblBool     GblStringBuffer_valid       (GBL_CSELF) GBL_NOEXCEPT;
//! Returns GBL_TRUE if the given GblStringBuffer is empty or holds only spacing characters
GBL_EXPORT GblBool     GblStringBuffer_blank       (GBL_CSELF) GBL_NOEXCEPT;
//! Returns GBL_TRUE if the given GblStringBuffer's internal buffer is on the stack and not heap
GBL_EXPORT GblBool     GblStringBuffer_stack       (GBL_CSELF) GBL_NOEXCEPT;

/*! \name  Conversions
 *  \brief Methods for converting to other string types
 *  \relatesalso GblStringBuffer
 *  @{
 */
//! Creates and returns a new GblStringRef based on the contents of the given buffer (don't forget to unref it)
GBL_EXPORT GblStringRef* GblStringBuffer_createRef (GBL_CSELF)        GBL_NOEXCEPT;
//! Returns a pointer to a constant NULL-terminated C string for the given GblStringBuffer
GBL_EXPORT const char*   GblStringBuffer_cString   (GBL_CSELF)        GBL_NOEXCEPT;
//! Returns a pointer to the interned string representation of the buffer, interning it if it hasn't been already
GBL_EXPORT const char*   GblStringBuffer_intern    (GBL_CSELF)        GBL_NOEXCEPT;
//! Converts the GblStringBuffer to a GblQuark, possibly interning it, and returning its GblQuark
GBL_EXPORT GblQuark      GblStringBuffer_quark     (GBL_CSELF)        GBL_NOEXCEPT;
//! Returns the GblQuark corresponding to the given GblStringBuffer only if it already exists
GBL_EXPORT GblQuark      GblStringBuffer_tryQuark  (GBL_CSELF)        GBL_NOEXCEPT;
//! Creates a GblStringView containing either the whole buffer or a subsection of it
GBL_EXPORT GblStringView GblStringBuffer_view      (GBL_CSELF,
                                                    size_t offset/*=0*/,
                                                    size_t len/*=0*/) GBL_NOEXCEPT;
//! @}

/*! \name Per-Character Access
 *  \brief Methods for getting and setting individual indices
 *  \relatesalso GblStringBuffer
 *  @{
 */
//! Returns the character stored in the given buffer at \p index, throwing an error and returning '\0' if invalid
GBL_EXPORT char       GblStringBuffer_at      (GBL_CSELF, size_t index)             GBL_NOEXCEPT;
//! Sets the character stored in the given buffer at \p index to \p value, returning an error upon failure
GBL_EXPORT GBL_RESULT GblStringBuffer_setChar (GBL_CSELF, size_t index, char value) GBL_NOEXCEPT;
//! @}

/*! \name Assignment
 *  \brief Methods for setting the value of a constructed string
 *  \relatesalso GblStringBuffer
 *  @{
 */
//! Assigns the value of the given GblStringBuffer to \p pStr, optionally taking its length, \p len
GBL_EXPORT const char* GblStringBuffer_set     (GBL_SELF, const char* pStr, size_t len/*=0*/) GBL_NOEXCEPT;
//! Assings the value of the formatted string to the given GblStringBuffer, similarly to C's sscanf()
GBL_EXPORT const char* GblStringBuffer_printf  (GBL_SELF, const char* pFmt, ...)              GBL_NOEXCEPT;
//! Equivalent to GblStringBuffer_printf(), except passing additional arguments via a va_list
GBL_EXPORT const char* GblStringBuffer_vPrintf (GBL_SELF, const char* pFmt, va_list varArgs)  GBL_NOEXCEPT;
//! @}

/*! \name  Prepending
 *  \brief Methods for adding to the beginning of the a string
 *  \relatesalso GblStringBuffer
 *  @{
 */
//! Inserts \p pStr (with optional length, \p len) at the beginning of the given buffer, returning a result code
GBL_EXPORT GBL_RESULT GblStringBuffer_prepend        (GBL_SELF, const char* pStr, size_t len/*=0*/) GBL_NOEXCEPT;
//! Inserts \p count copies of \p value at the beginning of the given buffer, returning a result code
GBL_EXPORT GBL_RESULT GblStringBuffer_prependPadding (GBL_SELF, char value, size_t count)           GBL_NOEXCEPT;
//! @}

/*! \name  Appending
 *  \brief Methods for adding to the end of a string
 *  \relatesalso GblStringBuffer
 *  @{
 */
//! Appends \p pStr (with optional length, \p len) to the given GblStringBuffer, returning a result code
GBL_EXPORT GBL_RESULT GblStringBuffer_append        (GBL_SELF, const char* pStr, size_t len/*=0*/) GBL_NOEXCEPT;
//! Appends a printf()-style formatted string to the given GblStringBuffer, returning a result code
GBL_EXPORT GBL_RESULT GblStringBuffer_appendPrintf  (GBL_SELF, const char* pFmt, ...)              GBL_NOEXCEPT;
//! Equivalent to GblStringBuffer_appendPrintf(), except additional arguments are provided as a va_list
GBL_EXPORT GBL_RESULT GblStringBuffer_appendVPrintf (GBL_SELF, const char* pFmt, va_list varArgs)  GBL_NOEXCEPT;
//! Adds \p count copies of \p value to the end of the given GblStringBuffer, returning a result code
GBL_EXPORT GBL_RESULT GblStringBuffer_appendPadding (GBL_SELF, char value, size_t count)           GBL_NOEXCEPT;
//! Adds the word "nil" to the end of the given GblStringBuffer, returning a result code
GBL_EXPORT GBL_RESULT GblStringBuffer_appendNil     (GBL_SELF)                                     GBL_NOEXCEPT;
//! Adds either "true" or "false" to the end of the given GblStringBuffer, depending on \p value, and returning a result code
GBL_EXPORT GBL_RESULT GblStringBuffer_appendBool    (GBL_SELF, GblBool value)                      GBL_NOEXCEPT;
//! Adds the stringified representation of the integer, \p value, to the end of the given GblStringBuffer, returning a result code
GBL_EXPORT GBL_RESULT GblStringBuffer_appendInt     (GBL_SELF, int value)                          GBL_NOEXCEPT;
//! Adds the stringified respresentation of the unsigned integer, \p value, to the end of the given GblStringBuffer, returning a result code
GBL_EXPORT GBL_RESULT GblStringBuffer_appendUint    (GBL_SELF, unsigned value)                     GBL_NOEXCEPT;
//! Adds the stringified representation of the float, \p value, to the end of the given GblStringBuffer, returning a result code
GBL_EXPORT GBL_RESULT GblStringBuffer_appendFloat   (GBL_SELF, float value)                        GBL_NOEXCEPT;
//! Adds the stringified representation of the double, \p value, to the end of the given GblStringBuffer, returning a result code
GBL_EXPORT GBL_RESULT GblStringBuffer_appendDouble  (GBL_SELF, double value)                       GBL_NOEXCEPT;
//! Adds the hexadecimal string representation of the pointer, \p pPtr, to the end of the given GblStringBuffer, returning a result code
GBL_EXPORT GBL_RESULT GblStringBuffer_appendPointer (GBL_SELF, const void* pPtr)                   GBL_NOEXCEPT;
//! @}

/*! \name  Modifying
 *  \brief Methods for general string modifications
 *  \relatesalso GblStringBuffer
 *  @{
 */
//! Inserts \p pString at the given \p index with optional \p length, returning a result code
GBL_EXPORT GBL_RESULT GblStringBuffer_insert    (GBL_SELF,
                                                 size_t      index,
                                                 const char* pString,
                                                 size_t      length/*=0*/)  GBL_NOEXCEPT;
//! Ovewrites a prtion of the string starting at \p index, with \p pString (\p length is optional), resizing as needed, and returning a result code
GBL_EXPORT GBL_RESULT GblStringBuffer_overwrite (GBL_SELF,
                                                 size_t      index,
                                                 const char* pString,
                                                 size_t      length/*=0*/)  GBL_NOEXCEPT;
//! Replaces \p limit instances (0 is infinite) of \p Substr with \p pReplacement, with both lenghts being optional, and returning a result code
GBL_EXPORT size_t     GblStringBuffer_replace   (GBL_SELF,
                                                 const char* pSubstr,
                                                 const char* pReplacement,
                                                 size_t      limit/*=0*/,
                                                 size_t      substrLen/*=0*/,
                                                 size_t      replLen/*=0*/) GBL_NOEXCEPT;
//! @}

/*! \name   Removing
 *  \brief Methods for erasing portions of a string
 *  \relatesalso GblStringBuffer
 *  @{
 */
//! Erases the portion of the string from \p offset to \p offset + \p len, resizing it, and returning a result code
GBL_EXPORT GBL_RESULT GblStringBuffer_erase     (GBL_SELF, size_t offset, size_t len)          GBL_NOEXCEPT;
//! Clears the contents of the given GblStringBuffer, resetting it  back to an empty state and returning a result code
GBL_EXPORT GBL_RESULT GblStringBuffer_clear     (GBL_SELF)                                     GBL_NOEXCEPT;
//! Removes all instances of \p pStr (\p len is optional) from the given buffer, returning the number of entries removed
GBL_EXPORT size_t     GblStringBuffer_remove    (GBL_SELF, const char* pStr, size_t len/*=0*/) GBL_NOEXCEPT;
//! Removes the last character from the given GblStringBuffer, returning a result code
GBL_EXPORT GBL_RESULT GblStringBuffer_chop      (GBL_SELF)                                     GBL_NOEXCEPT;
//! Removes any trailing newline characters (`\r` and `\n`) from the given GblStringBuffer, returning a result code
GBL_EXPORT GBL_RESULT GblStringBuffer_chomp     (GBL_SELF)                                     GBL_NOEXCEPT;
//! Removes any leading instances of \p value from the beginning of the given GblStringBuffer, returning a result code
GBL_EXPORT GBL_RESULT GblStringBuffer_trimStart (GBL_SELF, char value)                         GBL_NOEXCEPT;
//! Returns any trailing instances of \p value from the end of the given GblStringBuffer, returning a result code
GBL_EXPORT GBL_RESULT GblStringBuffer_trimEnd   (GBL_SELF, char value)                         GBL_NOEXCEPT;
//! @}

/*! \name Miscellaneous
 *  \brief Methods for other utilities and string operations
 *  \relatesalso GblStringBuffer
 *  @{
 */
//! Changes the casing of every character within the GblStringBuffer to lowercase, returning a result code
GBL_EXPORT GBL_RESULT GblStringBuffer_lower   (GBL_SELF) GBL_NOEXCEPT;
//! Changes the casing of every character within the GblStringBuffer to uppercase, returning a result code
GBL_EXPORT GBL_RESULT GblStringBuffer_upper   (GBL_SELF) GBL_NOEXCEPT;
//! Reverses the order of the characters with the given GblStringBuffer, returninga  result code
GBL_EXPORT GBL_RESULT GblStringBuffer_reverse (GBL_SELF) GBL_NOEXCEPT;
//! @}

/*! \name  Sizing
 *  \brief Methods for managing size and capacity
 *  \relatesalso GblStringBuffer
 *  @{
 */
//! Ensures that there is room for \p capacity characters within the given GblStringBuffer, returning a result code
GBL_EXPORT GBL_RESULT GblStringBuffer_reserve     (GBL_SELF, size_t capacity) GBL_NOEXCEPT;
//! Sets the length of the given GblStringBuffer to \p size, regrowing if necessary, but not shrinking, and returning a result code
GBL_EXPORT GBL_RESULT GblStringBuffer_resize      (GBL_SELF, size_t size)     GBL_NOEXCEPT;
//! Resizes the length of the given GblStringBuffer to be its current length + \p delta, returning a result code
GBL_EXPORT GBL_RESULT GblStringBuffer_grow        (GBL_SELF, size_t delta)    GBL_NOEXCEPT;
//! Resizes the length of the given GblStringBuffer to be its current length - \p delta, returning a result code
GBL_EXPORT GBL_RESULT GblStringBuffer_shrink      (GBL_SELF, size_t delta)    GBL_NOEXCEPT;
//! Shrinks the capacity of the given GblStringBuffer to match its length, if it doesn't already, returning a result code
GBL_EXPORT GBL_RESULT GblStringBuffer_shrinkToFit (GBL_SELF)                  GBL_NOEXCEPT;
//! @}

GBL_DECLS_END

//! \cond
#define GblStringBuffer_construct(...) \
    GBL_VA_OVERLOAD_CALL_ARGC(GblStringBuffer_construct, __VA_ARGS__)
#define GblStringBuffer_construct_1(self) \
    GblStringBuffer_construct_2(self, "")
#define GblStringBuffer_construct_2(self, str) \
    GblStringBuffer_construct_3(self, str, 0)
#define GblStringBuffer_construct_3(self, str, len) \
    GblStringBuffer_construct_4(self, str, len, sizeof(GblStringBuffer))
#define GblStringBuffer_construct_4(self, str, len, size) \
    GblStringBuffer_construct_5(self, str, len, size, GBL_NULL)
#define GblStringBuffer_construct_5(self, str, len, size, ctx) \
    ((GblStringBuffer_construct(self, str, len, size, ctx)))

#define GblStringBuffer_view(...) \
    GblStringBuffer_viewDefault_(__VA_ARGS__)
#define GblStringBuffer_viewDefault_(...) \
    GblStringBuffer_viewDefault__(__VA_ARGS__, 0, 0)
#define GblStringBuffer_viewDefault__(buffer, offset, length, ...) \
    ((GblStringBuffer_view)(buffer, offset, length))

#define GblStringBuffer_set(...) \
    GblStringBuffer_setDefault_(__VA_ARGS__)
#define GblStringBuffer_setDefault_(...) \
    GblStringBuffer_setDefault__(__VA_ARGS__, 0)
#define GblStringBuffer_setDefault__(buffer, str, len, ...) \
    ((GblStringBuffer_set)(buffer, str, len))

#define GblStringBuffer_insert(...) \
    GblStringBuffer_insertDefault_(__VA_ARGS__)
#define GblStringBuffer_insertDefault_(...) \
    GblStringBuffer_insertDefault__(__VA_ARGS__, 0)
#define GblStringBuffer_insertDefault__(buffer, offset, str, len, ...) \
    ((GblStringBuffer_insert)(buffer, offset, str, len))

#define GblStringBuffer_append(...) \
    GblStringBuffer_appendDefault_(__VA_ARGS__)
#define GblStringBuffer_appendDefault_(...) \
    GblStringBuffer_appendDefault__(__VA_ARGS__, 0)
#define GblStringBuffer_appendDefault__(buffer, str, len, ...) \
    ((GblStringBuffer_append)(buffer, str, len))

#define GblStringBuffer_prepend(...) \
    GblStringBuffer_prependDefault_(__VA_ARGS__)
#define GblStringBuffer_prependDefault_(...) \
    GblStringBuffer_prependDefault__(__VA_ARGS__, 0)
#define GblStringBuffer_prependDefault__(buffer, str, len, ...) \
    ((GblStringBuffer_prepend)(buffer, str, len))

#define GblStringBuffer_overwrite(...) \
    GblStringBuffer_overwriteDefault_(__VA_ARGS__)
#define GblStringBuffer_overwriteDefault_(...) \
    GblStringBuffer_overwriteDefault__(__VA_ARGS__, 0)
#define GblStringBuffer_overwriteDefault__(buffer, offset, str, len, ...) \
    ((GblStringBuffer_overwrite)(buffer, offset, str, len))

#define GblStringBuffer_remove(...) \
    GblStringBuffer_removeDefault_(__VA_ARGS__)
#define GblStringBuffer_removeDefault_(...) \
    GblStringBuffer_removeDefault__(__VA_ARGS__, 0)
#define GblStringBuffer_removeDefault__(buffer, str, len, ...) \
    ((GblStringBuffer_remove)(buffer, str, len))

#define GblStringBuffer_replace(...) \
    GblStringBuffer_replaceDefault_(__VA_ARGS__)
#define GblStringBuffer_replaceDefault_(...) \
    GblStringBuffer_replaceDefault__(__VA_ARGS__, 0, 0, 0)
#define GblStringBuffer_replaceDefault__(buffer, substr, repl, count, substrLen, replLen, ...) \
    (GblStringBuffer_replace(buffer, substr, repl, count, substrLen, replLen))
//! \endcond

#undef GBL_SELF_TYPE

#endif // GIMBAL_STRING_BUFFER_H

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
 *  \author     2023 Falco Girgis
 *  \copyright  MIT License
 */

#ifndef GIMBAL_STRING_BUFFER_H
#define GIMBAL_STRING_BUFFER_H

#include "../containers/gimbal_array_list.h"
#include "gimbal_string_view.h"
#include <ctype.h>
#include <stdarg.h>
#include <math.h>

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
    GblArrayList data;
} GblStringBuffer;

// no strv, total size first arg
GBL_EXPORT GBL_RESULT       GblStringBuffer_construct_4     (GBL_SELF,
                                                             GblStringView  view,
                                                             size_t         extraStackSize,
                                                             GblContext*    pCtx)                           GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblStringBuffer_construct_3     (GBL_SELF,
                                                             GblStringView  view,
                                                             size_t         extraStackSize)                 GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblStringBuffer_construct_2     (GBL_SELF,
                                                             GblStringView  view)                           GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblStringBuffer_construct_1     (GBL_SELF)                                      GBL_NOEXCEPT;

GBL_EXPORT GblStringBuffer* GblStringBuffer_createInPlace_4 (GBL_SELF,
                                                             GblStringView  view,
                                                             size_t         extraStackSize,
                                                             GblContext*    pCtx)                           GBL_NOEXCEPT;
GBL_EXPORT GblStringBuffer* GblStringBuffer_createInPlace_3 (GBL_SELF,
                                                             GblStringView  view,
                                                             size_t         extraStackSize)                 GBL_NOEXCEPT;
GBL_EXPORT GblStringBuffer* GblStringBuffer_createInPlace_2 (GBL_SELF,
                                                             GblStringView  view)                           GBL_NOEXCEPT;
GBL_EXPORT GblStringBuffer* GblStringBuffer_createInPlace_1 (GBL_SELF)                                      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT       GblStringBuffer_destruct        (GBL_SELF)                                      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT       GblStringBuffer_acquire         (GBL_SELF, char* pData, size_t  capacity)       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblStringBuffer_release         (GBL_SELF, char** ppStrPtr, size_t * pCapacity) GBL_NOEXCEPT;

// swap?

GBL_EXPORT char*            GblStringBuffer_stackBuffer     (GBL_CSELF)                                     GBL_NOEXCEPT;
GBL_EXPORT GblBool          GblStringBuffer_empty           (GBL_CSELF)                                     GBL_NOEXCEPT;
GBL_EXPORT GblBool          GblStringBuffer_valid           (GBL_CSELF)                                     GBL_NOEXCEPT;
// valid
// blank
GBL_EXPORT GblBool          GblStringBuffer_stack           (GBL_CSELF)                                     GBL_NOEXCEPT;
GBL_EXPORT char*            GblStringBuffer_data            (GBL_SELF)                                      GBL_NOEXCEPT;
GBL_EXPORT const char*      GblStringBuffer_cString         (GBL_CSELF)                                     GBL_NOEXCEPT;
GBL_EXPORT GblStringView    GblStringBuffer_view            (GBL_CSELF)                                     GBL_NOEXCEPT; // optional range
GBL_EXPORT GblQuark         GblStringBuffer_quark           (GBL_CSELF)                                     GBL_NOEXCEPT;
//try quark
//ref
GBL_EXPORT GblContext*      GblStringBuffer_context         (GBL_CSELF)                                     GBL_NOEXCEPT;
GBL_EXPORT size_t           GblStringBuffer_stackBytes      (GBL_CSELF)                                     GBL_NOEXCEPT;
GBL_EXPORT size_t           GblStringBuffer_length          (GBL_CSELF)                                     GBL_NOEXCEPT;
GBL_EXPORT size_t           GblStringBuffer_capacity        (GBL_CSELF)                                     GBL_NOEXCEPT;

GBL_EXPORT const char*      GblStringBuffer_set             (GBL_SELF, GblStringView view)                  GBL_NOEXCEPT; // no strv
GBL_EXPORT char             GblStringBuffer_char            (GBL_CSELF, size_t index)                       GBL_NOEXCEPT; // at
GBL_EXPORT GBL_RESULT       GblStringBuffer_setChar         (GBL_CSELF, size_t index, char value)           GBL_NOEXCEPT;
GBL_EXPORT const char*      GblStringBuffer_printf          (GBL_SELF, const char* pFmt, ...)               GBL_NOEXCEPT;
GBL_EXPORT const char*      GblStringBuffer_vPrintf         (GBL_SELF, const char* pFmt, va_list varArgs)   GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT       GblStringBuffer_insert          (GBL_SELF,
                                                             size_t         index,
                                                             GblStringView  view)                           GBL_NOEXCEPT; //no strv

GBL_EXPORT GBL_RESULT       GblStringBuffer_append          (GBL_SELF, GblStringView other)                 GBL_NOEXCEPT; //no strv
GBL_EXPORT GBL_RESULT       GblStringBuffer_prepend         (GBL_SELF, GblStringView other)                 GBL_NOEXCEPT; //no strv

GBL_EXPORT GBL_RESULT       GblStringBuffer_appendVPrintf   (GBL_SELF, const char* pFmt, va_list varArgs)   GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblStringBuffer_appendPrintf    (GBL_SELF, const char* pFmt, ...)               GBL_NOEXCEPT;

//insert/prepend variations
GBL_EXPORT GBL_RESULT       GblStringBuffer_appendNil       (GBL_SELF)                                      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblStringBuffer_appendBool      (GBL_SELF, GblBool value)                       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblStringBuffer_appendInt       (GBL_SELF, int value)                           GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblStringBuffer_appendUint      (GBL_SELF, unsigned value)                      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblStringBuffer_appendFloat     (GBL_SELF, float value)                         GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblStringBuffer_appendDouble    (GBL_SELF, double value)                        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblStringBuffer_appendPointer   (GBL_SELF, const void* pPtr)                    GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT       GblStringBuffer_erase           (GBL_SELF, size_t offset, size_t length)        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblStringBuffer_clear           (GBL_SELF)                                      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT       GblStringBuffer_overwrite       (GBL_SELF,
                                                             size_t         index,
                                                             GblStringView  other)                          GBL_NOEXCEPT; // no strv

GBL_EXPORT size_t           GblStringBuffer_replace         (GBL_SELF,
                                                             GblStringView substr,
                                                             GblStringView replacement,
                                                             size_t        limit)                           GBL_NOEXCEPT; // no strv?

GBL_EXPORT GBL_RESULT       GblStringBuffer_remove          (GBL_SELF, GblStringView substr)                GBL_NOEXCEPT; // no strv
GBL_EXPORT GBL_RESULT       GblStringBuffer_chop            (GBL_SELF)                                      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblStringBuffer_chomp           (GBL_SELF)                                      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblStringBuffer_lower           (GBL_SELF)                                      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblStringBuffer_upper           (GBL_SELF)                                      GBL_NOEXCEPT;
//reverse
GBL_EXPORT GBL_RESULT       GblStringBuffer_padLeft         (GBL_SELF, char value, size_t count)            GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblStringBuffer_padRight        (GBL_SELF, char value, size_t count)            GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblStringBuffer_trimStart       (GBL_SELF, char value)                          GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblStringBuffer_trimEnd         (GBL_SELF, char value)                          GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT       GblStringBuffer_reserve         (GBL_SELF, size_t capacity)                     GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblStringBuffer_resize          (GBL_SELF, size_t size)                         GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblStringBuffer_grow            (GBL_SELF, size_t delta)                        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblStringBuffer_shrink          (GBL_SELF, size_t delta)                        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblStringBuffer_shrinkToFit     (GBL_SELF)                                      GBL_NOEXCEPT;

GBL_DECLS_END

//! \cond
#define GblStringBuffer_replace(...) \
    GblStringBuffer_replaceDefault_(__VA_ARGS__)

#define GblStringBuffer_construct(...) \
    GBL_VA_OVERLOAD_CALL_ARGC(GblStringBuffer_construct, __VA_ARGS__)

#define GblStringBuffer_createInPlace(...) \
    GBL_VA_OVERLOAD_CALL_ARGC(GblStringBuffer_createInPlace, __VA_ARGS__)

#define GBL_STRING_BUFFER_ALLOCA(...) \
    GBL_VA_OVERLOAD_CALL_ARGC(GBL_STRING_BUFFER_ALLOCA, __VA_ARGS__)

#define GBL_STRING_BUFFER_ALLOCA_3(size, ctx, view) \
    GblStringBuffer_createInPlace_4(GBL_ALLOCA(sizeof(GblStringBuffer) + size), view, sizeof(GblStringBuffer)+size, ctx)

#define GBL_STRING_BUFFER_ALLOCA_2(size, ctx) \
    GBL_STRING_BUFFER_ALLOCA_3(size, ctx, GBL_STRV(""))

#define GBL_STRING_BUFFER_ALLOCA_1(size) \
    GBL_STRING_BUFFER_ALLOCA_2(size, GBL_NULL)

#define GBL_STRING_BUFFER_ALLOCA_0() \
    GBL_STRING_BUFFER_ALLOCA_1(0)

#define GblStringBuffer_replaceDefault_(...) \
    GblStringBuffer_replaceDefault__(__VA_ARGS__, 1)
#define GblStringBuffer_replaceDefault__(buffer, substr, repl, count, ...) \
    (GblStringBuffer_replace(buffer, substr, repl, count))
//! \endcond

#undef GBL_SELF_TYPE

#endif // GIMBAL_STRING_BUFFER_H

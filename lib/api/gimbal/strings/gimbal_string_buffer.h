/*! \file
 *  \brief GblStringBuffer structure and related functions
 */

#ifndef GIMBAL_STRING_BUFFER_H
#define GIMBAL_STRING_BUFFER_H

#include "../containers/gimbal_vector.h"
#include "gimbal_string_view.h"
#include <ctype.h>
#include <stdarg.h>
#include <math.h>

#define GBL_STRING_BUFFER_ALLOCA_4(data, len, extra, ctx)                       \
    GblStringBuffer_createInPlace(GBL_ALLOCA(sizeof(GblStringBuffer)+extra),    \
    data, len, sizeof(GblStringBuffer)+extra, ctx)                              \

#define GBL_STRING_BUFFER_ALLOCA_3(data, len, extra) \
    GBL_STRING_BUFFER_ALLOCA_4(data, len, extra, ctx)

#define GBL_STRING_BUFFER_ALLOCA_2(data, len) \
    GBL_STRING_BUFFER_ALLOCA_3(data, len, 0)

#define GBL_STRING_BUFFER_ALLOCA_1(cstring) \
    GBL_STRING_BUFFER_ALLOCA_2(cstring, 0)

#define GBL_STRING_BUFFER_ALLOCA_0() \
    GBL_STRING_BUFFER_ALLOCA_1(NULL)

#define GBL_STRING_BUFFER_ALLOCA(...) \
     GBL_VA_OVERLOAD_SELECT(GBL_STRING_BUFFER_ALLOCA, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)(__VA_ARGS__)

#define SELF    GblStringBuffer* pSelf
#define CSELF   const SELF

GBL_DECLS_BEGIN


/*! \brief Mutable string type optimized for building and writing
 *
 *  GblStringBuffer is a type of string which is optimized for efficient
 *  piece-wise construction, appending, and writing. It's equivalent to
 *  a "String Builder" type in other languages, such as Java.
 *
 *  It is typically used temporarily to construct a string, afterwards
 *  it is usually converted to another type for storage, such as a
 *  GblStringRef or the internal buffer is taken away from it to be stored
 *  elsewhere.
 *
 *  Internally it is implemented similarly to a C++-vector, with both a
 *  size and a capacity, growing when needed, but not immediatley shrinking.
 *
 *  \note GblStringBuffer supports being created with additional trailing storage,
 *  allowing it to be over-allocated with malloc() or GBL_ALLOCA(). This means
 *  it will not create a separate heap allocation until necessary, and will
 *  instead useits trailing allocation region as its internal buffer. This can
 *  be very efficient when building temporary strings.
 *
 *  \ingroup strings
 *  \sa GblStringRef, GblStringView, GblQuark
 */
typedef struct GblStringBuffer {
    GblVector data;
} GblStringBuffer;

GBL_INLINE GBL_RESULT       GblStringBuffer_construct_4     (SELF,
                                                             GblStringView  view,
                                                             GblSize        extraStackSize,
                                                             GblContext*    pCtx)                       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_construct_3     (SELF,
                                                             GblStringView  view,
                                                             GblSize        extraStackSize)             GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_construct_2     (SELF,
                                                             GblStringView  view)                       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_construct_1     (SELF)                                      GBL_NOEXCEPT;
#define                     GblStringBuffer_construct(...) \
                                GBL_VA_OVERLOAD_SELECT(GblStringBuffer_construct, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)(__VA_ARGS__)

GBL_INLINE GblStringBuffer* GblStringBuffer_createInPlace_4 (SELF,
                                                             GblStringView  view,
                                                             GblSize        extraStackSize,
                                                             GblContext*    pCtx)                       GBL_NOEXCEPT;
GBL_INLINE GblStringBuffer* GblStringBuffer_createInPlace_3 (SELF,
                                                             GblStringView  view,
                                                             GblSize        extraStackSize)             GBL_NOEXCEPT;
GBL_INLINE GblStringBuffer* GblStringBuffer_createInPlace_2 (SELF,
                                                             GblStringView  view)                       GBL_NOEXCEPT;
GBL_INLINE GblStringBuffer* GblStringBuffer_createInPlace_1 (SELF)                                      GBL_NOEXCEPT;
#define                     GblStringBuffer_createInPlace(...) \
                                GBL_VA_OVERLOAD_SELECT(GblStringBuffer_createInPlace, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)(__VA_ARGS__)


GBL_INLINE GBL_RESULT       GblStringBuffer_destruct        (SELF)                                      GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT       GblStringBuffer_acquire         (SELF, char* pData, GblSize capacity)       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_release         (SELF, char** ppStrPtr, GblSize* pCapacity) GBL_NOEXCEPT;

GBL_INLINE char*            GblStringBuffer_stackBuffer     (CSELF)                                     GBL_NOEXCEPT;
GBL_INLINE GblBool          GblStringBuffer_empty           (CSELF)                                     GBL_NOEXCEPT;
GBL_INLINE GblBool          GblStringBuffer_stack           (CSELF)                                     GBL_NOEXCEPT;
GBL_INLINE const char*      GblStringBuffer_cString         (CSELF)                                     GBL_NOEXCEPT;
GBL_INLINE GblStringView    GblStringBuffer_view            (CSELF)                                     GBL_NOEXCEPT;
GBL_INLINE GblContext*      GblStringBuffer_context         (CSELF)                                     GBL_NOEXCEPT;
GBL_INLINE GblSize          GblStringBuffer_stackBytes      (CSELF)                                     GBL_NOEXCEPT;
GBL_INLINE GblSize          GblStringBuffer_length          (CSELF)                                     GBL_NOEXCEPT;
GBL_INLINE GblSize          GblStringBuffer_capacity        (CSELF)                                     GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT       GblStringBuffer_set             (SELF, GblStringView view)                  GBL_NOEXCEPT;
GBL_INLINE char             GblStringBuffer_char            (CSELF, GblSize index)                      GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_charSet         (CSELF, GblSize index, char value)          GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT       GblStringBuffer_insert          (SELF,
                                                             GblSize        index,
                                                             GblStringView  view)                       GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT       GblStringBuffer_append          (SELF, GblStringView other)                 GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_prepend         (SELF, GblStringView other)                 GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT       GblStringBuffer_appendVPrintf   (SELF, const char* pFmt, va_list varArgs)   GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_appendPrintf    (SELF, const char* pFmt, ...)               GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT       GblStringBuffer_appendNil       (SELF)                                      GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_appendBool      (SELF, GblBool value)                       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_appendInt       (SELF, GblInt value)                        GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_appendUint      (SELF, GblUint value)                       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_appendFloat     (SELF, GblFloat value)                      GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_appendDouble    (SELF, double value)                        GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_appendPointer   (SELF, const void* pPtr)                    GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT       GblStringBuffer_erase           (SELF, GblSize offset, GblSize length)      GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_clear           (SELF)                                      GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_overwrite       (SELF,
                                                             GblSize        index,
                                                             GblStringView  other)                      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT       GblStringBuffer_replace         (SELF,
                                                             GblStringView substr,
                                                             GblStringView replacement,
                                                             GblSize       limit)                       GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT       GblStringBuffer_remove          (SELF, GblStringView substr)                GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_chop            (SELF)                                      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblStringBuffer_chomp           (SELF)                                      GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_lower           (SELF)                                      GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_upper           (SELF)                                      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT       GblStringBuffer_padLeft         (SELF, char value, GblSize count)           GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblStringBuffer_padRight        (SELF, char value, GblSize count)           GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblStringBuffer_trimStart       (SELF, char value)                          GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblStringBuffer_trimEnd         (SELF, char value)                          GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT       GblStringBuffer_reserve         (SELF, GblSize capacity)                    GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_resize          (SELF, GblSize size)                        GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_grow            (SELF, GblSize delta)                       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_shrink          (SELF, GblSize delta)                       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_shrinkToFit     (SELF)                                      GBL_NOEXCEPT;

//========== IMPL ==========

GBL_INLINE char* GblStringBuffer_stackBuffer(CSELF) GBL_NOEXCEPT {
    return (char*)GblVector_stackBuffer(&pSelf->data);
}

GBL_INLINE GblBool GblStringBuffer_empty(CSELF) GBL_NOEXCEPT {
    return pSelf? GblVector_empty(&pSelf->data) : GBL_TRUE;
}

GBL_INLINE GblBool GblStringBuffer_stack(CSELF) GBL_NOEXCEPT {
    return pSelf? GblVector_stack(&pSelf->data) : GBL_FALSE;
}

GBL_INLINE const char* GblStringBuffer_cString(CSELF) GBL_NOEXCEPT {
    GBL_ASSERT(pSelf);
    static const char empty = '\0';
    const char* pStr = &empty;
    if(pSelf) {
        if(pSelf->data.pData && pSelf->data.size)
            pStr = (const char*)pSelf->data.pData;
    }
    return pStr;
}

GBL_INLINE GblStringView GblStringBuffer_view(CSELF) GBL_NOEXCEPT {
    GblStringView view = {
        .pData          = NULL,
        .nullTerminated = 0,
        .length         = 0
    };
    if(pSelf) {
        view.pData          = (const char*)pSelf->data.pData;
        view.nullTerminated = 1;
        view.length         = pSelf->data.size;
    }
    return view;
}

GBL_INLINE GBL_RESULT GblStringBuffer_destruct(SELF) GBL_NOEXCEPT {
    return GblVector_destruct(&pSelf->data);
}

GBL_INLINE GBL_RESULT GblStringBuffer_clear(SELF) GBL_NOEXCEPT {
    return GblVector_clear(&pSelf->data);
}

GBL_INLINE GBL_RESULT GblStringBuffer_set(SELF, GblStringView view) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->data.pCtx);
    GBL_API_CALL(GblVector_assign(&pSelf->data, view.pData, view.length));
    GBL_API_END();
}


GBL_INLINE GBL_RESULT GblStringBuffer_construct_4(SELF,
                                                  GblStringView  view,
                                                  GblSize        structSize,
                                                  GblContext*    pCtx) GBL_NOEXCEPT
{
    GBL_API_BEGIN(pCtx);
    GBL_API_VERIFY_ARG(structSize >= sizeof(GblStringBuffer));
    GBL_API_CALL(GblVector_construct(&pSelf->data, 1, view.length, view.pData, structSize, pCtx));
    GBL_API_END();
}
GBL_INLINE GBL_RESULT GblStringBuffer_construct_3(SELF,
                                                  GblStringView   view,
                                                  GblSize        structSize) GBL_NOEXCEPT
{
    return GblStringBuffer_construct_4(pSelf, view, structSize, NULL);
}
GBL_INLINE GBL_RESULT GblStringBuffer_construct_2(SELF,
                                                  GblStringView  view) GBL_NOEXCEPT
{
    return GblStringBuffer_construct_3(pSelf, view, sizeof(GblStringBuffer));
}
GBL_INLINE GBL_RESULT GblStringBuffer_construct_1(SELF) GBL_NOEXCEPT
{
    return GblStringBuffer_construct_2(pSelf, GBL_STRING_VIEW(GBL_NULL));
}

GBL_INLINE GblStringBuffer* GblStringBuffer_createInPlace_4 (SELF,
                                                             GblStringView  view,
                                                             GblSize        extraStackSize,
                                                             GblContext*    pCtx) GBL_NOEXCEPT
{
    GblStringBuffer_construct_4(pSelf, view, extraStackSize, pCtx);
    return pSelf;
}

GBL_INLINE GblStringBuffer* GblStringBuffer_createInPlace_3 (SELF,
                                                             GblStringView  view,
                                                             GblSize        extraStackSize) GBL_NOEXCEPT
{
    GblStringBuffer_construct_3(pSelf, view, extraStackSize);
    return pSelf;
}

GBL_INLINE GblStringBuffer* GblStringBuffer_createInPlace_2 (SELF,
                                                             GblStringView  view) GBL_NOEXCEPT
{
    GblStringBuffer_construct_2(pSelf, view);
    return pSelf;
}

GBL_INLINE GblStringBuffer* GblStringBuffer_createInPlace_1 (SELF) GBL_NOEXCEPT
{
    GblStringBuffer_construct_1(pSelf);
    return pSelf;
}


GBL_INLINE GBL_RESULT GblStringBuffer_release(SELF, char** ppStrPtr, GblSize* pCapacity) GBL_NOEXCEPT {
    GblSize size = 0;
    return GblVector_release(&pSelf->data, (void**)ppStrPtr, &size, pCapacity);
}

GBL_INLINE GBL_RESULT GblStringBuffer_acquire(SELF, char* pData, GblSize capacity) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->data.pCtx);
    if(!pData && !capacity) GBL_API_DONE();
    if(!capacity && pData) capacity = strlen(pData);
    GBL_API_CALL(GblVector_acquire(&pSelf->data, pData, capacity, capacity));
    pSelf->data.size = strlen((const char*)pSelf->data.pData);
    GBL_API_END();
}

GBL_INLINE GblContext* GblStringBuffer_context(CSELF) GBL_NOEXCEPT {
    return pSelf? GblVector_context(&pSelf->data) : NULL;
}

GBL_INLINE GblSize GblStringBuffer_stackBytes(CSELF) GBL_NOEXCEPT {
    return pSelf? GblVector_stackBytes(&pSelf->data) : 0;
}

GBL_INLINE GblSize GblStringBuffer_length(CSELF) GBL_NOEXCEPT {
    return pSelf? GblVector_size(&pSelf->data) : 0;
}

GBL_INLINE GblSize GblStringBuffer_capacity(CSELF) GBL_NOEXCEPT {
    return pSelf? GblVector_capacity(&pSelf->data) : 0;
}

GBL_INLINE GBL_RESULT GblStringBuffer_reserve(SELF, GblSize capacity) GBL_NOEXCEPT {
    return GblVector_reserve(&pSelf->data, capacity);
}

GBL_INLINE GBL_RESULT GblStringBuffer_resize(SELF, GblSize size) GBL_NOEXCEPT {
    return GblVector_resize(&pSelf->data, size);
}

GBL_INLINE GBL_RESULT GblStringBuffer_grow(SELF, GblSize delta) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->data.pCtx);
    GBL_API_CALL(GblStringBuffer_resize(pSelf, GblStringBuffer_length(pSelf) + delta));
    GBL_API_END();
}
GBL_INLINE GBL_RESULT GblStringBuffer_shrink(SELF, GblSize delta) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->data.pCtx);
    GBL_API_CALL(GblStringBuffer_resize(pSelf, GblStringBuffer_length(pSelf) - delta));
    GBL_API_END();
}

GBL_INLINE GblInt GblStringBuffer_compare(CSELF, const GblStringBuffer* pOther) GBL_NOEXCEPT {
    GBL_ASSERT(pSelf);
    GBL_ASSERT(pOther);
    GblInt result = INT_MAX;
    const char* pCStr1 = NULL;
    const char* pCStr2 = NULL;
    GBL_API_BEGIN(pSelf->data.pCtx);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_POINTER(pOther);

    pCStr1 = GblStringBuffer_cString(pSelf);
    pCStr2 = GblStringBuffer_cString(pOther);

    if(pCStr1 == pCStr2) GBL_UNLIKELY {
        result = 0;
    } else if(!pCStr1) {
        result = INT_MIN;
    } else if(!pCStr2) {
        result = INT_MAX;
    } else GBL_LIKELY { //both valid
        result = strcmp(pCStr1, pCStr2);
    }
    GBL_API_END_BLOCK();
    return result;
}

GBL_INLINE GBL_RESULT GblStringBuffer_insert(SELF, GblSize index, GblStringView view) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->data.pCtx);
    GBL_API_VERIFY_POINTER(view.pData);
    GBL_API_VERIFY_EXPRESSION(view.length);
    GblVector_insert(&pSelf->data, index, view.length, view.pData);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblStringBuffer_appendPrintf(SELF, const char* pFmt, ...) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->data.pCtx);
    va_list varArgs;
    va_start(varArgs, pFmt);
    GBL_API_CALL(GblStringBuffer_appendVPrintf(pSelf, pFmt, varArgs));
    va_end(varArgs);
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblStringBuffer_appendNil(SELF) GBL_NOEXCEPT  {
    GBL_API_BEGIN(pSelf->data.pCtx);
    GBL_API_CALL(GblStringBuffer_append(pSelf, GBL_STRING_VIEW("nil")));
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblStringBuffer_appendBool(SELF, GblBool value) GBL_NOEXCEPT {
    const char* pStr = NULL;
    GBL_API_BEGIN(pSelf->data.pCtx);
    pStr = value ? "true" : "false";
    GBL_API_CALL(GblStringBuffer_append(pSelf, GBL_STRING_VIEW(pStr)));
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblStringBuffer_appendInt(SELF, GblInt value) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->data.pCtx);
    GBL_API_CALL(GblStringBuffer_appendPrintf(pSelf, "%d", value));
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblStringBuffer_appendUint(SELF, GblUint value) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->data.pCtx);
    GBL_API_CALL(GblStringBuffer_appendPrintf(pSelf, "%u", value));
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblStringBuffer_appendFloat(SELF, GblFloat value) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->data.pCtx);
    GBL_API_CALL(GblStringBuffer_appendPrintf(pSelf, "%." GBL_STRINGIFY(GBL_STRING_FLOAT_DECIMAL_PLACES) "f", value));
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblStringBuffer_appendDouble(SELF, double value) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->data.pCtx);
    GBL_API_CALL(GblStringBuffer_appendPrintf(pSelf, "%." GBL_STRINGIFY(GBL_STRING_FLOAT_DECIMAL_PLACES) "lf", value));
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblStringBuffer_appendPointer(SELF, const void* pPtr) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->data.pCtx);
    GBL_API_CALL(GblStringBuffer_appendPrintf(pSelf, "0x%x", pPtr));
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblStringBuffer_append(SELF, GblStringView view) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->data.pCtx);
    GBL_API_CALL(GblVector_append(&pSelf->data, view.pData, view.length));
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblStringBuffer_prepend(SELF, GblStringView view) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->data.pCtx);
    GBL_API_CALL(GblVector_prepend(&pSelf->data, view.pData, view.length));
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblStringBuffer_overwrite(SELF, GblSize index, GblStringView view) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->data.pCtx);
    GBL_API_VERIFY_ARG(index < GblStringBuffer_length(pSelf));
    GBL_API_CALL(GblStringBuffer_reserve(pSelf, index + view.length));
    memcpy(pSelf->data.pData+index, view.pData, view.length);
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblStringBuffer_erase(SELF, GblSize index, GblSize len) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->data.pCtx);
    GBL_API_CALL(GblVector_erase(&pSelf->data, index, len));
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblStringBuffer_shrinkToFit(SELF) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->data.pCtx);
    GBL_API_CALL(GblVector_shrinkToFit(&pSelf->data));
    GBL_API_END();
}

GBL_INLINE char GblStringBuffer_char(CSELF, GblSize index) GBL_NOEXCEPT {
    char result = '\0';
    GBL_API_BEGIN(pSelf->data.pCtx);
    const char* pChar = (const char*)GblVector_at(&pSelf->data, index);
    GBL_API_VERIFY_LAST_RECORD();
    if(pChar) result = *pChar;
    GBL_API_END_BLOCK();
    return result;
}

GBL_INLINE GBL_RESULT GblStringBuffer_charSet(CSELF, GblSize index, char value) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->data.pCtx);
    GBL_API_VERIFY(index < GblStringBuffer_length(pSelf), GBL_RESULT_ERROR_OUT_OF_RANGE);
    pSelf->data.pData[index] = value;
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblStringBuffer_chop(SELF) GBL_NOEXCEPT {
    return GblStringBuffer_shrink(pSelf, 1);
}

GBL_INLINE GBL_RESULT GblStringBuffer_lower(SELF) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->data.pCtx);
    for(GblSize c = 0; c < GblStringBuffer_length(pSelf); ++c) {
        GblStringBuffer_charSet(pSelf, c, tolower(GblStringBuffer_char(pSelf, c)));
    }
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblStringBuffer_upper(SELF) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->data.pCtx);
    for(GblSize c = 0; c < GblStringBuffer_length(pSelf); ++c) {
        GblStringBuffer_charSet(pSelf, c, toupper(GblStringBuffer_char(pSelf, c)));
    }
    GBL_API_END();
}


GBL_DECLS_END

#undef CSELF
#undef SELF

#endif // GIMBAL_STRING_BUFFER_H

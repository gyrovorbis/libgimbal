/*! \file
 *  \brief GblStringBuffer structure and related functions
 *  \ingroup strings
 */

#ifndef GIMBAL_STRING_BUFFER_H
#define GIMBAL_STRING_BUFFER_H

#include "../containers/gimbal_array_list.h"
#include "gimbal_string_view.h"
#include <ctype.h>
#include <stdarg.h>
#include <math.h>

#define GBL_STRING_BUFFER_ALLOCA(...) \
     GBL_VA_OVERLOAD_SELECT(GBL_STRING_BUFFER_ALLOCA, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)(__VA_ARGS__)

#define GBL_SELF_TYPE GblStringBuffer

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
    //GBL_PRIVATE(GblStringBuffer)
        GblArrayList data;
    //GBL_PRIVATE_END
} GblStringBuffer;

GBL_INLINE GBL_RESULT       GblStringBuffer_construct_4     (GBL_SELF,
                                                             GblStringView  view,
                                                             GblSize        extraStackSize,
                                                             GblContext*    pCtx)                           GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_construct_3     (GBL_SELF,
                                                             GblStringView  view,
                                                             GblSize        extraStackSize)                 GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_construct_2     (GBL_SELF,
                                                             GblStringView  view)                           GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_construct_1     (GBL_SELF)                                      GBL_NOEXCEPT;
#define                     GblStringBuffer_construct(...) \
                                GBL_VA_OVERLOAD_CALL(GblStringBuffer_construct, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)

GBL_INLINE GblStringBuffer* GblStringBuffer_createInPlace_4 (GBL_SELF,
                                                             GblStringView  view,
                                                             GblSize        extraStackSize,
                                                             GblContext*    pCtx)                           GBL_NOEXCEPT;
GBL_INLINE GblStringBuffer* GblStringBuffer_createInPlace_3 (GBL_SELF,
                                                             GblStringView  view,
                                                             GblSize        extraStackSize)                 GBL_NOEXCEPT;
GBL_INLINE GblStringBuffer* GblStringBuffer_createInPlace_2 (GBL_SELF,
                                                             GblStringView  view)                           GBL_NOEXCEPT;
GBL_INLINE GblStringBuffer* GblStringBuffer_createInPlace_1 (GBL_SELF)                                      GBL_NOEXCEPT;
#define                     GblStringBuffer_createInPlace(...) \
                                GBL_VA_OVERLOAD_CALL(GblStringBuffer_createInPlace, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)

GBL_INLINE GBL_RESULT       GblStringBuffer_destruct        (GBL_SELF)                                      GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT       GblStringBuffer_acquire         (GBL_SELF, char* pData, GblSize capacity)       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_release         (GBL_SELF, char** ppStrPtr, GblSize* pCapacity) GBL_NOEXCEPT;

GBL_INLINE char*            GblStringBuffer_stackBuffer     (GBL_CSELF)                                     GBL_NOEXCEPT;
GBL_INLINE GblBool          GblStringBuffer_empty           (GBL_CSELF)                                     GBL_NOEXCEPT;
GBL_INLINE GblBool          GblStringBuffer_stack           (GBL_CSELF)                                     GBL_NOEXCEPT;
GBL_INLINE char*            GblStringBuffer_data            (GBL_SELF)                                      GBL_NOEXCEPT;
GBL_INLINE const char*      GblStringBuffer_cString         (GBL_CSELF)                                     GBL_NOEXCEPT;
GBL_INLINE GblStringView    GblStringBuffer_view            (GBL_CSELF)                                     GBL_NOEXCEPT;
GBL_INLINE GblQuark         GblStringBuffer_quark           (GBL_CSELF)                                     GBL_NOEXCEPT;
GBL_INLINE GblContext*      GblStringBuffer_context         (GBL_CSELF)                                     GBL_NOEXCEPT;
GBL_INLINE GblSize          GblStringBuffer_stackBytes      (GBL_CSELF)                                     GBL_NOEXCEPT;
GBL_INLINE GblSize          GblStringBuffer_length          (GBL_CSELF)                                     GBL_NOEXCEPT;
GBL_INLINE GblSize          GblStringBuffer_capacity        (GBL_CSELF)                                     GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT       GblStringBuffer_set             (GBL_SELF, GblStringView view)                  GBL_NOEXCEPT;
GBL_INLINE char             GblStringBuffer_char            (GBL_CSELF, GblSize index)                      GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_setChar         (GBL_CSELF, GblSize index, char value)          GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT       GblStringBuffer_insert          (GBL_SELF,
                                                             GblSize        index,
                                                             GblStringView  view)                           GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT       GblStringBuffer_append          (GBL_SELF, GblStringView other)                 GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_prepend         (GBL_SELF, GblStringView other)                 GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT       GblStringBuffer_appendVPrintf   (GBL_SELF, const char* pFmt, va_list varArgs)   GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_appendPrintf    (GBL_SELF, const char* pFmt, ...)               GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT       GblStringBuffer_appendNil       (GBL_SELF)                                      GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_appendBool      (GBL_SELF, GblBool value)                       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_appendInt       (GBL_SELF, int value)                           GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_appendUint      (GBL_SELF, unsigned value)                      GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_appendFloat     (GBL_SELF, float value)                         GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_appendDouble    (GBL_SELF, double value)                        GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_appendPointer   (GBL_SELF, const void* pPtr)                    GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT       GblStringBuffer_erase           (GBL_SELF, GblSize offset, GblSize length)      GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_clear           (GBL_SELF)                                      GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_overwrite       (GBL_SELF,
                                                             GblSize        index,
                                                             GblStringView  other)                          GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT       GblStringBuffer_replace         (GBL_SELF,
                                                             GblStringView substr,
                                                             GblStringView replacement,
                                                             GblSize       limit)                           GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT       GblStringBuffer_remove          (GBL_SELF, GblStringView substr)                GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_chop            (GBL_SELF)                                      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblStringBuffer_chomp           (GBL_SELF)                                      GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_lower           (GBL_SELF)                                      GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_upper           (GBL_SELF)                                      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT       GblStringBuffer_padLeft         (GBL_SELF, char value, GblSize count)           GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblStringBuffer_padRight        (GBL_SELF, char value, GblSize count)           GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblStringBuffer_trimStart       (GBL_SELF, char value)                          GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblStringBuffer_trimEnd         (GBL_SELF, char value)                          GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT       GblStringBuffer_reserve         (GBL_SELF, GblSize capacity)                    GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_resize          (GBL_SELF, GblSize size)                        GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_grow            (GBL_SELF, GblSize delta)                       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_shrink          (GBL_SELF, GblSize delta)                       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblStringBuffer_shrinkToFit     (GBL_SELF)                                      GBL_NOEXCEPT;

//========== IMPL ==========

#define GBL_STRING_BUFFER_ALLOCA_3(size, ctx, view) \
    GblStringBuffer_createInPlace(GBL_ALLOCA(sizeof(GblStringBuffer) + size), view, sizeof(GblStringBuffer)+size, ctx)

#define GBL_STRING_BUFFER_ALLOCA_2(size, ctx) \
    GBL_STRING_BUFFER_ALLOCA_3(size, ctx, GBL_STRV(""))

#define GBL_STRING_BUFFER_ALLOCA_1(size) \
    GBL_STRING_BUFFER_ALLOCA_2(size, GBL_NULL)

#define GBL_STRING_BUFFER_ALLOCA_0() \
    GBL_STRING_BUFFER_ALLOCA_1(0)

GBL_INLINE char* GblStringBuffer_stackBuffer(GBL_CSELF) GBL_NOEXCEPT {
    return (char*)GblArrayList_stackBuffer(&pSelf->data);
}

GBL_INLINE GblBool GblStringBuffer_empty(GBL_CSELF) GBL_NOEXCEPT {
    return pSelf? GblArrayList_empty(&pSelf->data) : GBL_TRUE;
}

GBL_INLINE GblBool GblStringBuffer_stack(GBL_CSELF) GBL_NOEXCEPT {
    return pSelf? GblArrayList_stack(&pSelf->data) : GBL_FALSE;
}

GBL_INLINE char* GblStringBuffer_data(GBL_SELF) GBL_NOEXCEPT {
    char* pData = GBL_NULL;
    if(pSelf) {
        pData = (char*)GblArrayList_data(&pSelf->data);
    }
    return pData;
}

GBL_INLINE const char* GblStringBuffer_cString(GBL_CSELF) GBL_NOEXCEPT {
    GBL_ASSERT(pSelf);
    static const char empty = '\0';
    const char* pStr = &empty;
    if(pSelf) {
        if(GBL_PRIV(pSelf->data).pData && GBL_PRIV(pSelf->data).size)
            pStr = (const char*) GBL_PRIV(pSelf->data).pData;
    }
    return pStr;
}

GBL_INLINE GblStringView GblStringBuffer_view(GBL_CSELF) GBL_NOEXCEPT {
    GblStringView view = {
        .pData          = NULL,
        .nullTerminated = 0,
        .length         = 0
    };
    if(pSelf) {
        view.pData          = (const char*)GBL_PRIV(pSelf->data).pData;
        view.nullTerminated = 1;
        view.length         = GBL_PRIV(pSelf->data).size;
    }
    return view;
}

GBL_INLINE GblQuark GblStringBuffer_quark(GBL_CSELF) GBL_NOEXCEPT {
    const GblStringView view = GblStringBuffer_view(pSelf);
    return GblQuark_fromStringSized(view.pData, view.length);
}

GBL_INLINE GBL_RESULT GblStringBuffer_destruct(GBL_SELF) GBL_NOEXCEPT {
    return GblArrayList_destruct(&pSelf->data);
}

GBL_INLINE GBL_RESULT GblStringBuffer_clear(GBL_SELF) GBL_NOEXCEPT {
    return GblArrayList_clear(&pSelf->data);
}

GBL_INLINE GBL_RESULT GblStringBuffer_set(GBL_SELF, GblStringView view) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    GBL_CTX_CALL(GblArrayList_assign(&pSelf->data, view.pData, view.length));
    GBL_CTX_END();
}


GBL_INLINE GBL_RESULT GblStringBuffer_construct_4(GBL_SELF,
                                                  GblStringView  view,
                                                  GblSize        structSize,
                                                  GblContext*    pCtx) GBL_NOEXCEPT
{
    GBL_CTX_BEGIN(pCtx);
    GBL_CTX_VERIFY_ARG(structSize >= sizeof(GblStringBuffer));
    GBL_CTX_CALL(GblArrayList_construct(&pSelf->data, 1, view.length, view.pData, structSize, GBL_TRUE, pCtx));
    GBL_CTX_END();
}
GBL_INLINE GBL_RESULT GblStringBuffer_construct_3(GBL_SELF,
                                                  GblStringView   view,
                                                  GblSize        structSize) GBL_NOEXCEPT
{
    return GblStringBuffer_construct_4(pSelf, view, structSize, NULL);
}
GBL_INLINE GBL_RESULT GblStringBuffer_construct_2(GBL_SELF,
                                                  GblStringView  view) GBL_NOEXCEPT
{
    return GblStringBuffer_construct_3(pSelf, view, sizeof(GblStringBuffer));
}
GBL_INLINE GBL_RESULT GblStringBuffer_construct_1(GBL_SELF) GBL_NOEXCEPT
{
    return GblStringBuffer_construct_2(pSelf, GBL_STRING_VIEW(GBL_NULL));
}

GBL_INLINE GblStringBuffer* GblStringBuffer_createInPlace_4 (GBL_SELF,
                                                             GblStringView  view,
                                                             GblSize        extraStackSize,
                                                             GblContext*    pCtx) GBL_NOEXCEPT
{
    GblStringBuffer_construct_4(pSelf, view, extraStackSize, pCtx);
    return pSelf;
}

GBL_INLINE GblStringBuffer* GblStringBuffer_createInPlace_3 (GBL_SELF,
                                                             GblStringView  view,
                                                             GblSize        extraStackSize) GBL_NOEXCEPT
{
    GblStringBuffer_construct_3(pSelf, view, extraStackSize);
    return pSelf;
}

GBL_INLINE GblStringBuffer* GblStringBuffer_createInPlace_2 (GBL_SELF,
                                                             GblStringView  view) GBL_NOEXCEPT
{
    GblStringBuffer_construct_2(pSelf, view);
    return pSelf;
}

GBL_INLINE GblStringBuffer* GblStringBuffer_createInPlace_1 (GBL_SELF) GBL_NOEXCEPT
{
    GblStringBuffer_construct_1(pSelf);
    return pSelf;
}


GBL_INLINE GBL_RESULT GblStringBuffer_release(GBL_SELF, char** ppStrPtr, GblSize* pCapacity) GBL_NOEXCEPT {
    GblSize size = 0;
    return GblArrayList_release(&pSelf->data, (void**)ppStrPtr, &size, pCapacity);
}

GBL_INLINE GBL_RESULT GblStringBuffer_acquire(GBL_SELF, char* pData, GblSize capacity) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    if(!pData && !capacity) GBL_CTX_DONE();
    if(!capacity && pData) capacity = strlen(pData);
    GBL_CTX_CALL(GblArrayList_acquire(&pSelf->data, pData, capacity, capacity));
    GBL_PRIV(pSelf->data).size = strlen((const char*)GBL_PRIV(pSelf->data).pData);
    GBL_CTX_END();
}

GBL_INLINE GblContext* GblStringBuffer_context(GBL_CSELF) GBL_NOEXCEPT {
    return pSelf? GblArrayList_context(&pSelf->data) : NULL;
}

GBL_INLINE GblSize GblStringBuffer_stackBytes(GBL_CSELF) GBL_NOEXCEPT {
    return pSelf? GblArrayList_stackBytes(&pSelf->data) : 0;
}

GBL_INLINE GblSize GblStringBuffer_length(GBL_CSELF) GBL_NOEXCEPT {
    return pSelf? GblArrayList_size(&pSelf->data) : 0;
}

GBL_INLINE GblSize GblStringBuffer_capacity(GBL_CSELF) GBL_NOEXCEPT {
    return pSelf? GblArrayList_capacity(&pSelf->data) : 0;
}

GBL_INLINE GBL_RESULT GblStringBuffer_reserve(GBL_SELF, GblSize capacity) GBL_NOEXCEPT {
    return GblArrayList_reserve(&pSelf->data, capacity);
}

GBL_INLINE GBL_RESULT GblStringBuffer_resize(GBL_SELF, GblSize size) GBL_NOEXCEPT {
    return GblArrayList_resize(&pSelf->data, size);
}

GBL_INLINE GBL_RESULT GblStringBuffer_grow(GBL_SELF, GblSize delta) GBL_NOEXCEPT {
    return GblStringBuffer_resize(pSelf, GblStringBuffer_length(pSelf) + delta);
}

GBL_INLINE GBL_RESULT GblStringBuffer_shrink(GBL_SELF, GblSize delta) GBL_NOEXCEPT {
    return GblStringBuffer_resize(pSelf, GblStringBuffer_length(pSelf) - delta);
}

GBL_INLINE int GblStringBuffer_compare(GBL_CSELF, const GblStringBuffer* pOther) GBL_NOEXCEPT {
    GBL_ASSERT(pSelf);
    GBL_ASSERT(pOther);
    int result = INT_MAX;
    const char* pCStr1 = NULL;
    const char* pCStr2 = NULL;
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    GBL_CTX_VERIFY_POINTER(pSelf);
    GBL_CTX_VERIFY_POINTER(pOther);

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
    GBL_CTX_END_BLOCK();
    return result;
}

GBL_INLINE GBL_RESULT GblStringBuffer_insert(GBL_SELF, GblSize index, GblStringView view) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    GBL_CTX_VERIFY_POINTER(view.pData);
    GBL_CTX_VERIFY_EXPRESSION(view.length);
    GblArrayList_insert(&pSelf->data, index, view.length, view.pData);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblStringBuffer_appendPrintf(GBL_SELF, const char* pFmt, ...) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    va_list varArgs;
    va_start(varArgs, pFmt);
    GBL_CTX_CALL(GblStringBuffer_appendVPrintf(pSelf, pFmt, varArgs));
    va_end(varArgs);
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblStringBuffer_appendNil(GBL_SELF) GBL_NOEXCEPT  {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    GBL_CTX_CALL(GblStringBuffer_append(pSelf, GBL_STRING_VIEW("nil")));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblStringBuffer_appendBool(GBL_SELF, GblBool value) GBL_NOEXCEPT {
    const char* pStr = NULL;
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    pStr = value ? "true" : "false";
    GBL_CTX_CALL(GblStringBuffer_append(pSelf, GBL_STRING_VIEW(pStr)));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblStringBuffer_appendInt(GBL_SELF, int value) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    GBL_CTX_CALL(GblStringBuffer_appendPrintf(pSelf, "%d", value));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblStringBuffer_appendUint(GBL_SELF, unsigned value) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    GBL_CTX_CALL(GblStringBuffer_appendPrintf(pSelf, "%u", value));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblStringBuffer_appendFloat(GBL_SELF, float value) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    GBL_CTX_CALL(GblStringBuffer_appendPrintf(pSelf, "%." GBL_STRINGIFY(GBL_STRING_FLOAT_DECIMAL_PLACES) "f", value));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblStringBuffer_appendDouble(GBL_SELF, double value) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    GBL_CTX_CALL(GblStringBuffer_appendPrintf(pSelf, "%." GBL_STRINGIFY(GBL_STRING_FLOAT_DECIMAL_PLACES) "lf", value));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblStringBuffer_appendPointer(GBL_SELF, const void* pPtr) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    GBL_CTX_CALL(GblStringBuffer_appendPrintf(pSelf, "0x%x", pPtr));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblStringBuffer_append(GBL_SELF, GblStringView view) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    GBL_CTX_CALL(GblArrayList_append(&pSelf->data, view.pData, view.length));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblStringBuffer_prepend(GBL_SELF, GblStringView view) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    GBL_CTX_CALL(GblArrayList_prepend(&pSelf->data, view.pData, view.length));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblStringBuffer_overwrite(GBL_SELF, GblSize index, GblStringView view) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    //GBL_CTX_VERIFY_ARG(index < GblStringBuffer_length(pSelf));
    const GblBool resized = (index + view.length > GblStringBuffer_length(pSelf));
    GBL_CTX_CALL(GblStringBuffer_reserve(pSelf, index + view.length));
    memcpy(GBL_PRIV(pSelf->data).pData+index, view.pData, view.length);
    if(resized) GBL_PRIV(pSelf->data).pData[index + view.length] = '\0';
    GBL_PRIV(pSelf->data).size = strlen((const char*)GBL_PRIV(pSelf->data).pData);
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblStringBuffer_erase(GBL_SELF, GblSize index, GblSize len) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    GBL_CTX_CALL(GblArrayList_erase(&pSelf->data, index, len));
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblStringBuffer_shrinkToFit(GBL_SELF) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    GBL_CTX_CALL(GblArrayList_shrinkToFit(&pSelf->data));

    GBL_CTX_END();
}

GBL_INLINE char GblStringBuffer_char(GBL_CSELF, GblSize index) GBL_NOEXCEPT {
    char result = '\0';
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    const char* pChar = (const char*)GblArrayList_at(&pSelf->data, index);
    GBL_CTX_VERIFY_LAST_RECORD();
    if(pChar) result = *pChar;
    GBL_CTX_END_BLOCK();
    return result;
}

GBL_INLINE GBL_RESULT GblStringBuffer_setChar(GBL_CSELF, GblSize index, char value) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    GBL_CTX_VERIFY(index < GblStringBuffer_length(pSelf), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_PRIV(pSelf->data).pData[index] = value;
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblStringBuffer_chop(GBL_SELF) GBL_NOEXCEPT {
    return GblStringBuffer_shrink(pSelf, 1);
}

GBL_INLINE GBL_RESULT GblStringBuffer_lower(GBL_SELF) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    for(GblSize c = 0; c < GblStringBuffer_length(pSelf); ++c) {
        GblStringBuffer_setChar(pSelf, c, tolower(GblStringBuffer_char(pSelf, c)));
    }
    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblStringBuffer_upper(GBL_SELF) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    for(GblSize c = 0; c < GblStringBuffer_length(pSelf); ++c) {
        GblStringBuffer_setChar(pSelf, c, toupper(GblStringBuffer_char(pSelf, c)));
    }
    GBL_CTX_END();
}


GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_STRING_BUFFER_H

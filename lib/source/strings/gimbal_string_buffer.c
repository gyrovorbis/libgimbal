#include <gimbal/strings/gimbal_string_buffer.h>

GBL_EXPORT GBL_RESULT GblStringBuffer_appendVPrintf(GblStringBuffer* pSelf, const char* pFmt, va_list varArgs) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    size_t  newCapacity  = 0;
    size_t  expectedSize = 0;
    size_t  originalSize = 0;
    va_list varArgsCopy;

    GBL_CTX_VERIFY_POINTER(pFmt);

    //Check if shit's empty
    expectedSize = GblStringBuffer_length(pSelf) + strlen(pFmt) + 1;
    originalSize = GblStringBuffer_length(pSelf);
    if(expectedSize <= 1) GBL_UNLIKELY {
        if(GBL_PRIV(pSelf->data).pData) GBL_PRIV(pSelf->data).pData[0] = '\0';
        GBL_PRIV(pSelf->data).size = 0;
    } else GBL_LIKELY {
        //Make a first guess at the required size
        newCapacity = gblPow2Next(expectedSize);
        GBL_CTX_CALL(GblStringBuffer_reserve(pSelf, newCapacity));
        GBL_CTX_ERRNO_CLEAR();
        va_copy(varArgsCopy, varArgs);
        expectedSize = vsnprintf((char*)GBL_PRIV(pSelf->data).pData+originalSize,
                                 GBL_PRIV(pSelf->data).capacity-originalSize,
                                 pFmt,
                                 varArgsCopy);
        GBL_PRIV(pSelf->data).size = expectedSize + originalSize;
        //GBL_PRIV(pSelf->data).pData[expectedSize] = '\0';
        va_end(varArgsCopy);
        GBL_CTX_PERROR("vsnprintf failed with code: %zu", expectedSize);
        //Multi-pass, try again with real size!
        if(expectedSize >= GBL_PRIV(pSelf->data).capacity) GBL_UNLIKELY {
            newCapacity = gblPow2Next(expectedSize + 1);
            GBL_CTX_CALL(GblStringBuffer_reserve(pSelf, newCapacity));
            GBL_CTX_ERRNO_CLEAR();
            expectedSize = vsnprintf((char*)GBL_PRIV(pSelf->data).pData+originalSize, GBL_PRIV(pSelf->data).capacity-originalSize, pFmt, varArgs);
            GBL_PRIV(pSelf->data).size = expectedSize;
            GBL_CTX_PERROR("vsnprintf failed with code: %zu", expectedSize);
            GBL_CTX_VERIFY(expectedSize < GBL_PRIV(pSelf->data).capacity, GBL_RESULT_TRUNCATED); //better have written it all
        }
    }
    GBL_CTX_END();
}

GBL_EXPORT const char* GblStringBuffer_printf(GblStringBuffer* pSelf, const char* pFmt, ...) {
    va_list varArgs;
    va_start(varArgs, pFmt);

    const char* pRetVal = GblStringBuffer_vPrintf(pSelf, pFmt, varArgs);

    va_end(varArgs);
    return pRetVal;
}

GBL_EXPORT const char* GblStringBuffer_vPrintf(GblStringBuffer* pSelf, const char* pFmt, va_list varArgs) {
    GblStringBuffer_clear(pSelf);
    GblStringBuffer_appendVPrintf(pSelf, pFmt, varArgs);
    return GblStringBuffer_cString(pSelf);
}

GBL_EXPORT GBL_RESULT GblStringBuffer_remove(GblStringBuffer* pSelf, GblStringView view) {
    const char* pNeedle = NULL;
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    GBL_CTX_VERIFY_POINTER(view.pData);
    pNeedle = GBL_STRING_VIEW_CSTR(view);
    while(1) {
        const char* pLoc = strstr(GblStringBuffer_cString(pSelf), pNeedle);
        if(pLoc) {
            size_t  offset = pLoc - GblStringBuffer_cString(pSelf);
            GBL_CTX_CALL(GblStringBuffer_erase(pSelf, offset, view.length));
        } else break;
    }
    GBL_CTX_END();
}

GBL_EXPORT size_t GblStringBuffer_replace(GblStringBuffer* pSelf, GblStringView substr, GblStringView replacement, size_t  limit) {
    const char* pNeedle = NULL;
    size_t iterations = 0;
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    GBL_CTX_VERIFY_POINTER(substr.pData);
    GBL_CTX_VERIFY_POINTER(replacement.pData);
    GBL_CTX_VERIFY_ARG(limit);
    pNeedle = GBL_STRING_VIEW_CSTR(substr);
    while(iterations < limit) {
        const char* pLoc = strstr(GblStringBuffer_cString(pSelf), pNeedle);
        if(pLoc) {
          size_t  offset = pLoc - GblStringBuffer_cString(pSelf);
          GBL_CTX_CALL(GblStringBuffer_erase(pSelf, offset, substr.length));
          GBL_CTX_CALL(GblStringBuffer_insert(pSelf, offset, replacement));
          ++iterations;
        } else break;
    }
    GBL_CTX_END_BLOCK();
    return iterations;
}


GBL_EXPORT GBL_RESULT GblStringBuffer_chomp(GblStringBuffer* pSelf) {
    const size_t  len = GblStringBuffer_length(pSelf);
    const char* pCStr = GblStringBuffer_cString(pSelf);
    unsigned chopCount = 0;
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    GBL_CTX_VERIFY(len >= 1, GBL_RESULT_ERROR_UNDERFLOW);
    GBL_CTX_VERIFY_POINTER(pCStr);
    if(pCStr[len-1] == '\n' || pCStr[len-1] == '\r')
        ++chopCount;
    if(len >= 2) {
        if(pCStr[len-2] == '\n' || pCStr[len-2] == '\r')
            ++chopCount;
    }
    GBL_CTX_CALL(GblStringBuffer_shrink(pSelf, chopCount));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblStringBuffer_padLeft(GblStringBuffer* pSelf, char value, size_t  count) {
    const GblStringView view = {
        .pData          = &value,
        .nullTerminated = 0,
        .length         = 1
    };
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    for(size_t  i = 0; i < count; ++i)
        GBL_CTX_VERIFY_CALL(GblStringBuffer_prepend(pSelf, view));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblStringBuffer_padRight(GblStringBuffer* pSelf, char value, size_t  count) {
    const GblStringView view = {
        .pData          = &value,
        .nullTerminated = 0,
        .length         = 1
    };
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    for(size_t  i = 0; i < count; ++i)
        GBL_CTX_VERIFY_CALL(GblStringBuffer_append(pSelf, view));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblStringBuffer_trimStart(GblStringBuffer* pSelf, char value) {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    size_t  count = 0;

    while(GblStringBuffer_length(pSelf) > count &&
          GblStringBuffer_char(pSelf, count) == value) {
        ++count;
    }

    if(count) {
        GBL_CTX_VERIFY_CALL(GblStringBuffer_erase(pSelf, 0, count));
    }
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblStringBuffer_trimEnd(GblStringBuffer* pSelf, char value) {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    size_t  count = 0;

    while(GblStringBuffer_length(pSelf) > count &&
          GblStringBuffer_char(pSelf, GblStringBuffer_length(pSelf)-1-count) == value) {
        ++count;
    }

    if(count) {
        GBL_CTX_VERIFY_CALL(GblStringBuffer_erase(pSelf,
                                                  GblStringBuffer_length(pSelf)-1-(count-1),
                                                  count));
    }
    GBL_CTX_END();
}


GBL_EXPORT char* GblStringBuffer_stackBuffer(const GblStringBuffer* pSelf) {
    return (char*)GblArrayList_stackBuffer(&pSelf->data);
}

GBL_EXPORT GblBool GblStringBuffer_empty(const GblStringBuffer* pSelf) {
    GblBool empty = GBL_FALSE;

    if(!pSelf)
        empty = GBL_TRUE;
    else if(GblArrayList_size(&pSelf->data) == 0)
        empty = GBL_TRUE;
    else if(GblStringView_equals(GblStringBuffer_view(pSelf), ""))
        empty = GBL_TRUE;

    return empty;
}



GBL_EXPORT GblBool GblStringBuffer_stack(const GblStringBuffer* pSelf) {
    return pSelf? GblArrayList_stack(&pSelf->data) : GBL_FALSE;
}

GBL_EXPORT char* GblStringBuffer_data(GblStringBuffer* pSelf) {
    char* pData = GBL_NULL;
    if(pSelf) {
        pData = (char*)GblArrayList_data(&pSelf->data);
    }
    return pData;
}

GBL_EXPORT const char* GblStringBuffer_cString(const GblStringBuffer* pSelf) {
    GBL_ASSERT(pSelf);
    static const char empty = '\0';
    const char* pStr = &empty;
    if(pSelf) {
        if(GBL_PRIV(pSelf->data).pData && GBL_PRIV(pSelf->data).size)
            pStr = (const char*) GBL_PRIV(pSelf->data).pData;
    }
    return pStr;
}

GBL_EXPORT GblStringView GblStringBuffer_view(const GblStringBuffer* pSelf) {
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

GBL_EXPORT GblQuark GblStringBuffer_quark(const GblStringBuffer* pSelf) {
    return GblQuark_fromString(GblStringBuffer_cString(pSelf));
}

GBL_EXPORT GBL_RESULT GblStringBuffer_destruct(GblStringBuffer* pSelf) {
    return GblArrayList_destruct(&pSelf->data);
}

GBL_EXPORT GBL_RESULT GblStringBuffer_clear(GblStringBuffer* pSelf) {
    return GblArrayList_clear(&pSelf->data);
}

GBL_EXPORT const char* GblStringBuffer_set(GblStringBuffer* pSelf, GblStringView view) {
    const char* pValue = NULL;
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    GBL_CTX_CALL(GblArrayList_assign(&pSelf->data, view.pData, view.length));
    pValue = (const char*)GblArrayList_data(&pSelf->data);
    GBL_CTX_END_BLOCK();
    return pValue;
}

GBL_EXPORT GBL_RESULT GblStringBuffer_construct_4(GblStringBuffer* pSelf,
                                                  GblStringView  view,
                                                  size_t         structSize,
                                                  GblContext*    pCtx)
{
    GBL_CTX_BEGIN(pCtx);
    GBL_CTX_VERIFY_ARG(structSize >= sizeof(GblStringBuffer));
    GBL_CTX_CALL(GblArrayList_construct(&pSelf->data, 1, view.length, view.pData, structSize, GBL_TRUE, pCtx));
    GBL_CTX_END();
}
GBL_EXPORT GBL_RESULT GblStringBuffer_construct_3(GblStringBuffer* pSelf,
                                                  GblStringView   view,
                                                  size_t         structSize)
{
    return GblStringBuffer_construct_4(pSelf, view, structSize, NULL);
}
GBL_EXPORT GBL_RESULT GblStringBuffer_construct_2(GblStringBuffer* pSelf,
                                                  GblStringView  view)
{
    return GblStringBuffer_construct_3(pSelf, view, sizeof(GblStringBuffer));
}
GBL_EXPORT GBL_RESULT GblStringBuffer_construct_1(GblStringBuffer* pSelf)
{
    return GblStringBuffer_construct_2(pSelf, GBL_STRV(GBL_NULL));
}

GBL_EXPORT GblStringBuffer* GblStringBuffer_createInPlace_4 (GblStringBuffer* pSelf,
                                                             GblStringView    view,
                                                             size_t           extraStackSize,
                                                             GblContext*      pCtx)
{
    GblStringBuffer_construct_4(pSelf, view, extraStackSize, pCtx);
    return pSelf;
}

GBL_EXPORT GblStringBuffer* GblStringBuffer_createInPlace_3 (GblStringBuffer* pSelf,
                                                             GblStringView    view,
                                                             size_t           extraStackSize)
{
    GblStringBuffer_construct_3(pSelf, view, extraStackSize);
    return pSelf;
}

GBL_EXPORT GblStringBuffer* GblStringBuffer_createInPlace_2 (GblStringBuffer* pSelf,
                                                             GblStringView   view)
{
    GblStringBuffer_construct_2(pSelf, view);
    return pSelf;
}

GBL_EXPORT GblStringBuffer* GblStringBuffer_createInPlace_1 (GblStringBuffer* pSelf)
{
    GblStringBuffer_construct_1(pSelf);
    return pSelf;
}


GBL_EXPORT GBL_RESULT GblStringBuffer_release(GblStringBuffer* pSelf, char** ppStrPtr, size_t * pCapacity) {
    size_t  size = 0;
    return GblArrayList_release(&pSelf->data, (void**)ppStrPtr, &size, pCapacity);
}

GBL_EXPORT GBL_RESULT GblStringBuffer_acquire(GblStringBuffer* pSelf, char* pData, size_t  capacity) {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    if(!pData && !capacity) GBL_CTX_DONE();
    if(!capacity && pData) capacity = strlen(pData);
    GBL_CTX_CALL(GblArrayList_acquire(&pSelf->data, pData, capacity, capacity));
    GBL_PRIV(pSelf->data).size = strlen((const char*)GBL_PRIV(pSelf->data).pData);
    GBL_CTX_END();
}

GBL_EXPORT GblContext* GblStringBuffer_context(const GblStringBuffer* pSelf) {
    return pSelf? GblArrayList_context(&pSelf->data) : NULL;
}

GBL_EXPORT size_t  GblStringBuffer_stackBytes(const GblStringBuffer* pSelf) {
    return pSelf? GblArrayList_stackBytes(&pSelf->data) : 0;
}

GBL_EXPORT size_t  GblStringBuffer_length(const GblStringBuffer* pSelf) {
    return pSelf? GblArrayList_size(&pSelf->data) : 0;
}

GBL_EXPORT size_t  GblStringBuffer_capacity(const GblStringBuffer* pSelf) {
    return pSelf? GblArrayList_capacity(&pSelf->data) : 0;
}

GBL_EXPORT GBL_RESULT GblStringBuffer_reserve(GblStringBuffer* pSelf, size_t  capacity) {
    return GblArrayList_reserve(&pSelf->data, capacity);
}

GBL_EXPORT GBL_RESULT GblStringBuffer_resize(GblStringBuffer* pSelf, size_t  size) {
    return GblArrayList_resize(&pSelf->data, size);
}

GBL_EXPORT GBL_RESULT GblStringBuffer_grow(GblStringBuffer* pSelf, size_t  delta) {
    return GblStringBuffer_resize(pSelf, GblStringBuffer_length(pSelf) + delta);
}

GBL_EXPORT GBL_RESULT GblStringBuffer_shrink(GblStringBuffer* pSelf, size_t  delta) {
    return GblStringBuffer_resize(pSelf, GblStringBuffer_length(pSelf) - delta);
}

GBL_EXPORT int GblStringBuffer_compare(const GblStringBuffer* pSelf, const GblStringBuffer* pOther) {
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

GBL_EXPORT GBL_RESULT GblStringBuffer_insert(GblStringBuffer* pSelf, size_t  index, GblStringView view) {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    GBL_CTX_VERIFY_POINTER(view.pData);
    GBL_CTX_VERIFY_EXPRESSION(view.length);
    GblArrayList_insert(&pSelf->data, index, view.length, view.pData);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblStringBuffer_appendPrintf(GblStringBuffer* pSelf, const char* pFmt, ...) {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    va_list varArgs;
    va_start(varArgs, pFmt);
    GBL_CTX_CALL(GblStringBuffer_appendVPrintf(pSelf, pFmt, varArgs));
    va_end(varArgs);
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblStringBuffer_appendNil(GblStringBuffer* pSelf) {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    GBL_CTX_CALL(GblStringBuffer_append(pSelf, GBL_STRV("nil")));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblStringBuffer_appendBool(GblStringBuffer* pSelf, GblBool value) {
    const char* pStr = NULL;
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    pStr = value ? "true" : "false";
    GBL_CTX_CALL(GblStringBuffer_append(pSelf, GBL_STRV(pStr)));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblStringBuffer_appendInt(GblStringBuffer* pSelf, int value) {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    GBL_CTX_CALL(GblStringBuffer_appendPrintf(pSelf, "%d", value));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblStringBuffer_appendUint(GblStringBuffer* pSelf, unsigned value) {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    GBL_CTX_CALL(GblStringBuffer_appendPrintf(pSelf, "%u", value));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblStringBuffer_appendFloat(GblStringBuffer* pSelf, float value) {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    GBL_CTX_CALL(GblStringBuffer_appendPrintf(pSelf, "%." GBL_STRINGIFY(GBL_STRING_FLOAT_DECIMAL_PLACES) "f", value));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblStringBuffer_appendDouble(GblStringBuffer* pSelf, double value) {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    GBL_CTX_CALL(GblStringBuffer_appendPrintf(pSelf, "%." GBL_STRINGIFY(GBL_STRING_FLOAT_DECIMAL_PLACES) "lf", value));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblStringBuffer_appendPointer(GblStringBuffer* pSelf, const void* pPtr) {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    GBL_CTX_CALL(GblStringBuffer_appendPrintf(pSelf, "0x%x", pPtr));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblStringBuffer_append(GblStringBuffer* pSelf, GblStringView view) {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    GBL_CTX_CALL(GblArrayList_append(&pSelf->data, view.pData, view.length));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblStringBuffer_prepend(GblStringBuffer* pSelf, GblStringView view) {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    GBL_CTX_CALL(GblArrayList_prepend(&pSelf->data, view.pData, view.length));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblStringBuffer_overwrite(GblStringBuffer* pSelf, size_t index, GblStringView view) {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    //GBL_CTX_VERIFY_ARG(index < GblStringBuffer_length(pSelf));
    const GblBool resized = (index + view.length > GblStringBuffer_length(pSelf));
    GBL_CTX_CALL(GblStringBuffer_reserve(pSelf, index + view.length));
    memcpy(GBL_PRIV(pSelf->data).pData+index, view.pData, view.length);
    if(resized) GBL_PRIV(pSelf->data).pData[index + view.length] = '\0';
    GBL_PRIV(pSelf->data).size = strlen((const char*)GBL_PRIV(pSelf->data).pData);
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblStringBuffer_erase(GblStringBuffer* pSelf, size_t index, size_t len) {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    GBL_CTX_CALL(GblArrayList_erase(&pSelf->data, index, len));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblStringBuffer_shrinkToFit(GblStringBuffer* pSelf) {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    GBL_CTX_CALL(GblArrayList_shrinkToFit(&pSelf->data));

    GBL_CTX_END();
}

GBL_EXPORT char GblStringBuffer_char(const GblStringBuffer* pSelf, size_t index) {
    char result = '\0';
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    const char* pChar = (const char*)GblArrayList_at(&pSelf->data, index);
    GBL_CTX_VERIFY_LAST_RECORD();
    if(pChar) result = *pChar;
    GBL_CTX_END_BLOCK();
    return result;
}

GBL_EXPORT GBL_RESULT GblStringBuffer_setChar(const GblStringBuffer* pSelf, size_t  index, char value) {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    GBL_CTX_VERIFY(index < GblStringBuffer_length(pSelf), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_PRIV(pSelf->data).pData[index] = value;
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblStringBuffer_chop(GblStringBuffer* pSelf) {
    return GblStringBuffer_shrink(pSelf, 1);
}

GBL_EXPORT GBL_RESULT GblStringBuffer_lower(GblStringBuffer* pSelf) {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    for(size_t  c = 0; c < GblStringBuffer_length(pSelf); ++c) {
        GblStringBuffer_setChar(pSelf, c, tolower(GblStringBuffer_char(pSelf, c)));
    }
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblStringBuffer_upper(GblStringBuffer* pSelf) {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    for(size_t  c = 0; c < GblStringBuffer_length(pSelf); ++c) {
        GblStringBuffer_setChar(pSelf, c, toupper(GblStringBuffer_char(pSelf, c)));
    }
    GBL_CTX_END();
}

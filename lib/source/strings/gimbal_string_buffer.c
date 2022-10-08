#include <gimbal/strings/gimbal_string_buffer.h>

GBL_EXPORT GBL_RESULT GblStringBuffer_appendVPrintf(GblStringBuffer* pSelf, const char* pFmt, va_list varArgs) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    GblSize newCapacity = 0;
    size_t expectedSize = 0;
    GblSize originalSize = 0;
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
        GBL_PRIV(pSelf->data).size = expectedSize;
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

GBL_EXPORT GBL_RESULT GblStringBuffer_remove(GblStringBuffer* pSelf, GblStringView view) GBL_NOEXCEPT {
    const char* pNeedle = NULL;
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    GBL_CTX_VERIFY_POINTER(view.pData);
    pNeedle = GBL_STRING_VIEW_CSTR(view);
    while(1) {
        const char* pLoc = strstr(GblStringBuffer_cString(pSelf), pNeedle);
        if(pLoc) {
            GblSize offset = pLoc - GblStringBuffer_cString(pSelf);
            GBL_CTX_CALL(GblStringBuffer_erase(pSelf, offset, view.length));
        } else break;
    }
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblStringBuffer_replace(GblStringBuffer* pSelf, GblStringView substr, GblStringView replacement, GblSize limit) GBL_NOEXCEPT {
    const char* pNeedle = NULL;
    GblSize iterations = 0;
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    GBL_CTX_VERIFY_POINTER(substr.pData);
    GBL_CTX_VERIFY_POINTER(replacement.pData);
    GBL_CTX_VERIFY_ARG(limit);
    pNeedle = GBL_STRING_VIEW_CSTR(substr);
    while(iterations < limit) {
        const char* pLoc = strstr(GblStringBuffer_cString(pSelf), pNeedle);
        if(pLoc) {
          GblSize offset = pLoc - GblStringBuffer_cString(pSelf);
          GBL_CTX_CALL(GblStringBuffer_erase(pSelf, offset, substr.length));
          GBL_CTX_CALL(GblStringBuffer_insert(pSelf, offset, replacement));
          ++iterations;
        } else break;
    }
    GBL_CTX_END();
}


GBL_EXPORT GBL_RESULT GblStringBuffer_chomp(GblStringBuffer* pSelf) GBL_NOEXCEPT {
    const GblSize len = GblStringBuffer_length(pSelf);
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

GBL_EXPORT GBL_RESULT GblStringBuffer_padLeft(GblStringBuffer* pSelf, char value, GblSize count) GBL_NOEXCEPT {
    const GblStringView view = {
        .pData          = &value,
        .nullTerminated = 0,
        .length         = 1
    };
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    for(GblSize i = 0; i < count; ++i)
        GBL_CTX_VERIFY_CALL(GblStringBuffer_prepend(pSelf, view));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblStringBuffer_padRight(GblStringBuffer* pSelf, char value, GblSize count) GBL_NOEXCEPT {
    const GblStringView view = {
        .pData          = &value,
        .nullTerminated = 0,
        .length         = 1
    };
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    for(GblSize i = 0; i < count; ++i)
        GBL_CTX_VERIFY_CALL(GblStringBuffer_append(pSelf, view));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblStringBuffer_trimStart(GblStringBuffer* pSelf, char value) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    GblSize count = 0;

    while(GblStringBuffer_length(pSelf) > count &&
          GblStringBuffer_char(pSelf, count) == value) {
        ++count;
    }

    if(count) {
        GBL_CTX_VERIFY_CALL(GblStringBuffer_erase(pSelf, 0, count));
    }
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblStringBuffer_trimEnd(GblStringBuffer* pSelf, char value) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_PRIV(pSelf->data).pCtx);
    GblSize count = 0;

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


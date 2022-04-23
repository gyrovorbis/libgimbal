#ifndef GIMBAL_STRING_H
#define GIMBAL_STRING_H

#include "../containers/gimbal_vector.h"
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GblStringView { //contiguous sequence of chars, literally only has pointer and size
    const char* pBuffer;
    GblSize     size; // MAX == null terminated!
} GblStringView;

/* C++ string view shit:
 *   copy, substr, compare, startsWith, endsWith, find, rFind, find_first_of, find_last_of
 *   find_first_not_of, find_last_not_of, Sequential/Vector iterable char array container!
 */

// GBL API BEGIN WITH NULL CONTEXT CAN STILL DO SHIT?

typedef struct GblString {
    GblVector data; //continue dereestification
} GblString;

       // GBL_MAP_TUPLES(GBL_ENUM_TABLE_RETURN_STRING, GBL_MAP_TUPLES(GBL_EVAL, GBL_META_ENUM_TUPLE_VALUE_ARRAY table)) \


GBL_INLINE GBL_API gblStringIsEmpty(const GblString* pStr, GblBool* pResult) {
    return gblVectorIsEmpty(&pStr->data, pResult);
}

GBL_INLINE GBL_API gblStringIsStack(const GblString* pStr, GblBool* pResult) {
    return gblVectorIsStack(&pStr->data, pResult);
}

GBL_INLINE GBL_API gblStringCStr(const GblString* pStr, const char** ppStr) {
    GBL_ASSERT(pStr);
    GBL_ASSERT(ppStr);
    *ppStr = (const char*)pStr->data.pBuffer;
    return GBL_RESULT_SUCCESS;
}

GBL_INLINE GBL_API gblStringDestruct(GblString* pStr) {
    return gblVectorDestruct(&pStr->data);
}

GBL_INLINE GBL_API gblStringClear(GblString* pStr) {
    return gblVectorClear(&pStr->data);
}

GBL_INLINE GBL_API gblStringAssign(GblString* pStr, const GblStringView* pStrView) {
    GBL_API_BEGIN(pStr->data.pCtx);
    const void* pData = NULL;
    GblSize newSize = 0;
    if(pStrView) {
        if(pStrView->pBuffer) {
            pData = pStrView->pBuffer;
            newSize = pStrView->size? pStrView->size : strlen(pStrView->pBuffer);
        }
    }
    GBL_API_CALL(gblVectorAssign(&pStr->data, pData, newSize));
    GBL_API_END();
}

GBL_INLINE GBL_API gblStringConstruct(GblString* pString, GblSize size, GblContext* pCtx, const GblStringView* pView) {
    GblStringView defaultView = { GBL_NULL, 0 };
    //const GblSize extraSize = size - sizeof(GblString);
    GBL_API_BEGIN(pCtx);
    if(!pView) {
        pView = &defaultView;
    } else if(!pView->size && pView->pBuffer) {
        defaultView.pBuffer = pView->pBuffer;
        defaultView.size = strlen(pView->pBuffer);
        pView = &defaultView;
    }
    GBL_API_CALL(gblVectorConstruct_6(&pString->data, pCtx, 1, size, pView->pBuffer, pView->size));
    GBL_API_END();
}

GBL_INLINE GBL_API gblStringTake(GblString* pStr, char** ppStrPtr, GblSize* pCapacity) {
    GblSize size = 0;
    return gblVectorTake(&pStr->data, (void**)ppStrPtr, &size, pCapacity);
}

GBL_INLINE GBL_API gblStringGive(GblString* pStr, char* pData, GblSize capacity) {
    GBL_API_BEGIN(pStr->data.pCtx);
    if(!capacity) capacity = strlen(pData);
    GBL_API_CALL(gblVectorGive(&pStr->data, pData, capacity, capacity));
    pStr->data.size = strlen((const char*)pStr->data.pBuffer);
    GBL_API_END();
}

GBL_INLINE GBL_API gblStringContext(const GblString* pStr, GblContext** pCtx) {
    return gblVectorContext(&pStr->data, pCtx);
}

GBL_INLINE GBL_API gblStringStackBytes(const GblString* pStr, GblSize* pSize) {
    return gblVectorStackBytes(&pStr->data, pSize);
}

GBL_INLINE GBL_API gblStringLength(const GblString* pStr, GblSize* pLength) {
    return gblVectorSize(&pStr->data, pLength);
}

GBL_INLINE GBL_API gblStringCapacity(const GblString* pStr, GblSize* pCapacity) {
    return gblVectorCapacity(&pStr->data, pCapacity);
}

GBL_INLINE GBL_API gblStringReserve(GblString* pStr, GblSize capacity) {
    return gblVectorReserve(&pStr->data, capacity);
}

GBL_INLINE GBL_API gblStringResize(GblString* pStr, GblSize size) {
    return gblVectorResize(&pStr->data, size);
}

GBL_INLINE GBL_API gblStringCompare(const GblString* pStr1, const GblString* pStr2, GblInt* pResult) {
    GBL_ASSERT(pStr1);
    GBL_ASSERT(pStr2);
    GBL_ASSERT(pResult);
    if(!pStr1 || ! pStr2 || !pResult)
        return GBL_RESULT_ERROR_INVALID_POINTER;

    if(pStr1 == pStr2) GBL_UNLIKELY {
        *pResult = 0;
    } else if(!pStr1->data.pBuffer) {
        *pResult = INT_MIN;
    } else if(!pStr2->data.pBuffer) {
        *pResult = INT_MAX;
    } else GBL_LIKELY { //both valid
        *pResult = strcmp(pStr1->data.pBuffer, pStr2->data.pBuffer);
    }

    return GBL_RESULT_SUCCESS;
}

GBL_INLINE GBL_API gblStringInsert(GblString* pStr, GblSize index, const GblStringView* pView) {
    GblSize count = 0;
    GBL_API_BEGIN(pStr->data.pCtx);
    GBL_API_VERIFY_POINTER(pView);
    if(pView->size) {
        count = pView->size;
    } else if(pView->pBuffer) {
        count = strlen(pView->pBuffer);
    }
    GBL_API_VERIFY_EXPRESSION(count != 0);
    GBL_API_CALL(gblVectorInsert(&pStr->data, index, count, pView->pBuffer, NULL));
    GBL_API_END();
}

GBL_INLINE GBL_API gblStringVaSprintf(GblString* pStr, const char* pFmt, va_list varArgs) {
    GBL_API_BEGIN(pStr->data.pCtx);
    GblSize newCapacity = 0;
    size_t expectedSize = 0;
    va_list varArgsCopy;

    GBL_API_VERIFY_POINTER(pFmt);

    //Check if shit's empty
    expectedSize = strlen(pFmt) + 1;
    if(expectedSize <= 1) GBL_UNLIKELY {
        pStr->data.pBuffer[0] = '\0';
        pStr->data.size = 0;
    } else GBL_LIKELY {
        //Make a first guess at the required size
        newCapacity = GBL_VECTOR_CAPACITY_FROM_SIZE_(expectedSize);
        GBL_API_CALL(gblStringReserve(pStr, newCapacity));
        GBL_API_ERRNO_CLEAR();
        va_copy(varArgsCopy, varArgs);
        expectedSize = vsnprintf(pStr->data.pBuffer, pStr->data.capacity, pFmt, varArgsCopy);
        pStr->data.size = expectedSize;
        va_end(varArgsCopy);
        GBL_API_PERROR("vsnprintf failed with code: %zu", expectedSize);
        //Multi-pass, try again with real size!
        if(expectedSize >= pStr->data.capacity) GBL_UNLIKELY {
            newCapacity = GBL_VECTOR_CAPACITY_FROM_SIZE_(expectedSize + 1);
            GBL_API_CALL(gblStringReserve(pStr, newCapacity));
            GBL_API_ERRNO_CLEAR();
            expectedSize = vsnprintf(pStr->data.pBuffer, pStr->data.capacity, pFmt, varArgs);
            pStr->data.size = expectedSize;
            GBL_API_PERROR("vsnprintf failed with code: %zu", expectedSize);
            GBL_API_VERIFY(expectedSize < pStr->data.capacity, GBL_RESULT_TRUNCATED); //better have written it all
        }
    }
    GBL_API_END();
}

GBL_INLINE GBL_API gblStringSprintf(GblString* pStr, const char* pFmt, ...) {
    GBL_API_BEGIN(pStr->data.pCtx);
    va_list varArgs;
    va_start(varArgs, pFmt);
    GBL_API_CALL(gblStringVaSprintf(pStr, pFmt, varArgs));
    va_end(varArgs);
    GBL_API_END();
}

GBL_INLINE GBL_API gblStringToi(const GblString* pStr, GblInt* pValue) {
    const char* pCStr = NULL;
    GBL_API_BEGIN(pStr->data.pCtx);
    GBL_API_VERIFY_POINTER(pValue);
    GBL_API_CALL(gblStringCStr(pStr, &pCStr));
    *pValue = atoi(pCStr);
    GBL_API_END();
}

GBL_INLINE GBL_API gblStringTof(const GblString* pStr, GblFloat* pValue) {
    const char* pCStr = NULL;
    GBL_API_BEGIN(pStr->data.pCtx);
    GBL_API_VERIFY_POINTER(pValue);
    GBL_API_CALL(gblStringCStr(pStr, &pCStr));
    *pValue = atof(pCStr);
    GBL_API_END();
}

GBL_INLINE GBL_API gblStringTod(const GblString* pStr, double* pValue) {
    const char* pCStr = NULL;
    GBL_API_BEGIN(pStr->data.pCtx);
    GBL_API_VERIFY_POINTER(pValue);
    GBL_API_CALL(gblStringCStr(pStr, &pCStr));
    *pValue = atof(pCStr);
    GBL_API_END();
}

GBL_INLINE GBL_API gblStringTob(const GblString* pStr, GblBool* pValue) {
    const char* pCStr = NULL;
    *pValue = GBL_FALSE;
    GBL_API_BEGIN(pStr->data.pCtx);
    GBL_API_VERIFY_POINTER(pValue);
    GBL_API_CALL(gblStringCStr(pStr, &pCStr));
    if(strncmp(pCStr, "true", sizeof("true")) == 0) {
        *pValue = GBL_TRUE;
    } else {
        GblInt integer = 0;
        GBL_API_CALL(gblStringToi(pStr, &integer));
        *pValue = (integer != 0)? GBL_TRUE : GBL_FALSE;
    }
    GBL_API_END();
}

GBL_INLINE GBL_API gblStringToNil(const GblString* pStr, GblBool* pValue) {
    const char* pCStr = NULL;
    GblSize length = 0;
    *pValue = GBL_FALSE;
    GBL_API_BEGIN(pStr->data.pCtx);
    GBL_API_VERIFY_POINTER(pValue);
    GBL_API_CALL(gblStringCStr(pStr, &pCStr));
    GBL_API_CALL(gblStringLength(pStr, &length));
    if(!length || strncmp(pCStr, "nil", sizeof("nil")) == 0) {
        *pValue = GBL_TRUE;
    }
    GBL_API_END();
}

GBL_INLINE GBL_API gblStringFromNil(GblString* pStr) {
    GblStringView view;
    GBL_API_BEGIN(pStr->data.pCtx);
    view.pBuffer = "nil"; view.size = sizeof("nil");
    GBL_API_CALL(gblStringAssign(pStr, &view));
    GBL_API_END();
}

GBL_INLINE GBL_API gblStringFromb(GblString* pStr, GblBool value) {
    GblStringView view;
    GBL_API_BEGIN(pStr->data.pCtx);
    if(value) {
        view.pBuffer = "true"; view.size = sizeof("true");
    } else {
        view.pBuffer = "false"; view.size = sizeof("false");
    }
    GBL_API_CALL(gblStringAssign(pStr, &view));
    GBL_API_END();
}

GBL_INLINE GBL_API gblStringFromi(GblString* pStr, GblInt value) {
    GBL_API_BEGIN(pStr->data.pCtx);
    GBL_API_CALL(gblStringSprintf(pStr, "%d", value));
    GBL_API_END();
}

GBL_INLINE GBL_API gblStringFromu(GblString* pStr, GblUint value) {
    GBL_API_BEGIN(pStr->data.pCtx);
    GBL_API_CALL(gblStringSprintf(pStr, "%u", value));
    GBL_API_END();
}

GBL_INLINE GBL_API gblStringFromf(GblString* pStr, GblFloat value) {
    GBL_API_BEGIN(pStr->data.pCtx);
    GBL_API_CALL(gblStringSprintf(pStr, "%." GBL_STRINGIFY(GBL_STRING_FLOAT_DECIMAL_PLACES) "f", value));
    GBL_API_END();
}

GBL_INLINE GBL_API gblStringFromd(GblString* pStr, double value) {
    GBL_API_BEGIN(pStr->data.pCtx);
    GBL_API_CALL(gblStringSprintf(pStr, "%." GBL_STRINGIFY(GBL_STRING_FLOAT_DECIMAL_PLACES) "lf", value));
    GBL_API_END();
}

GBL_INLINE GBL_API gblStringFromp(GblString* pStr, const void* pPtr) {
    GBL_API_BEGIN(pStr->data.pCtx);
    GBL_API_CALL(gblStringSprintf(pStr, "0x%x", pPtr));
    GBL_API_END();
}

GBL_INLINE GBL_API gblStringConcat(GblString* pStr, const char* pOther) {
    GBL_API_BEGIN(NULL);
    GBL_API_CALL(gblVectorConcat(&pStr->data, pOther, strlen(pOther)));
    GBL_API_END();
}

/*
#if 0

GBL_API_INLINE_HELPER((GblSize, 0), STRING_LENGTH, (const GblString* pString), {
    GBL_API_CALL(gblStringLength(pString, &GBL_API_INLINE_RETVAL()));
});

GBL_API_INLINE_HELPER((GblSize, 0), STRING_CAPACITY, (const GblString* pString), {
    GBL_API_CALL(gblStringCapacity(pString, &GBL_API_INLINE_RETVAL()));
});

GBL_API_INLINE_HELPER((GblSize, 0), STRING_STACK_BYTES, (const GblString* pString), {
    GBL_API_CALL(gblStringStackBytes(pString, &GBL_API_INLINE_RETVAL()));
});

GBL_API_INLINE_HELPER((const char*, NULL), STRING_BUFFER, (const GblString* pString), {
    GBL_API_CALL(gblStringCStr(pString, &GBL_API_INLINE_RETVAL()));
});

GBL_API_INLINE_HELPER((GblContext, GBL_HANDLE_INVALID), STRING_CONTEXT, (const GblString* pString), {
    GBL_API_CALL(gblStringContext(pString, &GBL_API_INLINE_RETVAL()));
});

GBL_API_INLINE_HELPER((GblBool, GBL_TRUE), STRING_IS_STACK, (const GblString* pString), {
    GBL_API_CALL(gblStringIsStack(pString, &GBL_API_INLINE_RETVAL()));
});

GBL_API_INLINE_HELPER((GblBool, GBL_TRUE), STRING_IS_EMPTY, (const GblString* pString), {
    GBL_API_CALL(gblStringIsEmpty(pString, &GBL_API_INLINE_RETVAL()));
});

#endif
*/


/*
 * FIND, TOKENIZE, REPLACE, TO UPPER, TO LOWER, LOCATE LAST TOKEN FOR SUFFIX/EXTENSION SHIT
 *
GBL_API GblStringSubString(const GblString* pStr, GblStringView* pView);

// GblString ONLY goes to/from string view?
// String View is the adapter to fucking everything else?
*/



#ifdef __cplusplus
}
#endif

#endif // GblBAL_STRING_H

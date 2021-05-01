#ifndef GIMBAL_STRING_H
#define GIMBAL_STRING_H

#include "gimbal_api.h"
#include "gimbal_types.h"
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GblStringView { //contiguous sequence of chars, literally only has pointer and size
    const char* pBuffer;
    GblSize     size; // MAX == null terminated!
} GblStringView;

#if 0
GBL_API GblStringView(GblStringView* pView);
GblStringView gblStringViewFromBuffer(const char* pBuffer, GblSize count) {
    assert(count > 0 || !pBuffer);
    const GblStringView value = {
        pBuffer,
        count
    };
    return value;
}
GblStringView gblStringViewFromCString(const char* pCString) {
    assert(pBuffer);
    return GblStringViewFromBuffer(pCString, strlen(pCString));
}
GBL_API GblStringViewFromStringView(GblStringView* pDest, GblStringView* pSource, GblSize count);
GBL_API GblStringViewFromGblString(GblStringView* pDest, GblString* pString, GblSize count);

GBL_API GblStringViewIsValid(GblStringView* pView);
GBL_API GblStringViewSize(GblStringView* pView, GblSize* pCount);
GBL_API GblStringViewBuffer(GblStringView* pView, const char** ppBuff); // NOT NULL TERMINATED!!!
#endif

/* C++ string view shit:
 *   copy, substr, compare, startsWith, endsWith, find, rFind, find_first_of, find_last_of
 *   find_first_not_of, find_last_not_of, Sequential/Vector iterable char array container!
 */

// GBL API BEGIN WITH NULL CONTEXT CAN STILL DO SHIT?

typedef struct GblStringData {
    char*       pBuffer;
    GblSize     length;
    GblSize     capacity;
} GblStringData;

typedef struct GblString {
    GblContext      hCtx;
    GblStringData   data;
    GblSize         stackSize;
    char            stackBuffer[GBL_STRING_BUFFER_BASE_STACK_SIZE];
} GblString;

#if 0
//macro overload me?
GBL_API gblStringConstruct(GblString* pString, GblSize size, GblContext hCtx, const GblStringView* pView);
GBL_API gblStringDestruct(GblString* pStr);

GBL_API gblStringAssign(GblString* pStr, const GblStringView* pStrView);
GBL_API gblStringTake(GblString* pStr, char** ppStrPtr, GblSize* pCapacity);
GBL_API gblStringGive(GblString* pStr, char* pData, GblSize capacity);
GBL_API gblStringCStr(const GblString* pStr, const char** ppStr);

GBL_API gblStringContext(const GblString* pStr, GblContext* pCtx);
GBL_API gblStringStackSize(const GblString* pStr, GblSize* pSize);
GBL_API gblStringLength(const GblString* pStr, GblSize* pLength);
GBL_API gblStringCapacity(const GblString* pStr, GblSize* pCapacity);

GBL_API gblStringClear(GblString* pStr);
GBL_API gblStringReserve(GblString* pStr, GblSize capacity);
GBL_API gblStringResize(GblString* pStr, GblSize length);

GBL_API gblStringIsNull(const GblString* pStr, GblBool* pResult);
GBL_API gblStringIsEmpty(const GblString* pStr, GblBool* pResult);
GBL_API gblStringIsStack(const GblString* pStr, GblBool* pResult);

GBL_API gblStringCompare(const GblString* pStr1, const GblString* pStr2, GblBool* pResult);
GBL_API gblStringCat(const GblString* pStr, const GblStringView* pView); //+=

GBL_API gblStringSnprintf(const GblString* pStr, const char* pFmt, ...);
GBL_API gblStringVaSnprintf(const GblString* pStr, const char* pFmt, va_list varArgs);
#endif

//IMPLEMENT ME FOR CUSTOM ALLOCATION SCHEMES
GBL_INLINE GblSize GBL_STRING_CAPACITY_FROM_SIZE_(GblSize size) { return size; }

#if 0
GBL_INLINE GBL_API gblStringIsNull(const GblString* pStr, GblBool* pResult) {

    GBL_ASSERT(pStr);
    GBL_ASSERT(pResult);
    //!pBuffer should be sufficient, if length is zero, better not have a buffer...
    *pResult = (!pStr->data.pBuffer && !pStr->data.length);
    return GBL_RESULT_SUCCESS;
}
#endif

GBL_INLINE GBL_API gblStringIsEmpty(const GblString* pStr, GblBool* pResult) {
    GBL_ASSERT(pStr);
    GBL_ASSERT(pResult);
    *pResult = (!pStr->data.length);
    return GBL_RESULT_SUCCESS;
}

GBL_INLINE GBL_API gblStringIsStack(const GblString* pStr, GblBool* pResult) {
    GBL_ASSERT(pStr);
    GBL_ASSERT(pResult);
    *pResult = (pStr->data.pBuffer == pStr->stackBuffer);
    return GBL_RESULT_SUCCESS;
}

GBL_INLINE GBL_API gblStringCStr(const GblString* pStr, const char** ppStr) {
    GBL_ASSERT(pStr);
    GBL_ASSERT(ppStr);
    *ppStr = pStr->data.pBuffer;
    return GBL_RESULT_SUCCESS;
}

GBL_INLINE void gblStringInitialize_(GblString* pString) {
    pString->data.capacity = pString->stackSize;
    pString->data.pBuffer = pString->stackBuffer;
    pString->data.length = 0;
    pString->stackBuffer[0] = '\0';
}

GBL_INLINE GBL_API gblStringDestruct(GblString* pStr) {
    GBL_API_BEGIN(pStr->hCtx);
    // Check if we have a buffer to free
    if(pStr->data.pBuffer && pStr->data.pBuffer != pStr->stackBuffer) {
        GBL_API_FREE(pStr->data.pBuffer);
    }
    GBL_API_END();
}


GBL_INLINE GBL_API gblStringClear(GblString* pStr) {
    GBL_API_BEGIN(pStr->hCtx);
    GBL_API_CALL(gblStringDestruct(pStr));
    gblStringInitialize_(pStr);
    GBL_API_END();
}

GBL_INLINE GBL_API gblStringAlloc_(GblString* pStr, GblSize capacity) {
    GBL_API_BEGIN(pStr->hCtx);

    if(capacity <= pStr->stackSize) {
        pStr->data.pBuffer = pStr->stackBuffer;
        pStr->data.capacity = pStr->stackSize;
    } else {
        pStr->data.capacity = capacity;
        pStr->data.pBuffer = (char*)GBL_API_MALLOC(pStr->data.capacity);
    }
    pStr->data.pBuffer[0] = '\0';
    pStr->data.length = 0;

    GBL_API_END();
}


GBL_INLINE GBL_API gblStringAssign(GblString* pStr, const GblStringView* pStrView) {
    GBL_API_BEGIN(pStr->hCtx);
    const GblSize newSize = pStrView ? pStrView->size : 0;

    if(pStr->data.capacity < newSize) {
        GBL_API_CALL(gblStringClear(pStr));
        if(newSize) GBL_API_CALL(gblStringAlloc_(pStr, GBL_STRING_CAPACITY_FROM_SIZE_(newSize + 1)));
    }

    if(pStrView && pStrView->pBuffer) memcpy(pStr->data.pBuffer, pStrView->pBuffer, pStrView->size);
    pStr->data.length = newSize;
    pStr->data.pBuffer[newSize] = '\0';
    GBL_API_END();
}

GBL_INLINE GBL_API gblStringConstruct(GblString* pString, GblSize size, GblContext hCtx, const GblStringView* pView) {
    const GblStringView defaultView { GBL_NULL, 0 };
    const GblSize extraSize = size - sizeof(GblString);
    GBL_API_BEGIN(hCtx);

    GBL_API_VERIFY_POINTER(pView);
    GBL_API_VERIFY_ARG(size >= sizeof(GblString));

    pString->hCtx       = hCtx;
    pString->stackSize  = sizeof(pString->stackBuffer) + extraSize;
    gblStringInitialize_(pString);

    if(!pView) pView = &defaultView;

    GBL_API_CALL(gblStringAssign(pString, pView));
    GBL_API_END();
}

GBL_INLINE GBL_API gblStringTake(GblString* pStr, char** ppStrPtr, GblSize* pCapacity) {
    GBL_API_BEGIN(pStr->hCtx);
    GblBool stack = GBL_FALSE;
    GBL_API_VERIFY_POINTER(ppStrPtr);
    GBL_API_VERIFY_POINTER(pCapacity);
    GBL_API_CALL(gblStringIsStack(pStr, &stack));
    GBL_API_VERIFY(!stack, GBL_RESULT_ERROR_INVALID_OPERATION, "Cannot Take Stack Buffer!");
    *ppStrPtr = pStr->data.pBuffer;
    *pCapacity = pStr->data.capacity;
    gblStringInitialize_(pStr);
    GBL_API_END();
}

GBL_INLINE GBL_API gblStringGive(GblString* pStr, char* pData, GblSize capacity) {
    GBL_API_BEGIN(pStr->hCtx);
    GBL_API_VERIFY_POINTER(pData);
    GBL_API_VERIFY_ARG(capacity);
    gblStringDestruct(pStr);
    pStr->data.pBuffer = pData;
    pStr->data.capacity = capacity;
    pStr->data.length = strnlen(pData, pStr->data.capacity);
    GBL_API_END();
}

GBL_INLINE GBL_API gblStringContext(const GblString* pStr, GblContext* pCtx) {
    GBL_API_BEGIN(pStr->hCtx);
    GBL_API_VERIFY_POINTER(pCtx);
    *pCtx = pStr->hCtx;
    GBL_API_END();
}

GBL_INLINE GBL_API gblStringStackSize(const GblString* pStr, GblSize* pSize) {
    GBL_API_BEGIN(pStr->hCtx);
    GBL_API_VERIFY_POINTER(pSize);
    *pSize = pStr->stackSize;
    GBL_API_END();
}

GBL_INLINE GBL_API gblStringLength(const GblString* pStr, GblSize* pLength) {
    GBL_API_BEGIN(pStr->hCtx);
    GBL_API_VERIFY_POINTER(pLength);
    *pLength = pStr->data.length;
    GBL_API_END();
}

GBL_INLINE GBL_API gblStringCapacity(const GblString* pStr, GblSize* pCapacity) {
    GBL_API_BEGIN(pStr->hCtx);
    GBL_API_VERIFY_POINTER(pCapacity);
    *pCapacity = pStr->data.capacity;
    GBL_API_END();
}

GBL_INLINE GBL_API gblStringReserve(GblString* pStr, GblSize capacity) {
    GBL_API_BEGIN(pStr->hCtx);
    if(pStr->data.capacity < capacity) {
        GblBool stack = GBL_FALSE;
        GBL_API_CALL(gblStringIsStack(pStr, &stack));
        if(stack) {
            GblSize oldLength = pStr->data.length;
            GBL_API_CALL(gblStringAlloc_(pStr, capacity));
            memcpy(pStr->data.pBuffer, pStr->stackBuffer, oldLength+1);
        } else {
            pStr->data.pBuffer = (char*)GBL_API_REALLOC(pStr->data.pBuffer, capacity);
            pStr->data.capacity = capacity;
        }
    }
    GBL_API_END();
}


GBL_INLINE GBL_API gblStringResize(GblString* pStr, GblSize length) {
    GBL_API_BEGIN(pStr->hCtx);
    if(pStr->data.length > length) {
        pStr->data.pBuffer[length] = '\0';

    } else if(pStr->data.length > length) {

        if(length > pStr->data.capacity) {
            gblStringReserve(pStr, GBL_STRING_CAPACITY_FROM_SIZE_(length));
        }
        // Do we fill this shit in with something?
    }

    pStr->data.length = length;
    GBL_API_END();
}


GBL_INLINE GBL_API GblStringCompare(const GblString* pStr1, const GblString* pStr2, GblBool* pResult) {
    GBL_ASSERT(pStr1);
    GBL_ASSERT(pStr2);
    GBL_ASSERT(pResult);
    if(!pStr1 || ! pStr2 || !pResult)
        return GBL_RESULT_ERROR_INVALID_POINTER;

    if(pStr1 == pStr2) GBL_UNLIKELY {
        *pResult = GBL_TRUE;
    } else if(!pStr1->data.pBuffer) {
        *pResult = GBL_FALSE;
    } else if(!pStr1->data.pBuffer) {
        *pResult = GBL_FALSE;
    } else GBL_LIKELY { //both valid
        *pResult = (strcmp(pStr1->data.pBuffer, pStr2->data.pBuffer) == 0);
    }

    return GBL_RESULT_SUCCESS;
}

GBL_API gblStringCat(const GblString* pStr, const GblStringView* pView);
GBL_API gblStringSnprintf(const GblString* pStr, const char* pFmt, ...);
GBL_API gblStringVaSnprintf(const GblString* pStr, const char* pFmt, va_list varArgs);

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

#ifndef GblBAL_STRING_H
#define GblBAL_STRING_H

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


/* C++ string view shit:
 *   copy, substr, compare, startsWith, endsWith, find, rFind, find_first_of, find_last_of
 *   find_first_not_of, find_last_not_of, Sequential/Vector iterable char array container!
 */

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

//macro overload me?
GBL_API gblStringConstruct(GblString* pString, GblSize size, GblContext hCtx, const GblStringView* pView);
GBL_API gblStringDestruct(GblString* pStr);

GBL_API gblStringSet(GblString* pStr, const GblStringView* pStrView)
GBL_API gblStringCStr(const GblString* pStr, const char** ppStr);

GBL_API gblStringContext(const GblString* pStr, GblContext* pCtx);
GBL_API gblStringStackSize(const GblString* pStr, GblSize* pSize);
GBL_API gblStringLength(const GblString* pStr, GblSize* pLength);
GBL_API gblStringCapacity(const GblString* pStr, GblSize* pCapacity);

GBL_API GblStringReserve(const GblString* pStr, GblSize capacity);
GBL_API gblStringResize(const GblString* pStr, GblSize length);

GBL_API gblStringIsNull(const GblString* pStr, GblBool* pResult);
GBL_API gblStringIsEmpty(const GblString* pStr, GblBool* pResult);
GBL_API gblStringIsValid(const GblString* pStr, GblBool* pResult);
GBL_API gblStringIsStack(const GblString* pStr);

GBL_API GblStringCat(const GblString* pStr, const GblStringView* pView); //+=

GBL_API GblStringSnprintf(const GblString* pStr, const char* pFmt, ...);
GBL_API GblStringVaSnprintf(const GblString* pStr, const char* pFmt, va_list varArgs);

GBL_API gblStringConstruct(GblString* pString, GblSize size, GblContext hCtx, const GblStringView* pView) {
    GBL_API_BEGIN(hCtx);
    GBL_API_VERIFY_POINTER(pView);
    const GblSize extraSize = size - sizeof(GblString);
    GBL_API_VERIFY(extraSize >= 0);

    memset(pString, 0, sizeof(GblString));
    pString->hCtx = hCtx;
    pString->stackSize = sizeof(pString->stackBuffer) + extraSize;

    const GblStringView defaultView = gblStringViewFromCString("");

    if(!pView) { //default initialize
        pView = &defaultView;
    }

    GBL_API_CALL(gblStringSet(pString, pView));

    GBL_API_END();
}

GBL_API gblStringInitialize_(GblString* pString) {
    pString->data.capacity = sizeof(pString->stackSize);
    pString->data.pBuffer = pString->stackBuffer;
    pString->data.length = 0;
    pString->stackBuffer[0] = '\0';
}

GBL_API gblStringDestruct(GblString* pStr) {
    GBL_API_BEGIN(pStr->hCtx);
    // Check if we have a bufffer to free
    if(pStr->data.pBuffer && pStr->data.pBuffer != pStr->stackBuffer) {
        GBL_CALL(GBL_API_FREE(pStr->data.pBuffer));
    )
    GBL_API_END();
}

GBL_API gblStringClear(GblString* pStr) {
    GBL_API_BEGIN(pStr->hCtx);
    GBL_CALL(gblStringDestruct(pStr));
    GBL_CALL(gblStringInitialize_(pStr));
    GBL_API_END();
}

static inline GblSize GBL_STRING_CAPACITY_FROM_SIZE(GblSize size) { return size; }

GBL_API gblStringAlloc_(const GblString* pStr, GblSize capacity) {

    GBL_API_BEGIN(pStr->hCtx);
    if(capacity <= pStr->stackSize) {
        pStr->data.pBuffer = pStr->stackBuffer;
        pStr->data.capacity = pStr->stackSize;
    } else {
        pStr->data.capacity = capacity;
        pStr->data.pPtr = (char*)GBL_API_MALLOC(pStr->data.capacity);
    }
    pStr->data.pBuffer[0] = '\0';
    pStr->data.length = 0;

    GBL_API_END();
}

GBL_API gblStringSet(GblString* pStr, const GblStringView* pStrView) {
    GBL_API_BEGIN(pStr->hCtx);

    if(pStr->data.capacity < pStrView->size) {
        GBL_CALL(gblStringClear(pStr));
        GBL_CALL(gblStringAlloc_(pStr, GBL_STRING_CAPACITY_FROM_SIZE(pStringView->size + 1)));
    }

    memcpy(pStr.data.pPtr, pStrView->pBuffer, pStringView->size);
    pStr.data.pPtr[pStringView->size] = '\0';
    GBL_API_END();
}

GBL_API GblStringReserve(const GblString* pStr, GblSize capacity) {
    GBL_API_BEGIN(pStr->hCtx);
    if(pStr->data.capacity < capacity) {
        if(gblStringIsStack(pString)) {
            GblSize oldLength = pStr->data.length;
            GBL_API_CALL(gblStringAlloc_(pString, capacity));
            memcpy(pString->data.pBuffer, pString->stackBuffer, oldLength+1);
        } else {
            pString->data.pBuffer = GBL_API_REALLOC(pString->data.pBuffer, capacity);
            pString->data.capacity = capacity;
        }
    }
    GBL_API_END();
}

GBL_API gblStringIsNull(const GblString* pStr, GblBool* pResult) {
    return (!pStr->data.pPtr && !pStr->data.length);
}
GBL_API gblStringIsEmpty(const GblString* pStr, GblBool* pResult) {
    return (pStr->data.pPtr && !pStr->data.length);
}
GBL_API gblStringIsValid(const GblString* pStr, GblBool* pResult) {
    return (!gblStringIsNull(pStr) && !gblStringIsEmpty(pStr));
}
GBL_API gblStringIsStack(const GblString* pStr) {
    return (pStr->data.pBuffer == pStr->stackBuffer);
}

GBL_API gblStringCStr(const GblString* pStr, const char** ppStr) {
    return pStr->data.pBuffer;
}

GBL_API gblStringResize(const GblString* pStr, GblSize length) {
    GBL_API_BEGIN(pStr->hCtx);
    if(pStr->data.length > length) {
        pStr->data.pBuffer[length] = '\0';

    } else if(pStr->data.length > length) {

        if(length > pStr->data.capacity) {
            gblStringReserve(pStr, GBL_STRING_CAPACITY_FROM_SIZE(length));
        }
        // Do we fill this shit in with something?
    }

    pStr->data.length = length;
    GBL_API_END();
}


GBL_API GblStringCompare(const GblString* pStr, const GblString* pStr2);

//GblStringCompareString
//GblStringCompareCString
//GblStringCompareView

GBL_API GblStringCat(const GblString* pStr, const GblStringView* pView);
GBL_API GblStringSnprintf(const GblString* pStr, const char* pFmt, ...);
GBL_API GblStringVaSnprintf(const GblString* pStr, const char* pFmt, va_list varArgs);

/*
 * FIND, TOKENIZE, REPLACE, TO UPPER, TO LOWER, LOCATE LAST TOKEN FOR SUFFIX/EXTENSION SHIT
 *
GBL_API GblStringSubString(const GblString* pStr, GblStringView* pView);

// GblString ONLY goes to/from string view?
// String View is the adapter to fucking everything else?



#ifdef __cplusplus
}
#endif

#endif // GblBAL_STRING_H

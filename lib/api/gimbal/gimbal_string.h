#ifndef GIMBAL_STRING_H
#define GIMBAL_STRING_H

#include "gimbal_types.h"
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif



typedef struct GimStringView { //contiguous sequence of chars, literally only has pointer and size
    const char* pBuffer;
    GimSize     size; // MAX == null terminated!
} GimStringView;

GIM_API gimStringView(GimStringView* pView);
GIM_API gimStringViewFromBuffer(GimStringView* pView, const char* pBuffer, GimSize count);
GIM_API gimStringViewFromStringView(GimStringView* pDest, GimStringView* pSource, GimSize count);
GIM_API gimStringViewFromGimString(GimStringView* pDest, GimString* pString, GimSize count);

GIM_API gimStringViewIsValid(GimStringView* pView);
GIM_API gimStringViewSize(GimStringView* pView, GimSize* pCount);
GIM_API gimStringViewBuffer(GimStringView* pView, const char** ppBuff); // NOT NULL TERMINATED!!!


/* C++ string view shit:
 *   copy, substr, compare, startsWith, endsWith, find, rFind, find_first_of, find_last_of
 *   find_first_not_of, find_last_not_of, Sequential/Vector iterable char array container!
 */

typedef struct GimRef {
    void*       pData;
    GimSize     refCount;
    
} GimRef;

typedef struct GimStringData {
    char*       pBuffer;
    GimSize     length;
    GimSize     capacity;
} GimStringData;

typedef struct GimString {
    GimContext* pCtx;
    GimStringData pData;
    char    stackBuffer[GIMBAL_STRING_BUFFER_STACK_SIZE];
} GimString;


GIM_API gimStringInit(GimContext* pCtx, const GimStringView* pView);
GIM_API gimStringDeinit(GimString* pStr)

GIM_API gimStringLength(const GimString* pStr, GimSize* pLength);
GIM_API gimStringCapacity(const GimString* pStr, GimSize* pCapacity);
GIM_API gimStringIsNull(const GimString* pStr, GimBool* pResult);
GIM_API gimStringIsEmpty(const GimString* pStr, GimBool* pResult);
GIM_API gimStringIsValid(const GimString* pStr, GimBool* pResult);

GIM_API gimStringCStr(const GimString* pStr, const char** ppStr);
GIM_API gimStringSet(GimString* pDest, const GimStringView* pStrView);

GIM_API gimStringResize(const GimString* pStr, GimSize length);
GIM_API gimStringReserve(const GimString* pStr, GimSize capacity);

GIM_API gimStringCompare(const GimString* pStr, const GimString* pStr2);

gimStringCompareString
gimStringCompareCString
gimStringCompareView

GIM_API gimStringCat(const GimString* pStr, const GimStringView* pView);
GIM_API gimStringSnprintf(const GimString* pStr, const char* pFmt, ...);
GIM_API gimStringVaSnprintf(const GimString* pStr, const char* pFmt, va_list varArgs);

/*
 * FIND, TOKENIZE, REPLACE, TO UPPER, TO LOWER, LOCATE LAST TOKEN FOR SUFFIX/EXTENSION SHIT
 *
GIM_API gimStringSubString(const GimString* pStr, GimStringView* pView);

// GimString ONLY goes to/from string view?
// String View is the adapter to fucking everything else?



#ifdef __cplusplus
}
#endif

#endif // GIMBAL_STRING_H

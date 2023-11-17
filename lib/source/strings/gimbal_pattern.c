#include <gimbal/strings/gimbal_pattern.h>
#include <gimbal/strings/gimbal_string_view.h>
#include <gimbal/strings/gimbal_string_buffer.h>
#include <gimbal/utils/gimbal_ref.h>
#include <re.h>

#include <stdatomic.h>

#define GBL_PATTERN_COMPILE_BUFFER_SIZE_    1024
#define GBL_PATTERN_STRING_BUFFER_SIZE_     128

static atomic_short activePatterns_ = 0;

static const GblPattern* GblPattern_compileStatic_(const char* pRegExp) {
    if(!pRegExp) return NULL;

    return (const GblPattern*)re_compile(pRegExp);
}

GBL_EXPORT const GblPattern* GblPattern_create(const char* pRegExp) {
    GblPattern* pPattern = NULL;

    if(pRegExp) {
        void* pBuffer = GBL_ALLOCA(GBL_PATTERN_COMPILE_BUFFER_SIZE_);
        unsigned size = GBL_PATTERN_COMPILE_BUFFER_SIZE_;

        if(re_compile_to(pRegExp, pBuffer, &size)) {
            GBL_ASSERT(size);

            pPattern = GblRef_create(size);
            memcpy(pPattern, pBuffer, size);

            atomic_fetch_add(&activePatterns_, 1);
        }
    }

    return pPattern;
}

GBL_EXPORT GblRefCount GblPattern_unref(const GblPattern* pSelf) {
    const GblRefCount refCount =  GblRef_unref((void*)pSelf);

    if(pSelf && !refCount)
        atomic_fetch_sub(&activePatterns_, 1);

    return refCount;
}

GBL_EXPORT GblRefCount GblPattern_refCount(const GblPattern* pSelf) {
    return GblRef_refCount(pSelf);
}

GBL_EXPORT GblRefCount GblPattern_totalCount(void) {
    return atomic_load(&activePatterns_);
}

GBL_EXPORT const GblPattern* GblPattern_ref(const GblPattern* pSelf) {
    return GblRef_ref((void*)pSelf);
}

GBL_EXPORT int GblPattern_compare(const GblPattern* pSelf, const GblPattern* pRhs) {
    return re_compare((re_t)pSelf, (re_t)pRhs);
}

GBL_EXPORT GblBool GblPattern_equals(const GblPattern* pSelf, const GblPattern* pRhs) {
    return GblPattern_compare(pSelf, pRhs) == 0;
}

GBL_EXPORT size_t GblPattern_bytes(const GblPattern* pSelf) {
    return re_size((re_t)pSelf);
}

GBL_EXPORT GblBool (GblPattern_match)(const GblPattern* pSelf,
                                      const char*       pString,
                                      GblStringView*    pMatch,
                                      int*              pCount)
{
    GblBool       retVal = GBL_FALSE;

    // Default arguments
    GblStringView defaultMatch = { 0 };
    int           defaultCount = 1;

    // Running counters
    int count      = 0;
    int prevPos    = 0;
    int pos        = 0;
    int prevLength = 0;
    int length     = 0;

    // Set default arguments
    if(!pCount)
        pCount = &defaultCount;
    if(!pMatch)
        pMatch = &defaultMatch;

    const GblBool valid = !!(pSelf && pString);

    // Only run if pattern + string are valid
    if(valid) {
        // Iterate over substring until no match is found
        while((pos = re_matchp((re_t)pSelf, pString + prevPos + prevLength, &length)) != -1) {
            // Update moving window substring
            pString     += prevPos + prevLength;
            prevPos     = pos;
            prevLength  = length;
            // Break early if we've found the expected match number
            if(++count == *pCount)
                break;
        }
    }

    // Check whether we found the expected match
    if(valid && (                     // If our pattern match check is valid AND (
        (*pCount == -1 && count) ||   // If we were looking for the last match and found at least 1 OR
        (count == *pCount)))          // If we found a particular match number)
    {
        // SUCCESS: Match is the last substring
        pMatch->pData  = prevLength? pString + prevPos : NULL;
        pMatch->length = prevLength;
        retVal         = GBL_TRUE;
    } else {
        // FAILURE: No match found
        pMatch->pData  = NULL;
        pMatch->length = 0;
    }

    // Return the count and whether we succeeded
    *pCount = count;
    return retVal;
}

GBL_EXPORT GblBool (GblPattern_matchStr)(const char*    pRegExp,
                                         const char*    pString,
                                         GblStringView* pMatch,
                                         int*           pCount)
{
    return GblPattern_match(GblPattern_compileStatic_(pRegExp),
                            pString,
                            pMatch,
                            pCount);
}

GBL_EXPORT GblBool (GblPattern_matchNot)(const GblPattern* pSelf,
                                         const char*       pString,
                                         GblStringView*    pMatch,
                                         int*              pCount)
{
    GblBool retVal = GBL_FALSE;

    // Default arguments
    GblStringView defaultMatch = { 0 };
    int           defaultCount = 1;

    if(!pCount)
        pCount = &defaultCount;
    if(!pMatch)
        pMatch = &defaultMatch;

    if(!pSelf || !pString) {
        *pMatch = GblStringView_fromEmpty();
        *pCount = 0;
        return GBL_FALSE;
    }

    GblStringView token;
    GblStringView prevToken;
    int totalCount = 0;
    *pMatch = GblStringView_fromEmpty();

    int count = 1;
    // Find first token
    if(!GblPattern_match(pSelf, pString, &token, &count)) {
        *pMatch = GblStringView_fromString(pString);
        if(*pCount == 1)
            return GBL_TRUE;
        else if(*pCount == -1) {
            *pCount = 1;
            return GBL_TRUE;
        } else {
            *pCount = 1;
            return GBL_FALSE;
        }
    }

    // If there's a non-match before the first token
    if(token.pData && token.pData != pString) {
        *pMatch = GblStringView_fromString(pString, token.pData - pString);
        ++totalCount;
    }

    const size_t length = strlen(pString);

    prevToken = token;
    while((*pCount == -1 || totalCount < *pCount) &&
          token.pData + token.length < pString + length &&
          GblPattern_match(pSelf, token.pData += token.length, &token, &count))
    {
        const size_t gapLength = token.pData - (prevToken.pData + prevToken.length);
        if(gapLength > 0) {
            ++totalCount;
            *pMatch = GblStringView_fromString(prevToken.pData + prevToken.length,
                                               gapLength);
        }
        prevToken = token;
    }

    // check if we still have one last token remaining
    if((*pCount == -1 || totalCount < *pCount) &&
       token.pData + token.length < pString + length) {
        const char* pNewHead = prevToken.pData + prevToken.length;
        *pMatch = GblStringView_fromString(pNewHead,
                                           pString + length - pNewHead);

        ++totalCount;
    }

    count = 1;
    if(totalCount &&
        (*pCount == -1 || totalCount == *pCount) &&
       GblPattern_match(pSelf, pMatch->pData, &token, &count)) {
        pMatch->length = token.pData - pMatch->pData;
    }

    if(*pCount == -1)
        *pCount = totalCount;

    if(totalCount == *pCount)
        return GBL_TRUE;
    else {
        *pCount = totalCount;
        return GBL_FALSE;
    }
}

GBL_EXPORT GblBool GblPattern_matchNotStr(const char*    pRegExp,
                                          const char*    pString,
                                          GblStringView* pMatch,
                                          int*       pCount)
{
    return GblPattern_matchNot(GblPattern_compileStatic_(pRegExp),
                               pString,
                               pMatch,
                               pCount);
}

GBL_EXPORT GblBool GblPattern_matchExact(const GblPattern* pSelf, const char* pString) {
    if(!pSelf)
        return GBL_FALSE;
    if(!pString)
        return GBL_FALSE;

    GblStringView match;

    if(GblPattern_match(pSelf, pString, &match, NULL)) {
        return (match.pData  == pString &&
                match.length == strlen(pString));
    }

    return GBL_FALSE;
}

GBL_EXPORT GblBool GblPattern_matchExactStr(const char* pRegExp,
                                            const char* pString)
{
    return GblPattern_matchExact(GblPattern_compileStatic_(pRegExp), pString);
}


GBL_EXPORT size_t GblPattern_matchCount(const GblPattern* pSelf, const char* pString) {
    int count = -1;
    GblPattern_match(pSelf, pString, NULL, &count);
    return count;
}

GBL_EXPORT size_t GblPattern_matchCountStr(const char* pRegExp, const char* pString) {
    return GblPattern_matchCount(GblPattern_compileStatic_(pRegExp), pString);
}

GBL_EXPORT const char* GblPattern_string(const GblPattern* pSelf,
                                         GblStringBuffer*  pBuff) {
    GblStringBuffer_resize(pBuff, GBL_PATTERN_STRING_BUFFER_SIZE_);
    unsigned size = GblStringBuffer_capacity(pBuff) - 1;
    re_string((re_t)pSelf, GblStringBuffer_data(pBuff), &size);
    //GblStringBuffer_resize(pBuff, size);
    return GblStringBuffer_data(pBuff);
}

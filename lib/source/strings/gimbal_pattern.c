#include <gimbal/strings/gimbal_pattern.h>
#include <gimbal/strings/gimbal_string_view.h>
#include <re.h>


GBL_EXPORT const GblPattern* GblPattern_compile(const char* pRegExp) {
    return pRegExp? (GblPattern*)re_compile(pRegExp) : NULL;
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
    return GblPattern_match(GblPattern_compile(pRegExp),
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
    if(token.pData != pString) {
        *pMatch = GblStringView_fromStringSized(pString, token.pData - pString);
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
            *pMatch = GblStringView_fromStringSized(prevToken.pData + prevToken.length,
                                                    gapLength);
        }
        prevToken = token;
    }

    // check if we still have one last token remaining
    if((*pCount == -1 || totalCount < *pCount) &&
       token.pData + token.length < pString + length) {
        const char* pNewHead = prevToken.pData + prevToken.length;
        *pMatch = GblStringView_fromStringSized(pNewHead,
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
    return GblPattern_matchNot(GblPattern_compile(pRegExp),
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
    return GblPattern_matchExact(GblPattern_compile(pRegExp), pString);
}


GBL_EXPORT size_t GblPattern_matchCount(const GblPattern* pSelf, const char* pString) {
    int count = -1;
    GblPattern_match(pSelf, pString, NULL, &count);
    return count;
}

GBL_EXPORT size_t GblPattern_matchCountStr(const char* pRegExp, const char* pString) {
    return GblPattern_matchCount(GblPattern_compile(pRegExp), pString);
}

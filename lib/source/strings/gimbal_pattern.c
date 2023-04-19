#include <gimbal/strings/gimbal_pattern.h>
#include <gimbal/strings/gimbal_string_view.h>
#include <re.h>


GBL_EXPORT const GblPattern* GblPattern_compile(const char* pRegExp) {
    return pRegExp? (GblPattern*)re_compile(pRegExp) : NULL;
}

GBL_EXPORT GblBool (GblPattern_match)(const GblPattern* pSelf,
                                      const char*       pString,
                                      GblStringView*    pMatch,
                                      ssize_t*          pCount)
{
    GblBool retVal = GBL_FALSE;
    // Default arguments
    GblStringView defaultMatch = { 0 };
    ssize_t       defaultCount = 1;

    // Running counters
    ssize_t count      = 0;
    int     prevPos    = 0;
    int     pos        = 0;
    int     prevLength = 0;
    int     length     = 0;

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
        pMatch->pData  = pString + prevPos;
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
                                         ssize_t*       pCount)
{
    return GblPattern_match(GblPattern_compile(pRegExp),
                            pString,
                            pMatch,
                            pCount);
}

GBL_EXPORT GblBool (GblPattern_matchNot)(const GblPattern* pSelf,
                                         const char*       pString,
                                         GblStringView*    pMatch,
                                         ssize_t*          pCount)
{


}

GBL_EXPORT GblBool GblPattern_matchNotStr(const char*    pRegExp,
                                          const char*    pString,
                                          GblStringView* pMatch,
                                          ssize_t*       pCount)
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
    ssize_t count = -1;
    GblPattern_match(pSelf, pString, NULL, &count);
    return count;
}

GBL_EXPORT size_t GblPattern_matchCountStr(const char* pRegExp, const char* pString) {
    return GblPattern_matchCount(GblPattern_compile(pRegExp), pString);
}

#include <gimbal/strings/gimbal_pattern.h>
#include <re.h>

GBL_EXPORT int GblPattern_firstMatchc(const GblPattern* pSelf, const char* pText, int* pLength) {
    if(!pSelf)
        return -1;
    if(!pText)
        return -1;

    int length;
    if(!pLength)
        pLength = &length;

    return re_matchp((re_t)pSelf, pText, pLength);
}

GBL_EXPORT int GblPattern_firstMatch(const char* pRegExp, const char* pText, int* pLength) {
    if(!pRegExp)
        return -1;
    if(!pText)
        return -1;

    int length;
    if(!pLength)
        pLength = &length;

    return re_match(pRegExp, pText, pLength);
}

GBL_EXPORT GblBool GblPattern_exactMatchc(const GblPattern* pSelf, const char* pString) {
    if(!pSelf)
        return -1;
    if(!pString)
        return -1;

    const int strLen = strlen(pString);
    int len;

    GblPattern_firstMatchc(pSelf, pString, &len);

    return strLen == len;
}

GBL_EXPORT GblBool GblPattern_exactMatch(const char* pRegExp, const char* pString) {
    return GblPattern_exactMatchc(GblPattern_compile(pRegExp), pString);
}

GBL_EXPORT GblPattern* GblPattern_compile(const char* pRegExp) {
    return pRegExp? (GblPattern*)re_compile(pRegExp) : NULL;
}

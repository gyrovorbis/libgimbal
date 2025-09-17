#include <gimbal/strings/gimbal_string.h>
#include <gimbal/core/gimbal_ctx.h>
#include <ctype.h>

GBL_EXPORT char* gblStrdup(const char* pStr) {
    return gblStrndup(pStr, strlen(pStr));
}

GBL_EXPORT char* gblStrndup(const char* pStr, size_t length) {
    char* pNew = NULL;
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_POINTER(pStr);
    pNew = GBL_CTX_NEW(char, length+1);
    memcpy(pNew, pStr, length);
    pNew[length] = '\0';
    GBL_CTX_END_BLOCK();
    return pNew;
}

// Returns 0 if length is 0
GBL_INLINE int gblStrnCaseCmp2_(const char* s1, const char* s2, size_t n) {
    if (n == 0)
      return 0;

    while (n-- != 0 && tolower(*s1) == tolower(*s2)) {
        if (n == 0 || *s1 == '\0' || *s2 == '\0')
            break;

        s1++;
        s2++;
      }

    return tolower(*(unsigned char *) s1) - tolower(*(unsigned char *) s2);
}

GBL_EXPORT char* gblStrCaseStr(const char* s, const char* find) {
    char c, sc;
    size_t len;
    if ((c = *find++) != 0) {
        c = (char)tolower((unsigned char)c);
        len = strlen(find);
        do {
            do {
                if ((sc = *s++) == 0)
                    return (NULL);
            } while ((char)tolower((unsigned char)sc) != c);
        } while (gblStrnCaseCmp2_(s, find, len) != 0);
        s--;
    }
    return ((char *)s);
}

// Doesn't return 0 if length is 0
GBL_EXPORT int gblStrnCaseCmp(const char* pStr1, const char* pStr2, size_t length) {
    GBL_ASSERT(pStr1 && pStr2, "NULL string!");
    int result;
    size_t l = 0;

    if(pStr1 == pStr2) return 0;

    while((result = tolower(*pStr1) - tolower(*pStr2)) == 0) {
        if(*pStr1++ == '\0') break;
        if(*pStr2++ == '\0') break;
        if(++l == length) break;
    }

    return result;
}

GBL_EXPORT int gblStrCaseCmp(const char* pStr1, const char* pStr2) {
    const size_t len1 = strlen(pStr1);
    const size_t len2 = strlen(pStr2);
    return gblStrnCaseCmp(pStr1, pStr2, len1 < len2? len1 : len2);
}


GBL_EXPORT size_t gblStrnlen(const char* pStr1, size_t len) {
    for(size_t i = 0; i < len; ++i) {
        if(*pStr1++ == '\0')
            return i;
    }

    return len;
}

GBL_EXPORT char* gblStrnReverse(char* pString, size_t length) {
    length = gblStrnlen(pString, length);

    for(size_t i = 0; i < length / 2; ++i) {
        GBL_SWAP(pString[i], pString[length - i - 1]);
    }

    return pString;
}

GBL_EXPORT int gblAsciiDigitValue(char c) {
    return isdigit(c)? c-'0' : -1;
}

GBL_EXPORT int gblAsciiHexDigitValue(char c) {
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    return gblAsciiDigitValue(c);
}

#include <gimbal/strings/gimbal_string.h>
#include <gimbal/core/gimbal_ctx.h>

GBL_EXPORT char* gblStrdup(const char* pStr) {
    return gblStrndup(pStr, strlen(pStr));
}

GBL_EXPORT char* gblStrndup(const char* pStr, GblSize length) {
    char* pNew = NULL;
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_POINTER(pStr);
    pNew = GBL_CTX_NEW(char, length+1);
    memcpy(pNew, pStr, length);
    pNew[length+1] = '\0';
    GBL_CTX_END_BLOCK();
    return pNew;
}

GBL_EXPORT int gblStrnCaseCmp(const char* pStr1, const char* pStr2, GblSize length) {
    GBL_ASSERT(pStr1 && pStr2, "NULL string!");
    int result;
    GblSize l = 0;

    if(pStr1 == pStr2) return 0;

    while((result = tolower(*pStr1) - tolower(*pStr2)) == 0) {
        if(*pStr1++ == '\0') break;
        if(*pStr2++ == '\0') break;
        if(++l == length) break;
    }

    return result;
}

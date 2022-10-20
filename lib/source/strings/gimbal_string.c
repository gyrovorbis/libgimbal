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

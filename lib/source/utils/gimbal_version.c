#include <gimbal/utils/gimbal_version.h>
#include <gimbal/core/gimbal_decls.h>

GBL_EXPORT GBL_RESULT gblVersionInfoString(const GblVersionInfo* pInfo, char* pBuffer, size_t size) {
    GBL_ASSERT(pInfo, "");
    GBL_ASSERT(pBuffer, "");

    const int retVal = snprintf(pBuffer, size, "%u.%u.%u", pInfo->major, pInfo->minor, pInfo->patch);
    return retVal? GBL_RESULT_SUCCESS : GBL_RESULT_TRUNCATED;
}

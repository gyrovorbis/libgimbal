//include <evmu/evmu_version.h>
#include <gimbal/types/gimbal_typedefs.h>

GBL_API gblVersionInfoString(const GblVersionInfo* pInfo, char* pBuffer, size_t size) {
    GBL_ASSERT(pInfo, "");
    GBL_ASSERT(pBuffer, "");

    const int retVal = snprintf(pBuffer, size, "%u.%u.%u", pInfo->major, pInfo->minor, pInfo->patch);
    return retVal? GBL_RESULT_SUCCESS : GBL_RESULT_TRUNCATED;
}

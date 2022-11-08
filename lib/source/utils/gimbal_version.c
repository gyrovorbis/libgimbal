#include <gimbal/utils/gimbal_version.h>
#include <gimbal/core/gimbal_decls.h>
#include <inttypes.h>

GBL_EXPORT char* GblVersion_string(GblVersion value, char* pBuffer) GBL_NOEXCEPT {
    snprintf(pBuffer,
             GBL_VERSION_STRING_SIZE,
             "%u.%u.%u",
             GblVersion_major(value),
             GblVersion_minor(value),
             GblVersion_patch(value));
    return pBuffer;
}

GBL_EXPORT GblVersion GblVersion_parse(const char* pString) GBL_NOEXCEPT {
    unsigned major, minor, patch;

    sscanf(pString, "%u.%u.%u",
           &major, &minor, &patch);

    return GblVersion_make(major, minor, patch);
}

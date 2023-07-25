#include <gimbal/utils/gimbal_version.h>
#include <gimbal/core/gimbal_decls.h>
#include <inttypes.h>

GBL_EXPORT char* GblVersion_string(GblVersion value, char* pBuffer) {
    snprintf(pBuffer,
             GBL_VERSION_STRING_SIZE,
             "%u.%u.%u",
             GblVersion_major(value),
             GblVersion_minor(value),
             GblVersion_patch(value));
    return pBuffer;
}

GBL_EXPORT GblVersion GblVersion_parse(const char* pString) {
    unsigned major, minor, patch;

    sscanf(pString, "%u.%u.%u",
           &major, &minor, &patch);

    return GblVersion_make(major, minor, patch);
}

GBL_EXPORT GblVersion GblVersion_make(uint8_t major, uint16_t minor, uint8_t patch) {
    return GBL_VERSION_MAKE(major, minor, patch);
}

GBL_EXPORT uint8_t GblVersion_major(GblVersion self) {
    return (uint8_t)((uint32_t)((self >> GBL_VERSION_MAJOR_BITPOS) & GBL_VERSION_MAJOR_MASK));
}

GBL_EXPORT uint16_t GblVersion_minor(GblVersion self) {
    return (uint16_t)((uint32_t)((self >> GBL_VERSION_MINOR_BITPOS) & GBL_VERSION_MINOR_MASK));
}

GBL_EXPORT uint8_t GblVersion_patch(GblVersion self) {
    return (uint8_t)((uint32_t)((self >> GBL_VERSION_PATCH_BITPOS) & GBL_VERSION_PATCH_MASK));
}

GBL_EXPORT void GblVersion_setMajor(GblVersion* pSelf, uint8_t major) {
    *pSelf &= ~(GBL_VERSION_MAJOR_MASK << GBL_VERSION_MAJOR_BITPOS);
    *pSelf |= (major << GBL_VERSION_MAJOR_BITPOS);
}

GBL_EXPORT void GblVersion_setMinor(GblVersion* pSelf, uint16_t minor) {
    *pSelf &= ~(GBL_VERSION_MINOR_MASK << GBL_VERSION_MINOR_BITPOS);
    *pSelf |= (minor << GBL_VERSION_MINOR_BITPOS);
}

GBL_EXPORT void GblVersion_setPatch(GblVersion* pSelf, uint8_t patch) {
    *pSelf &= ~(GBL_VERSION_PATCH_MASK << GBL_VERSION_PATCH_BITPOS);
    *pSelf |= (patch << GBL_VERSION_PATCH_BITPOS);
}

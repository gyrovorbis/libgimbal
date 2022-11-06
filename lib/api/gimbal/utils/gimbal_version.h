/*! \file
 *  \brief GblVersion type, utlities and query functions
 *  \ingroup utils
 */

#ifndef GIMBAL_VERSION_H
#define GIMBAL_VERSION_H

#include "../core/gimbal_result.h"

#define GBL_VERSION_FIELD_MAJOR_BIT                 24u
#define GBL_VERSION_FIELD_MAJOR_MASK                0xff

#define GBL_VERSION_FIELD_MINOR_BIT                 8u
#define GBL_VERSION_FIELD_MINOR_MASK                0xffff

#define GBL_VERSION_FIELD_PATCH_BIT                 0u
#define GBL_VERSION_FIELD_PATCH_MASK                0xff

#define GBL_VERSION_MAKE(major, minor, patch)                                                       \
    (uint32_t)((major & (uint8_t)GBL_VERSION_FIELD_MAJOR_MASK) << GBL_VERSION_FIELD_MAJOR_BIT)   |  \
    (uint32_t)((minor & (uint16_t)GBL_VERSION_FIELD_MINOR_MASK) << GBL_VERSION_FIELD_MINOR_BIT)  |  \
    (uint32_t)((patch & (uint8_t)GBL_VERSION_FIELD_PATCH_MASK) << GBL_VERSION_FIELD_PATCH_BIT)

#define GBL_VERSION_EXTRACT_MAJOR(version)                                                  \
    (uint8_t)((uint32_t)((version >> GBL_VERSION_FIELD_MAJOR_BIT) & GBL_VERSION_FIELD_MAJOR_MASK))

#define GBL_VERSION_EXTRACT_MINOR(version)                                                  \
    (uint16_t)((uint32_t)((version >> GBL_VERSION_FIELD_MINOR_BIT) & GBL_VERSION_FIELD_MINOR_MASK))

#define GBL_VERSION_EXTRACT_PATCH(version)                                                  \
    (uint8_t)((uint32_t)((version >> GBL_VERSION_FIELD_PATCH_BIT) & GBL_VERSION_FIELD_PATCH_MASK))

#define GBL_VERSION_STRINGIFY(major, minor, patch) \
    GBL_STRINGIFY(major) "."                       \
    GBL_STRINGIFY(minor) "."                       \
    GBL_STRINGIFY(patch)

GBL_DECLS_BEGIN

typedef uint32_t    GblVersion;

typedef struct GblVersionInfo {
    uint8_t     major;
    uint16_t    minor;
    uint8_t     patch;
} GblVersionInfo;

GBL_INLINE GblVersionInfo GBL_VERSION_EXTRACT(GblVersion version) {
    const GblVersionInfo info = {
        GBL_VERSION_EXTRACT_MAJOR(version),
        GBL_VERSION_EXTRACT_MINOR(version),
        GBL_VERSION_EXTRACT_PATCH(version)
    };
    return info;
}


#define GBL_VERSION_STRING_SIZE_MAX     (3 * 3 + 2 + 1) // 3 chars * 3 subTypes + 2 dots + 1 null char
GBL_EXPORT GBL_RESULT gblVersionInfoString(const GblVersionInfo* pInfo, char* pBuffer, size_t size);



GBL_DECLS_END


#endif // GIMBAL_VERSION_H

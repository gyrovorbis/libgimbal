/*! \file
 *  \brief GblVersion type, utlities and query functions
 *  \ingroup utils
 *  \todo
 *      - implement fuzzy comparison: GblVersion_compare(version, ">=3.23.0")
 */

#ifndef GIMBAL_VERSION_H
#define GIMBAL_VERSION_H

#include "../core/gimbal_result.h"

#define GBL_VERSION_MAJOR_BITPOS    24u
#define GBL_VERSION_MAJOR_MASK      0xff

#define GBL_VERSION_MINOR_BITPOS    8u
#define GBL_VERSION_MINOR_MASK      0xffff

#define GBL_VERSION_PATCH_BITPOS    0u
#define GBL_VERSION_PATCH_MASK      0xff

#define GBL_VERSION_STRING_SIZE     (2*3 + 5 + 2 + 1)

#define GBL_VERSION_MAKE(major, minor, patch)               \
    ((uint32_t)((major & (uint8_t)GBL_VERSION_MAJOR_MASK)   \
                << GBL_VERSION_MAJOR_BITPOS) |              \
    (uint32_t)((minor & (uint16_t)GBL_VERSION_MINOR_MASK)   \
                << GBL_VERSION_MINOR_BITPOS)  |             \
    (uint32_t)((patch & (uint8_t)GBL_VERSION_PATCH_MASK)    \
               << GBL_VERSION_PATCH_BITPOS))

#define GBL_VERSION_STRINGIFY(major, minor, patch) \
    GBL_STRINGIFY(major) "."                       \
    GBL_STRINGIFY(minor) "."                       \
    GBL_STRINGIFY(patch)

#define GBL_SELF_TYPE GblVersion

GBL_DECLS_BEGIN

// ===== Public API =====

/// 32-bit unsigned integer representing packed version value
typedef uint32_t GblVersion;

GBL_INLINE GblVersion GblVersion_make     (uint8_t  major,
                                           uint16_t minor,
                                           uint8_t  patch)           GBL_NOEXCEPT;

GBL_EXPORT GblVersion GblVersion_parse    (const char* pString)      GBL_NOEXCEPT;

GBL_EXPORT char*      GblVersion_string   (GBL_VSELF, char* pBuffer) GBL_NOEXCEPT;

GBL_INLINE uint8_t    GblVersion_major    (GBL_VSELF)                GBL_NOEXCEPT;
GBL_INLINE uint16_t   GblVersion_minor    (GBL_VSELF)                GBL_NOEXCEPT;
GBL_INLINE uint8_t    GblVersion_patch    (GBL_VSELF)                GBL_NOEXCEPT;

GBL_INLINE void       GblVersion_setMajor (GBL_SELF, uint8_t major)  GBL_NOEXCEPT;
GBL_INLINE void       GblVersion_setMinor (GBL_SELF, uint16_t minor) GBL_NOEXCEPT;
GBL_INLINE void       GblVersion_setPatch (GBL_SELF, uint8_t patch)  GBL_NOEXCEPT;

// ===== Implementation =====
GBL_INLINE GblVersion GblVersion_make(uint8_t major, uint16_t minor, uint8_t patch) GBL_NOEXCEPT {
    return GBL_VERSION_MAKE(major, minor, patch);
}

GBL_INLINE uint8_t GblVersion_major(GBL_VSELF) GBL_NOEXCEPT {
    return (uint8_t)((uint32_t)((self >> GBL_VERSION_MAJOR_BITPOS) & GBL_VERSION_MAJOR_MASK));
}

GBL_INLINE uint16_t GblVersion_minor(GBL_VSELF) GBL_NOEXCEPT {
    return (uint16_t)((uint32_t)((self >> GBL_VERSION_MINOR_BITPOS) & GBL_VERSION_MINOR_MASK));
}

GBL_INLINE uint8_t GblVersion_patch(GBL_VSELF) GBL_NOEXCEPT {
    return (uint8_t)((uint32_t)((self >> GBL_VERSION_PATCH_BITPOS) & GBL_VERSION_PATCH_MASK));
}

GBL_INLINE void GblVersion_setMajor(GBL_SELF, uint8_t major) GBL_NOEXCEPT {
    *pSelf &= ~(GBL_VERSION_MAJOR_MASK << GBL_VERSION_MAJOR_BITPOS);
    *pSelf |= (major << GBL_VERSION_MAJOR_BITPOS);
}

GBL_INLINE void GblVersion_setMinor(GBL_SELF, uint16_t minor) GBL_NOEXCEPT {
    *pSelf &= ~(GBL_VERSION_MINOR_MASK << GBL_VERSION_MINOR_BITPOS);
    *pSelf |= (minor << GBL_VERSION_MINOR_BITPOS);
}

GBL_INLINE void GblVersion_setPatch(GBL_SELF, uint8_t patch) GBL_NOEXCEPT {
    *pSelf &= ~(GBL_VERSION_PATCH_MASK << GBL_VERSION_PATCH_BITPOS);
    *pSelf |= (patch << GBL_VERSION_PATCH_BITPOS);
}

GBL_DECLS_END

#undef GBL_SELF_TYPE


#endif // GIMBAL_VERSION_H

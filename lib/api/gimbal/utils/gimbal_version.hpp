#ifndef GIMBAL_VERSION_HPP
#define GIMBAL_VERSION_HPP

#include "gimbal_version.h"

namespace gbl {

class Version {
private:
    GblVersion version_ = 0;

public:
    constexpr Version(uint8_t  major = 0,
                      uint16_t minor = 0,
                      uint8_t  patch = 0) noexcept:
        version_(GBL_VERSION_MAKE_(major, minor, patch)) {}

    constexpr Version(GblVersion version) noexcept:
        version_(version) {}

    Version(const char* pString) {
        // check for errors, throw exception
        version_ = GblVersion_parse(pString);
    }

    constexpr uint8_t  major() const noexcept { return (version_ >> GBL_VERSION_MAJOR_BITPOS) & GBL_VERSION_MAJOR_MASK; }
    constexpr uint16_t minor() const noexcept { return (version_ >> GBL_VERSION_MINOR_BITPOS) & GBL_VERSION_MINOR_MASK; }
    constexpr uint8_t  patch() const noexcept { return (version_ >> GBL_VERSION_PATCH_BITPOS) & GBL_VERSION_PATCH_MASK; }

    constexpr void setMajor(uint8_t major) noexcept {
        version_ &= ~(GBL_VERSION_MAJOR_MASK << GBL_VERSION_MAJOR_BITPOS);
        version_ |= (major << GBL_VERSION_MAJOR_BITPOS);
    }

    constexpr void setMinor(uint16_t minor) noexcept {
        version_ &= ~(GBL_VERSION_MINOR_MASK << GBL_VERSION_MINOR_BITPOS);
        version_ |= (minor << GBL_VERSION_MINOR_BITPOS);
    }

    constexpr void setPatch(uint16_t patch) noexcept {
        version_ &= ~(GBL_VERSION_PATCH_MASK << GBL_VERSION_PATCH_BITPOS);
        version_ |= (patch << GBL_VERSION_PATCH_BITPOS);
    }

    constexpr operator GblVersion() const noexcept { return version_; }

};

static_assert(sizeof(Version) == sizeof(GblVersion),
              "C++ Version incompatiblly sized with C version!");
}

#endif // GIMBAL_VERSION_HPP

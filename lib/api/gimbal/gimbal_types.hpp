#ifndef GIMBAL_TYPES_HPP
#define GIMBAL_TYPES_HPP

#include "gimbal_api.h"
#include "gimbal_object.h"
#include "gimbal_generics.hpp"
#include <type_traits>
#include <string_view>
#include <stdexcept>
#include <cstring>
#include <string>


namespace gimbal {

    using Bool      = ::GblBool;
    using Size      = ::GblSize;
    using Float     = ::GblFloat;
    using Int       = ::GblInt;
    using Enum      = ::GblEnum;
    using Hash      = ::GblHash;


GBL_ENUM_TABLE_DECLARE_CPP(GBL_META_LOG_LEVEL_TABLE);
GBL_ENUM_TABLE_DECLARE_CPP(GBL_META_OBJECT_TYPE_TABLE);

    class Version: public PrimitiveBase<GblVersion> {
    public:
        Version(void) = default;
        Version(GblVersion version): PrimitiveBase(version) {}
        Version(GblVersionInfo info): Version(GBL_VERSION_MAKE(info.major, info.minor, info.patch)) {}
        Version(uint8_t major, uint16_t minor, uint8_t patch): Version(GblVersionInfo{ major, minor, patch }) {}
        Version(std::string_view string);

        constexpr uint8_t   getMajor(void) const { return GBL_VERSION_EXTRACT_MAJOR(*this); }
        constexpr uint16_t  getMinor(void) const { return GBL_VERSION_EXTRACT_MINOR(*this); }
        constexpr uint8_t   getPatch(void) const { return GBL_VERSION_EXTRACT_PATCH(*this); }

        constexpr GblVersion getValue(void) const { return getPrimitiveValue(); }
        constexpr GblVersionInfo getInfo(void) const {
            return { getMajor(), getMinor(), getPatch() };
        }

        std::string toString(void) const {
            char buffer[GBL_VERSION_STRING_SIZE_MAX] = { 0 };
            const auto info = getInfo();
            const GBL_RESULT result = gblVersionInfoString(&info, buffer, sizeof(buffer));
           // GBL_ASSERT(GBL_RESULT_SUCCESS(result));
            return buffer;
        }


        //overload spaceship operator
    };

    GBL_CHECK_C_CPP_TYPE_COMPAT(Version, GblVersion);



}

#endif // GIMBAL_TYPES_HPP

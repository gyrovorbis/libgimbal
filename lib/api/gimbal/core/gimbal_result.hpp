#ifndef GIMBAL_RESULT_HPP
#define GIMBAL_RESULT_HPP

#include "gimbal_result.h"
#include "../core/gimbal_api_generators.hpp"

namespace gimbal {

GBL_ENUM_TABLE_DECLARE_CPP_BEGIN(GBL_META_RESULT_TABLE)
public:
    enum class Type: uint8_t {
        Success,
        Partial,
        Error,
        Unknown,
        Count
    };

    constexpr Result(bool success) noexcept: Result(success? GBL_RESULT_SUCCESS : GBL_RESULT_ERROR) {}

    constexpr Type getType(void) const noexcept {
        if(isSuccess()) return Type::Success;
        else if(isPartial()) return Type::Partial;
        else if(isError()) return Type::Error;
        else if(isUnknown()) return Type::Unknown;
        else GBL_ASSERT(false, "Not typed!");
        return Type::Unknown;
    }

    constexpr bool isUnknown(void) const noexcept { return GBL_RESULT_UNKNOWN(getCode()); }
    constexpr bool isSuccess(void) const noexcept { return GBL_RESULT_SUCCESS(getCode()); }
    constexpr bool isPartial(void) const noexcept { return GBL_RESULT_PARTIAL(getCode()); }
    constexpr bool isError(void) const noexcept { return GBL_RESULT_ERROR(getCode()); }
    constexpr bool isUnavailable(void) const noexcept { return GBL_RESULT_UNAVAILABLE(getCode()); }
    constexpr bool isType(Type type) const noexcept { return type == getType(); }

    constexpr operator bool() const noexcept { return isSuccess(); }
GBL_ENUM_TABLE_DECLARE_CPP_END(GBL_META_RESULT_TABLE)

}

#endif // GIMBAL_RESULT_HPP

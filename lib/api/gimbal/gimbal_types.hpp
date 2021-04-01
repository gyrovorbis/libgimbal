#ifndef GIMBAL_TYPES_HPP
#define GIMBAL_TYPES_HPP

#include "gimbal_types.h"

namespace gimbal {

    using Bool      = ::GblBool;
    using Size      = ::GblSize;
    using Real      = ::GblReal;
    using Int       = ::GblInt;
    using Enum      = ::GblEnum;
    using Hash      = ::GblHash;
    using Version   = ::GblVersion;

    enum class LogLevel: Enum {
        Debug,
        Verbose,
        Info,
        Warning,
        Error,
        Count
    };



}

#endif // GIMBAL_TYPES_HPP

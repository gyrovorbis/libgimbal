#ifndef GIMBAL_TYPEDEFS_HPP
#define GIMBAL_TYPEDEFS_HPP

#include "gimbal_typedefs.h"
#include "../core/gimbal_ext.h"
#include "../core/gimbal_api_generators.hpp"
#include <type_traits>
#include <string_view>
#include <stdexcept>
#include <cstring>
#include <string>


namespace gimbal {

    namespace tags { };

    using Bool          = ::GblBool;
    using Size          = ::GblSize;
    using Float         = ::GblFloat;
    using Int           = ::GblInt;
    using Enum          = ::GblEnum;
    using Hash          = ::GblHash;
    using FnPtr         = ::GblFnPtr;
    using Ptr           = ::GblPtr;
    using Hash          = ::GblHash;


class Context;
class Handle;
class Object;

GBL_ENUM_TABLE_DECLARE_CPP(GBL_META_LOG_LEVEL_TABLE);
//GBL_ENUM_TABLE_DECLARE_CPP(GBL_META_OBJECT_TYPE_TABLE);




}

#endif // GIMBAL_TYPEDEFS_HPP

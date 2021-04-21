#ifndef GIMBAL_VARIANT_HPP
#define GIMBAL_VARIANT_HPP


#include "gimbal_variant.h"
#include "gimbal_types.hpp"

namespace gimbal {


//LuaVariant compat
//std::variant compat
//QVariant compat
//std::any compat
//std::string vs std::string_view
//exceptions for reest (maybe configurable?)
//asserts for reest? (maybe configurable)
//std::floating_point, std::integral concepts?
    class Variant: public GblVariant {
    public:

        enum class Type: Enum {
            Nil             = GBL_VARIANT_TYPE_NIL,
            Bool            = GBL_VARIANT_TYPE_BOOL,
            Int             = GBL_VARIANT_TYPE_INT,
            Float           = GBL_VARIANT_TYPE_FLOAT,
            String          = GBL_VARIANT_TYPE_STRING,
            Function        = GBL_VARIANT_TYPE_FUNCTION,
            VoidPtr         = GBL_VARIANT_TYPE_VOID_PTR,
            Table           = GBL_VARIANT_TYPE_TABLE,
            Userdata        = GBL_VARIANT_TYPE_USERDATA,
            BuiltinCount    = Userdata + 1,
            UserType        = BuiltinCount
        };

        constexpr Variant(void);


        template<typename T>
        constexpr T     getValue(void) const;

        template<typename T>
        constexpr void setValue(T&& value) const;

        //SPACE SHIP FUCKING OPERATOR

        template<typename T>
        constexpr bool operator==(T&& value) const;

        template<typename T>
        constexpr bool operator!=(T&& value) const;

        //Variant operator const();

        constexpr Type        getType(void) const;
        const char*           getTypeName(void) const;
        constexpr bool        isNil(void) const;
        constexpr bool        isRefType(void) const;
        constexpr bool        isIndexible(void) const;
    };

    static_assert(sizeof(Variant) == sizeof(GblVariant), "C and C++ types are not the same size!");

}

#endif // GIMBAL_VARIANT_HPP

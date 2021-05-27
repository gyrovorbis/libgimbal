#ifndef GIMBAL_VARIANT_HPP
#define GIMBAL_VARIANT_HPP


#include "gimbal_variant.h"
#include "gimbal_types.hpp"
#include "gimbal_string.hpp"

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
        constexpr Variant(std::nullptr_t);
        constexpr Variant(bool val);
        constexpr Variant(gimbal::Int val);
        constexpr Variant(gimbal::Float val);
        constexpr Variant(void* pUd);
        constexpr Variant(gimbal::String str);
        constexpr Variant(gimbal::Object* pObj);

        constexpr Variant(const Variant& rhs);
                  Variant(Variant&& rhs);

        ~Variant(void) = default; //gotta free the stringz!

        constexpr Type getType(void) const;

        template<typename T>
        constexpr T     getValue(void) const; //THROW BAD TYPE EXCEPTION

        template<typename T>
        constexpr void setValue(T&& value) const;

         template<typename T>
         Variant &operator=(T&& value);

         template<typename T>
         Variant &operator+=(T&& value); //throw bad type exception if not numeric

        //SPACE SHIP FUCKING OPERATOR
        template<typename T>
        constexpr bool operator==(T&& value) const;

        template<typename T>
        constexpr bool operator<=>(T&& value) const;

        Variant asBool(void) const;
        Variant asFloat(void) const;
        Variant asInteger(void) const;
        Variant asString(void) const;
        Variant asUserdata(void) const;
    };

    static_assert(sizeof(Variant) == sizeof(GblVariant), "C and C++ types are not the same size!");

}

#endif // GIMBAL_VARIANT_HPP

#ifndef GIMBAL_API_GENERATORS_HPP
#define GIMBAL_API_GENERATORS_HPP

#include "gimbal_api_generators.h"
#include <type_traits>
#include <functional>

namespace gbl {

#define GBL_CHECK_C_CPP_TYPE_COMPAT(CppType, CType) \
    GBL_STATIC_ASSERT_MSG(sizeof(CppType) == sizeof(CType), "sizeof(" #CppType ") != sizeof(" #CType")")

#define GBL_ENUM_TUPLE_DECL_ENUM_CPP(cName, value, name, string) \
    name = value,


        decltype(auto) constptr_cast(auto&& value) {
            return &std::add_lvalue_reference_t<std::add_const_t<decltype(value)>>(value);
        }

        template<typename To>
        To constptr_cast(auto&& value) {
            return static_cast<To>(constptr_cast(std::forward<decltype(value)>(value)));
        }


        template<typename P, typename CRTP>
        class PrimitiveCompatible {
        private:
            using ThisType = PrimitiveCompatible<P, CRTP>;
            const CRTP* derived(void) const { return static_cast<const CRTP*>(this); }
            CRTP* derived(void) { return static_cast<CRTP*>(this); }
        public:

            template<typename T>
            constexpr static std::enable_if_t<std::same_as<T, P>, P*> primitive_cast(CRTP* pDerived) noexcept { return &*static_cast<ThisType*>(pDerived); }

            constexpr const P* primitive_cast(void) const noexcept { return reinterpret_cast<const P*>(derived()->getPrimitiveAddress()); }

            constexpr operator P() const noexcept { return static_cast<P>(derived()->getPrimitiveValue()); }
            constexpr P* operator&(void) noexcept { return static_cast<P*>(derived()->getPrimitiveAddress()); }
            constexpr const P* operator&(void) const noexcept { return static_cast<const P*>(derived()->getPrimitiveAddress()); }
        };

        template<typename P>
        class PrimitiveBase: public PrimitiveCompatible<P, PrimitiveBase<P>> {
        protected:
            P primitive_={};
        public:
            using PrimitiveType = P;

            constexpr PrimitiveBase(void) noexcept = default;
            constexpr PrimitiveBase(P p) noexcept: primitive_(std::move(p)) {}

            constexpr P getPrimitiveValue(void) const noexcept { return primitive_; }
            constexpr const P* getPrimitiveAddress(void) const noexcept { return &primitive_; }
            constexpr P* getPrimitiveAddress(void) noexcept { return &primitive_; }
        };


#define GBL_ENUM_TABLE_DECLARE_CPP_BEGIN(table) \
    class GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, NAME)) : \
        public gimbal::PrimitiveBase<GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, CNAME))> \
    { \
        using CppType = GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, NAME));  \
        using CType = GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, CNAME));   \
        public: \
        enum Value { \
            GBL_MAP_TUPLES(GBL_ENUM_TUPLE_DECL_ENUM_CPP,  GBL_MAP_TUPLES(GBL_EVAL, GBL_META_ENUM_TUPLE_VALUE_ARRAY table))  \
        };  \
        constexpr GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, NAME))(void) noexcept: gimbal::PrimitiveBase<CType>(static_cast<CType>(0)) {} \
        constexpr GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, NAME))(Value value) noexcept: CppType(static_cast<CType>(value)) {}  \
        constexpr GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, NAME))(CType code) noexcept: gimbal::PrimitiveBase<CType>(code) {}  \
        constexpr operator Value() const noexcept { return getValue(); } \
        constexpr CType getCode(void) const noexcept { return static_cast<CType>(getPrimitiveValue()); }    \
        constexpr Value getValue(void) const noexcept { return static_cast<Value>(getPrimitiveValue()); }   \
        const char* toString(void) const { \
            const char* pStr = GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, STRINGIFIER))(getCode()); \
            if(strcmp(pStr, "") == 0) { \
                throw std::runtime_error(std::string("Unhandled Enum Value ") + std::to_string(getCode()));    \
            }   \
            return pStr; \
        } \
        constexpr GblEnum toInt(void) const noexcept { return static_cast<GblEnum>(getCode()); } \
        constexpr bool isInRange(void) const noexcept { return false; } \
        constexpr friend bool operator==(CppType rhs, Value lhs) {    \
            return rhs.getCode() == CppType(lhs);  \
        }   \
        constexpr friend bool operator!=(CppType rhs, Value lhs) {    \
            return !(rhs == lhs);   \
        }
}


#define GBL_ENUM_TABLE_DECLARE_CPP_END(table) \
    }; \
    GBL_CHECK_C_CPP_TYPE_COMPAT(GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, NAME)), GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, CNAME)));

#define GBL_ENUM_TABLE_DECLARE_CPP(table) \
    GBL_ENUM_TABLE_DECLARE_CPP_BEGIN(table) \
    GBL_ENUM_TABLE_DECLARE_CPP_END(table)



#endif // GIMBAL_API_GENERATORS_HPP

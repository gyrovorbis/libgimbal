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

#if 0
{
constexpr Variant(const GblVariant& rhs) noexcept: GblVariant(rhs) {}

friend bool operator()

}
#endif


GBL_ENUM_TABLE_DECLARE_CPP(GBL_META_VARIANT_TYPE_TABLE);


template<typename T>
struct VariantCompatibleTypeTraitsDefault {
    using Type = T;
    using ThisType = VariantCompatibleTypeTraitsDefault<T>;
    using ForwardType = T;
    struct Forwarder {
        static ForwardType forward(Type value) { return static_cast<ForwardType>(value); }
    };

    constexpr static auto variantType  = VariantType(VariantType::Nil);

};


template<typename T, typename=void>
struct VariantCompatibleTypeTraits: public VariantCompatibleTypeTraitsDefault<T> {

};


template<typename T>
concept variant_compatible =
        requires(T t) {
            VariantCompatibleTypeTraits<T>::variantType != VariantType::Nil;
        };

        template<typename Class>
            requires requires { typename Class::VariantCompatibleTypeTraits; }
        struct VariantCompatibleTypeTraits<Class>: public Class::VariantCompatibleTypeTraits {

        };

        template<>
        struct VariantCompatibleTypeTraits<gimbal::Bool>: public VariantCompatibleTypeTraitsDefault<gimbal::Bool> {
            constexpr static auto variantType = VariantType(VariantType::Bool);
        };

        template<>
        struct VariantCompatibleTypeTraits<gimbal::Int>: public VariantCompatibleTypeTraitsDefault<gimbal::Int> {
            constexpr static auto variantType = VariantType(VariantType::Int);
        };

        template<>
        struct VariantCompatibleTypeTraits<gimbal::Float>: public VariantCompatibleTypeTraitsDefault<gimbal::Float> {
            constexpr static auto variantType = VariantType(VariantType::Float);
        };

        template<>
        struct VariantCompatibleTypeTraits<void*>: public VariantCompatibleTypeTraitsDefault<void*> {
            constexpr static auto variantType = VariantType(VariantType::VoidPtr);
        };
#if 0
        template<typename S>
        requires std::is_same_v<std::decay_t<S>, const GblString*>
        struct VariantCompatibleTypeTraits<S>: public VariantCompatibleTypeTraitsDefault<S> {
            constexpr static auto variantType = VariantType(VariantType::String);
        };
#endif

        /* greater truths for type compat
        // 1) constructors
                a. implement main constructor using GblTypes: const GblType&, GblType&&
                b. do NOT delete default constructors, you have to static cast to base and defer them
            2) delete derived assignment operators
                a. implement assignment by using base type
                b. use a single perfect-forwarded function to avoid ambiguity, BUT
                                template<typename V>
                                requires std::is_convertible_v<V, GblVariant>
                                Variant& operator=(V&& rhs) {
                                    if constexpr(std::is_lvalue_reference_v<decltype(rhs)>) {
                                        Exception::checkThrow(gblVariantCopy(this, &rhs));
                                    } else if constexpr(std::is_rvalue_reference_v<decltype(rhs)>) {
                                        Exception::checkThrow(gblVariantMove(this, &rhs));
                                    }
                                    return *this;
                                }
                    make sure to constrain it so that it isn't instantiating on other nonrelated shit.

            3) Dereest the type traitsy shit
        */
    class Variant: public GblVariant {
    public:
        using Type = VariantType;

        GBL_ENUM_TABLE_DECLARE_CPP(GBL_META_VARIANT_OP_CMP_TYPE_TABLE);
        using OpCmpType = VariantOpCmpType;

        Variant(void) { Exception::checkThrow(gblVariantConstruct(this)); }

        template<typename V>
            requires (!std::is_convertible_v<V, GblVariant>)
        constexpr Variant(V arg) { Exception::checkThrow(gblVariantConstruct(this, VariantCompatibleTypeTraits<V>::Forwarder::forward(arg))); }


        Variant(const Variant& rhs):
            Variant(static_cast<const GblVariant&>(rhs)) {}
        Variant(Variant&& rhs):
            Variant(static_cast<GblVariant&&>(std::move(rhs))) {}

        template<typename V>
            requires std::is_convertible_v<V, GblVariant>
        Variant(V&& rhs) {
            if constexpr(std::is_lvalue_reference_v<decltype(rhs)>) {
                Exception::checkThrow(gblVariantConstruct(this, static_cast<const GblVariant*>(&rhs)));
            } else if constexpr(std::is_rvalue_reference_v<decltype(rhs)>) {
                Exception::checkThrow(gblVariantConstructMove(this, static_cast<GblVariant*>(&rhs)));
            }
        }

        ~Variant(void) { Exception::checkThrow(gblVariantDestruct(this)); }

        constexpr Type getType(void) const noexcept { return type; }

        template<typename T>
        constexpr T getValue(void) const {
            T value{};
            Exception::checkThrow(gblVariantGet(this, &value));
            return value;
        }

        template<typename T>
        T toValue(void) const {
            T value{};
            Exception::checkThrow(gblVariantTo(this, &value));
            return value;
        }

        void setValue(auto value) { Exception::checkThrow(gblVariantSet(this, value)); }

        // value type assignments
        template<typename V>
            requires (!std::is_convertible_v<V, GblVariant>)
        Variant& operator=(V value) {
            setValue(value);
            return *this;
        }
        // copy/move assignments with GblVariant/Variant
        template<typename V>
        requires std::is_convertible_v<V, GblVariant>
        Variant& operator=(V&& rhs) {
            if constexpr(std::is_lvalue_reference_v<decltype(rhs)>) {
                Exception::checkThrow(gblVariantCopy(this, &rhs));
            } else if constexpr(std::is_rvalue_reference_v<decltype(rhs)>) {
                Exception::checkThrow(gblVariantMove(this, &rhs));
            }
            return *this;
        }
        // force Variant to use GblVariant assignment operators
        Variant& operator=(Variant&& rhs) = delete;
        Variant& operator=(const Variant& rhs) = delete;

        template<typename V>
        requires std::is_convertible_v<V, GblVariant>
        static bool compare(const GblVariant& lhs, V&& rhs, OpCmpType compOp) {
            gimbal::Bool value = false;
            Exception::checkThrow(gblVariantCompare(&lhs, static_cast<const GblVariant*>(&rhs), compOp, &value));
            return value;
        }

        template<typename V>
        requires (!std::is_convertible_v<V, GblVariant>)
        static bool compare(const GblVariant& lhs, V rhs, OpCmpType compOp) {
            gimbal::Bool value = false;
            Exception::checkThrow(gblVariantCompare(&lhs, rhs, compOp, &value));
            return value;
        }

        friend bool operator==  (const GblVariant& lhs, auto rhs) { return compare(lhs, rhs, OpCmpType::Equal); }
        friend bool operator!=  (const GblVariant& lhs, auto rhs) { return compare(lhs, rhs, OpCmpType::NotEqual); }
        friend bool operator<   (const GblVariant& lhs, auto rhs) { return compare(lhs, rhs, OpCmpType::Less); }
        friend bool operator<=  (const GblVariant& lhs, auto rhs) { return compare(lhs, rhs, OpCmpType::LessOrEqual); }
        friend bool operator>   (const GblVariant& lhs, auto rhs) { return compare(lhs, rhs, OpCmpType::Greater); }
        friend bool operator>=  (const GblVariant& lhs, auto rhs) { return compare(lhs, rhs, OpCmpType::GreaterOrEqual); }
    };

    static_assert(sizeof(Variant) == sizeof(GblVariant), "C and C++ types are not the same size!");







}

#endif // GIMBAL_VARIANT_HPP

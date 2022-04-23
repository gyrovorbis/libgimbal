#ifndef GIMBAL_VARIANT_HPP
#define GIMBAL_VARIANT_HPP


#include "gimbal_variant.h"
#include "gimbal_typedefs.hpp"
#include "gimbal_string.hpp"
#include "../meta/gimbal_type.hpp"

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



template<typename T>
struct VariantCompatibleTypeTraitsDefault {
    using Type = T;
    using ThisType = VariantCompatibleTypeTraitsDefault<T>;
    using ForwardType = T;
    struct Forwarder {
        static ForwardType forward(Type value) { return static_cast<ForwardType>(value); }
    };

    inline const static auto variantType  = NilType();

};


template<typename T, typename=void>
struct VariantCompatibleTypeTraits: public VariantCompatibleTypeTraitsDefault<T> {

};


template<typename T>
concept variant_compatible =
        requires(T t) {
            VariantCompatibleTypeTraits<T>::variantType != NilType();
        };

        template<typename Class>
            requires requires { typename Class::VariantCompatibleTypeTraits; }
        struct VariantCompatibleTypeTraits<Class>: public Class::VariantCompatibleTypeTraits {

        };

        template<>
        struct VariantCompatibleTypeTraits<gimbal::Bool>: public VariantCompatibleTypeTraitsDefault<gimbal::Bool> {
            inline static auto variantType  = BoolType();
        };

        template<>
        struct VariantCompatibleTypeTraits<gimbal::Int>: public VariantCompatibleTypeTraitsDefault<gimbal::Int> {
            inline const static auto variantType  = Int32Type();
        };

        template<>
        struct VariantCompatibleTypeTraits<gimbal::UInt>: public VariantCompatibleTypeTraitsDefault<gimbal::UInt> {
            inline const static auto variantType  = Int32Type();
        };

        template<>
        struct VariantCompatibleTypeTraits<gimbal::Float>: public VariantCompatibleTypeTraitsDefault<gimbal::Float> {
            inline const static auto variantType  = FloatType();
        };

        template<>
        struct VariantCompatibleTypeTraits<void*>: public VariantCompatibleTypeTraitsDefault<void*> {
            inline const static auto variantType  = PointerType();
        };

        template<>
        struct VariantCompatibleTypeTraits<bool>: public VariantCompatibleTypeTraitsDefault<gimbal::Bool> {
            inline const static auto variantType  = BoolType();
        };

        template<>
        struct VariantCompatibleTypeTraits<GblString>: public VariantCompatibleTypeTraitsDefault<GblString> {
            inline const static auto variantType  = StringType();
            struct Forwarder {
                static const GblString* forward(const GblString& gblString) { return &gblString; }
            };
        };

        template<>
        struct VariantCompatibleTypeTraits<gimbal::String>: public VariantCompatibleTypeTraitsDefault<gimbal::String> {
            inline const static auto variantType  = StringType();

            struct Forwarder {
                static const GblString* forward(const gimbal::String& string) { return &string; }
            };
        };

#if 0
        template<typename S>
        requires std::same_as<std::decay_t<S>, const GblString*>
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
        // GIVE A CONSTRUCTOR THAT JUST TAKES A TYPE ID
     //   using Type = VariantType;

        GBL_ENUM_TABLE_DECLARE_CPP(GBL_META_VARIANT_OP_CMP_TYPE_TABLE);
        using OpCmpType = VariantOpCmpType;

        Variant(void) { Exception::checkThrow(gblVariantConstruct(this)); }

        Variant(Type type){
            Exception::checkThrow(gblVariantConstructt(this, type));
        }

        Variant(std::nullptr_t):
            Variant(NilType()) {}

        template<typename V>
            requires (!std::is_convertible_v<V, GblVariant> && !std::is_convertible_v<V, Type> && !std::is_convertible_v<V, GblString>)
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
#if 1
        template<typename V>
            requires std::is_convertible_v<V, GblString>
        Variant(V&& rhs) {
            if constexpr(std::is_lvalue_reference_v<decltype(rhs)>) {
                Exception::checkThrow(gblVariantConstruct(this, static_cast<const GblString*>(&rhs)));
            } else if constexpr(std::is_rvalue_reference_v<decltype(rhs)>) {
                Exception::checkThrow(gblVariantConstructt(this, StringType()));
                Exception::checkThrow(gblVariantMoves(this, static_cast<GblString*>(&rhs)));
            }
        }
#endif

        ~Variant(void) { Exception::checkThrow(gblVariantDestruct(this)); }

         Type getType(void) const noexcept { return type; }

        template<typename T>
        constexpr T getValue(void) const {
            T value{};
            Exception::checkThrow(gblVariantGet(this, &value));
            return value;
        }

        template<typename T>
        T toValue(void) const {
            T value{};
            toValue(&value);
            return value;
        }

        template<typename T>
        void toValue(T* pPtr) const {
            Exception::checkThrow(gblVariantTo(this, pPtr));
        }

        void setValue(auto value) { Exception::checkThrow(gblVariantSet(this, VariantCompatibleTypeTraits<decltype(value)>::Forwarder::forward(value))); }

        // value type assignments
        template<typename V>
            requires (!std::is_convertible_v<V, GblVariant> && !std::is_convertible_v<V, GblString>)
        Variant& operator=(V value) {
            setValue(value);
            return *this;
        }

#if 1
        template<typename V>
            requires std::is_convertible_v<V, GblString>
        Variant& operator=(V&& string) {
            if constexpr(std::is_lvalue_reference_v<decltype(string)>) {
                Exception::checkThrow(gblVariantSet(this, static_cast<const GblString*>(&string)));
            } else if constexpr(std::is_rvalue_reference_v<decltype(string)>) {
                Exception::checkThrow(gblVariantMove(this, &string));
            }
            return *this;
        }
#endif

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

        Variant& operator=(std::nullptr_t) {
            Exception::checkThrow(gblVariantSetNil(this));
            return *this;
        }

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
            Exception::checkThrow(gblVariantCompare(&lhs, VariantCompatibleTypeTraits<V>::Forwarder::forward(rhs), compOp, &value));
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

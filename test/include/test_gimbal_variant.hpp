#ifndef VARIANT_TESTS_HPP
#define VARIANT_TESTS_HPP

#include "test_gimbal.hpp"
#include <gimbal/types/gimbal_variant.hpp>
#include <gimbal/meta/gimbal_type.hpp>

namespace gimbal::test {

// test converting from string to numbers and other shit
// add that functionality to GblString
class Variant: public UnitTestSet {
    Q_OBJECT
    struct Cuntass {
        GblInt i;
        struct VariantCompatibleTypeTraits: public VariantCompatibleTypeTraitsDefault<Cuntass> {
            constexpr static auto variantType  = VariantType(VariantType::Int);
            struct Forwarder {
                static GblInt forward(const Cuntass& cunt) { return cunt.i; }
            };
        };
    };


private slots:
    void testString(void) {
        const auto str = gimbal::String("test", pCtx());

        gimbal::Variant v(static_cast<const GblString*>(&str));

        QCOMPARE(v.getType(), StringType());

        gimbal::StringView strView = v.getValue<const GblString*>();
        QCOMPARE(strView.getCString(), "test");

        gimbal::String string = v.toValue<GblString>();
        QCOMPARE(strView.getCString(), string.getCString());

        gimbal::Variant v2(v);

        QCOMPARE(v, v2);
        QVERIFY(v <= v2);
        QVERIFY(v >= v2);
        QVERIFY(!(v != v2));
        QVERIFY(!(v < v2));
        QVERIFY(!(v > v2));


        auto stringifyTest = [&](auto&& construct, gimbal::String expected=nullptr) {
            gimbal::Variant sv(std::forward<decltype(construct)>(construct));
            gimbal::String result = sv.toValue<GblString>();
            if(!expected.isEmpty()) QCOMPARE(result.getCString(), expected.getCString());
       };

       stringifyTest(-33.0f, "-33.000");
       stringifyTest(gimbal::Variant(), "nil");
       const char* pCStr = "Some heapy-ass mcHeapery String from fucking hell with all the heap allocs";
       const auto strLong = gimbal::String(pCStr, pCtx());
       stringifyTest(static_cast<const GblString*>(&strLong), gimbal::String(pCStr));
       stringifyTest((GblBool)GBL_FALSE, "false");
       stringifyTest(128, "128");
       stringifyTest(128u, "128");
       stringifyTest(static_cast<void*>(this));
        auto str3 = gimbal::String("Stringificator");
       //gimbal::Variant v3(str3);

        auto testStrVariant = [&]<typename T>(gimbal::String string, T expectedVal) {
            gimbal::Variant v(static_cast<const GblString*>(&string));
            gimbal::Variant v2(std::forward<decltype(expectedVal)>(expectedVal));
            QCOMPARE(v, v2);
            auto blk = GBL_API_TRY_BLOCK {
                auto value = v.toValue<T>();
                QCOMPARE(value, expectedVal);
            };
            QVERIFY(blk.getResult().isSuccess());
        };

       testStrVariant("false", GblBool(GBL_FALSE));
       testStrVariant("true", GblBool(GBL_TRUE));
       testStrVariant("-1234", -1234);
       testStrVariant("-36", -36);
       gimbal::String nstr("nil");
       gimbal::Variant nv(static_cast<const GblString*>(&nstr));
       QCOMPARE(nv, gimbal::Variant());
       //sketchier than all fuck!
       testStrVariant("0.000", 0.0f);
       testStrVariant("-33.400", -33.4f);
       //testStrVariant("0.100000", 0.1f);
    }


    void construct(void) {
        auto test = [&](auto arg) {
            gimbal::Variant v(arg);
            QCOMPARE(v.getType(), gimbal::VariantCompatibleTypeTraits<decltype(arg)>::variantType);
        };

        gimbal::Variant v1;
        QCOMPARE(v1.getType(), gimbal::VariantCompatibleTypeTraits<std::nullptr_t>::variantType);
        test(12);
        test(-33.0f);
        test(GblBool(true));
        test(reinterpret_cast<void*>(this));
        const auto str = gimbal::String("test");
    }

    void constructCopy(void) {
        GblVariant g1{.floating=-33.0f, .type=GBL_TYPE_FLOAT};
        gimbal::Variant v1(g1);
        QCOMPARE(v1.getType(), gimbal::FloatType());

        gimbal::Variant v2(34);
        gimbal::Variant v3(v2);
        QCOMPARE(v3.getType(), gimbal::Int32Type());
    }

    void constructMove(void) {
        gimbal::Variant v1(GblVariant{.floating=-33.0f, .type=GBL_TYPE_FLOAT});
        QCOMPARE(v1.getType(), gimbal::FloatType());

        gimbal::Variant v2(gimbal::Variant(34));
        QCOMPARE(v2.getType(), gimbal::Int32Type());
    }

    void getValue(void) {
        // test case
        auto test = [&](auto construct, auto value) {
            // verify getValue() works with the expected type
            gimbal::Variant v(construct);

            auto get = [&](auto result) {

                auto blk = GBL_API_TRY_BLOCK {
                    result = v.getValue<decltype(result)>();
                };

                if constexpr(std::same_as<decltype(result), std::decay_t<decltype(value)>>) {
                    QVERIFY(blk.getResult().isSuccess());
                    QCOMPARE(result, value);
                } else {
                    QVERIFY(blk.getResult() == Result::ErrorTypeMismatch);
                }
            };
            get(12);
            get(-33.0f);
            get(GblBool(false));
            get(reinterpret_cast<void*>(this));
        };

        test(12, 12);
        test(-33.0f, -33.0f);
        test(GblBool(false), gimbal::Bool(GBL_FALSE));
        test(reinterpret_cast<void*>(this), reinterpret_cast<void*>(this));
        auto g = GblVariant{.boolean=GBL_TRUE, .type=GBL_TYPE_BOOL};
        test(g, GblBool(GBL_TRUE));
        test(std::move(g), gimbal::Bool(true));
        auto v = gimbal::Variant(reinterpret_cast<void*>(this));
        test(v, reinterpret_cast<void*>(this));
        test(std::move(v), reinterpret_cast<void*>(this));


    }

    void toValue(void) {


    }

    void comparison(void) {
        // test case
        auto test = [&](auto construct) {
            // verify getValue() works with the expected type
            gimbal::Variant v(construct);

            auto cmp = [&](gimbal::Variant::OpCmpType cmp, auto&& rhs) {
                switch(cmp.getValue()) {
                case gimbal::Variant::OpCmpType::Equal:          return (v == std::forward<decltype(rhs)>(rhs));
                case gimbal::Variant::OpCmpType::NotEqual:       return (v != std::forward<decltype(rhs)>(rhs));
                case gimbal::Variant::OpCmpType::Less:           return (v < rhs);
                case gimbal::Variant::OpCmpType::LessOrEqual:    return (v <= rhs);
                case gimbal::Variant::OpCmpType::Greater:        return (v > rhs);
                case gimbal::Variant::OpCmpType::GreaterOrEqual: return (v >= rhs);
                default: return false;
                }
                return false;
            };

            QVERIFY(cmp(gimbal::Variant::OpCmpType::Equal, construct));
            QVERIFY(cmp(gimbal::Variant::OpCmpType::Equal, gimbal::Variant(construct)));

        };

        test(gimbal::Variant());
        test(12);
        test(-33.0f);
        test(GblBool(false));
        test(reinterpret_cast<void*>(this));
        test(gimbal::Variant(GBL_TRUE));
        test(false);
        test("char array");
        test(gimbal::String("Fuck you"));
        GblString gblStr;
        gblStringConstruct(&gblStr, sizeof(GblString), hCtx(), gimbal::constptr_cast(GblStringView{"Lulz", 0}));
        test(gblStr);
        gblStringDestruct(&gblStr);
        gimbal::Variant g((char*)"lulzor");
        test(std::move(g));
        auto v = gimbal::Variant(reinterpret_cast<void*>(this));
        test(v);
        test(std::move(v));
    }


    void setValue(void) {
        auto test = [&](auto construct, auto arg) {
            // verify getValue() works with the expected type
            gimbal::Variant v(construct);

            v.setValue(arg);
            QCOMPARE(v.getValue<decltype(arg)>(), arg);
        };

        test(12, 12);
        test(-33.0f, -33.0f);
        test(GblBool(false), gimbal::Bool(GBL_FALSE));
        test(reinterpret_cast<void*>(this), reinterpret_cast<void*>(this));
        auto g = GblVariant{.boolean=GBL_TRUE, .type=GBL_TYPE_BOOL};
        test(g, GblBool(GBL_TRUE));
        test(std::move(g), gimbal::Bool(true));
        auto v = gimbal::Variant(reinterpret_cast<void*>(this));
        test(v, reinterpret_cast<void*>(this));
        test(std::move(v), reinterpret_cast<void*>(this));
    }

    void assignment(void) {
        auto test = [&](auto construct, auto arg) {
            // verify getValue() works with the expected type
            gimbal::Variant v(construct);

            v = arg;
            QVERIFY(v == arg);
        };

        test(12, 14352);
        test(-33.0f, gimbal::Variant());
        test(GblBool(false), gimbal::Bool(GBL_TRUE));
        test(true, true);
        test(GBL_FALSE, GBL_FALSE);
        test(reinterpret_cast<void*>(this), reinterpret_cast<void*>(this));
        auto g = GblVariant{.boolean=GBL_TRUE, .type=GBL_TYPE_BOOL};
        test(g, -35433.0f);
        test("fucktwix", "fucktwix");
        test(gimbal::String(std::string("lul")), gimbal::String("lul"));
        test(std::move(g), gimbal::Bool(true));
        auto v = gimbal::Variant(reinterpret_cast<void*>(this));
        test(v, reinterpret_cast<void*>(this));
        test(std::move(v), gimbal::Variant(gimbal::Variant(3553)));
    }







};




}

#endif // VARIANT_TESTS_HPP

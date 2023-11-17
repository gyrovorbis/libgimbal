#include "strings/gimbal_string_view_test_suite.hpp"
#include <gimbal/strings/gimbal_string_view.hpp>
#include <ranges>

#define GBL_SELF_TYPE GblStringViewTestSuiteCpp

GBL_TEST_FIXTURE {
    uintptr_t padding;
};

GBL_TEST_INIT()
GBL_TEST_CASE_END

GBL_TEST_FINAL()
GBL_TEST_CASE_END

GBL_TEST_CASE(constructDefault)
    auto strv = gbl::StringView{};
    GBL_TEST_VERIFY(strv.empty());
    GBL_TEST_VERIFY(strv.data() == nullptr);
    GBL_TEST_VERIFY(strv.length() == 0);
    GBL_TEST_VERIFY(GblStringView_empty(strv));
    GBL_TEST_VERIFY(strv == GblStringView_fromEmpty());
GBL_TEST_CASE_END

GBL_TEST_CASE(constructCString)
    auto strv = gbl::StringView { "lolzer" };
    GBL_TEST_VERIFY(!strv.empty());
    GBL_TEST_VERIFY(strv.length() == 6);
    GBL_TEST_VERIFY(strv.isNullTerminated());
    GBL_TEST_VERIFY(strv.first() == 'l');
    GBL_TEST_VERIFY(strv.last() == 'r');
    GBL_TEST_VERIFY(strv[2] == 'l');
    GBL_TEST_VERIFY(strv == "lolzer");
    GBL_TEST_VERIFY(strv == std::string_view { "lolzer" });
GBL_TEST_CASE_END

GBL_TEST_CASE(constructGblStringView)
    auto strv = gbl::StringView { GblStringView_fromString("lolzer") };
    GBL_TEST_VERIFY(!GblStringView_empty(strv));
    GBL_TEST_VERIFY(GblStringView_at(strv, 1) == 'o');
    GBL_TEST_VERIFY(GblStringView_last(strv) == 'r');
    GBL_TEST_VERIFY(GblStringView_contains(strv, "lol"));
    GBL_TEST_VERIFY(strv.contains("lol"));
GBL_TEST_CASE_END

GBL_TEST_CASE(customLiteral)
    using namespace gbl;
    static_assert("lolzer"_strv[2] == 'l', "lolwtf");
GBL_TEST_CASE_END

GBL_TEST_CASE(stdString)
    auto strv = gbl::StringView { "lolzercopter" };
    auto str = strv.toStdString();
    auto strv2 = gbl::StringView { "lulz" };

    GBL_TEST_VERIFY(strv == str);

    std::swap(strv, strv2);
    GBL_TEST_VERIFY(strv == "lulz");
    GBL_TEST_VERIFY(strv2 == std::string("lolzercopter"));

    strv = strv2;
    GBL_TEST_VERIFY(strv >= strv2);
GBL_TEST_CASE_END

struct DummyType {
    int val = 0;
};

template<>
inline std::optional<DummyType> gbl::string_view_to_value<DummyType>(const StringView& view) {
    auto result = view.toValue<int32_t>();

    if(result.has_value())
        return std::optional {DummyType{result.value()}};
    else
        return std::nullopt;
}

GBL_TEST_CASE(conversions)
    GBL_TEST_VERIFY(gbl::StringView{"true"}.toValue<bool>() == true);
    GBL_TEST_VERIFY(gbl::StringView{"FALSE"}.toValue<bool>() == false);
    GBL_TEST_VERIFY(gbl::StringView{"F"}.toValue<char>() == 'F');
    GBL_TEST_VERIFY(!gbl::StringView{"256"}.toValue<uint8_t>().has_value());
    GBL_TEST_VERIFY(gbl::StringView{"244"}.toValue<uint8_t>() == 244);
    GBL_TEST_VERIFY(!gbl::StringView{"-256"}.toValue<uint16_t>().has_value());
    GBL_TEST_VERIFY(gbl::StringView{"278"}.toValue<uint16_t>() == 278);
    GBL_TEST_VERIFY(gbl::StringView{"-256"}.toValue<int16_t>() == -256);
    GBL_TEST_VERIFY(gbl::StringView{"32769"}.toValue<uint32_t>() == 32769);
    GBL_TEST_VERIFY(gbl::StringView{"0xabc"}.toValue<uint32_t>() == 0xabc);
    GBL_TEST_VERIFY(gbl::StringView{"-32769"}.toValue<int32_t>() == -32769);
    GBL_TEST_VERIFY(gbl::StringView("18446744073709551615").toValue<uint64_t>() == 18446744073709551615llu);
    GBL_TEST_VERIFY(gbl::StringView("-9223372036854775807").toValue<int64_t>() == -9223372036854775807ll);
    GBL_TEST_VERIFY(gbl::StringView("-7.812").toValue<float>() == -7.812f);
    GBL_TEST_VERIFY(gbl::StringView("-734.81223").toValue<double>() == -734.81223);
    GBL_TEST_VERIFY(gbl::StringView("0xdeadbabe").toValue<void*>() == reinterpret_cast<void*>(0xdeadbabe));
    GBL_TEST_VERIFY(gbl::StringView("77").toValue<DummyType>()->val == 77);
GBL_TEST_CASE_END

GBL_TEST_CASE(iterators)
    auto strv = gbl::StringView {"Lehmer"};

    // Test range-based for using standard iterators
    size_t i = 0;
    for(auto c : strv) {
        GBL_TEST_VERIFY(c == strv[i++]);
    }

    // test reverse iterators
    i = strv.length() - 1;
    for(auto it = strv.rbegin(); it != strv.rend(); ++it) {
        GBL_TEST_VERIFY(*it == strv[i--]);
    }

    // test ranges using iterator implementation
    GBL_TEST_VERIFY(std::distance(strv.begin(), std::ranges::find(strv, 'm')) == strv.find("m"));
GBL_TEST_CASE_END

GBL_TEST_REGISTER(constructDefault,
                  constructCString,
                  constructGblStringView,
                  customLiteral,
                  stdString,
                  conversions,
                  iterators);

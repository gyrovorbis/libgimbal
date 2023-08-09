#include "strings/gimbal_pattern_test_suite.hpp"
#include <gimbal/strings/gimbal_pattern.hpp>

#define GBL_SELF_TYPE GblPatternTestSuiteCpp

GBL_TEST_FIXTURE {
    std::size_t totalPatterns;
};

GBL_TEST_INIT()
    pFixture->totalPatterns = GblPattern_totalCount();
GBL_TEST_CASE_END

GBL_TEST_FINAL()
    GBL_TEST_VERIFY(GblPattern_totalCount() == pFixture->totalPatterns);
GBL_TEST_CASE_END

GBL_TEST_CASE(constructString)
    auto pat = gbl::Pattern("^\\d{3}[-.]?\\d{3}[-.]?\\d{4}$");
    //GBL_TEST_COMPARE(pat.refCount(), 1);
    GBL_TEST_VERIFY(pat.isValid());
    GBL_TEST_VERIFY(pat);
    GBL_TEST_VERIFY(pat == pat);
GBL_TEST_CASE_END

GBL_TEST_CASE(constructGblPattern)
    const GblPattern* pPattern = GblPattern_create("^\\d{3}[-.]?\\d{3}[-.]?\\d{4}$");
    auto pat = gbl::Pattern { pPattern };
    GBL_TEST_VERIFY(pat == pPattern);
GBL_TEST_CASE_END

GBL_TEST_CASE(constructCopy)
    auto pat = gbl::Pattern { "^\\d{3}[-.]?\\d{3}[-.]?\\d{4}$" };
    {
        auto pat2 = pat;

        GBL_TEST_VERIFY(pat == pat2);
        GBL_TEST_VERIFY(pat.refCount() == 2);
    }
    GBL_TEST_VERIFY(pat.refCount() == 1);
GBL_TEST_CASE_END

GBL_TEST_CASE(constructMove)
    const auto activeCount = GblPattern_totalCount();
    {
        auto pat = gbl::Pattern { "^\\d{3}[-.]?\\d{3}[-.]?\\d{4}$" };
        {
            auto pat2 = std::move(pat);

            GBL_TEST_VERIFY(pat2.refCount() == 1);
        }
    }
    GBL_TEST_VERIFY(GblPattern_totalCount() == activeCount);
GBL_TEST_CASE_END

GBL_TEST_CASE(bytes)
    const auto pat = gbl::Pattern { "lolz" };
    GBL_TEST_VERIFY(pat.bytes() == GblPattern_bytes(pat));
GBL_TEST_CASE_END

GBL_TEST_CASE(string)
    const auto pat = gbl::Pattern { "lolz" };
    GBL_TEST_VERIFY(pat.string() == "lolz");
GBL_TEST_CASE_END

GBL_TEST_REGISTER(constructString,
                  constructGblPattern,
                  constructCopy,
                  constructMove,
                  bytes,
                  string)

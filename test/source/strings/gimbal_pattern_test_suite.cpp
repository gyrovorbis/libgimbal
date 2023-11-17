#include "strings/gimbal_pattern_test_suite.hpp"
#include <gimbal/strings/gimbal_pattern.hpp>
#include <regex>
#include <gimbal/core/gimbal_error.h>

#define BENCHMARK_ITERATIONS_   1024

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

GBL_TEST_CASE(benchmarkCpp)
    const auto pat = std::regex {"(a(b(cd))e)"};

    for(size_t i = 0; i < BENCHMARK_ITERATIONS_; ++i)
        GBL_TEST_VERIFY(std::regex_search("abcde", pat));
GBL_TEST_CASE_END

GBL_TEST_CASE(benchmarkGbl)
    const auto pat = gbl::Pattern { "(a(b(cd))e)" };

    for(size_t i = 0; i < BENCHMARK_ITERATIONS_; ++i)
        GBL_TEST_VERIFY(pat.match("abcde"));
GBL_TEST_CASE_END

GBL_RESULT ctxCall_() {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY(GBL_FALSE,
                   GBL_RESULT_ERROR_INTERNAL,
                   "Test");
    GBL_CTX_END();
}

GBL_TEST_CASE(ctxBenchmark)
    GBL_TEST_EXPECT_ERROR();

    for(size_t i = 0; i < BENCHMARK_ITERATIONS_; ++i)
        ctxCall_();

    GBL_CTX_CLEAR_LAST_RECORD();
GBL_TEST_CASE_END

GBL_TEST_CASE(errorBenchmark)
    for(size_t i = 0; i < BENCHMARK_ITERATIONS_; ++i) {
        GblError* pError = GblError_create(GBL_ERROR_TYPE, GBL_ENUM_TYPE, GBL_RESULT_ERROR_INTERNAL, "Test");
        GblError_throw(pError);
    }
    GblError_clear();
GBL_TEST_CASE_END

GBL_TEST_CASE(exceptionBenchmark)
    for(size_t i = 0; i < BENCHMARK_ITERATIONS_; ++i) {
        try {
            throw std::out_of_range("Test");
        } catch(std::exception& except) {
            ;
        }
    }
GBL_TEST_CASE_END

GBL_TEST_REGISTER(constructString,
                  constructGblPattern,
                  constructCopy,
                  constructMove,
                  bytes,
                  string,
                  benchmarkCpp,
                  benchmarkGbl,
                  ctxBenchmark,
                  errorBenchmark,
                  exceptionBenchmark)

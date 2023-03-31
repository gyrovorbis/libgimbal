#include "strings/gimbal_pattern_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/strings/gimbal_pattern.h>

#define GBL_TEST_SUITE_SELF GblPatternTestSuite

GBL_TEST_FIXTURE {
    int padding;
};

GBL_TEST_INIT()
GBL_TEST_CASE_END

GBL_TEST_FINAL()
GBL_TEST_CASE_END

static GBL_RESULT GblPatternTestSuite_matchInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();
    GBL_TEST_COMPARE(GblPattern_firstMatch(NULL, "lol"), -1);
    GBL_TEST_COMPARE(GblPattern_firstMatch("lol", NULL), -1);
    GBL_TEST_COMPARE(GblPattern_firstMatch(NULL, NULL), -1);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_CTX_END();
}

static GBL_RESULT GblPatternTestSuite_matchExact_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_VERIFY(GblPattern_firstMatch("lol", "lol") >= 0);
    GBL_TEST_VERIFY(GblPattern_firstMatch("lol", "abc") == -1);
    GBL_TEST_VERIFY(GblPattern_firstMatch("lol", "loll") >= 0);
    GBL_TEST_VERIFY(GblPattern_firstMatch("lol", "llol") >= 0);
    GBL_CTX_END();
}

static GBL_RESULT GblPatternTestSuite_matchBegin_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_VERIFY(GblPattern_firstMatch("^lol", "lol") >= 0);
    GBL_TEST_VERIFY(GblPattern_firstMatch("^lol", "abc") == -1);
    GBL_TEST_VERIFY(GblPattern_firstMatch("^lol", "loll") >= 0);
    GBL_TEST_VERIFY(GblPattern_firstMatch("^lol", "llol") == -1);
    GBL_CTX_END();
}

static GBL_RESULT GblPatternTestSuite_matchEnd_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_VERIFY(GblPattern_firstMatch("lol$", "lol") >= 0);
    GBL_TEST_VERIFY(GblPattern_firstMatch("lol$", "abc") == -1);
    GBL_TEST_VERIFY(GblPattern_firstMatch("lol$", "loll") == -1);
    GBL_TEST_VERIFY(GblPattern_firstMatch("lol$", "llol") >= 0);
    GBL_CTX_END();
}

static GBL_RESULT GblPatternTestSuite_matchAnyChar_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_VERIFY(GblPattern_firstMatch("...", "lol") >= 0);
    GBL_TEST_VERIFY(GblPattern_firstMatch("..", "a") == -1);
    GBL_TEST_VERIFY(GblPattern_firstMatch("....", "loll") >= 0);
    GBL_TEST_VERIFY(GblPattern_firstMatch(".", "a") >= 0);
    GBL_TEST_VERIFY(GblPattern_firstMatch(".", "") == -1);
    GBL_CTX_END();
}


static GBL_RESULT GblPatternTestSuite_matchStar_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_TEST_SKIP("REWORK!");

    GBL_TEST_VERIFY(GblPattern_firstMatch("l*", "lol"));
    GBL_TEST_VERIFY(GblPattern_firstMatch("a*", "a"));
    GBL_TEST_VERIFY(!GblPattern_firstMatch("*", "loll"));
    GBL_TEST_VERIFY(GblPattern_firstMatch("b*", "a"));
    GBL_TEST_VERIFY(GblPattern_firstMatch("b*", "bbbbbbbbbbbbbb"));
    GBL_TEST_VERIFY(!GblPattern_firstMatch("*", ""));
    GBL_TEST_VERIFY(GblPattern_firstMatch("b*", ""));
    GBL_CTX_END();
}

static GBL_RESULT GblPatternTestSuite_matchPlus_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_VERIFY(GblPattern_firstMatch("l+", "lol") >= 0);
    GBL_TEST_VERIFY(GblPattern_firstMatch("a+", "a") >= 0);
    GBL_TEST_VERIFY(GblPattern_firstMatch("+", "loll") == -1);
    GBL_TEST_VERIFY(GblPattern_firstMatch("b+", "a") == -1);
    GBL_TEST_VERIFY(GblPattern_firstMatch("b+", "bbbbbbbbbbbbbb") >= 0);
    GBL_TEST_VERIFY(GblPattern_firstMatch("+", "") == -1);
    GBL_TEST_VERIFY(GblPattern_firstMatch("b+", "") == -1);
    GBL_CTX_END();
}

static GBL_RESULT GblPatternTestSuite_matchQuestion_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_TEST_SKIP("REWORK!");

    GBL_TEST_VERIFY(GblPattern_firstMatch("l?", "lol"));
    GBL_TEST_VERIFY(GblPattern_firstMatch("a?", "a"));
    GBL_TEST_VERIFY(!GblPattern_firstMatch("?", "loll"));
    GBL_TEST_VERIFY(GblPattern_firstMatch("b?", "a"));
    GBL_TEST_VERIFY(GblPattern_firstMatch("b?", "bbbbb"));
    GBL_TEST_VERIFY(!GblPattern_firstMatch("^b?$", "bbbbb"));
    GBL_TEST_VERIFY(!GblPattern_firstMatch("?", ""));
    GBL_TEST_VERIFY(GblPattern_firstMatch("b?", ""));
    GBL_CTX_END();
}

static GBL_RESULT GblPatternTestSuite_matchComplex_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);

    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_SKIP("REWORK!");

    GBL_TEST_VERIFY(GblPattern_firstMatch("^l.*l$", "ll"));
    GBL_TEST_VERIFY(GblPattern_firstMatch("^l.*l$", "lol"));
    GBL_TEST_VERIFY(GblPattern_firstMatch("^l.*l$", "luuuuul"));
    GBL_TEST_VERIFY(GblPattern_firstMatch("^l.*l$", "laul"));
    GBL_TEST_VERIFY(!GblPattern_firstMatch("^l.*l$", "olol"));
    GBL_TEST_VERIFY(!GblPattern_firstMatch("^l.*l$", "lolo"));
    GBL_CTX_END();
}

GBL_TEST_CASE(iso8601BasicDate)
    int length = 0;
    GBL_TEST_COMPARE(GblPattern_firstMatch("^[0-9]{8}$", "12345678", &length), 0);
    GBL_TEST_COMPARE(length, 8);
GBL_TEST_CASE_END

GBL_TEST_CASE(iso8601)
    //int length = 0;
   // GblPattern* pPattern = GblPattern_compile("^\d{4}-\d\d-\d\dT\d\d:\d\d:\d\d(\.\d+)?(([+-]\d\d:\d\d)|Z)?$");
   // GBL_TEST_VERIFY(GblPattern_exactMatchc(pPattern, "2002-12-31T23:00:00+01:00"));
GBL_TEST_CASE_END


GBL_TEST_REGISTER(matchInvalid,
                  matchExact,
                  matchBegin,
                  matchEnd,
                  matchAnyChar,
                  matchStar,
                  matchPlus,
                  matchQuestion,
                  matchComplex,
                  iso8601BasicDate,
                  iso8601)

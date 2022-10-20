#include "strings/gimbal_pattern_matcher_test_suite.h"
#include <gimbal/test/gimbal_test.h>
#include <gimbal/strings/gimbal_pattern_matcher.h>

static GBL_RESULT GblPatternMatcherTestSuite_matchInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();
    GBL_TEST_VERIFY(!GblPatternMatcher_match(NULL, "lol"));
    GBL_TEST_VERIFY(!GblPatternMatcher_match("lol", NULL));
    GBL_TEST_VERIFY(!GblPatternMatcher_match(NULL, NULL));
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_CTX_END();
}

static GBL_RESULT GblPatternMatcherTestSuite_matchExact_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_VERIFY(GblPatternMatcher_match("lol", "lol"));
    GBL_TEST_VERIFY(!GblPatternMatcher_match("lol", "abc"));
    GBL_TEST_VERIFY(GblPatternMatcher_match("lol", "loll"));
    GBL_TEST_VERIFY(GblPatternMatcher_match("lol", "llol"));
    GBL_CTX_END();
}

static GBL_RESULT GblPatternMatcherTestSuite_matchBegin_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_VERIFY(GblPatternMatcher_match("^lol", "lol"));
    GBL_TEST_VERIFY(!GblPatternMatcher_match("^lol", "abc"));
    GBL_TEST_VERIFY(GblPatternMatcher_match("^lol", "loll"));
    GBL_TEST_VERIFY(!GblPatternMatcher_match("^lol", "llol"));
    GBL_CTX_END();
}

static GBL_RESULT GblPatternMatcherTestSuite_matchEnd_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_VERIFY(GblPatternMatcher_match("lol$", "lol"));
    GBL_TEST_VERIFY(!GblPatternMatcher_match("lol$", "abc"));
    GBL_TEST_VERIFY(!GblPatternMatcher_match("lol$", "loll"));
    GBL_TEST_VERIFY(GblPatternMatcher_match("lol$", "llol"));
    GBL_CTX_END();
}

static GBL_RESULT GblPatternMatcherTestSuite_matchAnyChar_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_VERIFY(GblPatternMatcher_match("...", "lol"));
    GBL_TEST_VERIFY(!GblPatternMatcher_match("..", "a"));
    GBL_TEST_VERIFY(GblPatternMatcher_match("....", "loll"));
    GBL_TEST_VERIFY(GblPatternMatcher_match(".", "a"));
    GBL_TEST_VERIFY(!GblPatternMatcher_match(".", ""));
    GBL_CTX_END();
}


static GBL_RESULT GblPatternMatcherTestSuite_matchStar_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_VERIFY(GblPatternMatcher_match("l*", "lol"));
    GBL_TEST_VERIFY(GblPatternMatcher_match("a*", "a"));
    GBL_TEST_VERIFY(!GblPatternMatcher_match("*", "loll"));
    GBL_TEST_VERIFY(GblPatternMatcher_match("b*", "a"));
    GBL_TEST_VERIFY(GblPatternMatcher_match("b*", "bbbbbbbbbbbbbb"));
    GBL_TEST_VERIFY(!GblPatternMatcher_match("*", ""));
    GBL_TEST_VERIFY(GblPatternMatcher_match("b*", ""));
    GBL_CTX_END();
}

static GBL_RESULT GblPatternMatcherTestSuite_matchPlus_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_VERIFY(GblPatternMatcher_match("l+", "lol"));
    GBL_TEST_VERIFY(GblPatternMatcher_match("a+", "a"));
    GBL_TEST_VERIFY(!GblPatternMatcher_match("+", "loll"));
    GBL_TEST_VERIFY(!GblPatternMatcher_match("b+", "a"));
    GBL_TEST_VERIFY(GblPatternMatcher_match("b+", "bbbbbbbbbbbbbb"));
    GBL_TEST_VERIFY(!GblPatternMatcher_match("+", ""));
    GBL_TEST_VERIFY(!GblPatternMatcher_match("b+", ""));
    GBL_CTX_END();
}

static GBL_RESULT GblPatternMatcherTestSuite_matchQuestion_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_VERIFY(GblPatternMatcher_match("l?", "lol"));
    GBL_TEST_VERIFY(GblPatternMatcher_match("a?", "a"));
    GBL_TEST_VERIFY(!GblPatternMatcher_match("?", "loll"));
    GBL_TEST_VERIFY(GblPatternMatcher_match("b?", "a"));
    GBL_TEST_VERIFY(GblPatternMatcher_match("b?", "bbbbb"));
    GBL_TEST_VERIFY(!GblPatternMatcher_match("^b?$", "bbbbb"));
    GBL_TEST_VERIFY(!GblPatternMatcher_match("?", ""));
    GBL_TEST_VERIFY(GblPatternMatcher_match("b?", ""));
    GBL_CTX_END();
}

static GBL_RESULT GblPatternMatcherTestSuite_matchComplex_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_VERIFY(GblPatternMatcher_match("^l.*l$", "ll"));
    GBL_TEST_VERIFY(GblPatternMatcher_match("^l.*l$", "lol"));
    GBL_TEST_VERIFY(GblPatternMatcher_match("^l.*l$", "luuuuul"));
    GBL_TEST_VERIFY(GblPatternMatcher_match("^l.*l$", "laul"));
    GBL_TEST_VERIFY(!GblPatternMatcher_match("^l.*l$", "olol"));
    GBL_TEST_VERIFY(!GblPatternMatcher_match("^l.*l$", "lolo"));
    GBL_CTX_END();
}


GBL_EXPORT GblType GblPatternMatcherTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "matchInvalid",       GblPatternMatcherTestSuite_matchInvalid_    },
        { "matchExact",         GblPatternMatcherTestSuite_matchExact_      },
        { "matchBegin",         GblPatternMatcherTestSuite_matchBegin_      },
        { "matchEnd",           GblPatternMatcherTestSuite_matchEnd_        },
        { "matchAnyChar",       GblPatternMatcherTestSuite_matchAnyChar_    },
        { "matchStar",          GblPatternMatcherTestSuite_matchStar_       },
        { "matchPlus",          GblPatternMatcherTestSuite_matchPlus_       },
        { "matchQuestion",      GblPatternMatcherTestSuite_matchQuestion_   },
        { "matchComplex",       GblPatternMatcherTestSuite_matchComplex_    },
        { NULL,                 NULL                                        }
    };

    const static GblTestSuiteClassVTable vTable = {
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("GblPatternMatcherTestSuite"),
                                     &vTable,
                                     sizeof(GblPatternMatcherTestSuite),
                                     0,
                                     GBL_TYPE_FLAGS_NONE);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }

    return type;
}

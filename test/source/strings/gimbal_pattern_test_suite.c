#include "strings/gimbal_pattern_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/strings/gimbal_pattern.h>

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


GBL_EXPORT GblType GblPatternTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "matchInvalid",       GblPatternTestSuite_matchInvalid_    },
        { "matchExact",         GblPatternTestSuite_matchExact_      },
        { "matchBegin",         GblPatternTestSuite_matchBegin_      },
        { "matchEnd",           GblPatternTestSuite_matchEnd_        },
        { "matchAnyChar",       GblPatternTestSuite_matchAnyChar_    },
        { "matchStar",          GblPatternTestSuite_matchStar_       },
        { "matchPlus",          GblPatternTestSuite_matchPlus_       },
        { "matchQuestion",      GblPatternTestSuite_matchQuestion_   },
        { "matchComplex",       GblPatternTestSuite_matchComplex_    },
        { NULL,                 NULL                                        }
    };

    const static GblTestSuiteClassVTable vTable = {
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("GblPatternTestSuite"),
                                     &vTable,
                                     sizeof(GblPatternTestSuite),
                                     0,
                                     GBL_TYPE_FLAGS_NONE);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }

    return type;
}

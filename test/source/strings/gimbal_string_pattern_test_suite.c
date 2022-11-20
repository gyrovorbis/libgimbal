#include "strings/gimbal_string_pattern_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/strings/gimbal_string_pattern.h>

static GBL_RESULT GblStringPatternTestSuite_matchInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();
    GBL_TEST_VERIFY(!GblStringPattern_match(NULL, "lol"));
    GBL_TEST_VERIFY(!GblStringPattern_match("lol", NULL));
    GBL_TEST_VERIFY(!GblStringPattern_match(NULL, NULL));
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_CTX_END();
}

static GBL_RESULT GblStringPatternTestSuite_matchExact_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_VERIFY(GblStringPattern_match("lol", "lol"));
    GBL_TEST_VERIFY(!GblStringPattern_match("lol", "abc"));
    GBL_TEST_VERIFY(GblStringPattern_match("lol", "loll"));
    GBL_TEST_VERIFY(GblStringPattern_match("lol", "llol"));
    GBL_CTX_END();
}

static GBL_RESULT GblStringPatternTestSuite_matchBegin_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_VERIFY(GblStringPattern_match("^lol", "lol"));
    GBL_TEST_VERIFY(!GblStringPattern_match("^lol", "abc"));
    GBL_TEST_VERIFY(GblStringPattern_match("^lol", "loll"));
    GBL_TEST_VERIFY(!GblStringPattern_match("^lol", "llol"));
    GBL_CTX_END();
}

static GBL_RESULT GblStringPatternTestSuite_matchEnd_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_VERIFY(GblStringPattern_match("lol$", "lol"));
    GBL_TEST_VERIFY(!GblStringPattern_match("lol$", "abc"));
    GBL_TEST_VERIFY(!GblStringPattern_match("lol$", "loll"));
    GBL_TEST_VERIFY(GblStringPattern_match("lol$", "llol"));
    GBL_CTX_END();
}

static GBL_RESULT GblStringPatternTestSuite_matchAnyChar_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_VERIFY(GblStringPattern_match("...", "lol"));
    GBL_TEST_VERIFY(!GblStringPattern_match("..", "a"));
    GBL_TEST_VERIFY(GblStringPattern_match("....", "loll"));
    GBL_TEST_VERIFY(GblStringPattern_match(".", "a"));
    GBL_TEST_VERIFY(!GblStringPattern_match(".", ""));
    GBL_CTX_END();
}


static GBL_RESULT GblStringPatternTestSuite_matchStar_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_VERIFY(GblStringPattern_match("l*", "lol"));
    GBL_TEST_VERIFY(GblStringPattern_match("a*", "a"));
    GBL_TEST_VERIFY(!GblStringPattern_match("*", "loll"));
    GBL_TEST_VERIFY(GblStringPattern_match("b*", "a"));
    GBL_TEST_VERIFY(GblStringPattern_match("b*", "bbbbbbbbbbbbbb"));
    GBL_TEST_VERIFY(!GblStringPattern_match("*", ""));
    GBL_TEST_VERIFY(GblStringPattern_match("b*", ""));
    GBL_CTX_END();
}

static GBL_RESULT GblStringPatternTestSuite_matchPlus_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_VERIFY(GblStringPattern_match("l+", "lol"));
    GBL_TEST_VERIFY(GblStringPattern_match("a+", "a"));
    GBL_TEST_VERIFY(!GblStringPattern_match("+", "loll"));
    GBL_TEST_VERIFY(!GblStringPattern_match("b+", "a"));
    GBL_TEST_VERIFY(GblStringPattern_match("b+", "bbbbbbbbbbbbbb"));
    GBL_TEST_VERIFY(!GblStringPattern_match("+", ""));
    GBL_TEST_VERIFY(!GblStringPattern_match("b+", ""));
    GBL_CTX_END();
}

static GBL_RESULT GblStringPatternTestSuite_matchQuestion_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_VERIFY(GblStringPattern_match("l?", "lol"));
    GBL_TEST_VERIFY(GblStringPattern_match("a?", "a"));
    GBL_TEST_VERIFY(!GblStringPattern_match("?", "loll"));
    GBL_TEST_VERIFY(GblStringPattern_match("b?", "a"));
    GBL_TEST_VERIFY(GblStringPattern_match("b?", "bbbbb"));
    GBL_TEST_VERIFY(!GblStringPattern_match("^b?$", "bbbbb"));
    GBL_TEST_VERIFY(!GblStringPattern_match("?", ""));
    GBL_TEST_VERIFY(GblStringPattern_match("b?", ""));
    GBL_CTX_END();
}

static GBL_RESULT GblStringPatternTestSuite_matchComplex_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_VERIFY(GblStringPattern_match("^l.*l$", "ll"));
    GBL_TEST_VERIFY(GblStringPattern_match("^l.*l$", "lol"));
    GBL_TEST_VERIFY(GblStringPattern_match("^l.*l$", "luuuuul"));
    GBL_TEST_VERIFY(GblStringPattern_match("^l.*l$", "laul"));
    GBL_TEST_VERIFY(!GblStringPattern_match("^l.*l$", "olol"));
    GBL_TEST_VERIFY(!GblStringPattern_match("^l.*l$", "lolo"));
    GBL_CTX_END();
}


GBL_EXPORT GblType GblStringPatternTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "matchInvalid",       GblStringPatternTestSuite_matchInvalid_    },
        { "matchExact",         GblStringPatternTestSuite_matchExact_      },
        { "matchBegin",         GblStringPatternTestSuite_matchBegin_      },
        { "matchEnd",           GblStringPatternTestSuite_matchEnd_        },
        { "matchAnyChar",       GblStringPatternTestSuite_matchAnyChar_    },
        { "matchStar",          GblStringPatternTestSuite_matchStar_       },
        { "matchPlus",          GblStringPatternTestSuite_matchPlus_       },
        { "matchQuestion",      GblStringPatternTestSuite_matchQuestion_   },
        { "matchComplex",       GblStringPatternTestSuite_matchComplex_    },
        { NULL,                 NULL                                        }
    };

    const static GblTestSuiteClassVTable vTable = {
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("GblStringPatternTestSuite"),
                                     &vTable,
                                     sizeof(GblStringPatternTestSuite),
                                     0,
                                     GBL_TYPE_FLAGS_NONE);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }

    return type;
}

#include <gimbal/core/gimbal_ctx.h>
#include <gimbal/strings/gimbal_string_view.h>
#include <gimbal/test/gimbal_test_macros.h>
#include "strings/gimbal_string_view_test_suite.h"


static GBL_RESULT GblStringViewTestSuite_fromEmpty_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GblStringView view = GblStringView_fromEmpty();
    GBL_TEST_COMPARE(view.pData, NULL);
    GBL_TEST_COMPARE_UINT(view.nullTerminated, 0);
    GBL_TEST_COMPARE_UINT(view.length, 0);

    view = GBL_STRING_VIEW(NULL);
    GBL_TEST_COMPARE(view.pData, NULL);
    GBL_TEST_COMPARE_UINT(view.nullTerminated, 0);
    GBL_TEST_COMPARE_UINT(view.length, 0);

    view = GBL_STRV(NULL);
    GBL_TEST_COMPARE(view.pData, NULL);
    GBL_TEST_COMPARE_UINT(view.nullTerminated, 0);
    GBL_TEST_COMPARE_UINT(view.length, 0);

    GBL_CTX_END();
}
static GBL_RESULT GblStringViewTestSuite_fromString_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GblStringView view = GblStringView_fromString("lulz");
    GBL_TEST_COMPARE_UINT(view.length, 4);
    GBL_TEST_COMPARE_UINT(view.nullTerminated, 1);
    GBL_TEST_COMPARE(strcmp(view.pData, "lulz"), 0);

    view = GBL_STRING_VIEW("lulz");
    GBL_TEST_COMPARE_UINT(view.length, 4);
    GBL_TEST_COMPARE_UINT(view.nullTerminated, 1);
    GBL_TEST_COMPARE(strcmp(view.pData, "lulz"), 0);

    view = GBL_STRV("lulz");
    GBL_TEST_COMPARE_UINT(view.length, 4);
    GBL_TEST_COMPARE_UINT(view.nullTerminated, 1);
    GBL_TEST_COMPARE(strcmp(view.pData, "lulz"), 0);

    view = GBL_STRV("");
    GBL_TEST_COMPARE_UINT(view.length, 0);
    GBL_TEST_COMPARE_UINT(view.nullTerminated, 1);
    GBL_TEST_COMPARE(strcmp(view.pData, ""), 0);

    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_fromStringSized_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GblStringView view = GblStringView_fromStringSized("lulz", 4);
    GBL_TEST_COMPARE_UINT(view.length, 4);
    GBL_TEST_COMPARE_UINT(view.nullTerminated, 0);
    GBL_TEST_COMPARE(strcmp(view.pData, "lulz"), 0);

    view = GblStringView_fromStringSized("lulz", 0);
    GBL_TEST_COMPARE_UINT(view.length, 4);
    GBL_TEST_COMPARE_UINT(view.nullTerminated, 1);
    GBL_TEST_COMPARE(strcmp(view.pData, "lulz"), 0);

    view = GBL_STRING_VIEW("lulz", 2);
    GBL_TEST_COMPARE_UINT(view.length, 2);
    GBL_TEST_COMPARE_UINT(view.nullTerminated, 0);
    GBL_TEST_COMPARE(strcmp(view.pData, "lulz"), 0);

    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_fromQuark_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GblStringView view = GblStringView_fromQuark(GBL_QUARK_INVALID);
    GBL_TEST_COMPARE(view.pData, NULL);
    GBL_TEST_COMPARE_UINT(view.length, 0);
    GBL_TEST_COMPARE_UINT(view.nullTerminated, 0);

    view = GblStringView_fromQuark(GblQuark_fromString("lulz"));
    GBL_TEST_COMPARE(strcmp(view.pData, "lulz"), 0);
    GBL_TEST_COMPARE_UINT(view.nullTerminated, 1);
    GBL_TEST_COMPARE_UINT(view.length, 4);

    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_compare_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_TEST_COMPARE(GblStringView_compare(GblStringView_fromEmpty(),
                                      GBL_STRV(NULL)), 0);

    GBL_TEST_COMPARE(GblStringView_compare(GBL_STRV(NULL),
                                      GBL_STRV("")), 0);

    GBL_TEST_COMPARE(GblStringView_compare(GBL_STRV("Lulz"),
                                      GBL_STRING_VIEW("Lulzy", 4)), 0);

    GBL_TEST_VERIFY(GblStringView_compare(GBL_STRV("lulz"),
                                     GBL_STRV(NULL)) > 0);

    GBL_TEST_VERIFY(GblStringView_compare(GBL_STRV(NULL),
                                     GBL_STRV("Lulz")) < 0);

    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_compareIgnoreCase_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_TEST_COMPARE(GblStringView_compareIgnoreCase(GblStringView_fromEmpty(),
                                                GBL_STRV(NULL)), 0);

    GBL_TEST_COMPARE(GblStringView_compareIgnoreCase(GBL_STRV(NULL),
                                                GBL_STRV("")), 0);

    GBL_TEST_COMPARE(GblStringView_compareIgnoreCase(GBL_STRV("Lulz"),
                                                GBL_STRING_VIEW("Lulzy", 4)), 0);

    GBL_TEST_VERIFY(GblStringView_compareIgnoreCase(GBL_STRV("lulz"),
                                                GBL_STRV(NULL)) > 0);

    GBL_TEST_VERIFY(GblStringView_compareIgnoreCase(GBL_STRV(NULL),
                                               GBL_STRV("Lulz")) < 0);

    GBL_TEST_COMPARE(GblStringView_compareIgnoreCase(GBL_STRV("LULZ"),
                                                GBL_STRING_VIEW("lUlZ", 4)), 0);
    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_equals_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_TEST_COMPARE(GblStringView_equals(GblStringView_fromEmpty(),
                                     GBL_STRV(NULL)), 1);

    GBL_TEST_COMPARE(GblStringView_equals(GBL_STRV(NULL),
                                     GBL_STRV("")), 1);

    GBL_TEST_COMPARE(GblStringView_equals(GBL_STRV("Lulz"),
                                     GBL_STRING_VIEW("Lulzy", 4)), 1);

    GBL_TEST_COMPARE(GblStringView_equals(GBL_STRV("Lulz"),
                                     GBL_STRING_VIEW("nope")), 0);

    GBL_TEST_COMPARE(GblStringView_equals(GBL_STRV("lulz"),
                                     GBL_STRV(NULL)), 0);

    GBL_TEST_COMPARE(GblStringView_equals(GBL_STRV(NULL),
                                     GBL_STRV("Lulz")), 0);

    GBL_CTX_END();
}


static GBL_RESULT GblStringViewTestSuite_equalsIgnoreCase_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_TEST_COMPARE(GblStringView_equalsIgnoreCase(GblStringView_fromEmpty(),
                                               GBL_STRV(NULL)), 1);

    GBL_TEST_COMPARE(GblStringView_equalsIgnoreCase(GBL_STRV(NULL),
                                               GBL_STRV("")), 1);

    GBL_TEST_COMPARE(GblStringView_equalsIgnoreCase(GBL_STRV("Lulz"),
                                               GBL_STRING_VIEW("Lulzy", 4)), 1);

    GBL_TEST_COMPARE(GblStringView_equalsIgnoreCase(GBL_STRV("Lulz"),
                                               GBL_STRING_VIEW("LULz", 4)), 1);

    GBL_TEST_COMPARE(GblStringView_equalsIgnoreCase(GBL_STRV("lulz"),
                                               GBL_STRV(NULL)), 0);

    GBL_TEST_COMPARE(GblStringView_equalsIgnoreCase(GBL_STRV(NULL),
                                               GBL_STRV("Lulz")), 0);

    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_copyInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_TEST_EXPECT_ERROR();

    char buffer[6] = { '\0' };

    GBL_TEST_COMPARE(GblStringView_copy(GBL_STRV("Trolo"), buffer, 2, 3),
                GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblStringView_copy(GBL_STRV(NULL), buffer, 0, 1),
                GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}


static GBL_RESULT GblStringViewTestSuite_copy_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    char buffer[6] = { '\0' };

    GBL_CTX_VERIFY_CALL(GblStringView_copy(GBL_STRV("Trolololo"), buffer, 3, 3));

    GBL_TEST_COMPARE(strcmp(buffer, "lol"), 0);

    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_empty_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_VERIFY(GblStringView_empty(GBL_STRV(NULL)));
    GBL_TEST_VERIFY(GblStringView_empty(GBL_STRV("")));
    GBL_TEST_VERIFY(!GblStringView_empty(GBL_STRV("x")));
    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_blank_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_VERIFY(GblStringView_blank(GBL_STRV(NULL)));
    GBL_TEST_VERIFY(GblStringView_blank(GBL_STRV("")));
    GBL_TEST_VERIFY(!GblStringView_blank(GBL_STRV("x")));
    GBL_TEST_VERIFY(GblStringView_blank(GBL_STRV("\n\r\t ")));
    GBL_TEST_VERIFY(!GblStringView_blank(GBL_STRV("   \t\r\nx")));
    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_at_invalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblStringView_at(GBL_STRV(NULL), 0), '\0');
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblStringView_at(GBL_STRV(""), 0), '\0');
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblStringView_at(GBL_STRV("lulz"), strlen("lulz")), '\0');
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_at_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_COMPARE(GblStringView_at(GBL_STRV("lulz"), 0), 'l');
    GBL_TEST_COMPARE(GblStringView_at(GBL_STRV("lulz"), 3), 'z');
    GBL_CTX_END();
}


static GBL_RESULT GblStringViewTestSuite_first_invalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();
    GBL_TEST_COMPARE(GblStringView_first(GBL_STRV(NULL)), '\0');
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_first_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_COMPARE(GblStringView_first(GBL_STRV("lulz")), 'l');
    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_last_invalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();
    GBL_TEST_COMPARE(GblStringView_last(GBL_STRV(NULL)), '\0');
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_last_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_COMPARE(GblStringView_last(GBL_STRV("lulz")), 'z');
    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_removePrefix_invalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();

    GblStringView view = GblStringView_removePrefix(GBL_STRV(NULL), 1);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_TEST_COMPARE(view.pData, NULL);
    GBL_TEST_COMPARE_UINT(view.nullTerminated, 0);
    GBL_TEST_COMPARE_UINT(view.length, 0);
    GBL_CTX_CLEAR_LAST_RECORD();

    view = GblStringView_removePrefix(GBL_STRV("lulz"), 5);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_TEST_COMPARE(view.pData, NULL);
    GBL_TEST_COMPARE_UINT(view.nullTerminated, 0);
    GBL_TEST_COMPARE_UINT(view.length, 0);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_removePrefix_(GblTestSuite* pSelf, GblContext* pCtx) {
GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GblStringView view = GblStringView_removePrefix(GBL_STRV(""), 0);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(strcmp(view.pData, ""), 0);
    GBL_TEST_COMPARE_UINT(view.nullTerminated, 1);
    GBL_TEST_COMPARE_UINT(view.length, 0);

    view = GblStringView_removePrefix(GBL_STRV("lolz"), 0);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(strcmp(view.pData, "lolz"), 0);
    GBL_TEST_COMPARE_UINT(view.nullTerminated, 1);
    GBL_TEST_COMPARE_UINT(view.length, 4);

    view = GblStringView_removePrefix(GBL_STRV("lolz"), 2);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(strcmp(view.pData, "lz"), 0);
    GBL_TEST_COMPARE_UINT(view.nullTerminated, 1);
    GBL_TEST_COMPARE_UINT(view.length, 2);

    view = GblStringView_removePrefix(GBL_STRV("lolz"), 4);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(strcmp(view.pData, ""), 0);
    GBL_TEST_COMPARE_UINT(view.nullTerminated, 1);
    GBL_TEST_COMPARE_UINT(view.length, 0);

    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_removeSuffix_invalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_TEST_EXPECT_ERROR();

    GblStringView view = GblStringView_removeSuffix(GBL_STRV(NULL), 1);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_TEST_COMPARE(view.pData, NULL);
    GBL_TEST_COMPARE_UINT(view.nullTerminated, 0);
    GBL_TEST_COMPARE_UINT(view.length, 0);
    GBL_CTX_CLEAR_LAST_RECORD();

    view = GblStringView_removeSuffix(GBL_STRV("lulz"), 5);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_TEST_COMPARE(view.pData, NULL);
    GBL_TEST_COMPARE_UINT(view.nullTerminated, 0);
    GBL_TEST_COMPARE_UINT(view.length, 0);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}
static GBL_RESULT GblStringViewTestSuite_removeSuffix_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GblStringView view = GblStringView_removeSuffix(GBL_STRV(""), 0);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(strcmp(view.pData, ""), 0);
    GBL_TEST_COMPARE_UINT(view.nullTerminated, 1);
    GBL_TEST_COMPARE_UINT(view.length, 0);

    view = GblStringView_removeSuffix(GBL_STRV("lolz"), 1);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_VERIFY(GblStringView_equals(view, GBL_STRV("lol")));
    GBL_TEST_COMPARE_UINT(view.nullTerminated, 0);
    GBL_TEST_COMPARE_UINT(view.length, 3);

    view = GblStringView_removeSuffix(GBL_STRV("lolz"), 4);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_VERIFY(GblStringView_equals(view, GBL_STRV("")));
    GBL_TEST_COMPARE_UINT(view.nullTerminated, 0);
    GBL_TEST_COMPARE_UINT(view.length, 0);

    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_chomp_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_VERIFY(GblStringView_equals(GblStringView_chomp(GBL_STRV(NULL)),
                                    GBL_STRV("")));
    GBL_TEST_VERIFY(GblStringView_equals(GblStringView_chomp(GBL_STRV("lolz")),
                                    GBL_STRV("lolz")));
    GBL_TEST_VERIFY(GblStringView_equals(GblStringView_chomp(GBL_STRV("lolz\n\n")),
                                    GBL_STRV("lolz\n")));
    GBL_TEST_VERIFY(GblStringView_equals(GblStringView_chomp(GBL_STRV("lolz\r\n\r\n")),
                                    GBL_STRV("lolz\r\n")));
    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_substr_invalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_TEST_EXPECT_ERROR();

    GblStringView view = GblStringView_substr(GBL_STRV(NULL), 0, 1);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_TEST_COMPARE(view.pData, NULL);
    GBL_TEST_COMPARE_UINT(view.nullTerminated, 0);
    GBL_TEST_COMPARE_UINT(view.length, 0);
    GBL_CTX_CLEAR_LAST_RECORD();

    GblStringView_substr(GBL_STRV(NULL), 1, 0);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_TEST_COMPARE(view.pData, NULL);
    GBL_TEST_COMPARE_UINT(view.nullTerminated, 0);
    GBL_TEST_COMPARE_UINT(view.length, 0);
    GBL_CTX_CLEAR_LAST_RECORD();

    GblStringView_substr(GBL_STRV("lulz"), 0, 5);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_TEST_COMPARE(view.pData, NULL);
    GBL_TEST_COMPARE_UINT(view.nullTerminated, 0);
    GBL_TEST_COMPARE_UINT(view.length, 0);
    GBL_CTX_CLEAR_LAST_RECORD();

    GblStringView_substr(GBL_STRV("lulz"), 1, 4);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_TEST_COMPARE(view.pData, NULL);
    GBL_TEST_COMPARE_UINT(view.nullTerminated, 0);
    GBL_TEST_COMPARE_UINT(view.length, 0);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_substr_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GblStringView view = GblStringView_substr(GBL_STRV(NULL), 0, 0);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(view.pData, NULL);
    GBL_TEST_COMPARE_UINT(view.nullTerminated, 0);
    GBL_TEST_COMPARE_UINT(view.length, 0);

    view = GblStringView_substr(GBL_STRV(""), 0, 0);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(strcmp(view.pData, ""), 0);
    GBL_TEST_COMPARE_UINT(view.nullTerminated, 1);
    GBL_TEST_COMPARE_UINT(view.length, 0);

    view = GblStringView_substr(GBL_STRV("lolz"), 0, 4);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(strcmp(view.pData, "lolz"), 0);
    GBL_TEST_COMPARE_UINT(view.nullTerminated, 1);
    GBL_TEST_COMPARE_UINT(view.length, 4);

    view = GblStringView_substr(GBL_STRV("lolz"), 0, 3);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_VERIFY(GblStringView_equals(view, GBL_STRV("lol")));
    GBL_TEST_COMPARE_UINT(view.nullTerminated, 0);
    GBL_TEST_COMPARE_UINT(view.length, 3);

    view = GblStringView_substr(GBL_STRV("lolz"), 4, 0);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_VERIFY(GblStringView_equals(view, GBL_STRV("")));
    GBL_TEST_COMPARE_UINT(view.nullTerminated, 1);
    GBL_TEST_COMPARE_UINT(view.length, 0);


    view = GblStringView_substr(GBL_STRV("lolz"), 3, 1);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_VERIFY(GblStringView_equals(view, GBL_STRV("z")));
    GBL_TEST_COMPARE_UINT(view.nullTerminated, 1);
    GBL_TEST_COMPARE_UINT(view.length, 1);

    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_startsWith_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_VERIFY(GblStringView_startsWith(GBL_STRV("lolz"), GBL_STRV("lo")));
    GBL_TEST_VERIFY(GblStringView_startsWith(GBL_STRV("lolz"), GBL_STRV("lolz")));
    GBL_TEST_VERIFY(!GblStringView_startsWith(GBL_STRV("lolz"), GBL_STRV("z")));
    GBL_TEST_VERIFY(GblStringView_startsWith(GBL_STRV(NULL), GBL_STRV(NULL)));
    GBL_TEST_VERIFY(GblStringView_startsWith(GBL_STRV(""), GBL_STRV(NULL)));
    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_endsWith_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_VERIFY(GblStringView_endsWith(GBL_STRV("lolz"), GBL_STRV("lz")));
    GBL_TEST_VERIFY(GblStringView_endsWith(GBL_STRV("lolz"), GBL_STRV("lolz")));
    GBL_TEST_VERIFY(!GblStringView_endsWith(GBL_STRV("lolz"), GBL_STRV(" lolz")));
    GBL_TEST_VERIFY(GblStringView_endsWith(GBL_STRV(NULL), GBL_STRV(NULL)));
    GBL_TEST_VERIFY(GblStringView_endsWith(GBL_STRV(""), GBL_STRV(NULL)));
    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_contains_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_VERIFY(GblStringView_contains(GBL_STRV(NULL), GBL_STRV("")));
    GBL_TEST_VERIFY(!GblStringView_contains(GBL_STRV("lol"), GBL_STRV("")));
    GBL_TEST_VERIFY(!GblStringView_contains(GBL_STRV(""), GBL_STRV("lol")));
    GBL_TEST_VERIFY(GblStringView_contains(GBL_STRV("lolzer"), GBL_STRV("lze")));
    GBL_TEST_VERIFY(GblStringView_contains(GBL_STRV("lolzer"), GBL_STRV("lolzer")));
    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_count_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_COMPARE(GblStringView_count(GBL_STRV(NULL), GBL_STRV("")), 0);
    GBL_TEST_COMPARE(GblStringView_count(GBL_STRV("lol"), GBL_STRV(NULL)), 0);
    GBL_TEST_COMPARE(GblStringView_count(GBL_STRV(""), GBL_STRV("lol")), 0);
    GBL_TEST_COMPARE(GblStringView_count(GBL_STRV("lol"), GBL_STRV("lol")), 1);
    GBL_TEST_COMPARE(GblStringView_count(GBL_STRV("lolol"), GBL_STRV("lol")), 1);
    GBL_TEST_COMPARE(GblStringView_count(GBL_STRV("lollolz"), GBL_STRV("lol")), 2);
    GBL_TEST_COMPARE(GblStringView_count(GBL_STRV("lollolz"), GBL_STRV("zero")), 0);
    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_find_invalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblStringView_find(GBL_STRV(NULL), GBL_STRV("lol"), 1), GBL_STRING_VIEW_NPOS);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblStringView_find(GBL_STRV("lol"), GBL_STRV("lol"), 4), GBL_STRING_VIEW_NPOS);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_find_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_COMPARE(GblStringView_find(GBL_STRV(NULL), GBL_STRV("lol"), 0), GBL_STRING_VIEW_NPOS);
    GBL_TEST_COMPARE(GblStringView_find(GBL_STRV("lol"), GBL_STRV("l"), 0), 0);
    GBL_TEST_COMPARE(GblStringView_find(GBL_STRV("lol"), GBL_STRV("lol"), 0), 0);
    GBL_TEST_COMPARE(GblStringView_find(GBL_STRV("lol"), GBL_STRV("ol"), 0), 1);
    GBL_TEST_COMPARE(GblStringView_find(GBL_STRV("lol"), GBL_STRV("l"), 1), 2);
    GBL_TEST_COMPARE(GblStringView_find(GBL_STRV("lol"), GBL_STRV("lz"), 0), GBL_STRING_VIEW_NPOS);
    GBL_TEST_COMPARE(GblStringView_find(GBL_STRV("lol"), GBL_STRV(NULL), 0), GBL_STRING_VIEW_NPOS);
    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_rfind_invalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblStringView_rfind(GBL_STRV(NULL), GBL_STRV("lol"), 1), GBL_STRING_VIEW_NPOS);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblStringView_rfind(GBL_STRV("lol"), GBL_STRV("lol"), 4), GBL_STRING_VIEW_NPOS);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_rfind_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_COMPARE(GblStringView_rfind(GBL_STRV(NULL), GBL_STRV("lol"), GBL_STRING_VIEW_NPOS), GBL_STRING_VIEW_NPOS);
    GBL_TEST_COMPARE(GblStringView_rfind(GBL_STRV("lol"), GBL_STRV(""), GBL_STRING_VIEW_NPOS), GBL_STRING_VIEW_NPOS);
    GBL_TEST_COMPARE(GblStringView_rfind(GBL_STRV("lolz"), GBL_STRV("lol"), GBL_STRING_VIEW_NPOS), 0);
    GBL_TEST_COMPARE(GblStringView_rfind(GBL_STRV("lol"), GBL_STRV("lol"), 2), 0);
    GBL_TEST_COMPARE(GblStringView_rfind(GBL_STRV("zlol lol"), GBL_STRV("lol"), 4), 1);
    GBL_TEST_COMPARE(GblStringView_rfind(GBL_STRV("lol"), GBL_STRV("lo"), 1), 0);
    GBL_TEST_COMPARE(GblStringView_rfind(GBL_STRV("lol"), GBL_STRV("zl"), 1), GBL_STRING_VIEW_NPOS);
    GBL_TEST_COMPARE(GblStringView_rfind(GBL_STRV("lolol"), GBL_STRV("lol"), GBL_STRING_VIEW_NPOS), 2);
    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_findFirstOf_invalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();
    GBL_TEST_COMPARE(GblStringView_findFirstOf(GBL_STRV("lolz"), GBL_STRV(NULL), 5), GBL_STRING_VIEW_NPOS);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblStringView_findFirstOf(GBL_STRV(""), GBL_STRV("lolz"), 1), GBL_STRING_VIEW_NPOS);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_findFirstOf_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_COMPARE(GblStringView_findFirstOf(GBL_STRV("lolz"), GBL_STRV("o"), 0), 1);
    GBL_TEST_COMPARE(GblStringView_findFirstOf(GBL_STRV("lolz"), GBL_STRV("abcl"), 1), 2);
    GBL_TEST_COMPARE(GblStringView_findFirstOf(GBL_STRV("lolz"), GBL_STRV("zlo"), 0), 0);
    GBL_TEST_COMPARE(GblStringView_findFirstOf(GBL_STRV("lolz"), GBL_STRV("a"), 0), GBL_STRING_VIEW_NPOS);
    GBL_TEST_COMPARE(GblStringView_findFirstOf(GBL_STRV("lolz"), GBL_STRV(NULL), 0), GBL_STRING_VIEW_NPOS);
    GBL_TEST_COMPARE(GblStringView_findFirstOf(GBL_STRV(""), GBL_STRV(NULL), 0), GBL_STRING_VIEW_NPOS);
    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_findFirstNotOf_invalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();
    GBL_TEST_COMPARE(GblStringView_findFirstNotOf(GBL_STRV("lolz"), GBL_STRV(NULL), 5), GBL_STRING_VIEW_NPOS);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblStringView_findFirstNotOf(GBL_STRV(""), GBL_STRV("lolz"), 1), GBL_STRING_VIEW_NPOS);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_findFirstNotOf_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_COMPARE(GblStringView_findFirstNotOf(GBL_STRV("lolz"), GBL_STRV("lz"), 0), 1);
    GBL_TEST_COMPARE(GblStringView_findFirstNotOf(GBL_STRV("lolz"), GBL_STRV("abcl"), 1), 1);
    GBL_TEST_COMPARE(GblStringView_findFirstNotOf(GBL_STRV("lolz"), GBL_STRV("alo"), 0), 3);
    GBL_TEST_COMPARE(GblStringView_findFirstNotOf(GBL_STRV("lolz"), GBL_STRV("zol"), 0), GBL_STRING_VIEW_NPOS);
    GBL_TEST_COMPARE(GblStringView_findFirstNotOf(GBL_STRV("lolz"), GBL_STRV(NULL), 0), 0);
    GBL_TEST_COMPARE(GblStringView_findFirstNotOf(GBL_STRV(""), GBL_STRV(NULL), 0), 0);
    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_findLastOf_invalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();
    GBL_TEST_COMPARE(GblStringView_findLastOf(GBL_STRV("lolz"), GBL_STRV(NULL), 5), GBL_STRING_VIEW_NPOS);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblStringView_findLastOf(GBL_STRV(""), GBL_STRV("lolz"), 1), GBL_STRING_VIEW_NPOS);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_findLastOf_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_COMPARE(GblStringView_findLastOf(GBL_STRV("lolz"), GBL_STRV("o"), 0), GBL_STRING_VIEW_NPOS);
    GBL_TEST_COMPARE(GblStringView_findLastOf(GBL_STRV("lolz"), GBL_STRV("o"), GBL_STRING_VIEW_NPOS), 1);
    GBL_TEST_COMPARE(GblStringView_findLastOf(GBL_STRV("lolz"), GBL_STRV("abcl"), 3), 2);
    GBL_TEST_COMPARE(GblStringView_findLastOf(GBL_STRV("lolz"), GBL_STRV("zlo"), 1), 1);
    GBL_TEST_COMPARE(GblStringView_findLastOf(GBL_STRV("lolz"), GBL_STRV("a"), 0), GBL_STRING_VIEW_NPOS);
    GBL_TEST_COMPARE(GblStringView_findLastOf(GBL_STRV("lolz"), GBL_STRV(NULL), 3), GBL_STRING_VIEW_NPOS);
    GBL_TEST_COMPARE(GblStringView_findLastOf(GBL_STRV(""), GBL_STRV(NULL), 0), GBL_STRING_VIEW_NPOS);
    GBL_CTX_END();
}


static GBL_RESULT GblStringViewTestSuite_findLastNotOf_invalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();
    GBL_TEST_COMPARE(GblStringView_findLastNotOf(GBL_STRV("lolz"), GBL_STRV(NULL), 5), GBL_STRING_VIEW_NPOS);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblStringView_findLastNotOf(GBL_STRV(""), GBL_STRV("lolz"), 1), GBL_STRING_VIEW_NPOS);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_findLastNotOf_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_COMPARE(GblStringView_findLastNotOf(GBL_STRV("lolz"), GBL_STRV("ozl"), 0), GBL_STRING_VIEW_NPOS);
    GBL_TEST_COMPARE(GblStringView_findLastNotOf(GBL_STRV("lolz"), GBL_STRV("oz"), 0), 0);
    GBL_TEST_COMPARE(GblStringView_findLastNotOf(GBL_STRV("lolz"), GBL_STRV("abcl"), 2), 1);
    GBL_TEST_COMPARE(GblStringView_findLastNotOf(GBL_STRV("lolz"), GBL_STRV("alz"), GBL_STRING_VIEW_NPOS), 1);
    GBL_TEST_COMPARE(GblStringView_findLastNotOf(GBL_STRV("lolz"), GBL_STRV("zol"), 3), GBL_STRING_VIEW_NPOS);
    GBL_TEST_COMPARE(GblStringView_findLastNotOf(GBL_STRV("lolz"), GBL_STRV(NULL), 3), 3);
    GBL_TEST_COMPARE(GblStringView_findLastNotOf(GBL_STRV(""), GBL_STRV(NULL), 0), GBL_STRING_VIEW_NPOS);
    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_quark_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_COMPARE(GblStringView_quark(GBL_STRV(NULL)),
                GblQuark_fromString(""));

    GBL_TEST_COMPARE(GblStringView_quark(GBL_STRV("StringViewTestStringView")),
                GblQuark_fromString("StringViewTestStringView"));

    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_quarkTry_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_COMPARE(GblStringView_quarkTry(GBL_STRV(NULL)),
                GblQuark_tryString(""));

    GBL_TEST_COMPARE(GblStringView_quarkTry(GBL_STRV("Not Interned")),
                GblQuark_tryString("Not Interned"));

    GBL_TEST_COMPARE(GblStringView_quarkTry(GBL_STRV("StringViewTestStringView")),
                GblQuark_tryString("StringViewTestStringView"));
    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_intern_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_COMPARE(GblStringView_intern(GBL_STRV(NULL)),
                GblQuark_internString(""));

    GBL_TEST_COMPARE(GblStringView_intern(GBL_STRV("StringViewTestIntern")),
                GblQuark_internString("StringViewTestIntern"));

    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_hash_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_COMPARE(GblStringView_hash(GBL_STRV("trolo")),
                     gblHash("trolo", 5));
    GBL_TEST_COMPARE(GblStringView_hash(GBL_STRV(NULL)), 0);

    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_toCString_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    char buffer[100];
    GBL_TEST_COMPARE(strcmp(GblStringView_toCString(GBL_STRV("lol"), buffer, sizeof(buffer)), "lol"), 0);
    GBL_TEST_COMPARE(strcmp(GblStringView_toCString(GBL_STRV("lol"), buffer, 3), "lo"), 0);
    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_toNil_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_VERIFY(!GblStringView_toNil(GBL_STRV("lol")));
    GBL_TEST_VERIFY(GblStringView_toNil(GBL_STRV(NULL)));
    GBL_TEST_VERIFY(GblStringView_toNil(GBL_STRV("")));
    GBL_TEST_VERIFY(GblStringView_toNil(GBL_STRV("nil")));
    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_toBool_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_VERIFY(!GblStringView_toBool(GBL_STRV("lol")));
    GBL_TEST_VERIFY(!GblStringView_toBool(GBL_STRV(NULL)));
    GBL_TEST_VERIFY(!GblStringView_toBool(GBL_STRV("")));
    GBL_TEST_VERIFY(!GblStringView_toBool(GBL_STRV("false")));
    GBL_TEST_VERIFY(GblStringView_toBool(GBL_STRV("true")));
    GBL_TEST_VERIFY(!GblStringView_toBool(GBL_STRV("0")));
    GBL_TEST_VERIFY(GblStringView_toBool(GBL_STRV("1")));
    GBL_TEST_VERIFY(!GblStringView_toBool(GBL_STRV("nil")));
    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_toInt_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_COMPARE(GblStringView_toInt32(GBL_STRV(NULL)), 0);
    GBL_TEST_COMPARE(GblStringView_toInt16(GBL_STRV("")), 0);
    GBL_TEST_COMPARE(GblStringView_toInt32(GBL_STRV("0")), 0);
    GBL_TEST_COMPARE(GblStringView_toInt64(GBL_STRV("-22")), -22);
    GBL_TEST_COMPARE(GblStringView_toInt16(GBL_STRV("333")), 333);
    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_toUint_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_COMPARE(GblStringView_toUint8(GBL_STRV(NULL)), 0);
    GBL_TEST_COMPARE(GblStringView_toUint16(GBL_STRV("")), 0);
    GBL_TEST_COMPARE(GblStringView_toUint32(GBL_STRV("0")), 0);
    GBL_TEST_COMPARE(GblStringView_toUint64(GBL_STRV("-22")), (uint64_t)-22);
    GBL_TEST_COMPARE(GblStringView_toUint16(GBL_STRV("333")), 333);
    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_toFloat_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_COMPARE(GblStringView_toFloat(GBL_STRV(NULL)), 0.0f);
    GBL_TEST_COMPARE(GblStringView_toFloat(GBL_STRV("")), 0.0f);
    GBL_TEST_COMPARE(GblStringView_toFloat(GBL_STRV("0.0f")), 0.0f);
    GBL_TEST_COMPARE(GblStringView_toFloat(GBL_STRV("-22")), -22.0f);
    GBL_TEST_COMPARE(GblStringView_toFloat(GBL_STRV("333")), 333.0f);
    GBL_TEST_COMPARE(GblStringView_toFloat(GBL_STRV("3.33")), 3.33f);
    GBL_TEST_COMPARE(GblStringView_toFloat(GBL_STRV("-0.005")), -0.005f);
    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_toDouble_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);\
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_COMPARE(GblStringView_toDouble(GBL_STRV(NULL)), 0.0);
    GBL_TEST_COMPARE(GblStringView_toDouble(GBL_STRV("")), 0.0);
    GBL_TEST_COMPARE(GblStringView_toDouble(GBL_STRV("0.0")), 0.0);
    GBL_TEST_COMPARE(GblStringView_toDouble(GBL_STRV("-22")), -22.0);
    GBL_TEST_COMPARE(GblStringView_toDouble(GBL_STRV("333")), 333.0);
    GBL_TEST_COMPARE(GblStringView_toDouble(GBL_STRV("3.423")), 3.423);
    GBL_TEST_COMPARE(GblStringView_toDouble(GBL_STRV("-0.005")), -0.005);
    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_toPointer_invalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();

    GblBool success;
    GblStringView_toPointer(GBL_STRV(NULL), &success);
    GBL_TEST_VERIFY(!success);
    GBL_CTX_CLEAR_LAST_RECORD();

    GblStringView_toPointer(GBL_STRV("lolz"), &success);
    GBL_TEST_VERIFY(!success);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblStringViewTestSuite_toPointer_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_COMPARE(GblStringView_toPointer(GBL_STRV("0xdeadbeef")), (void*)0xdeadbeef);
    GBL_CTX_END();
}


GBL_EXPORT GblType GblStringViewTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "fromEmpty",              GblStringViewTestSuite_fromEmpty_               },
        { "fromString",             GblStringViewTestSuite_fromString_              },
        { "fromStringSized",        GblStringViewTestSuite_fromStringSized_         },
        { "fromQuark",              GblStringViewTestSuite_fromQuark_               },
        { "compare",                GblStringViewTestSuite_compare_                 },
        { "compareIgnoresCase",     GblStringViewTestSuite_compareIgnoreCase_       },
        { "equals",                 GblStringViewTestSuite_equals_                  },
        { "equalsIgnoresCase",      GblStringViewTestSuite_equalsIgnoreCase_        },
        { "copyInvalid",            GblStringViewTestSuite_copyInvalid_             },
        { "copy",                   GblStringViewTestSuite_copy_                    },
        { "empty",                  GblStringViewTestSuite_empty_                   },
        { "blank",                  GblStringViewTestSuite_blank_                   },
        { "atInvalid",              GblStringViewTestSuite_at_invalid_              },
        { "at",                     GblStringViewTestSuite_at_                      },
        { "firstInvalid",           GblStringViewTestSuite_first_invalid_           },
        { "first",                  GblStringViewTestSuite_first_                   },
        { "lastInvalid",            GblStringViewTestSuite_last_invalid_            },
        { "last",                   GblStringViewTestSuite_last_                    },
        { "removePrefixInvalid",    GblStringViewTestSuite_removePrefix_invalid_    },
        { "removePrefix",           GblStringViewTestSuite_removePrefix_            },
        { "removeSuffixInvalid",    GblStringViewTestSuite_removeSuffix_invalid_    },
        { "removeSuffix",           GblStringViewTestSuite_removeSuffix_            },
        { "chomp",                  GblStringViewTestSuite_chomp_                   },
        { "substrInvalid",          GblStringViewTestSuite_substr_invalid_          },
        { "substr",                 GblStringViewTestSuite_substr_                  },
        { "startsWith",             GblStringViewTestSuite_startsWith_              },
        { "endsWith",               GblStringViewTestSuite_endsWith_                },
        { "contains",               GblStringViewTestSuite_contains_                },
        { "count",                  GblStringViewTestSuite_count_                   },
        { "findInvalid",            GblStringViewTestSuite_find_invalid_            },
        { "find",                   GblStringViewTestSuite_find_                    },
        { "rfindInvalid",           GblStringViewTestSuite_rfind_invalid_           },
        { "rfind",                  GblStringViewTestSuite_rfind_                   },
        { "findFirstOfInvalid",     GblStringViewTestSuite_findFirstOf_invalid_     },
        { "findFirstOf",            GblStringViewTestSuite_findFirstOf_             },
        { "findFirstNotOfInvalid",  GblStringViewTestSuite_findFirstNotOf_invalid_  },
        { "findFirstNotOf",         GblStringViewTestSuite_findFirstNotOf_          },
        { "findLastOfInvalid",      GblStringViewTestSuite_findLastOf_invalid_      },
        { "findLastOf",             GblStringViewTestSuite_findLastOf_              },
        { "findLastNotOfInvalid",   GblStringViewTestSuite_findLastNotOf_invalid_   },
        { "findLastNotOf",          GblStringViewTestSuite_findLastNotOf_           },
        { "quark",                  GblStringViewTestSuite_quark_                   },
        { "quarkTry",               GblStringViewTestSuite_quarkTry_                },
        { "intern",                 GblStringViewTestSuite_intern_                  },
        { "hash",                   GblStringViewTestSuite_hash_                    },
        { "toCString",              GblStringViewTestSuite_toCString_               },
        { "toNil",                  GblStringViewTestSuite_toNil_                   },
        { "toBool",                 GblStringViewTestSuite_toBool_                  },
        { "toInt",                  GblStringViewTestSuite_toInt_                   },
        { "toUint",                 GblStringViewTestSuite_toUint_                  },
        { "toFloat",                GblStringViewTestSuite_toFloat_                 },
        { "toDouble",               GblStringViewTestSuite_toDouble_                },
        { "toPointerInvalid",       GblStringViewTestSuite_toPointer_invalid_       },
        { "toPointer",              GblStringViewTestSuite_toPointer_               },
        { NULL,                     NULL                                            }
    };

    const static GblTestSuiteClassVTable vTable = {
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("StringViewTestSuite"),
                                     &vTable,
                                     sizeof(GblStringViewTestSuite),
                                     0,
                                     GBL_TYPE_FLAGS_NONE);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }
    return type;
}

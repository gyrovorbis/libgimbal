#include "utils/gimbal_option_group_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/utils/gimbal_option_group.h>
#include <gimbal/meta/signals/gimbal_c_closure.h>
#include <gimbal/utils/gimbal_ref.h>
#include <math.h>

#define GBL_SELF_TYPE GblOptionGroupTestSuite

GBL_TEST_FIXTURE {
    GblOptionGroup* pGroup;
    GblRefCount     refCount;
    const char*     pFirst;
    uint32_t        second;
    GblBool         third;
    double          fourth;
    char            fifth;
    int16_t         sixth;
    float           seventh;
    GblBool         eighth;
    GblBool         ninth;
    GblBool         tenth;
    GblBool         eleventh;
};

GBL_TEST_INIT() {
    pFixture->refCount = GblRef_activeCount();
    GBL_TEST_CASE_END;
}

GBL_TEST_FINAL() {
    GBL_TEST_COMPARE(pFixture->refCount, GblRef_activeCount());
    GBL_TEST_CASE_END;
}

GBL_RESULT parseArg7_(GblOptionGroup* pGroup, const GblOption* pOption, GblStringView value, GblBool* pConsumed) {
    GBL_CTX_BEGIN(pGroup);

    GblOptionGroupTestSuite_* pFixture = GblBox_userdata(GBL_BOX(pGroup));

    // Verify that we were passed a value
    GBL_CTX_VERIFY(!GblStringView_empty(value),
                   GBL_RESULT_ERROR_INVALID_CMDLINE_ARG,
                   "Expected value for option [%s]!",
                   pOption->pLongName);

    // Convert value from string to float and store
    pFixture->seventh = GblStringView_toFloat(value);

    // Validate argument is within our range
    GBL_CTX_VERIFY(!isnan(pFixture->seventh)
                   && pFixture->seventh < 5.0f,
                   GBL_RESULT_ERROR_INVALID_CMDLINE_ARG,
                   "Failed to parse value for option %s. [%f]",
                   pOption->pLongName, pFixture->seventh);

    // Report back that we consumed its argument gracefully
    *pConsumed = GBL_TRUE;

    GBL_CTX_END();
}


GBL_RESULT parseArg8_(GblOptionGroup* pGroup, const GblOption* pOption, GblStringView value, GblBool* pConsumed) {
    GBL_UNUSED(pOption, value, pConsumed);
    GblTestFixture* pFixture = GblBox_userdata(GBL_BOX(pGroup));
    pFixture->eighth = GBL_TRUE;
    return GBL_RESULT_SUCCESS;
}

GBL_TEST_CASE(create) {
pFixture->pGroup =
    GBL_OPTION_GROUP(GblObject_create(GBL_OPTION_GROUP_TYPE,
         "name",        "Test Group",
         "prefix",      "test",
         "version",     GblVersion_make(1, 2, 3),
         "summary",     "Running...",
         "description", "Executing unit test suites.",
         "userdata",    pFixture,
         "options",     (GblOption[]) {
             { "first",   '1', GBL_OPTION_TYPE_STRING,   { &pFixture->pFirst }, "1st value", "value1", GBL_OPTION_FLAG_NONE   },
             { "second",  '2', GBL_OPTION_TYPE_UINT32,   { &pFixture->second }, "2nd value", "value2", GBL_OPTION_FLAG_NONE   },
             { "third",   '3', GBL_OPTION_TYPE_BOOL,     { &pFixture->third  }, "3rd value", "value3", GBL_OPTION_FLAG_NONE   },
             { "fourth",  '4', GBL_OPTION_TYPE_DOUBLE,   { &pFixture->fourth }, "4th value", "value4", GBL_OPTION_FLAG_NONE   },
             { "fifth",   '5', GBL_OPTION_TYPE_CHAR,     { &pFixture->fifth  }, "5th value", "value5", GBL_OPTION_FLAG_HIDDEN },
             { "sixth",   '6', GBL_OPTION_TYPE_INT16,    { &pFixture->sixth  }, "6th value", "value6", GBL_OPTION_FLAG_HIDDEN },
             { "seventh", '7', GBL_OPTION_TYPE_CALLBACK, { .pFn = parseArg7_ }, "7th value", "value7", GBL_OPTION_FLAG_NONE   },
             { "eighth",  '8', GBL_OPTION_TYPE_CALLBACK, { .pFn = parseArg8_ }, "8th value", "value8", GBL_OPTION_FLAG_NONE   },
             { "ninth",   '9', GBL_OPTION_TYPE_BOOL,     { &pFixture->ninth  }, "9th value", "value9", GBL_OPTION_FLAG_BOOL_NO_VALUE },
             { "tenth",    0,  GBL_OPTION_TYPE_BOOL,     { &pFixture->tenth  }, "10th value", "val10", GBL_OPTION_FLAG_BOOL_NO_VALUE|GBL_OPTION_FLAG_BOOL_INVERTED },
             { NULL,      'a', GBL_OPTION_TYPE_BOOL,     {&pFixture->eleventh}, "11th value", "val11", GBL_OPTION_FLAG_BOOL_INVERTED },
             { 0 }
         },
         NULL));

    GBL_TEST_VERIFY(pFixture->pGroup);
    GBL_TEST_COMPARE(pFixture->pGroup->optionCount, 11);
    GBL_TEST_COMPARE(pFixture->pGroup->pPrefix, "test");
    GBL_TEST_COMPARE(pFixture->pGroup->version, GblVersion_make(1, 2, 3));
    GBL_TEST_COMPARE(pFixture->pGroup->pSummary, "Running...");
    GBL_TEST_COMPARE(pFixture->pGroup->pDescription, "Executing unit test suites.");
    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(parseLong) {
    GblStringList* pList = GblStringList_create("--first", "Firsty");
    GBL_CTX_VERIFY_CALL(GblOptionGroup_parse(pFixture->pGroup, pList));
    GBL_TEST_VERIFY(GblStringList_empty(pList));
    GBL_TEST_COMPARE(pFixture->pFirst, "Firsty");
    GblStringList_destroy(pList);
    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(parseShort) {
    GblStringList* pList = GblStringList_create("-3", "true");
    GBL_CTX_VERIFY_CALL(GblOptionGroup_parse(pFixture->pGroup, pList));
    GBL_TEST_VERIFY(GblStringList_empty(pList));
    GBL_TEST_COMPARE(pFixture->third, GBL_TRUE);
    GblStringList_destroy(pList);
    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(parseMultiMixed) {
    GblStringList* pList = GblStringList_create("-4",      "11.234",
                                                "--second", "347",
                                                "--fifth",  "X",
                                                "-6",       "-7123",
                                                "bullshit", "arg");

    GBL_CTX_VERIFY_CALL(GblOptionGroup_parse(pFixture->pGroup, pList));

    GBL_TEST_COMPARE(GblStringList_size(pList), 2);
    GBL_TEST_COMPARE(pFixture->fourth, 11.234);
    GBL_TEST_COMPARE(pFixture->second, 347);
    GBL_TEST_COMPARE(pFixture->fifth, 'X');
    GBL_TEST_COMPARE(pFixture->sixth, -7123);

    GblStringList_destroy(pList);
    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(parseBoolNoValueFailure) {
    pFixture->third = GBL_FALSE;
    GBL_TEST_EXPECT_ERROR();
    GblStringList* pList = GblStringList_create("--third");

    GBL_TEST_VERIFY(!GBL_RESULT_SUCCESS(GblOptionGroup_parse(pFixture->pGroup, pList)));

    GBL_TEST_COMPARE(GblStringList_size(pList), 1);
    GBL_TEST_COMPARE(pFixture->third, GBL_FALSE);

    GblStringList_destroy(pList);
    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(parseUnknown) {
    pFixture->third = GBL_FALSE;
    GblStringList* pList = GblStringList_create("--unknown", "-z");

    GBL_CTX_VERIFY_CALL(GblOptionGroup_parse(pFixture->pGroup, pList));

    GBL_TEST_COMPARE(GblStringList_size(pList), 2);

    GblStringList_destroy(pList);
    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(parseNonOptionMatch) {
    GblStringList* pList = GblStringList_create("first", "-1", "Bowie", "third");

    GBL_CTX_VERIFY_CALL(GblOptionGroup_parse(pFixture->pGroup, pList));
    GBL_TEST_COMPARE(pFixture->pFirst, "Bowie");
    GBL_TEST_COMPARE(GblStringList_size(pList), 2);
    GblStringList_destroy(pList);
    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(parseDoubleHyphenExtras) {
    GblStringList* pList = GblStringList_create("-1", "Ziggy", "--", "--second", "347");

    GBL_CTX_VERIFY_CALL(GblOptionGroup_parse(pFixture->pGroup, pList));
    GBL_TEST_COMPARE(GblStringList_size(pList), 3);
    GBL_TEST_COMPARE(pFixture->pFirst, "Ziggy");
    GblStringList_destroy(pList);
    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(parsePrefixedWrong) {
    GblStringList* pList = GblStringList_create("--wrong-first", "Ziggy", "--wrong-second", "347");

    GBL_CTX_VERIFY_CALL(GblOptionGroup_parse(pFixture->pGroup, pList));
    GBL_TEST_COMPARE(GblStringList_size(pList), 4);
    GblStringList_destroy(pList);
    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(parsePrefixedLong) {
    GblStringList* pList = GblStringList_create("--test-fourth", "20.234",
                                                "--test-second", "348",
                                                "--test-fifth",  "Z",
                                                "--test-sixth",  "-6123");

    GBL_CTX_VERIFY_CALL(GblOptionGroup_parse(pFixture->pGroup, pList));

    GBL_TEST_COMPARE(GblStringList_size(pList), 0);
    GBL_TEST_COMPARE(pFixture->fourth, 20.234);
    GBL_TEST_COMPARE(pFixture->second, 348);
    GBL_TEST_COMPARE(pFixture->fifth, 'Z');
    GBL_TEST_COMPARE(pFixture->sixth, -6123);

    GblStringList_destroy(pList);
    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(parsePrefixedShort) {
    GblStringList* pList = GblStringList_create("-test-4", "21.234",
                                                "-test-2", "349",
                                                "-test-5",  "W",
                                                "-test-6",  "-6124");

    GBL_CTX_VERIFY_CALL(GblOptionGroup_parse(pFixture->pGroup, pList));

    GBL_TEST_COMPARE(GblStringList_size(pList), 8);
    GBL_TEST_COMPARE(pFixture->fourth, 20.234);
    GBL_TEST_COMPARE(pFixture->second, 348);
    GBL_TEST_COMPARE(pFixture->fifth, 'Z');
    GBL_TEST_COMPARE(pFixture->sixth, -6123);

    GblStringList_destroy(pList);
    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(parseCallback) {
    GblStringList* pList = GblStringList_create("--seventh", "3.14", "--wrong-second", "347");

    GBL_CTX_VERIFY_CALL(GblOptionGroup_parse(pFixture->pGroup, pList));
    GBL_TEST_COMPARE(GblStringList_size(pList), 2);
    GBL_TEST_COMPARE(pFixture->seventh, 3.14f);

    GblStringList_destroy(pList);
    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(parseCallbackNoArg) {
    GblStringList* pList = GblStringList_create("-8", "3.14", "--wrong-second", "347");

    GBL_CTX_VERIFY_CALL(GblOptionGroup_parse(pFixture->pGroup, pList));
    GBL_TEST_COMPARE(GblStringList_size(pList), 3);
    GBL_TEST_COMPARE(pFixture->eighth, GBL_TRUE);

    GblStringList_destroy(pList);
    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(parseEquals) {
    GblStringList* pList = GblStringList_create("--second=8", "3.14", "--seventh=4.14");

    GBL_CTX_VERIFY_CALL(GblOptionGroup_parse(pFixture->pGroup, pList));
    GBL_TEST_COMPARE(GblStringList_size(pList), 1);
    GBL_TEST_COMPARE(pFixture->second, 8);
    GBL_TEST_COMPARE(pFixture->seventh, 4.14f);

    GblStringList_destroy(pList);
    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(parseEqualsShort) {
    GblStringList* pList = GblStringList_create("-4=21.5", "3.14", "-5=K");

    GBL_CTX_VERIFY_CALL(GblOptionGroup_parse(pFixture->pGroup, pList));
    GBL_TEST_COMPARE(GblStringList_size(pList), 1);
    GBL_TEST_COMPARE(pFixture->fourth, 21.5f);
    GBL_TEST_COMPARE(pFixture->fifth, 'K');

    GblStringList_destroy(pList);
    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(parseEqualsOption) {
    GblStringList* pList = GblStringList_create("-1=--third", "--first=--fourth");

    GBL_CTX_VERIFY_CALL(GblOptionGroup_parse(pFixture->pGroup, pList));
    GBL_TEST_COMPARE(GblStringList_size(pList), 0);
    GBL_TEST_COMPARE(pFixture->pFirst, "--fourth");
    GblStringList_destroy(pList);
    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(parseOptionToStringFailure) {
    GBL_TEST_EXPECT_ERROR();

    GblStringList* pList = GblStringList_create("--first", "--third");
    GBL_TEST_VERIFY(!GBL_RESULT_SUCCESS(GblOptionGroup_parse(pFixture->pGroup, pList)));
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_TEST_COMPARE(GblStringList_size(pList), 2);
    GblStringList_destroy(pList);
    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(parseBoolInvertedNoValue) {
    GblStringList* pList = GblStringList_create("--tenth");
    pFixture->tenth = GBL_TRUE;
    GBL_TEST_VERIFY(GBL_RESULT_SUCCESS(GblOptionGroup_parse(pFixture->pGroup, pList)));
    GBL_TEST_COMPARE(GblStringList_size(pList), 0);
    GBL_TEST_VERIFY(!pFixture->tenth);
    GblStringList_destroy(pList);
    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(parseBoolInverted) {
    GblStringList* pList = GblStringList_create("-a", "true");
    pFixture->eleventh = GBL_TRUE;
    GBL_TEST_VERIFY(GBL_RESULT_SUCCESS(GblOptionGroup_parse(pFixture->pGroup, pList)));
    GBL_TEST_COMPARE(GblStringList_size(pList), 0);
    GBL_TEST_VERIFY(!pFixture->eleventh);
    GblStringList_destroy(pList);
    GBL_TEST_CASE_END;
}

static void parsePrePass_(GblOptionGroup* pSelf, GblStringList* pList) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(NULL);
    GblStringList_remove(pList, "lolol");
    GBL_CTX_END_BLOCK();
}

GBL_TEST_CASE(parsePrePassTake) {
    GblStringList* pList = GblStringList_create("lolol");
    GBL_CTX_VERIFY_CALL(GblSignal_connect(GBL_INSTANCE(pFixture->pGroup),
                                          "parsePrePass",
                                          GBL_INSTANCE(pFixture->pGroup),
                                          GBL_CALLBACK(parsePrePass_)));
    GBL_CTX_VERIFY_CALL(GblOptionGroup_parse(pFixture->pGroup, pList));
    GBL_TEST_VERIFY(GblStringList_empty(pList));
    GblStringList_destroy(pList);
    GBL_TEST_CASE_END;
}

static void parsePrePassError_(GblOptionGroup* pSelf, GblStringList* pList) {
    GBL_UNUSED(pList);
    pSelf->parseResult = GBL_RESULT_ERROR_INVALID_CMDLINE_ARG;
}

GBL_TEST_CASE(parsePrePassError) {
    GBL_TEST_EXPECT_ERROR();

    GblStringList* pList = GblStringList_create("lolol");

    GBL_CTX_VERIFY_CALL(GblSignal_connect(GBL_INSTANCE(pFixture->pGroup),
                                          "parsePrePass",
                                          GBL_INSTANCE(pFixture->pGroup),
                                          GBL_CALLBACK(parsePrePassError_)));
    GBL_TEST_COMPARE(GblOptionGroup_parse(pFixture->pGroup, pList),
                     GBL_RESULT_ERROR_INVALID_CMDLINE_ARG);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_TEST_VERIFY(GblStringList_empty(pList));
    GblStringList_destroy(pList);
    GBL_TEST_COMPARE(GblSignal_disconnect(GBL_INSTANCE(pFixture->pGroup),
                                          "parsePrePass",
                                           GBL_INSTANCE(pFixture->pGroup),
                                           NULL),
                     2);
    GBL_TEST_CASE_END;
}


static void parsePostPass_(GblOptionGroup* pSelf, GblStringList* pList) {
    GBL_UNUSED(pSelf);
    GblStringList_remove(pList, "extraArg");
}

GBL_TEST_CASE(parsePostPassTake) {
    // Create cmd-line argumen tlist
    GblStringList* pList = GblStringList_create("--test-fourth", "25.234",
                                                "--test-second", "347",
                                                "--test-fifth",  "Q",
                                                "--test-sixth",  "-2125",
                                                "extraArg"); //ignored by main pass

    // install our post-pass logic as slot
    GBL_CTX_VERIFY_CALL(GblSignal_connect(GBL_INSTANCE(pFixture->pGroup),
                                          "parsePostPass",
                                          GBL_INSTANCE(pFixture->pGroup),
                                          GBL_CALLBACK(parsePostPass_)));
    // parse the arguments
    GBL_CTX_VERIFY_CALL(GblOptionGroup_parse(pFixture->pGroup, pList));

    // ensure all arguments were consumed
    GBL_TEST_COMPARE(GblStringList_size(pList), 0);

    // check that all known values were properly set
    GBL_TEST_COMPARE(pFixture->fourth, 25.234);
    GBL_TEST_COMPARE(pFixture->second, 347);
    GBL_TEST_COMPARE(pFixture->fifth, 'Q');
    GBL_TEST_COMPARE(pFixture->sixth, -2125);

    // destroy argument list
    GblStringList_destroy(pList);
    GBL_TEST_CASE_END;
}

static void parsePostPassError_(GblOptionGroup* pSelf, GblStringList* pList) {
    GBL_UNUSED(pList);
    pSelf->parseResult = GBL_RESULT_ERROR_INVALID_CMDLINE_ARG;
}

GBL_TEST_CASE(parsePostPassError) {
    GBL_TEST_EXPECT_ERROR();

    GblStringList* pList = GblStringList_create("lolol");

    GBL_CTX_VERIFY_CALL(GblSignal_connect(GBL_INSTANCE(pFixture->pGroup),
                                          "parsePostPass",
                                          GBL_INSTANCE(pFixture->pGroup),
                                          GBL_CALLBACK(parsePostPassError_)));
    GBL_TEST_COMPARE(GblOptionGroup_parse(pFixture->pGroup, pList),
                     GBL_RESULT_ERROR_INVALID_CMDLINE_ARG);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_TEST_VERIFY(!GblStringList_empty(pList));
    GblStringList_destroy(pList);
    GBL_TEST_CASE_END;
}

static void parseErrorNotify_(GblOptionGroup* pSelf, GBL_RESULT result) {
    GBL_UNUSED(pSelf);
    GblCClosure* pClosure = GBL_C_CLOSURE(GblClosure_current());
    GblBool* pBool = GblBox_userdata(GBL_BOX(pClosure));
    if(result == GBL_RESULT_ERROR_INVALID_CMDLINE_ARG)
        *pBool = GBL_TRUE;
}

GBL_TEST_CASE(parseErrorNotify) {
    GBL_TEST_EXPECT_ERROR();

    GblStringList* pList = GblStringList_create("lolol");

    GblBool notified = GBL_FALSE;
    GblCClosure* pClosure = GblCClosure_create(GBL_CALLBACK(parseErrorNotify_),
                                               &notified);

    GBL_CTX_VERIFY_CALL(GblSignal_connectClosure(GBL_INSTANCE(pFixture->pGroup),
                                                 "parseError",
                                                 GBL_INSTANCE(pFixture->pGroup),
                                                 GBL_CLOSURE(pClosure)));

    GBL_TEST_COMPARE(GblOptionGroup_parse(pFixture->pGroup, pList),
                     GBL_RESULT_ERROR_INVALID_CMDLINE_ARG);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_VERIFY(notified);
    GBL_TEST_VERIFY(!GblStringList_empty(pList));
    GblStringList_destroy(pList);
    GBL_TEST_CASE_END;
}


GBL_TEST_CASE(destroy) {
    GBL_TEST_COMPARE(GBL_UNREF(pFixture->pGroup), 0);
    GBL_TEST_CASE_END;
}

GBL_TEST_REGISTER(create,
                  parseLong,
                  parseShort,
                  parseMultiMixed,
                  parseBoolNoValueFailure,
                  parseUnknown,
                  parseNonOptionMatch,
                  parseDoubleHyphenExtras,
                  parsePrefixedWrong,
                  parsePrefixedLong,
                  parsePrefixedShort,
                  parseCallback,
                  parseCallbackNoArg,
                  parseEquals,
                  parseEqualsShort,
                  parseEqualsOption,
                  parseOptionToStringFailure,
                  parseBoolInvertedNoValue,
                  parseBoolInverted,
                  parsePrePassTake,
                  parsePrePassError,
                  parsePostPassTake,
                  parsePostPassError,
                  parseErrorNotify,
                  destroy);

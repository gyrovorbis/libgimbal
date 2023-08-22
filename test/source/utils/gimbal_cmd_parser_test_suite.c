#include "utils/gimbal_cmd_parser_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/utils/gimbal_cmd_parser.h>
#include <gimbal/utils/gimbal_ref.h>

#define GBL_SELF_TYPE GblCmdParserTestSuite

GBL_TEST_FIXTURE {
    GblCmdParser* pParser;
    GblRefCount   refCount;

    const char*   pFirst;
    uint32_t      second;
    GblBool       third;

    float         fourth;
    char          fifth;

    const char*   pSixth;

    const char*   pPos1;
    int16_t       pos2;
    double        pos3;
};

GBL_TEST_INIT() {
    pFixture->refCount = GblRef_activeCount();
    GBL_TEST_CASE_END;
}

GBL_TEST_FINAL() {
    GBL_TEST_COMPARE(pFixture->refCount, GblRef_activeCount());
    GBL_TEST_CASE_END;
}

static GBL_RESULT parseOpt6_(GblOptionGroup*  pGroup,
                             const GblOption* pOption,
                             GblStringView    value,
                             GblBool*         pConsumed)
{
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(pOption);

    GblCmdParserTestSuite_* pFixture = GblBox_userdata(GBL_BOX(pGroup));
    pFixture->pSixth = value.pData;
    *pConsumed = GBL_TRUE;

    GBL_CTX_END();
}

GBL_TEST_CASE(create) {
    pFixture->pParser =
        (GblCmdParser*)GblObject_create(GBL_CMD_PARSER_TYPE,
            "enableHelpOption",    GBL_TRUE,
            "enableVersionOption", GBL_TRUE,
            "mainOptionGroup",     GblObject_create(GBL_OPTION_GROUP_TYPE,
                 "name",        "Application",
                 "prefix",      "app",
                 "version",     GblVersion_make(0, 0, 1),
                 "summary",     "Da Main Group...",
                 "description", "Manages options for main group",
                 "options",     (GblOption[]) {
                     { "first",   '1', GBL_OPTION_TYPE_STRING, { &pFixture->pFirst }, "1st value", "value1", GBL_OPTION_FLAG_NONE },
                     { "second",  '2', GBL_OPTION_TYPE_INT32,  { &pFixture->second }, "2nd value", "value2", GBL_OPTION_FLAG_NONE },
                     { "third",   '3', GBL_OPTION_TYPE_BOOL,   { &pFixture->third  }, "3rd value", "value3", GBL_OPTION_FLAG_BOOL_NO_VALUE },
                     { 0 }
                 },
                 NULL
             ),
             "optionGroups", (GblOptionGroup*[]) {
                (GblOptionGroup*)GblObject_create(GBL_OPTION_GROUP_TYPE,
                     "name",        "Module 1",
                     "prefix",      "mod1",
                     "version",     GblVersion_make(6, 5, 0),
                     "summary",     "Da First Option Group...",
                     "description", "Manages options for first group",
                     "options",     (GblOption[]) {
                         { "fourth", '4', GBL_OPTION_TYPE_FLOAT, { &pFixture->fourth }, "4th value", "value4", GBL_OPTION_FLAG_HIDDEN },
                         { "fifth",  '5', GBL_OPTION_TYPE_CHAR,  { &pFixture->fifth },  "5th value", "value5", GBL_OPTION_FLAG_NONE },
                         { 0 }
                     },
                     NULL
                 ),
                 (GblOptionGroup*)GblObject_create(GBL_OPTION_GROUP_TYPE,
                     "name",        "Module 2",
                     "prefix",      "mod2",
                     "version",     GblVersion_make(3, 77, 12),
                     "summary",     "Da second option group...",
                     "description", "Manages options for second group",
                     "userdata",    pFixture,
                     "options",     (GblOption[]) {
                         { "sixth", '6', GBL_OPTION_TYPE_CALLBACK, { .pFn = parseOpt6_ }, "6th value", "value1", GBL_OPTION_FLAG_NONE },
                         { 0 }
                     },
                     NULL
                 ),
                 NULL
             },
             "positionalArgs", (GblCmdArg[]) {
                 { "arg1", "Positional arg #1 (string)" },
                 { "arg2", "Positional arg #2 (int16)"  },
                 { "arg3", "Positional arg #3 (double)" },
                 { 0 }
             },
             NULL
        );

    GBL_TEST_VERIFY(!pFixture->pParser->allowExtraArgs);
    GBL_TEST_VERIFY(!pFixture->pParser->allowUnknownOptions);
    GBL_TEST_VERIFY(pFixture->pParser->firstArgAsExecutable);
    GBL_TEST_VERIFY(pFixture->pParser->enableHelpOption);
    GBL_TEST_VERIFY(pFixture->pParser->enableVersionOption);
    GBL_TEST_COMPARE(pFixture->pParser->parseResult, GBL_RESULT_UNKNOWN);
    GBL_TEST_VERIFY(!pFixture->pParser->pErrorMsg);
    GBL_TEST_COMPARE(GblCmdParser_optionGroupCount(pFixture->pParser), 2);
    GBL_TEST_COMPARE(GblCmdParser_positionalArgCount(pFixture->pParser), 3);
    GBL_TEST_COMPARE(GblCmdParser_executable(pFixture->pParser), NULL);
    GBL_TEST_VERIFY(GblStringList_empty(GblCmdParser_unknownOptions(pFixture->pParser)));
    GBL_TEST_COMPARE(GblCmdParser_positionalArgValueCount(pFixture->pParser), 0);

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(positionalArgsInsufficientInvalid) {
    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblCmdParser_parse(pFixture->pParser, GblStringList_create("lolol")),
                     GBL_RESULT_ERROR_INVALID_CMDLINE_ARG);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(pFixture->pParser->parseResult,
                     GBL_RESULT_ERROR_INVALID_CMDLINE_ARG);
    GBL_TEST_VERIFY(pFixture->pParser->pErrorMsg);

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(positionalArgsExtrasInvalid) {
    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblCmdParser_parse(pFixture->pParser,
                                        GblStringList_create("lolol", "2", "3", "4", "5")),
                     GBL_RESULT_ERROR_INVALID_CMDLINE_ARG);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(pFixture->pParser->parseResult,
                     GBL_RESULT_ERROR_INVALID_CMDLINE_ARG);
    GBL_TEST_VERIFY(pFixture->pParser->pErrorMsg);

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(positionalArgs) {
    GBL_CTX_VERIFY_CALL(GblCmdParser_parse(pFixture->pParser,
                                           GblStringList_create("exe", "string", "32", "4.0353")));

    GBL_TEST_COMPARE(pFixture->pParser->parseResult, GBL_RESULT_SUCCESS);
    GBL_TEST_VERIFY(!pFixture->pParser->pErrorMsg);

    GBL_TEST_COMPARE(GblCmdParser_executable(pFixture->pParser), "exe");
    GBL_TEST_COMPARE(GblCmdParser_positionalArgValueCount(pFixture->pParser), 3);

    GBL_CTX_VERIFY_CALL(GblCmdParser_positionalArgValue(pFixture->pParser,
                                                        0,
                                                        GBL_STRING_TYPE,
                                                        &pFixture->pPos1));
    GBL_TEST_COMPARE(pFixture->pPos1, "string");

    GBL_CTX_VERIFY_CALL(GblCmdParser_positionalArgValue(pFixture->pParser,
                                                        1,
                                                        GBL_INT16_TYPE,
                                                        &pFixture->pos2));
    GBL_TEST_COMPARE(pFixture->pos2, 32);

    GBL_CTX_VERIFY_CALL(GblCmdParser_positionalArgValue(pFixture->pParser,
                                                        2,
                                                        GBL_DOUBLE_TYPE,
                                                        &pFixture->pos3));
    GBL_TEST_COMPARE(pFixture->pos3, 4.0353);

    GBL_TEST_CASE_END;
}


GBL_TEST_CASE(positionalArgsNoExecutable) {
    pFixture->pParser->firstArgAsExecutable = GBL_FALSE;
    GBL_CTX_VERIFY_CALL(GblCmdParser_parse(pFixture->pParser,
                                           GblStringList_create("testzor", "99", "0.0353")));

    GBL_TEST_COMPARE(pFixture->pParser->parseResult, GBL_RESULT_SUCCESS);
    GBL_TEST_VERIFY(!pFixture->pParser->pErrorMsg);

    GBL_TEST_COMPARE(GblCmdParser_executable(pFixture->pParser), NULL);
    GBL_TEST_COMPARE(GblCmdParser_positionalArgValueCount(pFixture->pParser), 3);

    GBL_CTX_VERIFY_CALL(GblCmdParser_positionalArgValue(pFixture->pParser,
                                                        0,
                                                        GBL_STRING_TYPE,
                                                        &pFixture->pPos1));
    GBL_TEST_COMPARE(pFixture->pPos1, "testzor");

    GBL_CTX_VERIFY_CALL(GblCmdParser_positionalArgValue(pFixture->pParser,
                                                        1,
                                                        GBL_INT16_TYPE,
                                                        &pFixture->pos2));
    GBL_TEST_COMPARE(pFixture->pos2, 99);

    GBL_CTX_VERIFY_CALL(GblCmdParser_positionalArgValue(pFixture->pParser,
                                                        2,
                                                        GBL_DOUBLE_TYPE,
                                                        &pFixture->pos3));
    GBL_TEST_COMPARE(pFixture->pos3, 0.0353);

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(positionalArgsAllowExtra) {
    pFixture->pParser->allowExtraArgs = GBL_TRUE;

    GBL_CTX_VERIFY_CALL(GblCmdParser_parse(pFixture->pParser,
                                           GblStringList_create("--", "wince", "-99",
                                                                "-6.0353", "extra1", "true")));

    GBL_TEST_COMPARE(pFixture->pParser->parseResult, GBL_RESULT_SUCCESS);
    GBL_TEST_VERIFY(!pFixture->pParser->pErrorMsg);

    GBL_TEST_COMPARE(GblCmdParser_executable(pFixture->pParser), NULL);
    GBL_TEST_COMPARE(GblCmdParser_positionalArgValueCount(pFixture->pParser), 5);

    GBL_CTX_VERIFY_CALL(GblCmdParser_positionalArgValue(pFixture->pParser,
                                                        0,
                                                        GBL_STRING_TYPE,
                                                        &pFixture->pPos1));
    GBL_TEST_COMPARE(pFixture->pPos1, "wince");

    GBL_CTX_VERIFY_CALL(GblCmdParser_positionalArgValue(pFixture->pParser,
                                                        1,
                                                        GBL_INT16_TYPE,
                                                        &pFixture->pos2));
    GBL_TEST_COMPARE(pFixture->pos2, -99);

    GBL_CTX_VERIFY_CALL(GblCmdParser_positionalArgValue(pFixture->pParser,
                                                        2,
                                                        GBL_DOUBLE_TYPE,
                                                        &pFixture->pos3));
    GBL_TEST_COMPARE(pFixture->pos3, -6.0353);

    const char* pArg4 = NULL;
    GBL_CTX_VERIFY_CALL(GblCmdParser_positionalArgValue(pFixture->pParser,
                                                        3,
                                                        GBL_STRING_TYPE,
                                                        &pArg4));
    GBL_TEST_COMPARE(pArg4, "extra1");

    GblBool arg5 = GBL_FALSE;
    GBL_CTX_VERIFY_CALL(GblCmdParser_positionalArgValue(pFixture->pParser,
                                                        4,
                                                        GBL_BOOL_TYPE,
                                                        &arg5));
    GBL_TEST_COMPARE(arg5, GBL_TRUE);

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(positionalArgsUnknownOptionInvalid) {
    pFixture->pParser->firstArgAsExecutable = GBL_TRUE;
    pFixture->pParser->allowExtraArgs = GBL_FALSE;
    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblCmdParser_parse(pFixture->pParser,
                                        GblStringList_create("exe", "string", "32", "4.0353", "-fourth")),
                     GBL_RESULT_ERROR_INVALID_CMDLINE_ARG);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(pFixture->pParser->parseResult, GBL_RESULT_ERROR_INVALID_CMDLINE_ARG);
    GBL_TEST_VERIFY(pFixture->pParser->pErrorMsg);

    GBL_TEST_COMPARE(GblCmdParser_positionalArgValueCount(pFixture->pParser), 3);

    const GblStringList* pUnknowns = GblCmdParser_unknownOptions(pFixture->pParser);
    GBL_TEST_COMPARE(GblStringList_size(pUnknowns), 1);
    GBL_TEST_COMPARE(GblStringList_front(pUnknowns), "-fourth");

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(positionalArgsMainOptions) {
    GBL_CTX_VERIFY_CALL(GblCmdParser_parse(pFixture->pParser,
                                           GblStringList_create("exe", "string", "32", "4.0353",
                                                                "--first", "first", "--app-second", "-1234", "-3")));

    GBL_TEST_COMPARE(pFixture->pParser->parseResult, GBL_RESULT_SUCCESS);
    GBL_TEST_VERIFY(!pFixture->pParser->pErrorMsg);

    GBL_TEST_COMPARE(pFixture->pFirst, "first");
    GBL_TEST_COMPARE(pFixture->second, -1234);
    GBL_TEST_COMPARE(pFixture->third, GBL_TRUE);

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(positionalArgsMainOptionsAllowUnknowns) {
    pFixture->pParser->allowUnknownOptions = GBL_TRUE;
    pFixture->third = GBL_FALSE;

    GBL_CTX_VERIFY_CALL(GblCmdParser_parse(pFixture->pParser,
                                           GblStringList_create("exe", "stampede", "33", "9.0353",
                                                                "-1=no", "--app-second", "-13", "--unknown1", "-3", "-u")));

    GBL_TEST_COMPARE(pFixture->pParser->parseResult, GBL_RESULT_SUCCESS);
    GBL_TEST_VERIFY(!pFixture->pParser->pErrorMsg);

    GBL_TEST_COMPARE(pFixture->pFirst, "no");
    GBL_TEST_COMPARE(pFixture->second, -13);
    GBL_TEST_COMPARE(pFixture->third, GBL_TRUE);

    GBL_TEST_COMPARE(GblStringList_size(GblCmdParser_unknownOptions(pFixture->pParser)), 2);

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(positionalArgsAllOptions) {
    pFixture->pParser->allowUnknownOptions = GBL_FALSE;
    pFixture->third = GBL_FALSE;

    GblStringList* pArgs =
        GblStringList_create("/usr/local/app.exe", "stringy", "38", "17.0353",  //positional
                             "--first", "firsty", "--app-second", "1234", "-3", // main app options
                             "--mod1-fourth", "-125.76", "--mod1-fifth=b",      // module 1 options
                             "--mod2-sixth", "sixth");                          // module 2 options

    GBL_CTX_VERIFY_CALL(GblCmdParser_parse(pFixture->pParser, pArgs));

    // Verify no error status or messages
    GBL_TEST_COMPARE(pFixture->pParser->parseResult, GBL_RESULT_SUCCESS);
    GBL_TEST_VERIFY(!pFixture->pParser->pErrorMsg);

    // Verify executable path
    GBL_TEST_COMPARE(GblCmdParser_executable(pFixture->pParser), "/usr/local/app.exe");

    // Manually extract and verify positional arguments
    GBL_CTX_VERIFY_CALL(GblCmdParser_positionalArgValue(pFixture->pParser,
                                                        0,
                                                        GBL_STRING_TYPE,
                                                        &pFixture->pPos1));
    GBL_TEST_COMPARE(pFixture->pPos1, "stringy");

    GBL_CTX_VERIFY_CALL(GblCmdParser_positionalArgValue(pFixture->pParser,
                                                        1,
                                                        GBL_INT16_TYPE,
                                                        &pFixture->pos2));
    GBL_TEST_COMPARE(pFixture->pos2, 38);

    GBL_CTX_VERIFY_CALL(GblCmdParser_positionalArgValue(pFixture->pParser,
                                                        2,
                                                        GBL_DOUBLE_TYPE,
                                                        &pFixture->pos3));
    GBL_TEST_COMPARE(pFixture->pos3, 17.0353);

    // Verify main application's options
    GBL_TEST_COMPARE(pFixture->pFirst, "firsty");
    GBL_TEST_COMPARE(pFixture->second, 1234);
    GBL_TEST_COMPARE(pFixture->third, GBL_TRUE);

    // Verify module 1's options
    GBL_TEST_COMPARE(pFixture->fourth, -125.76f);
    GBL_TEST_COMPARE(pFixture->fifth, 'b');

    // Verify module 2's options
    GBL_TEST_COMPARE(pFixture->pSixth, "sixth");

    GBL_TEST_CASE_END;
}


GBL_TEST_CASE(positionalArgsAllOptionsDoubleHypen) {
    pFixture->third = GBL_FALSE;

    GblStringList* pArgs =
        GblStringList_create("/usr/local/app.elf",
                             "-1", "frustum", "--app-second=72",
                             "stringlet",
                             "--mod1-fourth=-77.76", "--mod1-fifth", "x",
                             "--mod2-sixth=sixty",
                             "--", "-38", "-17.0353");

    GBL_CTX_VERIFY_CALL(GblCmdParser_parse(pFixture->pParser, pArgs));

    // Verify no error status or messages
    GBL_TEST_COMPARE(pFixture->pParser->parseResult, GBL_RESULT_SUCCESS);
    GBL_TEST_VERIFY(!pFixture->pParser->pErrorMsg);

    // Verify executable path
    GBL_TEST_COMPARE(GblCmdParser_executable(pFixture->pParser), "/usr/local/app.elf");

    // Manually extract and verify positional arguments
    GBL_CTX_VERIFY_CALL(GblCmdParser_positionalArgValue(pFixture->pParser,
                                                        0,
                                                        GBL_STRING_TYPE,
                                                        &pFixture->pPos1));
    GBL_TEST_COMPARE(pFixture->pPos1, "stringlet");

    GBL_CTX_VERIFY_CALL(GblCmdParser_positionalArgValue(pFixture->pParser,
                                                        1,
                                                        GBL_INT16_TYPE,
                                                        &pFixture->pos2));
    GBL_TEST_COMPARE(pFixture->pos2, -38);

    GBL_CTX_VERIFY_CALL(GblCmdParser_positionalArgValue(pFixture->pParser,
                                                        2,
                                                        GBL_DOUBLE_TYPE,
                                                        &pFixture->pos3));
    GBL_TEST_COMPARE(pFixture->pos3, -17.0353);

    // Verify main application's options
    GBL_TEST_COMPARE(pFixture->pFirst, "frustum");
    GBL_TEST_COMPARE(pFixture->second, 72);
    GBL_TEST_COMPARE(pFixture->third, GBL_FALSE);

    // Verify module 1's options
    GBL_TEST_COMPARE(pFixture->fourth, -77.76f);
    GBL_TEST_COMPARE(pFixture->fifth, 'x');

    // Verify module 2's options
    GBL_TEST_COMPARE(pFixture->pSixth, "sixty");

    GBL_TEST_CASE_END;
}

// enable unknown options
// clear + repopulate positional args w/ multipass
// main app help option
// module help options
// version options
// process

GBL_TEST_CASE(destroy) {
    GBL_TEST_COMPARE(GBL_UNREF(pFixture->pParser), 0);
    GBL_TEST_CASE_END;
}

GBL_TEST_REGISTER(create,
                  positionalArgsInsufficientInvalid,
                  positionalArgsExtrasInvalid,
                  positionalArgs,
                  positionalArgsNoExecutable,
                  positionalArgsAllowExtra,
                  positionalArgsUnknownOptionInvalid,
                  positionalArgsMainOptions,
                  positionalArgsMainOptionsAllowUnknowns,
                  positionalArgsAllOptions,
                  positionalArgsAllOptionsDoubleHypen,
                  destroy);

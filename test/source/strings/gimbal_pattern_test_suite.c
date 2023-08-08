#include "strings/gimbal_pattern_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/strings/gimbal_pattern.h>
#include <gimbal/strings/gimbal_string_buffer.h>

#define GBL_SELF_TYPE GblPatternTestSuite

GBL_TEST_FIXTURE {
    const GblPattern* pPattern;
};

GBL_TEST_INIT()
GBL_TEST_CASE_END

GBL_TEST_FINAL()
GBL_TEST_CASE_END

GBL_TEST_CASE(compileInvalid)
    GBL_TEST_COMPARE(GblPattern_create(NULL), NULL);
GBL_TEST_CASE_END

GBL_TEST_CASE(compile)
    pFixture->pPattern = GblPattern_create("[0-9]{8}");
GBL_TEST_CASE_END

GBL_TEST_CASE(stringify)
    GblStringBuffer buff;

    GblStringBuffer_construct(&buff);

    GBL_TEST_COMPARE(GblPattern_string(pFixture->pPattern, &buff), "[0-9]{8}");

    GblStringBuffer_destruct(&buff);

GBL_TEST_CASE_END

GBL_TEST_CASE(matchInvalid)
    GBL_TEST_EXPECT_ERROR();

    GblStringView match;

    int count = 0;
    GBL_TEST_VERIFY(!GblPattern_match(NULL,
                                      "12345678",
                                      &match,
                                      &count));
    GBL_TEST_VERIFY(GblStringView_empty(match));
    GBL_TEST_COMPARE(count, 0);

    count = -1;
    GBL_TEST_VERIFY(!GblPattern_match(pFixture->pPattern,
                                      NULL,
                                      &match,
                                      &count));
    GBL_TEST_VERIFY(GblStringView_empty(match));
    GBL_TEST_COMPARE(count, 0);
GBL_TEST_CASE_END

GBL_TEST_CASE(matchNone)
    GblStringView match;
    int count = 0;

    GBL_TEST_VERIFY(GblPattern_match(pFixture->pPattern,
                                     "lolol",
                                     &match,
                                     &count));

    GBL_TEST_VERIFY(GblStringView_empty(match));
    GBL_TEST_COMPARE(count, 0);
GBL_TEST_CASE_END

GBL_TEST_CASE(matchDefaultMatchDefaultCount)
    GBL_TEST_VERIFY(GblPattern_match(pFixture->pPattern,
                                     "12345678"));
GBL_TEST_CASE_END

GBL_TEST_CASE(matchDefaultCount)
    GblStringView view;

    GBL_TEST_VERIFY(GblPattern_match(pFixture->pPattern,
                                     "12345678",
                                     &view));

    GBL_TEST_VERIFY(GblStringView_equals(view, GBL_STRV("12345678")));
GBL_TEST_CASE_END

GBL_TEST_CASE(match)
    GblStringView view;
    int count = 2;

    GBL_TEST_VERIFY(GblPattern_match(pFixture->pPattern,
                                     "12345678amd12345678",
                                     &view,
                                     &count));

    GBL_TEST_VERIFY(GblStringView_equals(view, GBL_STRV("12345678")));

    GBL_TEST_COMPARE(count, 2);
GBL_TEST_CASE_END

GBL_TEST_CASE(matchLast)
    GblStringView view;
    int count = -1;

    GBL_TEST_VERIFY(GblPattern_match(pFixture->pPattern,
                                     "12345678amd12345678",
                                     &view,
                                     &count));

    GBL_TEST_VERIFY(GblStringView_equals(view, GBL_STRV("12345678")));

    GBL_TEST_COMPARE(count, 2);
GBL_TEST_CASE_END

GBL_TEST_CASE(matchMultipleInARow)
    GblStringView view;
    int count = -1;

    GBL_TEST_VERIFY(GblPattern_match(pFixture->pPattern,
                                     "1111111122222222",
                                     &view,
                                     &count));

    GBL_TEST_VERIFY(GblStringView_equals(view, GBL_STRV("22222222")));
    GBL_TEST_COMPARE(count, 2);
GBL_TEST_CASE_END

GBL_TEST_CASE(matchNotInvalid)
    GBL_TEST_EXPECT_ERROR();

    GblStringView match;

    int count = 0;
    GBL_TEST_VERIFY(!GblPattern_matchNot(NULL,
                                         "12345678",
                                         &match,
                                         &count));
    GBL_TEST_VERIFY(GblStringView_empty(match));
    GBL_TEST_COMPARE(count, 0);

    count = -1;
    GBL_TEST_VERIFY(!GblPattern_matchNot(pFixture->pPattern,
                                         NULL,
                                         &match,
                                         &count));
    GBL_TEST_VERIFY(GblStringView_empty(match));
    GBL_TEST_COMPARE(count, 0);
GBL_TEST_CASE_END

GBL_TEST_CASE(matchNotNone)
    GblStringView match;
    int count = 0;

    GBL_TEST_VERIFY(GblPattern_matchNot(pFixture->pPattern,
                                        "12345678",
                                        &match,
                                        &count));
    GBL_TEST_VERIFY(GblStringView_empty(match));
GBL_TEST_CASE_END

GBL_TEST_CASE(matchNotDefaultMatchDefaultCount)
    GBL_TEST_VERIFY(!GblPattern_matchNot(pFixture->pPattern,
                                         "12345678"));
    GBL_TEST_VERIFY(GblPattern_matchNot(pFixture->pPattern,
                                        "amd"));
GBL_TEST_CASE_END

GBL_TEST_CASE(matchNotDefaultCount)
    GblStringView match;

    GBL_TEST_VERIFY(!GblPattern_matchNot(pFixture->pPattern,
                                         "12345678",
                                         &match));
    GBL_TEST_VERIFY(GblStringView_empty(match));

    GBL_TEST_VERIFY(GblPattern_matchNot(pFixture->pPattern,
                                        "amd12345678",
                                        &match));
    GBL_TEST_VERIFY(GblStringView_equals(match, GBL_STRV("amd")));
GBL_TEST_CASE_END

GBL_TEST_CASE(matchNot)
    GblStringView match;

    // Find second match
    int count = 2;
    GBL_TEST_VERIFY(GblPattern_matchNot(pFixture->pPattern,
                                        "12345678amd12345678nvidia12345678",
                                        &match,
                                        &count));
    GBL_TEST_VERIFY(GblStringView_equals(match, GBL_STRV("nvidia")));
    GBL_TEST_COMPARE(count, 2);
GBL_TEST_CASE_END

GBL_TEST_CASE(matchNotLast)
    GblStringView match;
    int           count;

    // Find last match
    count = -1;
    GBL_TEST_VERIFY(GblPattern_matchNot(pFixture->pPattern,
                                        "intel",
                                        &match,
                                        &count));
    GBL_TEST_VERIFY(GblStringView_equals(match, GBL_STRV("intel")));
    GBL_TEST_COMPARE(count, 1);

    // Find last match
    count = -1;
    GBL_TEST_VERIFY(GblPattern_matchNot(pFixture->pPattern,
                                        "12345678amd12345678nvidia12345678intel",
                                        &match,
                                        &count));
    GBL_TEST_VERIFY(GblStringView_equals(match, GBL_STRV("intel")));
    GBL_TEST_COMPARE(count, 3);
GBL_TEST_CASE_END

GBL_TEST_CASE(matchNotFirstMultiInARow)
    GblStringView match;
    int count;

    count = 1;
    GBL_TEST_VERIFY(GblPattern_matchNotStr("[ \n]",
                                           "  \ntoken",
                                           &match,
                                           &count));

    GBL_TEST_VERIFY(GblStringView_equals(match, GBL_STRV("token")));
    GBL_TEST_COMPARE(count, 1);
GBL_TEST_CASE_END

GBL_TEST_CASE(unref)
    GblPattern_unref(pFixture->pPattern);
GBL_TEST_CASE_END

GBL_TEST_CASE(matchLiteral)
    GblStringView view;
    int count = -1;
    GBL_TEST_VERIFY(GblPattern_matchStr("lol", "ablolcd", &view, &count));
    GBL_TEST_VERIFY(GblStringView_equals(view, GBL_STRV("lol")));
    GBL_TEST_COMPARE(count, 1);

    count = -1;
    GBL_TEST_VERIFY(GblPattern_matchStr("lol", "lolablolcdlol", &view, &count));
    GBL_TEST_VERIFY(GblStringView_equals(view, GBL_STRV("lol")));
    GBL_TEST_COMPARE(count, 3);

    count = 1;
    GBL_TEST_VERIFY(GblPattern_matchStr("lol", "lolablolcdlol", &view, &count));
    GBL_TEST_VERIFY(GblStringView_equals(view, GBL_STRV("lol")));
    GBL_TEST_COMPARE(count, 1);
GBL_TEST_CASE_END

GBL_TEST_CASE(matchLiteralBegin)
    GblStringView view;
    int count = -1;
    GBL_TEST_VERIFY(!GblPattern_matchStr("^lol", "ablolcd", &view, &count));
    GBL_TEST_VERIFY(GblStringView_empty(view));
    GBL_TEST_COMPARE(count, 0);

    count = -1;
    GBL_TEST_VERIFY(GblPattern_matchStr("^lol", "lolablolcdlol", &view, &count));
    GBL_TEST_VERIFY(GblStringView_equals(view, GBL_STRV("lol")));
    GBL_TEST_COMPARE(count, 1);

    count = 1;
    GBL_TEST_VERIFY(GblPattern_matchStr("^lol", "lolablolcdlol", &view, &count));
    GBL_TEST_VERIFY(GblStringView_equals(view, GBL_STRV("lol")));
    GBL_TEST_COMPARE(count, 1);
GBL_TEST_CASE_END

GBL_TEST_CASE(matchLiteralEnd)
    GblStringView view;
    int count = -1;
    GBL_TEST_VERIFY(!GblPattern_matchStr("lol$", "ablolcd", &view, &count));
    GBL_TEST_VERIFY(GblStringView_empty(view));
    GBL_TEST_COMPARE(count, 0);

    count = -1;
    GBL_TEST_VERIFY(GblPattern_matchStr("lol$", "lolablolcdlol", &view, &count));
    GBL_TEST_VERIFY(GblStringView_equals(view, GBL_STRV("lol")));
    GBL_TEST_COMPARE(count, 1);

    count = 1;
    GBL_TEST_VERIFY(GblPattern_matchStr("lol$", "lolablolcdlol", &view, &count));
    GBL_TEST_VERIFY(GblStringView_equals(view, GBL_STRV("lol")));
    GBL_TEST_COMPARE(count, 1);
GBL_TEST_CASE_END

GBL_TEST_CASE(matchDateTime)
    GblStringView view;
    int count = -1;
    GBL_TEST_VERIFY(GblPattern_matchStr("\\d{4}-[01]\\d-[0-3]\\dT[0-2]\\d:[0-5]\\d:[0-5]\\d",
                                        "2018-10-04T05:52:20",
                                        &view,
                                        &count));
    GBL_TEST_VERIFY(GblStringView_equals(view, GBL_STRV("2018-10-04T05:52:20")));
    GBL_TEST_COMPARE(count, 1);
GBL_TEST_CASE_END

GBL_TEST_REGISTER(compileInvalid,
                  compile,
                  stringify,
                  matchInvalid,
                  matchNone,
                  matchDefaultMatchDefaultCount,
                  matchDefaultCount,
                  match,
                  matchLast,
                  matchMultipleInARow,
                  matchNotInvalid,
                  matchNotNone,
                  matchNotDefaultMatchDefaultCount,
                  matchNotDefaultCount,
                  matchNot,
                  matchNotLast,
                  matchNotFirstMultiInARow,
                  matchLiteral,
                  matchLiteralBegin,
                  matchLiteralEnd,
                  matchDateTime,
                  unref)
#if 0

GBL_TEST_CASE(iso8601BasicDate)
    int length = 0;
    GBL_TEST_COMPARE(GblPattern_matchStr("^[0-9]{8}$", "12345678", &length), 0);
    GBL_TEST_COMPARE(length, 8);
GBL_TEST_CASE_END

GBL_TEST_CASE(iso8601)
    //20:42:09.051514 IP (tos 0x0, ttl 128, id 17307, offset 0, flags [DF], proto TCP (6), length 52)
    //int length = 0;
   // GblPattern* pPattern = GblPattern_compile("^\d{4}-\d\d-\d\dT\d\d:\d\d:\d\d(\.\d+)?(([+-]\d\d:\d\d)|Z)?$");
   // GBL_TEST_VERIFY(GblPattern_exactMatchc(pPattern, "2002-12-31T23:00:00+01:00"));
    //([0-9]{1,3}[\.]){3}[0-9]{1,3}
GBL_TEST_CASE_END
#endif

#include "utils/gimbal_scanner_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/utils/gimbal_scanner.h>

#define GBL_TEST_SUITE_SELF GblScannerTestSuite

GBL_TEST_FIXTURE {
    size_t      refCount;
    GblScanner* pEmptyScanner;
    GblScanner* pScanner;
    size_t      prevPos;
};

GBL_TEST_INIT()
    pFixture->refCount = GblRef_activeCount();
GBL_TEST_CASE_END

GBL_TEST_FINAL()
    GBL_TEST_COMPARE(GblRef_activeCount(), pFixture->refCount);
GBL_TEST_CASE_END

GBL_TEST_CASE(createEmpty)
    pFixture->pEmptyScanner = GblScanner_create(NULL, 0);
    GBL_TEST_VERIFY(pFixture->pEmptyScanner->status & GBL_SCANNER_EOF);
GBL_TEST_CASE_END

GBL_TEST_CASE(new)
    pFixture->pScanner = GBL_NEW(GblScanner,
                                 "delimeters", GBL_SCANNER_DELIMETERS_DEFAULT,
                                 "input",      "someshit 1283");

    GBL_TEST_COMPARE(GblScanner_input(pFixture->pScanner),
                     "someshit 1283");

    GBL_TEST_COMPARE(GblScanner_delimeters(pFixture->pScanner),
                     GBL_SCANNER_DELIMETERS_DEFAULT);

    GBL_TEST_COMPARE(GblScanner_tell(pFixture->pScanner), 0);

    GBL_TEST_VERIFY(GblStringView_empty(pFixture->pScanner->token));
    GBL_TEST_VERIFY(GblStringView_empty(pFixture->pScanner->next));

    GBL_TEST_COMPARE(pFixture->pScanner->status, GBL_SCANNER_OK);

    GBL_TEST_VERIFY(!pFixture->pScanner->pError);
GBL_TEST_CASE_END

GBL_TEST_CASE(peekTokenInvalid)
    GBL_TEST_VERIFY(!GblScanner_peekToken(pFixture->pEmptyScanner));
    GBL_TEST_VERIFY(pFixture->pEmptyScanner->status & GBL_SCANNER_PEEK_ERROR);
    GBL_TEST_VERIFY(GblStringView_empty(pFixture->pEmptyScanner->next));
GBL_TEST_CASE_END

GBL_TEST_CASE(scanTokenInvalid)
    GBL_TEST_VERIFY(!GblScanner_scanToken(pFixture->pEmptyScanner));
    GBL_TEST_VERIFY(pFixture->pEmptyScanner->status & GBL_SCANNER_SCAN_ERROR);
    GBL_TEST_VERIFY(pFixture->pEmptyScanner->status & GBL_SCANNER_EOF);
    GBL_TEST_VERIFY(GblStringView_empty(pFixture->pEmptyScanner->next));
    GBL_TEST_VERIFY(GblStringView_empty(pFixture->pEmptyScanner->token));
GBL_TEST_CASE_END

GBL_TEST_CASE(peekToken)
    GBL_TEST_VERIFY(GblScanner_peekToken(pFixture->pScanner));

    GBL_TEST_VERIFY(GblStringView_equals(pFixture->pScanner->next,
                                         GBL_STRV("someshit")));
    GBL_TEST_COMPARE(pFixture->pScanner->status, GBL_SCANNER_OK);

    GBL_TEST_VERIFY(GblScanner_peekToken(pFixture->pScanner));
    GBL_TEST_VERIFY(GblStringView_equals(pFixture->pScanner->next,
                                         GBL_STRV("someshit")));
    GBL_TEST_COMPARE(pFixture->pScanner->status, GBL_SCANNER_OK);
GBL_TEST_CASE_END

GBL_TEST_CASE(scanToken)
    GBL_TEST_VERIFY(GblScanner_scanToken(pFixture->pScanner));

    GBL_TEST_VERIFY(GblStringView_equals(pFixture->pScanner->token,
                                         GBL_STRV("someshit")));
    GBL_TEST_COMPARE(pFixture->pScanner->status, GBL_SCANNER_OK);

    GBL_TEST_VERIFY(GblScanner_scanToken(pFixture->pScanner));
    GBL_TEST_VERIFY(GblStringView_equals(pFixture->pScanner->token,
                                         GBL_STRV("1283")));
    GBL_TEST_COMPARE(pFixture->pScanner->status, GBL_SCANNER_EOF);
GBL_TEST_CASE_END

GBL_TEST_CASE(peekTokenEof)
    GBL_TEST_VERIFY(!GblScanner_peekToken(pFixture->pScanner));

    GBL_TEST_VERIFY(GblStringView_empty(pFixture->pScanner->next));
    GBL_TEST_COMPARE(pFixture->pScanner->status,
                     GBL_SCANNER_EOF | GBL_SCANNER_PEEK_ERROR);
GBL_TEST_CASE_END

GBL_TEST_CASE(scanTokenEof)
    GBL_TEST_VERIFY(!GblScanner_scanToken(pFixture->pScanner));

    GBL_TEST_VERIFY(GblStringView_empty(pFixture->pScanner->token));
    GBL_TEST_COMPARE(pFixture->pScanner->status,
                     GBL_SCANNER_EOF | GBL_SCANNER_SCAN_ERROR);
GBL_TEST_CASE_END

GBL_TEST_CASE(setInput)
    GblScanner_setInput(pFixture->pScanner, "1234 hello worldX:)");

    GBL_TEST_COMPARE(GblScanner_tell(pFixture->pScanner), 0);
    GBL_TEST_COMPARE(pFixture->pScanner->status, GBL_SCANNER_OK);
    GBL_TEST_VERIFY(GblStringView_empty(pFixture->pScanner->next));
    GBL_TEST_VERIFY(GblStringView_empty(pFixture->pScanner->token));

    GBL_TEST_VERIFY(GblScanner_scanToken(pFixture->pScanner));
    GBL_TEST_VERIFY(GblStringView_equals(pFixture->pScanner->token,
                                         GBL_STRV("1234")));
GBL_TEST_CASE_END

GBL_TEST_CASE(setDelimeters)
    GblScanner_setDelimeters(pFixture->pScanner, "[X]");

    GBL_TEST_VERIFY(GblScanner_scanToken(pFixture->pScanner));
    GBL_TEST_VERIFY(GblStringView_equals(pFixture->pScanner->token,
                                         GBL_STRV(" hello world")));
    GBL_TEST_COMPARE(pFixture->pScanner->status, GBL_SCANNER_OK);
GBL_TEST_CASE_END

GBL_TEST_CASE(tell)
    pFixture->prevPos = GblScanner_tell(pFixture->pScanner);
    GBL_TEST_COMPARE(pFixture->prevPos, 16);
GBL_TEST_CASE_END

GBL_TEST_CASE(reset)
    GblScanner_reset(pFixture->pScanner);

    GBL_TEST_VERIFY(GblStringView_empty(pFixture->pScanner->token));
    GBL_TEST_VERIFY(GblStringView_empty(pFixture->pScanner->next));
    GBL_TEST_COMPARE(GblScanner_tell(pFixture->pScanner), 0);
GBL_TEST_CASE_END

GBL_TEST_CASE(seek)
    GBL_TEST_VERIFY(GblScanner_seek(pFixture->pScanner, pFixture->prevPos));
    GBL_TEST_VERIFY(GblStringView_empty(pFixture->pScanner->token));
    GBL_TEST_VERIFY(GblStringView_empty(pFixture->pScanner->next));
    GBL_TEST_COMPARE(GblScanner_tell(pFixture->pScanner), 16);

    GBL_TEST_VERIFY(GblScanner_scanToken(pFixture->pScanner));
    GBL_TEST_VERIFY(GblStringView_equals(pFixture->pScanner->token,
                                         GBL_STRV(":)")));
    GBL_TEST_COMPARE(pFixture->pScanner->status, GBL_SCANNER_EOF);

    GBL_TEST_VERIFY(GblScanner_seek(pFixture->pScanner, -2));
    GBL_TEST_VERIFY(GblScanner_peekToken(pFixture->pScanner));
    GBL_TEST_VERIFY(GblStringView_equals(pFixture->pScanner->next,
                                         GBL_STRV(":)")));
    GBL_TEST_COMPARE(pFixture->pScanner->status, GBL_SCANNER_OK);
GBL_TEST_CASE_END

GBL_TEST_CASE(raiseError)
    GBL_TEST_EXPECT_ERROR();
    GblScanner_raiseError(pFixture->pScanner,
                          GBL_SCANNER_SCAN_ERROR,
                          "Failed to do some stuff!");

    GBL_TEST_COMPARE(pFixture->pScanner->status, GBL_SCANNER_SCAN_ERROR);
    GBL_TEST_COMPARE(pFixture->pScanner->pError,
                     "[line: 1, column: 18]: Failed to do some stuff!");
GBL_TEST_CASE_END

GBL_TEST_CASE(clearError)
    GblScanner_clearError(pFixture->pScanner);

    GBL_TEST_COMPARE(pFixture->pScanner->status, GBL_SCANNER_OK);
    GBL_TEST_VERIFY(!pFixture->pScanner->pError);
GBL_TEST_CASE_END

GBL_TEST_CASE(skipTokensInvalid)
    GBL_TEST_EXPECT_ERROR();
    GblObject_setProperties(GBL_OBJECT(pFixture->pScanner),
                            "delimeters", "[ \t\n\r]",
                            "input",      "dreamcast\tplaystation xbox\ngamecube\nvmu",
                            NULL);
    GBL_TEST_VERIFY(!GblScanner_skipTokens(pFixture->pScanner, 10));
    GBL_TEST_VERIFY(pFixture->pScanner->status & GBL_SCANNER_SKIP_ERROR);
GBL_TEST_CASE_END

GBL_TEST_CASE(skipTokens)
    GblScanner_reset(pFixture->pScanner);
    GBL_TEST_VERIFY(GblScanner_skipTokens(pFixture->pScanner, 3));
    GBL_TEST_VERIFY(GblScanner_peekToken(pFixture->pScanner));
    GBL_TEST_VERIFY(GblStringView_equals(pFixture->pScanner->next,
                                         GBL_STRV("gamecube")));
GBL_TEST_CASE_END

GBL_TEST_CASE(skipMatchInvalid)
    GBL_TEST_EXPECT_ERROR();
    GblScanner_reset(pFixture->pScanner);
    GBL_TEST_VERIFY(!GblScanner_skipMatch(pFixture->pScanner,
                                          "[z1-9]"));
    GBL_TEST_VERIFY(pFixture->pScanner->status & GBL_SCANNER_SKIP_ERROR);
GBL_TEST_CASE_END

GBL_TEST_CASE(skipMatch)
    GblScanner_reset(pFixture->pScanner);
    GBL_TEST_VERIFY(GblScanner_skipMatch(pFixture->pScanner,
                                         "\\Wplaystation\\W"));
    GBL_TEST_COMPARE(pFixture->pScanner->status, GBL_SCANNER_OK);
    GBL_TEST_VERIFY(GblScanner_peekToken(pFixture->pScanner));
    GBL_TEST_VERIFY(GblStringView_equals(pFixture->pScanner->next,
                                         GBL_STRV("xbox")));
GBL_TEST_CASE_END

GBL_TEST_CASE(skipToMatchInvalid)
    GBL_TEST_EXPECT_ERROR();
    GblScanner_reset(pFixture->pScanner);
    GBL_TEST_VERIFY(!GblScanner_skipToMatch(pFixture->pScanner,
                                            "[z1-9]"));
    GBL_TEST_VERIFY(pFixture->pScanner->status & GBL_SCANNER_SKIP_ERROR);
GBL_TEST_CASE_END

GBL_TEST_CASE(skipToMatch)
    GblScanner_reset(pFixture->pScanner);
    GBL_TEST_VERIFY(GblScanner_skipToMatch(pFixture->pScanner,
                                           "\\Wplaystation\\W"));
    GBL_TEST_COMPARE(pFixture->pScanner->status, GBL_SCANNER_OK);
    GBL_TEST_VERIFY(GblScanner_peekToken(pFixture->pScanner));
    GBL_TEST_VERIFY(GblStringView_equals(pFixture->pScanner->next,
                                         GBL_STRV("playstation")));
GBL_TEST_CASE_END

GBL_TEST_CASE(skipLinesInvalid)
    GBL_TEST_EXPECT_ERROR();
    GblScanner_reset(pFixture->pScanner);
    GBL_TEST_VERIFY(!GblScanner_skipLines(pFixture->pScanner,
                                          10));
    GBL_TEST_VERIFY(pFixture->pScanner->status & GBL_SCANNER_SKIP_ERROR);
    GBL_CTX_CLEAR_LAST_RECORD();
GBL_TEST_CASE_END

GBL_TEST_CASE(skipLines)
    GblScanner_reset(pFixture->pScanner);
    GBL_TEST_VERIFY(GblScanner_skipLines(pFixture->pScanner,
                                          2));
    GBL_TEST_COMPARE(pFixture->pScanner->status, GBL_SCANNER_OK);
    GBL_TEST_VERIFY(GblScanner_peekToken(pFixture->pScanner));
    GBL_TEST_VERIFY(GblStringView_equals(pFixture->pScanner->next,
                                         GBL_STRV("vmu")));
GBL_TEST_CASE_END

GBL_TEST_CASE(skipBytesInvalid)
    GBL_TEST_EXPECT_ERROR();
    GblScanner_reset(pFixture->pScanner);
    GBL_TEST_VERIFY(!GblScanner_skipBytes(pFixture->pScanner,
                                          99999));
    GBL_TEST_VERIFY(pFixture->pScanner->status & GBL_SCANNER_SKIP_ERROR);
    GBL_CTX_CLEAR_LAST_RECORD();
GBL_TEST_CASE_END

GBL_TEST_CASE(skipBytes)
    GblScanner_reset(pFixture->pScanner);
    GBL_TEST_VERIFY(GblScanner_skipBytes(pFixture->pScanner,
                                         5));
    GBL_TEST_COMPARE(pFixture->pScanner->status, GBL_SCANNER_OK);
    GBL_TEST_VERIFY(GblScanner_peekToken(pFixture->pScanner));
    GBL_TEST_VERIFY(GblStringView_equals(pFixture->pScanner->next,
                                         GBL_STRV("cast")));
GBL_TEST_CASE_END

GBL_TEST_CASE(scanf)
    GblScanner_setInput(pFixture->pScanner, "loolol 2041-04-20T14:01:55-0500");
    GblScanner_setDelimeters(pFixture->pScanner, "[ ]");

    GBL_TEST_VERIFY(GblScanner_scanToken(pFixture->pScanner));
    GBL_TEST_VERIFY(GblScanner_scanBytes(pFixture->pScanner, 1));

    unsigned year, month, day;
    unsigned hour, minute, second;
    signed utcOffset;
    GBL_TEST_COMPARE(GblScanner_scanf(pFixture->pScanner,
                                      "%4u-%2u-%2uT%2u:%2u:%2u%5d",
                                      &year, &month, &day,
                                      &hour, &minute, &second,
                                      &utcOffset),
                     7);

    GBL_TEST_COMPARE(year, 2041);
    GBL_TEST_COMPARE(month, 4);
    GBL_TEST_COMPARE(day, 20);
    GBL_TEST_COMPARE(hour, 14);
    GBL_TEST_COMPARE(minute, 1);
    GBL_TEST_COMPARE(second, 55);
    GBL_TEST_COMPARE(utcOffset, -500);
GBL_TEST_CASE_END

GBL_TEST_CASE(unref)
    GblScanner_unref(pFixture->pEmptyScanner);
    GBL_UNREF(pFixture->pScanner);
GBL_TEST_CASE_END

GBL_TEST_REGISTER(createEmpty,
                  new,
                  peekTokenInvalid,
                  scanTokenInvalid,
                  peekToken,
                  scanToken,
                  peekTokenEof,
                  scanTokenEof,
                  setInput,
                  setDelimeters,
                  tell,
                  reset,
                  seek,
                  raiseError,
                  clearError,
                  skipTokensInvalid,
                  skipTokens,
                  skipMatchInvalid,
                  skipMatch,
                  skipToMatchInvalid,
                  skipToMatch,
                  skipLinesInvalid,
                  skipLines,
                  skipBytesInvalid,
                  skipBytes,
                  scanf,
                  unref)

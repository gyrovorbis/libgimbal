#include "utils/gimbal_scanner_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/utils/gimbal_scanner.h>

#define GBL_SELF_TYPE GblScannerTestSuite

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
    GBL_TEST_EXPECT_ERROR();
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
    GBL_TEST_EXPECT_ERROR();
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

GBL_TEST_CASE(peekMatchInvalid)
    GblScanner_setInput(pFixture->pScanner, "1234 hello worldX:)");
    GBL_TEST_VERIFY(!GblScanner_peekMatch(pFixture->pEmptyScanner, "[ ]"));
    GBL_TEST_VERIFY(pFixture->pEmptyScanner->status & GBL_SCANNER_PEEK_ERROR);
    GBL_TEST_VERIFY(GblStringView_empty(pFixture->pEmptyScanner->next));
GBL_TEST_CASE_END

GBL_TEST_CASE(scanMatchInvalid)
    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_VERIFY(!GblScanner_scanMatch(pFixture->pEmptyScanner, "[ ]"));
    GBL_TEST_VERIFY(pFixture->pEmptyScanner->status & GBL_SCANNER_SCAN_ERROR);
    GBL_TEST_VERIFY(GblStringView_empty(pFixture->pEmptyScanner->token));
    GBL_TEST_VERIFY(GblStringView_empty(pFixture->pEmptyScanner->next));
GBL_TEST_CASE_END

GBL_TEST_CASE(peekMatch)
    GblScanner_setDelimeters(pFixture->pScanner, "[X]");
    GblScanner_skipTokens(pFixture->pScanner, 1);

    GBL_TEST_VERIFY(GblScanner_peekMatch(pFixture->pScanner, "^:\\)$"));
    GBL_TEST_VERIFY(GblStringView_equals(pFixture->pScanner->next, GBL_STRV(":)")));
    GBL_TEST_COMPARE(pFixture->pScanner->status, GBL_SCANNER_OK);
GBL_TEST_CASE_END

GBL_TEST_CASE(scanMatch)
    GBL_TEST_VERIFY(GblScanner_scanMatch(pFixture->pScanner, "^:\\)$"));
    GBL_TEST_VERIFY(GblStringView_equals(pFixture->pScanner->token, GBL_STRV(":)")));
    GBL_TEST_COMPARE(pFixture->pScanner->status, GBL_SCANNER_EOF);
GBL_TEST_CASE_END

GBL_TEST_CASE(peekLinesInvalid)
    GBL_TEST_EXPECT_ERROR();
    GBL_TEST_VERIFY(!GblScanner_peekLines(pFixture->pEmptyScanner, 2));
    GBL_TEST_VERIFY(pFixture->pEmptyScanner->status & GBL_SCANNER_PEEK_ERROR);
    GBL_TEST_VERIFY(GblStringView_empty(pFixture->pEmptyScanner->next));
GBL_TEST_CASE_END

GBL_TEST_CASE(scanLinesInvalid)
    GBL_TEST_EXPECT_ERROR();
    GblScanner_setInput(pFixture->pScanner, "\n\nHello\nWorld\n\nG'day");
    GblScanner_setDelimeters(pFixture->pScanner, "[\n]");
    GBL_TEST_VERIFY(!GblScanner_scanLines(pFixture->pEmptyScanner, 99));
    GBL_TEST_VERIFY(pFixture->pEmptyScanner->status & GBL_SCANNER_SCAN_ERROR);
    GBL_TEST_VERIFY(GblStringView_empty(pFixture->pEmptyScanner->token));
GBL_TEST_CASE_END

GBL_TEST_CASE(peekLines)
    GBL_TEST_VERIFY(GblScanner_peekLines(pFixture->pScanner, 3));
    GBL_TEST_VERIFY(GblStringView_equals(pFixture->pScanner->next, GBL_STRV("\n\nHello")));
    GBL_TEST_COMPARE(pFixture->pScanner->status, GBL_SCANNER_OK);
GBL_TEST_CASE_END

GBL_TEST_CASE(scanLines)
    GBL_TEST_VERIFY(GblScanner_scanLines(pFixture->pScanner, 5));
    GBL_TEST_VERIFY(GblStringView_equals(pFixture->pScanner->token, GBL_STRV("\n\nHello\nWorld\n")));
    GBL_TEST_COMPARE(pFixture->pScanner->status, GBL_SCANNER_OK);
GBL_TEST_CASE_END

GBL_TEST_CASE(peekBytesInvalid)
    GBL_TEST_EXPECT_ERROR();
    GblScanner_setInput(pFixture->pScanner, "123456789\nabcdefghijklmnop\nqrstuvyxyz\n");
    GblScanner_setDelimeters(pFixture->pScanner, "[\n]");
    GBL_TEST_VERIFY(!GblScanner_peekBytes(pFixture->pScanner, 999));
    GBL_TEST_VERIFY(GblStringView_empty(pFixture->pScanner->next));
    GBL_TEST_COMPARE(pFixture->pScanner->status, GBL_SCANNER_PEEK_ERROR);
GBL_TEST_CASE_END

GBL_TEST_CASE(scanBytesInvalid)
    GBL_TEST_EXPECT_ERROR();
    GBL_TEST_VERIFY(!GblScanner_scanBytes(pFixture->pScanner, 999));
    GBL_TEST_VERIFY(GblStringView_empty(pFixture->pScanner->token));
    GBL_TEST_COMPARE(pFixture->pScanner->status, GBL_SCANNER_SCAN_ERROR);
GBL_TEST_CASE_END

GBL_TEST_CASE(peekBytes)
    GBL_TEST_VERIFY(GblScanner_peekBytes(pFixture->pScanner, 10));
    GBL_TEST_VERIFY(GblStringView_equals(pFixture->pScanner->next, GBL_STRV("123456789\n")));
    GBL_TEST_COMPARE(pFixture->pScanner->status, GBL_SCANNER_OK);
GBL_TEST_CASE_END

GBL_TEST_CASE(scanBytes)
    GBL_TEST_VERIFY(GblScanner_scanBytes(pFixture->pScanner, 28));
    GBL_TEST_VERIFY(GblStringView_equals(pFixture->pScanner->token, GBL_STRV("123456789\nabcdefghijklmnop\nqrstuvyxyz\n")));
    GBL_TEST_COMPARE(pFixture->pScanner->status, GBL_SCANNER_EOF);
GBL_TEST_CASE_END

GBL_TEST_CASE(scanMatchMultiContiguous)
    GblScanner_setInput(pFixture->pScanner, "\n\nHello\n\tWorld\n \t\nG'day");
    GblScanner_setDelimeters(pFixture->pScanner, "[\n \t]");
    GBL_TEST_VERIFY(GblScanner_scanToken(pFixture->pScanner));
    GBL_TEST_VERIFY(GblStringView_equals(pFixture->pScanner->token, GBL_STRV("Hello")));
    GBL_TEST_VERIFY(GblScanner_scanToken(pFixture->pScanner));
    GBL_TEST_VERIFY(GblStringView_equals(pFixture->pScanner->token, GBL_STRV("World")));
    GBL_TEST_VERIFY(GblScanner_scanToken(pFixture->pScanner));
    GBL_TEST_VERIFY(GblStringView_equals(pFixture->pScanner->token, GBL_STRV("G'day")));
GBL_TEST_CASE_END

GBL_TEST_CASE(peekBoolInvalid)
    GblScanner_setInput(pFixture->pScanner, "\n\ntH\n1 false,0\t312,1024 \n \t\nG'day");
    GblScanner_setDelimeters(pFixture->pScanner, "[\n \t,]");

    GBL_TEST_EXPECT_ERROR();

    GblBool boolean = GBL_TRUE;
    GBL_TEST_VERIFY(!GblScanner_peekBool(pFixture->pScanner, &boolean));
    GBL_TEST_COMPARE(boolean, GBL_FALSE);

    GBL_CTX_CLEAR_LAST_RECORD();
GBL_TEST_CASE_END

GBL_TEST_CASE(peekBool)
    GblScanner_reset(pFixture->pScanner);
    GblScanner_skipTokens(pFixture->pScanner, 1);

    GblBool boolean = GBL_FALSE;

    GBL_TEST_VERIFY(GblScanner_peekBool(pFixture->pScanner, &boolean));
    GBL_TEST_VERIFY(boolean);

    GblScanner_skipTokens(pFixture->pScanner, 1);

    GBL_TEST_VERIFY(GblScanner_peekBool(pFixture->pScanner, &boolean));
    GBL_TEST_VERIFY(!boolean);

    GblScanner_skipTokens(pFixture->pScanner, 1);
    boolean = GBL_TRUE;

    GBL_TEST_VERIFY(GblScanner_peekBool(pFixture->pScanner, &boolean));
    GBL_TEST_VERIFY(!boolean);

    GblScanner_skipTokens(pFixture->pScanner, 1);
    GBL_TEST_VERIFY(GblScanner_peekBool(pFixture->pScanner, &boolean));
    GBL_TEST_VERIFY(boolean);
GBL_TEST_CASE_END

GBL_TEST_CASE(peekCharInvalid)
    GblScanner_setInput(pFixture->pScanner, "\n\ntH\nH y,23\t312,1024 \n \t\nG'day");
    GblScanner_setDelimeters(pFixture->pScanner, "[\n \t,]");

    GBL_TEST_EXPECT_ERROR();

    char character = '\0';
    GBL_TEST_VERIFY(!GblScanner_peekChar(pFixture->pScanner, &character));
    GBL_TEST_COMPARE(character, '\0');
    GBL_CTX_CLEAR_LAST_RECORD();
GBL_TEST_CASE_END

GBL_TEST_CASE(peekChar)
    GblScanner_skipTokens(pFixture->pScanner, 1);

    char character = '\0';
    GBL_TEST_VERIFY(GblScanner_peekChar(pFixture->pScanner, &character));
    GBL_TEST_COMPARE(character, 'H');

    GblScanner_skipTokens(pFixture->pScanner, 1);

    GBL_TEST_VERIFY(GblScanner_peek(pFixture->pScanner, &character));
    GBL_TEST_COMPARE(character, 'y');
GBL_TEST_CASE_END

GBL_TEST_CASE(peekUint8Invalid)
    GblScanner_setInput(pFixture->pScanner, "\n\ntH\n -12,265\t12,255 \n 0xa\t\nG'day");
    GblScanner_setDelimeters(pFixture->pScanner, "[\n \t,]");

    GBL_TEST_EXPECT_ERROR();

    uint8_t value = 0xff;
    GBL_TEST_VERIFY(!GblScanner_peekUint8(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 0);

    GblScanner_skipTokens(pFixture->pScanner, 1);
    value = 0xff;

    GBL_TEST_VERIFY(!GblScanner_peek(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 0);

    GblScanner_skipTokens(pFixture->pScanner, 1);
    value = 0xff;

    GBL_TEST_VERIFY(!GblScanner_peek(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 0);
    GBL_CTX_CLEAR_LAST_RECORD();
GBL_TEST_CASE_END

GBL_TEST_CASE(peekUint8)
    GblScanner_reset(pFixture->pScanner);
    GblScanner_skipTokens(pFixture->pScanner, 3);

    uint8_t value = 17;
    GBL_TEST_VERIFY(GblScanner_peekUint8(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 12);

    GblScanner_skipTokens(pFixture->pScanner, 1);

    GBL_TEST_VERIFY(GblScanner_peek(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 255);

    GblScanner_skipTokens(pFixture->pScanner, 1);

    GBL_TEST_VERIFY(GblScanner_peek(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 10);
GBL_TEST_CASE_END

GBL_TEST_CASE(peekUint16Invalid)
    GblScanner_setInput(pFixture->pScanner, "\n\ntH\n -12,999999\t12,255 \n 0xffb\t\nG'day");
    GblScanner_setDelimeters(pFixture->pScanner, "[\n \t,]");

    GBL_TEST_EXPECT_ERROR();

    uint16_t value = 0xff;
    GBL_TEST_VERIFY(!GblScanner_peekUint16(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 0);

    GblScanner_skipTokens(pFixture->pScanner, 1);
    value = 0xff;

    GBL_TEST_VERIFY(!GblScanner_peek(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 0);

    GblScanner_skipTokens(pFixture->pScanner, 1);
    value = 0xff;

    GBL_TEST_VERIFY(!GblScanner_peek(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 0);
    GBL_CTX_CLEAR_LAST_RECORD();
GBL_TEST_CASE_END

GBL_TEST_CASE(peekUint16)
    GblScanner_reset(pFixture->pScanner);
    GblScanner_skipTokens(pFixture->pScanner, 3);

    uint16_t value = 17;
    GBL_TEST_VERIFY(GblScanner_peekUint16(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 12);

    GblScanner_skipTokens(pFixture->pScanner, 1);

    GBL_TEST_VERIFY(GblScanner_peek(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 255);

    GblScanner_skipTokens(pFixture->pScanner, 1);

    GBL_TEST_VERIFY(GblScanner_peek(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 0xffb);
GBL_TEST_CASE_END

GBL_TEST_CASE(peekInt16Invalid)
    GblScanner_setInput(pFixture->pScanner, "\n\ntH\n -129999,999999\t12,-257 \n 0xffb\t\nG'day");
    GblScanner_setDelimeters(pFixture->pScanner, "[\n \t,]");

    GBL_TEST_EXPECT_ERROR();

    int16_t value = 0xff;
    GBL_TEST_VERIFY(!GblScanner_peekInt16(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 0);

    GblScanner_skipTokens(pFixture->pScanner, 1);
    value = 0xff;

    GBL_TEST_VERIFY(!GblScanner_peek(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 0);

    GblScanner_skipTokens(pFixture->pScanner, 1);
    value = 0xff;

    GBL_TEST_VERIFY(!GblScanner_peek(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 0);
    GBL_CTX_CLEAR_LAST_RECORD();
GBL_TEST_CASE_END

GBL_TEST_CASE(peekInt16)
    GblScanner_reset(pFixture->pScanner);
    GblScanner_skipTokens(pFixture->pScanner, 3);

    int16_t value = 17;
    GBL_TEST_VERIFY(GblScanner_peekInt16(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 12);

    GblScanner_skipTokens(pFixture->pScanner, 1);

    GBL_TEST_VERIFY(GblScanner_peek(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, -257);

    GblScanner_skipTokens(pFixture->pScanner, 1);

    GBL_TEST_VERIFY(GblScanner_peek(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 0xffb);
GBL_TEST_CASE_END

GBL_TEST_CASE(peekUint32Invalid)
    GblScanner_setInput(pFixture->pScanner, "\n\ntH\n -12,99999999999\t12,65536 \n 0xffffb\t\nG'day");
    GblScanner_setDelimeters(pFixture->pScanner, "[\n \t,]");

    GBL_TEST_EXPECT_ERROR();

    uint32_t value = 0xff;
    GBL_TEST_VERIFY(!GblScanner_peekUint32(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 0);

    GblScanner_skipTokens(pFixture->pScanner, 1);
    value = 0xff;

    GBL_TEST_VERIFY(!GblScanner_peek(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 0);

    GblScanner_skipTokens(pFixture->pScanner, 1);
    value = 0xff;

    GBL_TEST_VERIFY(!GblScanner_peek(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 0);
    GBL_CTX_CLEAR_LAST_RECORD();
GBL_TEST_CASE_END

GBL_TEST_CASE(peekUint32)
    GblScanner_reset(pFixture->pScanner);
    GblScanner_skipTokens(pFixture->pScanner, 3);

    uint32_t value = 17;
    GBL_TEST_VERIFY(GblScanner_peekUint32(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 12);

    GblScanner_skipTokens(pFixture->pScanner, 1);

    GBL_TEST_VERIFY(GblScanner_peek(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 65536);

    GblScanner_skipTokens(pFixture->pScanner, 1);

    GBL_TEST_VERIFY(GblScanner_peek(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 0xffffb);
GBL_TEST_CASE_END

GBL_TEST_CASE(peekInt32Invalid)
    GblScanner_setInput(pFixture->pScanner, "\n\ntH\n -129999999999,99999999999\t12,-32769 \n 0xFFFFB\t\nG'day");
    GblScanner_setDelimeters(pFixture->pScanner, "[\n \t,]");

    GBL_TEST_EXPECT_ERROR();

    int32_t value = 0xff;
    GBL_TEST_VERIFY(!GblScanner_peekInt32(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 0);

    GblScanner_skipTokens(pFixture->pScanner, 1);
    value = 0xff;

    GBL_TEST_VERIFY(!GblScanner_peek(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 0);

    GblScanner_skipTokens(pFixture->pScanner, 1);
    value = 0xff;

    GBL_TEST_VERIFY(!GblScanner_peek(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 0);
    GBL_CTX_CLEAR_LAST_RECORD();
GBL_TEST_CASE_END

GBL_TEST_CASE(peekInt32)
    GblScanner_reset(pFixture->pScanner);
    GblScanner_skipTokens(pFixture->pScanner, 3);

    int32_t value = 17;
    GBL_TEST_VERIFY(GblScanner_peekInt32(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 12);

    GblScanner_skipTokens(pFixture->pScanner, 1);

    GBL_TEST_VERIFY(GblScanner_peek(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, -32769);

    GblScanner_skipTokens(pFixture->pScanner, 1);

    GBL_TEST_VERIFY(GblScanner_peek(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 0xFFFFB);
GBL_TEST_CASE_END

GBL_TEST_CASE(peekUint64Invalid)
    GblScanner_setInput(pFixture->pScanner, "\n\ntH\n -12,999999999999999999999999999999\t12,4294967297  \n 0xFFFFFFFFFFFFFFFF\t\nG'day");
    GblScanner_setDelimeters(pFixture->pScanner, "[\n \t,]");

    GBL_TEST_EXPECT_ERROR();

    uint64_t value = 0xff;
    GBL_TEST_VERIFY(!GblScanner_peekUint64(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 0);

    GblScanner_skipTokens(pFixture->pScanner, 1);
    value = 0xff;

    GBL_TEST_VERIFY(!GblScanner_peek(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 0);

    GblScanner_skipTokens(pFixture->pScanner, 1);
    value = 0xff;

    GBL_TEST_VERIFY(!GblScanner_peek(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 0);
    GBL_CTX_CLEAR_LAST_RECORD();
GBL_TEST_CASE_END

GBL_TEST_CASE(peekUint64)
    GblScanner_reset(pFixture->pScanner);
    GblScanner_skipTokens(pFixture->pScanner, 3);

    uint64_t value = 17;
    GBL_TEST_VERIFY(GblScanner_peekUint64(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 12);

    GblScanner_skipTokens(pFixture->pScanner, 1);

    GBL_TEST_VERIFY(GblScanner_peek(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 4294967297);

    GblScanner_skipTokens(pFixture->pScanner, 1);

    GBL_TEST_VERIFY(GblScanner_peek(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 0xffffffffffffffff);
GBL_TEST_CASE_END

GBL_TEST_CASE(peekInt64Invalid)
    GblScanner_setInput(pFixture->pScanner, "\n\ntH\n -1299999999999999999999,99999999999999999999999\t12,-2147483650 \n 0xFFFFFFFFFFBB\t\nG'day");
    GblScanner_setDelimeters(pFixture->pScanner, "[\n \t,]");

    GBL_TEST_EXPECT_ERROR();

    int64_t value = 0xff;
    GBL_TEST_VERIFY(!GblScanner_peekInt64(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 0);

    GblScanner_skipTokens(pFixture->pScanner, 1);
    value = 0xff;

    GBL_TEST_VERIFY(!GblScanner_peek(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 0);

    GblScanner_skipTokens(pFixture->pScanner, 1);
    value = 0xff;

    GBL_TEST_VERIFY(!GblScanner_peek(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 0);
    GBL_CTX_CLEAR_LAST_RECORD();
GBL_TEST_CASE_END

GBL_TEST_CASE(peekInt64)
    GblScanner_reset(pFixture->pScanner);
    GblScanner_skipTokens(pFixture->pScanner, 3);

    int64_t value = 17;
    GBL_TEST_VERIFY(GblScanner_peekInt64(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 12);

    GblScanner_skipTokens(pFixture->pScanner, 1);

    GBL_TEST_VERIFY(GblScanner_peek(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, -2147483650ll);

    GblScanner_skipTokens(pFixture->pScanner, 1);

    GBL_TEST_VERIFY(GblScanner_peek(pFixture->pScanner, &value));
    GBL_TEST_COMPARE(value, 0xFFFFFFFFFFBB);
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
                  peekMatchInvalid,
                  scanMatchInvalid,
                  peekMatch,
                  scanMatch,
                  peekLinesInvalid,
                  scanLinesInvalid,
                  peekLines,
                  scanLines,
                  peekBytesInvalid,
                  scanBytesInvalid,
                  scanMatchMultiContiguous,
                  peekBoolInvalid,
                  peekBool,
                  peekCharInvalid,
                  peekChar,
                  peekUint8Invalid,
                  peekUint8,
                  peekUint16Invalid,
                  peekUint16,
                  peekInt16Invalid,
                  peekInt16,
                  peekUint32Invalid,
                  peekUint32,
                  peekInt32Invalid,
                  peekInt32,
                  peekUint64Invalid,
                  peekUint64,
                  peekInt64Invalid,
                  peekInt64,
                  unref)

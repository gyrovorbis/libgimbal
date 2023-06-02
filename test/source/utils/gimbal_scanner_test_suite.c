#include "utils/gimbal_scanner_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/utils/gimbal_scanner.h>

#define GBL_TEST_SUITE_SELF GblScannerTestSuite

GBL_TEST_FIXTURE {
    size_t refCount;
    GblScanner* pEmptyScanner;
    GblScanner* pScanner;
};

GBL_TEST_INIT()
    pFixture->refCount = GblRef_activeCount();
GBL_TEST_CASE_END

GBL_TEST_FINAL()
    GBL_TEST_COMPARE(GblRef_activeCount(), pFixture->refCount);
GBL_TEST_CASE_END

GBL_TEST_CASE(createEmpty)
    pFixture->pEmptyScanner = GblScanner_create(NULL, 0);
GBL_TEST_CASE_END

GBL_TEST_CASE(new)
    const char* pInput = "someshit 1283";

    pFixture->pScanner = GBL_NEW(GblScanner,
                                 "delimeters", GBL_SCANNER_DELIMETERS_DEFAULT"%",
                                 "input",      pInput);

    //GBL_TEST_COMPARE(GBL_PRIV_REF(pFixture->pScanner).pDelimeters,
    //                 GBL_SCANNER_DELIMETERS_DEFAULT"%");
GBL_TEST_CASE_END

#if 0

GBL_TEST_CASE(peekTokenInvalid)
    GBL_TEST_VERIFY(!GblScanner_peekToken(pFixture->pEmptyScanner));
    GBL_TEST_VERIFY(pFixture->pEmptyScanner->status & GBL_SCANNER_FLAG_READ_ERROR);
    GBL_TEST_VERIFY(GblStringView_empty(pFixture->pEmptyScanner->nextToken));
GBL_TEST_CASE_END

GBL_TEST_CASE(nextTokenInvalid)
    GBL_TEST_VERIFY(!GblScanner_nextToken(pFixture->pEmptyScanner));
    GBL_TEST_VERIFY(pFixture->pEmptyScanner->status & GBL_SCANNER_FLAG_READ_ERROR);
    GBL_TEST_VERIFY(pFixture->pEmptyScanner->status & GBL_SCANNER_FLAG_EOF);
    GBL_TEST_VERIFY(GblStringView_empty(pFixture->pEmptyScanner->nextToken));
    GBL_TEST_VERIFY(GblStringView_empty(pFixture->pEmptyScanner->token));
GBL_TEST_CASE_END

GBL_TEST_CASE(next)
    GBL_TEST_VERIFY(GblScanner_nextToken(pFixture->pScanner));
    GBL_TEST_VERIFY(GblStringView_compare(pFixture->pScanner->token, GBL_STRV("someshit")) == 0);
    GBL_TEST_VERIFY(pFixture->pScanner->status == 0);
GBL_TEST_CASE_END

GBL_TEST_CASE(peek)
    GBL_TEST_VERIFY(GblScanner_peekToken(pFixture->pScanner));
    GBL_TEST_VERIFY(GblStringView_compare(pFixture->pScanner->nextToken, GBL_STRV("1283")) == 0);
    GBL_TEST_VERIFY(pFixture->pScanner->status == 0);
GBL_TEST_CASE_END

GBL_TEST_CASE(nextAfterPeek)
    GBL_TEST_VERIFY(GblScanner_nextToken(pFixture->pScanner));
    GBL_TEST_VERIFY(GblStringView_compare(pFixture->pScanner->token, GBL_STRV("1283")) == 0);
    GBL_TEST_VERIFY(GblStringView_empty(pFixture->pScanner->nextToken));
    GBL_TEST_VERIFY(pFixture->pScanner->status == 0);
GBL_TEST_CASE_END

GBL_TEST_CASE(peekEof)
    GBL_TEST_VERIFY(!GblScanner_peekToken(pFixture->pScanner));
    GBL_TEST_VERIFY(GblStringView_empty(pFixture->pScanner->nextToken));
    GBL_TEST_VERIFY(pFixture->pScanner->status & (GBL_SCANNER_FLAG_EOF |
                                                  GBL_SCANNER_FLAG_READ_ERROR));
GBL_TEST_CASE_END
#endif
GBL_TEST_CASE(unref)
    GBL_UNREF(pFixture->pEmptyScanner);
    GBL_UNREF(pFixture->pScanner);
GBL_TEST_CASE_END

//                     "2041-04-20T14:01:55-0500");

GBL_TEST_REGISTER(createEmpty,
                  new,
//                  peekTokenInvalid,
//                  nextTokenInvalid,
//                  next,
//                  peek,
//                  nextAfterPeek,
//                  peekEof,
                  unref)

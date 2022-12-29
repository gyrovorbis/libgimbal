#include "utils/gimbal_cmd_parser_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/utils/gimbal_cmd_parser.h>

#define GBL_TEST_SUITE_SELF GblCmdParserTestSuite

GBL_TEST_FIXTURE {
    GblCmdParser* pGroup;
    GblRefCount   refCount;
};

GBL_TEST_INIT() {
    pFixture->refCount = GblRef_activeCount();
    GBL_TEST_CASE_END;
}

GBL_TEST_FINAL() {
    GBL_TEST_COMPARE(pFixture->refCount, GblRef_activeCount());
    GBL_TEST_CASE_END;
}


GBL_TEST_CASE(create) {

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(destroy) {

    GBL_TEST_CASE_END;
}

GBL_TEST_REGISTER(create,
                  destroy);

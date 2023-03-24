#include "core/gimbal_thread_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/core/gimbal_thread.h>

#define GBL_TEST_SUITE_SELF GblThreadTestSuite

GBL_TEST_FIXTURE {
    uint16_t     refCount;
    unsigned     foreachCount;
};

GBL_TEST_INIT() {
    pFixture->refCount = GblRef_activeCount();
    GBL_TEST_CASE_END;
}

GBL_TEST_FINAL() {
    GBL_TEST_COMPARE(GblRef_activeCount(), pFixture->refCount);
    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(create) {

    GBL_TEST_CASE_END;
}

GBL_TEST_REGISTER(create)

#include "strings/gimbal_string_ref_test_suite.hpp"
#include <gimbal/utils/gimbal_ref.h>
#include <gimbal/strings/gimbal_string_ref.hpp>

#define GBL_SELF_TYPE GblStringRefTestSuiteCpp

GBL_TEST_FIXTURE {
    size_t refCount;
};

GBL_TEST_INIT()
    pFixture->refCount = GblRef_activeCount();
GBL_TEST_CASE_END

GBL_TEST_FINAL()
    GBL_TEST_VERIFY(GblRef_activeCount() == pFixture->refCount);
GBL_TEST_CASE_END

GBL_TEST_CASE(constructDefault)
    auto ref = gbl::StringRef{};

GBL_TEST_CASE_END

GBL_TEST_REGISTER(constructDefault)

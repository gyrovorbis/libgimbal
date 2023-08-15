#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/core/gimbal_error.h>
#include "core/gimbal_error_test_suite.h"

#define GBL_SELF_TYPE GblErrorTestSuite

GBL_TEST_FIXTURE {
    size_t classCount;
    size_t instanceCount;
    GblError* pError;
};

GBL_TEST_INIT()
    pFixture->classCount    = GblType_classRefCount(GBL_ERROR_TYPE);
    pFixture->instanceCount = GblType_instanceCount(GBL_ERROR_TYPE);
GBL_TEST_CASE_END

GBL_TEST_FINAL()
    GBL_UNREF(pFixture->pError);
    GBL_TEST_COMPARE(GblType_classRefCount(GBL_ERROR_TYPE),
                     pFixture->classCount);
    GBL_TEST_COMPARE(GblType_instanceCount(GBL_ERROR_TYPE),
                     pFixture->instanceCount);
GBL_TEST_CASE_END

GBL_TEST_CASE(emptyCurrent)
    GBL_TEST_VERIFY(!GblError_current());
GBL_TEST_CASE_END

GBL_TEST_CASE(emptyClear)
    GBL_TEST_VERIFY(!GblError_clear());
GBL_TEST_CASE_END

GBL_TEST_CASE(emptyCatch)
    GBL_TEST_VERIFY(!GblError_catch(GBL_INVALID_TYPE));
    GBL_TEST_VERIFY(!GblError_catch(GBL_ERROR_TYPE));
GBL_TEST_CASE_END

GBL_TEST_CASE(create)
    pFixture->pError = GblError_create(GBL_ERROR_TYPE,
                                       GBL_ENUM_TYPE,
                                       GBL_RESULT_ERROR_INVALID_EXPRESSION,
                                       "Invalid expression: [%s]",
                                       "expressors");
    GBL_TEST_VERIFY(GblError_hasSource(pFixture->pError));
GBL_TEST_CASE_END

GBL_TEST_REGISTER(emptyCurrent,
                  emptyClear,
                  emptyCatch,
                  create);

// emptyCurrent
// emptyClear
// emptyCatch
// create derived
// rethrow
//



#if 0
GBL_TRY {
    GblArrayList_at(pSelf, 99999);
} GBL_CATCH(EvmuError, pError) {

} GBL_CATCH_MORE(GblError, pError) {

} GBL_CATCH_END;

#endif

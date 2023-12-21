#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/core/gimbal_exception.h>
#include "core/gimbal_exception_test_suite.h"

#define GBL_SELF_TYPE           GblExceptionTestSuite
#define BENCHMARK_ITERATIONS_   4096

GBL_TEST_FIXTURE {
    size_t classCount;
    size_t instanceCount;
    GblException* pError;
};

GBL_TEST_INIT()
    pFixture->classCount    = GblType_classRefCount(GBL_EXCEPTION_TYPE);
    pFixture->instanceCount = GblType_instanceCount(GBL_EXCEPTION_TYPE);
GBL_TEST_CASE_END

GBL_TEST_FINAL()
    GBL_UNREF(pFixture->pError);
    GBL_TEST_COMPARE(GblType_classRefCount(GBL_EXCEPTION_TYPE),
                     pFixture->classCount);
    GBL_TEST_COMPARE(GblType_instanceCount(GBL_EXCEPTION_TYPE),
                     pFixture->instanceCount);
GBL_TEST_CASE_END

GBL_TEST_CASE(emptyCurrent)
    GBL_TEST_VERIFY(!GblException_current());
GBL_TEST_CASE_END

GBL_TEST_CASE(emptyClear)
    GBL_TEST_VERIFY(!GblException_clear());
GBL_TEST_CASE_END

GBL_TEST_CASE(emptyCatch)
    GBL_TEST_VERIFY(!GblException_catch(GBL_INVALID_TYPE));
    GBL_TEST_VERIFY(!GblException_catch(GBL_EXCEPTION_TYPE));
GBL_TEST_CASE_END

GBL_TEST_CASE(create)
    pFixture->pError = GblException_create(GBL_EXCEPTION_TYPE,
                                           GBL_ENUM_TYPE,
                                           GBL_RESULT_ERROR_INVALID_EXPRESSION,
                                           "Invalid expression: [%s]",
                                           "expressors");
    GBL_TEST_VERIFY(GblException_hasSource(pFixture->pError));
GBL_TEST_CASE_END

GBL_TEST_CASE(benchmark)
    for(size_t i = 0; i < BENCHMARK_ITERATIONS_; ++i)
        GblException_throw(
            GblException_create(GBL_EXCEPTION_TYPE,
                                GBL_ENUM_TYPE,
                                GBL_RESULT_ERROR_TYPE_MISMATCH,
                                "Raising the %zuth exception!",
                                i));
    GblException_clear();
GBL_TEST_CASE_END

GBL_TEST_REGISTER(emptyCurrent,
                  emptyClear,
                  emptyCatch,
                  create,
                  benchmark);

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

} GBL_CATCH_MORE(GblException, pError) {

} GBL_CATCH_END;

#endif

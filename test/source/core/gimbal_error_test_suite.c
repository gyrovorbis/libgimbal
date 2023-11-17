#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/core/gimbal_error.h>
#include "core/gimbal_error_test_suite.h"
#include <gimbal/core/gimbal_logger.h>

#define BENCHMARK_ITERATIONS_ 200

#define GBL_SELF_TYPE GblErrorTestSuite

GBL_TEST_FIXTURE {
    size_t classCount;
    size_t instanceCount;
    GblError* pError;
};

GBL_TEST_INIT()
    if(GblError_pending()) {
        GBL_CTX_INFO("Clearing pending error: [%s]",
                     GblError_pending()->message);
        GblError_clear();
    }

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
    GBL_TEST_VERIFY(!GblError_pending());
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
    GBL_TEST_COMPARE(pFixture->pError->resultType, GBL_ENUM_TYPE);
    GBL_TEST_COMPARE(pFixture->pError->result, GBL_RESULT_ERROR_INVALID_EXPRESSION);
//    GBL_TEST_COMPARE(pFixture->pError->pMessage, "Invalid expression: [expressors]");
GBL_TEST_CASE_END

GBL_TEST_CASE(throw)
    GBL_TEST_VERIFY(!GblError_throw(pFixture->pError));
    GBL_TEST_COMPARE(GblError_pending(), pFixture->pError);
GBL_TEST_CASE_END

GBL_TEST_CASE(catchInvalid)
    GblError* pError = GblError_catch(GBL_PROPERTY_TYPE);
    GBL_TEST_VERIFY(!pError);
    GBL_TEST_COMPARE(GblError_pending(), pFixture->pError);
GBL_TEST_CASE_END

GBL_TEST_CASE(catch)
    GblError* pError = GblError_catch(GBL_ERROR_TYPE);
    GBL_TEST_COMPARE(pError, pFixture->pError);
    GBL_TEST_COMPARE(GblBox_refCount(GBL_BOX(pError)), 1);
    GBL_TEST_VERIFY(!GblError_pending());
GBL_TEST_CASE_END

GBL_TEST_CASE(unref)
    GBL_TEST_COMPARE(GblError_unref(pFixture->pError), 0);
    pFixture->pError = NULL;
GBL_TEST_CASE_END

GBL_RESULT ctxCall_() {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY(GBL_FALSE,
                   GBL_RESULT_ERROR_INTERNAL,
                   "Test");
    GBL_CTX_END();
}

GBL_TEST_CASE(ctxBenchmark)
    GBL_TEST_EXPECT_ERROR();

    for(size_t i = 0; i < BENCHMARK_ITERATIONS_; ++i)
        ctxCall_();

    GBL_CTX_CLEAR_LAST_RECORD();
GBL_TEST_CASE_END

GBL_TEST_CASE(errorBenchmark)
    for(size_t i = 0; i < BENCHMARK_ITERATIONS_; ++i) {
        GblError* pError = GblError_create(GBL_ERROR_TYPE, GBL_ENUM_TYPE, GBL_RESULT_ERROR_INTERNAL, "Test");
        GblError_throw(pError);
    }
    GblError_clear();
GBL_TEST_CASE_END

GBL_TEST_REGISTER(emptyCurrent,
                  emptyClear,
                  emptyCatch,
                  create,
                  throw,
                  catchInvalid,
                  catch,
                  unref,
                  ctxBenchmark,
                  errorBenchmark);

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

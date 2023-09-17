#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/core/gimbal_error.h>
#include "core/gimbal_error_test_suite.h"

#define TEST_DOMAIN_            (gblTestDomain_())
#define BENCHMARK_ITERATIONS_   4096

#define GBL_SELF_TYPE GblErrorTestSuite

GBL_TEST_FIXTURE {
    const GblError* pPending;
};

typedef enum STATUS_ {
    STATUS_OK,
    STATUS_ERROR,
    STATUS_FAILURE,
    STATUS_DEAD,
    STATUS_MANGLED,
    STATUS_COUNT
} STATUS_;

static const char* statusCodeString_(GblEnum code) {
    switch(code) {
    GBL_SWITCH_CASE_STRINGIFY(STATUS_OK);
    GBL_SWITCH_CASE_STRINGIFY(STATUS_ERROR);
    GBL_SWITCH_CASE_STRINGIFY(STATUS_FAILURE);
    GBL_SWITCH_CASE_STRINGIFY(STATUS_DEAD);
    GBL_SWITCH_CASE_STRINGIFY(STATUS_MANGLED);
    default: return "UNKNOWN";
    }
}

static const GblErrorDomain* gblTestDomain_(void) {
    static GblErrorDomain domain;
    static GblBool once = GBL_FALSE;

    if(!once) {
        domain.name          = GblQuark_fromStatic("STATUS");
        domain.pFnCodeString = statusCodeString_;
        once                 = GBL_TRUE;
    }

    return &domain;
}

GBL_TEST_INIT()
    GblError_clear();
GBL_TEST_CASE_END

GBL_TEST_FINAL()
    GblError_clear();
GBL_TEST_CASE_END

GBL_TEST_CASE(pendingEmpty)
    GBL_TEST_VERIFY(!GblError_pending());
GBL_TEST_CASE_END

GBL_TEST_CASE(domainEmpty)
    GBL_TEST_COMPARE(GblError_domain(), GBL_QUARK_INVALID);
GBL_TEST_CASE_END

GBL_TEST_CASE(stringEmpty)
    GBL_TEST_COMPARE(GblError_string(), "");
GBL_TEST_CASE_END

GBL_TEST_CASE(clearEmpty)
    GBL_TEST_VERIFY(!GblError_clear());
GBL_TEST_CASE_END

GBL_TEST_CASE(raiseCode)
    const GblError* pError =
        GblError_raise(GBL_ERROR_DOMAIN,
                       GBL_RESULT_ERROR_INVALID_EXPRESSION);

    GBL_TEST_VERIFY(pError);
    GBL_TEST_COMPARE(pError->code, GBL_RESULT_ERROR_INVALID_EXPRESSION);
    GBL_TEST_COMPARE(pError->pDomain, GBL_ERROR_DOMAIN);
    GBL_TEST_COMPARE(pError->message, "");
    GBL_TEST_VERIFY(pError->srcLocation.pFile);
    GBL_TEST_COMPARE(pError->srcLocation.pFunc, "GblErrorTestSuite_raiseCode_");
    GBL_TEST_COMPARE(pError->srcLocation.line, 73);

    pFixture->pPending = pError;
GBL_TEST_CASE_END

GBL_TEST_CASE(pending)
    GBL_TEST_COMPARE(GblError_pending(), pFixture->pPending);
GBL_TEST_CASE_END

GBL_TEST_CASE(domain)
    GBL_TEST_COMPARE(GblError_domain(), GblQuark_fromStatic("GblError"));
GBL_TEST_CASE_END

GBL_TEST_CASE(string)
    GBL_TEST_COMPARE(GblError_string(), "Invalid Expression");
GBL_TEST_CASE_END

GBL_TEST_CASE(reraise)
    const GblError* pError = GblError_reraise();

    GBL_TEST_VERIFY(pError);
    GBL_TEST_VERIFY(pError);
    GBL_TEST_COMPARE(pError->code, GBL_RESULT_ERROR_INVALID_EXPRESSION);
    GBL_TEST_COMPARE(pError->pDomain, GBL_ERROR_DOMAIN);
    GBL_TEST_COMPARE(pError->message, "");
    GBL_TEST_VERIFY(pError->srcLocation.pFile);
    GBL_TEST_COMPARE(pError->srcLocation.pFunc, "GblErrorTestSuite_reraise_");
    GBL_TEST_COMPARE(pError->srcLocation.line, 100);
GBL_TEST_CASE_END

GBL_TEST_CASE(clear)
    GBL_TEST_VERIFY(GblError_clear());
    GBL_TEST_VERIFY(!GblError_pending());
GBL_TEST_CASE_END

GBL_TEST_CASE(raiseCustomMessage)
    const GblError* pError =
        GblError_raise(TEST_DOMAIN_,
                       STATUS_DEAD,
                       "She's dead, bro!");

    GBL_TEST_VERIFY(pError);
    GBL_TEST_COMPARE(GblError_pending(), pError);
    GBL_TEST_COMPARE(GblError_domain(), GblQuark_fromStatic("STATUS"));
    GBL_TEST_COMPARE(GblError_string(), "STATUS_DEAD");
    GBL_TEST_COMPARE(pError->code, STATUS_DEAD);
    GBL_TEST_COMPARE(pError->message, "She's dead, bro!");
GBL_TEST_CASE_END

GBL_TEST_CASE(raiseCustomMessageVa)
    const GblError* pError =
        GblError_raise(TEST_DOMAIN_,
                       STATUS_MANGLED,
                       "Sega %s is %u times better.",
                       "Dreamcast",
                       9999);

    GBL_TEST_VERIFY(pError);
    GBL_TEST_COMPARE(GblError_pending(), pError);
    GBL_TEST_COMPARE(GblError_domain(), GblQuark_fromStatic("STATUS"));
    GBL_TEST_COMPARE(GblError_string(), "STATUS_MANGLED");
    GBL_TEST_COMPARE(pError->code, STATUS_MANGLED);
    GBL_TEST_COMPARE(pError->message, "Sega Dreamcast is 9999 times better.");
GBL_TEST_CASE_END

GBL_TEST_CASE(benchmark)
    for(size_t i = 0; i < BENCHMARK_ITERATIONS_; ++i) {
        GblError_raise(TEST_DOMAIN_,
                       (STATUS_)i % STATUS_COUNT,
                       "Raising the %zuth error!",
                       i);
    }
GBL_TEST_CASE_END

GBL_TEST_REGISTER(pendingEmpty,
                  domainEmpty,
                  stringEmpty,
                  clearEmpty,
                  raiseCode,
                  pending,
                  domain,
                  string,
                  reraise,
                  clear,
                  raiseCustomMessage,
                  raiseCustomMessageVa,
                  benchmark)


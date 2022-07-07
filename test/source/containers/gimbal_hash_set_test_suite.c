#include "containers/gimbal_hash_set_test_suite.h"
#include <gimbal/test/gimbal_test.h>
#include <gimbal/containers/gimbal_hash_set.h>

#define GBL_HASH_SET_TEST_SUITE_(inst)     ((GblHashSetTestSuite_*)GBL_INSTANCE_PRIVATE(inst, GBL_HASH_SET_TEST_SUITE_TYPE))

typedef struct GblHashSetTestSuite_ {
    char dummy[1];
} GblHashSetTestSuite_;



GBL_RESULT GblHashSetTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblHashSetTestSuite_* pSelf_ = GBL_HASH_SET_TEST_SUITE_(pSelf);
    memset(pSelf_, 0, sizeof(GblHashSetTestSuite_));
    GBL_API_END();
}

GBL_RESULT GblHashSetTestSuite_final_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}


GBL_EXPORT GblType GblHashSetTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { NULL,                     NULL  }
    };

    const static GblTestSuiteClassVTable vTable = {
        .pFnSuiteInit   = GblHashSetTestSuite_init_,
        .pFnSuiteFinal  = GblHashSetTestSuite_final_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_API_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("HashSetTestSuite"),
                                     &vTable,
                                     sizeof(GblHashSetTestSuite),
                                     sizeof(GblHashSetTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_API_VERIFY_LAST_RECORD();
        GBL_API_END_BLOCK();
    }
    return type;
}

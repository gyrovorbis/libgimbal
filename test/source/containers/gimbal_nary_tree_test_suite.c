#include "containers/gimbal_nary_tree_test_suite.h"
#include <gimbal/test/gimbal_test.h>
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/containers/gimbal_nary_tree.h>

#define GBL_NARY_TREE_TEST_SUITE_(inst)     ((GblNaryTreeTestSuite_*)GBL_INSTANCE_PRIVATE(inst, GBL_NARY_TREE_TEST_SUITE_TYPE))

typedef struct GblNaryTreeTestSuite_ {
    char dummy[1];
} GblNaryTreeTestSuite_;


static GBL_RESULT GblNaryTreeTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblNaryTreeTestSuite_* pSelf_ = GBL_NARY_TREE_TEST_SUITE_(pSelf);
    memset(pSelf_, 0, sizeof(GblNaryTreeTestSuite_));
    GBL_API_END();
}

static GBL_RESULT GblNaryTreeTestSuite_final_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblNaryTreeTestSuite_* pSelf_ = GBL_NARY_TREE_TEST_SUITE_(pSelf);
    GBL_API_END();
}

GBL_EXPORT GblType GblNaryTreeTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { NULL,             NULL     }
    };

    const static GblTestSuiteClassVTable vTable = {
        .pFnSuiteInit   = GblNaryTreeTestSuite_init_,
        .pFnSuiteFinal  = GblNaryTreeTestSuite_final_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_API_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("GblNaryTreeTestSuite"),
                                     &vTable,
                                     sizeof(GblNaryTreeTestSuite),
                                     sizeof(GblNaryTreeTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_API_VERIFY_LAST_RECORD();
        GBL_API_END_BLOCK();
    }
    return type;
}

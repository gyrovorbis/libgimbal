#include "meta/gimbal_interface_test_suite.h"
#include <gimbal/test/gimbal_test.h>
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/meta/gimbal_interface.h>

#define GBL_INTERFACE_TEST_SUITE_(inst)     (GBL_INSTANCE_PRIVATE(inst, GBL_INTERFACE_TEST_SUITE_TYPE))

typedef struct GblInterfaceTestSuite_ {
    GblSize initialRefCount;
} GblInterfaceTestSuite_;


static GBL_RESULT GblInterfaceTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblInterfaceTestSuite_* pSelf_ = GBL_INTERFACE_TEST_SUITE_(pSelf);
    memset(pSelf_, 0, sizeof(GblInterfaceTestSuite_));
    pSelf_->initialRefCount = GblType_classRefCount(GBL_INTERFACE_TYPE);
    GBL_API_END();
}

static GBL_RESULT GblInterfaceTestSuite_final_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblInterfaceTestSuite_* pSelf_ = GBL_INTERFACE_TEST_SUITE_(pSelf);
    GBL_TEST_COMPARE(GblType_classRefCount(GBL_INTERFACE_TYPE), pSelf_->initialRefCount);
    GBL_API_END();
}


GBL_EXPORT GblType GblInterfaceTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { NULL,     NULL    }
    };

    const static GblTestSuiteClassVTable vTable = {
        .pFnSuiteInit   = GblInterfaceTestSuite_init_,
        .pFnSuiteFinal  = GblInterfaceTestSuite_final_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_API_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("InterfaceTestSuite"),
                                     &vTable,
                                     sizeof(GblInterfaceTestSuite),
                                     sizeof(GblInterfaceTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_API_VERIFY_LAST_RECORD();
        GBL_API_END_BLOCK();
    }
    return type;
}

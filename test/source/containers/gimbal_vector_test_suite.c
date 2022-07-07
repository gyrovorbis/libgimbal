#include "containers/gimbal_vector_test_suite.h"
#include <gimbal/test/gimbal_test.h>
#include <gimbal/containers/gimbal_vector.h>

#define GBL_VECTOR_TEST_SUITE_(inst)     ((GblVectorTestSuite_*)GBL_INSTANCE_PRIVATE(inst, GBL_VECTOR_TEST_SUITE_TYPE))

typedef struct GblVectorTestSuite_ {
    struct {
        GblVector   vector;
        const char* pExtraData[4];
    } stringList;
} GblVectorTestSuite_;

const static char* initialStrings_[4] = {
    "entry1",
    "entry2",
    "entry3",
    "entry4"
};

GBL_RESULT GblVectorTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblVectorTestSuite_* pSelf_ = GBL_VECTOR_TEST_SUITE_(pSelf);
    memset(pSelf_, 0, sizeof(GblVectorTestSuite_));
    GBL_API_END();
}

GBL_RESULT GblVectorTestSuite_final_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}

GBL_RESULT GblVectorTestSuite_constructWithEntries_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblVectorTestSuite_* pSelf_ = GBL_VECTOR_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblVector_construct(&pSelf_->stringList.vector,
                                            sizeof(const char*),
                                            4,
                                            initialStrings_,
                                            sizeof(pSelf_->stringList),
                                            pCtx));
    GBL_TEST_COMPARE(GblVector_context(&pSelf_->stringList.vector), pCtx);
    GBL_TEST_COMPARE(GblVector_size(&pSelf_->stringList.vector), 4);
    GBL_TEST_VERIFY(!GblVector_empty(&pSelf_->stringList.vector));
    //GBL_TEST_VERIFY(GblVector_stack(&pSelf_->stringList.vector));
    GBL_API_END();
}


GBL_EXPORT GblType GblVectorTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "constructWithEntries",   GblVectorTestSuite_constructWithEntries_ },
        { NULL,                     NULL  }
    };

    const static GblTestSuiteClassVTable vTable = {
        .pFnSuiteInit   = GblVectorTestSuite_init_,
        .pFnSuiteFinal  = GblVectorTestSuite_final_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_API_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("VectorTestSuite"),
                                     &vTable,
                                     sizeof(GblVectorTestSuite),
                                     sizeof(GblVectorTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_API_VERIFY_LAST_RECORD();
        GBL_API_END_BLOCK();
    }
    return type;
}

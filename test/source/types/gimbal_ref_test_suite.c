#include "types/gimbal_ref_test_suite.h"
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/types/gimbal_ref.h>
#include <gimbal/test/gimbal_test.h>

#define GBL_REF_TEST_SUITE_(inst)   ((GblRefTestSuite_*)GBL_INSTANCE_PRIVATE(inst, GBL_REF_TEST_SUITE_TYPE))

typedef struct GblRefTestSuite_ {
    GblRef* pRefs[4];
    GblSize startRefCount;
} GblRefTestSuite_;

static unsigned dtorBadCalls_ = 0;
static unsigned dtorGoodCalls_ = 0;

static GBL_RESULT refDtorBad_(GblRef* pRef) {
    GBL_UNUSED(pRef);
    ++dtorBadCalls_;
    return GBL_RESULT_ERROR;
}

static GBL_RESULT refDtorGood_(GblRef* pRef) {
    GBL_UNUSED(pRef);
    ++dtorGoodCalls_;
    return GBL_RESULT_SUCCESS;
}
static GBL_RESULT GblRefTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblRefTestSuite_* pSelf_ = GBL_REF_TEST_SUITE_(pSelf);
    memset(pSelf_->pRefs, 0, sizeof(pSelf_->pRefs));
    dtorBadCalls_ = dtorGoodCalls_ = 0;
    pSelf_->startRefCount = GblRef_activeCount();
    GBL_API_CLEAR_LAST_RECORD();
    GBL_API_END();
}
static GBL_RESULT GblRefTestSuite_allocEmpty_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_API_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();
    void* pRef = GblRef_alloc(0);
    GBL_TEST_COMPARE(pRef, NULL);
    GBL_TEST_COMPARE(GBL_API_LAST_RESULT(), GBL_RESULT_ERROR_INVALID_ARG);
    GBL_API_CLEAR_LAST_RECORD();
    GBL_API_END();
}
static GBL_RESULT GblRefTestSuite_alloc_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblRefTestSuite_* pSelf_ = GBL_REF_TEST_SUITE_(pSelf);
    pSelf_->pRefs[0] = GblRef_alloc(256);
    strncpy(pSelf_->pRefs[0], "String1", 256);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_TEST_VERIFY(pSelf_->pRefs[0]);
    GBL_TEST_COMPARE(GblRef_activeCount(), pSelf_->startRefCount + 1);
    GBL_TEST_COMPARE(GblRef_refCount(pSelf_->pRefs[0]), 1);
    GBL_TEST_COMPARE(GblRef_context(pSelf_->pRefs[0]), NULL);
    GBL_API_END();
}
static GBL_RESULT GblRefTestSuite_allocWithContext_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblRefTestSuite_* pSelf_ = GBL_REF_TEST_SUITE_(pSelf);
    pSelf_->pRefs[1] = GblRef_allocWithContext(256, pCtx);
    strncpy(pSelf_->pRefs[1], "String2", 256);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_TEST_VERIFY(pSelf_->pRefs[1]);
    GBL_TEST_COMPARE(GblRef_activeCount(), pSelf_->startRefCount + 2);
    GBL_TEST_COMPARE(GblRef_refCount(pSelf_->pRefs[1]), 1);
    GBL_TEST_COMPARE(GblRef_context(pSelf_->pRefs[1]), pCtx);
    GBL_API_END();
}
static GBL_RESULT GblRefTestSuite_acquire_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblRefTestSuite_* pSelf_ = GBL_REF_TEST_SUITE_(pSelf);
    pSelf_->pRefs[2] = GblRef_acquire(pSelf_->pRefs[0]);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(pSelf_->pRefs[2], pSelf_->pRefs[0]);
    GBL_TEST_COMPARE(strncmp(pSelf_->pRefs[2], "String1", 256), 0);
    GBL_TEST_COMPARE(GblRef_refCount(pSelf_->pRefs[0]), 2);
    GBL_TEST_COMPARE(GblRef_activeCount(), pSelf_->startRefCount + 2);

    pSelf_->pRefs[3] = GblRef_acquire(pSelf_->pRefs[1]);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(pSelf_->pRefs[3], pSelf_->pRefs[1]);
    GBL_TEST_COMPARE(strncmp(pSelf_->pRefs[3], "String2", 256), 0);
    GBL_TEST_COMPARE(GblRef_refCount(pSelf_->pRefs[1]), 2);
    GBL_TEST_COMPARE(GblRef_activeCount(), pSelf_->startRefCount + 2);

    void* pEmpty = GblRef_acquire(NULL);
    GBL_TEST_COMPARE(pEmpty, NULL);

    GBL_API_END();
}
static GBL_RESULT GblRefTestSuite_release_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblRefTestSuite_* pSelf_ = GBL_REF_TEST_SUITE_(pSelf);
    GblRefCount refCount = GblRef_release(NULL);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(refCount, 0);

    refCount = GblRef_release(pSelf_->pRefs[2]);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(refCount, 1);
    GBL_TEST_COMPARE(GblRef_activeCount(), pSelf_->startRefCount + 2);
    GBL_TEST_COMPARE(strncmp(pSelf_->pRefs[2], "String1", 256), 0);

    refCount = GblRef_release(pSelf_->pRefs[3]);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(refCount, 1);
    GBL_TEST_COMPARE(GblRef_activeCount(), pSelf_->startRefCount + 2);
    GBL_TEST_COMPARE(strncmp(pSelf_->pRefs[3], "String2", 256), 0);

    GBL_API_END();
}
static GBL_RESULT GblRefTestSuite_destructFail_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblRefTestSuite_* pSelf_ = GBL_REF_TEST_SUITE_(pSelf);
    GBL_TEST_EXPECT_ERROR();
    GblRefCount refCount = GblRef_releaseWithDtor(pSelf_->pRefs[0], refDtorBad_);
    GBL_TEST_COMPARE(GBL_API_LAST_RESULT(), GBL_RESULT_ERROR);
    GBL_API_CLEAR_LAST_RECORD();
    GBL_TEST_COMPARE(refCount, 1);
    GBL_TEST_COMPARE(GblRef_activeCount(), pSelf_->startRefCount + 2);
    GBL_TEST_COMPARE(dtorBadCalls_, 1);
    GBL_TEST_COMPARE(strncmp(pSelf_->pRefs[0], "String1", 256), 0);
    GBL_API_END();
}

static GBL_RESULT GblRefTestSuite_destruct_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblRefTestSuite_* pSelf_ = GBL_REF_TEST_SUITE_(pSelf);
    GblRefCount refCount = GblRef_releaseWithDtor(pSelf_->pRefs[0], refDtorGood_);
    GBL_TEST_VERIFY(!GBL_RESULT_ERROR(GBL_API_LAST_RESULT()));
    GBL_API_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(refCount, 0);
    GBL_TEST_COMPARE(GblRef_activeCount(), pSelf_->startRefCount + 1);
    GBL_TEST_COMPARE(dtorGoodCalls_, 1);

    refCount = GblRef_releaseWithDtor(pSelf_->pRefs[1], refDtorGood_);
    GBL_TEST_VERIFY(!GBL_RESULT_ERROR(GBL_API_LAST_RESULT()));
    GBL_API_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(refCount, 0);
    GBL_TEST_COMPARE(GblRef_activeCount(), pSelf_->startRefCount +  0);
    GBL_TEST_COMPARE(dtorGoodCalls_, 2);

    GBL_API_END();
}

GBL_EXPORT GblType GblRefTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "allocEmpty",         GblRefTestSuite_allocEmpty_         },
        { "alloc",              GblRefTestSuite_alloc_              },
        { "allocWithContext",   GblRefTestSuite_allocWithContext_   },
        { "acquire",            GblRefTestSuite_acquire_            },
        { "release",            GblRefTestSuite_release_            },
        { "destructFail",       GblRefTestSuite_destructFail_       },
        { "destruct",           GblRefTestSuite_destruct_           },
        { NULL,                 NULL                                }
    };

    const static GblTestSuiteClassVTable vTable = {
        .pFnSuiteInit   = GblRefTestSuite_init_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_API_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("RefTestSuite"),
                                     &vTable,
                                     sizeof(GblRefTestSuite),
                                     sizeof(GblRefTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_API_VERIFY_LAST_RECORD();
        GBL_API_END_BLOCK();
    }
    return type;
}

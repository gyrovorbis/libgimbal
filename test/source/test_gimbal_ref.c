#include "test_gimbal_ref.h"
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/types/gimbal_ref.h>

static GblRef* pRefs_[4] = { NULL };
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

GBL_API GblRef_test_testCaseInit(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}
GBL_API GblRef_test_testCaseFinal(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}
GBL_API GblRef_test_init(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    memset(pRefs_, 0, sizeof(pRefs_));
    dtorBadCalls_ = dtorGoodCalls_ = 0;
    GblRef_reinit();
    GBL_API_CLEAR_LAST_RECORD();
    GBL_COMPARE(GblRef_activeCount(), 0);
    GBL_API_END();
}
GBL_API GblRef_test_allocEmpty(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();
    void* pRef = GblRef_alloc(0);
    GBL_COMPARE(pRef, NULL);
    GBL_COMPARE(GBL_API_LAST_RESULT(), GBL_RESULT_ERROR_INVALID_ARG);
    GBL_API_CLEAR_LAST_RECORD();
    GBL_API_END();
}
GBL_API GblRef_test_alloc(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    pRefs_[0] = GblRef_alloc(256);
    strncpy(pRefs_[0], "String1", 256);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_VERIFY(pRefs_[0]);
    GBL_COMPARE(GblRef_activeCount(), 1);
    GBL_COMPARE(GblRef_refCount(pRefs_[0]), 1);
    GBL_COMPARE(GblRef_context(pRefs_[0]), NULL);
    GBL_API_END();
}
GBL_API GblRef_test_allocWithContext(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    pRefs_[1] = GblRef_allocWithContext(256, pCtx);
    strncpy(pRefs_[1], "String2", 256);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_VERIFY(pRefs_[1]);
    GBL_COMPARE(GblRef_activeCount(), 2);
    GBL_COMPARE(GblRef_refCount(pRefs_[1]), 1);
    GBL_COMPARE(GblRef_context(pRefs_[1]), pCtx);
    GBL_API_END();
}
GBL_API GblRef_test_acquire(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    pRefs_[2] = GblRef_acquire(pRefs_[0]);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_COMPARE(pRefs_[2], pRefs_[0]);
    GBL_COMPARE(strncmp(pRefs_[2], "String1", 256), 0);
    GBL_COMPARE(GblRef_refCount(pRefs_[0]), 2);
    GBL_COMPARE(GblRef_activeCount(), 2);

    pRefs_[3] = GblRef_acquire(pRefs_[1]);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_COMPARE(pRefs_[3], pRefs_[1]);
    GBL_COMPARE(strncmp(pRefs_[3], "String2", 256), 0);
    GBL_COMPARE(GblRef_refCount(pRefs_[1]), 2);
    GBL_COMPARE(GblRef_activeCount(), 2);

    void* pEmpty = GblRef_acquire(NULL);
    GBL_COMPARE(pEmpty, NULL);

    GBL_API_END();
}
GBL_API GblRef_test_release(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GblRefCount refCount = GblRef_release(NULL);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_COMPARE(refCount, 0);

    refCount = GblRef_release(pRefs_[2]);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_COMPARE(refCount, 1);
    GBL_COMPARE(GblRef_activeCount(), 2);
    GBL_COMPARE(strncmp(pRefs_[2], "String1", 256), 0);

    refCount = GblRef_release(pRefs_[3]);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_COMPARE(refCount, 1);
    GBL_COMPARE(GblRef_activeCount(), 2);
    GBL_COMPARE(strncmp(pRefs_[3], "String2", 256), 0);

    GBL_API_END();
}
GBL_API GblRef_test_destructFail(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();
    GblRefCount refCount = GblRef_releaseWithDtor(pRefs_[0], refDtorBad_);
    GBL_COMPARE(GBL_API_LAST_RESULT(), GBL_RESULT_ERROR);
    GBL_API_CLEAR_LAST_RECORD();
    GBL_COMPARE(refCount, 1);
    GBL_COMPARE(GblRef_activeCount(), 2);
    GBL_COMPARE(dtorBadCalls_, 1);
    GBL_COMPARE(strncmp(pRefs_[0], "String1", 256), 0);
    GBL_API_END();
}

GBL_API GblRef_test_destruct(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GblRefCount refCount = GblRef_releaseWithDtor(pRefs_[0], refDtorGood_);
    GBL_VERIFY(!GBL_RESULT_ERROR(GBL_API_LAST_RESULT()));
    GBL_API_VERIFY_LAST_RECORD();
    GBL_COMPARE(refCount, 0);
    GBL_COMPARE(GblRef_activeCount(), 1);
    GBL_COMPARE(dtorGoodCalls_, 1);

    refCount = GblRef_releaseWithDtor(pRefs_[1], refDtorGood_);
    GBL_VERIFY(!GBL_RESULT_ERROR(GBL_API_LAST_RESULT()));
    GBL_API_VERIFY_LAST_RECORD();
    GBL_COMPARE(refCount, 0);
    GBL_COMPARE(GblRef_activeCount(), 0);
    GBL_COMPARE(dtorGoodCalls_, 2);

    GBL_API_END();
}
GBL_API GblRef_test_final(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_VERIFY_CALL(GblRef_reinit());
    GBL_API_END();
}

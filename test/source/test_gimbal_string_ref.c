#include "test_gimbal_string_ref.h"
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/types/gimbal_string_ref.h>

GblStringRef*   pRefs_[8]               = { NULL };
GblRefCount     beginActiveRefCount_    = 0;

static GBL_RESULT verifyEmpty_(GblContext* pCtx, GblStringRef* pRef, GblSize refCount, GblContext* pRefCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_VERIFY(pRef);
    GBL_COMPARE(pRef[0], '\0');
    GBL_COMPARE(strlen(pRef), 0);
    GBL_COMPARE(GblStringRef_refCount(pRef), refCount);
    GBL_COMPARE(GblStringRef_length(pRef), 0);
    GBL_COMPARE(GblStringRef_context(pRef), pRefCtx);
    GBL_VERIFY(GblStringRef_valid(pRef));
    GBL_VERIFY(GblStringRef_empty(pRef));
    const GblStringView view = GblStringRef_view(pRef);
    GBL_COMPARE(view.length, 0);
    GBL_COMPARE(view.pData, pRef);
    GBL_API_END();
}

static GBL_RESULT verifyString_(GblContext* pCtx, GblStringRef* pRef, const char* pValue, GblSize refCount, GblContext* pRefCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_VERIFY(pRef);
    GBL_COMPARE(pRef[0], pValue[0]);
    GBL_COMPARE(strcmp(pRef, pValue), 0);
    GBL_COMPARE(strlen(pRef), GblStringRef_length(pRef));
    GBL_COMPARE(strlen(pRef), strlen(pValue));
    GBL_COMPARE(GblStringRef_refCount(pRef), refCount);
    GBL_COMPARE(GblStringRef_context(pRef), pRefCtx);
    GBL_VERIFY(GblStringRef_valid(pRef));
    GBL_VERIFY(!GblStringRef_empty(pRef));
    const GblStringView view = GblStringRef_view(pRef);
    GBL_COMPARE(view.length, strlen(pValue));
    GBL_COMPARE(view.pData, pRef);
    GBL_COMPARE(GblStringRef_at(pRef, GblStringRef_length(pRef)-1), pValue[strlen(pValue)-1]);
    GBL_API_END();
}

GBL_API GblStringRef_test_testCaseInit(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}
GBL_API GblStringRef_test_testCaseFinal(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}

GBL_API GblStringRef_test_init(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    memset(pRefs_, 0, sizeof(pRefs_));
    beginActiveRefCount_ = GblRef_activeCount();
    GBL_API_END();
}

GBL_API GblStringRef_test_null(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_COMPARE(GblStringRef_acquire(NULL), NULL);
    GBL_COMPARE(GblStringRef_release(NULL), 0);
    GBL_COMPARE(GblStringRef_at(NULL, 0), '\0');
    GBL_COMPARE(GblStringRef_context(NULL), NULL);
    GBL_COMPARE(GblStringRef_refCount(NULL), 0);
    GBL_COMPARE(GblStringRef_length(NULL), 0);
    GBL_VERIFY(!GblStringRef_valid(NULL));
    const GblStringView view = GblStringRef_view(NULL);
    GBL_COMPARE(view.length, 0);
    GBL_COMPARE(view.pData, NULL);
    GBL_TEST_EXPECT_ERROR();
    GBL_COMPARE(GblStringRef_at(pRefs_[0], 0), '\0');
    GBL_COMPARE(GBL_API_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_API_END();
}

GBL_API GblStringRef_test_null_at(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();
    GBL_COMPARE(GblStringRef_at(pRefs_[0], 0), '\0');
    GBL_COMPARE(GBL_API_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_API_END();
}

GBL_API GblStringRef_test_createEmpty(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    pRefs_[0] = GblStringRef_create(NULL);
    GBL_API_VERIFY_CALL(verifyEmpty_(pCtx, pRefs_[0], 1, NULL));
    GBL_API_END();
}

GBL_API GblStringRef_test_empty_at(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();

    GBL_COMPARE(GblStringRef_at(pRefs_[0], 0), '\0');
    GBL_COMPARE(GBL_API_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);

    GBL_COMPARE(GblStringRef_at(pRefs_[0], 1), '\0');
    GBL_COMPARE(GBL_API_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_API_END();
}

GBL_API GblStringRef_test_create(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    pRefs_[1] = GblStringRef_create("lolz");
    GBL_API_VERIFY_CALL(verifyString_(pCtx, pRefs_[1], "lolz", 1, NULL));
    GBL_API_END();
}
GBL_API GblStringRef_test_createWithContext(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    pRefs_[2] = GblStringRef_createWithContext("contextual", pCtx);
    GBL_API_VERIFY_CALL(verifyString_(pCtx, pRefs_[2], "contextual", 1, pCtx));
    GBL_API_END();
}
GBL_API GblStringRef_test_createFromViewEmpty(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    pRefs_[3] = GblStringRef_createFromView(GBL_STRING_VIEW(NULL));
    GBL_API_VERIFY_CALL(verifyEmpty_(pCtx, pRefs_[3], 1, NULL));
    GBL_API_END();
}
GBL_API GblStringRef_test_createFromView(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    pRefs_[4] = GblStringRef_createFromView(GBL_STRING_VIEW("viewz"));
    GBL_API_VERIFY_CALL(verifyString_(pCtx, pRefs_[4], "viewz", 1, NULL));
    GBL_API_END();
}
GBL_API GblStringRef_test_createFromViewWithContext(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    pRefs_[5] = GblStringRef_createFromViewWithContext(GBL_STRING_VIEW("contextual_viewz"), pCtx);
    GBL_API_VERIFY_CALL(verifyString_(pCtx, pRefs_[5], "contextual_viewz", 1, pCtx));
    GBL_API_END();
}
GBL_API GblStringRef_test_acquire(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    pRefs_[6] = GblStringRef_acquire(pRefs_[5]);
    GBL_API_VERIFY_CALL(verifyString_(pCtx, pRefs_[6], "contextual_viewz", 2, pCtx));
    GBL_COMPARE(pRefs_[6], pRefs_[5]);
    GBL_API_END();
}
GBL_API GblStringRef_test_release(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblRefCount refCount = GblRef_activeCount();
    GblStringRef_release(pRefs_[6]);
    GBL_API_VERIFY_CALL(verifyString_(pCtx, pRefs_[5], "contextual_viewz", 1, pCtx));
    GBL_COMPARE(GblRef_activeCount(), refCount);

    GblStringRef_release(pRefs_[5]);
    GBL_COMPARE(GblRef_activeCount(), --refCount);

    GblStringRef_release(pRefs_[4]);
    GBL_COMPARE(GblRef_activeCount(), --refCount);

    GblStringRef_release(pRefs_[3]);
    GBL_COMPARE(GblRef_activeCount(), --refCount);

    GblStringRef_release(pRefs_[2]);
    GBL_COMPARE(GblRef_activeCount(), --refCount);

    GblStringRef_release(pRefs_[1]);
    GBL_COMPARE(GblRef_activeCount(), --refCount);

    GblStringRef_release(pRefs_[0]);
    GBL_COMPARE(GblRef_activeCount(), --refCount);
    GBL_API_END();
}

GBL_API GblStringRef_test_final(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_COMPARE(GblRef_activeCount(), beginActiveRefCount_);
    GBL_API_END();
}

#include "strings/gimbal_string_ref_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/core/gimbal_ctx.h>
#include <gimbal/strings/gimbal_string_ref.h>

#define GBL_STRING_REF_TEST_SUITE_(inst)    (GBL_PRIVATE(GblStringRefTestSuite, inst))

typedef struct GblStringRefTestSuite_ {
    GblStringRef*   pRefs[8];
    GblRefCount     beginActiveRefCount;
} GblStringRefTestSuite_;

static GBL_RESULT verifyEmpty_(GblContext* pCtx, GblStringRef* pRef, size_t  refCount, GblContext* pRefCtx) {
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_VERIFY(pRef);
    GBL_TEST_COMPARE(pRef[0], '\0');
    GBL_TEST_COMPARE(strlen(pRef), 0);
    GBL_TEST_COMPARE(GblStringRef_refCount(pRef), refCount);
    GBL_TEST_COMPARE(GblStringRef_length(pRef), 0);
    GBL_TEST_COMPARE(GblStringRef_context(pRef), pRefCtx);
    GBL_TEST_VERIFY(GblStringRef_valid(pRef));
    GBL_TEST_VERIFY(GblStringRef_empty(pRef));
    const GblStringView view = GblStringRef_view(pRef);
    GBL_TEST_COMPARE_UINT(view.length, 0);
    GBL_TEST_COMPARE(view.pData, pRef);
    GBL_CTX_END();
}

static GBL_RESULT verifyString_(GblContext* pCtx, GblStringRef* pRef, const char* pValue, size_t  refCount, GblContext* pRefCtx) {
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_VERIFY(pRef);
    GBL_TEST_COMPARE(pRef[0], pValue[0]);
    GBL_TEST_COMPARE(strcmp(pRef, pValue), 0);
    GBL_TEST_COMPARE(strlen(pRef), GblStringRef_length(pRef));
    GBL_TEST_COMPARE(strlen(pRef), strlen(pValue));
    GBL_TEST_COMPARE(GblStringRef_refCount(pRef), refCount);
    GBL_TEST_COMPARE(GblStringRef_context(pRef), pRefCtx);
    GBL_TEST_VERIFY(GblStringRef_valid(pRef));
    GBL_TEST_VERIFY(!GblStringRef_empty(pRef));
    const GblStringView view = GblStringRef_view(pRef);
    GBL_TEST_COMPARE_UINT(view.length, strlen(pValue));
    GBL_TEST_COMPARE(view.pData, pRef);
    GBL_TEST_COMPARE(GblStringRef_at(pRef, GblStringRef_length(pRef)-1), pValue[strlen(pValue)-1]);
    GBL_CTX_END();
}

static GBL_RESULT GblStringRefTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringRefTestSuite_* pSelf_ = GBL_STRING_REF_TEST_SUITE_(pSelf);
    memset(pSelf_->pRefs, 0, sizeof(pSelf_->pRefs));
    pSelf_->beginActiveRefCount = GblRef_activeCount();
    GBL_CTX_END();
}

static GBL_RESULT GblStringRefTestSuite_final_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringRefTestSuite_* pSelf_ = GBL_STRING_REF_TEST_SUITE_(pSelf);
    GBL_TEST_COMPARE(GblRef_activeCount(), pSelf_->beginActiveRefCount);
    GBL_CTX_END();
}

static GBL_RESULT GblStringRefTestSuite_null_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringRefTestSuite_* pSelf_ = GBL_STRING_REF_TEST_SUITE_(pSelf);
    GBL_TEST_COMPARE(GblStringRef_acquire(NULL), NULL);
    GBL_TEST_COMPARE(GblStringRef_release(NULL), 0);
    //GBL_TEST_COMPARE(GblStringRef_at(NULL, 0), '\0');
    GBL_TEST_COMPARE(GblStringRef_context(NULL), NULL);
    GBL_TEST_COMPARE(GblStringRef_refCount(NULL), 0);
    GBL_TEST_COMPARE(GblStringRef_length(NULL), 0);
    GBL_TEST_VERIFY(!GblStringRef_valid(NULL));
    const GblStringView view = GblStringRef_view(NULL);
    GBL_TEST_COMPARE_UINT(view.length, 0);
    GBL_TEST_COMPARE(view.pData, NULL);
    GBL_TEST_EXPECT_ERROR();
    GBL_TEST_COMPARE(GblStringRef_at(pSelf_->pRefs[0], 0), '\0');
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_CTX_END();
}

static GBL_RESULT GblStringRefTestSuite_null_at_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringRefTestSuite_* pSelf_ = GBL_STRING_REF_TEST_SUITE_(pSelf);
    GBL_TEST_EXPECT_ERROR();
    GBL_TEST_COMPARE(GblStringRef_at(pSelf_->pRefs[0], 0), '\0');
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_CTX_END();
}

static GBL_RESULT GblStringRefTestSuite_createEmpty_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringRefTestSuite_* pSelf_ = GBL_STRING_REF_TEST_SUITE_(pSelf);
    pSelf_->pRefs[0] = GblStringRef_create(NULL);
    GBL_CTX_VERIFY_CALL(verifyEmpty_(pCtx, pSelf_->pRefs[0], 1, NULL));
    GBL_CTX_END();
}

static GBL_RESULT GblStringRefTestSuite_empty_at_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringRefTestSuite_* pSelf_ = GBL_STRING_REF_TEST_SUITE_(pSelf);
    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblStringRef_at(pSelf_->pRefs[0], 0), '\0');
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblStringRef_at(pSelf_->pRefs[0], 1), '\0');
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_CTX_END();
}

static GBL_RESULT GblStringRefTestSuite_create_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringRefTestSuite_* pSelf_ = GBL_STRING_REF_TEST_SUITE_(pSelf);
    pSelf_->pRefs[1] = GblStringRef_create("lolz");
    GBL_CTX_VERIFY_CALL(verifyString_(pCtx, pSelf_->pRefs[1], "lolz", 1, NULL));
    GBL_CTX_END();
}

static GBL_RESULT GblStringRefTestSuite_createWithContext_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringRefTestSuite_* pSelf_ = GBL_STRING_REF_TEST_SUITE_(pSelf);
    pSelf_->pRefs[2] = GblStringRef_createWithContext("contextual", pCtx);
    GBL_CTX_VERIFY_CALL(verifyString_(pCtx, pSelf_->pRefs[2], "contextual", 1, pCtx));
    GBL_CTX_END();
}

static GBL_RESULT GblStringRefTestSuite_createFromViewEmpty_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringRefTestSuite_* pSelf_ = GBL_STRING_REF_TEST_SUITE_(pSelf);
    pSelf_->pRefs[3] = GblStringRef_createFromView(GBL_STRV(NULL));
    GBL_CTX_VERIFY_CALL(verifyEmpty_(pCtx, pSelf_->pRefs[3], 1, NULL));
    GBL_CTX_END();
}

static GBL_RESULT GblStringRefTestSuite_createFromView_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringRefTestSuite_* pSelf_ = GBL_STRING_REF_TEST_SUITE_(pSelf);
    pSelf_->pRefs[4] = GblStringRef_createFromView(GBL_STRV("viewz"));
    GBL_CTX_VERIFY_CALL(verifyString_(pCtx, pSelf_->pRefs[4], "viewz", 1, NULL));
    GBL_CTX_END();
}

static GBL_RESULT GblStringRefTestSuite_createFromViewWithContext_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringRefTestSuite_* pSelf_ = GBL_STRING_REF_TEST_SUITE_(pSelf);
    pSelf_->pRefs[5] = GblStringRef_createFromViewWithContext(GBL_STRV("contextual_viewz"), pCtx);
    GBL_CTX_VERIFY_CALL(verifyString_(pCtx, pSelf_->pRefs[5], "contextual_viewz", 1, pCtx));
    GBL_CTX_END();
}

static GBL_RESULT GblStringRefTestSuite_acquire_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringRefTestSuite_* pSelf_ = GBL_STRING_REF_TEST_SUITE_(pSelf);
    pSelf_->pRefs[6] = GblStringRef_acquire(pSelf_->pRefs[5]);
    GBL_CTX_VERIFY_CALL(verifyString_(pCtx, pSelf_->pRefs[6], "contextual_viewz", 2, pCtx));
    GBL_TEST_COMPARE(pSelf_->pRefs[6], pSelf_->pRefs[5]);
    GBL_CTX_END();
}

static GBL_RESULT GblStringRefTestSuite_release_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringRefTestSuite_* pSelf_ = GBL_STRING_REF_TEST_SUITE_(pSelf);
    GblRefCount refCount = GblRef_activeCount();
    GblStringRef_release(pSelf_->pRefs[6]);
    GBL_CTX_VERIFY_CALL(verifyString_(pCtx, pSelf_->pRefs[5], "contextual_viewz", 1, pCtx));
    GBL_TEST_COMPARE(GblRef_activeCount(), refCount);

    GblStringRef_release(pSelf_->pRefs[5]);
    GBL_TEST_COMPARE(GblRef_activeCount(), --refCount);

    GblStringRef_release(pSelf_->pRefs[4]);
    GBL_TEST_COMPARE(GblRef_activeCount(), --refCount);

    GblStringRef_release(pSelf_->pRefs[3]);
    GBL_TEST_COMPARE(GblRef_activeCount(), --refCount);

    GblStringRef_release(pSelf_->pRefs[2]);
    GBL_TEST_COMPARE(GblRef_activeCount(), --refCount);

    GblStringRef_release(pSelf_->pRefs[1]);
    GBL_TEST_COMPARE(GblRef_activeCount(), --refCount);

    GblStringRef_release(pSelf_->pRefs[0]);
    GBL_TEST_COMPARE(GblRef_activeCount(), --refCount);
    GBL_CTX_END();
}

GBL_EXPORT GblType GblStringRefTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "null",                       GblStringRefTestSuite_null_                         },
        { "nullAt",                     GblStringRefTestSuite_null_at_                      },
        { "createEmpty",                GblStringRefTestSuite_createEmpty_                  },
        { "emptyAt",                    GblStringRefTestSuite_empty_at_                     },
        { "create",                     GblStringRefTestSuite_create_                       },
        { "createWithContext",          GblStringRefTestSuite_createWithContext_            },
        { "createFromViewEmpty",        GblStringRefTestSuite_createFromViewEmpty_          },
        { "createFromView",             GblStringRefTestSuite_createFromView_               },
        { "createFromViewWithContext",  GblStringRefTestSuite_createFromViewWithContext_    },
        { "acquire",                    GblStringRefTestSuite_acquire_                      },
        { "release",                    GblStringRefTestSuite_release_                      },
        { NULL,                         NULL                                                }
    };

    const static GblTestSuiteVTable vTable = {
        .pFnSuiteInit   = GblStringRefTestSuite_init_,
        .pFnSuiteFinal  = GblStringRefTestSuite_final_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStatic("StringRefTestSuite"),
                                     &vTable,
                                     sizeof(GblStringRefTestSuite),
                                     sizeof(GblStringRefTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }
    return type;
}


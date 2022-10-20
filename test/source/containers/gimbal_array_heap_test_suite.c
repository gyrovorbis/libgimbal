#include "containers/gimbal_array_heap_test_suite.h"
#include <gimbal/test/gimbal_test.h>
#include <gimbal/containers/gimbal_array_heap.h>

#define GBL_ARRAY_HEAP_TEST_SUITE_(inst)     ((GblArrayHeapTestSuite_*)GBL_INSTANCE_PRIVATE(inst, GBL_ARRAY_HEAP_TEST_SUITE_TYPE))

#define GBL_ARRAY_HEAP_PROFILE_SIZE_     1024

typedef struct GblArrayHeapTestSuite_ {
    GblArrayHeap heaps[5];
} GblArrayHeapTestSuite_;

static const char* stringLiterals_[] = {
    "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
    "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"
};

static int comparator_(const void* pV1, const void* pV2) {
    return strcmp(*(const char**)pV1, *(const char**)pV2);
}

static GBL_RESULT GblArrayHeapTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblArrayHeapTestSuite_* pSelf_ = GBL_ARRAY_HEAP_TEST_SUITE_(pSelf);
    memset(pSelf_, 0, sizeof(GblArrayHeapTestSuite_));
    GBL_CTX_END();
}


GBL_EXPORT GblType GblArrayHeapTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { NULL,                 NULL                                        }
    };

    const static GblTestSuiteClassVTable vTable = {
        .pFnSuiteInit   = GblArrayHeapTestSuite_init_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("GblArrayHeapTestSuite"),
                                     &vTable,
                                     sizeof(GblArrayHeapTestSuite),
                                     sizeof(GblArrayHeapTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }

    return type;
}

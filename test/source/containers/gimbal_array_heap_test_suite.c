#include "containers/gimbal_array_heap_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/containers/gimbal_array_heap.h>

#define GBL_ARRAY_HEAP_TEST_SUITE_(inst)     (GBL_PRIVATE(GblArrayHeapTestSuite, inst))

#define GBL_ARRAY_HEAP_PROFILE_SIZE_     1024

typedef struct GblArrayHeapTestSuite_ {
    GblArrayHeap heaps[5];
} GblArrayHeapTestSuite_;

static const char* stringLiterals_[] = {
    "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
    "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"
};

static int comparator_(const void* pV1, const void* pV2) {
    return strcmp((const char*)pV2, (const char*)pV1);
}

static GBL_RESULT GblArrayHeapTestSuite_verify_(GblTestSuite* pSelf, size_t  heapIndex, size_t  size, const char* pHead) {
    GBL_CTX_BEGIN(pSelf);
    GblArrayHeapTestSuite_* pSelf_ = GBL_ARRAY_HEAP_TEST_SUITE_(pSelf);
    GblArrayHeap* pHeap = &pSelf_->heaps[heapIndex];
    GBL_TEST_COMPARE(GblArrayHeap_size(pHeap), size);
    if(!size) GBL_TEST_VERIFY(GblArrayHeap_empty(pHeap));
    else GBL_TEST_COMPARE((const char*)GblArrayHeap_peek(pHeap), pHead);
    GBL_CTX_END();
}

static GBL_RESULT GblArrayHeapTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblArrayHeapTestSuite_* pSelf_ = GBL_ARRAY_HEAP_TEST_SUITE_(pSelf);
    memset(pSelf_, 0, sizeof(GblArrayHeapTestSuite_));
    GBL_CTX_END();
}

static GBL_RESULT GblArrayHeapTestSuite_create_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblArrayHeapTestSuite_* pSelf_ = GBL_ARRAY_HEAP_TEST_SUITE_(pSelf);
    GBL_CTX_VERIFY_CALL(GblArrayHeap_construct(&pSelf_->heaps[0],
                                               sizeof(const char*),
                                               comparator_,
                                               sizeof(GblArrayHeap),
                                               pCtx));
    GBL_TEST_COMPARE(GblArrayHeap_context(&pSelf_->heaps[0]), pCtx);
    GBL_TEST_COMPARE(GblArrayHeap_elementSize(&pSelf_->heaps[0]), sizeof(const char*));
    GBL_CTX_VERIFY_CALL(GblArrayHeapTestSuite_verify_(pSelf, 0, 0, NULL));
    GBL_CTX_END();
}

static GBL_RESULT GblArrayHeapTestSuite_peekInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblArrayHeapTestSuite_* pSelf_ = GBL_ARRAY_HEAP_TEST_SUITE_(pSelf);
    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblArrayHeap_peek(&pSelf_->heaps[0]), NULL);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblArrayHeapTestSuite_popInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblArrayHeapTestSuite_* pSelf_ = GBL_ARRAY_HEAP_TEST_SUITE_(pSelf);
    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblArrayHeap_pop(&pSelf_->heaps[0], NULL), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblArrayHeapTestSuite_push_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblArrayHeapTestSuite_* pSelf_ = GBL_ARRAY_HEAP_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblArrayHeap_push(&pSelf_->heaps[0], stringLiterals_[1]));
    GBL_CTX_VERIFY_CALL(GblArrayHeapTestSuite_verify_(pSelf, 0, 1, "b"));

    GBL_CTX_END();
}

static GBL_RESULT GblArrayHeapTestSuite_pushLower_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblArrayHeapTestSuite_* pSelf_ = GBL_ARRAY_HEAP_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblArrayHeap_push(&pSelf_->heaps[0], stringLiterals_[3]));
    GBL_CTX_VERIFY_CALL(GblArrayHeapTestSuite_verify_(pSelf, 0, 2, "b"));

    GBL_CTX_END();
}

static GBL_RESULT GblArrayHeapTestSuite_pushHigher_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblArrayHeapTestSuite_* pSelf_ = GBL_ARRAY_HEAP_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblArrayHeap_push(&pSelf_->heaps[0], stringLiterals_[0]));
    GBL_CTX_VERIFY_CALL(GblArrayHeapTestSuite_verify_(pSelf, 0, 3, "a"));

    GBL_CTX_END();
}

static GBL_RESULT GblArrayHeapTestSuite_pushMiddle_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblArrayHeapTestSuite_* pSelf_ = GBL_ARRAY_HEAP_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblArrayHeap_push(&pSelf_->heaps[0], stringLiterals_[2]));
    GBL_CTX_VERIFY_CALL(GblArrayHeapTestSuite_verify_(pSelf, 0, 4, "a"));

    GBL_CTX_END();
}

static GBL_RESULT GblArrayHeapTestSuite_pop_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblArrayHeapTestSuite_* pSelf_ = GBL_ARRAY_HEAP_TEST_SUITE_(pSelf);

    char buffer[100];

    GBL_CTX_VERIFY_CALL(GblArrayHeap_pop(&pSelf_->heaps[0], buffer));
    GBL_TEST_COMPARE(buffer, (const char*)"a");
    GBL_CTX_VERIFY_CALL(GblArrayHeapTestSuite_verify_(pSelf, 0, 3, "b"));

    GBL_CTX_VERIFY_CALL(GblArrayHeap_pop(&pSelf_->heaps[0], buffer));
    GBL_TEST_COMPARE(buffer, (const char*)"b");
    GBL_CTX_VERIFY_CALL(GblArrayHeapTestSuite_verify_(pSelf, 0, 2, "c"));

    GBL_CTX_VERIFY_CALL(GblArrayHeap_pop(&pSelf_->heaps[0], buffer));
    GBL_TEST_COMPARE(buffer, (const char*)"c");
    GBL_CTX_VERIFY_CALL(GblArrayHeapTestSuite_verify_(pSelf, 0, 1, "d"));

    GBL_CTX_VERIFY_CALL(GblArrayHeap_pop(&pSelf_->heaps[0], buffer));
    GBL_TEST_COMPARE(buffer, (const char*)"d");
    GBL_CTX_VERIFY_CALL(GblArrayHeapTestSuite_verify_(pSelf, 0, 0, NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblArrayHeapTestSuite_pushReverse_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblArrayHeapTestSuite_* pSelf_ = GBL_ARRAY_HEAP_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblArrayHeap_push(&pSelf_->heaps[0], stringLiterals_[4]));
    GBL_CTX_VERIFY_CALL(GblArrayHeapTestSuite_verify_(pSelf, 0, 1, "e"));

    GBL_CTX_VERIFY_CALL(GblArrayHeap_push(&pSelf_->heaps[0], stringLiterals_[3]));
    GBL_CTX_VERIFY_CALL(GblArrayHeapTestSuite_verify_(pSelf, 0, 2, "d"));

    GBL_CTX_VERIFY_CALL(GblArrayHeap_push(&pSelf_->heaps[0], stringLiterals_[2]));
    GBL_CTX_VERIFY_CALL(GblArrayHeapTestSuite_verify_(pSelf, 0, 3, "c"));

    GBL_CTX_VERIFY_CALL(GblArrayHeap_push(&pSelf_->heaps[0], stringLiterals_[1]));
    GBL_CTX_VERIFY_CALL(GblArrayHeapTestSuite_verify_(pSelf, 0, 4, "b"));

    GBL_CTX_VERIFY_CALL(GblArrayHeap_push(&pSelf_->heaps[0], stringLiterals_[0]));
    GBL_CTX_VERIFY_CALL(GblArrayHeapTestSuite_verify_(pSelf, 0, 5, "a"));

    GBL_CTX_END();
}

static GBL_RESULT GblArrayHeapTestSuite_reserve_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblArrayHeapTestSuite_* pSelf_ = GBL_ARRAY_HEAP_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblArrayHeap_reserve(&pSelf_->heaps[0], 40));

    GBL_TEST_COMPARE(GblArrayHeap_capacity(&pSelf_->heaps[0]), 40);

    GBL_CTX_END();
}

static GBL_RESULT GblArrayHeapTestSuite_shrinkToFit_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblArrayHeapTestSuite_* pSelf_ = GBL_ARRAY_HEAP_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblArrayHeap_shrinkToFit(&pSelf_->heaps[0]));

    GBL_TEST_COMPARE(GblArrayHeap_capacity(&pSelf_->heaps[0]), GblArrayHeap_size(&pSelf_->heaps[0]));

    GBL_CTX_END();
}

static GBL_RESULT GblArrayHeapTestSuite_copy_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblArrayHeapTestSuite_* pSelf_ = GBL_ARRAY_HEAP_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblArrayHeap_construct(&pSelf_->heaps[1], 1, comparator_));

    GBL_CTX_VERIFY_CALL(GblArrayHeap_copy(&pSelf_->heaps[1], &pSelf_->heaps[0]));

    GBL_CTX_VERIFY_CALL(GblArrayHeapTestSuite_verify_(pSelf, 0, 5, "a"));
    GBL_CTX_VERIFY_CALL(GblArrayHeapTestSuite_verify_(pSelf, 1, 5, "a"));

    GBL_CTX_END();
}

static GBL_RESULT GblArrayHeapTestSuite_move_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblArrayHeapTestSuite_* pSelf_ = GBL_ARRAY_HEAP_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblArrayHeap_construct(&pSelf_->heaps[2], 1, comparator_));

    GBL_CTX_VERIFY_CALL(GblArrayHeap_move(&pSelf_->heaps[2], &pSelf_->heaps[0]));

    GBL_CTX_VERIFY_CALL(GblArrayHeapTestSuite_verify_(pSelf, 0, 0, NULL));
    GBL_CTX_VERIFY_CALL(GblArrayHeapTestSuite_verify_(pSelf, 2, 5, "a"));

    GBL_CTX_END();
}

static GBL_RESULT GblArrayHeapTestSuite_clear_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblArrayHeapTestSuite_* pSelf_ = GBL_ARRAY_HEAP_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblArrayHeap_clear(&pSelf_->heaps[1]));


    GBL_CTX_VERIFY_CALL(GblArrayHeapTestSuite_verify_(pSelf, 1, 0, NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblArrayHeapTestSuite_releaseAcquire_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblArrayHeapTestSuite_* pSelf_ = GBL_ARRAY_HEAP_TEST_SUITE_(pSelf);

    void* pData;
    size_t  size, capacity;

    GBL_CTX_VERIFY_CALL(GblArrayHeap_release(&pSelf_->heaps[2], &pData, &size, &capacity));

    GBL_CTX_VERIFY_CALL(GblArrayHeap_acquire(&pSelf_->heaps[0], pData, size, capacity));

    GBL_CTX_VERIFY_CALL(GblArrayHeapTestSuite_verify_(pSelf, 0, 5, "a"));
    GBL_CTX_VERIFY_CALL(GblArrayHeapTestSuite_verify_(pSelf, 2, 0, NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblArrayHeapTestSuite_destroy_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblArrayHeapTestSuite_* pSelf_ = GBL_ARRAY_HEAP_TEST_SUITE_(pSelf);
    GBL_CTX_VERIFY_CALL(GblArrayHeap_destruct(&pSelf_->heaps[0]));
    GBL_CTX_VERIFY_CALL(GblArrayHeap_destruct(&pSelf_->heaps[1]));
    GBL_CTX_VERIFY_CALL(GblArrayHeap_destruct(&pSelf_->heaps[2]));
    GBL_CTX_END();
}


GBL_EXPORT GblType GblArrayHeapTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "create",             GblArrayHeapTestSuite_create_           },
        { "peekInvalid",        GblArrayHeapTestSuite_peekInvalid_      },
        { "popInvalid",         GblArrayHeapTestSuite_popInvalid_       },
        { "push",               GblArrayHeapTestSuite_push_             },
        { "pushLower",          GblArrayHeapTestSuite_pushLower_        },
        { "pushHigher",         GblArrayHeapTestSuite_pushHigher_       },
        { "pushMiddle",         GblArrayHeapTestSuite_pushMiddle_       },
        { "pop",                GblArrayHeapTestSuite_pop_              },
        { "pushReverse",        GblArrayHeapTestSuite_pushReverse_      },
        { "reserve",            GblArrayHeapTestSuite_reserve_          },
        { "shrinkToFit",        GblArrayHeapTestSuite_shrinkToFit_      },
        { "copy",               GblArrayHeapTestSuite_copy_             },
        { "move",               GblArrayHeapTestSuite_move_             },
        { "clear",              GblArrayHeapTestSuite_clear_            },
        { "releaseAcquire",     GblArrayHeapTestSuite_releaseAcquire_   },
        { "destroy",            GblArrayHeapTestSuite_destroy_          },
        { NULL,                 NULL                                    }
    };

    const static GblTestSuiteVTable vTable = {
        .pFnSuiteInit   = GblArrayHeapTestSuite_init_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStatic("GblArrayHeapTestSuite"),
                                     &vTable,
                                     sizeof(GblArrayHeapTestSuite),
                                     sizeof(GblArrayHeapTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }

    return type;
}

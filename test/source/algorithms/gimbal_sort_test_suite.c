#include "algorithms/gimbal_sort_test_suite.h"
#include <gimbal/algorithms/gimbal_sort.h>
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/algorithms/gimbal_random.h>

#define GBL_SORT_TEST_SUITE_(inst)          (GBL_PRIVATE(GblSortTestSuite, inst))

#define GBL_SORT_TEST_SUITE_WORD_COUNT_     512
#define GBL_SORT_TEST_SUITE_WORD_SIZE_MAX_  50
#define GBL_SORT_TEST_SUITE_WORD_SIZE_MIN_  20
#define GBL_SORT_TEST_SUITE_WORD_CHARS_     NULL

typedef struct GblSortTestSuite_ {
    char words[GBL_SORT_TEST_SUITE_WORD_COUNT_][GBL_SORT_TEST_SUITE_WORD_SIZE_MAX_+1];
    char sorted[GBL_SORT_TEST_SUITE_WORD_COUNT_][GBL_SORT_TEST_SUITE_WORD_SIZE_MAX_+1];
} GblSortTestSuite_;

static void* GblSortTestSuite_randomizeWords_(GblTestSuite* pSelf) {
    GblSortTestSuite_* pSelf_ = GBL_SORT_TEST_SUITE_(pSelf);

    memcpy(pSelf_->sorted, pSelf_->words, sizeof(pSelf_->words));

    return pSelf_->sorted;
}

static int GblSortTestSuite_comparator_(const void* p1, const void* p2) {
    return strcmp((const char*)p1, (const char*)p2);
}

static GBL_RESULT GblSortTestSuite_testSort_(GblTestSuite* pSelf, GblSortFn pFnSort) {
    GBL_CTX_BEGIN(pSelf);

    GblSortTestSuite_* pSelf_ = GBL_SORT_TEST_SUITE_(pSelf);

    pFnSort(GblSortTestSuite_randomizeWords_(pSelf),
            GBL_SORT_TEST_SUITE_WORD_COUNT_,
            GBL_SORT_TEST_SUITE_WORD_SIZE_MAX_+1,
            GblSortTestSuite_comparator_);

    for(size_t i = 1; i < GBL_SORT_TEST_SUITE_WORD_COUNT_; ++i)
        GBL_TEST_VERIFY(GblSortTestSuite_comparator_(pSelf_->sorted[i - 1],
                                                     pSelf_->sorted[i]) <= 0);

    GBL_CTX_END();
}

static GBL_RESULT GblSortTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblSortTestSuite_* pSelf_ = GBL_SORT_TEST_SUITE_(pSelf);
    for(size_t  w = 0; w < GBL_SORT_TEST_SUITE_WORD_COUNT_; ++w)
        gblRandString(pSelf_->words[w],
                      GBL_SORT_TEST_SUITE_WORD_SIZE_MIN_,
                      GBL_SORT_TEST_SUITE_WORD_SIZE_MAX_,
                      GBL_SORT_TEST_SUITE_WORD_CHARS_);

    GBL_CTX_END();
}

static GBL_RESULT GblSortTestSuite_bubbleSort_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GBL_CTX_VERIFY_CALL(GblSortTestSuite_testSort_(pSelf, gblSortBubble));
    GBL_CTX_END();
}

static GBL_RESULT GblSortTestSuite_selectionSort_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GBL_CTX_VERIFY_CALL(GblSortTestSuite_testSort_(pSelf, gblSortSelection));
    GBL_CTX_END();
}

static GBL_RESULT GblSortTestSuite_insertionSort_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GBL_CTX_VERIFY_CALL(GblSortTestSuite_testSort_(pSelf, gblSortInsertion));
    GBL_CTX_END();
}

static GBL_RESULT GblSortTestSuite_shellSort_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GBL_CTX_VERIFY_CALL(GblSortTestSuite_testSort_(pSelf, gblSortShell));
    GBL_CTX_END();
}

static GBL_RESULT GblSortTestSuite_combSort_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GBL_CTX_VERIFY_CALL(GblSortTestSuite_testSort_(pSelf, gblSortComb));
    GBL_CTX_END();
}

static GBL_RESULT GblSortTestSuite_mergeSort_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GBL_CTX_VERIFY_CALL(GblSortTestSuite_testSort_(pSelf, gblSortMerge));
    GBL_CTX_END();
}

static GBL_RESULT GblSortTestSuite_quickSort_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GBL_CTX_VERIFY_CALL(GblSortTestSuite_testSort_(pSelf, gblSortQuick));
    GBL_CTX_END();
}

static GBL_RESULT GblSortTestSuite_heapSort_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GBL_CTX_VERIFY_CALL(GblSortTestSuite_testSort_(pSelf, gblSortHeap));
    GBL_CTX_END();
}

GBL_EXPORT GblType GblSortTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "bubbleSort",         GblSortTestSuite_bubbleSort_       },
        { "selectionSort",      GblSortTestSuite_selectionSort_    },
        { "insertionSort",      GblSortTestSuite_insertionSort_    },
        { "shellSort",          GblSortTestSuite_shellSort_        },
        { "heapSort",           GblSortTestSuite_heapSort_         },
        { "combSort",           GblSortTestSuite_combSort_         },
        { "mergeSort",          GblSortTestSuite_mergeSort_        },
        { "quickSort",          GblSortTestSuite_quickSort_        },
        { NULL,                 NULL                               }
    };

    const static GblTestSuiteVTable vTable = {
        .pFnSuiteInit   = GblSortTestSuite_init_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        type = GblTestSuite_register(GblQuark_internStatic("GblSortTestSuite"),
                                     &vTable,
                                     sizeof(GblSortTestSuite),
                                     sizeof(GblSortTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
    }
    return type;
}

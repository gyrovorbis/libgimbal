#include "containers/gimbal_array_list_test_suite.h"
#include <gimbal/test/gimbal_test.h>
#include <gimbal/containers/gimbal_array_list.h>

#define GBL_ARRAY_LIST_TEST_SUITE_PUSH_BACK_STRESS_TEST_SIZE_ 2047
#define GBL_ARRAY_LIST_TEST_SUITE_(inst)     ((GblArrayListTestSuite_*)GBL_INSTANCE_PRIVATE(inst, GBL_ARRAY_LIST_TEST_SUITE_TYPE))

typedef struct GblArrayListTestSuite_ {
    struct {
        GblArrayList   vector;
        const char* pExtraData[4];
    } stringList;
    void*   pReleasedData;
    GblSize releasedSize;
    GblSize releasedCapacity;
} GblArrayListTestSuite_;

const static char* initialStrings_[4] = {
    "1",
    "2",
    "3",
    "4"
};

GBL_RESULT GblArrayListTestSuite_verify_(const GblArrayList* pVec, const char** pList, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblSize i = 0;
    const char* pString;

    while ((pString = pList[i])) {
        const char* pData = *(const char**)GblArrayList_at(pVec, i++);
        GBL_TEST_VERIFY(pData);
        GBL_TEST_COMPARE(pString, pData);
    }

    GBL_TEST_COMPARE(i, GblArrayList_size(pVec));
    GBL_API_END();
}

GBL_RESULT GblArrayListTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayListTestSuite_* pSelf_ = GBL_ARRAY_LIST_TEST_SUITE_(pSelf);
    memset(pSelf_, 0, sizeof(GblArrayListTestSuite_));
    GBL_API_END();
}

GBL_RESULT GblArrayListTestSuite_final_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}

GBL_RESULT GblArrayListTestSuite_constructWithEntries_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayListTestSuite_* pSelf_ = GBL_ARRAY_LIST_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblArrayList_construct(&pSelf_->stringList.vector,
                                            sizeof(const char*),
                                            4,
                                            initialStrings_,
                                            sizeof(pSelf_->stringList),
                                            GBL_FALSE,
                                            pCtx));

    GBL_TEST_COMPARE(GblArrayList_stackBuffer(&pSelf_->stringList.vector),
                     pSelf_->stringList.pExtraData);
    GBL_TEST_COMPARE(GblArrayList_elementSize(&pSelf_->stringList.vector), sizeof(const char*));
    GBL_TEST_COMPARE(GblArrayList_context(&pSelf_->stringList.vector), pCtx);
    GBL_TEST_COMPARE(GblArrayList_size(&pSelf_->stringList.vector), 4);
    GBL_TEST_VERIFY(!GblArrayList_empty(&pSelf_->stringList.vector));

    GBL_API_VERIFY_CALL(GblArrayListTestSuite_verify_(&pSelf_->stringList.vector,
                                                   (const char*[]) {
                                                        "1", "2", "3", "4", NULL
                                                    },
                                                    pCtx));

    GBL_TEST_VERIFY(GblArrayList_stack(&pSelf_->stringList.vector));
    GBL_API_END();
}

GBL_RESULT GblArrayListTestSuite_assign_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayListTestSuite_* pSelf_ = GBL_ARRAY_LIST_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblArrayList_assign(&pSelf_->stringList.vector,
                                         (const char*[]) {
                                            "5", "6", "7", "8", "9", "10"
                                         },
                                         6));
    GBL_API_VERIFY_CALL(GblArrayListTestSuite_verify_(&pSelf_->stringList.vector,
                                                   (const char*[]) {
                                                        "5", "6", "7", "8", "9", "10", NULL
                                                    },
                                                    pCtx));

    GBL_TEST_VERIFY(!GblArrayList_stack(&pSelf_->stringList.vector));
    GBL_API_END();
}

GBL_RESULT GblArrayListTestSuite_release_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayListTestSuite_* pSelf_ = GBL_ARRAY_LIST_TEST_SUITE_(pSelf);
    const GblSize prevCapacity = GblArrayList_capacity(&pSelf_->stringList.vector);
    GBL_API_VERIFY_CALL(GblArrayList_release(&pSelf_->stringList.vector,
                                          &pSelf_->pReleasedData,
                                          &pSelf_->releasedSize,
                                          &pSelf_->releasedCapacity));
    GBL_TEST_VERIFY(pSelf_->pReleasedData);
    GBL_TEST_COMPARE(pSelf_->releasedSize, 6);
    GBL_TEST_COMPARE(pSelf_->releasedCapacity, prevCapacity);
    GBL_TEST_VERIFY(GblArrayList_empty(&pSelf_->stringList.vector));
    GBL_TEST_COMPARE(GblArrayList_data(&pSelf_->stringList.vector),
                     pSelf_->stringList.pExtraData);

    GBL_TEST_VERIFY(GblArrayList_stack(&pSelf_->stringList.vector));

    GBL_API_END();
}

GBL_RESULT GblArrayListTestSuite_acquire_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayListTestSuite_* pSelf_ = GBL_ARRAY_LIST_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblArrayList_acquire(&pSelf_->stringList.vector,
                                          pSelf_->pReleasedData,
                                          pSelf_->releasedSize,
                                          pSelf_->releasedCapacity));

    GBL_TEST_COMPARE(GblArrayList_data(&pSelf_->stringList.vector), pSelf_->pReleasedData);
    GBL_TEST_COMPARE(GblArrayList_capacity(&pSelf_->stringList.vector), pSelf_->releasedCapacity);

    GBL_API_VERIFY_CALL(GblArrayListTestSuite_verify_(&pSelf_->stringList.vector,
                                                   (const char*[]) {
                                                        "5", "6", "7", "8", "9", "10", NULL
                                                    },
                                                    pCtx));
    GBL_TEST_VERIFY(!GblArrayList_stack(&pSelf_->stringList.vector));

    GBL_API_END();
}

GBL_RESULT GblArrayListTestSuite_atInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayListTestSuite_* pSelf_ = GBL_ARRAY_LIST_TEST_SUITE_(pSelf);
    GBL_TEST_EXPECT_ERROR();
    void* pData = GblArrayList_at(&pSelf_->stringList.vector, 7);
    GBL_TEST_COMPARE(GBL_API_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_TEST_COMPARE(pData, NULL);
    GBL_API_CLEAR_LAST_RECORD();
    GBL_API_END();
}

GBL_RESULT GblArrayListTestSuite_front_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayListTestSuite_* pSelf_ = GBL_ARRAY_LIST_TEST_SUITE_(pSelf);
    GBL_TEST_COMPARE(*(const char**)GblArrayList_front(&pSelf_->stringList.vector),
                     "5");
    GBL_API_END();
}

GBL_RESULT GblArrayListTestSuite_back_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayListTestSuite_* pSelf_ = GBL_ARRAY_LIST_TEST_SUITE_(pSelf);
    GBL_TEST_COMPARE(*(const char**)GblArrayList_back(&pSelf_->stringList.vector),
                     "10");
    GBL_API_END();
}

GBL_RESULT GblArrayListTestSuite_insertInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayListTestSuite_* pSelf_ = GBL_ARRAY_LIST_TEST_SUITE_(pSelf);
    GBL_TEST_EXPECT_ERROR();
    void* pData = GblArrayList_insert(&pSelf_->stringList.vector,
                                   20,
                                   1,
                                   "lol");
    GBL_TEST_COMPARE(GBL_API_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_TEST_COMPARE(pData, NULL);
    GBL_API_CLEAR_LAST_RECORD();
    GBL_API_END();
}

GBL_RESULT GblArrayListTestSuite_insertFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayListTestSuite_* pSelf_ = GBL_ARRAY_LIST_TEST_SUITE_(pSelf);

    void* pData = GblArrayList_insert(&pSelf_->stringList.vector,
                                   0,
                                   2,
                                   (const char*[]) {
                                      "-1", "0"
                                   });
    GBL_API_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(*(const char**)pData, "-1");

    GBL_API_VERIFY_CALL(GblArrayListTestSuite_verify_(&pSelf_->stringList.vector,
                                                   (const char*[]) {
                                                        "-1", "0", "5", "6", "7", "8", "9", "10", NULL
                                                    },
                                                    pCtx));

    GBL_API_END();
}

GBL_RESULT GblArrayListTestSuite_insertBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayListTestSuite_* pSelf_ = GBL_ARRAY_LIST_TEST_SUITE_(pSelf);

    void* pData = GblArrayList_insert(&pSelf_->stringList.vector,
                                   8,
                                   1,
                                   (const char*[]) {
                                      "11",
                                   });
    GBL_API_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(*(const char**)pData, "11");

    GBL_API_VERIFY_CALL(GblArrayListTestSuite_verify_(&pSelf_->stringList.vector,
                                                   (const char*[]) {
                                                        "-1", "0", "5", "6", "7", "8", "9", "10", "11", NULL
                                                    },
                                                    pCtx));
    GBL_API_END();
}

GBL_RESULT GblArrayListTestSuite_insertMiddle_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayListTestSuite_* pSelf_ = GBL_ARRAY_LIST_TEST_SUITE_(pSelf);

    void* pData = GblArrayList_insert(&pSelf_->stringList.vector,
                                   2,
                                   3,
                                   (const char*[]) {
                                      "1", "2", "3"
                                   });
    GBL_API_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(*(const char**)pData, "1");

    GBL_API_VERIFY_CALL(GblArrayListTestSuite_verify_(&pSelf_->stringList.vector,
                                                   (const char*[]) {
                                                        "-1", "0", "1", "2", "3", "5", "6", "7", "8", "9", "10", "11", NULL
                                                    },
                                                    pCtx));
    GBL_API_END();
}

GBL_RESULT GblArrayListTestSuite_pushFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayListTestSuite_* pSelf_ = GBL_ARRAY_LIST_TEST_SUITE_(pSelf);
    const char* pStr = "-2";
    GBL_API_VERIFY_CALL(GblArrayList_pushFront(&pSelf_->stringList.vector,
                                            &pStr));
    GBL_API_VERIFY_CALL(GblArrayListTestSuite_verify_(&pSelf_->stringList.vector,
                                                   (const char*[]) {
                                                        "-2", "-1", "0", "1", "2", "3", "5", "6", "7", "8", "9", "10", "11", NULL
                                                    },
                                                    pCtx));
    GBL_API_END();
}

GBL_RESULT GblArrayListTestSuite_pushBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayListTestSuite_* pSelf_ = GBL_ARRAY_LIST_TEST_SUITE_(pSelf);
    const char* pStr = "12";
    GBL_API_VERIFY_CALL(GblArrayList_pushBack(&pSelf_->stringList.vector,
                                           &pStr));
    GBL_API_VERIFY_CALL(GblArrayListTestSuite_verify_(&pSelf_->stringList.vector,
                                                   (const char*[]) {
                                                        "-2", "-1", "0", "1", "2", "3", "5", "6", "7", "8", "9", "10", "11", "12", NULL
                                                    },
                                                    pCtx));
    GBL_API_END();
}

GBL_RESULT GblArrayListTestSuite_append_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayListTestSuite_* pSelf_ = GBL_ARRAY_LIST_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblArrayList_append(&pSelf_->stringList.vector,
                                         (const char*[]) {
                                            "13", "14"
                                         },
                                         2));
    GBL_API_VERIFY_CALL(GblArrayListTestSuite_verify_(&pSelf_->stringList.vector,
                                                   (const char*[]) {
                                                        "-2", "-1", "0", "1", "2", "3", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", NULL
                                                    },
                                                    pCtx));
    GBL_API_END();
}

GBL_RESULT GblArrayListTestSuite_prepend_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayListTestSuite_* pSelf_ = GBL_ARRAY_LIST_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblArrayList_prepend(&pSelf_->stringList.vector,
                                         (const char*[]) {
                                            "-4", "-3"
                                         },
                                         2));
    GBL_API_VERIFY_CALL(GblArrayListTestSuite_verify_(&pSelf_->stringList.vector,
                                                   (const char*[]) {
                                                        "-4", "-3", "-2", "-1", "0", "1", "2", "3", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", NULL
                                                    },
                                                    pCtx));
    GBL_API_END();
}

GBL_RESULT GblArrayListTestSuite_popFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayListTestSuite_* pSelf_ = GBL_ARRAY_LIST_TEST_SUITE_(pSelf);
    char* pString = NULL;
    GBL_API_VERIFY_CALL(GblArrayList_popFront(&pSelf_->stringList.vector,
                                           &pString));
    GBL_TEST_COMPARE(pString, "-4");
    GBL_API_VERIFY_CALL(GblArrayListTestSuite_verify_(&pSelf_->stringList.vector,
                                                   (const char*[]) {
                                                        "-3", "-2", "-1", "0", "1", "2", "3", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", NULL
                                                    },
                                                    pCtx));

    GBL_API_END();
}

GBL_RESULT GblArrayListTestSuite_popBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayListTestSuite_* pSelf_ = GBL_ARRAY_LIST_TEST_SUITE_(pSelf);
    char* pString = NULL;
    GBL_API_VERIFY_CALL(GblArrayList_popBack(&pSelf_->stringList.vector,
                                           &pString));
    GBL_TEST_COMPARE(pString, "14");
    GBL_API_VERIFY_CALL(GblArrayListTestSuite_verify_(&pSelf_->stringList.vector,
                                                   (const char*[]) {
                                                        "-3", "-2", "-1", "0", "1", "2", "3", "5", "6", "7", "8", "9", "10", "11", "12", "13", NULL
                                                    },
                                                    pCtx));
    GBL_API_END();
}

GBL_RESULT GblArrayListTestSuite_eraseInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayListTestSuite_* pSelf_ = GBL_ARRAY_LIST_TEST_SUITE_(pSelf);
    GBL_TEST_EXPECT_ERROR();
    GblArrayList_erase(&pSelf_->stringList.vector,
                    128,
                    0);
    GBL_TEST_COMPARE(GBL_API_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_API_CLEAR_LAST_RECORD();
    GBL_API_END();
}

GBL_RESULT GblArrayListTestSuite_eraseFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayListTestSuite_* pSelf_ = GBL_ARRAY_LIST_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblArrayList_erase(&pSelf_->stringList.vector,
                                        0,
                                        3));
    GBL_API_VERIFY_CALL(GblArrayListTestSuite_verify_(&pSelf_->stringList.vector,
                                                   (const char*[]) {
                                                        "0", "1", "2", "3", "5", "6", "7", "8", "9", "10", "11", "12", "13", NULL
                                                    },
                                                    pCtx));
    GBL_API_END();
}

GBL_RESULT GblArrayListTestSuite_eraseBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayListTestSuite_* pSelf_ = GBL_ARRAY_LIST_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblArrayList_erase(&pSelf_->stringList.vector,
                                        GblArrayList_size(&pSelf_->stringList.vector)-1,
                                        1));
    GBL_API_VERIFY_CALL(GblArrayList_erase(&pSelf_->stringList.vector,
                                        GblArrayList_size(&pSelf_->stringList.vector)-2,
                                        2));
    GBL_API_VERIFY_CALL(GblArrayListTestSuite_verify_(&pSelf_->stringList.vector,
                                                   (const char*[]) {
                                                        "0", "1", "2", "3", "5", "6", "7", "8", "9", "10",  NULL
                                                    },
                                                    pCtx));
    GBL_API_END();
}

GBL_RESULT GblArrayListTestSuite_eraseMiddle_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayListTestSuite_* pSelf_ = GBL_ARRAY_LIST_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblArrayList_erase(&pSelf_->stringList.vector,
                                        1,
                                        6));
    GBL_API_VERIFY_CALL(GblArrayListTestSuite_verify_(&pSelf_->stringList.vector,
                                                   (const char*[]) {
                                                        "0", "8", "9", "10",  NULL
                                                    },
                                                    pCtx));

    GBL_TEST_VERIFY(!GblArrayList_stack(&pSelf_->stringList.vector));

    GBL_API_END();
}

GBL_RESULT GblArrayListTestSuite_emplace_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayListTestSuite_* pSelf_ = GBL_ARRAY_LIST_TEST_SUITE_(pSelf);
    char** ppEntry = GblArrayList_emplace(&pSelf_->stringList.vector, 2);
    *ppEntry = "2";
    GBL_API_VERIFY_CALL(GblArrayListTestSuite_verify_(&pSelf_->stringList.vector,
                                                   (const char*[]) {
                                                        "0", "8", "2", "9", "10",  NULL
                                                    },
                                                    pCtx));
    GBL_API_END();
}

GBL_RESULT GblArrayListTestSuite_emplaceFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayListTestSuite_* pSelf_ = GBL_ARRAY_LIST_TEST_SUITE_(pSelf);
    char** ppEntry = GblArrayList_emplaceFront(&pSelf_->stringList.vector);
    *ppEntry = "-1";
    GBL_API_VERIFY_CALL(GblArrayListTestSuite_verify_(&pSelf_->stringList.vector,
                                                   (const char*[]) {
                                                        "-1", "0", "8", "2", "9", "10",  NULL
                                                    },
                                                    pCtx));
    GBL_API_END();
}

GBL_RESULT GblArrayListTestSuite_emplaceBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayListTestSuite_* pSelf_ = GBL_ARRAY_LIST_TEST_SUITE_(pSelf);
    char** ppEntry = GblArrayList_emplaceBack(&pSelf_->stringList.vector);
    *ppEntry = "13";
    GBL_API_VERIFY_CALL(GblArrayListTestSuite_verify_(&pSelf_->stringList.vector,
                                                   (const char*[]) {
                                                        "-1", "0", "8", "2", "9", "10", "13", NULL
                                                    },
                                                    pCtx));
    GBL_API_END();
}

GBL_RESULT GblArrayListTestSuite_clear_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayListTestSuite_* pSelf_ = GBL_ARRAY_LIST_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblArrayList_clear(&pSelf_->stringList.vector));
    GBL_TEST_VERIFY(GblArrayList_empty(&pSelf_->stringList.vector));
    GBL_TEST_COMPARE(GblArrayList_data(&pSelf_->stringList.vector),
                     pSelf_->stringList.pExtraData);

    GBL_TEST_VERIFY(GblArrayList_stack(&pSelf_->stringList.vector));
    GBL_API_END();
}

GBL_RESULT GblArrayListTestSuite_reserve_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GblArrayListTestSuite_* pSelf_ = GBL_ARRAY_LIST_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblArrayList_reserve(&pSelf_->stringList.vector, 128));
    GBL_TEST_VERIFY(GblArrayList_empty(&pSelf_->stringList.vector));
    GBL_TEST_COMPARE(GblArrayList_capacity(&pSelf_->stringList.vector),  128);
    GBL_TEST_VERIFY(!GblArrayList_stack(&pSelf_->stringList.vector));

    GBL_API_END();
}

GBL_RESULT GblArrayListTestSuite_resizeSmaller_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayListTestSuite_* pSelf_ = GBL_ARRAY_LIST_TEST_SUITE_(pSelf);

    GblSize oldCapacity = GblArrayList_capacity(&pSelf_->stringList.vector);
    GBL_API_VERIFY_CALL(GblArrayList_resize(&pSelf_->stringList.vector, 4));
    GBL_TEST_COMPARE(GblArrayList_size(&pSelf_->stringList.vector), 4);
    GBL_TEST_COMPARE(GblArrayList_capacity(&pSelf_->stringList.vector), oldCapacity);
    GBL_TEST_VERIFY(!GblArrayList_stack(&pSelf_->stringList.vector));


    GBL_API_END();
}

GBL_RESULT GblArrayListTestSuite_resizeLarger_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayListTestSuite_* pSelf_ = GBL_ARRAY_LIST_TEST_SUITE_(pSelf);

    GBL_API_VERIFY_CALL(GblArrayList_resize(&pSelf_->stringList.vector, 256));
    GBL_TEST_COMPARE(GblArrayList_size(&pSelf_->stringList.vector), 256);
    GBL_TEST_COMPARE(GblArrayList_capacity(&pSelf_->stringList.vector), 256);

    GBL_API_END();
}

GBL_RESULT GblArrayListTestSuite_shrinkToFitHeap_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayListTestSuite_* pSelf_ = GBL_ARRAY_LIST_TEST_SUITE_(pSelf);

    GBL_API_VERIFY_CALL(GblArrayList_resize(&pSelf_->stringList.vector, 5));

    *((const char**)GblArrayList_at(&pSelf_->stringList.vector, 0)) = "1";
    *((const char**)GblArrayList_at(&pSelf_->stringList.vector, 1)) = "2";
    *((const char**)GblArrayList_at(&pSelf_->stringList.vector, 2)) = "3";
    *((const char**)GblArrayList_at(&pSelf_->stringList.vector, 3)) = "4";
    *((const char**)GblArrayList_at(&pSelf_->stringList.vector, 4)) = "5";

    GBL_API_VERIFY_CALL(GblArrayList_shrinkToFit(&pSelf_->stringList.vector));
    GBL_TEST_COMPARE(GblArrayList_capacity(&pSelf_->stringList.vector), 5);
    GBL_TEST_VERIFY(!GblArrayList_stack(&pSelf_->stringList.vector));

    GBL_API_VERIFY_CALL(GblArrayListTestSuite_verify_(&pSelf_->stringList.vector,
                                                   (const char*[]) {
                                                        "1", "2", "3", "4", "5", NULL
                                                    },
                                                    pCtx));

    GBL_API_END();
}

GBL_RESULT GblArrayListTestSuite_shrinkToFitStack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayListTestSuite_* pSelf_ = GBL_ARRAY_LIST_TEST_SUITE_(pSelf);

    GBL_API_VERIFY_CALL(GblArrayList_resize(&pSelf_->stringList.vector, 4));

    GBL_API_VERIFY_CALL(GblArrayList_shrinkToFit(&pSelf_->stringList.vector));
    GBL_TEST_COMPARE(GblArrayList_capacity(&pSelf_->stringList.vector), 4);
    GBL_TEST_VERIFY(GblArrayList_stack(&pSelf_->stringList.vector));

    GBL_API_VERIFY_CALL(GblArrayListTestSuite_verify_(&pSelf_->stringList.vector,
                                                   (const char*[]) {
                                                        "1", "2", "3", "4", NULL
                                                    },
                                                    pCtx));

    GBL_API_END();
}




GBL_RESULT GblArrayListTestSuite_pushBackStressTest_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayListTestSuite_* pSelf_ = GBL_ARRAY_LIST_TEST_SUITE_(pSelf);

    const char* pLiteral = "LOLOL";

    for(GblSize i = 0; i < GBL_ARRAY_LIST_TEST_SUITE_PUSH_BACK_STRESS_TEST_SIZE_; ++i) {
        GblArrayList_pushBack(&pSelf_->stringList.vector, &pLiteral);
    }

    GBL_API_END();
}


GBL_RESULT GblArrayListTestSuite_destruct_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblArrayListTestSuite_* pSelf_ = GBL_ARRAY_LIST_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblArrayList_destruct(&pSelf_->stringList.vector));
    GBL_API_END();
}

GBL_EXPORT GblType GblArrayListTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "constructWithEntries",   GblArrayListTestSuite_constructWithEntries_    },
        { "assign",                 GblArrayListTestSuite_assign_                  },
        { "release",                GblArrayListTestSuite_release_                 },
        { "acquire",                GblArrayListTestSuite_acquire_                 },
        { "atInvalid",              GblArrayListTestSuite_atInvalid_               },
        { "front",                  GblArrayListTestSuite_front_                   },
        { "back",                   GblArrayListTestSuite_back_                    },
        { "insertInvalid",          GblArrayListTestSuite_insertInvalid_           },
        { "insertFront",            GblArrayListTestSuite_insertFront_             },
        { "insertBack",             GblArrayListTestSuite_insertBack_              },
        { "insertMiddle",           GblArrayListTestSuite_insertMiddle_            },
        { "pushFront",              GblArrayListTestSuite_pushFront_               },
        { "pushBack",               GblArrayListTestSuite_pushBack_                },
        { "append",                 GblArrayListTestSuite_append_                  },
        { "prepend",                GblArrayListTestSuite_prepend_                 },
        { "popFront",               GblArrayListTestSuite_popFront_                },
        { "popBack",                GblArrayListTestSuite_popBack_                 },
        { "eraseInvalid",           GblArrayListTestSuite_eraseInvalid_            },
        { "eraseFront",             GblArrayListTestSuite_eraseFront_              },
        { "eraseBack",              GblArrayListTestSuite_eraseBack_               },
        { "eraseMiddle",            GblArrayListTestSuite_eraseMiddle_             },
        { "emplace",                GblArrayListTestSuite_emplace_                 },
        { "emplaceFront",           GblArrayListTestSuite_emplaceFront_            },
        { "emplaceBack",            GblArrayListTestSuite_emplaceBack_             },
        { "clear",                  GblArrayListTestSuite_clear_                   },
        { "reserve",                GblArrayListTestSuite_reserve_                 },
        { "resizeSmaller",          GblArrayListTestSuite_resizeSmaller_           },
        { "resizeLarger",           GblArrayListTestSuite_resizeSmaller_           },
        { "shrinkToFitHeap",        GblArrayListTestSuite_shrinkToFitHeap_         },
        { "shrinkToFitStack",       GblArrayListTestSuite_shrinkToFitStack_        },
        { "pushBackStressTest",     GblArrayListTestSuite_pushBackStressTest_      },
        { "destruct",               GblArrayListTestSuite_destruct_                },
        { NULL,                     NULL  }
    };

    const static GblTestSuiteClassVTable vTable = {
        .pFnSuiteInit   = GblArrayListTestSuite_init_,
        .pFnSuiteFinal  = GblArrayListTestSuite_final_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_API_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("ArrayListTestSuite"),
                                     &vTable,
                                     sizeof(GblArrayListTestSuite),
                                     sizeof(GblArrayListTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_API_VERIFY_LAST_RECORD();
        GBL_API_END_BLOCK();
    }
    return type;
}
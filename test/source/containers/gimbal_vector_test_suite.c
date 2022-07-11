#include "containers/gimbal_vector_test_suite.h"
#include <gimbal/test/gimbal_test.h>
#include <gimbal/containers/gimbal_vector.h>

#define GBL_VECTOR_TEST_SUITE_(inst)     ((GblVectorTestSuite_*)GBL_INSTANCE_PRIVATE(inst, GBL_VECTOR_TEST_SUITE_TYPE))

typedef struct GblVectorTestSuite_ {
    struct {
        GblVector   vector;
        const char* pExtraData[4];
    } stringList;
    void*   pReleasedData;
    GblSize releasedSize;
    GblSize releasedCapacity;
} GblVectorTestSuite_;

const static char* initialStrings_[4] = {
    "1",
    "2",
    "3",
    "4"
};

GBL_RESULT GblVectorTestSuite_verify_(const GblVector* pVec, const char** pList, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblSize i = 0;
    const char* pString;

    while ((pString = pList[i])) {
        const char* pData = *(const char**)GblVector_at(pVec, i++);
        GBL_TEST_VERIFY(pData);
        GBL_TEST_COMPARE(pString, pData);
    }

    GBL_TEST_COMPARE(i, GblVector_size(pVec));
    GBL_API_END();
}

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

    GBL_TEST_COMPARE(GblVector_stackBuffer(&pSelf_->stringList.vector),
                     pSelf_->stringList.pExtraData);
    GBL_TEST_COMPARE(GblVector_elementSize(&pSelf_->stringList.vector), sizeof(const char*));
    GBL_TEST_COMPARE(GblVector_context(&pSelf_->stringList.vector), pCtx);
    GBL_TEST_COMPARE(GblVector_size(&pSelf_->stringList.vector), 4);
    GBL_TEST_VERIFY(!GblVector_empty(&pSelf_->stringList.vector));

    GBL_API_VERIFY_CALL(GblVectorTestSuite_verify_(&pSelf_->stringList.vector,
                                                   (const char*[]) {
                                                        "1", "2", "3", "4", NULL
                                                    },
                                                    pCtx));

    //GBL_TEST_VERIFY(GblVector_stack(&pSelf_->stringList.vector));
    GBL_API_END();
}

GBL_RESULT GblVectorTestSuite_assign_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblVectorTestSuite_* pSelf_ = GBL_VECTOR_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblVector_assign(&pSelf_->stringList.vector,
                                         (const char*[]) {
                                            "5", "6", "7", "8", "9", "10"
                                         },
                                         6));
    GBL_API_VERIFY_CALL(GblVectorTestSuite_verify_(&pSelf_->stringList.vector,
                                                   (const char*[]) {
                                                        "5", "6", "7", "8", "9", "10", NULL
                                                    },
                                                    pCtx));
    GBL_API_END();
}

GBL_RESULT GblVectorTestSuite_release_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblVectorTestSuite_* pSelf_ = GBL_VECTOR_TEST_SUITE_(pSelf);
    const GblSize prevCapacity = GblVector_capacity(&pSelf_->stringList.vector);
    GBL_API_VERIFY_CALL(GblVector_release(&pSelf_->stringList.vector,
                                          &pSelf_->pReleasedData,
                                          &pSelf_->releasedSize,
                                          &pSelf_->releasedCapacity));
    GBL_TEST_VERIFY(pSelf_->pReleasedData);
    GBL_TEST_COMPARE(pSelf_->releasedSize, 6);
    GBL_TEST_COMPARE(pSelf_->releasedCapacity, prevCapacity);
    GBL_TEST_VERIFY(GblVector_empty(&pSelf_->stringList.vector));
    GBL_TEST_COMPARE(GblVector_data(&pSelf_->stringList.vector),
                     pSelf_->stringList.pExtraData);
    GBL_API_END();
}

GBL_RESULT GblVectorTestSuite_acquire_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblVectorTestSuite_* pSelf_ = GBL_VECTOR_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblVector_acquire(&pSelf_->stringList.vector,
                                          pSelf_->pReleasedData,
                                          pSelf_->releasedSize,
                                          pSelf_->releasedCapacity));

    GBL_TEST_COMPARE(GblVector_data(&pSelf_->stringList.vector), pSelf_->pReleasedData);
    GBL_TEST_COMPARE(GblVector_capacity(&pSelf_->stringList.vector), pSelf_->releasedCapacity);

    GBL_API_VERIFY_CALL(GblVectorTestSuite_verify_(&pSelf_->stringList.vector,
                                                   (const char*[]) {
                                                        "5", "6", "7", "8", "9", "10", NULL
                                                    },
                                                    pCtx));

    GBL_API_END();
}

GBL_RESULT GblVectorTestSuite_atInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblVectorTestSuite_* pSelf_ = GBL_VECTOR_TEST_SUITE_(pSelf);
    GBL_TEST_EXPECT_ERROR();
    void* pData = GblVector_at(&pSelf_->stringList.vector, 7);
    GBL_TEST_COMPARE(GBL_API_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_TEST_COMPARE(pData, NULL);
    GBL_API_CLEAR_LAST_RECORD();
    GBL_API_END();
}

GBL_RESULT GblVectorTestSuite_front_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblVectorTestSuite_* pSelf_ = GBL_VECTOR_TEST_SUITE_(pSelf);
    GBL_TEST_COMPARE(*(const char**)GblVector_front(&pSelf_->stringList.vector),
                     "5");
    GBL_API_END();
}

GBL_RESULT GblVectorTestSuite_back_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblVectorTestSuite_* pSelf_ = GBL_VECTOR_TEST_SUITE_(pSelf);
    GBL_TEST_COMPARE(*(const char**)GblVector_back(&pSelf_->stringList.vector),
                     "10");
    GBL_API_END();
}

GBL_RESULT GblVectorTestSuite_insertInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblVectorTestSuite_* pSelf_ = GBL_VECTOR_TEST_SUITE_(pSelf);
    GBL_TEST_EXPECT_ERROR();
    void* pData = GblVector_insert(&pSelf_->stringList.vector,
                                   20,
                                   1,
                                   "lol");
    GBL_TEST_COMPARE(GBL_API_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_TEST_COMPARE(pData, NULL);
    GBL_API_CLEAR_LAST_RECORD();
    GBL_API_END();
}

GBL_RESULT GblVectorTestSuite_insertFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblVectorTestSuite_* pSelf_ = GBL_VECTOR_TEST_SUITE_(pSelf);

    void* pData = GblVector_insert(&pSelf_->stringList.vector,
                                   0,
                                   2,
                                   (const char*[]) {
                                      "-1", "0"
                                   });
    GBL_API_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(*(const char**)pData, "-1");

    GBL_API_VERIFY_CALL(GblVectorTestSuite_verify_(&pSelf_->stringList.vector,
                                                   (const char*[]) {
                                                        "-1", "0", "5", "6", "7", "8", "9", "10", NULL
                                                    },
                                                    pCtx));

    GBL_API_END();
}

GBL_RESULT GblVectorTestSuite_insertBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblVectorTestSuite_* pSelf_ = GBL_VECTOR_TEST_SUITE_(pSelf);

    void* pData = GblVector_insert(&pSelf_->stringList.vector,
                                   8,
                                   1,
                                   (const char*[]) {
                                      "11",
                                   });
    GBL_API_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(*(const char**)pData, "11");

    GBL_API_VERIFY_CALL(GblVectorTestSuite_verify_(&pSelf_->stringList.vector,
                                                   (const char*[]) {
                                                        "-1", "0", "5", "6", "7", "8", "9", "10", "11", NULL
                                                    },
                                                    pCtx));
    GBL_API_END();
}

GBL_RESULT GblVectorTestSuite_insertMiddle_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblVectorTestSuite_* pSelf_ = GBL_VECTOR_TEST_SUITE_(pSelf);

    void* pData = GblVector_insert(&pSelf_->stringList.vector,
                                   2,
                                   3,
                                   (const char*[]) {
                                      "1", "2", "3"
                                   });
    GBL_API_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(*(const char**)pData, "1");

    GBL_API_VERIFY_CALL(GblVectorTestSuite_verify_(&pSelf_->stringList.vector,
                                                   (const char*[]) {
                                                        "-1", "0", "1", "2", "3", "5", "6", "7", "8", "9", "10", "11", NULL
                                                    },
                                                    pCtx));
    GBL_API_END();
}

GBL_RESULT GblVectorTestSuite_pushFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblVectorTestSuite_* pSelf_ = GBL_VECTOR_TEST_SUITE_(pSelf);
    const char* pStr = "-2";
    GBL_API_VERIFY_CALL(GblVector_pushFront(&pSelf_->stringList.vector,
                                            &pStr));
    GBL_API_VERIFY_CALL(GblVectorTestSuite_verify_(&pSelf_->stringList.vector,
                                                   (const char*[]) {
                                                        "-2", "-1", "0", "1", "2", "3", "5", "6", "7", "8", "9", "10", "11", NULL
                                                    },
                                                    pCtx));
    GBL_API_END();
}

GBL_RESULT GblVectorTestSuite_pushBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblVectorTestSuite_* pSelf_ = GBL_VECTOR_TEST_SUITE_(pSelf);
    const char* pStr = "12";
    GBL_API_VERIFY_CALL(GblVector_pushBack(&pSelf_->stringList.vector,
                                           &pStr));
    GBL_API_VERIFY_CALL(GblVectorTestSuite_verify_(&pSelf_->stringList.vector,
                                                   (const char*[]) {
                                                        "-2", "-1", "0", "1", "2", "3", "5", "6", "7", "8", "9", "10", "11", "12", NULL
                                                    },
                                                    pCtx));
    GBL_API_END();
}

GBL_RESULT GblVectorTestSuite_append_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblVectorTestSuite_* pSelf_ = GBL_VECTOR_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblVector_append(&pSelf_->stringList.vector,
                                         (const char*[]) {
                                            "13", "14"
                                         },
                                         2));
    GBL_API_VERIFY_CALL(GblVectorTestSuite_verify_(&pSelf_->stringList.vector,
                                                   (const char*[]) {
                                                        "-2", "-1", "0", "1", "2", "3", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", NULL
                                                    },
                                                    pCtx));
    GBL_API_END();
}

GBL_RESULT GblVectorTestSuite_prepend_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblVectorTestSuite_* pSelf_ = GBL_VECTOR_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblVector_prepend(&pSelf_->stringList.vector,
                                         (const char*[]) {
                                            "-4", "-3"
                                         },
                                         2));
    GBL_API_VERIFY_CALL(GblVectorTestSuite_verify_(&pSelf_->stringList.vector,
                                                   (const char*[]) {
                                                        "-4", "-3", "-2", "-1", "0", "1", "2", "3", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", NULL
                                                    },
                                                    pCtx));
    GBL_API_END();
}

GBL_RESULT GblVectorTestSuite_popFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblVectorTestSuite_* pSelf_ = GBL_VECTOR_TEST_SUITE_(pSelf);
    char* pString = NULL;
    GBL_API_VERIFY_CALL(GblVector_popFront(&pSelf_->stringList.vector,
                                           &pString));
    GBL_TEST_COMPARE(pString, "-4");
    GBL_API_VERIFY_CALL(GblVectorTestSuite_verify_(&pSelf_->stringList.vector,
                                                   (const char*[]) {
                                                        "-3", "-2", "-1", "0", "1", "2", "3", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", NULL
                                                    },
                                                    pCtx));

    GBL_API_END();
}

GBL_RESULT GblVectorTestSuite_popBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblVectorTestSuite_* pSelf_ = GBL_VECTOR_TEST_SUITE_(pSelf);
    char* pString = NULL;
    GBL_API_VERIFY_CALL(GblVector_popBack(&pSelf_->stringList.vector,
                                           &pString));
    GBL_TEST_COMPARE(pString, "14");
    GBL_API_VERIFY_CALL(GblVectorTestSuite_verify_(&pSelf_->stringList.vector,
                                                   (const char*[]) {
                                                        "-3", "-2", "-1", "0", "1", "2", "3", "5", "6", "7", "8", "9", "10", "11", "12", "13", NULL
                                                    },
                                                    pCtx));
    GBL_API_END();
}

GBL_RESULT GblVectorTestSuite_eraseInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblVectorTestSuite_* pSelf_ = GBL_VECTOR_TEST_SUITE_(pSelf);
    GBL_TEST_EXPECT_ERROR();
    GblVector_erase(&pSelf_->stringList.vector,
                    128,
                    0);
    GBL_TEST_COMPARE(GBL_API_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_API_CLEAR_LAST_RECORD();
    GBL_API_END();
}

GBL_RESULT GblVectorTestSuite_eraseFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblVectorTestSuite_* pSelf_ = GBL_VECTOR_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblVector_erase(&pSelf_->stringList.vector,
                                        0,
                                        3));
    GBL_API_VERIFY_CALL(GblVectorTestSuite_verify_(&pSelf_->stringList.vector,
                                                   (const char*[]) {
                                                        "0", "1", "2", "3", "5", "6", "7", "8", "9", "10", "11", "12", "13", NULL
                                                    },
                                                    pCtx));
    GBL_API_END();
}

GBL_RESULT GblVectorTestSuite_eraseBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblVectorTestSuite_* pSelf_ = GBL_VECTOR_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblVector_erase(&pSelf_->stringList.vector,
                                        GblVector_size(&pSelf_->stringList.vector)-1,
                                        1));
    GBL_API_VERIFY_CALL(GblVector_erase(&pSelf_->stringList.vector,
                                        GblVector_size(&pSelf_->stringList.vector)-2,
                                        2));
    GBL_API_VERIFY_CALL(GblVectorTestSuite_verify_(&pSelf_->stringList.vector,
                                                   (const char*[]) {
                                                        "0", "1", "2", "3", "5", "6", "7", "8", "9", "10",  NULL
                                                    },
                                                    pCtx));
    GBL_API_END();
}

GBL_RESULT GblVectorTestSuite_eraseMiddle_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblVectorTestSuite_* pSelf_ = GBL_VECTOR_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblVector_erase(&pSelf_->stringList.vector,
                                        1,
                                        3));
    GBL_API_VERIFY_CALL(GblVectorTestSuite_verify_(&pSelf_->stringList.vector,
                                                   (const char*[]) {
                                                        "0", "5", "6", "7", "8", "9", "10",  NULL
                                                    },
                                                    pCtx));
    GBL_API_END();
}

GBL_RESULT GblVectorTestSuite_clear_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblVectorTestSuite_* pSelf_ = GBL_VECTOR_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblVector_clear(&pSelf_->stringList.vector));
    GBL_TEST_VERIFY(GblVector_empty(&pSelf_->stringList.vector));
    GBL_TEST_COMPARE(GblVector_data(&pSelf_->stringList.vector),
                     pSelf_->stringList.pExtraData);
    GBL_API_END();
}

GBL_RESULT GblVectorTestSuite_reserve_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblVectorTestSuite_* pSelf_ = GBL_VECTOR_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblVector_reserve(&pSelf_->stringList.vector, 1024));
    GBL_TEST_VERIFY(GblVector_empty(&pSelf_->stringList.vector));
    GBL_TEST_VERIFY(GblVector_capacity(&pSelf_->stringList.vector) >=
                    GblVector_elementSize(&pSelf_->stringList.vector) * 1024);
    GBL_TEST_VERIFY(!GblVector_stack(&pSelf_->stringList.vector));
    GBL_API_END();
}

GBL_RESULT GblVectorTestSuite_resize_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblVectorTestSuite_* pSelf_ = GBL_VECTOR_TEST_SUITE_(pSelf);

    GblSize oldCapacity = GblVector_capacity(&pSelf_->stringList.vector);
    GBL_API_VERIFY_CALL(GblVector_resize(&pSelf_->stringList.vector, 2048));
    GBL_TEST_COMPARE(GblVector_size(&pSelf_->stringList.vector), 2048);
    //GBL_TEST_VERIFY(GblVector_capacity(&pSelf_->stringList.vector) > oldCapacity);

    oldCapacity = GblVector_capacity(&pSelf_->stringList.vector);
    GBL_API_VERIFY_CALL(GblVector_resize(&pSelf_->stringList.vector, 32));
    GBL_TEST_COMPARE(GblVector_size(&pSelf_->stringList.vector), 32);
    //GBL_TEST_COMPARE(GblVector_capacity(&pSelf_->stringList.vector), oldCapacity);

    GBL_API_END();
}

GBL_RESULT GblVectorTestSuite_shrinkToFit_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblVectorTestSuite_* pSelf_ = GBL_VECTOR_TEST_SUITE_(pSelf);
    const GblSize oldCapacity = GblVector_capacity(&pSelf_->stringList.vector);
    GBL_API_VERIFY_CALL(GblVector_shrinkToFit(&pSelf_->stringList.vector));
    GBL_TEST_VERIFY(GblVector_capacity(&pSelf_->stringList.vector) < oldCapacity);
    GBL_API_END();
}

GBL_RESULT GblVectorTestSuite_destruct_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblVectorTestSuite_* pSelf_ = GBL_VECTOR_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblVector_destruct(&pSelf_->stringList.vector));
    GBL_API_END();
}

GBL_EXPORT GblType GblVectorTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "constructWithEntries",   GblVectorTestSuite_constructWithEntries_    },
        { "assign",                 GblVectorTestSuite_assign_                  },
        { "release",                GblVectorTestSuite_release_                 },
        { "acquire",                GblVectorTestSuite_acquire_                 },
        { "atInvalid",              GblVectorTestSuite_atInvalid_               },
        { "front",                  GblVectorTestSuite_front_                   },
        { "back",                   GblVectorTestSuite_back_                    },
        { "insertInvalid",          GblVectorTestSuite_insertInvalid_           },
        { "insertFront",            GblVectorTestSuite_insertFront_             },
        { "insertBack",             GblVectorTestSuite_insertBack_              },
        { "insertMiddle",           GblVectorTestSuite_insertMiddle_            },
        { "pushFront",              GblVectorTestSuite_pushFront_               },
        { "pushBack",               GblVectorTestSuite_pushBack_                },
        { "append",                 GblVectorTestSuite_append_                  },
        { "prepend",                GblVectorTestSuite_prepend_                 },
        { "popFront",               GblVectorTestSuite_popFront_                },
        { "popBack",                GblVectorTestSuite_popBack_                 },
        { "eraseInvalid",           GblVectorTestSuite_eraseInvalid_            },
        { "eraseFront",             GblVectorTestSuite_eraseFront_              },
        { "eraseBack",              GblVectorTestSuite_eraseBack_               },
        { "eraseMiddle",            GblVectorTestSuite_eraseMiddle_             },
        { "clear",                  GblVectorTestSuite_clear_                   },
        { "reserve",                GblVectorTestSuite_reserve_                 },
        { "resize",                 GblVectorTestSuite_resize_                  },
        { "shrinkToFit",            GblVectorTestSuite_shrinkToFit_             },
        { "destruct",               GblVectorTestSuite_destruct_                },
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

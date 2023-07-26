#include "containers/gimbal_array_deque_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/core/gimbal_ctx.h>
#include <gimbal/containers/gimbal_array_deque.h>

#define GBL_ARRAY_DEQUE_TEST_SUITE_(inst)     (GBL_PRIVATE(GblArrayDequeTestSuite, inst))

#define GBL_ARRAY_DEQUE_PROFILE_SIZE_     1024

typedef struct GblArrayDequeTestSuite_ {
    GblArrayDeque deques[5];
} GblArrayDequeTestSuite_;

static const char* stringLiterals_[] = {
    "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
    "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"
};

static GBL_RESULT GblArrayDequeTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblArrayDequeTestSuite_* pSelf_ = GBL_ARRAY_DEQUE_TEST_SUITE_(pSelf);
    memset(pSelf_, 0, sizeof(GblArrayDequeTestSuite_));
    GBL_CTX_END();
}

static GBL_RESULT GblArrayDequeTestSuite_verify_(GblContext* pCtx, const GblArrayDeque* pBuffer, ...) {
    va_list varArgs;
    va_start(varArgs, pBuffer);
    GBL_CTX_BEGIN(pCtx);

    size_t  count = 0;
    const char* pStr = NULL;

    while((pStr = va_arg(varArgs, const char*))) {
        GBL_TEST_VERIFY(count < GblArrayDeque_size(pBuffer));

        if(!count) {
            const char** ppFront = GblArrayDeque_front(pBuffer);
            GBL_TEST_VERIFY(ppFront);
            GBL_TEST_COMPARE(*ppFront, pStr);

        }

        const char** ppAt = GblArrayDeque_at(pBuffer, count);
        GBL_TEST_VERIFY(ppAt);
        GBL_TEST_COMPARE(*ppAt, pStr);

        if(++count == GblArrayDeque_size(pBuffer)) {
            const char** ppBack = GblArrayDeque_back(pBuffer);
            GBL_TEST_VERIFY(ppBack);
            GBL_TEST_COMPARE(*ppBack, pStr);

        }
    }

    GBL_TEST_COMPARE(GblArrayDeque_size(pBuffer), count);
    GBL_TEST_COMPARE(GblArrayDeque_empty(pBuffer), count? GBL_FALSE : GBL_TRUE);
    GBL_TEST_COMPARE(GblArrayDeque_full(pBuffer),
                     (GblArrayDeque_size(pBuffer) == GblArrayDeque_capacity(pBuffer)? GBL_TRUE : GBL_FALSE));

    GBL_CTX_END_BLOCK();
    va_end(varArgs);
    return GBL_CTX_RESULT();
}


static GBL_RESULT GblArrayDequeTestSuite_constructEmpty_(GblTestSuite* pSelf, GblContext* pCtx) {
    GblArrayDequeTestSuite_* pSelf_ = GBL_ARRAY_DEQUE_TEST_SUITE_(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_CTX_VERIFY_CALL(GblArrayDeque_construct(&pSelf_->deques[0],
                                                sizeof(const char*),
                                                0, 0, NULL, pCtx));

    GBL_CTX_VERIFY_CALL(GblArrayDequeTestSuite_verify_(pCtx, &pSelf_->deques[0], NULL));

    GBL_TEST_COMPARE(GblArrayDeque_context(&pSelf_->deques[0]), pCtx);
    GBL_TEST_COMPARE(GblArrayDeque_elementSize(&pSelf_->deques[0]), sizeof(const char*));

    GBL_CTX_END();
}

static GBL_RESULT GblArrayDequeTestSuite_constructWithData_(GblTestSuite* pSelf, GblContext* pCtx) {
    GblArrayDequeTestSuite_* pSelf_ = GBL_ARRAY_DEQUE_TEST_SUITE_(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_CTX_VERIFY_CALL(GblArrayDeque_construct(&pSelf_->deques[1],
                                                sizeof(const char*),
                                                0, 1, &stringLiterals_[0], pCtx));

    GBL_CTX_VERIFY_CALL(GblArrayDequeTestSuite_verify_(pCtx, &pSelf_->deques[1], "a", NULL));

    GBL_TEST_VERIFY(GblArrayDeque_full(&pSelf_->deques[1]));
    GBL_TEST_COMPARE(GblArrayDeque_context(&pSelf_->deques[1]), pCtx);
    GBL_TEST_COMPARE(GblArrayDeque_elementSize(&pSelf_->deques[1]), sizeof(const char*));

    GBL_CTX_END();
}

static GBL_RESULT GblArrayDequeTestSuite_copy_(GblTestSuite* pSelf, GblContext* pCtx) {
    GblArrayDequeTestSuite_* pSelf_ = GBL_ARRAY_DEQUE_TEST_SUITE_(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_CTX_VERIFY_CALL(GblArrayDeque_construct(&pSelf_->deques[3],
                                                9999,
                                                0, 0, NULL, pCtx));

    GBL_CTX_VERIFY_CALL(GblArrayDeque_copy(&pSelf_->deques[3], &pSelf_->deques[1]));

    GBL_CTX_VERIFY_CALL(GblArrayDequeTestSuite_verify_(pCtx, &pSelf_->deques[3], "a", NULL));

    GBL_TEST_VERIFY(GblArrayDeque_full(&pSelf_->deques[3]));
    GBL_TEST_COMPARE(GblArrayDeque_context(&pSelf_->deques[3]), pCtx);
    GBL_TEST_COMPARE(GblArrayDeque_elementSize(&pSelf_->deques[3]), sizeof(const char*));

    GBL_CTX_END();
}

static GBL_RESULT GblArrayDequeTestSuite_move_(GblTestSuite* pSelf, GblContext* pCtx) {
    GblArrayDequeTestSuite_* pSelf_ = GBL_ARRAY_DEQUE_TEST_SUITE_(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_CTX_VERIFY_CALL(GblArrayDeque_construct(&pSelf_->deques[4],
                                                33,
                                                0, 0, NULL, pCtx));

    GBL_CTX_VERIFY_CALL(GblArrayDeque_move(&pSelf_->deques[4], &pSelf_->deques[3]));

    GBL_CTX_VERIFY_CALL(GblArrayDequeTestSuite_verify_(pCtx, &pSelf_->deques[4], "a", NULL));

    GBL_TEST_VERIFY(GblArrayDeque_full(&pSelf_->deques[4]));
    GBL_TEST_COMPARE(GblArrayDeque_context(&pSelf_->deques[4]), pCtx);
    GBL_TEST_COMPARE(GblArrayDeque_elementSize(&pSelf_->deques[4]), sizeof(const char*));

    GBL_CTX_VERIFY_CALL(GblArrayDequeTestSuite_verify_(pCtx, &pSelf_->deques[3], NULL));
    GBL_TEST_COMPARE(GblArrayDeque_capacity(&pSelf_->deques[3]), 0);

    GBL_CTX_END();
}

static GBL_RESULT GblArrayDequeTestSuite_atInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GblArrayDequeTestSuite_* pSelf_ = GBL_ARRAY_DEQUE_TEST_SUITE_(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_TEST_EXPECT_ERROR();

    void* pData = GblArrayDeque_at(&pSelf_->deques[1], 1);
    GBL_TEST_VERIFY(!pData);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);

    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}


static GBL_RESULT GblArrayDequeTestSuite_pushBackResize_(GblTestSuite* pSelf, GblContext* pCtx) {
    GblArrayDequeTestSuite_* pSelf_ = GBL_ARRAY_DEQUE_TEST_SUITE_(pSelf);
    GBL_CTX_BEGIN(pCtx);

    for(size_t  i = 0; i < 10; ++i) {
        GBL_CTX_VERIFY_CALL(GblArrayDeque_pushBack(&pSelf_->deques[0], &stringLiterals_[10+i]));
    }

    GBL_CTX_VERIFY_CALL(GblArrayDequeTestSuite_verify_(pCtx, &pSelf_->deques[0],
                        "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", NULL));

    GBL_TEST_COMPARE(GblArrayDeque_capacity(&pSelf_->deques[0]), 16);

    GBL_CTX_END();
}

static GBL_RESULT GblArrayDequeTestSuite_emplaceBackResize_(GblTestSuite* pSelf, GblContext* pCtx) {
    GblArrayDequeTestSuite_* pSelf_ = GBL_ARRAY_DEQUE_TEST_SUITE_(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_CTX_VERIFY_CALL(GblArrayDeque_construct(&pSelf_->deques[2],
                                                sizeof(const char*),
                                                0, 0, NULL, pCtx));

    for(size_t  i = 0; i < 10; ++i) {
        const char** pSlot = GblArrayDeque_emplaceBack(&pSelf_->deques[2]);
        GBL_TEST_VERIFY(pSlot);
        *pSlot = stringLiterals_[10+i];
    }

    GBL_CTX_VERIFY_CALL(GblArrayDequeTestSuite_verify_(pCtx, &pSelf_->deques[2],
                        "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", NULL));

    GBL_TEST_COMPARE(GblArrayDeque_capacity(&pSelf_->deques[2]), 16);

    GBL_CTX_END();
}

static GBL_RESULT GblArrayDequeTestSuite_pushFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GblArrayDequeTestSuite_* pSelf_ = GBL_ARRAY_DEQUE_TEST_SUITE_(pSelf);
    GBL_CTX_BEGIN(pCtx);

    for(size_t  i = 0; i < 6; ++i) {
        GBL_CTX_VERIFY_CALL(GblArrayDeque_pushFront(&pSelf_->deques[0], &stringLiterals_[9-i]));
    }

    GBL_CTX_VERIFY_CALL(GblArrayDequeTestSuite_verify_(pCtx, &pSelf_->deques[0],
                        "e", "f", "g", "h", "i", "j",
                        "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", NULL));

    GBL_TEST_COMPARE(GblArrayDeque_capacity(&pSelf_->deques[0]), 16);

    GBL_CTX_END();
}

static GBL_RESULT GblArrayDequeTestSuite_pushFrontResize_(GblTestSuite* pSelf, GblContext* pCtx) {
    GblArrayDequeTestSuite_* pSelf_ = GBL_ARRAY_DEQUE_TEST_SUITE_(pSelf);
    GBL_CTX_BEGIN(pCtx);

    for(size_t  i = 0; i < 4; ++i) {
        GBL_CTX_VERIFY_CALL(GblArrayDeque_pushFront(&pSelf_->deques[0], &stringLiterals_[3-i]));
    }

    GBL_CTX_VERIFY_CALL(GblArrayDequeTestSuite_verify_(pCtx, &pSelf_->deques[0],
                        "a", "b", "c", "d", "e", "f", "g", "h", "i", "j",
                        "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", NULL));

    GBL_TEST_COMPARE(GblArrayDeque_capacity(&pSelf_->deques[0]), 32);

    GBL_CTX_END();
}

static GBL_RESULT GblArrayDequeTestSuite_emplaceFrontResize_(GblTestSuite* pSelf, GblContext* pCtx) {
    GblArrayDequeTestSuite_* pSelf_ = GBL_ARRAY_DEQUE_TEST_SUITE_(pSelf);
    GBL_CTX_BEGIN(pCtx);

    for(size_t  i = 0; i < 10; ++i) {
        const char** pSlot = GblArrayDeque_emplaceFront(&pSelf_->deques[2]);
        GBL_TEST_VERIFY(pSlot);
        *pSlot = stringLiterals_[9-i];
    }

    GBL_CTX_VERIFY_CALL(GblArrayDequeTestSuite_verify_(pCtx, &pSelf_->deques[2],
                        "a", "b", "c", "d", "e", "f", "g", "h", "i", "j",
                        "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", NULL));

    GBL_TEST_COMPARE(GblArrayDeque_capacity(&pSelf_->deques[2]), 32);

    GBL_CTX_END();
}

static GBL_RESULT GblArrayDequeTestSuite_popBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GblArrayDequeTestSuite_* pSelf_ = GBL_ARRAY_DEQUE_TEST_SUITE_(pSelf);
    GBL_CTX_BEGIN(pCtx);

    for(size_t  i = 0; i < 5; ++i) {
        const char ** ppStr = GblArrayDeque_popBack(&pSelf_->deques[0]);
        GBL_TEST_COMPARE(*ppStr, stringLiterals_[19-i]);
    }

    GBL_CTX_VERIFY_CALL(GblArrayDequeTestSuite_verify_(pCtx, &pSelf_->deques[0],
                        "a", "b", "c", "d", "e", "f", "g", "h", "i", "j",
                        "k", "l", "m", "n", "o",  NULL));
    GBL_CTX_END();
}

static GBL_RESULT GblArrayDequeTestSuite_popBackInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GblArrayDequeTestSuite_* pSelf_ = GBL_ARRAY_DEQUE_TEST_SUITE_(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_TEST_EXPECT_ERROR();

    GblArrayDeque_popBack(&pSelf_->deques[1]);

    void* pPtr = GblArrayDeque_popBack(&pSelf_->deques[1]);
    GBL_TEST_VERIFY(!pPtr);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblArrayDequeTestSuite_popFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GblArrayDequeTestSuite_* pSelf_ = GBL_ARRAY_DEQUE_TEST_SUITE_(pSelf);
    GBL_CTX_BEGIN(pCtx);

    for(size_t  i = 0; i < 5; ++i) {
        const char ** ppStr = GblArrayDeque_popFront(&pSelf_->deques[0]);
        GBL_TEST_COMPARE(*ppStr, stringLiterals_[i]);
    }

    GBL_CTX_VERIFY_CALL(GblArrayDequeTestSuite_verify_(pCtx, &pSelf_->deques[0],
                        "f", "g", "h", "i", "j",
                        "k", "l", "m", "n", "o",  NULL));
    GBL_CTX_END();
}

static GBL_RESULT GblArrayDequeTestSuite_popFrontInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GblArrayDequeTestSuite_* pSelf_ = GBL_ARRAY_DEQUE_TEST_SUITE_(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_TEST_EXPECT_ERROR();
    void* pPtr = GblArrayDeque_popFront(&pSelf_->deques[1]);
    GBL_TEST_VERIFY(!pPtr);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblArrayDequeTestSuite_resizeShrink_(GblTestSuite* pSelf, GblContext* pCtx) {
    GblArrayDequeTestSuite_* pSelf_ = GBL_ARRAY_DEQUE_TEST_SUITE_(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_CTX_VERIFY_CALL(GblArrayDeque_resize(&pSelf_->deques[0], 5));

    GBL_CTX_VERIFY_CALL(GblArrayDequeTestSuite_verify_(pCtx, &pSelf_->deques[0],
                        "f", "g", "h", "i", "j", NULL));

    GBL_TEST_COMPARE(GblArrayDeque_capacity(&pSelf_->deques[0]), 32);

    GBL_CTX_END();
}

static GBL_RESULT GblArrayDequeTestSuite_resizeGrow_(GblTestSuite* pSelf, GblContext* pCtx) {
    GblArrayDequeTestSuite_* pSelf_ = GBL_ARRAY_DEQUE_TEST_SUITE_(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_CTX_VERIFY_CALL(GblArrayDeque_resize(&pSelf_->deques[0], 10));

    GBL_CTX_VERIFY_CALL(GblArrayDequeTestSuite_verify_(pCtx, &pSelf_->deques[0],
                        "f", "g", "h", "i", "j",
                        "k", "l", "m", "n", "o",  NULL));

    GBL_TEST_COMPARE(GblArrayDeque_capacity(&pSelf_->deques[0]), 32);

    GBL_CTX_END();
}

static GBL_RESULT GblArrayDequeTestSuite_reserveIgnored_(GblTestSuite* pSelf, GblContext* pCtx) {
    GblArrayDequeTestSuite_* pSelf_ = GBL_ARRAY_DEQUE_TEST_SUITE_(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_CTX_VERIFY_CALL(GblArrayDeque_reserve(&pSelf_->deques[0], 16));

    GBL_CTX_VERIFY_CALL(GblArrayDequeTestSuite_verify_(pCtx, &pSelf_->deques[0],
                        "f", "g", "h", "i", "j",
                        "k", "l", "m", "n", "o",  NULL));

    GBL_TEST_COMPARE(GblArrayDeque_capacity(&pSelf_->deques[0]), 32);

    GBL_CTX_END();
}

static GBL_RESULT GblArrayDequeTestSuite_reserveGrow_(GblTestSuite* pSelf, GblContext* pCtx) {
    GblArrayDequeTestSuite_* pSelf_ = GBL_ARRAY_DEQUE_TEST_SUITE_(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_CTX_VERIFY_CALL(GblArrayDeque_reserve(&pSelf_->deques[0], 35));

    GBL_CTX_VERIFY_CALL(GblArrayDequeTestSuite_verify_(pCtx, &pSelf_->deques[0],
                        "f", "g", "h", "i", "j",
                        "k", "l", "m", "n", "o",  NULL));

#if GBL_ARRAY_DEQUE_FORCE_POW2 == 1
    GBL_TEST_COMPARE(GblArrayDeque_capacity(&pSelf_->deques[0]), 64);
#else
    GBL_TEST_COMPARE(GblArrayDeque_capacity(&pSelf_->deques[0]), 35);
#endif
    GBL_CTX_END();
}

static GBL_RESULT GblArrayDequeTestSuite_shrinkToFitShrink_(GblTestSuite* pSelf, GblContext* pCtx) {
    GblArrayDequeTestSuite_* pSelf_ = GBL_ARRAY_DEQUE_TEST_SUITE_(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_CTX_VERIFY_CALL(GblArrayDeque_shrinkToFit(&pSelf_->deques[0]));

    GBL_CTX_VERIFY_CALL(GblArrayDequeTestSuite_verify_(pCtx, &pSelf_->deques[0],
                        "f", "g", "h", "i", "j",
                        "k", "l", "m", "n", "o",  NULL));

#if GBL_ARRAY_DEQUE_FORCE_POW2 == 1
    GBL_TEST_COMPARE(GblArrayDeque_capacity(&pSelf_->deques[0]), 16);
#else
    GBL_TEST_COMPARE(GblArrayDeque_capacity(&pSelf_->deques[0]), 10);
#endif

    GBL_CTX_END();
}

static GBL_RESULT GblArrayDequeTestSuite_shrinkToFitIgnored_(GblTestSuite* pSelf, GblContext* pCtx) {
    GblArrayDequeTestSuite_* pSelf_ = GBL_ARRAY_DEQUE_TEST_SUITE_(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_CTX_VERIFY_CALL(GblArrayDeque_shrinkToFit(&pSelf_->deques[0]));

    GBL_CTX_VERIFY_CALL(GblArrayDequeTestSuite_verify_(pCtx, &pSelf_->deques[0],
                        "f", "g", "h", "i", "j",
                        "k", "l", "m", "n", "o",  NULL));

#if GBL_ARRAY_DEQUE_FORCE_POW2 == 1
    GBL_TEST_COMPARE(GblArrayDeque_capacity(&pSelf_->deques[0]), 16);
#else
    GBL_TEST_COMPARE(GblArrayDeque_capacity(&pSelf_->deques[0]), 10);
#endif


    GBL_CTX_END();
}

static GBL_RESULT GblArrayDequeTestSuite_insertInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GblArrayDequeTestSuite_* pSelf_ = GBL_ARRAY_DEQUE_TEST_SUITE_(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_TEST_EXPECT_ERROR();

    const char** ppSlot = GblArrayDeque_insert(&pSelf_->deques[0],
                                               11,
                                               &stringLiterals_[0],
                                               3);

    GBL_TEST_VERIFY(!ppSlot);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}


static GBL_RESULT GblArrayDequeTestSuite_insertFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GblArrayDequeTestSuite_* pSelf_ = GBL_ARRAY_DEQUE_TEST_SUITE_(pSelf);
    GBL_CTX_BEGIN(pCtx);

    const char** ppSlot = GblArrayDeque_insert(&pSelf_->deques[0],
                                               0,
                                               &stringLiterals_[0],
                                               3);

    GBL_TEST_COMPARE(*ppSlot, "a");

    GBL_CTX_VERIFY_CALL(GblArrayDequeTestSuite_verify_(pCtx, &pSelf_->deques[0],
                        "a", "b", "c", "f", "g", "h", "i", "j",
                        "k", "l", "m", "n", "o",  NULL));

    GBL_CTX_END();
}


static GBL_RESULT GblArrayDequeTestSuite_insertBackEmpty_(GblTestSuite* pSelf, GblContext* pCtx) {
    GblArrayDequeTestSuite_* pSelf_ = GBL_ARRAY_DEQUE_TEST_SUITE_(pSelf);
    GBL_CTX_BEGIN(pCtx);

    const char** ppSlot = GblArrayDeque_insert(&pSelf_->deques[3],
                                               0,
                                               &stringLiterals_[0],
                                               3);

    GBL_TEST_COMPARE(*ppSlot, "a");

    GBL_CTX_VERIFY_CALL(GblArrayDequeTestSuite_verify_(pCtx, &pSelf_->deques[3],
                        "a", "b", "c", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblArrayDequeTestSuite_insertBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GblArrayDequeTestSuite_* pSelf_ = GBL_ARRAY_DEQUE_TEST_SUITE_(pSelf);
    GBL_CTX_BEGIN(pCtx);

    const char** ppSlot = GblArrayDeque_insert(&pSelf_->deques[0],
                                               GblArrayDeque_size(&pSelf_->deques[0]),
                                               &stringLiterals_[15],
                                               5);

    GBL_TEST_COMPARE(*ppSlot, "p");

    GBL_CTX_VERIFY_CALL(GblArrayDequeTestSuite_verify_(pCtx, &pSelf_->deques[0],
                        "a", "b", "c", "f", "g", "h", "i", "j",
                        "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblArrayDequeTestSuite_insertMiddleFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GblArrayDequeTestSuite_* pSelf_ = GBL_ARRAY_DEQUE_TEST_SUITE_(pSelf);
    GBL_CTX_BEGIN(pCtx);

    const char** ppSlot = GblArrayDeque_insert(&pSelf_->deques[0],
                                               3,
                                               &stringLiterals_[3],
                                               2);

    GBL_TEST_COMPARE(*ppSlot, "d");

    GBL_CTX_VERIFY_CALL(GblArrayDequeTestSuite_verify_(pCtx, &pSelf_->deques[0],
                        "a", "b", "c", "d", "e", "f", "g", "h", "i", "j",
                        "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblArrayDequeTestSuite_insertMiddleEnd_(GblTestSuite* pSelf, GblContext* pCtx) {
    GblArrayDequeTestSuite_* pSelf_ = GBL_ARRAY_DEQUE_TEST_SUITE_(pSelf);
    GBL_CTX_BEGIN(pCtx);

    const char** ppSlot = GblArrayDeque_insert(&pSelf_->deques[0],
                                               16,
                                               &stringLiterals_[0],
                                               13);

    GBL_TEST_COMPARE(*ppSlot, "a");

    GBL_CTX_VERIFY_CALL(GblArrayDequeTestSuite_verify_(pCtx, &pSelf_->deques[0],
                        "a", "b", "c", "d", "e", "f", "g", "h", "i", "j",
                        "k", "l", "m", "n", "o", "p",
                        "a", "b", "c", "d", "e", "f", "g", "h", "i", "j",
                        "k", "l", "m",
                        "q", "r", "s", "t", NULL));

    GBL_CTX_END();
}


static GBL_RESULT GblArrayDequeTestSuite_clear_(GblTestSuite* pSelf, GblContext* pCtx) {
    GblArrayDequeTestSuite_* pSelf_ = GBL_ARRAY_DEQUE_TEST_SUITE_(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GblArrayDeque_clear(&pSelf_->deques[0]);

    GBL_CTX_VERIFY_CALL(GblArrayDequeTestSuite_verify_(pCtx, &pSelf_->deques[0], NULL));

    GBL_TEST_COMPARE(GblArrayDeque_size(&pSelf_->deques[0]), 0);
    GBL_TEST_COMPARE(GblArrayDeque_capacity(&pSelf_->deques[0]), 0);

    GBL_CTX_END();
}

static GBL_RESULT GblArrayDequeTestSuite_atProfile_(GblTestSuite* pSelf, GblContext* pCtx) {
    GblArrayDequeTestSuite_* pSelf_ = GBL_ARRAY_DEQUE_TEST_SUITE_(pSelf);
    GBL_CTX_BEGIN(pCtx);

    char buffer[] = "LOLOL";
    char* pLiteral = buffer;

    GblArrayDeque_clear(&pSelf_->deques[0]);
    GblArrayDeque_pushBack(&pSelf_->deques[0], &pLiteral);


    for(size_t  i = 0; i < GBL_ARRAY_DEQUE_PROFILE_SIZE_; ++i) {
        pLiteral = GblArrayDeque_at(&pSelf_->deques[0],
                                    gblRandRange(0, GblArrayDeque_size(&pSelf_->deques[0])-1));
        pLiteral[0] = 'l';
    }

    GBL_CTX_END();
}

static GBL_RESULT GblArrayDequeTestSuite_pushBackProfile_(GblTestSuite* pSelf, GblContext* pCtx) {
    GblArrayDequeTestSuite_* pSelf_ = GBL_ARRAY_DEQUE_TEST_SUITE_(pSelf);
    GBL_CTX_BEGIN(pCtx);

    const char* pLiteral = "LOLOL";

    GblArrayDeque_clear(&pSelf_->deques[0]);

    for(size_t  i = 0; i < GBL_ARRAY_DEQUE_PROFILE_SIZE_; ++i) {
        GblArrayDeque_pushBack(&pSelf_->deques[0], &pLiteral);
    }

    GBL_CTX_END();
}

static GBL_RESULT GblArrayDequeTestSuite_pushFrontProfile_(GblTestSuite* pSelf, GblContext* pCtx) {
    GblArrayDequeTestSuite_* pSelf_ = GBL_ARRAY_DEQUE_TEST_SUITE_(pSelf);
    GBL_CTX_BEGIN(pCtx);

    const char* pLiteral = "LOLOL";

    GblArrayDeque_clear(&pSelf_->deques[0]);

    for(size_t  i = 0; i < GBL_ARRAY_DEQUE_PROFILE_SIZE_; ++i) {
        GblArrayDeque_pushFront(&pSelf_->deques[0], &pLiteral);
    }

    GBL_CTX_END();
}


static GBL_RESULT GblArrayDequeTestSuite_destruct_(GblTestSuite* pSelf, GblContext* pCtx) {
    GblArrayDequeTestSuite_* pSelf_ = GBL_ARRAY_DEQUE_TEST_SUITE_(pSelf);
    GBL_CTX_BEGIN(pCtx);

    for(size_t  i = 0; i < 5; ++i)
        GBL_CTX_VERIFY_CALL(GblArrayDeque_destruct(&pSelf_->deques[i]));

    GBL_CTX_END();
}

GBL_EXPORT GblType GblArrayDequeTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "constructEmpty",     GblArrayDequeTestSuite_constructEmpty_      },
        { "constructWithData",  GblArrayDequeTestSuite_constructWithData_   },
        { "copy",               GblArrayDequeTestSuite_copy_                },
        { "move",               GblArrayDequeTestSuite_move_                },
        { "atInvalid",          GblArrayDequeTestSuite_atInvalid_           },
        { "pushBackResize",     GblArrayDequeTestSuite_pushBackResize_      },
        { "emplaceBackResize",  GblArrayDequeTestSuite_emplaceBackResize_   },
        { "pushFront",          GblArrayDequeTestSuite_pushFront_           },
        { "pushFrontResize",    GblArrayDequeTestSuite_pushFrontResize_     },
        { "emplaceFrontResize", GblArrayDequeTestSuite_emplaceFrontResize_  },
        { "popBack",            GblArrayDequeTestSuite_popBack_             },
        { "popBackInvalid",     GblArrayDequeTestSuite_popBackInvalid_      },
        { "popFront",           GblArrayDequeTestSuite_popFront_            },
        { "popFrontInvalid",    GblArrayDequeTestSuite_popFrontInvalid_     },
        { "resizeShrink",       GblArrayDequeTestSuite_resizeShrink_        },
        { "resizeGrow",         GblArrayDequeTestSuite_resizeGrow_          },
        { "reserveIgnored",     GblArrayDequeTestSuite_reserveIgnored_      },
        { "reserveGrow",        GblArrayDequeTestSuite_reserveGrow_         },
        { "shrinkToFitShrink",  GblArrayDequeTestSuite_shrinkToFitShrink_   },
        { "shrinkToFitIgnored", GblArrayDequeTestSuite_shrinkToFitIgnored_  },
        { "insertInvalid",      GblArrayDequeTestSuite_insertInvalid_       },
        { "insertFront",        GblArrayDequeTestSuite_insertFront_         },
        { "insertBackEmpty",    GblArrayDequeTestSuite_insertBackEmpty_     },
        { "insertBack",         GblArrayDequeTestSuite_insertBack_          },
        { "insertMiddleFront",  GblArrayDequeTestSuite_insertMiddleFront_   },
        { "insertMiddleEnd",    GblArrayDequeTestSuite_insertMiddleEnd_     },
        { "clear",              GblArrayDequeTestSuite_clear_               },
        { "atProfile",          GblArrayDequeTestSuite_atProfile_           },
        { "pushBackProfile",    GblArrayDequeTestSuite_pushBackProfile_     },
        { "pushFrontProfile",   GblArrayDequeTestSuite_pushFrontProfile_    },
        { "destruct",           GblArrayDequeTestSuite_destruct_            },
        { NULL,                 NULL                                        }
    };

    const static GblTestSuiteVTable vTable = {
        .pFnSuiteInit   = GblArrayDequeTestSuite_init_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("GblArrayDequeTestSuite"),
                                     &vTable,
                                     sizeof(GblArrayDequeTestSuite),
                                     sizeof(GblArrayDequeTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }

    return type;
}

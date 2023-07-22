#include "containers/gimbal_ring_buffer_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/core/gimbal_ctx.h>
#include <gimbal/containers/gimbal_ring_buffer.h>

#define GBL_RING_BUFFER_TEST_SUITE_(inst)     ((GblRingBufferTestSuite_*)GBL_INSTANCE_PRIVATE(inst, GBL_RING_BUFFER_TEST_SUITE_TYPE))

typedef struct GblRingBufferTestSuite_ {
    GblRingBuffer ringBuffer[4];
} GblRingBufferTestSuite_;

static const char* stringLiterals_[] = {
    "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
    "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"
};

static GBL_RESULT GblRingBufferTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblRingBufferTestSuite_* pSelf_ = GBL_RING_BUFFER_TEST_SUITE_(pSelf);
    memset(pSelf_, 0, sizeof(GblRingBufferTestSuite_));
    GBL_CTX_END();
}

static GBL_RESULT GblRingBufferTestSuite_verify_(GblContext* pCtx, const GblRingBuffer* pBuffer, ...) {
    va_list varArgs;
    va_start(varArgs, pBuffer);
    GBL_CTX_BEGIN(pCtx);

    size_t  count = 0;
    const char* pStr = NULL;

    while((pStr = va_arg(varArgs, const char*))) {
        GBL_TEST_VERIFY(count < GblRingBuffer_size(pBuffer));

        if(!count) {
            const char** ppFront = GblRingBuffer_front(pBuffer);
            GBL_TEST_VERIFY(ppFront);
            GBL_TEST_COMPARE(*ppFront, pStr);

        }

        const char** ppAt = GblRingBuffer_at(pBuffer, count);
        GBL_TEST_VERIFY(ppAt);
        GBL_TEST_COMPARE(*ppAt, pStr);

        if(++count == GblRingBuffer_size(pBuffer)) {
            const char** ppBack = GblRingBuffer_back(pBuffer);
            GBL_TEST_VERIFY(ppBack);
            GBL_TEST_COMPARE(*ppBack, pStr);

        }
    }

    GBL_TEST_COMPARE(GblRingBuffer_size(pBuffer), count);
    GBL_TEST_COMPARE(GblRingBuffer_empty(pBuffer), count? GBL_FALSE : GBL_TRUE);
    GBL_TEST_COMPARE(GblRingBuffer_full(pBuffer),
                     (GblRingBuffer_size(pBuffer) == GblRingBuffer_capacity(pBuffer)? GBL_TRUE : GBL_FALSE));

    GBL_CTX_END_BLOCK();
    va_end(varArgs);
    return GBL_CTX_RESULT();
}

static GBL_RESULT GblRingBufferTestSuite_constructEmpty_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingBufferTestSuite_* pSelf_ = GBL_RING_BUFFER_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblRingBuffer_construct(&pSelf_->ringBuffer[0],
                                                sizeof(const char*),
                                                10));
    GBL_CTX_VERIFY_CALL(GblRingBufferTestSuite_verify_(pCtx, &pSelf_->ringBuffer[0], NULL));


    GBL_CTX_END();
}

static GBL_RESULT GblRingBufferTestSuite_constructValues_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingBufferTestSuite_* pSelf_ = GBL_RING_BUFFER_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblRingBuffer_construct(&pSelf_->ringBuffer[1],
                                                sizeof(const char*),
                                                10,
                                                3,
                                                stringLiterals_,
                                                pCtx));

    GBL_CTX_VERIFY_CALL(GblRingBufferTestSuite_verify_(pCtx, &pSelf_->ringBuffer[1], "a", "b", "c", NULL));
    GBL_TEST_COMPARE(GblRingBuffer_context(&pSelf_->ringBuffer[1]), pCtx);
    GBL_TEST_COMPARE(GblRingBuffer_capacity(&pSelf_->ringBuffer[1]), 10);
    GBL_TEST_COMPARE(GblRingBuffer_elementSize(&pSelf_->ringBuffer[1]), sizeof(const char*));

    GBL_CTX_END();
}

static GBL_RESULT GblRingBufferTestSuite_copy_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingBufferTestSuite_* pSelf_ = GBL_RING_BUFFER_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblRingBuffer_construct(&pSelf_->ringBuffer[2],
                                                sizeof(GblTestSuite),
                                                4,
                                                0,
                                                NULL,
                                                pCtx));

    GBL_CTX_VERIFY_CALL(GblRingBuffer_copy(&pSelf_->ringBuffer[2],
                                           &pSelf_->ringBuffer[1]));

    GBL_CTX_VERIFY_CALL(GblRingBufferTestSuite_verify_(pCtx, &pSelf_->ringBuffer[2], "a", "b", "c", NULL));
    GBL_TEST_COMPARE(GblRingBuffer_context(&pSelf_->ringBuffer[2]), pCtx);
    GBL_TEST_COMPARE(GblRingBuffer_capacity(&pSelf_->ringBuffer[2]), 10);
    GBL_TEST_COMPARE(GblRingBuffer_elementSize(&pSelf_->ringBuffer[2]), sizeof(const char*));

    GBL_CTX_END();
}

static GBL_RESULT GblRingBufferTestSuite_move_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingBufferTestSuite_* pSelf_ = GBL_RING_BUFFER_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblRingBuffer_construct(&pSelf_->ringBuffer[3],
                                                sizeof(GblContext*),
                                                4,
                                                0,
                                                NULL,
                                                pCtx));

    GBL_CTX_VERIFY_CALL(GblRingBuffer_move(&pSelf_->ringBuffer[3],
                                           &pSelf_->ringBuffer[2]));

    GBL_CTX_VERIFY_CALL(GblRingBufferTestSuite_verify_(pCtx, &pSelf_->ringBuffer[2], NULL));

    GBL_CTX_VERIFY_CALL(GblRingBufferTestSuite_verify_(pCtx, &pSelf_->ringBuffer[3], "a", "b", "c", NULL));
    GBL_TEST_COMPARE(GblRingBuffer_context(&pSelf_->ringBuffer[3]), pCtx);
    GBL_TEST_COMPARE(GblRingBuffer_capacity(&pSelf_->ringBuffer[3]), 10);
    GBL_TEST_COMPARE(GblRingBuffer_elementSize(&pSelf_->ringBuffer[3]), sizeof(const char*));

    GBL_CTX_END();
}

static GBL_RESULT GblRingBufferTestSuite_atInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblRingBufferTestSuite_* pSelf_ = GBL_RING_BUFFER_TEST_SUITE_(pSelf);

    GBL_TEST_EXPECT_ERROR();

    const void* pData = GblRingBuffer_at(&pSelf_->ringBuffer[0], 0);
    GBL_TEST_VERIFY(!pData);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    pData = GblRingBuffer_at(&pSelf_->ringBuffer[1], 10);
    GBL_TEST_VERIFY(!pData);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}


static GBL_RESULT GblRingBufferTestSuite_frontInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblRingBufferTestSuite_* pSelf_ = GBL_RING_BUFFER_TEST_SUITE_(pSelf);

    GBL_TEST_EXPECT_ERROR();

    const void* pData = GblRingBuffer_front(&pSelf_->ringBuffer[0]);
    GBL_TEST_VERIFY(!pData);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblRingBufferTestSuite_backInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblRingBufferTestSuite_* pSelf_ = GBL_RING_BUFFER_TEST_SUITE_(pSelf);

    GBL_TEST_EXPECT_ERROR();

    const void* pData = GblRingBuffer_back(&pSelf_->ringBuffer[0]);
    GBL_TEST_VERIFY(!pData);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblRingBufferTestSuite_pushBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingBufferTestSuite_* pSelf_ = GBL_RING_BUFFER_TEST_SUITE_(pSelf);

    for(size_t  o = 3; o <= 9; ++o)
        GBL_CTX_VERIFY_CALL(GblRingBuffer_pushBack(&pSelf_->ringBuffer[1], &stringLiterals_[o]));

    GBL_CTX_VERIFY_CALL(GblRingBufferTestSuite_verify_(pCtx,
                                                       &pSelf_->ringBuffer[1],
                                                       "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", NULL));

    GBL_CTX_END();
}


static GBL_RESULT GblRingBufferTestSuite_pushBackOverflow_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblRingBufferTestSuite_* pSelf_ = GBL_RING_BUFFER_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblRingBuffer_pushBack(&pSelf_->ringBuffer[1], &stringLiterals_[10]));

    GBL_CTX_VERIFY_CALL(GblRingBufferTestSuite_verify_(pCtx,
                                                       &pSelf_->ringBuffer[1],
                                                       "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", NULL));

    GBL_CTX_VERIFY_CALL(GblRingBuffer_pushBack(&pSelf_->ringBuffer[1], &stringLiterals_[11]));

    GBL_CTX_VERIFY_CALL(GblRingBufferTestSuite_verify_(pCtx,
                                                       &pSelf_->ringBuffer[1],
                                                       "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblRingBufferTestSuite_emplaceBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblRingBufferTestSuite_* pSelf_ = GBL_RING_BUFFER_TEST_SUITE_(pSelf);

    for(size_t  o = 0; o < 5; ++o) {
        const char** ppData = GblRingBuffer_emplaceBack(&pSelf_->ringBuffer[0]);
        GBL_TEST_VERIFY(ppData);
        *ppData = stringLiterals_[o];
    }

    GBL_CTX_VERIFY_CALL(GblRingBufferTestSuite_verify_(pCtx,
                                                       &pSelf_->ringBuffer[0],
                                                       "a", "b", "c", "d", "e", NULL));
    GBL_CTX_END();
}

static GBL_RESULT GblRingBufferTestSuite_emplaceBackOverflow_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblRingBufferTestSuite_* pSelf_ = GBL_RING_BUFFER_TEST_SUITE_(pSelf);

    for(size_t  o = 5; o < 12; ++o) {
        const char** ppData = GblRingBuffer_emplaceBack(&pSelf_->ringBuffer[0]);
        GBL_TEST_VERIFY(ppData);
        *ppData = stringLiterals_[o];
    }

    GBL_CTX_VERIFY_CALL(GblRingBufferTestSuite_verify_(pCtx,
                                                       &pSelf_->ringBuffer[0],
                                                       "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblRingBufferTestSuite_popFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblRingBufferTestSuite_* pSelf_ = GBL_RING_BUFFER_TEST_SUITE_(pSelf);

    for(size_t  o = 0; o < 5; ++o) {
        const char** ppData = GblRingBuffer_popFront(&pSelf_->ringBuffer[0]);
        GBL_TEST_VERIFY(ppData);
        GBL_TEST_COMPARE(*ppData, stringLiterals_[o+2]);
    }

    GBL_CTX_VERIFY_CALL(GblRingBufferTestSuite_verify_(pCtx,
                                                       &pSelf_->ringBuffer[0],
                                                       "h", "i", "j", "k", "l", NULL));

    for(size_t  o = 0; o < 5; ++o) {
        const char** ppData = GblRingBuffer_popFront(&pSelf_->ringBuffer[0]);
        GBL_TEST_VERIFY(ppData);
        GBL_TEST_COMPARE(*ppData, stringLiterals_[o+7]);
    }

    GBL_CTX_VERIFY_CALL(GblRingBufferTestSuite_verify_(pCtx,
                                                       &pSelf_->ringBuffer[0],
                                                       NULL));
    GBL_CTX_END();
}

static GBL_RESULT GblRingBufferTestSuite_popFrontInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblRingBufferTestSuite_* pSelf_ = GBL_RING_BUFFER_TEST_SUITE_(pSelf);

    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblRingBuffer_popFront(&pSelf_->ringBuffer[0]), NULL);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblRingBufferTestSuite_clear_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblRingBufferTestSuite_* pSelf_ = GBL_RING_BUFFER_TEST_SUITE_(pSelf);

    GblRingBuffer_clear(&pSelf_->ringBuffer[1]);

    GBL_CTX_VERIFY_CALL(GblRingBufferTestSuite_verify_(pCtx,
                                                       &pSelf_->ringBuffer[1],
                                                       NULL));

    GBL_CTX_END();
}

static GBL_RESULT GblRingBufferTestSuite_destruct_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblRingBufferTestSuite_* pSelf_ = GBL_RING_BUFFER_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblRingBuffer_destruct(&pSelf_->ringBuffer[0]));
    GBL_CTX_VERIFY_CALL(GblRingBuffer_destruct(&pSelf_->ringBuffer[1]));
    GBL_CTX_VERIFY_CALL(GblRingBuffer_destruct(&pSelf_->ringBuffer[2]));
    GBL_CTX_VERIFY_CALL(GblRingBuffer_destruct(&pSelf_->ringBuffer[3]));

    GBL_CTX_END();
}

GBL_EXPORT GblType GblRingBufferTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "constructEmpty",      GblRingBufferTestSuite_constructEmpty_      },
        { "constructValues",     GblRingBufferTestSuite_constructValues_     },
        { "copy",                GblRingBufferTestSuite_copy_                },
        { "move",                GblRingBufferTestSuite_move_                },
        { "atInvalid",           GblRingBufferTestSuite_atInvalid_           },
        { "frontInvalid",        GblRingBufferTestSuite_frontInvalid_        },
        { "backInvalid",         GblRingBufferTestSuite_backInvalid_         },
        { "pushBack",            GblRingBufferTestSuite_pushBack_            },
        { "pushBackOverflow",    GblRingBufferTestSuite_pushBackOverflow_    },
        { "emplaceBack",         GblRingBufferTestSuite_emplaceBack_         },
        { "emplaceBackOverflow", GblRingBufferTestSuite_emplaceBackOverflow_ },
        { "popFront",            GblRingBufferTestSuite_popFront_            },
        { "popFrontInvalid",     GblRingBufferTestSuite_popFrontInvalid_     },
        { "clear",               GblRingBufferTestSuite_clear_               },
        { "destruct",            GblRingBufferTestSuite_destruct_            },
        { NULL,                  NULL                                        }
    };

    const static GblTestSuiteVTable vTable = {
        .pFnSuiteInit   = GblRingBufferTestSuite_init_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("GblRingBufferTestSuite"),
                                     &vTable,
                                     sizeof(GblRingBufferTestSuite),
                                     sizeof(GblRingBufferTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }

    return type;
}

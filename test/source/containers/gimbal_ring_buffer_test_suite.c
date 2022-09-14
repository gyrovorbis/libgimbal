#include "containers/gimbal_ring_buffer_test_suite.h"
#include <gimbal/test/gimbal_test.h>
#include <gimbal/core/gimbal_api_frame.h>
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
    GBL_API_BEGIN(pCtx);
    GblRingBufferTestSuite_* pSelf_ = GBL_RING_BUFFER_TEST_SUITE_(pSelf);
    memset(pSelf_, 0, sizeof(GblRingBufferTestSuite_));
    GBL_API_END();
}

static GBL_RESULT GblRingBufferTestSuite_verify_(GblContext* pCtx, const GblRingBuffer* pBuffer, ...) {
    va_list varArgs;
    va_start(varArgs, pBuffer);
    GBL_API_BEGIN(pCtx);

    GblSize count = 0;
    const char* pStr = NULL;

    for(GblSize i = 0; i < GblRingBuffer_size(pBuffer); ++i) {
        GBL_API_WARN("%u = %s", i, *(const char**)GblRingBuffer_at(pBuffer, i));
    }

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

    GBL_API_END_BLOCK();
    va_end(varArgs);
    return GBL_API_RESULT();
}

static GBL_RESULT GblRingBufferTestSuite_constructEmpty_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GblRingBufferTestSuite_* pSelf_ = GBL_RING_BUFFER_TEST_SUITE_(pSelf);

    GBL_API_VERIFY_CALL(GblRingBuffer_construct(&pSelf_->ringBuffer[0],
                                                sizeof(const char*),
                                                10));
    GBL_API_VERIFY_CALL(GblRingBufferTestSuite_verify_(pCtx, &pSelf_->ringBuffer[0], NULL));


    GBL_API_END();
}

static GBL_RESULT GblRingBufferTestSuite_constructValues_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GblRingBufferTestSuite_* pSelf_ = GBL_RING_BUFFER_TEST_SUITE_(pSelf);

    GBL_API_VERIFY_CALL(GblRingBuffer_construct(&pSelf_->ringBuffer[1],
                                                sizeof(const char*),
                                                10,
                                                3,
                                                stringLiterals_,
                                                pCtx));

    GBL_API_VERIFY_CALL(GblRingBufferTestSuite_verify_(pCtx, &pSelf_->ringBuffer[1], "a", "b", "c", NULL));
    GBL_TEST_COMPARE(GblRingBuffer_context(&pSelf_->ringBuffer[1]), pCtx);
    GBL_TEST_COMPARE(GblRingBuffer_capacity(&pSelf_->ringBuffer[1]), 10);
    GBL_TEST_COMPARE(GblRingBuffer_elementSize(&pSelf_->ringBuffer[1]), sizeof(const char*));
    GBL_TEST_COMPARE(*(const char**)GblRingBuffer_data(&pSelf_->ringBuffer[1]), "a");

    GBL_API_END();
}

static GBL_RESULT GblRingBufferTestSuite_atInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblRingBufferTestSuite_* pSelf_ = GBL_RING_BUFFER_TEST_SUITE_(pSelf);

    GBL_TEST_EXPECT_ERROR();

    const void* pData = GblRingBuffer_at(&pSelf_->ringBuffer[0], 0);
    GBL_TEST_VERIFY(!pData);
    GBL_TEST_COMPARE(GBL_API_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_API_CLEAR_LAST_RECORD();

    pData = GblRingBuffer_at(&pSelf_->ringBuffer[1], 10);
    GBL_TEST_VERIFY(!pData);
    GBL_TEST_COMPARE(GBL_API_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_API_CLEAR_LAST_RECORD();

    GBL_API_END();
}


static GBL_RESULT GblRingBufferTestSuite_frontInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblRingBufferTestSuite_* pSelf_ = GBL_RING_BUFFER_TEST_SUITE_(pSelf);

    GBL_TEST_EXPECT_ERROR();

    const void* pData = GblRingBuffer_front(&pSelf_->ringBuffer[0]);
    GBL_TEST_VERIFY(!pData);
    GBL_TEST_COMPARE(GBL_API_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_API_CLEAR_LAST_RECORD();

    GBL_API_END();
}

static GBL_RESULT GblRingBufferTestSuite_backInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblRingBufferTestSuite_* pSelf_ = GBL_RING_BUFFER_TEST_SUITE_(pSelf);

    GBL_TEST_EXPECT_ERROR();

    const void* pData = GblRingBuffer_back(&pSelf_->ringBuffer[0]);
    GBL_TEST_VERIFY(!pData);
    GBL_TEST_COMPARE(GBL_API_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_API_CLEAR_LAST_RECORD();

    GBL_API_END();
}

static GBL_RESULT GblRingBufferTestSuite_pushBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GblRingBufferTestSuite_* pSelf_ = GBL_RING_BUFFER_TEST_SUITE_(pSelf);

    for(GblSize o = 3; o < 9; ++o)
        GBL_API_VERIFY_CALL(GblRingBuffer_pushBack(&pSelf_->ringBuffer[1], &stringLiterals_[o]));

    GBL_API_VERIFY_CALL(GblRingBufferTestSuite_verify_(pCtx,
                                                       &pSelf_->ringBuffer[1],
                                                       "a", "b", "c", "d", "e", "f", "g", "h", "i", NULL));

    GBL_API_END();
}


static GBL_RESULT GblRingBufferTestSuite_pushBackOverflow_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GBL_TEST_SKIP("unimplemented");
    GblRingBufferTestSuite_* pSelf_ = GBL_RING_BUFFER_TEST_SUITE_(pSelf);

    GBL_API_VERIFY_CALL(GblRingBuffer_pushBack(&pSelf_->ringBuffer[1], &stringLiterals_[9]));

    GBL_API_VERIFY_CALL(GblRingBufferTestSuite_verify_(pCtx,
                                                       &pSelf_->ringBuffer[1],
                                                       "b", "c", "d", "e", "f", "g", "h", "i", "j", NULL));



    GBL_API_END();
}

static GBL_RESULT GblRingBufferTestSuite_emplaceBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblRingBufferTestSuite_* pSelf_ = GBL_RING_BUFFER_TEST_SUITE_(pSelf);

    for(GblSize o = 0; o < 5; ++o) {
        const char** ppData = GblRingBuffer_emplaceBack(&pSelf_->ringBuffer[0]);
        GBL_TEST_VERIFY(ppData);
        *ppData = stringLiterals_[o];
    }

    GBL_API_VERIFY_CALL(GblRingBufferTestSuite_verify_(pCtx,
                                                       &pSelf_->ringBuffer[0],
                                                       "a", "b", "c", "d", "e", NULL));
    GBL_API_END();
}

static GBL_RESULT GblRingBufferTestSuite_emplaceBackOverflow_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblRingBufferTestSuite_* pSelf_ = GBL_RING_BUFFER_TEST_SUITE_(pSelf);
    GBL_TEST_SKIP("unimplemented");
    GBL_API_END();
}

static GBL_RESULT GblRingBufferTestSuite_popFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblRingBufferTestSuite_* pSelf_ = GBL_RING_BUFFER_TEST_SUITE_(pSelf);
    GBL_TEST_SKIP("unimplemented");

    const char** ppData = GblRingBuffer_popFront(&pSelf_->ringBuffer[0]);
    GBL_TEST_VERIFY(ppData);
    GBL_TEST_COMPARE(*ppData, "a");

    GBL_API_VERIFY_CALL(GblRingBufferTestSuite_verify_(pCtx,
                                                       &pSelf_->ringBuffer[0],
                                                       "b", "c", "d", "e", NULL));
    GBL_API_END();
}

static GBL_RESULT GblRingBufferTestSuite_append_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblRingBufferTestSuite_* pSelf_ = GBL_RING_BUFFER_TEST_SUITE_(pSelf);
    GBL_TEST_SKIP("unimplemented");
    GBL_API_END();
}


static GBL_RESULT GblRingBufferTestSuite_clear_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblRingBufferTestSuite_* pSelf_ = GBL_RING_BUFFER_TEST_SUITE_(pSelf);
    GBL_TEST_SKIP("unimplemented");
    GBL_API_END();
}

static GBL_RESULT GblRingBufferTestSuite_reserve_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblRingBufferTestSuite_* pSelf_ = GBL_RING_BUFFER_TEST_SUITE_(pSelf);
    GBL_TEST_SKIP("unimplemented");
    GBL_API_END();
}


static GBL_RESULT GblRingBufferTestSuite_reserveOverflow_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblRingBufferTestSuite_* pSelf_ = GBL_RING_BUFFER_TEST_SUITE_(pSelf);
    GBL_TEST_SKIP("unimplemented");
    GBL_API_END();
}

static GBL_RESULT GblRingBufferTestSuite_resizeGrow_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblRingBufferTestSuite_* pSelf_ = GBL_RING_BUFFER_TEST_SUITE_(pSelf);
    GBL_TEST_SKIP("unimplemented");
    GBL_API_END();
}

static GBL_RESULT GblRingBufferTestSuite_resizeGrowReserve_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblRingBufferTestSuite_* pSelf_ = GBL_RING_BUFFER_TEST_SUITE_(pSelf);
    GBL_TEST_SKIP("unimplemented");
    GBL_API_END();
}

static GBL_RESULT GblRingBufferTestSuite_resizeShrink_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblRingBufferTestSuite_* pSelf_ = GBL_RING_BUFFER_TEST_SUITE_(pSelf);
    GBL_TEST_SKIP("unimplemented");
    GBL_API_END();
}

static GBL_RESULT GblRingBufferTestSuite_shrinkToFit_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblRingBufferTestSuite_* pSelf_ = GBL_RING_BUFFER_TEST_SUITE_(pSelf);
    GBL_TEST_SKIP("unimplemented");
    GBL_API_END();
}

static GBL_RESULT GblRingBufferTestSuite_destruct_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblRingBufferTestSuite_* pSelf_ = GBL_RING_BUFFER_TEST_SUITE_(pSelf);

    GBL_API_VERIFY_CALL(GblRingBuffer_destruct(&pSelf_->ringBuffer[0]));
    GBL_API_VERIFY_CALL(GblRingBuffer_destruct(&pSelf_->ringBuffer[1]));

    GBL_API_END();
}

GBL_EXPORT GblType GblRingBufferTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "constructEmpty",      GblRingBufferTestSuite_constructEmpty_      },
        { "constructValues",     GblRingBufferTestSuite_constructValues_     },
        { "atInvalid",           GblRingBufferTestSuite_atInvalid_           },
        { "frontInvalid",        GblRingBufferTestSuite_frontInvalid_        },
        { "backInvalid",         GblRingBufferTestSuite_backInvalid_         },
        { "pushBack",            GblRingBufferTestSuite_pushBack_            },
        { "pushBackOverflow",    GblRingBufferTestSuite_pushBackOverflow_    },
        { "emplaceBack",         GblRingBufferTestSuite_emplaceBack_         },
        { "emplaceBackOverflow", GblRingBufferTestSuite_emplaceBackOverflow_ },
        { "popFront",            GblRingBufferTestSuite_popFront_            },
        { "append",              GblRingBufferTestSuite_append_              },
        { "clear",               GblRingBufferTestSuite_clear_               },
        { "reserve",             GblRingBufferTestSuite_reserve_             },
        { "reserveOverflow",     GblRingBufferTestSuite_reserveOverflow_     },
        { "resizeGrow",          GblRingBufferTestSuite_resizeGrow_          },
        { "resizeGrowReserve",   GblRingBufferTestSuite_resizeGrowReserve_   },
        { "resizeShrink",        GblRingBufferTestSuite_resizeShrink_        },
        { "shrinkToFit",         GblRingBufferTestSuite_shrinkToFit_         },
        { "destruct",            GblRingBufferTestSuite_destruct_            },
        { NULL,                  NULL                                        }
    };

    const static GblTestSuiteClassVTable vTable = {
        .pFnSuiteInit   = GblRingBufferTestSuite_init_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_API_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("GblRingBufferTestSuite"),
                                     &vTable,
                                     sizeof(GblRingBufferTestSuite),
                                     sizeof(GblRingBufferTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_API_VERIFY_LAST_RECORD();
        GBL_API_END_BLOCK();
    }

    return type;
}

#include "algorithms/gimbal_hash_test_suite.h"
#include <gimbal/algorithms/gimbal_hash.h>
#include <gimbal/test/gimbal_test_macros.h>

#define GBL_HASH_TEST_SUITE_(inst)   ((GblHashTestSuite_*)GBL_INSTANCE_PRIVATE(inst, GBL_HASH_TEST_SUITE_TYPE))

#define GBL_HASH_TEST_SUITE_WORD_COUNT_     512
#define GBL_HASH_TEST_SUITE_WORD_SIZE_MAX_  50
#define GBL_HASH_TEST_SUITE_WORD_SIZE_MIN_  20
#define GBL_HASH_TEST_SUITE_WORD_CHARS_     NULL

typedef struct GblHashTestSuite_ {
    char words[GBL_HASH_TEST_SUITE_WORD_COUNT_][GBL_HASH_TEST_SUITE_WORD_SIZE_MAX_+1];
    size_t  total;
} GblHashTestSuite_;

static void* GblHashTestSuite_randomizeWords_(GblTestSuite* pSelf) {
    GblHashTestSuite_* pSelf_ = GBL_HASH_TEST_SUITE_(pSelf);
    for(size_t  w = 0; w < GBL_HASH_TEST_SUITE_WORD_COUNT_; ++w)
        gblRandString(pSelf_->words[w],
                      GBL_HASH_TEST_SUITE_WORD_SIZE_MIN_,
                      GBL_HASH_TEST_SUITE_WORD_SIZE_MAX_,
                      GBL_HASH_TEST_SUITE_WORD_CHARS_);
    return pSelf_->words;
}

static GBL_RESULT GblHashTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblHashTestSuite_randomizeWords_(pSelf);

    GBL_CTX_END();
}

static GBL_RESULT GblHashTestSuite_fnv1_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblHashTestSuite_* pSelf_ = GBL_HASH_TEST_SUITE_(pSelf);

    volatile size_t  total = 0;
    for(size_t  w = 0; w < GBL_HASH_TEST_SUITE_WORD_COUNT_; ++w) {
        total += gblHashFnv1(pSelf_->words[w],
                             strlen(pSelf_->words[w]));
    }

    pSelf_->total = total;

    GBL_CTX_END();
}

static GBL_RESULT GblHashTestSuite_murmur_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblHashTestSuite_* pSelf_ = GBL_HASH_TEST_SUITE_(pSelf);

    volatile size_t  total = 0;
    for(size_t  w = 0; w < GBL_HASH_TEST_SUITE_WORD_COUNT_; ++w) {
        total += gblHashMurmur(pSelf_->words[w],
                               strlen(pSelf_->words[w]));
    }

    pSelf_->total = total;

    GBL_CTX_END();
}


static GBL_RESULT GblHashTestSuite_superFast_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblHashTestSuite_* pSelf_ = GBL_HASH_TEST_SUITE_(pSelf);

    volatile size_t  total = 0;
    for(size_t  w = 0; w < GBL_HASH_TEST_SUITE_WORD_COUNT_; ++w) {
        total += gblHashSuperFast(pSelf_->words[w],
                                  strlen(pSelf_->words[w]));
    }

    pSelf_->total = total;

    GBL_CTX_END();
}


static GBL_RESULT GblHashTestSuite_pearson_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblHashTestSuite_* pSelf_ = GBL_HASH_TEST_SUITE_(pSelf);

    // warm up the algorithm
    volatile size_t  total = gblHashPearson(pSelf_->words[0], 1);
    for(size_t  w = 0; w < GBL_HASH_TEST_SUITE_WORD_COUNT_; ++w) {
        total += gblHashPearson(pSelf_->words[w],
                                strlen(pSelf_->words[w]));
    }

    pSelf_->total = total;

    GBL_CTX_END();
}



static GBL_RESULT GblHashTestSuite_sip_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblHashTestSuite_* pSelf_ = GBL_HASH_TEST_SUITE_(pSelf);

    volatile size_t  total = 0;
    for(size_t  w = 0; w < GBL_HASH_TEST_SUITE_WORD_COUNT_; ++w) {
        total += gblHashSip(pSelf_->words[w],
                            strlen(pSelf_->words[w]));
    }

    pSelf_->total = total;

    GBL_CTX_END();
}


static GBL_RESULT GblHashTestSuite_crc_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblHashTestSuite_* pSelf_ = GBL_HASH_TEST_SUITE_(pSelf);

    volatile size_t  total = 0;
    for(size_t  w = 0; w < GBL_HASH_TEST_SUITE_WORD_COUNT_; ++w) {
        total += gblHashCrc(pSelf_->words[w],
                            strlen(pSelf_->words[w]));
    }

    pSelf_->total = total;

    GBL_CTX_END();
}


GBL_EXPORT GblType GblHashTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "fnv1",       GblHashTestSuite_fnv1_         },
        { "murmur",     GblHashTestSuite_murmur_       },
        { "superFast",  GblHashTestSuite_superFast_    },
        { "pearson",    GblHashTestSuite_pearson_      },
        { "sip",        GblHashTestSuite_sip_          },
        { "crc",        GblHashTestSuite_crc_          },
        { NULL,     NULL                               }
    };

    const static GblTestSuiteClassVTable vTable = {
        .pFnSuiteInit   = GblHashTestSuite_init_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("GblHashTestSuite"),
                                     &vTable,
                                     sizeof(GblHashTestSuite),
                                     sizeof(GblHashTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }
    return type;
}

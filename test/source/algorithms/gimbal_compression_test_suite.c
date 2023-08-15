#include "algorithms/gimbal_compression_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/algorithms/gimbal_compression.h>
#include <gimbal/algorithms/gimbal_random.h>

#define RANDOM_BUFFER_SIZE_     (1024 * 4)

#define GBL_SELF_TYPE GblCompressionTestSuite

GBL_TEST_FIXTURE {
    uint8_t* pSrcBuffer;
    uint8_t* pCompBuffer;
    uint8_t* pDstBuffer;
    int      compressedSize;
};

GBL_TEST_INIT()
    pFixture->pSrcBuffer  = GBL_CTX_MALLOC(RANDOM_BUFFER_SIZE_);
    pFixture->pDstBuffer  = GBL_CTX_MALLOC(RANDOM_BUFFER_SIZE_);
    pFixture->pCompBuffer = GBL_CTX_MALLOC(RANDOM_BUFFER_SIZE_);
    memset(pFixture->pSrcBuffer, 0, RANDOM_BUFFER_SIZE_);
    // Leave some nonrandom space so it has something to compress
    gblRandBuffer(pFixture->pSrcBuffer, RANDOM_BUFFER_SIZE_ - 512);
GBL_TEST_CASE_END

GBL_TEST_FINAL()
    GBL_CTX_FREE(pFixture->pSrcBuffer);
    GBL_CTX_FREE(pFixture->pDstBuffer);
    GBL_CTX_FREE(pFixture->pCompBuffer);
GBL_TEST_CASE_END

GBL_TEST_CASE(compress)
    pFixture->compressedSize =
        gblCompress(pFixture->pSrcBuffer,
                    pFixture->pCompBuffer,
                    RANDOM_BUFFER_SIZE_,
                    RANDOM_BUFFER_SIZE_,
                    -1);
    GBL_TEST_VERIFY(pFixture->compressedSize > 0);
GBL_TEST_CASE_END

GBL_TEST_CASE(decompress)
    gblDecompress(pFixture->pCompBuffer,
                  pFixture->pDstBuffer,
                  pFixture->compressedSize,
                  RANDOM_BUFFER_SIZE_);
GBL_TEST_CASE_END

GBL_TEST_CASE(verify)
    for(size_t b = 0; b < RANDOM_BUFFER_SIZE_; ++b)
        GBL_TEST_COMPARE(pFixture->pDstBuffer[b],
                         pFixture->pSrcBuffer[b]);
GBL_TEST_CASE_END

GBL_TEST_REGISTER(compress,
                  decompress,
                  verify)

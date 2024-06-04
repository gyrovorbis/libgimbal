#include <gimbal/algorithms/gimbal_random.h>
#include <gimbal/test/gimbal_test_macros.h>
#include "algorithms/gimbal_random_test_suite.h"
#include <time.h>

#define BENCHMARK_ITERATIONS_   9999

#define GBL_SELF_TYPE GblRandomTestSuite

GBL_TEST_FIXTURE {
    int randomNumbers[2][BENCHMARK_ITERATIONS_];
};

GBL_TEST_INIT()

GBL_TEST_CASE_END

GBL_TEST_FINAL()

GBL_TEST_CASE_END

#define SIZE 2

void findDuplicates_(GblRandomGeneratorFn pGen) {
    GBL_CTX_BEGIN(NULL);
    gblSetRand(pGen);

     uint8_t reference[SIZE] = {0};
    uint8_t current[SIZE] = {0};
    for(int i = 0; i < SIZE; ++i)
     reference[i] = (uint8_t)(gblRand() % 256);


    for(uint64_t i = 1; i < UINT64_MAX; i++)
    {
        for(int j = 0; j < SIZE; j++)
            current[j] = (uint8_t)(gblRand() % 256);


        if(!memcmp(reference, current, SIZE))
        {
            char* pTempBuff = GBL_ALLOCA(SIZE * 4 + 1);
            pTempBuff[0] = '\0';
            char byteBuff[10];
            GBL_CTX_INFO("Duplicate found after %llu generations", (unsigned long long)i);
            GBL_CTX_PUSH();
            for(int k = 0; k < SIZE; k++) {
                snprintf(byteBuff, sizeof(byteBuff), "%02X ", current[k]);
                strcat(pTempBuff, byteBuff);
            }
            GBL_CTX_INFO("%s", pTempBuff);
            GBL_CTX_POP(1);
            break;
        }
    }

    GBL_CTX_END_BLOCK();
}

void benchmark_(GblRandomGeneratorFn pGen, GblTestFixture* pFixture, int index) {
    gblSetRand(pGen);

    size_t duplicates = 0;

    for(size_t i = 0; i < BENCHMARK_ITERATIONS_; ++i) {
        pFixture->randomNumbers[index][i] = gblRand();

#if 1
        for(size_t j = 0; j < i; ++j) {
            if(pFixture->randomNumbers[index][i] ==
               pFixture->randomNumbers[index][j]) {
                ++duplicates;
                break;
            }
        }
#endif
    }

//    GBL_CTX_INFO("Found %zu duplicates!", duplicates);
}

GBL_TEST_CASE(libCDuplicate)
    findDuplicates_(gblRandLibC);
GBL_TEST_CASE_END

GBL_TEST_CASE(lehmerDuplicate)
    findDuplicates_(gblRandLehmer);
GBL_TEST_CASE_END

GBL_TEST_CASE(libCBenchmark)
    benchmark_(gblRandLibC, pFixture, 0);
GBL_TEST_CASE_END

GBL_TEST_CASE(lehmerBenchmark)
    benchmark_(gblRandLehmer, pFixture, 1);
GBL_TEST_CASE_END

GBL_TEST_REGISTER(libCDuplicate,
                  lehmerDuplicate,
                  libCBenchmark,
                  lehmerBenchmark)

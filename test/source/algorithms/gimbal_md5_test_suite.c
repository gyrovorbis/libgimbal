#include "algorithms/gimbal_md5_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/algorithms/gimbal_md5.h>


#define GBL_SELF_TYPE GblMd5TestSuite

GBL_TEST_FIXTURE {
    GblMd5Context ctx;
};

static char *test_data[] = {
    "abc",
    "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
    "A million repetitions of 'a'"};
static char *test_results[] = {
    "90015098 3CD24FB0 D6963F7D 28E17F72",
    "8215EF07 96A20BCA AAE116D3 876C664A",
    "36A92CC9 4A9E0FA2 1F625F8B FB007ADF"};

GBL_TEST_INIT()
    (void)pFixture;
GBL_TEST_CASE_END

GBL_TEST_FINAL()
    (void)pFixture;
GBL_TEST_CASE_END

GBL_TEST_CASE(smallInit)
    GblMd5_init(&pFixture->ctx);
GBL_TEST_CASE_END

GBL_TEST_CASE(smallUpdate)
    GblMd5_update(&pFixture->ctx, test_data[0], strlen(test_data[0]));
GBL_TEST_CASE_END

GBL_TEST_CASE(smallFinal)
    uint8_t digest[GBL_MD5_DIGEST_SIZE];
    char    buffer[GBL_MD5_DIGEST_STRING_SIZE];

    GblMd5_final(&pFixture->ctx, digest);
    GBL_TEST_COMPARE(GblMd5_string(digest, buffer),
                     test_results[0]);
GBL_TEST_CASE_END

GBL_TEST_CASE(largeInit)
    GblMd5_init(&pFixture->ctx);
GBL_TEST_CASE_END

GBL_TEST_CASE(largeUpdate)
    GblMd5_update(&pFixture->ctx, test_data[1], strlen(test_data[1]));
GBL_TEST_CASE_END

GBL_TEST_CASE(largeFinal)
    uint8_t digest[GBL_MD5_DIGEST_SIZE];
    char    buffer[GBL_MD5_DIGEST_STRING_SIZE];

    GblMd5_final(&pFixture->ctx, digest);
    GBL_TEST_COMPARE(GblMd5_string(digest, buffer),
                     test_results[1]);
GBL_TEST_CASE_END

GBL_TEST_CASE(hundredRepsInit)
    GblMd5_init(&pFixture->ctx);
GBL_TEST_CASE_END

GBL_TEST_CASE(hundredRepsUpdate)
    for(int k = 0; k < 100; k++)
        GblMd5_update(&pFixture->ctx, "a", 1);
GBL_TEST_CASE_END


GBL_TEST_CASE(hundredRepsFinal)
    uint8_t digest[GBL_MD5_DIGEST_SIZE];
    char    buffer[GBL_MD5_DIGEST_STRING_SIZE];

    GblMd5_final(&pFixture->ctx, digest);
    GBL_TEST_COMPARE(GblMd5_string(digest, buffer),
                     test_results[2]);
GBL_TEST_CASE_END


GBL_TEST_REGISTER(smallInit,
                  smallUpdate,
                  smallFinal,
                  largeInit,
                  largeUpdate,
                  largeFinal,
                  hundredRepsInit,
                  hundredRepsUpdate,
                  hundredRepsFinal);

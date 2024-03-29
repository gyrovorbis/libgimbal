#include "algorithms/gimbal_sha1_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/algorithms/gimbal_sha1.h>

#define GBL_SELF_TYPE GblSha1TestSuite

GBL_TEST_FIXTURE {
    GblSha1Context ctx;
};

static char *test_data[] = {
    "abc",
    "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
    "A hundred repetitions of 'a'"};
static char *test_results[] = {
    "A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D",
    "84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1",
    "7F900025 7A4918D7 072655EA 468540CD CBD42E0C"};

GBL_TEST_INIT()
    (void)pFixture;
GBL_TEST_CASE_END

GBL_TEST_FINAL()
    (void)pFixture;
GBL_TEST_CASE_END

GBL_TEST_CASE(smallInit)
    GblSha1_init(&pFixture->ctx);
GBL_TEST_CASE_END

GBL_TEST_CASE(smallUpdate)
    GblSha1_update(&pFixture->ctx, test_data[0], strlen(test_data[0]));
GBL_TEST_CASE_END

GBL_TEST_CASE(smallFinal)
    uint8_t digest[GBL_SHA1_DIGEST_SIZE];
    char    buffer[GBL_SHA1_DIGEST_STRING_SIZE];

    GblSha1_final(&pFixture->ctx, digest);
    GBL_TEST_COMPARE(GblSha1_string(digest, buffer),
                     test_results[0]);
GBL_TEST_CASE_END

GBL_TEST_CASE(largeInit)
    GblSha1_init(&pFixture->ctx);
GBL_TEST_CASE_END

GBL_TEST_CASE(largeUpdate)
    GblSha1_update(&pFixture->ctx, test_data[1], strlen(test_data[1]));
GBL_TEST_CASE_END

GBL_TEST_CASE(largeFinal)
    uint8_t digest[GBL_SHA1_DIGEST_SIZE];
    char    buffer[GBL_SHA1_DIGEST_STRING_SIZE];

    GblSha1_final(&pFixture->ctx, digest);
    GBL_TEST_COMPARE(GblSha1_string(digest, buffer),
                     test_results[1]);
GBL_TEST_CASE_END

GBL_TEST_CASE(hundredRepsInit)
    GblSha1_init(&pFixture->ctx);
GBL_TEST_CASE_END

GBL_TEST_CASE(hundredRepsUpdate)
    for(int k = 0; k < 100; k++)
        GblSha1_update(&pFixture->ctx, "a", 1);
GBL_TEST_CASE_END


GBL_TEST_CASE(hundredRepsFinal)
    uint8_t digest[GBL_SHA1_DIGEST_SIZE];
    char    buffer[GBL_SHA1_DIGEST_STRING_SIZE];

    GblSha1_final(&pFixture->ctx, digest);
    GBL_TEST_COMPARE(GblSha1_string(digest, buffer),
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

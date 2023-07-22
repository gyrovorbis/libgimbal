#include "utils/gimbal_uuid_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/core/gimbal_ctx.h>
#include <gimbal/utils/gimbal_uuid.h>

#define GBL_UUID_TEST_SUITE__V4_COUNT    256

static GBL_RESULT GblUuidTestSuite_versionInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblUuid_version(NULL), 0);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_INVALID_POINTER);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblUuidTestSuite_compareInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();

    GblUuid uuid;

    GBL_TEST_VERIFY(GblUuid_compare(&uuid, NULL) != 0);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_INVALID_POINTER);
    GBL_CTX_CLEAR_LAST_RECORD();


    GBL_TEST_VERIFY(GblUuid_compare(NULL, &uuid) != 0);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_INVALID_POINTER);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblUuidTestSuite_isNilInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblUuid_isNil(NULL), GBL_TRUE);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_INVALID_POINTER);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}


static GBL_RESULT GblUuidTestSuite_initNilInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblUuid_initNil(NULL), GBL_RESULT_ERROR_INVALID_POINTER);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblUuidTestSuite_initNil_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GblUuid uuid1, uuid2;

    GBL_CTX_VERIFY_CALL(GblUuid_initNil(&uuid1));
    GBL_CTX_VERIFY_CALL(GblUuid_initNil(&uuid2));

    GBL_TEST_VERIFY(GblUuid_isNil(&uuid1));
    GBL_TEST_VERIFY(GblUuid_isNil(&uuid2));

    GBL_TEST_COMPARE(GblUuid_compare(&uuid1, &uuid2), 0);
    GBL_TEST_COMPARE(GblUuid_version(&uuid1), 0);

    GBL_CTX_END();
}

static GBL_RESULT GblUuidTestSuite_initV4Invalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblUuid_initV4(NULL), GBL_RESULT_ERROR_INVALID_POINTER);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblUuidTestSuite_initV4_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GblUuid uuids[GBL_UUID_TEST_SUITE__V4_COUNT];

    for(int u = 0; u < GBL_UUID_TEST_SUITE__V4_COUNT; ++u) {
        GBL_CTX_VERIFY_CALL(GblUuid_initV4(&uuids[u]));

        GBL_TEST_COMPARE(GblUuid_version(&uuids[u]), 4);
        GBL_TEST_VERIFY(!GblUuid_isNil(&uuids[u]));

        for(int u2 = 0; u2 < u-1; ++u2) {
            GBL_TEST_VERIFY(GblUuid_compare(&uuids[u], &uuids[u2]) != 0);
        }

        GBL_TEST_COMPARE(GblUuid_compare(&uuids[u], &uuids[u]), 0);
    }

    GBL_CTX_END();
}

static GBL_RESULT GblUuidTestSuite_initFromStringInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();

    GblUuid uuid;

    // NULL UUID
    GBL_TEST_COMPARE(GblUuid_initFromString(NULL, NULL), GBL_RESULT_ERROR_INVALID_POINTER);
    GBL_CTX_CLEAR_LAST_RECORD();

    // NULL string
    GBL_TEST_COMPARE(GblUuid_initFromString(&uuid, NULL), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    // String too small
    GBL_TEST_COMPARE(GblUuid_initFromString(&uuid, ""), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    // String too large
    GBL_TEST_COMPARE(GblUuid_initFromString(&uuid, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();

    // String with bullshit characters
    GBL_TEST_COMPARE(GblUuid_initFromString(&uuid, "qwrb9644-2b63-4dc3-8017-c191009d6725"), GBL_RESULT_ERROR_INVALID_ARG);
    GBL_CTX_CLEAR_LAST_RECORD();

    // String with incorrect hyphen placement
    GBL_TEST_COMPARE(GblUuid_initFromString(&uuid, "454b964-42b63-4dc3-8017-c191009d6725"), GBL_RESULT_ERROR_INVALID_ARG);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

static GBL_RESULT GblUuidTestSuite_initFromString_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);

    const char* uuidStrings[] = {
        "454b9644-2b63-4dc3-8017-c191009d6725",
        "e6accf31-0a2e-44f9-842a-ece814e98ea3",
        "7e52fd6e-1402-4b5f-a741-ccb2b6c5c2be",
        "5d6ac0b1-bfb0-4ad1-95b5-8a437c438818",
        "8812c244-616e-4af8-a118-d02eadf8975e",
        "44a94cdc-9a45-41f5-bcd9-8ab49882550c",
        "adfc0144-bd2a-4fed-ad53-7db074da3964",
        "9b6807ef-fa9c-4d05-ad0a-6d4499a9e6c7",
        "2e78f517-1055-4b72-b6d3-1025789aa6c7",
        "a4455fb7-2be8-4df7-8e7b-1389016984d9"
    };

    GblUuid uuids[GBL_COUNT_OF(uuidStrings)];

    GBL_CTX_BEGIN(pCtx);

    for(int u = 0; u < GBL_COUNT_OF(uuidStrings); ++u) {
        GBL_CTX_VERIFY_CALL(GblUuid_initFromString(&uuids[u], uuidStrings[u]));

        GBL_TEST_COMPARE(GblUuid_version(&uuids[u]), 4);
        GBL_TEST_VERIFY(!GblUuid_isNil(&uuids[u]));

        for(int u2 = 0; u2 < u-1; ++u2) {
            GBL_TEST_VERIFY(GblUuid_compare(&uuids[u], &uuids[u2]) != 0);
        }

        GblUuid tempUuid;
        GBL_CTX_VERIFY_CALL(GblUuid_initFromString(&tempUuid, uuidStrings[u]));

        GBL_TEST_COMPARE(GblUuid_compare(&uuids[u], &tempUuid), 0);

    }

    GBL_CTX_END();
}

static GBL_RESULT GblUuidTestSuite_toString_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);

    const char* uuidStrings[] = {
        "454b9644-2b63-4dc3-8017-c191009d6725",
        "e6accf31-0a2e-44f9-842a-ece814e98ea3",
        "7e52fd6e-1402-4b5f-a741-ccb2b6c5c2be",
        "5d6ac0b1-bfb0-4ad1-95b5-8a437c438818",
        "8812c244-616e-4af8-a118-d02eadf8975e",
        "44a94cdc-9a45-41f5-bcd9-8ab49882550c",
        "adfc0144-bd2a-4fed-ad53-7db074da3964",
        "9b6807ef-fa9c-4d05-ad0a-6d4499a9e6c7",
        "2e78f517-1055-4b72-b6d3-1025789aa6c7",
        "a4455fb7-2be8-4df7-8e7b-1389016984d9"
    };

    GblUuid uuids[GBL_COUNT_OF(uuidStrings)];
    char uuidStringBuffer[GBL_UUID_STRING_SIZE];

    GBL_CTX_BEGIN(pCtx);

    for(int u = 0; u < GBL_COUNT_OF(uuidStrings); ++u) {
        GBL_CTX_VERIFY_CALL(GblUuid_initFromString(&uuids[u], uuidStrings[u]));
        GBL_CTX_VERIFY_CALL(GblUuid_toString(&uuids[u], uuidStringBuffer));
        GBL_TEST_COMPARE(uuidStrings[u], uuidStringBuffer);

        GblUuid tempUuid;
        GBL_CTX_VERIFY_CALL(GblUuid_initFromString(&tempUuid, uuidStringBuffer));
        GBL_TEST_COMPARE(GblUuid_compare(&uuids[u], &tempUuid), 0);

    }

    GBL_CTX_END();
}

GBL_EXPORT GblType GblUuidTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "versionInvalid",         GblUuidTestSuite_versionInvalid_        },
        { "compareInvalid",         GblUuidTestSuite_compareInvalid_        },
        { "isNilInvalid",           GblUuidTestSuite_isNilInvalid_          },
        { "initNilInvalid",         GblUuidTestSuite_initNilInvalid_        },
        { "initNil",                GblUuidTestSuite_initNil_               },
        { "initV4",                 GblUuidTestSuite_initV4_                },
        { "initV4Invalid",          GblUuidTestSuite_initV4Invalid_         },
        { "initFromStringInvalid",  GblUuidTestSuite_initFromStringInvalid_ },
        { "initFromString",         GblUuidTestSuite_initFromString_        },
        { "toString",               GblUuidTestSuite_toString_              },
        { NULL,                     NULL                                    }
    };

    const static GblTestSuiteVTable vTable = {
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("GblUuidTestSuite"),
                                     &vTable,
                                     sizeof(GblUuidTestSuite),
                                     0,
                                     GBL_TYPE_FLAGS_NONE);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }
    return type;
}

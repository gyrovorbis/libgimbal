#include "utils/gimbal_version_test_suite.h"
#include <gimbal/utils/gimbal_version.h>
#include <gimbal/test/gimbal_test_macros.h>

static GBL_RESULT GblVersionTestSuite_make_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GblVersion version = GblVersion_make(13, 258, 254);

    GBL_TEST_COMPARE(version, GBL_VERSION_MAKE(13, 258, 254));
    GBL_TEST_VERIFY(version > GBL_VERSION_MAKE(13, 257, 255));

    GBL_TEST_COMPARE(GblVersion_major(version), 13);
    GBL_TEST_COMPARE(GblVersion_minor(version), 258);
    GBL_TEST_COMPARE(GblVersion_patch(version), 254);

    GBL_CTX_END();
}

static GBL_RESULT GblVersionTestSuite_parse_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GblVersion version = GblVersion_parse("128.512.3");

    GBL_TEST_COMPARE(GblVersion_major(version), 128);
    GBL_TEST_COMPARE(GblVersion_minor(version), 512);
    GBL_TEST_COMPARE(GblVersion_patch(version), 3);

    GBL_CTX_END();
}

static GBL_RESULT GblVersionTestSuite_string_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    char buffer[GBL_VERSION_STRING_SIZE];
    GblVersion version = GblVersion_parse("128.65535.3");

    GBL_TEST_COMPARE(GblVersion_string(version, buffer), "128.65535.3");

    GBL_CTX_END();
}

static GBL_RESULT GblVersionTestSuite_set_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GblVersion version;

    GblVersion_setMajor(&version, 255);
    GblVersion_setMinor(&version, 512);
    GblVersion_setPatch(&version, 0);

    GBL_TEST_COMPARE(GblVersion_major(version), 255);
    GBL_TEST_COMPARE(GblVersion_minor(version), 512);
    GBL_TEST_COMPARE(GblVersion_patch(version), 0);

    GBL_CTX_END();
}

GBL_EXPORT GblType GblVersionTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "make",   GblVersionTestSuite_make_   },
        { "parse",  GblVersionTestSuite_parse_  },
        { "string", GblVersionTestSuite_string_ },
        { "set",    GblVersionTestSuite_set_    },
        { NULL,                     NULL        }
    };

    const static GblTestSuiteVTable vTable = {
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStatic("GblVersionTestSuite"),
                                     &vTable,
                                     sizeof(GblVersionTestSuite),
                                     0,
                                     GBL_TYPE_FLAGS_NONE);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }
    return type;
}

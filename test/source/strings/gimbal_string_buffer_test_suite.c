#include "strings/gimbal_string_buffer_test_suite.h"
#include <gimbal/test/gimbal_test.h>
#include <gimbal/strings/gimbal_string_buffer.h>

#define GBL_STRING_BUFFER_TEST_SUITE_(inst) ((GblStringBufferTestSuite*)GBL_INSTANCE_PRIVATE(inst, GBL_STRING_BUFFER_TEST_SUITE))

typedef struct GblStringBufferTestSuite_ {
    GblStringBuffer basicBuffer;
    struct {
        GblStringBuffer extBuffer;
        char            buffer[32];
    };
} GblStringBufferTestSuite_;


GBL_EXPORT GblType GblStringBufferTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { NULL, NULL }
    };

    const static GblTestSuiteClassVTable vTable = {
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_API_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("StringBufferTestSuite"),
                                     &vTable,
                                     sizeof(GblStringBufferTestSuite),
                                     sizeof(GblStringBufferTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_API_VERIFY_LAST_RECORD();
        GBL_API_END_BLOCK();
    }
    return type;
}


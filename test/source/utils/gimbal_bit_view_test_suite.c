#include "utils/gimbal_bit_view_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/utils/gimbal_bit_view.h>
#include <gimbal/strings/gimbal_string_buffer.h>

#define GBL_SELF_TYPE GblBitViewTestSuite

GBL_TEST_FIXTURE {
    unsigned dummy;
};

GBL_TEST_INIT() {
#if GBL_BIG_ENDIAN == 1
    GBL_TEST_SKIP("Unimplemented for big endian!");
#endif
    GBL_TEST_CASE_END;
}

GBL_TEST_FINAL() {
    GBL_TEST_CASE_END;
}

static GBL_RESULT verifyBits_(GblTestSuite* pSelf,
                              const GblBitView* pView,
                              const char* pPattern)
{
    GBL_CTX_BEGIN(pSelf);

    GBL_TEST_COMPARE(pView->length,
                     strlen(pPattern));

    for(size_t  b = 0; b < pView->length; ++b) {

        GBL_TEST_COMPARE(GblBitView_at(pView, b),
                         (pPattern[b] == '1')? GBL_TRUE : GBL_FALSE);
    }

    GBL_CTX_END();
}

GBL_TEST_CASE(fromBuffer) {
    char buffer[GBL_BIT_VIEW_BYTES(12)] = {
        0xfa, 0xbf
    };
    GblBitView view;

    GBL_TEST_COMPARE(GblBitView_fromBuffer(&view, buffer, 14, 1),
                     &view);

    GBL_TEST_CALL(verifyBits_(pSelf, &view, "10111111111110"));

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(fromView) {
    char buffer[GBL_BIT_VIEW_BYTES(12)] = {
        0xfa, 0xbf
    };

    const GblBitView view = {
        .pData = buffer,
        .length = 14,
        .offset = 1
    };

    GblBitView subView;

    GBL_TEST_COMPARE(GblBitView_fromView(&subView, &view, 5, 1),
                     &subView);

    GBL_TEST_CALL(verifyBits_(pSelf, &subView, "01111"));

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(string) {
    char buffer[GBL_BIT_VIEW_BYTES(12)] = {
        0xfa, 0xbf
    };

    const GblBitView view = {
        .pData = buffer,
        .length = 14,
        .offset = 1
    };

    GblStringBuffer strBuffer;

    GblStringBuffer_construct(&strBuffer);

    GBL_TEST_COMPARE(GblBitView_string(&view, &strBuffer, 1, 5),
                     "01111");

    GblStringBuffer_destruct(&strBuffer);

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(uint64) {
    char buffer[GBL_BIT_VIEW_BYTES(12)] = {
        0xfa, 0xbf
    };

    const GblBitView view = {
        .pData = buffer,
        .length = 14,
        .offset = 1
    };

    GblBitView subView;

    GBL_TEST_COMPARE(GblBitView_fromView(&subView, &view, 5, 1),
                     &subView);

    GBL_TEST_CALL(verifyBits_(pSelf, &subView, "01111"));


    GBL_TEST_COMPARE(GblBitView_uint64(&subView),
                     0x1e);

    GBL_TEST_CASE_END;
}

GBL_TEST_REGISTER(fromBuffer,
                  fromView,
                  string,
                  uint64);

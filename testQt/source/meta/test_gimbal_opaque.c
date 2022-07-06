#include "meta/test_gimbal_opaque.h"
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/types/gimbal_variant.h>

static GblType opaqueType_ = GBL_INVALID_TYPE;

GBL_API GblOpaque_test_testCaseInit(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}

GBL_API GblOpaque_test_testCaseFinal(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}

GBL_API GblOpaque_test_init(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}

GBL_API GblOpaque_test_register(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    opaqueType_ = GblPointer_registerOpaque("CString");
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END();
}

GBL_API GblOpaque_test_variant(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(opaqueType_);

    // Default constructor
    GBL_API_VERIFY_CALL(GblVariant_constructDefault(&v, opaqueType_));
    GBL_TEST_VERIFY(GblVariant_type(&v) == opaqueType_);
    GBL_TEST_COMPARE(GblVariant_getPointer(&v), NULL);

    // Copy Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));

    // Move Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructMove(&v2, &v));
    GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Utility / Value Copy Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructOpaque(&v, opaqueType_, "Hi"));
    GBL_TEST_COMPARE(strcmp(GblVariant_getPointer(&v), "Hi"), 0); // Utility / Get Value
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Value Move Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructValueMove(&v, opaqueType_, "Bye"));
    GBL_TEST_COMPARE(strcmp(GblVariant_getPointer(&v), "Bye"), 0); // Utility / Get Value

    // Utility / Value Set Copy
    GBL_API_VERIFY_CALL(GblVariant_setOpaque(&v, opaqueType_, "Shy"));
    GBL_TEST_COMPARE(strcmp(GblVariant_getPointer(&v), "Shy"), 0); // Utility / Get Value

    // Value Set Move
    GBL_API_VERIFY_CALL(GblVariant_setValueMove(&v, opaqueType_, "Fly"));
    GBL_TEST_COMPARE(strcmp(GblVariant_getPointer(&v), "Fly"), 0); // Utility / Get Value

    // Value Get Move
    const char* pValue = NULL;;
    GBL_API_VERIFY_CALL(GblVariant_getValueMove(&v, &pValue));
    GBL_TEST_COMPARE(strcmp(pValue, "Fly"), 0);

    // Value Get Peek
    GblVariant_setOpaque(&v, opaqueType_, "DIE");
    GBL_API_VERIFY_CALL(GblVariant_getValuePeek(&v, &pValue));
    GBL_TEST_COMPARE(strcmp(pValue, "DIE"), 0);

    // Destructor
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_TEST_COMPARE(refCountBegin, GblType_classRefCount(opaqueType_));
    GBL_API_END();
}

GBL_API GblOpaque_test_conversions(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GblVariant v1, v2;

    GBL_API_VERIFY_CALL(GblVariant_constructOpaque(&v1, opaqueType_, "Test"));
    GBL_API_VERIFY_CALL(GblVariant_constructDefault(&v2, GBL_POINTER_TYPE));

    GBL_TEST_VERIFY(GblVariant_canConvert(opaqueType_, GBL_POINTER_TYPE));

    GBL_API_VERIFY_CALL(GblVariant_convert(&v1, &v2));

    GBL_TEST_COMPARE(strcmp(GblVariant_getPointer(&v2), "Test"), 0);

    GBL_TEST_VERIFY(GblVariant_equals(&v1, &v2));

    GBL_TEST_VERIFY(!GblVariant_canConvert(GBL_POINTER_TYPE, opaqueType_));

    GBL_TEST_VERIFY(GblVariant_canConvert(opaqueType_, GBL_STRING_TYPE));

    GBL_API_VERIFY_CALL(GblVariant_destruct(&v1));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));

    GBL_API_END();
}

GBL_API GblOpaque_test_final(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_TEST_COMPARE(GblType_classRefCount(opaqueType_), 0);
    GBL_API_END();
}

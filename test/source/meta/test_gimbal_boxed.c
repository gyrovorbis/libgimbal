#include "meta/test_gimbal_boxed.h"
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/types/gimbal_variant.h>

static GblType boxedType_ = GBL_INVALID_TYPE;

GBL_RESULT GblBox_copy_(void* pBoxed, void** ppNewBoxed) {
    GBL_API_BEGIN(NULL);
    *ppNewBoxed = (void*)GblStringRef_acquire(pBoxed);
    GBL_API_END();
}

GBL_RESULT GblBox_free_(void* pBoxed) {
    GBL_API_BEGIN(NULL);
    GblStringRef_release(pBoxed);
    GBL_API_END();
}

GBL_API GblBoxed_test_testCaseInit(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}

GBL_API GblBoxed_test_testCaseFinal(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}

GBL_API GblBoxed_test_init(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}

GBL_API GblBoxed_test_register(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    static const GblBoxedClassVTable vTable = {
        GblBox_copy_,
        GblBox_free_
    };
    boxedType_ = GblBoxed_register("Box",
                                   &vTable);
    GBL_API_VERIFY_LAST_RECORD();

    GBL_VERIFY(GBL_BOXED_TYPE_CHECK(boxedType_));
    GBL_API_END();
}


GBL_API GblBoxed_test_variant(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(boxedType_);

    // Default constructor
    GBL_API_VERIFY_CALL(GblVariant_constructDefault(&v, boxedType_));
    GBL_VERIFY(GblVariant_type(&v) == boxedType_);
    GBL_COMPARE(GblVariant_getBoxedPeek(&v), NULL);

    // Copy Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    //GBL_VERIFY(GblVariant_equals(&v, &v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Move Value Ctor + Copy Ctor
    GBL_API_VERIFY_CALL(GblVariant_constructBoxedMove(&v, boxedType_, (void*)GblStringRef_create("test")));
    GBL_API_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    GBL_VERIFY(GblVariant_equals(&v, &v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Utility / Value Copy Constructor
    const char* pValue = GblStringRef_create("Hi");
    GBL_API_VERIFY_CALL(GblVariant_constructBoxedCopy(&v, boxedType_, (void*)pValue));
    GBL_COMPARE(strcmp(GblVariant_getBoxedPeek(&v), "Hi"), 0); // Utility / Get Value
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));
    GblStringRef_release(pValue);

    // Value Move Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructValueMove(&v, boxedType_, (void*)GblStringRef_create("Bye")));
    GBL_COMPARE(strcmp(GblVariant_getBoxedPeek(&v), "Bye"), 0); // Utility / Get Value

    // Utility / Value Set Copy
    pValue = GblStringRef_create("Shy");
    GBL_API_VERIFY_CALL(GblVariant_setBoxedCopy(&v, boxedType_, (void*)pValue));
    GBL_COMPARE(strcmp(GblVariant_getBoxedPeek(&v), "Shy"), 0); // Utility / Get Value
    GblStringRef_release(pValue);

    // Value Set Move
    GBL_API_VERIFY_CALL(GblVariant_setValueMove(&v, boxedType_, (void*)GblStringRef_create("Fly")));
    GBL_COMPARE(strcmp(GblVariant_getBoxedPeek(&v), "Fly"), 0); // Utility / Get Value

    // Value Get Move
    GBL_API_VERIFY_CALL(GblVariant_getValueMove(&v, &pValue));
    GBL_COMPARE(strcmp(pValue, "Fly"), 0);
    GblStringRef_release(pValue);

    // Value Get Copy
    GblVariant_setBoxedMove(&v, boxedType_, (void*)GblStringRef_create("DIE"));
    pValue = GblVariant_getBoxedCopy(&v);
    GBL_COMPARE(strcmp(pValue, "DIE"), 0);
    GblStringRef_release(pValue);

    // Destructor
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_COMPARE(refCountBegin, GblType_classRefCount(boxedType_));
    GBL_API_END();
}

GBL_API GblBoxed_test_conversions(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GblVariant v1, v2, v3, v4;

    GBL_API_VERIFY_CALL(GblVariant_constructBoxedMove(&v1, boxedType_, (void*)GblStringRef_create("Test")));
    GBL_API_VERIFY_CALL(GblVariant_constructDefault(&v2, GBL_POINTER_TYPE));

    GBL_VERIFY(GblVariant_canConvert(boxedType_, GBL_POINTER_TYPE));

    GBL_API_VERIFY_CALL(GblVariant_convert(&v1, &v2));

    GBL_COMPARE(strcmp(GblVariant_getPointer(&v2), "Test"), 0);

    GBL_VERIFY(GblVariant_equals(&v1, &v2));

    GBL_VERIFY(GblVariant_canConvert(GBL_POINTER_TYPE, boxedType_));

    GBL_VERIFY(GblVariant_canConvert(boxedType_, GBL_STRING_TYPE));

    GBL_API_VERIFY_CALL(GblVariant_constructBool(&v3, GBL_FALSE));
    GBL_API_VERIFY_CALL(GblVariant_convert(&v1, &v3));
    GBL_VERIFY(GblVariant_equals(&v1, &v3));
    GBL_VERIFY(GblVariant_getBool(&v3) == GBL_TRUE);

    GBL_API_VERIFY_CALL(GblVariant_constructNil(&v4));

    GBL_API_VERIFY_CALL(GblVariant_convert(&v4, &v1));
    GBL_API_VERIFY_CALL(GblVariant_equals(&v1, &v4));
    GBL_COMPARE(GblVariant_getBoxedPeek(&v1), NULL);

    GBL_API_VERIFY_CALL(GblVariant_destruct(&v1));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v3));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v4));

    GBL_API_END();
}

GBL_API GblBoxed_test_final(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_COMPARE(GblType_classRefCount(boxedType_), 0);
    GBL_API_END();
}

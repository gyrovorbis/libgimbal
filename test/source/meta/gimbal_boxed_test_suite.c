#include "meta/gimbal_boxed_test_suite.h"
#include <gimbal/utils/gimbal_test.h>
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/types/gimbal_variant.h>

#define GBL_BOXED_TEST_SUITE_(inst)     ((GblBoxedTestSuite_*)GBL_INSTANCE_PRIVATE(inst, GBL_BOXED_TEST_SUITE_TYPE))

typedef struct GblBoxedTestSuite_ {
    GblType boxedType;
} GblBoxedTestSuite_;

static GBL_RESULT GblBox_copy_(void* pBoxed, void** ppNewBoxed) {
    GBL_API_BEGIN(NULL);
    *ppNewBoxed = (void*)GblStringRef_acquire(pBoxed);
    GBL_API_END();
}

static GBL_RESULT GblBox_free_(void* pBoxed) {
    GBL_API_BEGIN(NULL);
    GblStringRef_release(pBoxed);
    GBL_API_END();
}

static GBL_RESULT GblBoxedTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblBoxedTestSuite_* pSelf_ = GBL_BOXED_TEST_SUITE_(pSelf);
    memset(pSelf_, 0, sizeof(GblBoxedTestSuite_));
    GBL_API_END();
}

static GBL_RESULT GblBoxedTestSuite_final_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblBoxedTestSuite_* pSelf_ = GBL_BOXED_TEST_SUITE_(pSelf);
    GBL_TEST_COMPARE(GblType_classRefCount(pSelf_->boxedType), 0);
    GBL_API_END();
}

static GBL_RESULT GblBoxedTestSuite_register_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblBoxedTestSuite_* pSelf_ = GBL_BOXED_TEST_SUITE_(pSelf);
    static const GblBoxedClassVTable vTable = {
        GblBox_copy_,
        GblBox_free_
    };
    pSelf_->boxedType = GblBoxed_register("Box",
                                   &vTable);
    GBL_API_VERIFY_LAST_RECORD();

    GBL_TEST_VERIFY(GBL_BOXED_TYPE_CHECK(pSelf_->boxedType));
    GBL_API_END();
}


static GBL_RESULT GblBoxedTestSuite_variant_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblBoxedTestSuite_* pSelf_ = GBL_BOXED_TEST_SUITE_(pSelf);
    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(pSelf_->boxedType);

    // Default constructor
    GBL_API_VERIFY_CALL(GblVariant_constructDefault(&v, pSelf_->boxedType));
    GBL_TEST_VERIFY(GblVariant_type(&v) == pSelf_->boxedType);
    GBL_TEST_COMPARE(GblVariant_getBoxedPeek(&v), NULL);

    // Copy Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    //GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Move Value Ctor + Copy Ctor
    GBL_API_VERIFY_CALL(GblVariant_constructBoxedMove(&v, pSelf_->boxedType, (void*)GblStringRef_create("test")));
    GBL_API_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Utility / Value Copy Constructor
    const char* pValue = GblStringRef_create("Hi");
    GBL_API_VERIFY_CALL(GblVariant_constructBoxedCopy(&v, pSelf_->boxedType, (void*)pValue));
    GBL_TEST_COMPARE(strcmp(GblVariant_getBoxedPeek(&v), "Hi"), 0); // Utility / Get Value
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));
    GblStringRef_release(pValue);

    // Value Move Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructValueMove(&v, pSelf_->boxedType, (void*)GblStringRef_create("Bye")));
    GBL_TEST_COMPARE(strcmp(GblVariant_getBoxedPeek(&v), "Bye"), 0); // Utility / Get Value

    // Utility / Value Set Copy
    pValue = GblStringRef_create("Shy");
    GBL_API_VERIFY_CALL(GblVariant_setBoxedCopy(&v, pSelf_->boxedType, (void*)pValue));
    GBL_TEST_COMPARE(strcmp(GblVariant_getBoxedPeek(&v), "Shy"), 0); // Utility / Get Value
    GblStringRef_release(pValue);

    // Value Set Move
    GBL_API_VERIFY_CALL(GblVariant_setValueMove(&v, pSelf_->boxedType, (void*)GblStringRef_create("Fly")));
    GBL_TEST_COMPARE(strcmp(GblVariant_getBoxedPeek(&v), "Fly"), 0); // Utility / Get Value

    // Value Get Move
    GBL_API_VERIFY_CALL(GblVariant_getValueMove(&v, &pValue));
    GBL_TEST_COMPARE(strcmp(pValue, "Fly"), 0);
    GblStringRef_release(pValue);

    // Value Get Copy
    GblVariant_setBoxedMove(&v, pSelf_->boxedType, (void*)GblStringRef_create("DIE"));
    pValue = GblVariant_getBoxedCopy(&v);
    GBL_TEST_COMPARE(strcmp(pValue, "DIE"), 0);
    GblStringRef_release(pValue);

    // Destructor
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_TEST_COMPARE(refCountBegin, GblType_classRefCount(pSelf_->boxedType));
    GBL_API_END();
}

static GBL_RESULT GblBoxedTestSuite_conversions_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblBoxedTestSuite_* pSelf_ = GBL_BOXED_TEST_SUITE_(pSelf);
    GblVariant v1, v2, v3, v4;

    GBL_API_VERIFY_CALL(GblVariant_constructBoxedMove(&v1, pSelf_->boxedType, (void*)GblStringRef_create("Test")));
    GBL_API_VERIFY_CALL(GblVariant_constructDefault(&v2, GBL_POINTER_TYPE));

    GBL_TEST_VERIFY(GblVariant_canConvert(pSelf_->boxedType, GBL_POINTER_TYPE));

    GBL_API_VERIFY_CALL(GblVariant_convert(&v1, &v2));

    GBL_TEST_COMPARE(strcmp(GblVariant_getPointer(&v2), "Test"), 0);

    GBL_TEST_VERIFY(GblVariant_equals(&v1, &v2));

    GBL_TEST_VERIFY(GblVariant_canConvert(GBL_POINTER_TYPE, pSelf_->boxedType));

    GBL_TEST_VERIFY(GblVariant_canConvert(pSelf_->boxedType, GBL_STRING_TYPE));

    GBL_API_VERIFY_CALL(GblVariant_constructBool(&v3, GBL_FALSE));
    GBL_API_VERIFY_CALL(GblVariant_convert(&v1, &v3));
    GBL_TEST_VERIFY(GblVariant_equals(&v1, &v3));
    GBL_TEST_VERIFY(GblVariant_getBool(&v3) == GBL_TRUE);

    GBL_API_VERIFY_CALL(GblVariant_constructNil(&v4));

    GBL_API_VERIFY_CALL(GblVariant_convert(&v4, &v1));
    GBL_API_VERIFY_CALL(GblVariant_equals(&v1, &v4));
    GBL_TEST_COMPARE(GblVariant_getBoxedPeek(&v1), NULL);

    GBL_API_VERIFY_CALL(GblVariant_destruct(&v1));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v3));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v4));

    GBL_API_END();
}

GBL_EXPORT GblType GblBoxedTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "register",       GblBoxedTestSuite_register_        },
        { "variant",        GblBoxedTestSuite_variant_         },
        { "conversions",    GblBoxedTestSuite_conversions_     },
        { NULL,             NULL                               }
    };

    const static GblTestSuiteClassVTable vTable = {
        .pFnSuiteInit   = GblBoxedTestSuite_init_,
        .pFnSuiteFinal  = GblBoxedTestSuite_final_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_API_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("BoxedTestSuite"),
                                     &vTable,
                                     sizeof(GblBoxedTestSuite),
                                     sizeof(GblBoxedTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_API_VERIFY_LAST_RECORD();
        GBL_API_END_BLOCK();
    }
    return type;
}

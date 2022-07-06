#include "meta/gimbal_opaque_test_suite.h"
#include <gimbal/utils/gimbal_test.h>
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/types/gimbal_variant.h>

#define GBL_OPAQUE_TEST_SUITE_(inst)    ((GblOpaqueTestSuite_*)GBL_INSTANCE_PRIVATE(inst, GBL_OPAQUE_TEST_SUITE_TYPE))

typedef struct GblOpaqueTestSuite_ {
    GblType opaqueType;
} GblOpaqueTestSuite_;

static GBL_RESULT GblOpaqueTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblOpaqueTestSuite_* pSelf_ = GBL_OPAQUE_TEST_SUITE_(pSelf);
    memset(pSelf_, 0, sizeof(GblOpaqueTestSuite_));
    GBL_API_END();
}

static GBL_RESULT GblOpaqueTestSuite_final_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblOpaqueTestSuite_* pSelf_ = GBL_OPAQUE_TEST_SUITE_(pSelf);
    GBL_TEST_COMPARE(GblType_classRefCount(pSelf_->opaqueType), 0);
    GBL_API_END();
}

static GBL_RESULT GblOpaqueTestSuite_register_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblOpaqueTestSuite_* pSelf_ = GBL_OPAQUE_TEST_SUITE_(pSelf);
    pSelf_->opaqueType = GblPointer_registerOpaque("CString");
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END();
}

static GBL_RESULT GblOpaqueTestSuite_variant_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GblOpaqueTestSuite_* pSelf_ = GBL_OPAQUE_TEST_SUITE_(pSelf);
    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(pSelf_->opaqueType);

    // Default constructor
    GBL_API_VERIFY_CALL(GblVariant_constructDefault(&v, pSelf_->opaqueType));
    GBL_TEST_VERIFY(GblVariant_type(&v) == pSelf_->opaqueType);
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
    GBL_API_VERIFY_CALL(GblVariant_constructOpaque(&v, pSelf_->opaqueType, "Hi"));
    GBL_TEST_COMPARE(strcmp(GblVariant_getPointer(&v), "Hi"), 0); // Utility / Get Value
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Value Move Constructor
    GBL_API_VERIFY_CALL(GblVariant_constructValueMove(&v, pSelf_->opaqueType, "Bye"));
    GBL_TEST_COMPARE(strcmp(GblVariant_getPointer(&v), "Bye"), 0); // Utility / Get Value

    // Utility / Value Set Copy
    GBL_API_VERIFY_CALL(GblVariant_setOpaque(&v, pSelf_->opaqueType, "Shy"));
    GBL_TEST_COMPARE(strcmp(GblVariant_getPointer(&v), "Shy"), 0); // Utility / Get Value

    // Value Set Move
    GBL_API_VERIFY_CALL(GblVariant_setValueMove(&v, pSelf_->opaqueType, "Fly"));
    GBL_TEST_COMPARE(strcmp(GblVariant_getPointer(&v), "Fly"), 0); // Utility / Get Value

    // Value Get Move
    const char* pValue = NULL;;
    GBL_API_VERIFY_CALL(GblVariant_getValueMove(&v, &pValue));
    GBL_TEST_COMPARE(strcmp(pValue, "Fly"), 0);

    // Value Get Peek
    GblVariant_setOpaque(&v, pSelf_->opaqueType, "DIE");
    GBL_API_VERIFY_CALL(GblVariant_getValuePeek(&v, &pValue));
    GBL_TEST_COMPARE(strcmp(pValue, "DIE"), 0);

    // Destructor
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_TEST_COMPARE(refCountBegin, GblType_classRefCount(pSelf_->opaqueType));
    GBL_API_END();
}

static GBL_RESULT GblOpaqueTestSuite_conversions_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblOpaqueTestSuite_* pSelf_ = GBL_OPAQUE_TEST_SUITE_(pSelf);

    GblVariant v1, v2;

    GBL_API_VERIFY_CALL(GblVariant_constructOpaque(&v1, pSelf_->opaqueType, "Test"));
    GBL_API_VERIFY_CALL(GblVariant_constructDefault(&v2, GBL_POINTER_TYPE));

    GBL_TEST_VERIFY(GblVariant_canConvert(pSelf_->opaqueType, GBL_POINTER_TYPE));

    GBL_API_VERIFY_CALL(GblVariant_convert(&v1, &v2));

    GBL_TEST_COMPARE(strcmp(GblVariant_getPointer(&v2), "Test"), 0);

    GBL_TEST_VERIFY(GblVariant_equals(&v1, &v2));

    GBL_TEST_VERIFY(!GblVariant_canConvert(GBL_POINTER_TYPE, pSelf_->opaqueType));

    GBL_TEST_VERIFY(GblVariant_canConvert(pSelf_->opaqueType, GBL_STRING_TYPE));

    GBL_API_VERIFY_CALL(GblVariant_destruct(&v1));
    GBL_API_VERIFY_CALL(GblVariant_destruct(&v2));

    GBL_API_END();
}

GBL_EXPORT GblType GblOpaqueTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "register",       GblOpaqueTestSuite_register_        },
        { "variant",        GblOpaqueTestSuite_variant_         },
        { "conversions",    GblOpaqueTestSuite_conversions_     },
        { NULL,             NULL                                }
    };

    const static GblTestSuiteClassVTable vTable = {
        .pFnSuiteInit   = GblOpaqueTestSuite_init_,
        .pFnSuiteFinal  = GblOpaqueTestSuite_final_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_API_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("OpaqueTestSuite"),
                                     &vTable,
                                     sizeof(GblOpaqueTestSuite),
                                     sizeof(GblOpaqueTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_API_VERIFY_LAST_RECORD();
        GBL_API_END_BLOCK();
    }
    return type;
}

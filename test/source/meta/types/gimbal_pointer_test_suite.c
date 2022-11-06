#include "meta/types/gimbal_pointer_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/core/gimbal_ctx.h>
#include <gimbal/meta/types/gimbal_variant.h>

#define GBL_POINTER_TEST_SUITE_(inst)    ((GblPointerTestSuite_*)GBL_INSTANCE_PRIVATE(inst, GBL_POINTER_TEST_SUITE_TYPE))

typedef struct GblPointerTestSuite_ {
    GblType pointerType;
} GblPointerTestSuite_;

static GBL_RESULT GblPointerTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblPointerTestSuite_* pSelf_ = GBL_POINTER_TEST_SUITE_(pSelf);
    memset(pSelf_, 0, sizeof(GblPointerTestSuite_));
    GBL_CTX_END();
}

static GBL_RESULT GblPointerTestSuite_final_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblPointerTestSuite_* pSelf_ = GBL_POINTER_TEST_SUITE_(pSelf);
    GBL_TEST_COMPARE(GblType_classRefCount(pSelf_->pointerType), 0);
    GblType_unregister(pSelf_->pointerType);
    GBL_CTX_END();
}

static GBL_RESULT GblPointerTestSuite_register_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblPointerTestSuite_* pSelf_ = GBL_POINTER_TEST_SUITE_(pSelf);
    pSelf_->pointerType = GblPointer_register("CString");
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END();
}

static GBL_RESULT GblPointerTestSuite_variant_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblPointerTestSuite_* pSelf_ = GBL_POINTER_TEST_SUITE_(pSelf);
    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(pSelf_->pointerType);

    // Default constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructDefault(&v, pSelf_->pointerType));
    GBL_TEST_VERIFY(GblVariant_typeOf(&v) == pSelf_->pointerType);
    GBL_TEST_COMPARE(GblVariant_getPointer(&v), NULL);

    // Copy Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));

    // Move Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructMove(&v2, &v));
    GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Utility / Value Copy Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructPointer(&v, pSelf_->pointerType, "Hi"));
    GBL_TEST_COMPARE(strcmp(GblVariant_getPointer(&v), "Hi"), 0); // Utility / Get Value
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Value Move Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueMove(&v, pSelf_->pointerType, "Bye"));
    GBL_TEST_COMPARE(strcmp(GblVariant_getPointer(&v), "Bye"), 0); // Utility / Get Value

    // Utility / Value Set Copy
    GBL_CTX_VERIFY_CALL(GblVariant_setPointer(&v, pSelf_->pointerType, "Shy"));
    GBL_TEST_COMPARE(strcmp(GblVariant_getPointer(&v), "Shy"), 0); // Utility / Get Value

    // Value Set Move
    GBL_CTX_VERIFY_CALL(GblVariant_setValueMove(&v, pSelf_->pointerType, "Fly"));
    GBL_TEST_COMPARE(strcmp(GblVariant_getPointer(&v), "Fly"), 0); // Utility / Get Value

    // Value Get Move
    const char* pValue = NULL;;
    GBL_CTX_VERIFY_CALL(GblVariant_getValueMove(&v, &pValue));
    GBL_TEST_COMPARE(strcmp(pValue, "Fly"), 0);

    // Value Get Peek
    GblVariant_setPointer(&v, pSelf_->pointerType, "DIE");
    GBL_CTX_VERIFY_CALL(GblVariant_getValuePeek(&v, &pValue));
    GBL_TEST_COMPARE(strcmp(pValue, "DIE"), 0);

    // Destructor
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_TEST_COMPARE(refCountBegin, GblType_classRefCount(pSelf_->pointerType));
    GBL_CTX_END();
}

static GBL_RESULT GblPointerTestSuite_conversions_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblPointerTestSuite_* pSelf_ = GBL_POINTER_TEST_SUITE_(pSelf);

    GblVariant v1, v2;

    GBL_CTX_VERIFY_CALL(GblVariant_constructPointer(&v1, pSelf_->pointerType, "Test"));
    GBL_CTX_VERIFY_CALL(GblVariant_constructDefault(&v2, GBL_POINTER_TYPE));

    GBL_TEST_VERIFY(GblVariant_canConvert(pSelf_->pointerType, GBL_POINTER_TYPE));

    GBL_CTX_VERIFY_CALL(GblVariant_convert(&v1, &v2));

    GBL_TEST_COMPARE(strcmp(GblVariant_getPointer(&v2), "Test"), 0);

    GBL_TEST_VERIFY(GblVariant_equals(&v1, &v2));

    GBL_TEST_VERIFY(!GblVariant_canConvert(GBL_POINTER_TYPE, pSelf_->pointerType));

    GBL_TEST_VERIFY(GblVariant_canConvert(pSelf_->pointerType, GBL_STRING_TYPE));

    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v1));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));

    GBL_CTX_END();
}

GBL_EXPORT GblType GblPointerTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "register",       GblPointerTestSuite_register_        },
        { "variant",        GblPointerTestSuite_variant_         },
        { "conversions",    GblPointerTestSuite_conversions_     },
        { NULL,             NULL                                }
    };

    const static GblTestSuiteClassVTable vTable = {
        .pFnSuiteInit   = GblPointerTestSuite_init_,
        .pFnSuiteFinal  = GblPointerTestSuite_final_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("GblPointerTestSuite"),
                                     &vTable,
                                     sizeof(GblPointerTestSuite),
                                     sizeof(GblPointerTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }
    return type;
}

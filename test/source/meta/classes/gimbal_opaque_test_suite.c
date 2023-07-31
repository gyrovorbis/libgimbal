#include "meta/classes/gimbal_opaque_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/core/gimbal_ctx.h>
#include <gimbal/meta/types/gimbal_variant.h>

#define GBL_OPAQUE_TEST_SUITE_(inst)     (GBL_PRIVATE(GblOpaqueTestSuite, inst))

typedef struct GblOpaqueTestSuite_ {
    GblType opaqueType;
} GblOpaqueTestSuite_;

static GBL_RESULT GblOpaque_copy_(void* pOpaque, void** ppNewOpaque) {
    GBL_CTX_BEGIN(NULL);
    *ppNewOpaque = (void*)GblStringRef_acquire(pOpaque);
    GBL_CTX_END();
}

static GBL_RESULT GblOpaque_free_(void* pOpaque) {
    GBL_CTX_BEGIN(NULL);
    GblStringRef_release(pOpaque);
    GBL_CTX_END();
}

static GBL_RESULT GblOpaqueTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblOpaqueTestSuite_* pSelf_ = GBL_OPAQUE_TEST_SUITE_(pSelf);
    memset(pSelf_, 0, sizeof(GblOpaqueTestSuite_));
    GBL_CTX_END();
}

static GBL_RESULT GblOpaqueTestSuite_final_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblOpaqueTestSuite_* pSelf_ = GBL_OPAQUE_TEST_SUITE_(pSelf);
    GBL_TEST_COMPARE(GblType_classRefCount(pSelf_->opaqueType), 0);
    GblType_unregister(pSelf_->opaqueType);
    GBL_CTX_END();
}

static GBL_RESULT GblOpaqueTestSuite_register_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblOpaqueTestSuite_* pSelf_ = GBL_OPAQUE_TEST_SUITE_(pSelf);
    static const GblOpaqueVTable vTable = {
        GblOpaque_copy_,
        GblOpaque_free_
    };
    pSelf_->opaqueType = GblOpaque_register("TestOpaque",
                                            &vTable);
    GBL_CTX_VERIFY_LAST_RECORD();

    GBL_CTX_VERIFY_TYPE(pSelf_->opaqueType, GBL_OPAQUE_TYPE);
    GBL_CTX_END();
}


static GBL_RESULT GblOpaqueTestSuite_variant_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblOpaqueTestSuite_* pSelf_ = GBL_OPAQUE_TEST_SUITE_(pSelf);
    GblVariant v = GBL_VARIANT_INIT;
    GBL_VARIANT(v2);
    const GblRefCount refCountBegin = GblType_classRefCount(pSelf_->opaqueType);

    // Default constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructDefault(&v, pSelf_->opaqueType));
    GBL_TEST_VERIFY(GblVariant_typeOf(&v) == pSelf_->opaqueType);
    GBL_TEST_COMPARE(GblVariant_opaquePeek(&v), NULL);

    // Copy Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    //GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Move Value Ctor + Copy Ctor
    GBL_CTX_VERIFY_CALL(GblVariant_constructOpaqueMove(&v, pSelf_->opaqueType, (void*)GblStringRef_create("test")));
    GBL_CTX_VERIFY_CALL(GblVariant_constructCopy(&v2, &v));
    GBL_TEST_VERIFY(GblVariant_equals(&v, &v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Utility / Value Copy Constructor
    const char* pValue = GblStringRef_create("Hi");
    GBL_CTX_VERIFY_CALL(GblVariant_constructOpaqueCopy(&v, pSelf_->opaqueType, (void*)pValue));
    GBL_TEST_COMPARE(strcmp(GblVariant_opaquePeek(&v), "Hi"), 0); // Utility / Get Value
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));
    GblStringRef_release(pValue);

    // Value Move Constructor
    GBL_CTX_VERIFY_CALL(GblVariant_constructValueMove(&v, pSelf_->opaqueType, (void*)GblStringRef_create("Bye")));
    GBL_TEST_COMPARE(strcmp(GblVariant_opaquePeek(&v), "Bye"), 0); // Utility / Get Value

    // Utility / Value Set Copy
    pValue = GblStringRef_create("Shy");
    GBL_CTX_VERIFY_CALL(GblVariant_setOpaqueCopy(&v, pSelf_->opaqueType, (void*)pValue));
    GBL_TEST_COMPARE(strcmp(GblVariant_opaquePeek(&v), "Shy"), 0); // Utility / Get Value
    GblStringRef_release(pValue);

    // Value Set Move
    GBL_CTX_VERIFY_CALL(GblVariant_setValueMove(&v, pSelf_->opaqueType, (void*)GblStringRef_create("Fly")));
    GBL_TEST_COMPARE(strcmp(GblVariant_opaquePeek(&v), "Fly"), 0); // Utility / Get Value

    // Value Get Move
    GBL_CTX_VERIFY_CALL(GblVariant_valueMove(&v, &pValue));
    GBL_TEST_COMPARE(strcmp(pValue, "Fly"), 0);
    GblStringRef_release(pValue);

    // Value Get Copy
    GblVariant_setOpaqueMove(&v, pSelf_->opaqueType, (void*)GblStringRef_create("DIE"));
    pValue = GblVariant_opaqueCopy(&v);
    GBL_TEST_COMPARE(strcmp(pValue, "DIE"), 0);
    GblStringRef_release(pValue);

    // Destructor
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v));

    // Reference leak check
    GBL_TEST_COMPARE(refCountBegin, GblType_classRefCount(pSelf_->opaqueType));
    GBL_CTX_END();
}

static GBL_RESULT GblOpaqueTestSuite_conversions_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblOpaqueTestSuite_* pSelf_ = GBL_OPAQUE_TEST_SUITE_(pSelf);
    GblVariant v1, v2, v3, v4;

    GBL_CTX_VERIFY_CALL(GblVariant_constructOpaqueMove(&v1, pSelf_->opaqueType, (void*)GblStringRef_create("Test")));
    GBL_CTX_VERIFY_CALL(GblVariant_constructDefault(&v2, GBL_POINTER_TYPE));

    GBL_TEST_VERIFY(GblVariant_canConvert(pSelf_->opaqueType, GBL_POINTER_TYPE));

    GBL_CTX_VERIFY_CALL(GblVariant_convert(&v1, &v2));

    GBL_TEST_COMPARE(strcmp(GblVariant_pointer(&v2), "Test"), 0);

    GBL_TEST_VERIFY(GblVariant_equals(&v1, &v2));

    GBL_TEST_VERIFY(GblVariant_canConvert(GBL_POINTER_TYPE, pSelf_->opaqueType));

    GBL_TEST_VERIFY(GblVariant_canConvert(pSelf_->opaqueType, GBL_STRING_TYPE));

    GBL_CTX_VERIFY_CALL(GblVariant_constructBool(&v3, GBL_FALSE));
    GBL_CTX_VERIFY_CALL(GblVariant_convert(&v1, &v3));
    GBL_TEST_VERIFY(GblVariant_equals(&v1, &v3));
    GBL_TEST_VERIFY(GblVariant_bool(&v3) == GBL_TRUE);

    GBL_CTX_VERIFY_CALL(GblVariant_constructNil(&v4));

    GBL_CTX_VERIFY_CALL(GblVariant_convert(&v4, &v1));
    GBL_CTX_VERIFY_CALL(GblVariant_equals(&v1, &v4));
    GBL_TEST_COMPARE(GblVariant_opaquePeek(&v1), NULL);

    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v1));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v2));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v3));
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(&v4));

    GBL_CTX_END();
}

GBL_EXPORT GblType GblOpaqueTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "register",       GblOpaqueTestSuite_register_        },
        { "variant",        GblOpaqueTestSuite_variant_         },
        { "conversions",    GblOpaqueTestSuite_conversions_     },
        { NULL,             NULL                                }
    };

    const static GblTestSuiteVTable vTable = {
        .pFnSuiteInit   = GblOpaqueTestSuite_init_,
        .pFnSuiteFinal  = GblOpaqueTestSuite_final_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("GblOpaqueTestSuite"),
                                     &vTable,
                                     sizeof(GblOpaqueTestSuite),
                                     sizeof(GblOpaqueTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }
    return type;
}

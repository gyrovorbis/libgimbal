#include "meta/instances/gimbal_instance_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/meta/instances/gimbal_instance.h>

#define GBL_INSTANCE_TEST_SUITE_(inst)      ((GblInstanceTestSuite_*)GBL_INSTANCE_PRIVATE(inst, GBL_INSTANCE_TEST_SUITE_TYPE))

// instantiate from: 1) non instantiable
//                   2) type with private data
// swizzling incompatible class
// instance GET CLASS
// instance TRY CLASS
// instance vcall

typedef struct GblInstanceTestSuite_ {
    size_t          instanceStartInstanceRefCount;
    size_t          instanceStartClassRefCount;
    GblInstance*    pInstance;
} GblInstanceTestSuite_;

GBL_EXPORT GBL_RESULT GblInstanceTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblInstanceTestSuite_* pSelf_           = GBL_INSTANCE_TEST_SUITE_(pSelf);
    memset(pSelf_, 0, sizeof(GblInstanceTestSuite_));
    pSelf_->instanceStartInstanceRefCount   = GblType_instanceRefCount(GBL_INSTANCE_TYPE);
    pSelf_->instanceStartClassRefCount      = GblType_classRefCount(GBL_INSTANCE_TYPE);

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblInstanceTestSuite_createInvalid(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_TEST_EXPECT_ERROR();
    GblInstance* pInst = GblInstance_create(GBL_INVALID_TYPE);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_INVALID_TYPE);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_TEST_COMPARE(pInst, NULL);

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblInstanceTestSuite_createWithClassInvalid(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_TEST_EXPECT_ERROR();

    GblInstance* pInst = GblInstance_createWithClass(NULL);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_INVALID_TYPE);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_TEST_COMPARE(pInst, NULL);

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblInstanceTestSuite_destroyNull(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_COMPARE(GblInstance_destroy(NULL), 0);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblInstanceTestSuite_constructInvalid(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GblInstance instance;

    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblInstance_construct(&instance, GBL_INVALID_TYPE),
                     GBL_RESULT_ERROR_INVALID_TYPE);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblInstance_construct(NULL, GBL_INSTANCE_TYPE),
                     GBL_RESULT_ERROR_INVALID_POINTER);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblInstanceTestSuite_constructWithClassInvalid(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GblInstance instance;
    GblClass    klass;

    GBL_TEST_EXPECT_ERROR();

    GBL_CTX_VERIFY_CALL(GblClass_constructFloating(&klass, GBL_INSTANCE_TYPE));

    GBL_TEST_COMPARE(GblInstance_constructWithClass(&instance, NULL),
                     GBL_RESULT_ERROR_INVALID_POINTER);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblInstance_constructWithClass(NULL, &klass),
                     GBL_RESULT_ERROR_INVALID_POINTER);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_CTX_VERIFY_CALL(GblClass_destructFloating(&klass));

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblInstanceTestSuite_destructNull(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_COMPARE(GblInstance_destruct(NULL), 0);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblInstanceTestSuite_checkNull(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_VERIFY(GblInstance_check(NULL, GBL_INVALID_TYPE));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblInstanceTestSuite_checkInvalid(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_VERIFY(!GblInstance_check(NULL, GBL_INSTANCE_TYPE));
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_INVALID_POINTER);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblInstanceTestSuite_castNull(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_COMPARE(GblInstance_cast(NULL, GBL_INVALID_TYPE), NULL);
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblInstanceTestSuite_castInvalid(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_TEST_EXPECT_ERROR();
    GBL_TEST_COMPARE(GblInstance_cast(NULL, GBL_INSTANCE_TYPE), NULL);
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_INVALID_POINTER);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblInstanceTestSuite_tryNull(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_TEST_COMPARE(GblInstance_try(NULL, GBL_INVALID_TYPE), NULL);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(GblInstance_try(NULL, GBL_INSTANCE_TYPE), NULL);
    GBL_CTX_VERIFY_LAST_RECORD();

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblInstanceTestSuite_publicNull(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_TEST_COMPARE(GblInstance_public(NULL, GBL_INVALID_TYPE), NULL);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(GBL_INSTANCE_PUBLIC(NULL, GBL_INVALID_TYPE), NULL);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(GblInstance_public(NULL, GBL_INSTANCE_TYPE), NULL);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(GBL_INSTANCE_PUBLIC(NULL, GBL_INSTANCE_TYPE), NULL);
    GBL_CTX_VERIFY_LAST_RECORD();

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblInstanceTestSuite_privateNull(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_TEST_COMPARE(GblInstance_private(NULL, GBL_INVALID_TYPE), NULL);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(GBL_INSTANCE_PRIVATE(NULL, GBL_INVALID_TYPE), NULL);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(GblInstance_private(NULL, GBL_INSTANCE_TYPE), NULL);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(GBL_INSTANCE_PRIVATE(NULL, GBL_INSTANCE_TYPE), NULL);
    GBL_CTX_VERIFY_LAST_RECORD();

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblInstanceTestSuite_typeOfNull(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_TEST_COMPARE(GblInstance_typeOf(NULL), (uintptr_t)NULL);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_TEST_COMPARE(GBL_INSTANCE_TYPEOF(NULL), (uintptr_t)NULL);
    GBL_CTX_VERIFY_LAST_RECORD();

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblInstanceTestSuite_sizeNull(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_COMPARE(GblInstance_size(NULL), 0);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblInstanceTestSuite_privateSizeNull(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_COMPARE(GblInstance_privateSize(NULL), 0);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblInstanceTestSuite_totalSizeNull(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_COMPARE(GblInstance_totalSize(NULL), 0);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblInstanceTestSuite_classInvalid(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_COMPARE(GblInstance_class(NULL), NULL);
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblInstanceTestSuite_swizzleClassInvalid(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GblInstance instance;
    GblClass    klass;

    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblInstance_swizzleClass(&instance, NULL),
                     GBL_RESULT_ERROR_INVALID_POINTER);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblInstance_swizzleClass(NULL, &klass),
                     GBL_RESULT_ERROR_INVALID_POINTER);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblInstanceTestSuite_sinkClassInvalid(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblInstance_sinkClass(NULL),
                     GBL_RESULT_ERROR_INVALID_POINTER);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblInstanceTestSuite_floatClassInvalid(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);

    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblInstance_floatClass(NULL),
                     GBL_RESULT_ERROR_INVALID_POINTER);
    GBL_CTX_CLEAR_LAST_RECORD();

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblInstanceTestSuite_final_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblInstanceTestSuite_* pSelf_ = GBL_INSTANCE_TEST_SUITE_(pSelf);
    GBL_TEST_COMPARE(GblType_instanceRefCount(GBL_INSTANCE_TYPE),   pSelf_->instanceStartInstanceRefCount);
    GBL_TEST_COMPARE(GblType_classRefCount(GBL_INSTANCE_TYPE),      pSelf_->instanceStartClassRefCount);

    GBL_CTX_END();
}

GBL_EXPORT GblType GblInstanceTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "createInvalid",              GblInstanceTestSuite_createInvalid              },
        { "createWithClassInvalid",     GblInstanceTestSuite_createWithClassInvalid     },
        { "destroyNull",                GblInstanceTestSuite_destroyNull                },
        { "constructInvalid",           GblInstanceTestSuite_constructInvalid           },
        { "constructWithClassInvalid",  GblInstanceTestSuite_constructWithClassInvalid  },
        { "destructNull",               GblInstanceTestSuite_destructNull               },
        { "checkNull",                  GblInstanceTestSuite_checkNull                  },
        { "checkInvalid",               GblInstanceTestSuite_checkInvalid               },
        { "castNull",                   GblInstanceTestSuite_castNull                   },
        { "castInvalid",                GblInstanceTestSuite_castInvalid                },
        { "tryNull",                    GblInstanceTestSuite_tryNull                    },
        { "publicNull",                 GblInstanceTestSuite_publicNull                 },
        { "privateNull",                GblInstanceTestSuite_privateNull                },
        { "typeOfNull",                 GblInstanceTestSuite_typeOfNull                 },
        { "sizeNull",                   GblInstanceTestSuite_sizeNull                   },
        { "privateSizeNull",            GblInstanceTestSuite_privateSizeNull            },
        { "totalSizeNull",              GblInstanceTestSuite_totalSizeNull              },
        { "classInvalid",               GblInstanceTestSuite_classInvalid               },
        { "swizzleClassInvalid",        GblInstanceTestSuite_swizzleClassInvalid        },
        { "sinkClassInvalid",           GblInstanceTestSuite_sinkClassInvalid           },
        { "floatClassInvalid",          GblInstanceTestSuite_floatClassInvalid          },
        { NULL,                         NULL                                            }
    };

    const static GblTestSuiteClassVTable vTable = {
        .pFnSuiteInit   = GblInstanceTestSuite_init_,
        .pFnSuiteFinal  = GblInstanceTestSuite_final_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("InstanceTestSuite"),
                                     &vTable,
                                     sizeof(GblInstanceTestSuite),
                                     sizeof(GblInstanceTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }
    return type;
}

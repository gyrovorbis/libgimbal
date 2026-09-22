#ifndef GIMBAL_VARIANT_TEST_SUITE_H
#define GIMBAL_VARIANT_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_VARIANT_TEST_SUITE_TYPE           (GblVariantTestSuite_type())
#define GBL_VARIANT_TEST_SUTE_STRUCT          GblVariantTestSuite
#define GBL_VARIANT_TEST_SUITE_CLASS_STRUCT   GblVariantTestSuiteClass
#define GBL_VARIANT_TEST_SUITE(inst)          (GBL_CAST(GblVariantTestSuite, inst))
#define GBL_VARIANT_TEST_SUITE_CLASS(klass)   (GBL_CLASS_CAST(GblVariantTestSuite, klass))
#define GBL_VARIANT_TEST_SUITE_CLASSOF(inst)  (GBL_CLASSOF(GblVariantTestSuite, inst))

GBL_DECLS_BEGIN

typedef struct GblVariantTestSuiteClass {
    GblTestSuiteClass   base;
} GblVariantTestSuiteClass;

typedef struct GblVariantTestSuite {
    union {
        GblVariantTestSuiteClass*   pClass;
        GblTestSuite                base;
    };
} GblVariantTestSuite;

GBL_EXPORT GblType GblVariantTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_VARIANT_TEST_SUITE_H

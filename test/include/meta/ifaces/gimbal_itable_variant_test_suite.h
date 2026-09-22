#ifndef GIMBAL_ITABLE_VARIANT_TEST_SUITE_H
#define GIMBAL_ITABLE_VARIANT_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_ITABLE_VARIANT_TEST_SUITE_TYPE           (GBL_TYPEID(GblITableVariantTestSuite))
#define GBL_ITABLE_VARIANT_TEST_SUITE(self)          (GBL_CAST(GblITableVariantTestSuite, self))
#define GBL_ITABLE_VARIANT_TEST_SUITE_CLASS(klass)   (GBL_CLASS_CAST(GblITableVariantTestSuite, klass))
#define GBL_ITABLE_VARIANT_TEST_SUITE_CLASSOF(self)  (GBL_CLASSOF(GblITableVariantTestSuite, self))

GBL_DECLS_BEGIN

GBL_DECLARE_TYPE         (GblITableVariantTestSuite);
GBL_CLASS_DERIVE_EMPTY   (GblITableVariantTestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GblITableVariantTestSuite, GblTestSuite)

GBL_DECLS_END

#endif // GIMBAL_ITABLE_VARIANT_TEST_SUITE_H

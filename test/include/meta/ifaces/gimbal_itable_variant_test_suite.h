#ifndef GIMBAL_ITABLE_VARIANT_TEST_SUITE_H
#define GIMBAL_ITABLE_VARIANT_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_ITABLE_VARIANT_TEST_SUITE_TYPE             (GBL_TYPEOF(GblITableVariantTestSuite))
#define GBL_ITABLE_VARIANT_TEST_SUITE(self)            (GBL_INSTANCE_CAST(self, GblITableVariantTestSuite))
#define GBL_ITABLE_VARIANT_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST(klass, GblITableVariantTestSuite))
#define GBL_ITABLE_VARIANT_TEST_SUITE_GET_CLASS(self)  (GBL_INSTANCE_GET_CLASS(self, GblITableVariantTestSuite))

GBL_DECLS_BEGIN

GBL_DECLARE_TYPE         (GblITableVariantTestSuite);
GBL_CLASS_DERIVE_EMPTY   (GblITableVariantTestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GblITableVariantTestSuite, GblTestSuite)

GBL_DECLS_END

#endif // GIMBAL_ITABLE_VARIANT_TEST_SUITE_H

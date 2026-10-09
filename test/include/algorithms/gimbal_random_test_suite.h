#ifndef GIMBAL_RANDOM_TEST_SUITE_H
#define GIMBAL_RANDOM_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_RANDOM_TEST_SUITE_TYPE           (GBL_TYPEID(GblRandomTestSuite))
#define GBL_RANDOM_TEST_SUITE(self)          (GBL_CAST(GblRandomTestSuite, self))
#define GBL_RANDOM_TEST_SUITE_CLASS(klass)   (GBL_CLASS_CAST(GblRandomTestSuite, klass))
#define GBL_RANDOM_TEST_SUITE_CLASSOF(self)  (GBL_CLASSOF(GblRandomTestSuite, self))

GBL_DECLS_BEGIN

GBL_DECLARE_TYPE         (GblRandomTestSuite);
GBL_CLASS_DERIVE_EMPTY   (GblRandomTestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GblRandomTestSuite, GblTestSuite)

GBL_DECLS_END

#endif // GIMBAL_RANDOM_TEST_SUITE_H

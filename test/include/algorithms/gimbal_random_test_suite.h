#ifndef GIMBAL_RANDOM_TEST_SUITE_H
#define GIMBAL_RANDOM_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_RANDOM_TEST_SUITE_TYPE             (GBL_TYPEID(GblRandomTestSuite))
#define GBL_RANDOM_TEST_SUITE(self)            (GBL_CAST(self, GblRandomTestSuite))
#define GBL_RANDOM_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST(klass, GblRandomTestSuite))
#define GBL_RANDOM_TEST_SUITE_GET_CLASS(self)  (GBL_CLASSOF(self, GblRandomTestSuite))

GBL_DECLS_BEGIN

GBL_DECLARE_TYPE         (GblRandomTestSuite);
GBL_CLASS_DERIVE_EMPTY   (GblRandomTestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GblRandomTestSuite, GblTestSuite)

GBL_DECLS_END

#endif // GIMBAL_RANDOM_TEST_SUITE_H

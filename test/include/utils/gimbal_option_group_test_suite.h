#ifndef GIMBAL_OPTION_GROUP_TEST_SUITE_H
#define GIMBAL_OPTION_GROUP_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_OPTION_GROUP_TEST_SUITE_TYPE             (GBL_TYPEID(GblOptionGroupTestSuite))

#define GBL_OPTION_GROUP_TEST_SUITE(inst)            (GBL_CAST(inst, GBL_OPTION_GROUP_TEST_SUITE_TYPE, GblOptionGroupTestSuite))
#define GBL_OPTION_GROUP_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST(klass, GBL_OPTION_GROUP_TEST_SUITE_TYPE, GblOptionGroupTestSuiteClass))
#define GBL_OPTION_GROUP_TEST_SUITE_GET_CLASS(inst)  (GBL_INSTANCE_GET_CLASS_CAST(inst, GBL_OPTION_GROUP_TEST_SUITE_TYPE, GblOptionGroupTestSuiteClass))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblOptionGroupTestSuite, GblTestSuite)

GBL_INSTANCE_DERIVE_EMPTY(GblOptionGroupTestSuite, GblTestSuite)

GBL_EXPORT GblType GblOptionGroupTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_OPTION_GROUP_TEST_SUITE_H

#ifndef GIMBAL_MODULE_TEST_SUITE_H
#define GIMBAL_MODULE_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_MODULE_TEST_SUITE_TYPE             (GBL_TYPEOF(GblModuleTestSuite))

#define GBL_MODULE_TEST_SUITE(inst)            (GBL_INSTANCE_CAST(inst, GBL_MODULE_TEST_SUITE_TYPE, GblModuleTestSuite))
#define GBL_MODULE_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST(klass, GBL_MODULE_TEST_SUITE_TYPE, GblModuleTestSuiteClass))
#define GBL_MODULE_TEST_SUITE_GET_CLASS(inst)  (GBL_INSTANCE_CLASS_CAST(inst, GBL_MODULE_TEST_SUITE_TYPE, GblModuleTestSuiteClass))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblModuleTestSuite, GblTestSuite)

GBL_INSTANCE_DERIVE_EMPTY(GblModuleTestSuite, GblTestSuite)

GBL_EXPORT GblType GblModuleTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_MODULE_TEST_SUITE_H

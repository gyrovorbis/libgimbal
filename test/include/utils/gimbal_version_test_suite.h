#ifndef GIMBAL_VERSION_TEST_SUITE_H
#define GIMBAL_VERSION_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_VERSION_TEST_SUITE_TYPE             (GblVersionTestSuite_type())

#define GBL_VERSION_TEST_SUITE(inst)            (GBL_CAST(inst, GBL_VERSION_TEST_SUITE_TYPE, GblVersionTestSuite))
#define GBL_VERSION_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST(klass, GBL_VERSION_TEST_SUITE_TYPE, GblVersionTestSuiteClass))
#define GBL_VERSION_TEST_SUITE_GET_CLASS(inst)  (GBL_INSTANCE_GET_CLASS_CAST(inst, GBL_VERSION_TEST_SUITE_TYPE, GblVersionTestSuiteClass))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblVersionTestSuite, GblTestSuite)

GBL_INSTANCE_DERIVE_EMPTY(GblVersionTestSuite, GblTestSuite)

GBL_EXPORT GblType GblVersionTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_VERSION_TEST_SUITE_H

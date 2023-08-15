#ifndef GIMBAL_ERROR_TEST_SUITE_H
#define GIMBAL_ERROR_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_ERROR_TEST_SUITE_TYPE             (GBL_TYPEID(GblErrorTestSuite))

#define GBL_ERROR_TEST_SUITE(inst)            (GBL_CAST(inst, GBL_ERROR_TEST_SUITE_TYPE, GblErrorTestSuite))
#define GBL_ERROR_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST(klass, GBL_ERROR_TEST_SUITE_TYPE, GblErrorTestSuiteClass))
#define GBL_ERROR_TEST_SUITE_GET_CLASS(inst)  (GBL_INSTANCE_GET_CLASS_CAST(inst, GBL_ERROR_TEST_SUITE_TYPE, GblErrorTestSuiteClass))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblErrorTestSuite, GblTestSuite)

GBL_INSTANCE_DERIVE_EMPTY(GblErrorTestSuite, GblTestSuite)

GBL_EXPORT GblType GblErrorTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_ERROR_TEST_SUITE_H

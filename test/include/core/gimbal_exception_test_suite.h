#ifndef GIMBAL_EXCEPTION_TEST_SUITE_H
#define GIMBAL_EXCEPTION_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_EXCEPTION_TEST_SUITE_TYPE             (GBL_TYPEID(GblExceptionTestSuite))

#define GBL_EXCEPTION_TEST_SUITE(inst)            (GBL_CAST(inst, GBL_EXCEPTION_TEST_SUITE_TYPE, GblExceptionTestSuite))
#define GBL_EXCEPTION_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST(klass, GBL_EXCEPTION_TEST_SUITE_TYPE, GblExceptionTestSuiteClass))
#define GBL_EXCEPTION_TEST_SUITE_GET_CLASS(inst)  (GBL_INSTANCE_GET_CLASS_CAST(inst, GBL_EXCEPTION_TEST_SUITE_TYPE, GblExceptionTestSuiteClass))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblExceptionTestSuite, GblTestSuite)

GBL_INSTANCE_DERIVE_EMPTY(GblExceptionTestSuite, GblTestSuite)

GBL_EXPORT GblType GblExceptionTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_EXCEPTION_TEST_SUITE_H

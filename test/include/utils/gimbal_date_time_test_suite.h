#ifndef GIMBAL_DATE_TIME_TEST_SUITE_H
#define GIMBAL_DATE_TIME_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_DATE_TIME_TEST_SUITE_TYPE             (GBL_TYPEID(GblDateTimeTestSuite))

#define GBL_DATE_TIME_TEST_SUITE(inst)            (GBL_CAST(inst, GBL_DATE_TIME_TEST_SUITE_TYPE, GblDateTimeTestSuite))
#define GBL_DATE_TIME_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST(klass, GBL_DATE_TIME_TEST_SUITE_TYPE, GblDateTimeTestSuiteClass))
#define GBL_DATE_TIME_TEST_SUITE_GET_CLASS(inst)  (GBL_INSTANCE_GET_CLASS_CAST(inst, GBL_DATE_TIME_TEST_SUITE_TYPE, GblDateTimeTestSuiteClass))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblDateTimeTestSuite, GblTestSuite)

GBL_INSTANCE_DERIVE_EMPTY(GblDateTimeTestSuite, GblTestSuite)

GBL_EXPORT GblType GblDateTimeTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_DATE_TIME_TEST_SUITE_H

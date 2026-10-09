#ifndef GIMBAL_DATE_TIME_TEST_SUITE_H
#define GIMBAL_DATE_TIME_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_DATE_TIME_TEST_SUITE_TYPE             (GBL_TYPEID(GblDateTimeTestSuite))

#define GBL_DATE_TIME_TEST_SUITE(inst)          (GBL_CAST(GblDateTimeTestSuite, inst))
#define GBL_DATE_TIME_TEST_SUITE_CLASS(klass)   (GBL_CLASS_CAST(GblDateTimeTestSuite, klass))
#define GBL_DATE_TIME_TEST_SUITE_CLASSOF(inst)  (GBL_CLASSOF(GblDateTimeTestSuite, inst))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblDateTimeTestSuite, GblTestSuite)

GBL_INSTANCE_DERIVE_EMPTY(GblDateTimeTestSuite, GblTestSuite)

GBL_EXPORT GblType GblDateTimeTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_DATE_TIME_TEST_SUITE_H

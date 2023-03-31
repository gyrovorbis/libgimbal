#ifndef GIMBAL_PATTERN_TEST_SUITE_H
#define GIMBAL_PATTERN_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_PATTERN_TEST_SUITE_TYPE             (GblPatternTestSuite_type())

#define GBL_PATTERN_TEST_SUITE(inst)            (GBL_INSTANCE_CAST(inst, GBL_PATTERN_TEST_SUITE_TYPE, GblPatternTestSuite))
#define GBL_PATTERN_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST(klass, GBL_PATTERN_TEST_SUITE_TYPE, GblPatternTestSuiteClass))
#define GBL_PATTERN_TEST_SUITE_GET_CLASS(inst)  (GBL_INSTANCE_CLASS_CAST(inst, GBL_PATTERN_TEST_SUITE_TYPE, GblPatternTestSuiteClass))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblPatternTestSuite, GblTestSuite)

GBL_INSTANCE_DERIVE_EMPTY(GblPatternTestSuite, GblTestSuite)

GBL_EXPORT GblType GblPatternTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_PATTERN_TEST_SUITE_H

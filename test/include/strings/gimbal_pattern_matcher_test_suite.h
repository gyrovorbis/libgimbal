#ifndef GIMBAL_PATTERN_MATCHER_TEST_SUITE_H
#define GIMBAL_PATTERN_MATCHER_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_PATTERN_MATCHER_TEST_SUITE_TYPE             (GblPatternMatcherTestSuite_type())

#define GBL_PATTERN_MATCHER_TEST_SUITE(inst)            (GBL_INSTANCE_CAST(inst, GBL_PATTERN_MATCHER_TEST_SUITE_TYPE, GblPatternMatcherTestSuite))
#define GBL_PATTERN_MATCHER_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST(klass, GBL_PATTERN_MATCHER_TEST_SUITE_TYPE, GblPatternMatcherTestSuiteClass))
#define GBL_PATTERN_MATCHER_TEST_SUITE_GET_CLASS(inst)  (GBL_INSTANCE_CLASS_CAST(inst, GBL_PATTERN_MATCHER_TEST_SUITE_TYPE, GblPatternMatcherTestSuiteClass))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblPatternMatcherTestSuite, GblTestSuite)

GBL_INSTANCE_DERIVE_EMPTY(GblPatternMatcherTestSuite, GblTestSuite)

GBL_EXPORT GblType GblPatternMatcherTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_PATTERN_MATCHER_TEST_SUITE_H

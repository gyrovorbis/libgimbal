#ifndef GIMBAL_STRING_PATTERN_TEST_SUITE_H
#define GIMBAL_STRING_PATTERN_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_STRING_PATTERN_TEST_SUITE_TYPE             (GblStringPatternTestSuite_type())

#define GBL_STRING_PATTERN_TEST_SUITE(inst)            (GBL_INSTANCE_CAST(inst, GBL_STRING_PATTERN_TEST_SUITE_TYPE, GblStringPatternTestSuite))
#define GBL_STRING_PATTERN_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST(klass, GBL_STRING_PATTERN_TEST_SUITE_TYPE, GblStringPatternTestSuiteClass))
#define GBL_STRING_PATTERN_TEST_SUITE_GET_CLASS(inst)  (GBL_INSTANCE_CLASS_CAST(inst, GBL_STRING_PATTERN_TEST_SUITE_TYPE, GblStringPatternTestSuiteClass))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblStringPatternTestSuite, GblTestSuite)

GBL_INSTANCE_DERIVE_EMPTY(GblStringPatternTestSuite, GblTestSuite)

GBL_EXPORT GblType GblStringPatternTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_STRING_PATTERN_TEST_SUITE_H

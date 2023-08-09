#ifndef GIMBAL_PATTERN_TEST_SUITE_HPP
#define GIMBAL_PATTERN_TEST_SUITE_HPP

#include <gimbal/gimbal_test.h>

#define GBL_PATTERN_TEST_SUITE_CPP_TYPE   (GBL_TYPEID(GblPatternTestSuiteCpp))

GBL_CLASS_DERIVE_EMPTY(GblPatternTestSuiteCpp, GblTestSuite)

GBL_INSTANCE_DERIVE_EMPTY(GblPatternTestSuiteCpp, GblTestSuite)

GBL_EXPORT GBL_DECLARE_TYPE(GblPatternTestSuiteCpp);

#endif // GIMBAL_PATTERN_TEST_SUITE_HPP

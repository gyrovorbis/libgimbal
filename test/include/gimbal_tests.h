#ifndef GIMBAL_TESTS_H
#define GIMBAL_TESTS_H

#include <gimbal/test/gimbal_test_scenario.h>

GBL_DECLARE_TYPE(GimbalTests);

#define GIMBAL_TEST_SCENARIO_TYPE  GBL_TYPEID(GimbalTests)

GBL_CLASS_DERIVE_EMPTY(GimbalTests, GblTestScenario)
GBL_INSTANCE_DERIVE_EMPTY(GimbalTests, GblTestScenario)

GBL_EXPORT GblType GimbalTestScenario_type(void) GBL_NOEXCEPT;

#endif
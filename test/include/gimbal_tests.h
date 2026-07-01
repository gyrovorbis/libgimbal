#ifndef GIMBAL_TESTS_H
#define GIMBAL_TESTS_H

#include <gimbal/test/gimbal_test_scenario.h>

#define GIMBAL_TEST_SCENARIO_TYPE GBL_TYPEID(GimbalTests)

GBL_DERIVE_EMPTY_TYPE(GimbalTests, GblTestScenario)

GBL_EXPORT GblType GimbalTestScenario_type(void) GBL_NOEXCEPT;

#endif
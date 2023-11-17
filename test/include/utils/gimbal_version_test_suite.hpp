#ifndef GIMBAL_VERSION_TEST_SUITE_HPP
#define GIMBAL_VERSION_TEST_SUITE_HPP

#include <gimbal/gimbal_test.h>

#define GBL_VERSION_TEST_SUITE_CPP_TYPE   (GBL_TYPEID(GblVersionTestSuiteCpp))

GBL_CLASS_DERIVE_EMPTY(GblVersionTestSuiteCpp, GblTestSuite)

GBL_INSTANCE_DERIVE_EMPTY(GblVersionTestSuiteCpp, GblTestSuite)

GBL_EXPORT GBL_DECLARE_TYPE(GblVersionTestSuiteCpp);

#endif // GIMBAL_VERSION_TEST_SUITE_HPP

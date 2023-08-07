#ifndef GIMBAL_QUARK_TEST_SUITE_HPP
#define GIMBAL_QUARK_TEST_SUITE_HPP

#include <gimbal/gimbal_test.h>

#define GBL_QUARK_TEST_SUITE_CPP_TYPE   (GBL_TYPEID(GblQuarkTestSuiteCpp))

GBL_CLASS_DERIVE_EMPTY(GblQuarkTestSuiteCpp, GblTestSuite)

GBL_INSTANCE_DERIVE_EMPTY(GblQuarkTestSuiteCpp, GblTestSuite)

GBL_EXPORT GBL_DECLARE_TYPE(GblQuarkTestSuiteCpp);

#endif // GIMBAL_QUARK_TEST_SUITE_HPP

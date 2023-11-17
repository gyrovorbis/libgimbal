#ifndef GIMBAL_STRING_REF_TEST_SUITE_HPP
#define GIMBAL_STRING_REF_TEST_SUITE_HPP

#include <gimbal/gimbal_test.h>

#define GBL_STRING_REF_TEST_SUITE_CPP_TYPE   (GBL_TYPEID(GblStringRefTestSuiteCpp))

GBL_CLASS_DERIVE_EMPTY(GblStringRefTestSuiteCpp, GblTestSuite)

GBL_INSTANCE_DERIVE_EMPTY(GblStringRefTestSuiteCpp, GblTestSuite)

GBL_EXPORT GBL_DECLARE_TYPE(GblStringRefTestSuiteCpp);

#endif // GIMBAL_STRING_REF_TEST_SUITE_HPP

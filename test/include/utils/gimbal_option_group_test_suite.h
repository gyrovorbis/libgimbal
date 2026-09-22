#ifndef GIMBAL_OPTION_GROUP_TEST_SUITE_H
#define GIMBAL_OPTION_GROUP_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_OPTION_GROUP_TEST_SUITE_TYPE             (GBL_TYPEID(GblOptionGroupTestSuite))

#define GBL_OPTION_GROUP_TEST_SUITE(inst)          (GBL_CAST(GblOptionGroupTestSuite, inst))
#define GBL_OPTION_GROUP_TEST_SUITE_CLASS(klass)   (GBL_CLASS_CAST(GblOptionGroupTestSuite, klass))
#define GBL_OPTION_GROUP_TEST_SUITE_CLASSOF(inst)  (GBL_CLASSOF(GblOptionGroupTestSuite, inst))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblOptionGroupTestSuite, GblTestSuite)

GBL_INSTANCE_DERIVE_EMPTY(GblOptionGroupTestSuite, GblTestSuite)

GBL_EXPORT GblType GblOptionGroupTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_OPTION_GROUP_TEST_SUITE_H

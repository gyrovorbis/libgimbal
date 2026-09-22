#ifndef GIMBAL_MODULE_TEST_SUITE_H
#define GIMBAL_MODULE_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_MODULE_TEST_SUITE_TYPE             (GBL_TYPEID(GblModuleTestSuite))

#define GBL_MODULE_TEST_SUITE(inst)          (GBL_CAST(GblModuleTestSuite, inst))
#define GBL_MODULE_TEST_SUITE_CLASS(klass)   (GBL_CLASS_CAST(GblModuleTestSuite, klass))
#define GBL_MODULE_TEST_SUITE_CLASSOF(inst)  (GBL_CLASSOF(GblModuleTestSuite, inst))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblModuleTestSuite, GblTestSuite)

GBL_INSTANCE_DERIVE_EMPTY(GblModuleTestSuite, GblTestSuite)

GBL_EXPORT GblType GblModuleTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_MODULE_TEST_SUITE_H

#ifndef GIMBAL_VERSION_TEST_SUITE_H
#define GIMBAL_VERSION_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_VERSION_TEST_SUITE_TYPE             (GblVersionTestSuite_type())

#define GBL_VERSION_TEST_SUITE(inst)          (GBL_CAST(GblVersionTestSuite, inst))
#define GBL_VERSION_TEST_SUITE_CLASS(klass)   (GBL_CLASS_CAST(GblVersionTestSuite, klass))
#define GBL_VERSION_TEST_SUITE_CLASSOF(inst)  (GBL_CLASSOF(GblVersionTestSuite, inst))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblVersionTestSuite, GblTestSuite)

GBL_INSTANCE_DERIVE_EMPTY(GblVersionTestSuite, GblTestSuite)

GBL_EXPORT GblType GblVersionTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_VERSION_TEST_SUITE_H

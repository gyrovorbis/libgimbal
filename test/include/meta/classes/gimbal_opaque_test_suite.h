#ifndef GIMBAL_OPAQUE_TEST_SUITE_H
#define GIMBAL_OPAQUE_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_OPAQUE_TEST_SUITE_TYPE             (GblOpaqueTestSuite_type())

#define GBL_OPAQUE_TEST_SUITE(inst)            (GBL_CAST(inst, GBL_OPAQUE_TEST_SUITE_TYPE, GblOpaqueTestSuite))
#define GBL_OPAQUE_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST(klass, GBL_OPAQUE_TEST_SUITE_TYPE, GblOpaqueTestSuiteClass))
#define GBL_OPAQUE_TEST_SUITE_GET_CLASS(inst)  (GBL_INSTANCE_GET_CLASS_CAST(inst, GBL_OPAQUE_TEST_SUITE_TYPE, GblOpaqueTestSuiteClass))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblOpaqueTestSuite, GblTestSuite)

GBL_INSTANCE_DERIVE_EMPTY(GblOpaqueTestSuite, GblTestSuite)

GBL_EXPORT GblType GblOpaqueTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_OPAQUE_TEST_SUITE_H

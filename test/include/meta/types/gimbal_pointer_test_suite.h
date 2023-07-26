#ifndef GIMBAL_POINTER_TEST_SUITE_H
#define GIMBAL_POINTER_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_POINTER_TEST_SUITE_TYPE             (GblPointerTestSuite_type())

#define GBL_POINTER_TEST_SUITE(inst)            (GBL_CAST(inst, GBL_POINTER_TEST_SUITE_TYPE, GblPointerTestSuite))
#define GBL_POINTER_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST(klass, GBL_POINTER_TEST_SUITE_TYPE, GblPointerTestSuiteClass))
#define GBL_POINTER_TEST_SUITE_GET_CLASS(inst)  (GBL_INSTANCE_GET_CLASS_CAST(inst, GBL_POINTER_TEST_SUITE_TYPE, GblPointerTestSuiteClass))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblPointerTestSuite, GblTestSuite)

GBL_INSTANCE_DERIVE_EMPTY(GblPointerTestSuite, GblTestSuite)

GBL_EXPORT GblType GblPointerTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_POINTER_TEST_SUITE_H

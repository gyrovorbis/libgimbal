#ifndef GIMBAL_ARRAY_HEAP_TEST_SUITE_H
#define GIMBAL_ARRAY_HEAP_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_ARRAY_HEAP_TEST_SUITE_TYPE             (GblArrayHeapTestSuite_type())

#define GBL_ARRAY_HEAP_TEST_SUITE(inst)            (GBL_CAST(inst, GBL_ARRAY_HEAP_TEST_SUITE_TYPE, GblArrayHeapTestSuite))
#define GBL_ARRAY_HEAP_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST(klass, GBL_ARRAY_HEAP_TEST_SUITE_TYPE, GblArrayHeapTestSuiteClass))
#define GBL_ARRAY_HEAP_TEST_SUITE_GET_CLASS(inst)  (GBL_INSTANCE_GET_CLASS_CAST(inst, GBL_ARRAY_HEAP_TEST_SUITE_TYPE, GblArrayHeapTestSuiteClass))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblArrayHeapTestSuite, GblTestSuite)

GBL_INSTANCE_DERIVE_EMPTY(GblArrayHeapTestSuite, GblTestSuite)

GBL_EXPORT GblType GblArrayHeapTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_ARRAY_HEAP_TEST_SUITE_H

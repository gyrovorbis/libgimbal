#ifndef GIMBAL_ARRAY_DEQUE_TEST_SUITE_H
#define GIMBAL_ARRAY_DEQUE_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_ARRAY_DEQUE_TEST_SUITE_TYPE             (GblArrayDequeTestSuite_type())

#define GBL_ARRAY_DEQUE_TEST_SUITE(inst)            (GBL_CAST(inst, GBL_ARRAY_DEQUE_TEST_SUITE_TYPE, GblArrayDequeTestSuite))
#define GBL_ARRAY_DEQUE_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST(klass, GBL_ARRAY_DEQUE_TEST_SUITE_TYPE, GblArrayDequeTestSuiteClass))
#define GBL_ARRAY_DEQUE_TEST_SUITE_GET_CLASS(inst)  (GBL_INSTANCE_GET_CLASS_CAST(inst, GBL_ARRAY_DEQUE_TEST_SUITE_TYPE, GblArrayDequeTestSuiteClass))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblArrayDequeTestSuite, GblTestSuite)

GBL_INSTANCE_DERIVE_EMPTY(GblArrayDequeTestSuite, GblTestSuite)

GBL_EXPORT GblType GblArrayDequeTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_ARRAY_DEQUE_TEST_SUITE_H

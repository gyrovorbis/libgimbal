#ifndef GIMBAL_ARRAY_DEQUE_TEST_SUITE_H
#define GIMBAL_ARRAY_DEQUE_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_ARRAY_DEQUE_TEST_SUITE_TYPE             (GblArrayDequeTestSuite_type())

#define GBL_ARRAY_DEQUE_TEST_SUITE(inst)          (GBL_CAST(GblArrayDequeTestSuite, inst))
#define GBL_ARRAY_DEQUE_TEST_SUITE_CLASS(klass)   (GBL_CLASS_CAST(GblArrayDequeTestSuite, klass))
#define GBL_ARRAY_DEQUE_TEST_SUITE_CLASSOF(inst)  (GBL_CLASSOF(GblArrayDequeTestSuite, inst))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblArrayDequeTestSuite, GblTestSuite)

GBL_INSTANCE_DERIVE_EMPTY(GblArrayDequeTestSuite, GblTestSuite)

GBL_EXPORT GblType GblArrayDequeTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_ARRAY_DEQUE_TEST_SUITE_H

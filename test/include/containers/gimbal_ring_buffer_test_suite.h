#ifndef GIMBAL_RING_BUFFER_TEST_SUITE_H
#define GIMBAL_RING_BUFFER_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_RING_BUFFER_TEST_SUITE_TYPE             (GblRingBufferTestSuite_type())

#define GBL_RING_BUFFER_TEST_SUITE(inst)            (GBL_CAST(inst, GBL_RING_BUFFER_TEST_SUITE_TYPE, GblRingBufferTestSuite))
#define GBL_RING_BUFFER_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST(klass, GBL_RING_BUFFER_TEST_SUITE_TYPE, GblRingBufferTestSuiteClass))
#define GBL_RING_BUFFER_TEST_SUITE_GET_CLASS(inst)  (GBL_INSTANCE_GET_CLASS_CAST(inst, GBL_RING_BUFFER_TEST_SUITE_TYPE, GblRingBufferTestSuiteClass))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblRingBufferTestSuite, GblTestSuite)

GBL_INSTANCE_DERIVE_EMPTY(GblRingBufferTestSuite, GblTestSuite)

GBL_EXPORT GblType GblRingBufferTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_RING_BUFFER_TEST_SUITE_H

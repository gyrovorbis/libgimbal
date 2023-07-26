#ifndef GBL_RING_LIST_TEST_SUITE_H
#define GBL_RING_LIST_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_RING_LIST_TEST_SUITE_TYPE             (GBL_TYPEID(GblRingListTestSuite))

#define GBL_RING_LIST_TEST_SUITE(inst)            (GBL_CAST(inst, GblRingListTestSuite))
#define GBL_RING_LIST_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST(klass, GblRingListTestSuite))
#define GBL_RING_LIST_TEST_SUITE_GET_CLASS(inst)  (GBL_CLASSOF(inst, GblRingListTestSuite))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY   (GblRingListTestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GblRingListTestSuite, GblTestSuite)

GBL_EXPORT GblType GblRingListTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GBL_RING_LIST_TEST_SUITE_H

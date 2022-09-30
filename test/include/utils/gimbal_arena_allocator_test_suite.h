#ifndef GIMBAL_ARENA_ALLOCATOR_TEST_SUITE_H
#define GIMBAL_ARENA_ALLOCATOR_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_ARENA_ALLOCATOR_TEST_SUITE_TYPE             (GBL_TYPEOF(GblArenaAllocatorTestSuite))

#define GBL_ARENA_ALLOCATOR_TEST_SUITE(inst)            (GBL_INSTANCE_CAST(inst, GblArenaAllocatorTestSuite))
#define GBL_ARENA_ALLOCATOR_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST(klass, GblArenaAllocatorTestSuite))
#define GBL_ARENA_ALLOCATOR_TEST_SUITE_GET_CLASS(inst)  (GBL_INSTANCE_GET_CLASS(inst, GblArenaAllocatorTestSuite))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY   (GblArenaAllocatorTestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GblArenaAllocatorTestSuite, GblTestSuite)

GBL_EXPORT GblType GblArenaAllocatorTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_ARENA_ALLOCATOR_TEST_SUITE_H

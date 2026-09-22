#ifndef GIMBAL_ARENA_ALLOCATOR_TEST_SUITE_H
#define GIMBAL_ARENA_ALLOCATOR_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_ARENA_ALLOCATOR_TEST_SUITE_TYPE             (GBL_TYPEID(GblArenaAllocatorTestSuite))

#define GBL_ARENA_ALLOCATOR_TEST_SUITE(inst)          (GBL_CAST(GblArenaAllocatorTestSuite, inst))
#define GBL_ARENA_ALLOCATOR_TEST_SUITE_CLASS(klass)   (GBL_CLASS_CAST(GblArenaAllocatorTestSuite, klass))
#define GBL_ARENA_ALLOCATOR_TEST_SUITE_CLASSOF(inst)  (GBL_CLASSOF(GblArenaAllocatorTestSuite, inst))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY   (GblArenaAllocatorTestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GblArenaAllocatorTestSuite, GblTestSuite)

GBL_EXPORT GblType GblArenaAllocatorTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_ARENA_ALLOCATOR_TEST_SUITE_H

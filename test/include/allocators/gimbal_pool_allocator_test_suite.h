#ifndef GIMBAL_POOL_ALLOCATOR_TEST_SUITE_H
#define GIMBAL_POOL_ALLOCATOR_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_POOL_ALLOCATOR_TEST_SUITE_TYPE             (GBL_TYPEID(GblPoolAllocatorTestSuite))

#define GBL_POOL_ALLOCATOR_TEST_SUITE(inst)            (GBL_CAST(inst, GblPoolAllocatorTestSuite))
#define GBL_POOL_ALLOCATOR_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST(klass, GblPoolAllocatorTestSuite))
#define GBL_POOL_ALLOCATOR_TEST_SUITE_GET_CLASS(inst)  (GBL_CLASSOF(inst, GblPoolAllocatorTestSuite))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY   (GblPoolAllocatorTestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GblPoolAllocatorTestSuite, GblTestSuite)

GBL_EXPORT GblType GblPoolAllocatorTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_POOL_ALLOCATOR_TEST_SUITE_H

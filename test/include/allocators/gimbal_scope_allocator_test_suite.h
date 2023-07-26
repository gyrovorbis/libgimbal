#ifndef GIMBAL_SCOPE_ALLOCATOR_TEST_SUITE_H
#define GIMBAL_SCOPE_ALLOCATOR_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_SCOPE_ALLOCATOR_TEST_SUITE_TYPE             (GBL_TYPEID(GblScopeAllocatorTestSuite))

#define GBL_SCOPE_ALLOCATOR_TEST_SUITE(inst)            (GBL_CAST(inst, GblScopeAllocatorTestSuite))
#define GBL_SCOPE_ALLOCATOR_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST(klass, GblScopeAllocatorTestSuite))
#define GBL_SCOPE_ALLOCATOR_TEST_SUITE_GET_CLASS(inst)  (GBL_CLASSOF(inst, GblScopeAllocatorTestSuite))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY   (GblScopeAllocatorTestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GblScopeAllocatorTestSuite, GblTestSuite)

GBL_EXPORT GblType GblScopeAllocatorTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_SCOPE_ALLOCATOR_TEST_SUITE_H

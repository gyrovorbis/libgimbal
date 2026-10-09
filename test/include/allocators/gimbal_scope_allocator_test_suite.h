#ifndef GIMBAL_SCOPE_ALLOCATOR_TEST_SUITE_H
#define GIMBAL_SCOPE_ALLOCATOR_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_SCOPE_ALLOCATOR_TEST_SUITE_TYPE             (GBL_TYPEID(GblScopeAllocatorTestSuite))

#define GBL_SCOPE_ALLOCATOR_TEST_SUITE(inst)          (GBL_CAST(GblScopeAllocatorTestSuite, inst))
#define GBL_SCOPE_ALLOCATOR_TEST_SUITE_CLASS(klass)   (GBL_CLASS_CAST(GblScopeAllocatorTestSuite, klass))
#define GBL_SCOPE_ALLOCATOR_TEST_SUITE_CLASSOF(inst)  (GBL_CLASSOF(GblScopeAllocatorTestSuite, inst))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY   (GblScopeAllocatorTestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GblScopeAllocatorTestSuite, GblTestSuite)

GBL_EXPORT GblType GblScopeAllocatorTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_SCOPE_ALLOCATOR_TEST_SUITE_H

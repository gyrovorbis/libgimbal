#ifndef GIMBAL_THREAD_TEST_SUITE_H
#define GIMBAL_THREAD_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_THREAD_TEST_SUITE_TYPE             (GBL_TYPEID(GblThreadTestSuite))

#define GBL_THREAD_TEST_SUITE(inst)          (GBL_CAST(GblThreadTestSuite, inst))
#define GBL_THREAD_TEST_SUITE_CLASS(klass)   (GBL_CLASS_CAST(GblThreadTestSuite, klass))
#define GBL_THREAD_TEST_SUITE_CLASSOF(inst)  (GBL_CLASSOF(GblThreadTestSuite, inst))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblThreadTestSuite, GblTestSuite)

GBL_INSTANCE_DERIVE_EMPTY(GblThreadTestSuite, GblTestSuite)

GBL_EXPORT GblType GblThreadTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_THREAD_TEST_SUITE_H

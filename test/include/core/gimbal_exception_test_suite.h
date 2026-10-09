#ifndef GIMBAL_EXCEPTION_TEST_SUITE_H
#define GIMBAL_EXCEPTION_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_EXCEPTION_TEST_SUITE_TYPE             (GBL_TYPEID(GblExceptionTestSuite))

#define GBL_EXCEPTION_TEST_SUITE(inst)          (GBL_CAST(GblExceptionTestSuite, inst))
#define GBL_EXCEPTION_TEST_SUITE_CLASS(klass)   (GBL_CLASS_CAST(GblExceptionTestSuite, klass))
#define GBL_EXCEPTION_TEST_SUITE_CLASSOF(inst)  (GBL_CLASSOF(GblExceptionTestSuite, inst))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblExceptionTestSuite, GblTestSuite)

GBL_INSTANCE_DERIVE_EMPTY(GblExceptionTestSuite, GblTestSuite)

GBL_EXPORT GblType GblExceptionTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_EXCEPTION_TEST_SUITE_H

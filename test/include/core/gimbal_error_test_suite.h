#ifndef GIMBAL_ERROR_TEST_SUITE_H
#define GIMBAL_ERROR_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_ERROR_TEST_SUITE_TYPE             (GBL_TYPEID(GblErrorTestSuite))

#define GBL_ERROR_TEST_SUITE(inst)          (GBL_CAST(GblErrorTestSuite, inst))
#define GBL_ERROR_TEST_SUITE_CLASS(klass)   (GBL_CLASS_CAST(GblErrorTestSuite, klass))
#define GBL_ERROR_TEST_SUITE_CLASSOF(inst)  (GBL_CLASSOF(GblErrorTestSuite, inst))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblErrorTestSuite, GblTestSuite)

GBL_INSTANCE_DERIVE_EMPTY(GblErrorTestSuite, GblTestSuite)

GBL_EXPORT GblType GblErrorTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_ERROR_TEST_SUITE_H

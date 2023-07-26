#ifndef GIMBAL_HASH_TEST_SUITE_H
#define GIMBAL_HASH_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_HASH_TEST_SUITE_TYPE             (GBL_TYPEID(GblHashTestSuite))

#define GBL_HASH_TEST_SUITE(inst)            (GBL_CAST(inst, GblHashTestSuite))
#define GBL_HASH_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST(klass, GblHashTestSuite))
#define GBL_HASH_TEST_SUITE_GET_CLASS(inst)  (GBL_CLASSOF(inst, GblHashTestSuite))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblHashTestSuite, GblTestSuite)


GBL_INSTANCE_DERIVE_EMPTY(GblHashTestSuite, GblTestSuite)

GBL_EXPORT GblType GblHashTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_HASH_TEST_SUITE_H

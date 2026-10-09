#ifndef GIMBAL_HASH_TEST_SUITE_H
#define GIMBAL_HASH_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_HASH_TEST_SUITE_TYPE             (GBL_TYPEID(GblHashTestSuite))

#define GBL_HASH_TEST_SUITE(inst)          (GBL_CAST(GblHashTestSuite, inst))
#define GBL_HASH_TEST_SUITE_CLASS(klass)   (GBL_CLASS_CAST(GblHashTestSuite, klass))
#define GBL_HASH_TEST_SUITE_CLASSOF(inst)  (GBL_CLASSOF(GblHashTestSuite, inst))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblHashTestSuite, GblTestSuite)


GBL_INSTANCE_DERIVE_EMPTY(GblHashTestSuite, GblTestSuite)

GBL_EXPORT GblType GblHashTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_HASH_TEST_SUITE_H

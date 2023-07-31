#ifndef GIMBAL_MD5_TEST_SUITE_H
#define GIMBAL_MD5_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_MD5_TEST_SUITE_TYPE             (GBL_TYPEID(GblMd5TestSuite))
#define GBL_MD5_TEST_SUITE(self)            (GBL_CAST(self, GblMd5TestSuite))
#define GBL_MD5_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST(klass, GblMd5TestSuite))
#define GBL_MD5_TEST_SUITE_GET_CLASS(self)  (GBL_CLASSOF(self, GblMd5TestSuite))

GBL_DECLS_BEGIN

GBL_DECLARE_TYPE         (GblMd5TestSuite);
GBL_CLASS_DERIVE_EMPTY   (GblMd5TestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GblMd5TestSuite, GblTestSuite)

GBL_DECLS_END

#endif // GIMBAL_MD5_TEST_SUITE_H

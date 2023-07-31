#ifndef GIMBAL_SHA1_TEST_SUITE_H
#define GIMBAL_SHA1_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_SHA1_TEST_SUITE_TYPE             (GBL_TYPEID(GblSha1TestSuite))
#define GBL_SHA1_TEST_SUITE(self)            (GBL_CAST(self, GblSha1TestSuite))
#define GBL_SHA1_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST(klass, GblSha1TestSuite))
#define GBL_SHA1_TEST_SUITE_GET_CLASS(self)  (GBL_CLASSOF(self, GblSha1TestSuite))

GBL_DECLS_BEGIN

GBL_DECLARE_TYPE         (GblSha1TestSuite);
GBL_CLASS_DERIVE_EMPTY   (GblSha1TestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GblSha1TestSuite, GblTestSuite)

GBL_DECLS_END

#endif // GIMBAL_SHA1_TEST_SUITE_H

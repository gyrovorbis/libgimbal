#ifndef GIMBAL_COMPRESSION_TEST_SUITE_H
#define GIMBAL_COMPRESSION_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_COMPRESSION_TEST_SUITE_TYPE             (GBL_TYPEID(GblCompressionTestSuite))
#define GBL_COMPRESSION_TEST_SUITE(self)            (GBL_CAST(self, GblCompressionTestSuite))
#define GBL_COMPRESSION_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST(klass, GblCompressionTestSuite))
#define GBL_COMPRESSION_TEST_SUITE_GET_CLASS(self)  (GBL_CLASSOF(self, GblCompressionTestSuite))

GBL_DECLS_BEGIN

GBL_DECLARE_TYPE         (GblCompressionTestSuite);
GBL_CLASS_DERIVE_EMPTY   (GblCompressionTestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GblCompressionTestSuite, GblTestSuite)

GBL_DECLS_END

#endif // GIMBAL_COMPRESSION_TEST_SUITE_H

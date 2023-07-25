#ifndef GIMBAL_BOX_TEST_SUITE_H
#define GIMBAL_BOX_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_BOX_TEST_SUITE_TYPE             (GBL_TYPEOF(GblBoxTestSuite))
#define GBL_BOX_TEST_SUITE(self)            (GBL_INSTANCE_CAST(self, GblBoxTestSuite))
#define GBL_BOX_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST(klass, GblBoxTestSuite))
#define GBL_BOX_TEST_SUITE_GET_CLASS(self)  (GBL_INSTANCE_GET_CLASS(self, GblBoxTestSuite))

GBL_DECLS_BEGIN

GBL_DECLARE_TYPE         (GblBoxTestSuite);
GBL_CLASS_DERIVE_EMPTY   (GblBoxTestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GblBoxTestSuite, GblTestSuite)

GBL_DECLS_END

#endif // GIMBAL_BOX_TEST_SUITE_H

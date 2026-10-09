#ifndef GIMBAL_BIT_VIEW_TEST_SUITE_H
#define GIMBAL_BIT_VIEW_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_BIT_VIEW_TEST_SUITE_TYPE             (GBL_TYPEID(GblBitViewTestSuite))

#define GBL_BIT_VIEW_TEST_SUITE(inst)          (GBL_CAST(GblBitViewTestSuite, inst))
#define GBL_BIT_VIEW_TEST_SUITE_CLASS(klass)   (GBL_CLASS_CAST(GblBitViewTestSuite, klass))
#define GBL_BIT_VIEW_TEST_SUITE_CLASSOF(inst)  (GBL_CLASSOF(GblBitViewTestSuite, inst))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblBitViewTestSuite, GblTestSuite)

GBL_INSTANCE_DERIVE_EMPTY(GblBitViewTestSuite, GblTestSuite)

GBL_EXPORT GblType GblBitViewTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_BIT_VIEW_TEST_SUITE_H

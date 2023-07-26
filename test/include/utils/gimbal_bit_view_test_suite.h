#ifndef GIMBAL_BIT_VIEW_TEST_SUITE_H
#define GIMBAL_BIT_VIEW_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_BIT_VIEW_TEST_SUITE_TYPE             (GBL_TYPEID(GblBitViewTestSuite))

#define GBL_BIT_VIEW_TEST_SUITE(inst)            (GBL_CAST(inst, GBL_BIT_VIEW_TEST_SUITE_TYPE, GblBitViewTestSuite))
#define GBL_BIT_VIEW_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST(klass, GBL_BIT_VIEW_TEST_SUITE_TYPE, GblBitViewTestSuiteClass))
#define GBL_BIT_VIEW_TEST_SUITE_GET_CLASS(inst)  (GBL_INSTANCE_GET_CLASS_CAST(inst, GBL_BIT_VIEW_TEST_SUITE_TYPE, GblBitViewTestSuiteClass))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblBitViewTestSuite, GblTestSuite)

GBL_INSTANCE_DERIVE_EMPTY(GblBitViewTestSuite, GblTestSuite)

GBL_EXPORT GblType GblBitViewTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_BIT_VIEW_TEST_SUITE_H

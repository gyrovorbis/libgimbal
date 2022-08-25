#ifndef GIMBAL_NARY_TREE_TEST_SUITE_H
#define GIMBAL_NARY_TREE_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_NARY_TREE_TEST_SUITE_TYPE             (GblNaryTreeTestSuite_type())

#define GBL_NARY_TREE_TEST_SUITE(inst)            (GBL_INSTANCE_CAST(inst, GBL_NARY_TREE_TEST_SUITE_TYPE, GblNaryTreeTestSuite))
#define GBL_NARY_TREE_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST(klass, GBL_NARY_TREE_TEST_SUITE_TYPE, GblNaryTreeTestSuiteClass))
#define GBL_NARY_TREE_TEST_SUITE_GET_CLASS(inst)  (GBL_INSTANCE_CLASS_CAST(inst, GBL_NARY_TREE_TEST_SUITE_TYPE, GblNaryTreeTestSuite))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblNaryTreeTestSuite, GblTestSuite)

GBL_INSTANCE_DERIVE_EMPTY(GblNaryTreeTestSuite, GblTestSuite)

GBL_EXPORT GblType GblNaryTreeTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_NARY_TREE_TEST_SUITE_H

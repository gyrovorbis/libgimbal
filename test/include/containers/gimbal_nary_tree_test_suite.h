#ifndef GIMBAL_NARY_TREE_TEST_SUITE_H
#define GIMBAL_NARY_TREE_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_NARY_TREE_TEST_SUITE_TYPE             (GblNaryTreeTestSuite_type())

#define GBL_NARY_TREE_TEST_SUITE(inst)          (GBL_CAST(GblNaryTreeTestSuite, inst))
#define GBL_NARY_TREE_TEST_SUITE_CLASS(klass)   (GBL_CLASS_CAST(GblNaryTreeTestSuite, klass))
#define GBL_NARY_TREE_TEST_SUITE_CLASSOF(inst)  (GBL_CLASSOF(GblNaryTreeTestSuite, inst))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblNaryTreeTestSuite, GblTestSuite)

GBL_INSTANCE_DERIVE_EMPTY(GblNaryTreeTestSuite, GblTestSuite)

GBL_EXPORT GblType GblNaryTreeTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_NARY_TREE_TEST_SUITE_H

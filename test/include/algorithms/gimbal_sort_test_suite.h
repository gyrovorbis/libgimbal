#ifndef GIMBAL_SORT_TEST_SUITE_H
#define GIMBAL_SORT_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_SORT_TEST_SUITE_TYPE             (GBL_TYPEID(GblSortTestSuite))

#define GBL_SORT_TEST_SUITE(inst)          (GBL_CAST(GblSortTestSuite, inst))
#define GBL_SORT_TEST_SUITE_CLASS(klass)   (GBL_CLASS_CAST(GblSortTestSuite, klass))
#define GBL_SORT_TEST_SUITE_CLASSOF(inst)  (GBL_CLASSOF(GblSortTestSuite, inst))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblSortTestSuite, GblTestSuite)


GBL_INSTANCE_DERIVE_EMPTY(GblSortTestSuite, GblTestSuite)

GBL_EXPORT GblType GblSortTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_SORT_TEST_SUITE_H

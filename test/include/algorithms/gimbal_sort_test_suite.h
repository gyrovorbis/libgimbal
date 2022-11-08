#ifndef GIMBAL_SORT_TEST_SUITE_H
#define GIMBAL_SORT_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_SORT_TEST_SUITE_TYPE             (GBL_TYPEOF(GblSortTestSuite))

#define GBL_SORT_TEST_SUITE(inst)            (GBL_INSTANCE_CAST(inst, GblSortTestSuite))
#define GBL_SORT_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST(klass, GblSortTestSuite))
#define GBL_SORT_TEST_SUITE_GET_CLASS(inst)  (GBL_INSTANCE_GET_CLASS(inst, GblSortTestSuite))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblSortTestSuite, GblTestSuite)


GBL_INSTANCE_DERIVE_EMPTY(GblSortTestSuite, GblTestSuite)

GBL_EXPORT GblType GblSortTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_SORT_TEST_SUITE_H

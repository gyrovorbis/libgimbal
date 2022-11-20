#ifndef GIMBAL_STRING_LIST_TEST_SUITE_H
#define GIMBAL_STRING_LIST_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_STRING_LIST_TEST_SUITE_TYPE                 (GblStringListTestSuite_type())

#define GBL_STRING_LIST_TEST_SUITE(instance)            (GBL_INSTANCE_CAST(instance, GblStringListTestSuite))
#define GBL_STRING_LIST_TEST_SUITE_CLASS(klass)         (GBL_CLASS_CAST(klass, GblStringListTestSuite))
#define GBL_STRING_LIST_TEST_SUITE_GET_CLASS(instance)  (GBL_INSTANCE_GET_CLASS(instance, GblStringListTestSuite))

GBL_DECLS_BEGIN

typedef struct GblStringListTestSuiteClass {
    GblTestSuiteClass   base;
} GblStringListTestSuiteClass;

typedef struct GblStringListTestSuite {
    union {
        GblStringListTestSuiteClass*    pClass;
        GblTestSuite                    base;
    };
} GblStringListTestSuite;

GBL_EXPORT GblType GblStringListTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_STRING_LIST_TEST_SUITE_H

#ifndef GIMBAL_LINKED_LIST_TEST_SUITE_H
#define GIMBAL_LINKED_LIST_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_LINKED_LIST_TEST_SUITE_TYPE             (GblLinkedListTestSuite_type())
#define GBL_LINKED_LIST_TEST_SUTE_STRUCT            GblLinkedListTestSuite
#define GBL_LINKED_LIST_TEST_SUITE_CLASS_STRUCT     GblLinkedListTestSuiteClass
#define GBL_LINKED_LIST_TEST_SUITE(inst)            (GBL_CAST_PREFIX(inst, GBL_LINKED_LIST_TEST_SUITE))
#define GBL_LINKED_LIST_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST_PREFIX(klass, GBL_LINKED_LIST_TEST_SUITE))
#define GBL_LINKED_LIST_TEST_SUITE_GET_CLASS(inst)  (GBL_INSTANCE_GET_CLASS_CAST_PREFIX(inst, GBL_LINKED_LIST_TEST_SUITE))

GBL_DECLS_BEGIN

typedef struct GblLinkedListTestSuiteClass {
    GblTestSuiteClass   base;
} GblLinkedListTestSuiteClass;

typedef struct GblLinkedListTestSuite {
    union {
        GblLinkedListTestSuiteClass*    pClass;
        GblTestSuite                    base;
    };
} GblLinkedListTestSuite;

GBL_EXPORT GblType GblLinkedListTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_LINKED_LIST_TEST_SUITE_H

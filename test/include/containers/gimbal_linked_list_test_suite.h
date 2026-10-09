#ifndef GIMBAL_LINKED_LIST_TEST_SUITE_H
#define GIMBAL_LINKED_LIST_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_LINKED_LIST_TEST_SUITE_TYPE           (GblLinkedListTestSuite_type())
#define GBL_LINKED_LIST_TEST_SUTE_STRUCT          GblLinkedListTestSuite
#define GBL_LINKED_LIST_TEST_SUITE_CLASS_STRUCT   GblLinkedListTestSuiteClass
#define GBL_LINKED_LIST_TEST_SUITE(inst)          (GBL_CAST(GblLinkedListTestSuite, inst))
#define GBL_LINKED_LIST_TEST_SUITE_CLASS(klass)   (GBL_CLASS_CAST(GblLinkedListTestSuite, klass))
#define GBL_LINKED_LIST_TEST_SUITE_CLASSOF(inst)  (GBL_CLASSOF(GblLinkedListTestSuite, inst))

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

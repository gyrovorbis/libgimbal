#ifndef GIMBAL_DOUBLY_LINKED_LIST_TEST_SUITE_H
#define GIMBAL_DOUBLY_LINKED_LIST_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GBL_DOUBLY_LINKED_LIST_TEST_SUITE_TYPE             (GblDoublyLinkedListTestSuite_type())
#define GBL_DOUBLY_LINKED_LIST_TEST_SUTE_STRUCT            GblDoublyLinkedListTestSuite
#define GBL_DOUBLY_LINKED_LIST_TEST_SUITE_CLASS_STRUCT     GblDoublyLinkedListTestSuiteClass
#define GBL_DOUBLY_LINKED_LIST_TEST_SUITE(inst)            (GBL_CAST_PREFIX(inst, GBL_DOUBLY_LINKED_LIST_TEST_SUITE))
#define GBL_DOUBLY_LINKED_LIST_TEST_SUITE_CLASS(klass)     (GBL_CLASS_CAST_PREFIX(klass, GBL_DOUBLY_LINKED_LIST_TEST_SUITE))
#define GBL_DOUBLY_LINKED_LIST_TEST_SUITE_GET_CLASS(inst)  (GBL_INSTANCE_GET_CLASS_CAST_PREFIX(inst, GBL_DOUBLY_LINKED_LIST_TEST_SUITE))

GBL_DECLS_BEGIN

typedef struct GblDoublyLinkedListTestSuiteClass {
    GblTestSuiteClass   base;
} GblDoublyLinkedListTestSuiteClass;

typedef struct GblDoublyLinkedListTestSuite {
    union {
        GblDoublyLinkedListTestSuiteClass*  pClass;
        GblTestSuite                        base;
    };
} GblDoublyLinkedListTestSuite;

GBL_EXPORT GblType GblDoublyLinkedListTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_DOUBLY_LINKED_LIST_TEST_SUITE_H

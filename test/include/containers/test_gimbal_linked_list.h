#ifndef TEST_GIMBAL_LINKED_LIST_H
#define TEST_GIMBAL_LINKED_LIST_H

#include <gimbal/types/gimbal_typedefs.h>
#include <gimbal/utils/gimbal_test.h>

#define GBL_LINKED_LIST_TEST_SET(X)         \
    X(GblLinkedList_test_testCaseInit)      \
    X(GblLinkedList_test_testCaseFinal)     \
    X(GblLinkedList_test_init)              \
    X(GblLinkedList_test_pushBack)          \
    X(GblLinkedList_test_pushFront)         \
    X(GblLinkedList_test_remove)            \
    X(GblLinkedList_test_moveBack)          \
    X(GblLinkedList_test_moveFront)         \
    X(GblLinkedList_test_joinBack)          \
    X(GblLinkedList_test_joinFront)         \
    X(GblLinkedList_test_erase)             \
    X(GblLinkedList_test_replace)           \
    X(GblLinkedList_test_swap)              \
    X(GblLinkedList_test_clear)             \
    X(GblLinkedList_test_final)

GBL_DECLS_BEGIN
GBL_LINKED_LIST_TEST_SET(GBL_TEST_CASE_DECL)
GBL_DECLS_END

#endif // TEST_GIMBAL_LINKED_LIST_H

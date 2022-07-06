#ifndef TEST_GIMBAL_DOUBLY_LINKED_LIST_H
#define TEST_GIMBAL_DOUBLY_LINKED_LIST_H

#include <gimbal/types/gimbal_typedefs.h>
#include <gimbal/utils/gimbal_test.h>

#define GBL_TEST_CASE_DECL(kase)    GBL_API kase(GblContext* pCtx);

#define GBL_DOUBLY_LINKED_LIST_TEST_SET(X)         \
    X(GblDoublyLinkedList_test_testCaseInit)      \
    X(GblDoublyLinkedList_test_testCaseFinal)     \
    X(GblDoublyLinkedList_test_init)              \
    X(GblDoublyLinkedList_test_pushBack)          \
    X(GblDoublyLinkedList_test_pushFront)         \
    X(GblDoublyLinkedList_test_remove)            \
    X(GblDoublyLinkedList_test_moveBack)          \
    X(GblDoublyLinkedList_test_moveFront)         \
    X(GblDoublyLinkedList_test_joinBack)          \
    X(GblDoublyLinkedList_test_joinFront)         \
    X(GblDoublyLinkedList_test_erase)             \
    X(GblDoublyLinkedList_test_replace)           \
    X(GblDoublyLinkedList_test_swap)              \
    X(GblDoublyLinkedList_test_clear)             \
    X(GblDoublyLinkedList_test_final)

GBL_DECLS_BEGIN
GBL_DOUBLY_LINKED_LIST_TEST_SET(GBL_TEST_CASE_DECL)
GBL_DECLS_END

#endif // TEST_GIMBAL_DOUBLY_LINKED_LIST_H

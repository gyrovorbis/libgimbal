#include "containers/test_gimbal_doubly_linked_list.h"
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/containers/gimbal_doubly_linked_list.h>

typedef struct TestStruct_ {
    char* pString;
    GblDoublyLinkedListNode listNode;
} TestStruct_;

static GBL_DOUBLY_LINKED_LIST_NODE(list1_);
static GBL_DOUBLY_LINKED_LIST_NODE(list2_);

static TestStruct_ testStructs_[6] = {
    { "node0", GBL_DOUBLY_LINKED_LIST_NODE_INITIALIZER() },
    { "node1", GBL_DOUBLY_LINKED_LIST_NODE_INITIALIZER() },
    { "node2", GBL_DOUBLY_LINKED_LIST_NODE_INITIALIZER() },
    { "node3", GBL_DOUBLY_LINKED_LIST_NODE_INITIALIZER() },
    { "node4", GBL_DOUBLY_LINKED_LIST_NODE_INITIALIZER() },
    { "node5", GBL_DOUBLY_LINKED_LIST_NODE_INITIALIZER() }
};

GBL_API GblDoublyLinkedList_test_testCaseInit(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}

GBL_API GblDoublyLinkedList_test_testCaseFinal(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}

GBL_API GblDoublyLinkedList_test_init(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_VERIFY(GblDoublyLinkedList_empty(&list1_));
    GBL_COMPARE(GblDoublyLinkedList_count(&list1_), 0);
    GBL_COMPARE(GblDoublyLinkedList_front(&list1_), NULL);
    GBL_COMPARE(GblDoublyLinkedList_back(&list1_), NULL);
    GBL_API_END();
}

GBL_API verifyDListNode_(GblContext* pCtx, GblDoublyLinkedListNode* pList, GblDoublyLinkedListNode* pNode, GblSize expectedIndex, const char* pString) {
    GBL_API_BEGIN(pCtx);
    TestStruct_* pTest = GBL_DOUBLY_LINKED_LIST_ENTRY(pNode,
                                                      TestStruct_,
                                                      listNode);
    GBL_VERIFY(strcmp(pTest->pString, pString) == 0);
    GBL_COMPARE(GblDoublyLinkedList_find(pList, pNode), expectedIndex);
    GBL_COMPARE(GblDoublyLinkedList_at(pList, expectedIndex), pNode);
    GBL_VERIFY(GblDoublyLinkedList_contains(pList, pNode));
    GBL_API_END();
}

GBL_API GblDoublyLinkedList_test_pushBack(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedList_pushBack(&list1_, &testStructs_[1].listNode);
    GBL_VERIFY(!GblDoublyLinkedList_empty(&list1_));
    GBL_COMPARE(GblDoublyLinkedList_count(&list1_), 1);
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &list1_,
                                        GblDoublyLinkedList_front(&list1_),
                                        0,
                                        "node1"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &list1_,
                                        GblDoublyLinkedList_back(&list1_),
                                        0,
                                        "node1"));

    GblDoublyLinkedList_pushBack(&list1_, &testStructs_[2].listNode);
    GBL_COMPARE(GblDoublyLinkedList_count(&list1_), 2);
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &list1_,
                                        GblDoublyLinkedList_front(&list1_),
                                        0,
                                        "node1"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &list1_,
                                        GblDoublyLinkedList_back(&list1_),
                                        1,
                                        "node2"));
    GBL_API_END();
}

GBL_API GblDoublyLinkedList_test_pushFront(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedList_pushFront(&list1_, &testStructs_[0].listNode);
    GBL_COMPARE(GblDoublyLinkedList_count(&list1_), 3);
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &list1_,
                                        GblDoublyLinkedList_front(&list1_),
                                        0,
                                        "node0"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &list1_,
                                        GblDoublyLinkedList_back(&list1_),
                                        2,
                                        "node2"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &list1_,
                                        GblDoublyLinkedList_at(&list1_, 1),
                                        1,
                                        "node1"));

    GblDoublyLinkedList_pushFront(&list2_, &testStructs_[5].listNode);
    GblDoublyLinkedList_pushFront(&list2_, &testStructs_[3].listNode);
    GblDoublyLinkedList_pushFront(&list2_, &testStructs_[4].listNode);
    GBL_COMPARE(GblDoublyLinkedList_count(&list2_), 3);
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &list2_,
                                        GblDoublyLinkedList_front(&list2_),
                                        0,
                                        "node4"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &list2_,
                                        GblDoublyLinkedList_back(&list2_),
                                        2,
                                        "node5"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &list2_,
                                        GblDoublyLinkedList_at(&list2_, 1),
                                        1,
                                        "node3"));

    GBL_API_END();
}

GBL_API GblDoublyLinkedList_test_remove(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    //GBL_VERIFY(!GblDoublyLinkedList_remove(&list2_, &testStructs_[0].listNode));
    GBL_COMPARE(GblDoublyLinkedList_count(&list2_), 3);
    GblDoublyLinkedList_remove(&list2_, &testStructs_[3].listNode);
    GBL_COMPARE(GblDoublyLinkedList_count(&list2_), 2);
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &list2_,
                                        GblDoublyLinkedList_front(&list2_),
                                        0,
                                        "node4"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &list2_,
                                        GblDoublyLinkedList_back(&list2_),
                                        1,
                                        "node5"));
    GBL_API_END();
}

GBL_API GblDoublyLinkedList_test_moveBack(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedList_moveBack(&list1_, &testStructs_[3].listNode);
    GBL_COMPARE(GblDoublyLinkedList_count(&list1_), 4);
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &list1_,
                                        GblDoublyLinkedList_back(&list1_),
                                        3,
                                        "node3"));

    GblDoublyLinkedList_moveBack(&list1_, &testStructs_[4].listNode);
    GBL_COMPARE(GblDoublyLinkedList_count(&list1_), 5);
    GBL_COMPARE(GblDoublyLinkedList_count(&list2_), 1);
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &list1_,
                                        GblDoublyLinkedList_back(&list1_),
                                        4,
                                        "node4"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &list2_,
                                        GblDoublyLinkedList_back(&list2_),
                                        0,
                                        "node5"));
    GBL_API_END();
}

GBL_API GblDoublyLinkedList_test_moveFront(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedList_moveFront(&list1_, &testStructs_[5].listNode);
    GBL_COMPARE(GblDoublyLinkedList_count(&list1_), 6);
    GBL_COMPARE(GblDoublyLinkedList_count(&list2_), 0);
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &list1_,
                                        GblDoublyLinkedList_front(&list1_),
                                        0,
                                        "node5"));
    GblDoublyLinkedList_moveFront(&list1_, &testStructs_[0].listNode);
    GBL_COMPARE(GblDoublyLinkedList_count(&list1_), 6);
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &list1_,
                                        GblDoublyLinkedList_front(&list1_),
                                        0,
                                        "node0"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &list1_,
                                        GblDoublyLinkedList_at(&list1_, 1),
                                        1,
                                        "node5"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &list1_,
                                        GblDoublyLinkedList_back(&list1_),
                                        5,
                                        "node4"));

    GBL_API_END();
}

GBL_API GblDoublyLinkedList_test_joinBack(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedList_joinBack(&list2_, &list1_);
    GBL_COMPARE(GblDoublyLinkedList_count(&list2_), 6);
    GBL_COMPARE(GblDoublyLinkedList_count(&list1_), 0);
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &list2_,
                                        GblDoublyLinkedList_front(&list2_),
                                        0,
                                        "node0"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &list2_,
                                        GblDoublyLinkedList_at(&list2_, 1),
                                        1,
                                        "node5"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &list2_,
                                        GblDoublyLinkedList_back(&list2_),
                                        5,
                                        "node4"));

    GblDoublyLinkedList_moveBack(&list1_, GblDoublyLinkedList_front(&list2_));

    GblDoublyLinkedList_joinBack(&list2_, &list1_);
    GBL_COMPARE(GblDoublyLinkedList_count(&list2_), 6);
    GBL_COMPARE(GblDoublyLinkedList_count(&list1_), 0);
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &list2_,
                                        GblDoublyLinkedList_at(&list2_, 0),
                                        0,
                                        "node5"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &list2_,
                                        GblDoublyLinkedList_back(&list2_),
                                        5,
                                        "node0"));
    GBL_API_END();
}

GBL_API GblDoublyLinkedList_test_joinFront(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedList_joinFront(&list1_, &list2_);
    GBL_COMPARE(GblDoublyLinkedList_count(&list1_), 6);
    GBL_COMPARE(GblDoublyLinkedList_count(&list2_), 0);
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &list1_,
                                        GblDoublyLinkedList_at(&list1_, 0),
                                        0,
                                        "node5"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &list1_,
                                        GblDoublyLinkedList_back(&list1_),
                                        5,
                                        "node0"));

    GblDoublyLinkedList_moveBack(&list2_, GblDoublyLinkedList_back(&list1_));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &list2_,
                                        GblDoublyLinkedList_back(&list2_),
                                        0,
                                        "node0"));
    GblDoublyLinkedList_joinFront(&list1_, &list2_);
    GBL_COMPARE(GblDoublyLinkedList_count(&list1_), 6);
    GBL_COMPARE(GblDoublyLinkedList_count(&list2_), 0);
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &list1_,
                                        GblDoublyLinkedList_front(&list1_),
                                        0,
                                        "node0"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &list1_,
                                        GblDoublyLinkedList_at(&list1_, 1),
                                        1,
                                        "node5"));

    GBL_API_END();
}

GBL_API GblDoublyLinkedList_test_erase(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_VERIFY(!GblDoublyLinkedList_erase(&list2_, 1));
    GBL_VERIFY(GblDoublyLinkedList_erase(&list1_, 1));
    GBL_COMPARE(GblDoublyLinkedList_count(&list1_), 5);
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &list1_,
                                        GblDoublyLinkedList_at(&list1_, 1),
                                        1,
                                        "node1"));
    GBL_API_END();
}

GBL_API GblDoublyLinkedList_test_replace(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedList_pushBack(&list2_, &testStructs_[5].listNode);
    GblDoublyLinkedList_replace(&list1_, GblDoublyLinkedList_at(&list1_, 1), &testStructs_[5].listNode);
    GBL_COMPARE(GblDoublyLinkedList_count(&list2_), 0);
    GBL_COMPARE(GblDoublyLinkedList_count(&list1_), 5);
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &list1_,
                                        GblDoublyLinkedList_at(&list1_, 1),
                                        1,
                                        "node5"));
    GBL_API_END();
}

GBL_API GblDoublyLinkedList_test_swap(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedList_pushBack(&list2_, &testStructs_[1].listNode);
    GblDoublyLinkedList_swap(&list1_, GblDoublyLinkedList_at(&list1_, 1), GblDoublyLinkedList_front(&list2_));
    GBL_COMPARE(GblDoublyLinkedList_count(&list2_), 1);
    GBL_COMPARE(GblDoublyLinkedList_count(&list1_), 5);
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &list1_,
                                        GblDoublyLinkedList_at(&list1_, 1),
                                        1,
                                        "node1"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &list2_,
                                        GblDoublyLinkedList_at(&list2_, 0),
                                        0,
                                        "node5"));


    GBL_API_END();
}

GBL_API GblDoublyLinkedList_test_clear(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedList_clear(&list1_);
    GBL_VERIFY(GblDoublyLinkedList_empty(&list1_));
    GblDoublyLinkedList_clear(&list2_);
    GBL_COMPARE(GblDoublyLinkedList_count(&list2_), 0);
    GBL_API_END();
}

GBL_API GblDoublyLinkedList_test_final(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}

#include "test_gimbal_linked_list.h"
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/containers/gimbal_linked_list.h>

typedef struct TestStruct_ {
    char* pString;
    GblLinkedListNode listNode;
} TestStruct_;

static GBL_LINKED_LIST_NODE(list1_);
static GBL_LINKED_LIST_NODE(list2_);
static TestStruct_ testStructs_[6] = {
    { "node0", { NULL } },
    { "node1", { NULL } },
    { "node2", { NULL } },
    { "node3", { NULL } },
    { "node4", { NULL } },
    { "node5", { NULL } }
};

GBL_API GblLinkedList_test_testCaseInit(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}

GBL_API GblLinkedList_test_testCaseFinal(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}

GBL_API GblLinkedList_test_init(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_VERIFY(GblLinkedList_empty(&list1_));
    GBL_COMPARE(GblLinkedList_count(&list1_), 0);
    GBL_COMPARE(GblLinkedList_front(&list1_), NULL);
    GBL_COMPARE(GblLinkedList_back(&list1_), NULL);
    GBL_API_END();
}

GBL_API verifyListNode_(GblContext* pCtx, GblLinkedListNode* pList, GblLinkedListNode* pNode, GblSize expectedIndex, const char* pString) {
    GBL_API_BEGIN(pCtx);
    TestStruct_* pTest = GBL_LINKED_LIST_ENTRY(pNode,
                                               TestStruct_,
                                               listNode);
    GBL_VERIFY(strcmp(pTest->pString, pString) == 0);
    GBL_COMPARE(GblLinkedList_find(pList, pNode), expectedIndex);
    GBL_COMPARE(GblLinkedList_at(pList, expectedIndex), pNode);
    GBL_VERIFY(GblLinkedList_contains(pList, pNode));
    GBL_API_END();
}

GBL_API GblLinkedList_test_pushBack(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblLinkedList_pushBack(&list1_, &testStructs_[1].listNode);
    GBL_VERIFY(!GblLinkedList_empty(&list1_));
    GBL_COMPARE(GblLinkedList_count(&list1_), 1);
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1_,
                                        GblLinkedList_front(&list1_),
                                        0,
                                        "node1"));
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1_,
                                        GblLinkedList_back(&list1_),
                                        0,
                                        "node1"));

    GblLinkedList_pushBack(&list1_, &testStructs_[2].listNode);
    GBL_COMPARE(GblLinkedList_count(&list1_), 2);
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1_,
                                        GblLinkedList_front(&list1_),
                                        0,
                                        "node1"));
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1_,
                                        GblLinkedList_back(&list1_),
                                        1,
                                        "node2"));
    GBL_API_END();
}

GBL_API GblLinkedList_test_pushFront(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblLinkedList_pushFront(&list1_, &testStructs_[0].listNode);
    GBL_COMPARE(GblLinkedList_count(&list1_), 3);
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1_,
                                        GblLinkedList_front(&list1_),
                                        0,
                                        "node0"));
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1_,
                                        GblLinkedList_back(&list1_),
                                        2,
                                        "node2"));
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1_,
                                        GblLinkedList_at(&list1_, 1),
                                        1,
                                        "node1"));

    GblLinkedList_pushFront(&list2_, &testStructs_[5].listNode);
    GblLinkedList_pushFront(&list2_, &testStructs_[3].listNode);
    GblLinkedList_pushFront(&list2_, &testStructs_[4].listNode);
    GBL_COMPARE(GblLinkedList_count(&list2_), 3);
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list2_,
                                        GblLinkedList_front(&list2_),
                                        0,
                                        "node4"));
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list2_,
                                        GblLinkedList_back(&list2_),
                                        2,
                                        "node5"));
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list2_,
                                        GblLinkedList_at(&list2_, 1),
                                        1,
                                        "node3"));

    GBL_API_END();
}

GBL_API GblLinkedList_test_remove(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_VERIFY(!GblLinkedList_remove(&list2_, &testStructs_[0].listNode));
    GBL_COMPARE(GblLinkedList_count(&list2_), 3);
    GBL_VERIFY(GblLinkedList_remove(&list2_, &testStructs_[3].listNode));
    GBL_COMPARE(GblLinkedList_count(&list2_), 2);
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list2_,
                                        GblLinkedList_front(&list2_),
                                        0,
                                        "node4"));
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list2_,
                                        GblLinkedList_back(&list2_),
                                        1,
                                        "node5"));
    GBL_API_END();
}

GBL_API GblLinkedList_test_moveBack(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblLinkedList_moveBack(&list1_, &testStructs_[3].listNode);
    GBL_COMPARE(GblLinkedList_count(&list1_), 4);
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1_,
                                        GblLinkedList_back(&list1_),
                                        3,
                                        "node3"));

    GblLinkedList_moveBack(&list1_, &testStructs_[4].listNode);
    GBL_COMPARE(GblLinkedList_count(&list1_), 5);
    GBL_COMPARE(GblLinkedList_count(&list2_), 1);
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1_,
                                        GblLinkedList_back(&list1_),
                                        4,
                                        "node4"));
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list2_,
                                        GblLinkedList_back(&list2_),
                                        0,
                                        "node5"));
    GBL_API_END();
}

GBL_API GblLinkedList_test_moveFront(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblLinkedList_moveFront(&list1_, &testStructs_[5].listNode);
    GBL_COMPARE(GblLinkedList_count(&list1_), 6);
    GBL_COMPARE(GblLinkedList_count(&list2_), 0);
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1_,
                                        GblLinkedList_front(&list1_),
                                        0,
                                        "node5"));
    GblLinkedList_moveFront(&list1_, &testStructs_[0].listNode);
    GBL_COMPARE(GblLinkedList_count(&list1_), 6);
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1_,
                                        GblLinkedList_front(&list1_),
                                        0,
                                        "node0"));
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1_,
                                        GblLinkedList_at(&list1_, 1),
                                        1,
                                        "node5"));
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1_,
                                        GblLinkedList_back(&list1_),
                                        5,
                                        "node4"));

    GBL_API_END();
}

GBL_API GblLinkedList_test_joinBack(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblLinkedList_joinBack(&list2_, &list1_);
    GBL_COMPARE(GblLinkedList_count(&list2_), 6);
    GBL_COMPARE(GblLinkedList_count(&list1_), 0);
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list2_,
                                        GblLinkedList_front(&list2_),
                                        0,
                                        "node0"));
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list2_,
                                        GblLinkedList_at(&list2_, 1),
                                        1,
                                        "node5"));
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list2_,
                                        GblLinkedList_back(&list2_),
                                        5,
                                        "node4"));

    GblLinkedList_moveBack(&list1_, GblLinkedList_front(&list2_));

    GblLinkedList_joinBack(&list2_, &list1_);
    GBL_COMPARE(GblLinkedList_count(&list2_), 6);
    GBL_COMPARE(GblLinkedList_count(&list1_), 0);
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list2_,
                                        GblLinkedList_at(&list2_, 0),
                                        0,
                                        "node5"));
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list2_,
                                        GblLinkedList_back(&list2_),
                                        5,
                                        "node0"));
    GBL_API_END();
}

GBL_API GblLinkedList_test_joinFront(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblLinkedList_joinFront(&list1_, &list2_);
    GBL_COMPARE(GblLinkedList_count(&list1_), 6);
    GBL_COMPARE(GblLinkedList_count(&list2_), 0);
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1_,
                                        GblLinkedList_at(&list1_, 0),
                                        0,
                                        "node5"));
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1_,
                                        GblLinkedList_back(&list1_),
                                        5,
                                        "node0"));

    GblLinkedList_moveBack(&list2_, GblLinkedList_back(&list1_));
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list2_,
                                        GblLinkedList_back(&list2_),
                                        0,
                                        "node0"));
    GblLinkedList_joinFront(&list1_, &list2_);
    GBL_COMPARE(GblLinkedList_count(&list1_), 6);
    GBL_COMPARE(GblLinkedList_count(&list2_), 0);
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1_,
                                        GblLinkedList_front(&list1_),
                                        0,
                                        "node0"));
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1_,
                                        GblLinkedList_at(&list1_, 1),
                                        1,
                                        "node5"));

    GBL_API_END();
}

GBL_API GblLinkedList_test_erase(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_VERIFY(!GblLinkedList_erase(&list2_, 1));
    GBL_VERIFY(GblLinkedList_erase(&list1_, 1));
    GBL_COMPARE(GblLinkedList_count(&list1_), 5);
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1_,
                                        GblLinkedList_at(&list1_, 1),
                                        1,
                                        "node1"));
    GBL_API_END();
}

GBL_API GblLinkedList_test_replace(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblLinkedList_pushBack(&list2_, &testStructs_[5].listNode);
    GblLinkedList_replace(&list1_, GblLinkedList_at(&list1_, 1), &testStructs_[5].listNode);
    GBL_COMPARE(GblLinkedList_count(&list2_), 0);
    GBL_COMPARE(GblLinkedList_count(&list1_), 5);
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1_,
                                        GblLinkedList_at(&list1_, 1),
                                        1,
                                        "node5"));
    GBL_API_END();
}

GBL_API GblLinkedList_test_swap(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblLinkedList_pushBack(&list2_, &testStructs_[1].listNode);
    GblLinkedList_swap(&list1_, GblLinkedList_at(&list1_, 1), GblLinkedList_front(&list2_));
    GBL_COMPARE(GblLinkedList_count(&list2_), 1);
    GBL_COMPARE(GblLinkedList_count(&list1_), 5);
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1_,
                                        GblLinkedList_at(&list1_, 1),
                                        1,
                                        "node1"));
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list2_,
                                        GblLinkedList_at(&list2_, 0),
                                        0,
                                        "node5"));


    GBL_API_END();
}

GBL_API GblLinkedList_test_clear(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblLinkedList_clear(&list1_);
    GBL_VERIFY(GblLinkedList_empty(&list1_));
    GblLinkedList_clear(&list2_);
    GBL_COMPARE(GblLinkedList_count(&list2_), 0);
    GBL_API_END();
}

GBL_API GblLinkedList_test_final(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}

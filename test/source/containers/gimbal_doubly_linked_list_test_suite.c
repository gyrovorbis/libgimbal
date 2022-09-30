#include "containers/gimbal_doubly_linked_list_test_suite.h"
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/containers/gimbal_doubly_linked_list.h>
#include <gimbal/test/gimbal_test.h>
#include <string.h>

#define GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(inst)    ((GblDoublyLinkedListTestSuite_*)GBL_INSTANCE_PRIVATE(inst, GBL_DOUBLY_LINKED_LIST_TEST_SUITE_TYPE))

typedef struct TestStruct_ {
    char* pString;
    GblDoublyLinkedListNode listNode;
} TestStruct_;

typedef struct GblDoublyLinkedListTestSuite_ {
    GblDoublyLinkedListNode   list1;
    GblDoublyLinkedListNode   list2;
    GblDoublyLinkedListNode   list3;
    TestStruct_               testStructs[11];
} GblDoublyLinkedListTestSuite_;

static GBL_RESULT GblDoublyLinkedListTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);
    memset(pSelf_, 0, sizeof(GblDoublyLinkedListTestSuite_));
    pSelf_->testStructs[0].pString = "node0";
    GblDoublyLinkedList_init(&pSelf_->list1);
    GblDoublyLinkedList_init(&pSelf_->list2);
    pSelf_->testStructs[1].pString = "node1";
    pSelf_->testStructs[2].pString = "node2";
    pSelf_->testStructs[3].pString = "node3";
    pSelf_->testStructs[4].pString = "node4";
    pSelf_->testStructs[5].pString = "node5";
    pSelf_->testStructs[6].pString = "node6";
    pSelf_->testStructs[7].pString = "node7";
    pSelf_->testStructs[8].pString = "node8";
    pSelf_->testStructs[9].pString = "node9";
    pSelf_->testStructs[10].pString = "node10";
    GBL_API_END();
}

static GBL_RESULT GblDoublyLinkedListTestSuite_empty_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);
    GBL_TEST_VERIFY(GblDoublyLinkedList_empty(&pSelf_->list1));
    GBL_TEST_COMPARE(GblDoublyLinkedList_count(&pSelf_->list1), 0);
    GBL_TEST_COMPARE(GblDoublyLinkedList_front(&pSelf_->list1), NULL);
    GBL_TEST_COMPARE(GblDoublyLinkedList_back(&pSelf_->list1), NULL);
    GBL_API_END();
}

static GBL_RESULT verifyDListNode_(GblContext* pCtx, GblDoublyLinkedListNode* pList, GblDoublyLinkedListNode* pNode, intptr_t expectedIndex, const char* pString) {
    GBL_API_BEGIN(pCtx);
    TestStruct_* pTest = GBL_DOUBLY_LINKED_LIST_ENTRY(pNode,
                                                      TestStruct_,
                                                      listNode);
    GBL_TEST_COMPARE(pTest->pString, pString);
    if(expectedIndex > 0)
        GBL_TEST_COMPARE(GblDoublyLinkedList_find(pList, pNode), expectedIndex);
    GBL_TEST_COMPARE(GblDoublyLinkedList_at(pList, expectedIndex), pNode);
    GBL_TEST_VERIFY(GblDoublyLinkedList_contains(pList, pNode));
    GBL_API_END();
}

static GBL_RESULT GblDoublyLinkedListTestSuite_pushBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);
    GblDoublyLinkedList_pushBack(&pSelf_->list1, &pSelf_->testStructs[1].listNode);
    GBL_TEST_VERIFY(!GblDoublyLinkedList_empty(&pSelf_->list1));
    GBL_TEST_COMPARE(GblDoublyLinkedList_count(&pSelf_->list1), 1);
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblDoublyLinkedList_front(&pSelf_->list1),
                                        0,
                                        "node1"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblDoublyLinkedList_back(&pSelf_->list1),
                                        0,
                                        "node1"));

    GblDoublyLinkedList_pushBack(&pSelf_->list1, &pSelf_->testStructs[2].listNode);
    GBL_TEST_COMPARE(GblDoublyLinkedList_count(&pSelf_->list1), 2);
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblDoublyLinkedList_front(&pSelf_->list1),
                                        0,
                                        "node1"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblDoublyLinkedList_back(&pSelf_->list1),
                                        1,
                                        "node2"));
    GBL_API_END();
}

static GBL_RESULT GblDoublyLinkedListTestSuite_pushFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);
    GblDoublyLinkedList_pushFront(&pSelf_->list1, &pSelf_->testStructs[0].listNode);
    GBL_TEST_COMPARE(GblDoublyLinkedList_count(&pSelf_->list1), 3);
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblDoublyLinkedList_front(&pSelf_->list1),
                                        0,
                                        "node0"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblDoublyLinkedList_back(&pSelf_->list1),
                                        2,
                                        "node2"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblDoublyLinkedList_at(&pSelf_->list1, 1),
                                        1,
                                        "node1"));

    GblDoublyLinkedList_pushFront(&pSelf_->list2, &pSelf_->testStructs[5].listNode);
    GblDoublyLinkedList_pushFront(&pSelf_->list2, &pSelf_->testStructs[3].listNode);
    GblDoublyLinkedList_pushFront(&pSelf_->list2, &pSelf_->testStructs[4].listNode);
    GBL_TEST_COMPARE(GblDoublyLinkedList_count(&pSelf_->list2), 3);
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblDoublyLinkedList_front(&pSelf_->list2),
                                        0,
                                        "node4"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblDoublyLinkedList_back(&pSelf_->list2),
                                        2,
                                        "node5"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblDoublyLinkedList_at(&pSelf_->list2, 1),
                                        1,
                                        "node3"));

    GBL_API_END();
}


static GBL_RESULT GblDoublyLinkedListTestSuite_popBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    TestStruct_ test = {
        .pString = "test",
        GBL_DOUBLY_LINKED_LIST_NODE_INITIALIZER()
    };

    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);
    GblDoublyLinkedList_pushBack(&pSelf_->list1, &test.listNode);

    const GblSize count = GblDoublyLinkedList_count(&pSelf_->list1);
    GblDoublyLinkedListNode* pNode = GblDoublyLinkedList_popBack(&pSelf_->list1);

    GBL_TEST_COMPARE(GblDoublyLinkedList_count(&pSelf_->list1), count-1);
    GBL_TEST_COMPARE(GBL_DOUBLY_LINKED_LIST_ENTRY(pNode, TestStruct_, listNode), &test);

    GBL_API_END();
}

static GBL_RESULT GblDoublyLinkedListTestSuite_popFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    TestStruct_ test = {
        .pString = "test",
        GBL_DOUBLY_LINKED_LIST_NODE_INITIALIZER()
    };

    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);
    GblDoublyLinkedList_pushFront(&pSelf_->list1, &test.listNode);

    const GblSize count = GblDoublyLinkedList_count(&pSelf_->list1);
    GblDoublyLinkedListNode* pNode = GblDoublyLinkedList_popFront(&pSelf_->list1);

    GBL_TEST_COMPARE(GblDoublyLinkedList_count(&pSelf_->list1), count-1);
    GBL_TEST_COMPARE(GBL_DOUBLY_LINKED_LIST_ENTRY(pNode, TestStruct_, listNode), &test);

    GBL_API_END();
}


static GBL_RESULT GblDoublyLinkedListTestSuite_remove_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);
    //GBL_TEST_VERIFY(!GblDoublyLinkedList_remove(&pSelf_->list2, &pSelf_->testStructs[0].listNode));
    GBL_TEST_COMPARE(GblDoublyLinkedList_count(&pSelf_->list2), 3);
    GblDoublyLinkedList_remove(&pSelf_->testStructs[3].listNode);
    GBL_TEST_COMPARE(GblDoublyLinkedList_count(&pSelf_->list2), 2);
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblDoublyLinkedList_front(&pSelf_->list2),
                                        0,
                                        "node4"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblDoublyLinkedList_back(&pSelf_->list2),
                                        1,
                                        "node5"));
    GBL_API_END();
}

static GBL_RESULT GblDoublyLinkedListTestSuite_moveBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);
    GblDoublyLinkedList_moveBack(&pSelf_->list1, &pSelf_->testStructs[3].listNode);
    GBL_TEST_COMPARE(GblDoublyLinkedList_count(&pSelf_->list1), 4);
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblDoublyLinkedList_back(&pSelf_->list1),
                                        3,
                                        "node3"));

    GblDoublyLinkedList_moveBack(&pSelf_->list1, &pSelf_->testStructs[4].listNode);
    GBL_TEST_COMPARE(GblDoublyLinkedList_count(&pSelf_->list1), 5);
    GBL_TEST_COMPARE(GblDoublyLinkedList_count(&pSelf_->list2), 1);
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblDoublyLinkedList_back(&pSelf_->list1),
                                        4,
                                        "node4"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblDoublyLinkedList_back(&pSelf_->list2),
                                        0,
                                        "node5"));
    GBL_API_END();
}

static GBL_RESULT GblDoublyLinkedListTestSuite_moveFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);
    GblDoublyLinkedList_moveFront(&pSelf_->list1, &pSelf_->testStructs[5].listNode);
    GBL_TEST_COMPARE(GblDoublyLinkedList_count(&pSelf_->list1), 6);
    GBL_TEST_COMPARE(GblDoublyLinkedList_count(&pSelf_->list2), 0);
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblDoublyLinkedList_front(&pSelf_->list1),
                                        0,
                                        "node5"));
    GblDoublyLinkedList_moveFront(&pSelf_->list1, &pSelf_->testStructs[0].listNode);
    GBL_TEST_COMPARE(GblDoublyLinkedList_count(&pSelf_->list1), 6);
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblDoublyLinkedList_front(&pSelf_->list1),
                                        0,
                                        "node0"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblDoublyLinkedList_at(&pSelf_->list1, 1),
                                        1,
                                        "node5"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblDoublyLinkedList_back(&pSelf_->list1),
                                        5,
                                        "node4"));

    GBL_API_END();
}

static GBL_RESULT GblDoublyLinkedListTestSuite_joinBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);
    GblDoublyLinkedList_joinBack(&pSelf_->list2, &pSelf_->list1);
    GBL_TEST_COMPARE(GblDoublyLinkedList_count(&pSelf_->list2), 6);
    GBL_TEST_COMPARE(GblDoublyLinkedList_count(&pSelf_->list1), 0);
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblDoublyLinkedList_front(&pSelf_->list2),
                                        0,
                                        "node0"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblDoublyLinkedList_at(&pSelf_->list2, 1),
                                        1,
                                        "node5"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblDoublyLinkedList_back(&pSelf_->list2),
                                        5,
                                        "node4"));

    GblDoublyLinkedList_moveBack(&pSelf_->list1, GblDoublyLinkedList_front(&pSelf_->list2));

    GblDoublyLinkedList_joinBack(&pSelf_->list2, &pSelf_->list1);
    GBL_TEST_COMPARE(GblDoublyLinkedList_count(&pSelf_->list2), 6);
    GBL_TEST_COMPARE(GblDoublyLinkedList_count(&pSelf_->list1), 0);
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblDoublyLinkedList_at(&pSelf_->list2, 0),
                                        0,
                                        "node5"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblDoublyLinkedList_back(&pSelf_->list2),
                                        5,
                                        "node0"));
    GBL_API_END();
}

static GBL_RESULT GblDoublyLinkedListTestSuite_joinFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);
    GblDoublyLinkedList_joinFront(&pSelf_->list1, &pSelf_->list2);
    GBL_TEST_COMPARE(GblDoublyLinkedList_count(&pSelf_->list1), 6);
    GBL_TEST_COMPARE(GblDoublyLinkedList_count(&pSelf_->list2), 0);
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblDoublyLinkedList_at(&pSelf_->list1, 0),
                                        0,
                                        "node5"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblDoublyLinkedList_back(&pSelf_->list1),
                                        5,
                                        "node0"));

    GblDoublyLinkedList_moveBack(&pSelf_->list2, GblDoublyLinkedList_back(&pSelf_->list1));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblDoublyLinkedList_back(&pSelf_->list2),
                                        0,
                                        "node0"));
    GblDoublyLinkedList_joinFront(&pSelf_->list1, &pSelf_->list2);
    GBL_TEST_COMPARE(GblDoublyLinkedList_count(&pSelf_->list1), 6);
    GBL_TEST_COMPARE(GblDoublyLinkedList_count(&pSelf_->list2), 0);
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblDoublyLinkedList_front(&pSelf_->list1),
                                        0,
                                        "node0"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblDoublyLinkedList_at(&pSelf_->list1, 1),
                                        1,
                                        "node5"));

    GBL_API_END();
}

static GBL_RESULT GblDoublyLinkedListTestSuite_erase_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);
    GBL_TEST_VERIFY(!GblDoublyLinkedList_erase(&pSelf_->list2, 1));
    GBL_TEST_VERIFY(GblDoublyLinkedList_erase(&pSelf_->list1, 1));
    GBL_TEST_COMPARE(GblDoublyLinkedList_count(&pSelf_->list1), 5);
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblDoublyLinkedList_at(&pSelf_->list1, 1),
                                        1,
                                        "node1"));
    GBL_API_END();
}

static GBL_RESULT GblDoublyLinkedListTestSuite_replace_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);
    GblDoublyLinkedList_pushBack(&pSelf_->list2, &pSelf_->testStructs[5].listNode);
    GblDoublyLinkedList_replace(GblDoublyLinkedList_at(&pSelf_->list1, 1), &pSelf_->testStructs[5].listNode);
    GBL_TEST_COMPARE(GblDoublyLinkedList_count(&pSelf_->list2), 0);
    GBL_TEST_COMPARE(GblDoublyLinkedList_count(&pSelf_->list1), 5);
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblDoublyLinkedList_at(&pSelf_->list1, 1),
                                        1,
                                        "node5"));
    GBL_API_END();
}

static GBL_RESULT GblDoublyLinkedListTestSuite_swap_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);
    GblDoublyLinkedList_pushBack(&pSelf_->list2, &pSelf_->testStructs[1].listNode);
    GblDoublyLinkedList_swap(GblDoublyLinkedList_at(&pSelf_->list1, 1), GblDoublyLinkedList_front(&pSelf_->list2));
    GBL_TEST_COMPARE(GblDoublyLinkedList_count(&pSelf_->list2), 1);
    GBL_TEST_COMPARE(GblDoublyLinkedList_count(&pSelf_->list1), 5);
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblDoublyLinkedList_at(&pSelf_->list1, 1),
                                        1,
                                        "node1"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblDoublyLinkedList_at(&pSelf_->list2, 0),
                                        0,
                                        "node5"));


    GBL_API_END();
}

static GBL_RESULT GblDoublyLinkedListTestSuite_clear_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);
    GblDoublyLinkedList_clear(&pSelf_->list1);
    GBL_TEST_VERIFY(GblDoublyLinkedList_empty(&pSelf_->list1));
    GblDoublyLinkedList_clear(&pSelf_->list2);
    GBL_TEST_COMPARE(GblDoublyLinkedList_count(&pSelf_->list2), 0);
    GBL_API_END();
}


static GBL_RESULT GblDoublyLinkedListTestSuite_reverse_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);

    GblDoublyLinkedList_pushBack(&pSelf_->list1, &pSelf_->testStructs[0].listNode);
    GblDoublyLinkedList_pushBack(&pSelf_->list1, &pSelf_->testStructs[1].listNode);
    GblDoublyLinkedList_pushBack(&pSelf_->list1, &pSelf_->testStructs[2].listNode);

    GblDoublyLinkedList_reverse(&pSelf_->list1);


    GBL_TEST_COMPARE(GblDoublyLinkedList_count(&pSelf_->list1), 3);
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[2].listNode,
                                         0,
                                         "node2"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[1].listNode,
                                         1,
                                         "node1"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[0].listNode,
                                         2,
                                         "node0"));

    GBL_API_END();
}

static GBL_RESULT GblDoublyLinkedListTestSuite_atNegative_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE(GblDoublyLinkedList_count(&pSelf_->list1), 3);
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[2].listNode,
                                         -3,
                                         "node2"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[1].listNode,
                                         -2,
                                         "node1"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[0].listNode,
                                         -1,
                                         "node0"));

    GBL_API_END();
}


static GBL_RESULT GblDoublyLinkedListTestSuite_eraseNegative_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);


    GBL_TEST_COMPARE(GblDoublyLinkedList_erase(&pSelf_->list1, -3), &pSelf_->testStructs[2].listNode);
    GBL_TEST_COMPARE(GblDoublyLinkedList_erase(&pSelf_->list1, -1), &pSelf_->testStructs[0].listNode);

    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[1].listNode,
                                         -1,
                                         "node1"));

    GBL_API_END();
}


static GBL_RESULT GblDoublyLinkedListTestSuite_insertBefore_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);


    GblDoublyLinkedList_insertBefore(&pSelf_->testStructs[1].listNode,
                                     &pSelf_->testStructs[2].listNode);

    GblDoublyLinkedList_insertBefore(&pSelf_->testStructs[1].listNode,
                                     &pSelf_->testStructs[3].listNode);

    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[2].listNode,
                                         0,
                                         "node2"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[3].listNode,
                                         1,
                                         "node3"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[1].listNode,
                                         2,
                                         "node1"));

    GBL_API_END();
}

static GBL_RESULT GblDoublyLinkedListTestSuite_insertAfter_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);


    GblDoublyLinkedList_insertAfter(&pSelf_->testStructs[1].listNode,
                                     &pSelf_->testStructs[0].listNode);

    GblDoublyLinkedList_insertAfter(&pSelf_->testStructs[2].listNode,
                                     &pSelf_->testStructs[4].listNode);

    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[2].listNode,
                                         0,
                                         "node2"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[4].listNode,
                                         1,
                                         "node4"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[3].listNode,
                                         2,
                                         "node3"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[1].listNode,
                                         3,
                                         "node1"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[0].listNode,
                                         4,
                                         "node0"));

    GBL_API_END();
}


static GBL_RESULT GblDoublyLinkedListTestSuite_insertFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);


    GBL_TEST_VERIFY(GblDoublyLinkedList_insert(&pSelf_->list1, 0,
                                               &pSelf_->testStructs[5].listNode));

    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[5].listNode,
                                         0,
                                         "node5"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[2].listNode,
                                         1,
                                         "node2"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[4].listNode,
                                         2,
                                         "node4"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[3].listNode,
                                         3,
                                         "node3"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[1].listNode,
                                         4,
                                         "node1"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[0].listNode,
                                         5,
                                         "node0"));

    GBL_API_END();
}

static GBL_RESULT GblDoublyLinkedListTestSuite_insertBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);


    GBL_TEST_VERIFY(GblDoublyLinkedList_insert(&pSelf_->list1, 6,
                                               &pSelf_->testStructs[6].listNode));

    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[5].listNode,
                                         0,
                                         "node5"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[2].listNode,
                                         1,
                                         "node2"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[4].listNode,
                                         2,
                                         "node4"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[3].listNode,
                                         3,
                                         "node3"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[1].listNode,
                                         4,
                                         "node1"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[0].listNode,
                                         5,
                                         "node0"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[6].listNode,
                                         6,
                                         "node6"));

    GBL_API_END();
}

static GBL_RESULT GblDoublyLinkedListTestSuite_insertMiddle_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);


    GBL_TEST_VERIFY(GblDoublyLinkedList_insert(&pSelf_->list1, 6,
                                               &pSelf_->testStructs[7].listNode));

    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[5].listNode,
                                         0,
                                         "node5"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[2].listNode,
                                         1,
                                         "node2"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[4].listNode,
                                         2,
                                         "node4"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[3].listNode,
                                         3,
                                         "node3"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[1].listNode,
                                         4,
                                         "node1"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[0].listNode,
                                         5,
                                         "node0"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[7].listNode,
                                         6,
                                         "node7"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[6].listNode,
                                         7,
                                         "node6"));

    GBL_API_END();
}

static GBL_RESULT GblDoublyLinkedListTestSuite_insertFrontNegative_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);


    GBL_TEST_VERIFY(GblDoublyLinkedList_insert(&pSelf_->list1, -9,
                                               &pSelf_->testStructs[8].listNode));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[8].listNode,
                                         0,
                                         "node8"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[5].listNode,
                                         1,
                                         "node5"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[2].listNode,
                                         2,
                                         "node2"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[4].listNode,
                                         3,
                                         "node4"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[3].listNode,
                                         4,
                                         "node3"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[1].listNode,
                                         5,
                                         "node1"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[0].listNode,
                                         6,
                                         "node0"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[7].listNode,
                                         7,
                                         "node7"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[6].listNode,
                                         8,
                                         "node6"));

    GBL_API_END();
}

static GBL_RESULT GblDoublyLinkedListTestSuite_insertBackNegative_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);


    GBL_TEST_VERIFY(GblDoublyLinkedList_insert(&pSelf_->list1, -1,
                                               &pSelf_->testStructs[9].listNode));

    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[8].listNode,
                                         0,
                                         "node8"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[5].listNode,
                                         1,
                                         "node5"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[2].listNode,
                                         2,
                                         "node2"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[4].listNode,
                                         3,
                                         "node4"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[3].listNode,
                                         4,
                                         "node3"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[1].listNode,
                                         5,
                                         "node1"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[0].listNode,
                                         6,
                                         "node0"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[7].listNode,
                                         7,
                                         "node7"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[6].listNode,
                                         8,
                                         "node6"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[9].listNode,
                                         9,
                                         "node9"));

    GBL_API_END();
}

static GBL_RESULT GblDoublyLinkedListTestSuite_insertMiddleNegative_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);


    GBL_TEST_VERIFY(GblDoublyLinkedList_insert(&pSelf_->list1, -3,
                                               &pSelf_->testStructs[10].listNode));

    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[8].listNode,
                                         0,
                                         "node8"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[5].listNode,
                                         1,
                                         "node5"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[2].listNode,
                                         2,
                                         "node2"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[4].listNode,
                                         3,
                                         "node4"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[3].listNode,
                                         4,
                                         "node3"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[1].listNode,
                                         5,
                                         "node1"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[0].listNode,
                                         6,
                                         "node0"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[7].listNode,
                                         7,
                                         "node7"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[10].listNode,
                                         8,
                                         "node10"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[6].listNode,
                                         9,
                                         "node6"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[9].listNode,
                                         10,
                                         "node9"));

    GBL_API_END();
}

static GBL_RESULT GblDoublyLinkedListTestSuite_rotateNegative_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);


    GblDoublyLinkedList_rotate(&pSelf_->list1, -3);

    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[10].listNode,
                                         0,
                                         "node10"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[6].listNode,
                                         1,
                                         "node6"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[9].listNode,
                                         2,
                                         "node9"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[8].listNode,
                                         3,
                                         "node8"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[5].listNode,
                                         4,
                                         "node5"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[2].listNode,
                                         5,
                                         "node2"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[4].listNode,
                                         6,
                                         "node4"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[3].listNode,
                                         7,
                                         "node3"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[1].listNode,
                                         8,
                                         "node1"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[0].listNode,
                                         9,
                                         "node0"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[7].listNode,
                                         10,
                                         "node7"));


    GBL_API_END();
}

static GBL_RESULT GblDoublyLinkedListTestSuite_rotate_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);


    GblDoublyLinkedList_rotate(&pSelf_->list1, 3);

    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[8].listNode,
                                         0,
                                         "node8"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[5].listNode,
                                         1,
                                         "node5"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[2].listNode,
                                         2,
                                         "node2"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[4].listNode,
                                         3,
                                         "node4"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[3].listNode,
                                         4,
                                         "node3"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[1].listNode,
                                         5,
                                         "node1"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[0].listNode,
                                         6,
                                         "node0"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[7].listNode,
                                         7,
                                         "node7"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[10].listNode,
                                         8,
                                         "node10"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[6].listNode,
                                         9,
                                         "node6"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[9].listNode,
                                         10,
                                         "node9"));

    GBL_API_END();
}


static GBL_RESULT GblDoublyLinkedListTestSuite_removeBefore_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);


    GBL_TEST_COMPARE(GblDoublyLinkedList_removeBefore(&pSelf_->testStructs[5].listNode),
                     &pSelf_->testStructs[8].listNode);

    GBL_TEST_COMPARE(GblDoublyLinkedList_removeBefore(&pSelf_->testStructs[9].listNode),
                     &pSelf_->testStructs[6].listNode);

    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[5].listNode,
                                         0,
                                         "node5"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[2].listNode,
                                         1,
                                         "node2"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[4].listNode,
                                         2,
                                         "node4"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[3].listNode,
                                         3,
                                         "node3"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[1].listNode,
                                         4,
                                         "node1"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[0].listNode,
                                         5,
                                         "node0"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[7].listNode,
                                         6,
                                         "node7"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[10].listNode,
                                         7,
                                         "node10"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[9].listNode,
                                         8,
                                         "node9"));

    GBL_API_END();
}

static GBL_RESULT GblDoublyLinkedListTestSuite_removeAfter_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);


    GBL_TEST_COMPARE(GblDoublyLinkedList_removeAfter(&pSelf_->testStructs[5].listNode),
                     &pSelf_->testStructs[2].listNode);

    GBL_TEST_COMPARE(GblDoublyLinkedList_removeAfter(&pSelf_->testStructs[10].listNode),
                     &pSelf_->testStructs[9].listNode);

    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[5].listNode,
                                         0,
                                         "node5"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[4].listNode,
                                         1,
                                         "node4"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[3].listNode,
                                         2,
                                         "node3"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[1].listNode,
                                         3,
                                         "node1"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[0].listNode,
                                         4,
                                         "node0"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[7].listNode,
                                         5,
                                         "node7"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[10].listNode,
                                         6,
                                         "node10"));

    GBL_API_END();
}


static GBL_RESULT GblDoublyLinkedListTestSuite_joinAtFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);


    GblDoublyLinkedList_init(&pSelf_->list2);
    GblDoublyLinkedList_pushBack(&pSelf_->list2, &pSelf_->testStructs[2].listNode);
    GblDoublyLinkedList_pushBack(&pSelf_->list2, &pSelf_->testStructs[9].listNode);

    GBL_TEST_VERIFY(GblDoublyLinkedList_join(&pSelf_->list1, 0, &pSelf_->list2));

    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[2].listNode,
                                         0,
                                         "node2"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[9].listNode,
                                         1,
                                         "node9"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[5].listNode,
                                         2,
                                         "node5"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[4].listNode,
                                         3,
                                         "node4"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[3].listNode,
                                         4,
                                         "node3"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[1].listNode,
                                         5,
                                         "node1"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[0].listNode,
                                         6,
                                         "node0"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[7].listNode,
                                         7,
                                         "node7"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[10].listNode,
                                         8,
                                         "node10"));
    GBL_API_END();
}

static GBL_RESULT GblDoublyLinkedListTestSuite_joinAtBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);


    GblDoublyLinkedList_init(&pSelf_->list3);
    GblDoublyLinkedList_pushBack(&pSelf_->list3, &pSelf_->testStructs[6].listNode);
    GblDoublyLinkedList_pushBack(&pSelf_->list3, &pSelf_->testStructs[8].listNode);

    GBL_TEST_VERIFY(GblDoublyLinkedList_join(&pSelf_->list1, 9, &pSelf_->list3));

    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[2].listNode,
                                         0,
                                         "node2"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[9].listNode,
                                         1,
                                         "node9"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[5].listNode,
                                         2,
                                         "node5"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[4].listNode,
                                         3,
                                         "node4"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[3].listNode,
                                         4,
                                         "node3"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[1].listNode,
                                         5,
                                         "node1"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[0].listNode,
                                         6,
                                         "node0"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[7].listNode,
                                         7,
                                         "node7"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[10].listNode,
                                         8,
                                         "node10"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[6].listNode,
                                         9,
                                         "node6"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[8].listNode,
                                         10,
                                         "node8"));
    GBL_API_END();
}


static GBL_RESULT GblDoublyLinkedListTestSuite_joinAtMiddle_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);


    GblDoublyLinkedList_init(&pSelf_->list3);
    GblDoublyLinkedList_remove(&pSelf_->testStructs[6].listNode);
    GblDoublyLinkedList_remove(&pSelf_->testStructs[8].listNode);
    GblDoublyLinkedList_pushBack(&pSelf_->list3, &pSelf_->testStructs[6].listNode);
    GblDoublyLinkedList_pushBack(&pSelf_->list3, &pSelf_->testStructs[8].listNode);

    GBL_TEST_VERIFY(GblDoublyLinkedList_join(&pSelf_->list1, 6, &pSelf_->list3));

    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[2].listNode,
                                         0,
                                         "node2"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[9].listNode,
                                         1,
                                         "node9"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[5].listNode,
                                         2,
                                         "node5"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[4].listNode,
                                         3,
                                         "node4"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[3].listNode,
                                         4,
                                         "node3"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[1].listNode,
                                         5,
                                         "node1"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[6].listNode,
                                         6,
                                         "node6"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[8].listNode,
                                         7,
                                         "node8"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[0].listNode,
                                         8,
                                         "node0"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[7].listNode,
                                         9,
                                         "node7"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[10].listNode,
                                         10,
                                         "node10"));
    GBL_API_END();
}


static GBL_RESULT GblDoublyLinkedListTestSuite_joinAtFrontNegative_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);


    GblDoublyLinkedList_init(&pSelf_->list3);
    GblDoublyLinkedList_remove(&pSelf_->testStructs[6].listNode);
    GblDoublyLinkedList_remove(&pSelf_->testStructs[8].listNode);
    GblDoublyLinkedList_pushBack(&pSelf_->list3, &pSelf_->testStructs[6].listNode);
    GblDoublyLinkedList_pushBack(&pSelf_->list3, &pSelf_->testStructs[8].listNode);

    GBL_TEST_VERIFY(GblDoublyLinkedList_join(&pSelf_->list1, -10, &pSelf_->list3));

    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[6].listNode,
                                         0,
                                         "node6"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[8].listNode,
                                         1,
                                         "node8"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[2].listNode,
                                         2,
                                         "node2"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[9].listNode,
                                         3,
                                         "node9"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[5].listNode,
                                         4,
                                         "node5"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[4].listNode,
                                         5,
                                         "node4"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[3].listNode,
                                         6,
                                         "node3"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[1].listNode,
                                         7,
                                         "node1"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[0].listNode,
                                         8,
                                         "node0"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[7].listNode,
                                         9,
                                         "node7"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[10].listNode,
                                         10,
                                         "node10"));
    GBL_API_END();
}

static GBL_RESULT GblDoublyLinkedListTestSuite_joinAtBackNegative_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);


    GblDoublyLinkedList_init(&pSelf_->list3);
    GblDoublyLinkedList_remove(&pSelf_->testStructs[6].listNode);
    GblDoublyLinkedList_remove(&pSelf_->testStructs[8].listNode);
    GblDoublyLinkedList_pushBack(&pSelf_->list3, &pSelf_->testStructs[6].listNode);
    GblDoublyLinkedList_pushBack(&pSelf_->list3, &pSelf_->testStructs[8].listNode);

    GBL_TEST_VERIFY(GblDoublyLinkedList_join(&pSelf_->list1, -1, &pSelf_->list3));

    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[2].listNode,
                                         0,
                                         "node2"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[9].listNode,
                                         1,
                                         "node9"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[5].listNode,
                                         2,
                                         "node5"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[4].listNode,
                                         3,
                                         "node4"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[3].listNode,
                                         4,
                                         "node3"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[1].listNode,
                                         5,
                                         "node1"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[0].listNode,
                                         6,
                                         "node0"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[7].listNode,
                                         7,
                                         "node7"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[10].listNode,
                                         8,
                                         "node10"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[6].listNode,
                                         9,
                                         "node6"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[8].listNode,
                                         10,
                                         "node8"));
    GBL_API_END();
}


static GBL_RESULT GblDoublyLinkedListTestSuite_joinAtMiddleNegative_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);


    GblDoublyLinkedList_init(&pSelf_->list3);
    GblDoublyLinkedList_remove(&pSelf_->testStructs[6].listNode);
    GblDoublyLinkedList_remove(&pSelf_->testStructs[8].listNode);
    GblDoublyLinkedList_pushBack(&pSelf_->list3, &pSelf_->testStructs[6].listNode);
    GblDoublyLinkedList_pushBack(&pSelf_->list3, &pSelf_->testStructs[8].listNode);

    GBL_TEST_VERIFY(GblDoublyLinkedList_join(&pSelf_->list1, -3, &pSelf_->list3));

    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[2].listNode,
                                         0,
                                         "node2"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[9].listNode,
                                         1,
                                         "node9"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[5].listNode,
                                         2,
                                         "node5"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[4].listNode,
                                         3,
                                         "node4"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[3].listNode,
                                         4,
                                         "node3"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[1].listNode,
                                         5,
                                         "node1"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[0].listNode,
                                         6,
                                         "node0"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[6].listNode,
                                         7,
                                         "node6"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[8].listNode,
                                         8,
                                         "node8"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[7].listNode,
                                         9,
                                         "node7"));
    GBL_API_VERIFY_CALL(verifyDListNode_(pCtx,
                                         &pSelf_->list1,
                                         &pSelf_->testStructs[10].listNode,
                                         10,
                                         "node10"));
    GBL_API_END();
}


GBL_EXPORT GblType GblDoublyLinkedListTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "empty",                GblDoublyLinkedListTestSuite_empty_                },
        { "pushBack",             GblDoublyLinkedListTestSuite_pushBack_             },
        { "pushFront",            GblDoublyLinkedListTestSuite_pushFront_            },
        { "popBack",              GblDoublyLinkedListTestSuite_popBack_              },
        { "popFront",             GblDoublyLinkedListTestSuite_popFront_             },
        { "remove",               GblDoublyLinkedListTestSuite_remove_               },
        { "moveBack",             GblDoublyLinkedListTestSuite_moveBack_             },
        { "moveFront",            GblDoublyLinkedListTestSuite_moveFront_            },
        { "joinBack",             GblDoublyLinkedListTestSuite_joinBack_             },
        { "joinFront",            GblDoublyLinkedListTestSuite_joinFront_            },
        { "erase",                GblDoublyLinkedListTestSuite_erase_                },
        { "replace",              GblDoublyLinkedListTestSuite_replace_              },
        { "swap",                 GblDoublyLinkedListTestSuite_swap_                 },
        { "clear",                GblDoublyLinkedListTestSuite_clear_                },
        { "reverse",              GblDoublyLinkedListTestSuite_reverse_              },
        { "atNegative",           GblDoublyLinkedListTestSuite_atNegative_           },
        { "eraseNegative",        GblDoublyLinkedListTestSuite_eraseNegative_        },
        { "insertBefore",         GblDoublyLinkedListTestSuite_insertBefore_         },
        { "insertAfter",          GblDoublyLinkedListTestSuite_insertAfter_          },
        { "insertFront",          GblDoublyLinkedListTestSuite_insertFront_          },
        { "insertBack",           GblDoublyLinkedListTestSuite_insertBack_           },
        { "insertMiddle",         GblDoublyLinkedListTestSuite_insertMiddle_         },
        { "insertFrontNegative",  GblDoublyLinkedListTestSuite_insertFrontNegative_  },
        { "insertBackNegative",   GblDoublyLinkedListTestSuite_insertBackNegative_   },
        { "insertMiddleNegative", GblDoublyLinkedListTestSuite_insertMiddleNegative_ },
        { "rotateNegative",       GblDoublyLinkedListTestSuite_rotateNegative_       },
        { "rotate",               GblDoublyLinkedListTestSuite_rotate_               },
        { "removeBefore",         GblDoublyLinkedListTestSuite_removeBefore_         },
        { "removeAfter",          GblDoublyLinkedListTestSuite_removeAfter_          },
        { "joinAtFront",          GblDoublyLinkedListTestSuite_joinAtFront_          },
        { "joinAtBack",           GblDoublyLinkedListTestSuite_joinAtBack_           },
        { "joinAtMiddle",         GblDoublyLinkedListTestSuite_joinAtMiddle_         },
        { "joinAtFrontNegative",  GblDoublyLinkedListTestSuite_joinAtFrontNegative_  },
        { "joinAtBackNegative",   GblDoublyLinkedListTestSuite_joinAtBackNegative_   },
        { "joinAtMiddleNegative", GblDoublyLinkedListTestSuite_joinAtMiddleNegative_ },
        { NULL,                   NULL                                               }
    };

    const static GblTestSuiteClassVTable vTable = {
        .pFnSuiteInit   = GblDoublyLinkedListTestSuite_init_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_API_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("GblDoublyLinkedListTestSuite"),
                                     &vTable,
                                     sizeof(GblDoublyLinkedListTestSuite),
                                     sizeof(GblDoublyLinkedListTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_API_VERIFY_LAST_RECORD();
        GBL_API_END_BLOCK();
    }
    return type;
}

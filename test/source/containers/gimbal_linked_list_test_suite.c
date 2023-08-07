#include "containers/gimbal_linked_list_test_suite.h"
#include <gimbal/core/gimbal_ctx.h>
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/containers/gimbal_linked_list.h>
#include <gimbal/meta/instances/gimbal_instance.h>

#define GBL_LINKED_LIST_TEST_SUITE_(inst)   (GBL_PRIVATE(GblLinkedListTestSuite, inst))

typedef struct TestStruct_ {
    char* pString;
    GblLinkedListNode listNode;
} TestStruct_;

typedef struct GblLinkedListTestSuite_ {
    GblLinkedListNode   list1;
    GblLinkedListNode   list2;
    TestStruct_         testStructs[9];
} GblLinkedListTestSuite_;

static GBL_RESULT GblLinkedListTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);
    pSelf_->list1.pNext = &pSelf_->list1;
    pSelf_->list2.pNext = &pSelf_->list2;
    pSelf_->testStructs[0].pString = "node0";
    pSelf_->testStructs[0].listNode.pNext = NULL;
    pSelf_->testStructs[1].pString = "node1";
    pSelf_->testStructs[1].listNode.pNext = NULL;
    pSelf_->testStructs[2].pString = "node2";
    pSelf_->testStructs[2].listNode.pNext = NULL;
    pSelf_->testStructs[3].pString = "node3";
    pSelf_->testStructs[3].listNode.pNext = NULL;
    pSelf_->testStructs[4].pString = "node4";
    pSelf_->testStructs[4].listNode.pNext = NULL;
    pSelf_->testStructs[5].pString = "node5";
    pSelf_->testStructs[5].listNode.pNext = NULL;
    pSelf_->testStructs[6].pString = "node6";
    pSelf_->testStructs[6].listNode.pNext = NULL;
    pSelf_->testStructs[7].pString = "node7";
    pSelf_->testStructs[7].listNode.pNext = NULL;
    pSelf_->testStructs[8].pString = "node8";
    pSelf_->testStructs[8].listNode.pNext = NULL;
    GBL_CTX_END();
}

static GBL_RESULT GblLinkedListTestSuite_empty_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);
    GBL_TEST_VERIFY(GblLinkedList_empty(&pSelf_->list1));
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 0);
    GBL_TEST_COMPARE(GblLinkedList_front(&pSelf_->list1), NULL);
    GBL_TEST_COMPARE(GblLinkedList_back(&pSelf_->list1), NULL);
    GBL_CTX_END();
}

static GBL_RESULT verifyListNode_(GblContext* pCtx,
                                  GblLinkedListNode* pList,
                                  GblLinkedListNode* pNode,
                                  size_t  expectedIndex,
                                  const char* pString)
{
    GBL_CTX_BEGIN(pCtx);
    TestStruct_* pTest = GBL_LINKED_LIST_ENTRY(pNode,
                                               TestStruct_,
                                               listNode);
    GBL_TEST_COMPARE(pTest->pString, pString);
    GBL_TEST_COMPARE(GblLinkedList_find(pList, pNode), expectedIndex);
    GBL_TEST_COMPARE(GblLinkedList_at(pList, expectedIndex), pNode);
    GBL_TEST_VERIFY(GblLinkedList_contains(pList, pNode));
    GBL_CTX_END();
}

static GBL_RESULT GblLinkedListTestSuite_pushBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);

    GblLinkedList_pushBack(&pSelf_->list1, &pSelf_->testStructs[1].listNode);
    GBL_TEST_VERIFY(!GblLinkedList_empty(&pSelf_->list1));
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 1);
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_front(&pSelf_->list1),
                                        0,
                                        "node1"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_back(&pSelf_->list1),
                                        0,
                                        "node1"));

    GblLinkedList_pushBack(&pSelf_->list1, &pSelf_->testStructs[2].listNode);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 2);
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_front(&pSelf_->list1),
                                        0,
                                        "node1"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_back(&pSelf_->list1),
                                        1,
                                        "node2"));
    GBL_CTX_END();
}

static GBL_RESULT GblLinkedListTestSuite_pushFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);

    GblLinkedList_pushFront(&pSelf_->list1, &pSelf_->testStructs[0].listNode);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 3);
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_front(&pSelf_->list1),
                                        0,
                                        "node0"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_back(&pSelf_->list1),
                                        2,
                                        "node2"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_at(&pSelf_->list1, 1),
                                        1,
                                        "node1"));

    GblLinkedList_pushFront(&pSelf_->list2, &pSelf_->testStructs[5].listNode);
    GblLinkedList_pushFront(&pSelf_->list2, &pSelf_->testStructs[3].listNode);
    GblLinkedList_pushFront(&pSelf_->list2, &pSelf_->testStructs[4].listNode);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list2), 3);
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblLinkedList_front(&pSelf_->list2),
                                        0,
                                        "node4"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblLinkedList_back(&pSelf_->list2),
                                        2,
                                        "node5"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblLinkedList_at(&pSelf_->list2, 1),
                                        1,
                                        "node3"));

    GBL_CTX_END();
}

static GBL_RESULT GblLinkedListTestSuite_popBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    TestStruct_ test = {
        .pString = "test",
        GBL_LINKED_LIST_NODE_INITIALIZER()
    };

    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);
    GblLinkedList_pushBack(&pSelf_->list1, &test.listNode);

    const size_t  count = GblLinkedList_count(&pSelf_->list1);
    GblLinkedListNode* pNode = GblLinkedList_popBack(&pSelf_->list1);

    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), count-1);
    GBL_TEST_COMPARE(GBL_LINKED_LIST_ENTRY(pNode, TestStruct_, listNode), &test);

    GBL_CTX_END();
}

static GBL_RESULT GblLinkedListTestSuite_popFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    TestStruct_ test = {
        .pString = "test",
        GBL_LINKED_LIST_NODE_INITIALIZER()
    };

    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);
    GblLinkedList_pushFront(&pSelf_->list1, &test.listNode);

    const size_t  count = GblLinkedList_count(&pSelf_->list1);
    GblLinkedListNode* pNode = GblLinkedList_popFront(&pSelf_->list1);

    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), count-1);
    GBL_TEST_COMPARE(GBL_LINKED_LIST_ENTRY(pNode, TestStruct_, listNode), &test);

    GBL_CTX_END();
}

static GBL_RESULT GblLinkedListTestSuite_remove_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_VERIFY(!GblLinkedList_remove(&pSelf_->list2, &pSelf_->testStructs[0].listNode));
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list2), 3);
    GBL_TEST_VERIFY(GblLinkedList_remove(&pSelf_->list2, &pSelf_->testStructs[3].listNode));
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list2), 2);
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblLinkedList_front(&pSelf_->list2),
                                        0,
                                        "node4"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblLinkedList_back(&pSelf_->list2),
                                        1,
                                        "node5"));
    GBL_CTX_END();
}

static GBL_RESULT GblLinkedListTestSuite_moveBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);
    GblLinkedList_moveBack(&pSelf_->list1, &pSelf_->testStructs[3].listNode);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 4);
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_back(&pSelf_->list1),
                                        3,
                                        "node3"));

    GblLinkedList_moveBack(&pSelf_->list1, &pSelf_->testStructs[4].listNode);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 5);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list2), 1);
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_back(&pSelf_->list1),
                                        4,
                                        "node4"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblLinkedList_back(&pSelf_->list2),
                                        0,
                                        "node5"));
    GBL_CTX_END();
}

static GBL_RESULT GblLinkedListTestSuite_moveFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);
    GblLinkedList_moveFront(&pSelf_->list1, &pSelf_->testStructs[5].listNode);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 6);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list2), 0);
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_front(&pSelf_->list1),
                                        0,
                                        "node5"));
    GblLinkedList_moveFront(&pSelf_->list1, &pSelf_->testStructs[0].listNode);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 6);
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_front(&pSelf_->list1),
                                        0,
                                        "node0"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_at(&pSelf_->list1, 1),
                                        1,
                                        "node5"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_back(&pSelf_->list1),
                                        5,
                                        "node4"));

    GBL_CTX_END();
}

static GBL_RESULT GblLinkedListTestSuite_joinBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);
    GblLinkedList_joinBack(&pSelf_->list2, &pSelf_->list1);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list2), 6);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 0);
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblLinkedList_front(&pSelf_->list2),
                                        0,
                                        "node0"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblLinkedList_at(&pSelf_->list2, 1),
                                        1,
                                        "node5"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblLinkedList_back(&pSelf_->list2),
                                        5,
                                        "node4"));

    GblLinkedList_moveBack(&pSelf_->list1, GblLinkedList_front(&pSelf_->list2));

    GblLinkedList_joinBack(&pSelf_->list2, &pSelf_->list1);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list2), 6);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 0);
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblLinkedList_at(&pSelf_->list2, 0),
                                        0,
                                        "node5"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblLinkedList_back(&pSelf_->list2),
                                        5,
                                        "node0"));
    GBL_CTX_END();
}

static GBL_RESULT GblLinkedListTestSuite_joinFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);
    GblLinkedList_joinFront(&pSelf_->list1, &pSelf_->list2);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 6);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list2), 0);
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_at(&pSelf_->list1, 0),
                                        0,
                                        "node5"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_back(&pSelf_->list1),
                                        5,
                                        "node0"));

    GblLinkedList_moveBack(&pSelf_->list2, GblLinkedList_back(&pSelf_->list1));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblLinkedList_back(&pSelf_->list2),
                                        0,
                                        "node0"));

    GblLinkedList_joinFront(&pSelf_->list1, &pSelf_->list2);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 6);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list2), 0);
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_front(&pSelf_->list1),
                                        0,
                                        "node0"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_at(&pSelf_->list1, 1),
                                        1,
                                        "node5"));

    GBL_CTX_END();
}

static GBL_RESULT GblLinkedListTestSuite_erase_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);
    GBL_TEST_VERIFY(!GblLinkedList_erase(&pSelf_->list2, 1));
    GBL_TEST_VERIFY(GblLinkedList_erase(&pSelf_->list1, 1));
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 5);
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_at(&pSelf_->list1, 1),
                                        1,
                                        "node1"));
    GBL_CTX_END();
}

static GBL_RESULT GblLinkedListTestSuite_replace_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);
    GblLinkedList_pushBack(&pSelf_->list2, &pSelf_->testStructs[5].listNode);
    GblLinkedList_replace(&pSelf_->list1, GblLinkedList_at(&pSelf_->list1, 1), &pSelf_->testStructs[5].listNode);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list2), 0);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 5);
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_at(&pSelf_->list1, 1),
                                        1,
                                        "node5"));
    GBL_CTX_END();
}

static GBL_RESULT GblLinkedListTestSuite_swap_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);
    GblLinkedList_pushBack(&pSelf_->list2, &pSelf_->testStructs[1].listNode);
    GblLinkedList_swap(&pSelf_->list1, GblLinkedList_at(&pSelf_->list1, 1), GblLinkedList_front(&pSelf_->list2));
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list2), 1);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 5);
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_at(&pSelf_->list1, 1),
                                        1,
                                        "node1"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblLinkedList_at(&pSelf_->list2, 0),
                                        0,
                                        "node5"));


    GBL_CTX_END();
}

static GBL_RESULT GblLinkedListTestSuite_clear_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);
    GblLinkedList_clear(&pSelf_->list1);
    GBL_TEST_VERIFY(GblLinkedList_empty(&pSelf_->list1));
    GblLinkedList_clear(&pSelf_->list2);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list2), 0);
    GBL_CTX_END();
}

static GBL_RESULT GblLinkedListTestSuite_reverse_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);

    GblLinkedList_pushBack(&pSelf_->list1, &pSelf_->testStructs[0].listNode);
    GblLinkedList_pushBack(&pSelf_->list1, &pSelf_->testStructs[1].listNode);
    GblLinkedList_pushBack(&pSelf_->list1, &pSelf_->testStructs[2].listNode);

    GblLinkedList_reverse(&pSelf_->list1);


    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 3);
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        &pSelf_->testStructs[2].listNode,
                                        0,
                                        "node2"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        &pSelf_->testStructs[1].listNode,
                                        1,
                                        "node1"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        &pSelf_->testStructs[0].listNode,
                                        2,
                                        "node0"));

    GBL_CTX_END();
}

static GBL_RESULT GblLinkedListTestSuite_insertAfter_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);

    GblLinkedList_insertAfter(&pSelf_->testStructs[2].listNode,
                              &pSelf_->testStructs[3].listNode);


    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 4);
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        &pSelf_->testStructs[2].listNode,
                                        0,
                                        "node2"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        &pSelf_->testStructs[3].listNode,
                                        1,
                                        "node3"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        &pSelf_->testStructs[1].listNode,
                                        2,
                                        "node1"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        &pSelf_->testStructs[0].listNode,
                                        3,
                                        "node0"));

    GBL_CTX_END();
}

static GBL_RESULT GblLinkedListTestSuite_insertFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_VERIFY(GblLinkedList_insert(&pSelf_->list1, &pSelf_->testStructs[4].listNode, 0));


    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 5);

    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        &pSelf_->testStructs[4].listNode,
                                        0,
                                        "node4"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        &pSelf_->testStructs[2].listNode,
                                        1,
                                        "node2"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        &pSelf_->testStructs[3].listNode,
                                        2,
                                        "node3"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        &pSelf_->testStructs[1].listNode,
                                        3,
                                        "node1"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        &pSelf_->testStructs[0].listNode,
                                        4,
                                        "node0"));

    GBL_CTX_END();
}

static GBL_RESULT GblLinkedListTestSuite_insertBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_VERIFY(GblLinkedList_insert(&pSelf_->list1, &pSelf_->testStructs[5].listNode, 5));


    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 6);

    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        &pSelf_->testStructs[4].listNode,
                                        0,
                                        "node4"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        &pSelf_->testStructs[2].listNode,
                                        1,
                                        "node2"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        &pSelf_->testStructs[3].listNode,
                                        2,
                                        "node3"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        &pSelf_->testStructs[1].listNode,
                                        3,
                                        "node1"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        &pSelf_->testStructs[0].listNode,
                                        4,
                                        "node0"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        &pSelf_->testStructs[5].listNode,
                                        5,
                                        "node5"));

    GBL_CTX_END();
}

static GBL_RESULT GblLinkedListTestSuite_insertMiddle_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_VERIFY(GblLinkedList_insert(&pSelf_->list1, &pSelf_->testStructs[6].listNode, 3));


    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 7);

    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        &pSelf_->testStructs[4].listNode,
                                        0,
                                        "node4"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        &pSelf_->testStructs[2].listNode,
                                        1,
                                        "node2"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        &pSelf_->testStructs[3].listNode,
                                        2,
                                        "node3"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        &pSelf_->testStructs[6].listNode,
                                        3,
                                        "node6"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        &pSelf_->testStructs[1].listNode,
                                        4,
                                        "node1"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        &pSelf_->testStructs[0].listNode,
                                        5,
                                        "node0"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        &pSelf_->testStructs[5].listNode,
                                        6,
                                        "node5"));

    GBL_CTX_END();
}



static GBL_RESULT GblLinkedListTestSuite_beforeMiddle_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE(GblLinkedList_beforeMiddle(&pSelf_->list1),
                     GblLinkedList_at(&pSelf_->list1, 3));

    GBL_CTX_END();
}

static GBL_RESULT GblLinkedListTestSuite_middle_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE(GblLinkedList_middle(&pSelf_->list1),
                     GblLinkedList_at(&pSelf_->list1, 4));

    GBL_CTX_END();
}

static GBL_RESULT GblLinkedListTestSuite_middleOf2_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);
    GBL_LINKED_LIST_NODE(list);

    GblLinkedList_pushBack(&list, &pSelf_->testStructs[7].listNode);
    GblLinkedList_pushBack(&list, &pSelf_->testStructs[8].listNode);

    GBL_TEST_VERIFY(GblLinkedList_middle(&list) == GblLinkedList_at(&list, 1));
    GBL_TEST_VERIFY(GblLinkedList_beforeMiddle(&list) == GblLinkedList_at(&list, 0));

    GBL_CTX_END();
}

static GBL_RESULT GblLinkedListTestSuite_splitAfter_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);

    GBL_LINKED_LIST_NODE(head);
    GblLinkedList_splitAfter(&pSelf_->list1, &head, GblLinkedList_at(&pSelf_->list1, 4));

    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 5);

    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        &pSelf_->testStructs[4].listNode,
                                        0,
                                        "node4"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        &pSelf_->testStructs[2].listNode,
                                        1,
                                        "node2"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        &pSelf_->testStructs[3].listNode,
                                        2,
                                        "node3"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        &pSelf_->testStructs[6].listNode,
                                        3,
                                        "node6"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        &pSelf_->testStructs[1].listNode,
                                        4,
                                        "node1"));

    GBL_TEST_COMPARE(GblLinkedList_count(&head), 2);

    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &head,
                                        &pSelf_->testStructs[0].listNode,
                                        0,
                                        "node0"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &head,
                                        &pSelf_->testStructs[5].listNode,
                                        1,
                                        "node5"));

    GblLinkedList_joinBack(&pSelf_->list1, &head);


    GBL_CTX_END();
}


static GBL_RESULT GblLinkedListTestSuite_splitAfterMiddle2_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);
    GBL_LINKED_LIST_NODE(list);
    GBL_LINKED_LIST_NODE(list2);

    GblLinkedList_pushBack(&list, &pSelf_->testStructs[7].listNode);
    GblLinkedList_pushBack(&list, &pSelf_->testStructs[8].listNode);

    GblLinkedList_splitAfter(&list, &list2, GblLinkedList_beforeMiddle(&list));

    GBL_TEST_COMPARE(GblLinkedList_count(&list), 1);
    GBL_TEST_COMPARE(GblLinkedList_count(&list2), 1);
    GBL_TEST_COMPARE(GblLinkedList_front(&list), &pSelf_->testStructs[7].listNode);
    GBL_TEST_COMPARE(GblLinkedList_front(&list2), &pSelf_->testStructs[8].listNode);

    GBL_CTX_END();
}

static GBL_RESULT GblLinkedListTestSuite_splitBefore_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);

    GBL_LINKED_LIST_NODE(head);
    GblLinkedList_splitAfter(&pSelf_->list1, &head, GblLinkedList_at(&pSelf_->list1, 0));

    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 1);

    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        &pSelf_->testStructs[4].listNode,
                                        0,
                                        "node4"));
    GBL_TEST_COMPARE(GblLinkedList_count(&head), 6);

    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &head,
                                        &pSelf_->testStructs[2].listNode,
                                        0,
                                        "node2"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &head,
                                        &pSelf_->testStructs[3].listNode,
                                        1,
                                        "node3"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &head,
                                        &pSelf_->testStructs[6].listNode,
                                        2,
                                        "node6"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &head,
                                        &pSelf_->testStructs[1].listNode,
                                        3,
                                        "node1"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &head,
                                        &pSelf_->testStructs[0].listNode,
                                        4,
                                        "node0"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &head,
                                        &pSelf_->testStructs[5].listNode,
                                        5,
                                        "node5"));

    GBL_CTX_END();
}


static int comparator_(const void* pValue1, const void* pValue2, void* pClosure) {
    GBL_UNUSED(pClosure);
    return strcmp((const char*)GBL_LINKED_LIST_ENTRY(pValue1, TestStruct_, listNode)->pString,
                  (const char*)GBL_LINKED_LIST_ENTRY(pValue2, TestStruct_, listNode)->pString);

}

static GBL_RESULT GblLinkedListTestSuite_joinSortedAppend_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);

    GBL_LINKED_LIST_NODE(list1);
    GBL_LINKED_LIST_NODE(list2);
    GblLinkedList_clear(&pSelf_->list1);

    GblLinkedList_pushBack(&list1,
                           &pSelf_->testStructs[0].listNode);

    GblLinkedList_pushBack(&list1,
                           &pSelf_->testStructs[1].listNode);

    GblLinkedList_pushBack(&list1,
                           &pSelf_->testStructs[2].listNode);

    GblLinkedList_pushBack(&list2,
                           &pSelf_->testStructs[3].listNode);

    GblLinkedList_pushBack(&list2,
                           &pSelf_->testStructs[4].listNode);

    GblLinkedList_joinSorted(&list1, &list2, comparator_, NULL);


    GBL_TEST_COMPARE(GblLinkedList_count(&list1), 5);

    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1,
                                        &pSelf_->testStructs[0].listNode,
                                        0,
                                        "node0"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1,
                                        &pSelf_->testStructs[1].listNode,
                                        1,
                                        "node1"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1,
                                        &pSelf_->testStructs[2].listNode,
                                        2,
                                        "node2"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1,
                                        &pSelf_->testStructs[3].listNode,
                                        3,
                                        "node3"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1,
                                        &pSelf_->testStructs[4].listNode,
                                        4,
                                        "node4"));

    GblLinkedList_clear(&list1);

    GBL_CTX_END();
}

static GBL_RESULT GblLinkedListTestSuite_joinSortedPrepend_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);

    GBL_LINKED_LIST_NODE(list1);
    GBL_LINKED_LIST_NODE(list2);

    GblLinkedList_pushBack(&list2,
                           &pSelf_->testStructs[0].listNode);

    GblLinkedList_pushBack(&list2,
                           &pSelf_->testStructs[1].listNode);

    GblLinkedList_pushBack(&list2,
                           &pSelf_->testStructs[2].listNode);

    GblLinkedList_pushBack(&list1,
                           &pSelf_->testStructs[3].listNode);

    GblLinkedList_pushBack(&list1,
                           &pSelf_->testStructs[4].listNode);

    GblLinkedList_pushBack(&list1,
                           &pSelf_->testStructs[5].listNode);

    GblLinkedList_joinSorted(&list1, &list2, comparator_, NULL);


    GBL_TEST_COMPARE(GblLinkedList_count(&list1), 6);

    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1,
                                        &pSelf_->testStructs[0].listNode,
                                        0,
                                        "node0"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1,
                                        &pSelf_->testStructs[1].listNode,
                                        1,
                                        "node1"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1,
                                        &pSelf_->testStructs[2].listNode,
                                        2,
                                        "node2"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1,
                                        &pSelf_->testStructs[3].listNode,
                                        3,
                                        "node3"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1,
                                        &pSelf_->testStructs[4].listNode,
                                        4,
                                        "node4"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1,
                                        &pSelf_->testStructs[5].listNode,
                                        5,
                                        "node5"));

    GblLinkedList_clear(&list1);

    GBL_CTX_END();
}


static GBL_RESULT GblLinkedListTestSuite_joinSorted2_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);

    GBL_LINKED_LIST_NODE(list1);
    GBL_LINKED_LIST_NODE(list2);

    GblLinkedList_pushBack(&list1,
                           &pSelf_->testStructs[0].listNode);

    GblLinkedList_pushBack(&list2,
                           &pSelf_->testStructs[1].listNode);


    GblLinkedList_joinSorted(&list1, &list2, comparator_, NULL);

    GBL_TEST_COMPARE(GblLinkedList_count(&list1), 2);

    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1,
                                        &pSelf_->testStructs[0].listNode,
                                        0,
                                        "node0"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1,
                                        &pSelf_->testStructs[1].listNode,
                                        1,
                                        "node1"));

    GblLinkedList_clear(&list1);


    GblLinkedList_pushBack(&list1,
                           &pSelf_->testStructs[1].listNode);

    GblLinkedList_pushBack(&list2,
                           &pSelf_->testStructs[0].listNode);


    GblLinkedList_joinSorted(&list1, &list2, comparator_, NULL);

    GBL_TEST_COMPARE(GblLinkedList_count(&list1), 2);

    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1,
                                        &pSelf_->testStructs[0].listNode,
                                        0,
                                        "node0"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1,
                                        &pSelf_->testStructs[1].listNode,
                                        1,
                                        "node1"));

    GblLinkedList_clear(&list1);

    GBL_CTX_END();
}

static GBL_RESULT GblLinkedListTestSuite_mergeSort_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);

    GBL_LINKED_LIST_NODE(list1);

    GblLinkedList_pushBack(&list1,
                           &pSelf_->testStructs[3].listNode);
    GblLinkedList_pushBack(&list1,
                           &pSelf_->testStructs[1].listNode);
    GblLinkedList_pushBack(&list1,
                           &pSelf_->testStructs[7].listNode);
    GblLinkedList_pushBack(&list1,
                           &pSelf_->testStructs[2].listNode);
    GblLinkedList_pushBack(&list1,
                           &pSelf_->testStructs[4].listNode);
    GblLinkedList_pushBack(&list1,
                           &pSelf_->testStructs[5].listNode);
    GblLinkedList_pushBack(&list1,
                           &pSelf_->testStructs[6].listNode);
    GblLinkedList_pushBack(&list1,
                           &pSelf_->testStructs[0].listNode);

    GblLinkedList_mergeSort(&list1, comparator_, NULL);


    GBL_TEST_COMPARE(GblLinkedList_count(&list1), 8);
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1,
                                        &pSelf_->testStructs[0].listNode,
                                        0,
                                        "node0"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1,
                                        &pSelf_->testStructs[1].listNode,
                                        1,
                                        "node1"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1,
                                        &pSelf_->testStructs[2].listNode,
                                        2,
                                        "node2"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1,
                                        &pSelf_->testStructs[3].listNode,
                                        3,
                                        "node3"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1,
                                        &pSelf_->testStructs[4].listNode,
                                        4,
                                        "node4"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1,
                                        &pSelf_->testStructs[5].listNode,
                                        5,
                                        "node5"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1,
                                        &pSelf_->testStructs[6].listNode,
                                        6,
                                        "node6"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1,
                                        &pSelf_->testStructs[7].listNode,
                                        7,
                                        "node7"));

    GblLinkedList_clear(&list1);

    GblLinkedList_pushBack(&list1,
                           &pSelf_->testStructs[7].listNode);
    GblLinkedList_pushBack(&list1,
                           &pSelf_->testStructs[4].listNode);
    GblLinkedList_pushBack(&list1,
                           &pSelf_->testStructs[3].listNode);
    GblLinkedList_pushBack(&list1,
                           &pSelf_->testStructs[0].listNode);
    GblLinkedList_pushBack(&list1,
                           &pSelf_->testStructs[1].listNode);
    GblLinkedList_pushBack(&list1,
                           &pSelf_->testStructs[5].listNode);
    GblLinkedList_pushBack(&list1,
                           &pSelf_->testStructs[6].listNode);
    GblLinkedList_pushBack(&list1,
                           &pSelf_->testStructs[2].listNode);

    GblLinkedList_mergeSort(&list1, comparator_, NULL);


    GBL_TEST_COMPARE(GblLinkedList_count(&list1), 8);
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1,
                                        &pSelf_->testStructs[0].listNode,
                                        0,
                                        "node0"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1,
                                        &pSelf_->testStructs[1].listNode,
                                        1,
                                        "node1"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1,
                                        &pSelf_->testStructs[2].listNode,
                                        2,
                                        "node2"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1,
                                        &pSelf_->testStructs[3].listNode,
                                        3,
                                        "node3"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1,
                                        &pSelf_->testStructs[4].listNode,
                                        4,
                                        "node4"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1,
                                        &pSelf_->testStructs[5].listNode,
                                        5,
                                        "node5"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1,
                                        &pSelf_->testStructs[6].listNode,
                                        6,
                                        "node6"));
    GBL_CTX_VERIFY_CALL(verifyListNode_(pCtx,
                                        &list1,
                                        &pSelf_->testStructs[7].listNode,
                                        7,
                                        "node7"));

    GblLinkedList_clear(&list1);
    GBL_CTX_END();
}


GBL_EXPORT GblType GblLinkedListTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "empty",              GblLinkedListTestSuite_empty_            },
        { "pushBack",           GblLinkedListTestSuite_pushBack_         },
        { "pushFront",          GblLinkedListTestSuite_pushFront_        },
        { "popBack",            GblLinkedListTestSuite_popBack_          },
        { "popFront",           GblLinkedListTestSuite_popFront_         },
        { "remove",             GblLinkedListTestSuite_remove_           },
        { "moveBack",           GblLinkedListTestSuite_moveBack_         },
        { "moveFront",          GblLinkedListTestSuite_moveFront_        },
        { "joinBack",           GblLinkedListTestSuite_joinBack_         },
        { "joinFront",          GblLinkedListTestSuite_joinFront_        },
        { "erase",              GblLinkedListTestSuite_erase_            },
        { "replace",            GblLinkedListTestSuite_replace_          },
        { "swap",               GblLinkedListTestSuite_swap_             },
        { "clear",              GblLinkedListTestSuite_clear_            },
        { "reverse",            GblLinkedListTestSuite_reverse_          },
        { "insertAfter",        GblLinkedListTestSuite_insertAfter_      },
        { "insertFront",        GblLinkedListTestSuite_insertFront_      },
        { "insertBack",         GblLinkedListTestSuite_insertBack_       },
        { "insertMiddle",       GblLinkedListTestSuite_insertMiddle_     },
        { "beforeMiddle",       GblLinkedListTestSuite_beforeMiddle_     },
        { "middle",             GblLinkedListTestSuite_middle_           },
        { "middleOf2",          GblLinkedListTestSuite_middleOf2_        },
        { "splitAfter",         GblLinkedListTestSuite_splitAfter_       },
        { "splitAfterMiddle2",  GblLinkedListTestSuite_splitAfterMiddle2_},
        { "splitBefore",        GblLinkedListTestSuite_splitBefore_      },
        { "joinSortedAppend",   GblLinkedListTestSuite_joinSortedAppend_ },
        { "joinSortedPrepend",  GblLinkedListTestSuite_joinSortedPrepend_},
        { "joinSorted2",        GblLinkedListTestSuite_joinSorted2_      },
        { "mergeSort",          GblLinkedListTestSuite_mergeSort_        },
        { NULL,                 NULL                                     },
    };

    const static GblTestSuiteVTable vTable = {
        .pFnSuiteInit   = GblLinkedListTestSuite_init_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStatic("GblLinkedListTestSuite"),
                                     &vTable,
                                     sizeof(GblLinkedListTestSuite),
                                     sizeof(GblLinkedListTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }
    return type;
}

#include "containers/gimbal_linked_list_test_suite.h"
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/test/gimbal_test.h>
#include <gimbal/containers/gimbal_linked_list.h>
#include <gimbal/meta/instances/gimbal_instance.h>

#define GBL_LINKED_LIST_TEST_SUITE_(inst)   ((GblLinkedListTestSuite_*)GBL_INSTANCE_PRIVATE(inst, GBL_LINKED_LIST_TEST_SUITE_TYPE))

typedef struct TestStruct_ {
    char* pString;
    GblLinkedListNode listNode;
} TestStruct_;

typedef struct GblLinkedListTestSuite_ {
    GblLinkedListNode   list1;
    GblLinkedListNode   list2;
    TestStruct_         testStructs[6];
} GblLinkedListTestSuite_;

static GBL_RESULT GblLinkedListTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
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
    GBL_API_END();
}

static GBL_RESULT GblLinkedListTestSuite_empty_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);
    GBL_TEST_VERIFY(GblLinkedList_empty(&pSelf_->list1));
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 0);
    GBL_TEST_COMPARE(GblLinkedList_front(&pSelf_->list1), NULL);
    GBL_TEST_COMPARE(GblLinkedList_back(&pSelf_->list1), NULL);
    GBL_API_END();
}

static GBL_RESULT verifyListNode_(GblContext* pCtx,
                                  GblLinkedListNode* pList,
                                  GblLinkedListNode* pNode,
                                  GblSize expectedIndex,
                                  const char* pString)
{
    GBL_API_BEGIN(pCtx);
    TestStruct_* pTest = GBL_LINKED_LIST_ENTRY(pNode,
                                               TestStruct_,
                                               listNode);
    GBL_TEST_COMPARE(pTest->pString, pString);
    GBL_TEST_COMPARE(GblLinkedList_find(pList, pNode), expectedIndex);
    GBL_TEST_COMPARE(GblLinkedList_at(pList, expectedIndex), pNode);
    GBL_TEST_VERIFY(GblLinkedList_contains(pList, pNode));
    GBL_API_END();
}

static GBL_RESULT GblLinkedListTestSuite_pushBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);

    GblLinkedList_pushBack(&pSelf_->list1, &pSelf_->testStructs[1].listNode);
    GBL_TEST_VERIFY(!GblLinkedList_empty(&pSelf_->list1));
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 1);
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_front(&pSelf_->list1),
                                        0,
                                        "node1"));
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_back(&pSelf_->list1),
                                        0,
                                        "node1"));

    GblLinkedList_pushBack(&pSelf_->list1, &pSelf_->testStructs[2].listNode);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 2);
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_front(&pSelf_->list1),
                                        0,
                                        "node1"));
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_back(&pSelf_->list1),
                                        1,
                                        "node2"));
    GBL_API_END();
}

static GBL_RESULT GblLinkedListTestSuite_pushFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);

    GblLinkedList_pushFront(&pSelf_->list1, &pSelf_->testStructs[0].listNode);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 3);
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_front(&pSelf_->list1),
                                        0,
                                        "node0"));
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_back(&pSelf_->list1),
                                        2,
                                        "node2"));
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_at(&pSelf_->list1, 1),
                                        1,
                                        "node1"));

    GblLinkedList_pushFront(&pSelf_->list2, &pSelf_->testStructs[5].listNode);
    GblLinkedList_pushFront(&pSelf_->list2, &pSelf_->testStructs[3].listNode);
    GblLinkedList_pushFront(&pSelf_->list2, &pSelf_->testStructs[4].listNode);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list2), 3);
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblLinkedList_front(&pSelf_->list2),
                                        0,
                                        "node4"));
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblLinkedList_back(&pSelf_->list2),
                                        2,
                                        "node5"));
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblLinkedList_at(&pSelf_->list2, 1),
                                        1,
                                        "node3"));

    GBL_API_END();
}

static GBL_RESULT GblLinkedListTestSuite_popBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    TestStruct_ test = {
        .pString = "test",
        GBL_LINKED_LIST_NODE_INITIALIZER()
    };

    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);
    GblLinkedList_pushBack(&pSelf_->list1, &test.listNode);

    const GblSize count = GblLinkedList_count(&pSelf_->list1);
    GblLinkedListNode* pNode = GblLinkedList_popBack(&pSelf_->list1);

    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), count-1);
    GBL_TEST_COMPARE(GBL_LINKED_LIST_ENTRY(pNode, TestStruct_, listNode), &test);

    GBL_API_END();
}

static GBL_RESULT GblLinkedListTestSuite_popFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    TestStruct_ test = {
        .pString = "test",
        GBL_LINKED_LIST_NODE_INITIALIZER()
    };

    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);
    GblLinkedList_pushFront(&pSelf_->list1, &test.listNode);

    const GblSize count = GblLinkedList_count(&pSelf_->list1);
    GblLinkedListNode* pNode = GblLinkedList_popFront(&pSelf_->list1);

    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), count-1);
    GBL_TEST_COMPARE(GBL_LINKED_LIST_ENTRY(pNode, TestStruct_, listNode), &test);

    GBL_API_END();
}

static GBL_RESULT GblLinkedListTestSuite_remove_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);

    GBL_TEST_VERIFY(!GblLinkedList_remove(&pSelf_->list2, &pSelf_->testStructs[0].listNode));
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list2), 3);
    GBL_TEST_VERIFY(GblLinkedList_remove(&pSelf_->list2, &pSelf_->testStructs[3].listNode));
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list2), 2);
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblLinkedList_front(&pSelf_->list2),
                                        0,
                                        "node4"));
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblLinkedList_back(&pSelf_->list2),
                                        1,
                                        "node5"));
    GBL_API_END();
}

static GBL_RESULT GblLinkedListTestSuite_moveBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);
    GblLinkedList_moveBack(&pSelf_->list1, &pSelf_->testStructs[3].listNode);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 4);
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_back(&pSelf_->list1),
                                        3,
                                        "node3"));

    GblLinkedList_moveBack(&pSelf_->list1, &pSelf_->testStructs[4].listNode);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 5);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list2), 1);
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_back(&pSelf_->list1),
                                        4,
                                        "node4"));
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblLinkedList_back(&pSelf_->list2),
                                        0,
                                        "node5"));
    GBL_API_END();
}

static GBL_RESULT GblLinkedListTestSuite_moveFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);
    GblLinkedList_moveFront(&pSelf_->list1, &pSelf_->testStructs[5].listNode);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 6);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list2), 0);
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_front(&pSelf_->list1),
                                        0,
                                        "node5"));
    GblLinkedList_moveFront(&pSelf_->list1, &pSelf_->testStructs[0].listNode);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 6);
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_front(&pSelf_->list1),
                                        0,
                                        "node0"));
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_at(&pSelf_->list1, 1),
                                        1,
                                        "node5"));
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_back(&pSelf_->list1),
                                        5,
                                        "node4"));

    GBL_API_END();
}

static GBL_RESULT GblLinkedListTestSuite_joinBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);
    GblLinkedList_joinBack(&pSelf_->list2, &pSelf_->list1);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list2), 6);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 0);
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblLinkedList_front(&pSelf_->list2),
                                        0,
                                        "node0"));
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblLinkedList_at(&pSelf_->list2, 1),
                                        1,
                                        "node5"));
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblLinkedList_back(&pSelf_->list2),
                                        5,
                                        "node4"));

    GblLinkedList_moveBack(&pSelf_->list1, GblLinkedList_front(&pSelf_->list2));

    GblLinkedList_joinBack(&pSelf_->list2, &pSelf_->list1);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list2), 6);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 0);
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblLinkedList_at(&pSelf_->list2, 0),
                                        0,
                                        "node5"));
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblLinkedList_back(&pSelf_->list2),
                                        5,
                                        "node0"));
    GBL_API_END();
}

static GBL_RESULT GblLinkedListTestSuite_joinFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);
    GblLinkedList_joinFront(&pSelf_->list1, &pSelf_->list2);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 6);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list2), 0);
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_at(&pSelf_->list1, 0),
                                        0,
                                        "node5"));
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_back(&pSelf_->list1),
                                        5,
                                        "node0"));

    GblLinkedList_moveBack(&pSelf_->list2, GblLinkedList_back(&pSelf_->list1));
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblLinkedList_back(&pSelf_->list2),
                                        0,
                                        "node0"));

    GblLinkedList_joinFront(&pSelf_->list1, &pSelf_->list2);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 6);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list2), 0);
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_front(&pSelf_->list1),
                                        0,
                                        "node0"));
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_at(&pSelf_->list1, 1),
                                        1,
                                        "node5"));

    GBL_API_END();
}

static GBL_RESULT GblLinkedListTestSuite_erase_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);
    GBL_TEST_VERIFY(!GblLinkedList_erase(&pSelf_->list2, 1));
    GBL_TEST_VERIFY(GblLinkedList_erase(&pSelf_->list1, 1));
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 5);
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_at(&pSelf_->list1, 1),
                                        1,
                                        "node1"));
    GBL_API_END();
}

static GBL_RESULT GblLinkedListTestSuite_replace_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);
    GblLinkedList_pushBack(&pSelf_->list2, &pSelf_->testStructs[5].listNode);
    GblLinkedList_replace(&pSelf_->list1, GblLinkedList_at(&pSelf_->list1, 1), &pSelf_->testStructs[5].listNode);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list2), 0);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 5);
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_at(&pSelf_->list1, 1),
                                        1,
                                        "node5"));
    GBL_API_END();
}

static GBL_RESULT GblLinkedListTestSuite_swap_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);
    GblLinkedList_pushBack(&pSelf_->list2, &pSelf_->testStructs[1].listNode);
    GblLinkedList_swap(&pSelf_->list1, GblLinkedList_at(&pSelf_->list1, 1), GblLinkedList_front(&pSelf_->list2));
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list2), 1);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 5);
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        GblLinkedList_at(&pSelf_->list1, 1),
                                        1,
                                        "node1"));
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list2,
                                        GblLinkedList_at(&pSelf_->list2, 0),
                                        0,
                                        "node5"));


    GBL_API_END();
}

static GBL_RESULT GblLinkedListTestSuite_clear_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);
    GblLinkedList_clear(&pSelf_->list1);
    GBL_TEST_VERIFY(GblLinkedList_empty(&pSelf_->list1));
    GblLinkedList_clear(&pSelf_->list2);
    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list2), 0);
    GBL_API_END();
}

static GBL_RESULT GblLinkedListTestSuite_reverse_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblLinkedListTestSuite_* pSelf_ = GBL_LINKED_LIST_TEST_SUITE_(pSelf);

    GblLinkedList_pushBack(&pSelf_->list1, &pSelf_->testStructs[0].listNode);
    GblLinkedList_pushBack(&pSelf_->list1, &pSelf_->testStructs[1].listNode);
    GblLinkedList_pushBack(&pSelf_->list1, &pSelf_->testStructs[2].listNode);

    GblLinkedList_reverse(&pSelf_->list1);


    GBL_TEST_COMPARE(GblLinkedList_count(&pSelf_->list1), 3);
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        &pSelf_->testStructs[2].listNode,
                                        0,
                                        "node2"));
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        &pSelf_->testStructs[1].listNode,
                                        1,
                                        "node1"));
    GBL_API_VERIFY_CALL(verifyListNode_(pCtx,
                                        &pSelf_->list1,
                                        &pSelf_->testStructs[0].listNode,
                                        2,
                                        "node0"));

    GBL_API_END();
}

GBL_EXPORT GblType GblLinkedListTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "empty",      GblLinkedListTestSuite_empty_       },
        { "pushBack",   GblLinkedListTestSuite_pushBack_    },
        { "pushFront",  GblLinkedListTestSuite_pushFront_   },
        { "popBack",    GblLinkedListTestSuite_popBack_     },
        { "popFront",   GblLinkedListTestSuite_popFront_    },
        { "remove",     GblLinkedListTestSuite_remove_      },
        { "moveBack",   GblLinkedListTestSuite_moveBack_    },
        { "moveFront",  GblLinkedListTestSuite_moveFront_   },
        { "joinBack",   GblLinkedListTestSuite_joinBack_    },
        { "joinFront",  GblLinkedListTestSuite_joinFront_   },
        { "erase",      GblLinkedListTestSuite_erase_       },
        { "replace",    GblLinkedListTestSuite_replace_     },
        { "swap",       GblLinkedListTestSuite_swap_        },
        { "clear",      GblLinkedListTestSuite_clear_       },
        { "reverse",    GblLinkedListTestSuite_reverse_     },
        { NULL,         NULL                                },
    };

    const static GblTestSuiteClassVTable vTable = {
        .pFnSuiteInit   = GblLinkedListTestSuite_init_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_API_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("LinkedListTestSuite"),
                                     &vTable,
                                     sizeof(GblLinkedListTestSuite),
                                     sizeof(GblLinkedListTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_API_VERIFY_LAST_RECORD();
        GBL_API_END_BLOCK();
    }
    return type;
}

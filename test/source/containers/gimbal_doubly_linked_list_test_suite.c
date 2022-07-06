#include "containers/gimbal_doubly_linked_list_test_suite.h"
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/containers/gimbal_doubly_linked_list.h>
#include <gimbal/utils/gimbal_test.h>
#include <string.h>

#define GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(inst)    ((GblDoublyLinkedListTestSuite_*)GBL_INSTANCE_PRIVATE(inst, GBL_DOUBLY_LINKED_LIST_TEST_SUITE_TYPE))

typedef struct TestStruct_ {
    char* pString;
    GblDoublyLinkedListNode listNode;
} TestStruct_;

typedef struct GblDoublyLinkedListTestSuite_ {
    GblDoublyLinkedListNode   list1;
    GblDoublyLinkedListNode   list2;
    TestStruct_               testStructs[6];
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

static GBL_RESULT verifyDListNode_(GblContext* pCtx, GblDoublyLinkedListNode* pList, GblDoublyLinkedListNode* pNode, GblSize expectedIndex, const char* pString) {
    GBL_API_BEGIN(pCtx);
    TestStruct_* pTest = GBL_DOUBLY_LINKED_LIST_ENTRY(pNode,
                                                      TestStruct_,
                                                      listNode);
    GBL_TEST_COMPARE_STRING(pTest->pString, pString);
    GBL_TEST_COMPARE_INT(GblDoublyLinkedList_find(pList, pNode), expectedIndex);
    GBL_TEST_COMPARE_POINTER(GblDoublyLinkedList_at(pList, expectedIndex), pNode);
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

static GBL_RESULT GblDoublyLinkedListTestSuite_remove_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblDoublyLinkedListTestSuite_* pSelf_ = GBL_DOUBLY_LINKED_LIST_TEST_SUITE_(pSelf);
    //GBL_TEST_VERIFY(!GblDoublyLinkedList_remove(&pSelf_->list2, &pSelf_->testStructs[0].listNode));
    GBL_TEST_COMPARE(GblDoublyLinkedList_count(&pSelf_->list2), 3);
    GblDoublyLinkedList_remove(&pSelf_->list2, &pSelf_->testStructs[3].listNode);
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
    GblDoublyLinkedList_replace(&pSelf_->list1, GblDoublyLinkedList_at(&pSelf_->list1, 1), &pSelf_->testStructs[5].listNode);
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
    GblDoublyLinkedList_swap(&pSelf_->list1, GblDoublyLinkedList_at(&pSelf_->list1, 1), GblDoublyLinkedList_front(&pSelf_->list2));
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


GBL_EXPORT GblType GblDoublyLinkedListTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "empty",      GblDoublyLinkedListTestSuite_empty_       },
        { "pushBack",   GblDoublyLinkedListTestSuite_pushBack_    },
        { "pushFront",  GblDoublyLinkedListTestSuite_pushFront_   },
        { "remove",     GblDoublyLinkedListTestSuite_remove_      },
        { "moveBack",   GblDoublyLinkedListTestSuite_moveBack_    },
        { "moveFront",  GblDoublyLinkedListTestSuite_moveFront_   },
        { "joinBack",   GblDoublyLinkedListTestSuite_joinBack_    },
        { "joinFront",  GblDoublyLinkedListTestSuite_joinFront_   },
        { "erase",      GblDoublyLinkedListTestSuite_erase_       },
        { "replace",    GblDoublyLinkedListTestSuite_replace_     },
        { "swap",       GblDoublyLinkedListTestSuite_swap_        },
        { "clear",      GblDoublyLinkedListTestSuite_clear_       },
        { NULL,         NULL                                      }
    };

    const static GblTestSuiteClassVTable vTable = {
        .pFnSuiteInit   = GblDoublyLinkedListTestSuite_init_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_API_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("DoublyLinkedListTestSuite"),
                                     &vTable,
                                     sizeof(GblDoublyLinkedListTestSuite),
                                     sizeof(GblDoublyLinkedListTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_API_VERIFY_LAST_RECORD();
        GBL_API_END_BLOCK();
    }
    return type;
}

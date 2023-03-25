#include "containers/gimbal_nary_tree_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/core/gimbal_ctx.h>
#include <gimbal/containers/gimbal_nary_tree.h>

#define GBL_NARY_TREE_TEST_SUITE_(inst)     ((GblNaryTreeTestSuite_*)GBL_INSTANCE_PRIVATE(inst, GBL_NARY_TREE_TEST_SUITE_TYPE))
#define NARY_OBJECT_(pSelf, index)          (&pSelf_->objects[index])
#define NARY_OBJECT_NODE_(pSelf_, index)    (&NARY_OBJECT_(pSelf_, index)->node)
#define NARY_OBJECT_OF_(nodePtr)            GBL_NARY_TREE_ENTRY(nodePtr, NaryObject_, node)
#define NARY_OBJECT_COUNT_                  11

typedef struct NaryObject_ {
    int             pad;
    GblNaryTreeNode node;
    int             id;
} NaryObject_;

typedef struct GblNaryTreeTestSuite_ {
    NaryObject_ objects[NARY_OBJECT_COUNT_+1];
} GblNaryTreeTestSuite_;

static GBL_RESULT NaryObject_verifyChildren_(GblContext* pCtx, const NaryObject_* pNode, ...) {
    va_list varArgs;
    va_start(varArgs, pNode);
    GBL_CTX_BEGIN(pCtx);

    int curIndex = 0;
    int curId = 0;
    GblNaryTreeNode* pChild = pNode->node.pChildFirst;
    GblNaryTreeNode* pPrev = NULL;

    while((curId = va_arg(varArgs, int)) != -1) {
        GBL_CTX_VERIFY_EXPRESSION(pChild);

        GBL_TEST_COMPARE(NARY_OBJECT_OF_(pChild)->id, curId);
        GBL_TEST_COMPARE(NARY_OBJECT_OF_(GblNaryTree_childAt(&pNode->node, curIndex))->id, curId);
        GBL_TEST_COMPARE(GblNaryTree_childIndex(&pNode->node, pChild), curIndex);
        GBL_TEST_VERIFY(GblNaryTree_isChild(&pNode->node, pChild));
        GBL_TEST_VERIFY(GblNaryTree_isParent(pChild, &pNode->node));
        GBL_TEST_VERIFY(GblNaryTree_isAncestor(pChild, &pNode->node));
        GBL_TEST_VERIFY(GblNaryTree_isDescendent(&pNode->node, pChild));
        GBL_TEST_VERIFY(GblNaryTree_isInternal(&pNode->node));
        GBL_TEST_VERIFY(GblNaryTree_isConnected(pChild));

        if(pPrev) {
            GBL_TEST_COMPARE(GblNaryTree_childBefore(&pNode->node, pChild), pPrev);
            GBL_TEST_VERIFY(GblNaryTree_isSibling(pChild, pPrev));
        }

        pPrev = pChild;
        pChild = pChild->pSiblingNext;
        ++curIndex;
    }

    GBL_TEST_COMPARE(GblNaryTree_childCount(&pNode->node), curIndex);
    GBL_TEST_COMPARE(GblNaryTree_childLast(&pNode->node), pPrev);

    GBL_CTX_END_BLOCK();
    va_end(varArgs);
    return GBL_CTX_RESULT();
}

static GBL_RESULT GblNaryTreeTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblNaryTreeTestSuite_* pSelf_ = GBL_NARY_TREE_TEST_SUITE_(pSelf);
    memset(pSelf_, 0, sizeof(GblNaryTreeTestSuite_));

    for(size_t  i = 0; i < NARY_OBJECT_COUNT_; ++i) {
        NARY_OBJECT_(pSelf_, i)->id = i;
    }

    GBL_CTX_END();
}

static GBL_RESULT GblNaryTreeTestSuite_final_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    //GblNaryTreeTestSuite_* pSelf_ = GBL_NARY_TREE_TEST_SUITE_(pSelf);
    GBL_CTX_END();
}

static GBL_RESULT GblNaryTreeTestSuite_empty_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblNaryTreeTestSuite_* pSelf_ = GBL_NARY_TREE_TEST_SUITE_(pSelf);

    GBL_TEST_VERIFY(!GblNaryTree_isConnected(NARY_OBJECT_NODE_(pSelf_, 0)));
    GBL_TEST_VERIFY(GblNaryTree_isRoot(NARY_OBJECT_NODE_(pSelf_, 0)));
    GBL_TEST_VERIFY(GblNaryTree_isLeaf(NARY_OBJECT_NODE_(pSelf_, 0)));
    GBL_TEST_COMPARE(GblNaryTree_depth(NARY_OBJECT_NODE_(pSelf_, 0)), 0);

    GBL_TEST_COMPARE(GblNaryTree_flags(NARY_OBJECT_NODE_(pSelf_, 0)),
                     GBL_NARY_TREE_NODE_FLAG_ROOT | GBL_NARY_TREE_NODE_FLAG_LEAF);

    GBL_CTX_VERIFY_CALL(NaryObject_verifyChildren_(pCtx,
                                                   NARY_OBJECT_(pSelf_, 0),
                                                   -1));
    GBL_CTX_END();
}

static GBL_RESULT GblNaryTreeTestSuite_addChildFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblNaryTreeTestSuite_* pSelf_ = GBL_NARY_TREE_TEST_SUITE_(pSelf);

    GblNaryTree_addChildFront(NARY_OBJECT_NODE_(pSelf_, 0), NARY_OBJECT_NODE_(pSelf, 1));

    GBL_TEST_VERIFY(GblNaryTree_isConnected(NARY_OBJECT_NODE_(pSelf_, 0)));
    GBL_TEST_VERIFY(GblNaryTree_isRoot(NARY_OBJECT_NODE_(pSelf_, 0)));
    GBL_TEST_VERIFY(!GblNaryTree_isLeaf(NARY_OBJECT_NODE_(pSelf_, 0)));

    GblNaryTree_addChildFront(NARY_OBJECT_NODE_(pSelf_, 0), NARY_OBJECT_NODE_(pSelf, 2));

    GBL_CTX_VERIFY_CALL(NaryObject_verifyChildren_(pCtx, NARY_OBJECT_(pSelf_, 0), 2, 1, -1));

    GBL_CTX_END();
}

static GBL_RESULT GblNaryTreeTestSuite_addChildBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblNaryTreeTestSuite_* pSelf_ = GBL_NARY_TREE_TEST_SUITE_(pSelf);

    GblNaryTree_addChildBack(NARY_OBJECT_NODE_(pSelf_, 0), NARY_OBJECT_NODE_(pSelf, 3));

    GBL_CTX_VERIFY_CALL(NaryObject_verifyChildren_(pCtx, NARY_OBJECT_(pSelf_, 0), 2, 1, 3, -1));

    GBL_CTX_END();
}

static GBL_RESULT GblNaryTreeTestSuite_addChildBefore_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblNaryTreeTestSuite_* pSelf_ = GBL_NARY_TREE_TEST_SUITE_(pSelf);

    GblNaryTree_addChildBefore(NARY_OBJECT_NODE_(pSelf_, 0), NARY_OBJECT_NODE_(pSelf, 2), NARY_OBJECT_NODE_(pSelf, 4));
    GblNaryTree_addChildBefore(NARY_OBJECT_NODE_(pSelf_, 0), NARY_OBJECT_NODE_(pSelf, 3), NARY_OBJECT_NODE_(pSelf, 5));

    GBL_CTX_VERIFY_CALL(NaryObject_verifyChildren_(pCtx, NARY_OBJECT_(pSelf_, 0), 4, 2, 1, 5, 3, -1));

    GBL_CTX_END();
}

static GBL_RESULT GblNaryTreeTestSuite_addChildAfter_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblNaryTreeTestSuite_* pSelf_ = GBL_NARY_TREE_TEST_SUITE_(pSelf);

    GblNaryTree_addChildAfter(NARY_OBJECT_NODE_(pSelf_, 0), NARY_OBJECT_NODE_(pSelf, 4), NARY_OBJECT_NODE_(pSelf, 6));
    GblNaryTree_addChildAfter(NARY_OBJECT_NODE_(pSelf_, 0), NARY_OBJECT_NODE_(pSelf, 3), NARY_OBJECT_NODE_(pSelf, 7));

    GBL_CTX_VERIFY_CALL(NaryObject_verifyChildren_(pCtx, NARY_OBJECT_(pSelf_, 0), 4, 6, 2, 1, 5, 3, 7, -1));

    GBL_CTX_END();
}

static GBL_RESULT GblNaryTreeTestSuite_addChildTo_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblNaryTreeTestSuite_* pSelf_ = GBL_NARY_TREE_TEST_SUITE_(pSelf);

    GblNaryTree_addChildTo(NARY_OBJECT_NODE_(pSelf_, 0), 0, NARY_OBJECT_NODE_(pSelf, 8));
    GblNaryTree_addChildTo(NARY_OBJECT_NODE_(pSelf_, 0), 1, NARY_OBJECT_NODE_(pSelf, 9));
    GblNaryTree_addChildTo(NARY_OBJECT_NODE_(pSelf_, 0), 9, NARY_OBJECT_NODE_(pSelf, 10));

    GBL_CTX_VERIFY_CALL(NaryObject_verifyChildren_(pCtx, NARY_OBJECT_(pSelf_, 0), 8, 9, 4, 6, 2, 1, 5, 3, 7, 10, -1));

    GBL_CTX_END();
}

static GBL_RESULT GblNaryTreeTestSuite_moveChildFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblNaryTreeTestSuite_* pSelf_ = GBL_NARY_TREE_TEST_SUITE_(pSelf);

    GblNaryTree_moveChildFront(NARY_OBJECT_NODE_(pSelf_, 0), NARY_OBJECT_NODE_(pSelf, 8));
    GblNaryTree_moveChildFront(NARY_OBJECT_NODE_(pSelf_, 0), NARY_OBJECT_NODE_(pSelf, 2));
    GblNaryTree_moveChildFront(NARY_OBJECT_NODE_(pSelf_, 0), NARY_OBJECT_NODE_(pSelf, 10));

    GBL_CTX_VERIFY_CALL(NaryObject_verifyChildren_(pCtx, NARY_OBJECT_(pSelf_, 0), 10, 2, 8, 9, 4, 6, 1, 5, 3, 7, -1));

    GBL_CTX_END();
}

static GBL_RESULT GblNaryTreeTestSuite_moveChildBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblNaryTreeTestSuite_* pSelf_ = GBL_NARY_TREE_TEST_SUITE_(pSelf);

    GblNaryTree_moveChildBack(NARY_OBJECT_NODE_(pSelf_, 0), NARY_OBJECT_NODE_(pSelf, 10));
    GblNaryTree_moveChildBack(NARY_OBJECT_NODE_(pSelf_, 0), NARY_OBJECT_NODE_(pSelf, 8));
    GblNaryTree_moveChildBack(NARY_OBJECT_NODE_(pSelf_, 0), NARY_OBJECT_NODE_(pSelf, 8));

    GBL_CTX_VERIFY_CALL(NaryObject_verifyChildren_(pCtx, NARY_OBJECT_(pSelf_, 0), 2, 9, 4, 6, 1, 5, 3, 7, 10, 8, -1));

    GBL_CTX_END();
}

static GBL_RESULT GblNaryTreeTestSuite_moveChildTo_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblNaryTreeTestSuite_* pSelf_ = GBL_NARY_TREE_TEST_SUITE_(pSelf);

    GblNaryTree_moveChildTo(NARY_OBJECT_NODE_(pSelf_, 0), 0, NARY_OBJECT_NODE_(pSelf, 2));
    GblNaryTree_moveChildTo(NARY_OBJECT_NODE_(pSelf_, 0), 3, NARY_OBJECT_NODE_(pSelf, 8));
    GblNaryTree_moveChildTo(NARY_OBJECT_NODE_(pSelf_, 0), 10, NARY_OBJECT_NODE_(pSelf, 3));

    GBL_CTX_VERIFY_CALL(NaryObject_verifyChildren_(pCtx, NARY_OBJECT_(pSelf_, 0), 2, 9, 4, 8, 6, 1, 5, 7, 10, 3, -1));

    GBL_CTX_END();
}

static GBL_RESULT GblNaryTreeTestSuite_removeChild_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblNaryTreeTestSuite_* pSelf_ = GBL_NARY_TREE_TEST_SUITE_(pSelf);

    GblNaryTree_removeChild(NARY_OBJECT_NODE_(pSelf_, 0), NARY_OBJECT_NODE_(pSelf, 2));
    GblNaryTree_removeChild(NARY_OBJECT_NODE_(pSelf_, 0), NARY_OBJECT_NODE_(pSelf, 8));
    GblNaryTree_removeChild(NARY_OBJECT_NODE_(pSelf_, 0), NARY_OBJECT_NODE_(pSelf, 3));

    GBL_CTX_VERIFY_CALL(NaryObject_verifyChildren_(pCtx, NARY_OBJECT_(pSelf_, 0), 9, 4, 6, 1, 5, 7, 10, -1));

    GBL_CTX_END();
}

static GBL_RESULT GblNaryTreeTestSuite_removeChildFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblNaryTreeTestSuite_* pSelf_ = GBL_NARY_TREE_TEST_SUITE_(pSelf);

    GblNaryTree_removeChildFront(NARY_OBJECT_NODE_(pSelf_, 0));

    GBL_CTX_VERIFY_CALL(NaryObject_verifyChildren_(pCtx, NARY_OBJECT_(pSelf_, 0), 4, 6, 1, 5, 7, 10, -1));

    GblNaryTree_addChildFront(NARY_OBJECT_NODE_(pSelf_, 2), NARY_OBJECT_NODE_(pSelf_, 3));
    GblNaryTree_removeChildFront(NARY_OBJECT_NODE_(pSelf, 2));
    GBL_CTX_VERIFY_CALL(NaryObject_verifyChildren_(pCtx, NARY_OBJECT_(pSelf_, 2), -1));

    GBL_CTX_END();
}

static GBL_RESULT GblNaryTreeTestSuite_removeChildBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblNaryTreeTestSuite_* pSelf_ = GBL_NARY_TREE_TEST_SUITE_(pSelf);

    GblNaryTree_removeChildBack(NARY_OBJECT_NODE_(pSelf_, 0));

    GBL_CTX_VERIFY_CALL(NaryObject_verifyChildren_(pCtx, NARY_OBJECT_(pSelf_, 0), 4, 6, 1, 5, 7, -1));

    GblNaryTree_addChildFront(NARY_OBJECT_NODE_(pSelf_, 2), NARY_OBJECT_NODE_(pSelf_, 3));
    GblNaryTree_removeChildBack(NARY_OBJECT_NODE_(pSelf, 2));
    GBL_CTX_VERIFY_CALL(NaryObject_verifyChildren_(pCtx, NARY_OBJECT_(pSelf_, 2), -1));

    GBL_CTX_END();
}

static GBL_RESULT GblNaryTreeTestSuite_removeChildAt_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblNaryTreeTestSuite_* pSelf_ = GBL_NARY_TREE_TEST_SUITE_(pSelf);

    GblNaryTree_removeChildAt(NARY_OBJECT_NODE_(pSelf_, 0), 0);
    GblNaryTree_removeChildAt(NARY_OBJECT_NODE_(pSelf_, 0), 1);
    GblNaryTree_removeChildAt(NARY_OBJECT_NODE_(pSelf_, 0), 2);

    GBL_CTX_VERIFY_CALL(NaryObject_verifyChildren_(pCtx, NARY_OBJECT_(pSelf_, 0), 6, 5, -1));

    GBL_CTX_END();
}

static GBL_RESULT GblNaryTreeTestSuite_replaceChild_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblNaryTreeTestSuite_* pSelf_ = GBL_NARY_TREE_TEST_SUITE_(pSelf);

    GblNaryTree_replaceChild(NARY_OBJECT_NODE_(pSelf_, 0), NARY_OBJECT_NODE_(pSelf_, 6), NARY_OBJECT_NODE_(pSelf_, 1));
    GblNaryTree_replaceChild(NARY_OBJECT_NODE_(pSelf_, 0), NARY_OBJECT_NODE_(pSelf_, 5), NARY_OBJECT_NODE_(pSelf_, 2));

    GBL_CTX_VERIFY_CALL(NaryObject_verifyChildren_(pCtx, NARY_OBJECT_(pSelf_, 0), 1, 2, -1));

    GBL_CTX_END();
}

static GBL_RESULT GblNaryTreeTestSuite_replaceChildAt_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblNaryTreeTestSuite_* pSelf_ = GBL_NARY_TREE_TEST_SUITE_(pSelf);

    GblNaryTree_replaceChildAt(NARY_OBJECT_NODE_(pSelf_, 0), 0, NARY_OBJECT_NODE_(pSelf_, 3));
    GblNaryTree_replaceChildAt(NARY_OBJECT_NODE_(pSelf_, 0), 1, NARY_OBJECT_NODE_(pSelf_, 4));

    GBL_CTX_VERIFY_CALL(NaryObject_verifyChildren_(pCtx, NARY_OBJECT_(pSelf_, 0), 3, 4, -1));

    GBL_CTX_END();
}

static GBL_RESULT GblNaryTreeTestSuite_swapChildren_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblNaryTreeTestSuite_* pSelf_ = GBL_NARY_TREE_TEST_SUITE_(pSelf);

    GblNaryTree_swapChildren(NARY_OBJECT_NODE_(pSelf_, 0), NARY_OBJECT_NODE_(pSelf_, 4), NARY_OBJECT_NODE_(pSelf_, 3));
    GBL_CTX_VERIFY_CALL(NaryObject_verifyChildren_(pCtx, NARY_OBJECT_(pSelf_, 0), 4, 3, -1));

    GblNaryTree_swapChildren(NARY_OBJECT_NODE_(pSelf_, 0), NARY_OBJECT_NODE_(pSelf_, 4), NARY_OBJECT_NODE_(pSelf_, 3));
    GBL_CTX_VERIFY_CALL(NaryObject_verifyChildren_(pCtx, NARY_OBJECT_(pSelf_, 0), 3, 4, -1));

    GblNaryTree_addChildFront(NARY_OBJECT_NODE_(pSelf_, 0), NARY_OBJECT_NODE_(pSelf_, 2));
    GblNaryTree_swapChildren(NARY_OBJECT_NODE_(pSelf_, 0), NARY_OBJECT_NODE_(pSelf_, 3), NARY_OBJECT_NODE_(pSelf_, 4));
    GBL_CTX_VERIFY_CALL(NaryObject_verifyChildren_(pCtx, NARY_OBJECT_(pSelf_, 0), 2, 4, 3, -1));

    GBL_CTX_END();
}

static GBL_RESULT GblNaryTreeTestSuite_swapChildrenAt_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblNaryTreeTestSuite_* pSelf_ = GBL_NARY_TREE_TEST_SUITE_(pSelf);

    GblNaryTree_swapChildrenAt(NARY_OBJECT_NODE_(pSelf_, 0), 0, 1);
    GBL_CTX_VERIFY_CALL(NaryObject_verifyChildren_(pCtx, NARY_OBJECT_(pSelf_, 0), 4, 2, 3, -1));

    GblNaryTree_swapChildrenAt(NARY_OBJECT_NODE_(pSelf_, 0), 0, 2);
    GBL_CTX_VERIFY_CALL(NaryObject_verifyChildren_(pCtx, NARY_OBJECT_(pSelf_, 0), 3, 2, 4, -1));

    GblNaryTree_swapChildrenAt(NARY_OBJECT_NODE_(pSelf_, 0), 1, 2);
    GBL_CTX_VERIFY_CALL(NaryObject_verifyChildren_(pCtx, NARY_OBJECT_(pSelf_, 0), 3, 4, 2, -1));

    GBL_CTX_END();
}

static GBL_RESULT GblNaryTreeTestSuite_reverseChildren_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblNaryTreeTestSuite_* pSelf_ = GBL_NARY_TREE_TEST_SUITE_(pSelf);

    GblNaryTree_reverseChildren(NARY_OBJECT_NODE_(pSelf_, 0));
    GBL_CTX_VERIFY_CALL(NaryObject_verifyChildren_(pCtx, NARY_OBJECT_(pSelf_, 0), 2, 4, 3, -1));

    GBL_CTX_END();
}

static GBL_RESULT GblNaryTreeTestSuite_root_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblNaryTreeTestSuite_* pSelf_ = GBL_NARY_TREE_TEST_SUITE_(pSelf);

    GblNaryTree_addChildFront(NARY_OBJECT_NODE_(pSelf_, 9), NARY_OBJECT_NODE_(pSelf_, 0));
    GblNaryTree_addChildFront(NARY_OBJECT_NODE_(pSelf_, 10), NARY_OBJECT_NODE_(pSelf_, 9));

    GBL_TEST_COMPARE(GblNaryTree_depth(NARY_OBJECT_NODE_(pSelf_, 0)), 2);

    GBL_TEST_COMPARE(GblNaryTree_root(NARY_OBJECT_NODE_(pSelf_, 0)), NARY_OBJECT_NODE_(pSelf_, 10));
    GBL_TEST_COMPARE(GblNaryTree_root(NARY_OBJECT_NODE_(pSelf_, 9)), NARY_OBJECT_NODE_(pSelf_, 10));

    GBL_CTX_END();
}

static GBL_RESULT GblNaryTreeTestSuite_base_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblNaryTreeTestSuite_* pSelf_ = GBL_NARY_TREE_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE(GblNaryTree_base(NARY_OBJECT_NODE_(pSelf_, 0), 0), NARY_OBJECT_NODE_(pSelf_, 10));
    GBL_TEST_COMPARE(GblNaryTree_base(NARY_OBJECT_NODE_(pSelf_, 0), 1), NARY_OBJECT_NODE_(pSelf_, 9));
    GBL_TEST_COMPARE(GblNaryTree_base(NARY_OBJECT_NODE_(pSelf_, 0), 2), NARY_OBJECT_NODE_(pSelf_, 0));

    GBL_TEST_COMPARE(GblNaryTree_base(NARY_OBJECT_NODE_(pSelf_, 9), 0), NARY_OBJECT_NODE_(pSelf_, 10));
    GBL_TEST_COMPARE(GblNaryTree_base(NARY_OBJECT_NODE_(pSelf_, 9), 1), NARY_OBJECT_NODE_(pSelf_, 9));

    GBL_TEST_COMPARE(GblNaryTree_base(NARY_OBJECT_NODE_(pSelf_, 10), 0), NARY_OBJECT_NODE_(pSelf_, 10));

    GBL_CTX_END();
}

static GBL_RESULT GblNaryTreeTestSuite_ancestor_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblNaryTreeTestSuite_* pSelf_ = GBL_NARY_TREE_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE(GblNaryTree_ancestor(NARY_OBJECT_NODE_(pSelf_, 0), 0), NARY_OBJECT_NODE_(pSelf_, 9));
    GBL_TEST_COMPARE(GblNaryTree_ancestor(NARY_OBJECT_NODE_(pSelf_, 0), 1), NARY_OBJECT_NODE_(pSelf_, 10));

    GBL_TEST_COMPARE(GblNaryTree_ancestor(NARY_OBJECT_NODE_(pSelf_, 9), 0), NARY_OBJECT_NODE_(pSelf_, 10));

    GBL_CTX_END();
}

static GBL_RESULT GblNaryTreeTestSuite_ancestorHeight_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblNaryTreeTestSuite_* pSelf_ = GBL_NARY_TREE_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE(GblNaryTree_ancestorHeight(NARY_OBJECT_NODE_(pSelf_, 0), NARY_OBJECT_NODE_(pSelf_, 9)), 0);
    GBL_TEST_COMPARE(GblNaryTree_ancestorHeight(NARY_OBJECT_NODE_(pSelf_, 0), NARY_OBJECT_NODE_(pSelf_, 10)), 1);

    GBL_TEST_COMPARE(GblNaryTree_ancestorHeight(NARY_OBJECT_NODE_(pSelf_, 9), NARY_OBJECT_NODE_(pSelf_, 10)), 0);

    GBL_CTX_END();
}

static GBL_RESULT GblNaryTreeTestSuite_siblingCount_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblNaryTreeTestSuite_* pSelf_ = GBL_NARY_TREE_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE(GblNaryTree_siblingCount(NARY_OBJECT_NODE_(pSelf_, 10)), 0);
    GBL_TEST_COMPARE(GblNaryTree_siblingCount(NARY_OBJECT_NODE_(pSelf_, 0)), 0);
    GBL_TEST_COMPARE(GblNaryTree_siblingCount(NARY_OBJECT_NODE_(pSelf_, 2)), 2);
    GBL_TEST_COMPARE(GblNaryTree_siblingCount(NARY_OBJECT_NODE_(pSelf_, 4)), 2);

    GBL_CTX_END();
}

static GBL_RESULT GblNaryTreeTestSuite_siblingLast_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblNaryTreeTestSuite_* pSelf_ = GBL_NARY_TREE_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE(GblNaryTree_siblingLast(NARY_OBJECT_NODE_(pSelf_, 10)), NULL);
    GBL_TEST_COMPARE(GblNaryTree_siblingLast(NARY_OBJECT_NODE_(pSelf_, 2)), NARY_OBJECT_NODE_(pSelf_, 3));
    GBL_TEST_COMPARE(GblNaryTree_siblingLast(NARY_OBJECT_NODE_(pSelf_, 4)), NARY_OBJECT_NODE_(pSelf_, 3));
    GBL_TEST_COMPARE(GblNaryTree_siblingLast(NARY_OBJECT_NODE_(pSelf_, 3)), NARY_OBJECT_NODE_(pSelf_, 4));

    GBL_CTX_END();
}

static GBL_RESULT GblNaryTreeTestSuite_siblingBefore_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblNaryTreeTestSuite_* pSelf_ = GBL_NARY_TREE_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE(GblNaryTree_siblingBefore(NARY_OBJECT_NODE_(pSelf_, 10)), NULL);
    GBL_TEST_COMPARE(GblNaryTree_siblingBefore(NARY_OBJECT_NODE_(pSelf_, 2)), NULL);
    GBL_TEST_COMPARE(GblNaryTree_siblingBefore(NARY_OBJECT_NODE_(pSelf_, 4)), NARY_OBJECT_NODE_(pSelf_, 2));
    GBL_TEST_COMPARE(GblNaryTree_siblingBefore(NARY_OBJECT_NODE_(pSelf_, 3)), NARY_OBJECT_NODE_(pSelf_, 4));

    GBL_CTX_END();
}

static GBL_RESULT GblNaryTreeTestSuite_siblingFirst_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblNaryTreeTestSuite_* pSelf_ = GBL_NARY_TREE_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE(GblNaryTree_siblingFirst(NARY_OBJECT_NODE_(pSelf_, 10)), NULL);
    GBL_TEST_COMPARE(GblNaryTree_siblingFirst(NARY_OBJECT_NODE_(pSelf_, 2)), NARY_OBJECT_NODE_(pSelf_, 4));
    GBL_TEST_COMPARE(GblNaryTree_siblingFirst(NARY_OBJECT_NODE_(pSelf_, 4)), NARY_OBJECT_NODE_(pSelf_, 2));
    GBL_TEST_COMPARE(GblNaryTree_siblingFirst(NARY_OBJECT_NODE_(pSelf_, 3)), NARY_OBJECT_NODE_(pSelf_, 2));

    GBL_CTX_END();
}

static GBL_RESULT GblNaryTreeTestSuite_siblingAt_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblNaryTreeTestSuite_* pSelf_ = GBL_NARY_TREE_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE(GblNaryTree_siblingAt(NARY_OBJECT_NODE_(pSelf_, 2), 0), NARY_OBJECT_NODE_(pSelf_, 4));
    GBL_TEST_COMPARE(GblNaryTree_siblingAt(NARY_OBJECT_NODE_(pSelf_, 2), 1), NARY_OBJECT_NODE_(pSelf_, 3));

    GBL_TEST_COMPARE(GblNaryTree_siblingAt(NARY_OBJECT_NODE_(pSelf_, 4), 0), NARY_OBJECT_NODE_(pSelf_, 2));
    GBL_TEST_COMPARE(GblNaryTree_siblingAt(NARY_OBJECT_NODE_(pSelf_, 4), 1), NARY_OBJECT_NODE_(pSelf_, 3));

    GBL_TEST_COMPARE(GblNaryTree_siblingAt(NARY_OBJECT_NODE_(pSelf_, 3), 0), NARY_OBJECT_NODE_(pSelf_, 2));
    GBL_TEST_COMPARE(GblNaryTree_siblingAt(NARY_OBJECT_NODE_(pSelf_, 3), 1), NARY_OBJECT_NODE_(pSelf_, 4));

    GBL_CTX_END();
}

static GBL_RESULT GblNaryTreeTestSuite_siblingIndex_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblNaryTreeTestSuite_* pSelf_ = GBL_NARY_TREE_TEST_SUITE_(pSelf);

    GBL_TEST_COMPARE(GblNaryTree_siblingIndex(NARY_OBJECT_NODE_(pSelf_, 2), NARY_OBJECT_NODE_(pSelf_, 4)), 0);
    GBL_TEST_COMPARE(GblNaryTree_siblingIndex(NARY_OBJECT_NODE_(pSelf_, 2), NARY_OBJECT_NODE_(pSelf_, 3)), 1);

    GBL_TEST_COMPARE(GblNaryTree_siblingIndex(NARY_OBJECT_NODE_(pSelf_, 4), NARY_OBJECT_NODE_(pSelf_, 2)), 0);
    GBL_TEST_COMPARE(GblNaryTree_siblingIndex(NARY_OBJECT_NODE_(pSelf_, 4), NARY_OBJECT_NODE_(pSelf_, 3)), 1);

    GBL_TEST_COMPARE(GblNaryTree_siblingIndex(NARY_OBJECT_NODE_(pSelf_, 3), NARY_OBJECT_NODE_(pSelf_, 2)), 0);
    GBL_TEST_COMPARE(GblNaryTree_siblingIndex(NARY_OBJECT_NODE_(pSelf_, 3), NARY_OBJECT_NODE_(pSelf_, 4)), 1);

    GBL_CTX_END();
}

static GBL_RESULT GblNaryTreeTestSuite_disconnect_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    GblNaryTreeTestSuite_* pSelf_ = GBL_NARY_TREE_TEST_SUITE_(pSelf);

    GblNaryTree_disconnect(NARY_OBJECT_NODE_(pSelf, 0));

    GBL_TEST_VERIFY(!GblNaryTree_isConnected(NARY_OBJECT_NODE_(pSelf_, 0)));
    GBL_TEST_VERIFY(!GblNaryTree_isChild(NARY_OBJECT_NODE_(pSelf_, 0), NARY_OBJECT_NODE_(pSelf_, 4)));
    GBL_TEST_VERIFY(!GblNaryTree_isSibling(NARY_OBJECT_NODE_(pSelf_, 3), NARY_OBJECT_NODE_(pSelf_, 2)));
    GBL_TEST_VERIFY(!GblNaryTree_isAncestor(NARY_OBJECT_NODE_(pSelf, 0), NARY_OBJECT_NODE_(pSelf, 9)));
    GBL_TEST_VERIFY(!GblNaryTree_isAncestor(NARY_OBJECT_NODE_(pSelf, 0), NARY_OBJECT_NODE_(pSelf, 10)));

    GBL_CTX_END();
}


typedef struct TraversalClosure_ {
    int count;
    int index[NARY_OBJECT_COUNT_+1];
    int countReturnTrue;
} TraversalClosure_;

GblBool traversalIt_(const GblNaryTreeNode* pNode, void* pUd) {
    TraversalClosure_* pClosure = pUd;
    pClosure->index[pClosure->count++] = NARY_OBJECT_OF_(pNode)->id;
    if(pClosure->count == pClosure->countReturnTrue) return GBL_TRUE;
    else return GBL_FALSE;
}

static GblBool GblNaryTreeTestSuite_traverse_(GblTestSuite* pSelf, uintptr_t mask, TraversalClosure_* pClosure) {
    GblNaryTreeTestSuite_* pSelf_ = GBL_NARY_TREE_TEST_SUITE_(pSelf);

    for(size_t  i = 0; i < NARY_OBJECT_COUNT_; ++i) {
        memset(NARY_OBJECT_NODE_(pSelf_, i), 0, sizeof(NaryObject_));
        NARY_OBJECT_(pSelf_, i)->id = i;
    }

    GblNaryTree_addChildBack(NARY_OBJECT_NODE_(pSelf_, 0), NARY_OBJECT_NODE_(pSelf_, 1));
    GblNaryTree_addChildBack(NARY_OBJECT_NODE_(pSelf_, 0), NARY_OBJECT_NODE_(pSelf_, 2));
    GblNaryTree_addChildBack(NARY_OBJECT_NODE_(pSelf_, 0), NARY_OBJECT_NODE_(pSelf_, 3));

    GblNaryTree_addChildBack(NARY_OBJECT_NODE_(pSelf_, 1), NARY_OBJECT_NODE_(pSelf_, 4));
    GblNaryTree_addChildBack(NARY_OBJECT_NODE_(pSelf_, 1), NARY_OBJECT_NODE_(pSelf_, 5));

    GblNaryTree_addChildBack(NARY_OBJECT_NODE_(pSelf_, 3), NARY_OBJECT_NODE_(pSelf_, 6));

    GblNaryTree_addChildBack(NARY_OBJECT_NODE_(pSelf_, 6), NARY_OBJECT_NODE_(pSelf_, 7));
    GblNaryTree_addChildBack(NARY_OBJECT_NODE_(pSelf_, 6), NARY_OBJECT_NODE_(pSelf_, 8));
    GblNaryTree_addChildBack(NARY_OBJECT_NODE_(pSelf_, 6), NARY_OBJECT_NODE_(pSelf_, 9));
    GblNaryTree_addChildBack(NARY_OBJECT_NODE_(pSelf_, 6), NARY_OBJECT_NODE_(pSelf_, 10));

    pClosure->countReturnTrue = -1;

    for(size_t  i = 0; i < NARY_OBJECT_COUNT_; ++i)
        pClosure->index[i] = -1;

    return GblNaryTree_traverse(NARY_OBJECT_NODE_(pSelf, 0),
                                mask,
                                traversalIt_,
                                pClosure);
}


static GBL_RESULT GblNaryTreeTestSuite_traversePreOrder_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);

    TraversalClosure_ closure = { 0 };

    GBL_TEST_VERIFY(!GblNaryTreeTestSuite_traverse_(pSelf,
                                                    GBL_NARY_TREE_TRAVERSAL_MASK(GBL_NARY_TREE_TRAVERSAL_ORDER_PRE,
                                                                                 GBL_NARY_TREE_NODE_FLAGS_ALL),
                                                    &closure));

    GBL_TEST_COMPARE(closure.count, NARY_OBJECT_COUNT_);
    GBL_TEST_COMPARE(closure.index[0], 0);
    GBL_TEST_COMPARE(closure.index[1], 1);
    GBL_TEST_COMPARE(closure.index[2], 4);
    GBL_TEST_COMPARE(closure.index[3], 5);
    GBL_TEST_COMPARE(closure.index[4], 2);
    GBL_TEST_COMPARE(closure.index[5], 3);
    GBL_TEST_COMPARE(closure.index[6], 6);
    GBL_TEST_COMPARE(closure.index[7], 7);
    GBL_TEST_COMPARE(closure.index[8], 8);
    GBL_TEST_COMPARE(closure.index[9], 9);
    GBL_TEST_COMPARE(closure.index[10], 10);
    GBL_CTX_END();
}

static GBL_RESULT GblNaryTreeTestSuite_traversePostOrder_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    TraversalClosure_ closure = {0};

    GBL_TEST_VERIFY(!GblNaryTreeTestSuite_traverse_(pSelf,
                                                    GBL_NARY_TREE_TRAVERSAL_MASK(GBL_NARY_TREE_TRAVERSAL_ORDER_POST,
                                                                                 GBL_NARY_TREE_NODE_FLAGS_ALL),
                                                    &closure));

    GBL_TEST_COMPARE(closure.count, NARY_OBJECT_COUNT_);
    GBL_TEST_COMPARE(closure.index[0], 4);
    GBL_TEST_COMPARE(closure.index[1], 5);
    GBL_TEST_COMPARE(closure.index[2], 1);
    GBL_TEST_COMPARE(closure.index[3], 2);
    GBL_TEST_COMPARE(closure.index[4], 7);
    GBL_TEST_COMPARE(closure.index[5], 8);
    GBL_TEST_COMPARE(closure.index[6], 9);
    GBL_TEST_COMPARE(closure.index[7], 10);
    GBL_TEST_COMPARE(closure.index[8], 6);
    GBL_TEST_COMPARE(closure.index[9], 3);
    GBL_TEST_COMPARE(closure.index[10], 0);
    GBL_CTX_END();
}

static GBL_RESULT GblNaryTreeTestSuite_traverseInOrder_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    TraversalClosure_ closure = { 0 };

    GBL_TEST_VERIFY(!GblNaryTreeTestSuite_traverse_(pSelf,
                                                    GBL_NARY_TREE_TRAVERSAL_MASK(GBL_NARY_TREE_TRAVERSAL_ORDER_IN,
                                                                                 GBL_NARY_TREE_NODE_FLAGS_ALL),
                                                    &closure));

    GBL_TEST_COMPARE(closure.count, NARY_OBJECT_COUNT_);
    GBL_TEST_COMPARE(closure.index[0], 4);
    GBL_TEST_COMPARE(closure.index[1], 1);
    GBL_TEST_COMPARE(closure.index[2], 5);
    GBL_TEST_COMPARE(closure.index[3], 2);
    GBL_TEST_COMPARE(closure.index[4], 0);
    GBL_TEST_COMPARE(closure.index[5], 3);
    GBL_TEST_COMPARE(closure.index[6], 7);
    GBL_TEST_COMPARE(closure.index[7], 8);
    GBL_TEST_COMPARE(closure.index[8], 9);
    GBL_TEST_COMPARE(closure.index[9], 6);
    GBL_TEST_COMPARE(closure.index[10], 10);
    GBL_CTX_END();
}


GBL_EXPORT GblType GblNaryTreeTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "empty",            GblNaryTreeTestSuite_empty_            },
        { "addChildFront",    GblNaryTreeTestSuite_addChildFront_    },
        { "addChildBack",     GblNaryTreeTestSuite_addChildBack_     },
        { "addChildBefore",   GblNaryTreeTestSuite_addChildBefore_   },
        { "addChildAfter",    GblNaryTreeTestSuite_addChildAfter_    },
        { "addChildTo",       GblNaryTreeTestSuite_addChildTo_       },
        { "moveChildFront",   GblNaryTreeTestSuite_moveChildFront_   },
        { "moveChildBack",    GblNaryTreeTestSuite_moveChildBack_    },
        { "moveChildTo",      GblNaryTreeTestSuite_moveChildTo_      },
        { "removeChild",      GblNaryTreeTestSuite_removeChild_      },
        { "removeChildFront", GblNaryTreeTestSuite_removeChildFront_ },
        { "removeChildBack",  GblNaryTreeTestSuite_removeChildBack_  },
        { "removeChildAt",    GblNaryTreeTestSuite_removeChildAt_    },
        { "replaceChild",     GblNaryTreeTestSuite_replaceChild_     },
        { "replaceChildAt",   GblNaryTreeTestSuite_replaceChildAt_   },
        { "swapChildren",     GblNaryTreeTestSuite_swapChildren_     },
        { "swapChildrenAt",   GblNaryTreeTestSuite_swapChildrenAt_   },
        { "reverseChildren",  GblNaryTreeTestSuite_reverseChildren_  },
        { "root",             GblNaryTreeTestSuite_root_             },
        { "base",             GblNaryTreeTestSuite_base_             },
        { "ancestor",         GblNaryTreeTestSuite_ancestor_         },
        { "ancestorHeight",   GblNaryTreeTestSuite_ancestorHeight_   },
        { "siblingCount",     GblNaryTreeTestSuite_siblingCount_     },
        { "siblingLast",      GblNaryTreeTestSuite_siblingLast_      },
        { "siblingBefore",    GblNaryTreeTestSuite_siblingBefore_    },
        { "siblingFirst",     GblNaryTreeTestSuite_siblingFirst_     },
        { "siblingAt",        GblNaryTreeTestSuite_siblingAt_        },
        { "siblingIndex",     GblNaryTreeTestSuite_siblingIndex_     },
        { "disconnect",       GblNaryTreeTestSuite_disconnect_       },
        { "traversePreOrder", GblNaryTreeTestSuite_traversePreOrder_ },
        { "traversePostOrder",GblNaryTreeTestSuite_traversePostOrder_},
        { "traverseInOrder",  GblNaryTreeTestSuite_traverseInOrder_  },
        { NULL,               NULL                                   }
    };

    const static GblTestSuiteClassVTable vTable = {
        .pFnSuiteInit   = GblNaryTreeTestSuite_init_,
        .pFnSuiteFinal  = GblNaryTreeTestSuite_final_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("GblNaryTreeTestSuite"),
                                     &vTable,
                                     sizeof(GblNaryTreeTestSuite),
                                     sizeof(GblNaryTreeTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }
    return type;
}

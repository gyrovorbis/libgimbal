/*! \file
 *  \brief GblNaryTreeNode structure and related functions
 *  \todo Finish implementing commented out API operations
 *  \ingroup containers
 */

#ifndef GIMBAL_NARY_TREE_H
#define GIMBAL_NARY_TREE_H

#include "../core/gimbal_ctx.h"

#define GBL_NARY_TREE_NPOS                           GBL_NPOS
#define GBL_NARY_TREE_ENTRY(node, structure, field)  GBL_CONTAINER_OF(node, structure, field)
#define GBL_NARY_TREE_TRAVERSAL_MASK(order, flags)   ((order << 0x3) | (flags))

#define GBL_SELF_TYPE GblNaryTreeNode

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblNaryTreeNode);

typedef GblBool (*GblNaryTreeIterFn)(const GblNaryTreeNode* pNode, void* pUd);

typedef enum GBL_NARY_TREE_NODE_FLAGS {
    GBL_NARY_TREE_NODE_FLAG_ROOT     = 0x1,
    GBL_NARY_TREE_NODE_FLAG_INTERNAL = 0x2,
    GBL_NARY_TREE_NODE_FLAG_LEAF     = 0x4,
    GBL_NARY_TREE_NODE_FLAGS_ALL     = 0x7
} GBL_NARY_TREE_NODE_FLAGS;

typedef enum GBL_NARY_TREE_TRAVERSAL_ORDER {
    GBL_NARY_TREE_TRAVERSAL_ORDER_PRE,
    GBL_NARY_TREE_TRAVERSAL_ORDER_IN,
    GBL_NARY_TREE_TRAVERSAL_ORDER_POST,
    GBL_NARY_TREE_TRAVERSAL_ORDER_LEVEL
} GBL_NARY_TREE_TRAVERSAL_ORDER;

/*! \brief Represents a single intrusive node within an N-Ary tree structure
 *
 *  GblNaryTreeNode is a single node which is to be embedded within another
 *  structure, providing heirarchial relationships between its instances. As
 *  relationships are established between these instances, they become nodes
 *  comprising an aggregate tree structure.
 *
 *  A single node contains only a pointer to its parent node plus a linked list of
 *  its child nodes, resulting in a minimal memory footprint. The API attempts to
 *  present psuedo list-style semantics for accessing ancestors, bases, siblings,
 *  and children by index; however, these are linked structures and such operations
 *  are of O(N) time complexity, requiring traversals for random access.
 *
 *  \ingroup containers
 */
typedef struct GblNaryTreeNode {
    struct GblNaryTreeNode* pParent;        ///< Node's parent
    struct GblNaryTreeNode* pChildFirst;    ///< Node's first child (beginning of child linked list)
    struct GblNaryTreeNode* pSiblingNext;   ///< Node's next sibling (next entry of child linked list)
} GblNaryTreeNode;

//GBL_INLINE GblSize        GblNaryTree_size                (GBL_CSELF)                                                    GBL_NOEXCEPT;
//GBL_INLINE GblSize        GblNaryTree_width               (GBL_CSELF, GblSize depth);                                    GBL_NOEXCEPT;
//GBL_INLINE GblSize        GblNaryTree_height              (GBL_CSELF)                                                    GBL_NOEXCEPT;
GBL_INLINE GblSize          GblNaryTree_depth               (GBL_CSELF)                                                    GBL_NOEXCEPT;
//GBL_INLINE GblSize        GblNaryTree_breadth             (GBL_CSELF)                                                    GBL_NOEXCEPT;
//GBL_INLINE GblSize        GblNaryTree_degree              (GBL_CSELF)                                                    GBL_NOEXCEPT;
//GBL_INLINE GblSize        GblNaryTree_arity               (GBL_CSELF)                                                    GBL_NOEXCEPT;
GBL_INLINE GblFlags         GblNaryTree_flags               (GBL_CSELF)                                                    GBL_NOEXCEPT;

GBL_INLINE GblBool          GblNaryTree_isConnected         (GBL_CSELF)                                                    GBL_NOEXCEPT;
GBL_INLINE GblBool          GblNaryTree_isRoot              (GBL_CSELF)                                                    GBL_NOEXCEPT;
GBL_INLINE GblBool          GblNaryTree_isInternal          (GBL_CSELF)                                                    GBL_NOEXCEPT;
GBL_INLINE GblBool          GblNaryTree_isLeaf              (GBL_CSELF)                                                    GBL_NOEXCEPT;
//GBL_INLINE GblBool        GblNaryTree_isBalanced          (GBL_CSELF)                                                    GBL_NOEXCEPT;
GBL_INLINE GblBool          GblNaryTree_isParent            (GBL_CSELF, const GblNaryTreeNode* pOther)                     GBL_NOEXCEPT;
GBL_INLINE GblBool          GblNaryTree_isAncestor          (GBL_CSELF, const GblNaryTreeNode* pOther)                     GBL_NOEXCEPT;
GBL_INLINE GblBool          GblNaryTree_isSibling           (GBL_CSELF, const GblNaryTreeNode* pOther)                     GBL_NOEXCEPT;
GBL_INLINE GblBool          GblNaryTree_isChild             (GBL_CSELF, const GblNaryTreeNode* pOther)                     GBL_NOEXCEPT;
GBL_INLINE GblBool          GblNaryTree_isDescendent        (GBL_CSELF, const GblNaryTreeNode* pOther)                     GBL_NOEXCEPT;
//GBL_INLINE GblBool        GblNaryTree_isRelative          (GBL_CSELF, const GblNaryTreeNode* pOther)                     GBL_NOEXCEPT;

GBL_INLINE GblSize          GblNaryTree_childCount          (GBL_CSELF)                                                    GBL_NOEXCEPT;
GBL_INLINE GblNaryTreeNode* GblNaryTree_childLast           (GBL_CSELF)                                                    GBL_NOEXCEPT;
GBL_INLINE GblNaryTreeNode* GblNaryTree_childBefore         (GBL_CSELF, const GblNaryTreeNode* pChild)                     GBL_NOEXCEPT;
GBL_INLINE GblNaryTreeNode* GblNaryTree_childAt             (GBL_CSELF, GblSize index)                                     GBL_NOEXCEPT;
GBL_INLINE GblSize          GblNaryTree_childIndex          (GBL_CSELF, const GblNaryTreeNode* pChild)                     GBL_NOEXCEPT;

GBL_INLINE void             GblNaryTree_addChildFront       (GBL_SELF, GblNaryTreeNode* pChild)                            GBL_NOEXCEPT;
GBL_INLINE void             GblNaryTree_addChildBack        (GBL_SELF, GblNaryTreeNode* pChild)                            GBL_NOEXCEPT;
GBL_INLINE void             GblNaryTree_addChildTo          (GBL_SELF, GblSize index, GblNaryTreeNode* pChild)             GBL_NOEXCEPT;
GBL_INLINE void             GblNaryTree_addChildBefore      (GBL_SELF, GblNaryTreeNode* pBefore,GblNaryTreeNode* pChild)   GBL_NOEXCEPT;
GBL_INLINE void             GblNaryTree_addChildAfter       (GBL_SELF, GblNaryTreeNode* pAfter, GblNaryTreeNode* pChild)   GBL_NOEXCEPT;

GBL_INLINE void             GblNaryTree_moveChildFront      (GBL_SELF, GblNaryTreeNode* pChild)                            GBL_NOEXCEPT;
GBL_INLINE void             GblNaryTree_moveChildBack       (GBL_SELF, GblNaryTreeNode* pChild)                            GBL_NOEXCEPT;
GBL_INLINE void             GblNaryTree_moveChildTo         (GBL_SELF, GblSize index, GblNaryTreeNode* pChild)             GBL_NOEXCEPT;

GBL_INLINE GblNaryTreeNode* GblNaryTree_removeChild         (GBL_SELF, GblNaryTreeNode* pChild)                            GBL_NOEXCEPT;
GBL_INLINE GblNaryTreeNode* GblNaryTree_removeChildFront    (GBL_SELF)                                                     GBL_NOEXCEPT;
GBL_INLINE GblNaryTreeNode* GblNaryTree_removeChildBack     (GBL_SELF)                                                     GBL_NOEXCEPT;
GBL_INLINE GblNaryTreeNode* GblNaryTree_removeChildAt       (GBL_SELF, GblSize index)                                      GBL_NOEXCEPT;

GBL_INLINE void             GblNaryTree_replaceChild        (GBL_SELF, GblNaryTreeNode* pOld, GblNaryTreeNode* pNew)       GBL_NOEXCEPT;
GBL_INLINE void             GblNaryTree_replaceChildAt      (GBL_SELF, GblSize index, GblNaryTreeNode* pNewChild)          GBL_NOEXCEPT;

GBL_INLINE void             GblNaryTree_swapChildren        (GBL_SELF, GblNaryTreeNode* pChild1, GblNaryTreeNode* pChild2) GBL_NOEXCEPT;
GBL_INLINE void             GblNaryTree_swapChildrenAt      (GBL_SELF, GblSize index1, GblSize index2)                     GBL_NOEXCEPT;

GBL_INLINE void             GblNaryTree_reverseChildren     (GBL_SELF)                                                     GBL_NOEXCEPT;

GBL_INLINE GblNaryTreeNode* GblNaryTree_root                (GBL_SELF)                                                     GBL_NOEXCEPT;
GBL_INLINE GblNaryTreeNode* GblNaryTree_base                (GBL_SELF, GblSize depth)                                      GBL_NOEXCEPT;
GBL_INLINE GblNaryTreeNode* GblNaryTree_ancestor            (GBL_CSELF, GblSize height)                                    GBL_NOEXCEPT;
GBL_INLINE GblSize          GblNaryTree_ancestorHeight      (GBL_CSELF, const GblNaryTreeNode* pParent)                    GBL_NOEXCEPT;

GBL_INLINE GblSize          GblNaryTree_siblingCount        (GBL_CSELF)                                                    GBL_NOEXCEPT;
GBL_INLINE GblNaryTreeNode* GblNaryTree_siblingLast         (GBL_CSELF)                                                    GBL_NOEXCEPT;
GBL_INLINE GblNaryTreeNode* GblNaryTree_siblingBefore       (GBL_CSELF)                                                    GBL_NOEXCEPT;
GBL_INLINE GblNaryTreeNode* GblNaryTree_siblingFirst        (GBL_CSELF)                                                    GBL_NOEXCEPT;
GBL_INLINE GblNaryTreeNode* GblNaryTree_siblingAt           (GBL_CSELF, GblSize index)                                     GBL_NOEXCEPT;
GBL_INLINE GblSize          GblNaryTree_siblingIndex        (GBL_CSELF, const GblNaryTreeNode* pOther)                     GBL_NOEXCEPT;

GBL_INLINE void             GblNaryTree_disconnect          (GBL_SELF)                                                     GBL_NOEXCEPT;

//GBL_INLINE GblNaryTreeNode* GblNaryTree_lowestCommonAncestor(GBL_CSELF, const GblNaryTreeNode* pOther)                     GBL_NOEXCEPT;
//GBL_INLINE GblSize          GblNaryTree_distance            (GBL_CSELF, const GblNaryTreeNode* pOther)                     GBL_NOEXCEPT;
GBL_INLINE GblBool          GblNaryTree_traverse            (GBL_CSELF, GblFlags mask, GblNaryTreeIterFn pFnIt, void* pUd) GBL_NOEXCEPT;
GBL_INLINE GblBool          GblNaryTree_traverseInOrder     (GBL_CSELF, GblFlags mask, GblNaryTreeIterFn pFnIter, void* pUd) GBL_NOEXCEPT;
GBL_INLINE GblBool          GblNaryTree_traversePreOrder    (GBL_CSELF, GblFlags mask, GblNaryTreeIterFn pFnIter, void* pUd) GBL_NOEXCEPT;
GBL_INLINE GblBool          GblNaryTree_traversePostOrder   (GBL_CSELF, GblFlags mask, GblNaryTreeIterFn pFnIter, void* pUd) GBL_NOEXCEPT;


// ===== IMPL ====

GBL_INLINE GblBool GblNaryTree_isLeaf(GBL_CSELF) GBL_NOEXCEPT {
    return !GblNaryTree_childCount(pSelf)? GBL_TRUE : GBL_FALSE;
}

GBL_INLINE GblBool GblNaryTree_isInternal(GBL_CSELF) GBL_NOEXCEPT {
    return !GblNaryTree_isLeaf(pSelf);
}

GBL_INLINE GblBool GblNaryTree_isRoot(GBL_CSELF) GBL_NOEXCEPT {
    return pSelf->pParent? GBL_FALSE : GBL_TRUE;
}

GBL_INLINE GblBool GblNaryTree_isConnected(GBL_CSELF) GBL_NOEXCEPT {
    return pSelf->pParent || pSelf->pChildFirst || pSelf->pSiblingNext;
}

GBL_INLINE GblBool GblNaryTree_isParent(GBL_CSELF, const GblNaryTreeNode* pOther) GBL_NOEXCEPT {
    return pSelf->pParent == pOther? GBL_TRUE : GBL_FALSE;
}

GBL_INLINE GblBool GblNaryTree_isAncestor(GBL_CSELF, const GblNaryTreeNode* pOther) GBL_NOEXCEPT {
    return GblNaryTree_ancestorHeight(pSelf, pOther) != GBL_NARY_TREE_NPOS? GBL_TRUE : GBL_FALSE;
}

GBL_INLINE GblBool GblNaryTree_isSibling(GBL_CSELF, const GblNaryTreeNode* pOther) GBL_NOEXCEPT {
    if(pSelf->pParent && pSelf->pParent == pOther->pParent && pSelf != pOther) {
        return GBL_TRUE;
    }
    return GBL_FALSE;
}

GBL_INLINE GblBool GblNaryTree_isChild(GBL_CSELF, const GblNaryTreeNode* pOther) GBL_NOEXCEPT {
    return GblNaryTree_childIndex(pSelf, pOther) != GBL_NARY_TREE_NPOS? GBL_TRUE : GBL_FALSE;
}

GBL_INLINE GblBool GblNaryTree_isDescendent(GBL_CSELF, const GblNaryTreeNode* pOther) GBL_NOEXCEPT {
    return GblNaryTree_isAncestor(pOther, pSelf);
}

GBL_INLINE GblFlags GblNaryTree_flags(GBL_CSELF) GBL_NOEXCEPT {
    GblFlags flags = GblNaryTree_isRoot(pSelf)? GBL_NARY_TREE_NODE_FLAG_ROOT : 0;
    if(GblNaryTree_isInternal(pSelf)) flags |= GBL_NARY_TREE_NODE_FLAG_INTERNAL;
    else flags |= GBL_NARY_TREE_NODE_FLAG_LEAF;
    return flags;
}

GBL_INLINE GblSize GblNaryTree_childCount(GBL_CSELF) GBL_NOEXCEPT {
    GblSize size = 0;
    for(const GblNaryTreeNode* pNode = pSelf->pChildFirst;
        pNode;
        pNode = pNode->pSiblingNext)
            ++size;
    return size;
}

GBL_INLINE GblNaryTreeNode* GblNaryTree_childLast(GBL_CSELF) GBL_NOEXCEPT {
    GblNaryTreeNode* pNode = pSelf->pChildFirst;
    for(;
        pNode && pNode->pSiblingNext;
        pNode = pNode->pSiblingNext);
    return pNode;
}

GBL_INLINE GblNaryTreeNode* GblNaryTree_childBefore(GBL_CSELF, const GblNaryTreeNode* pChild) GBL_NOEXCEPT {
    GblNaryTreeNode* pNode = pSelf->pChildFirst;
    for(;
        pNode;
        pNode = pNode->pSiblingNext)
            if(pNode->pSiblingNext == pChild)
                return pNode;
    return GBL_NULL;
}

GBL_INLINE GblNaryTreeNode* GblNaryTree_childAt(GBL_CSELF, GblSize index) GBL_NOEXCEPT {
    for(GblNaryTreeNode* pNode = pSelf->pChildFirst;
        pNode;
        pNode = pNode->pSiblingNext)
            if(index-- == 0) return pNode;
    return GBL_NULL;
}

GBL_INLINE GblSize GblNaryTree_childIndex(GBL_CSELF, const GblNaryTreeNode* pChild) GBL_NOEXCEPT {
    GblSize i = 0;
    for(GblNaryTreeNode* pNode = pSelf->pChildFirst;
        pNode;
        pNode = pNode->pSiblingNext)
    {
        if(pNode == pChild) return i;
        ++i;
    }
    return GBL_NARY_TREE_NPOS;
}

GBL_INLINE void GblNaryTree_addChildFront(GBL_SELF, GblNaryTreeNode* pChild) GBL_NOEXCEPT {
    pChild->pParent = pSelf;
    pChild->pSiblingNext = pSelf->pChildFirst;
    pSelf->pChildFirst = pChild;
}

GBL_INLINE void GblNaryTree_addChildBack(GBL_SELF, GblNaryTreeNode* pChild) GBL_NOEXCEPT {
    GblNaryTreeNode* pBack = GblNaryTree_childLast(pSelf);
    if(pBack) pBack->pSiblingNext = pChild;
    else pSelf->pChildFirst = pChild;
    pChild->pParent = pSelf;
    pChild->pSiblingNext = GBL_NULL;
}

GBL_INLINE void GblNaryTree_addChildTo(GBL_SELF, GblSize index, GblNaryTreeNode* pChild) GBL_NOEXCEPT {
    if(!index) { //insert beginning
        pChild->pSiblingNext = pSelf->pChildFirst;
        pSelf->pChildFirst = pChild;
        pChild->pParent = pSelf;
    } else {
        GblSize i = 0;
        GblNaryTreeNode* pPrev = GBL_NULL;
        for(GblNaryTreeNode* pNode = pSelf->pChildFirst;
            pNode;
            pNode = pNode->pSiblingNext)
        {
            if(i++ == index) { // insert middle
                pChild->pSiblingNext = pNode;
                if(pPrev) pPrev->pSiblingNext = pChild;
                pChild->pParent = pSelf;
            }
            pPrev = pNode;
        }
        if(i == index) { // insert end
            if(pPrev) pPrev->pSiblingNext = pChild;
            pChild->pSiblingNext = GBL_NULL;
            pChild->pParent = pSelf;
        }
    }
}

GBL_INLINE void GblNaryTree_addChildBefore(GBL_SELF, GblNaryTreeNode* pBefore, GblNaryTreeNode* pChild) GBL_NOEXCEPT  {
    GblNaryTreeNode* pBeforeBefore = GblNaryTree_childBefore(pSelf, pBefore);
    if(pBeforeBefore) {
        pBeforeBefore->pSiblingNext = pChild;
    } else {
        pSelf->pChildFirst = pChild;
    }
    pChild->pSiblingNext = pBefore;
    pChild->pParent = pSelf;
}

GBL_INLINE void GblNaryTree_addChildAfter(GBL_SELF, GblNaryTreeNode* pAfter, GblNaryTreeNode* pChild) GBL_NOEXCEPT {
    pChild->pSiblingNext = pAfter->pSiblingNext;
    pAfter->pSiblingNext = pChild;
    pChild->pParent = pSelf;
}

GBL_INLINE void GblNaryTree_moveChildFront(GBL_SELF, GblNaryTreeNode* pChild) GBL_NOEXCEPT {
    GblNaryTree_removeChild(pSelf, pChild);
    GblNaryTree_addChildFront(pSelf, pChild);
}

GBL_INLINE void GblNaryTree_moveChildBack(GBL_SELF, GblNaryTreeNode* pChild) GBL_NOEXCEPT {
    GblNaryTree_removeChild(pSelf, pChild);
    GblNaryTree_addChildBack(pSelf, pChild);
}

GBL_INLINE void GblNaryTree_moveChildTo(GBL_SELF, GblSize index, GblNaryTreeNode* pChild) GBL_NOEXCEPT {
    GblNaryTreeNode* pPrev = index? GblNaryTree_childAt(pSelf, index-1) : GBL_NULL;

    GblNaryTree_removeChild(pSelf, pChild);
    if(pPrev)
        GblNaryTree_addChildAfter(pSelf, pPrev, pChild);
    else
        GblNaryTree_addChildFront(pSelf, pChild);
}


GBL_INLINE GblNaryTreeNode* GblNaryTree_removeChild(GBL_SELF, GblNaryTreeNode* pChild) GBL_NOEXCEPT {
    GblNaryTreeNode* pBefore = GblNaryTree_childBefore(pSelf, pChild);
    if(pBefore) pBefore->pSiblingNext = pChild->pSiblingNext;
    else pSelf->pChildFirst = pChild->pSiblingNext;
    pChild->pParent = GBL_NULL;
    pChild->pSiblingNext = GBL_NULL;
    return GBL_NULL;
}

GBL_INLINE GblNaryTreeNode* GblNaryTree_removeChildFront(GBL_SELF) GBL_NOEXCEPT {
    GblNaryTreeNode* pFirst = pSelf->pChildFirst;
    pSelf->pChildFirst = pSelf->pChildFirst->pSiblingNext;
    pFirst->pSiblingNext = GBL_NULL;
    pFirst->pParent = GBL_NULL;
    return GBL_NULL;
}

GBL_INLINE GblNaryTreeNode* GblNaryTree_removeChildBack(GBL_SELF) GBL_NOEXCEPT {
    if(pSelf->pChildFirst) {
        if(!pSelf->pChildFirst->pSiblingNext) {
            pSelf->pChildFirst->pParent = GBL_NULL;
            pSelf->pChildFirst->pSiblingNext = GBL_NULL;
            pSelf->pChildFirst = GBL_NULL;
        } else {
            GblNaryTreeNode* pNode = pSelf->pChildFirst;
            for(;
                pNode->pSiblingNext->pSiblingNext;
                pNode = pNode->pSiblingNext)
                ;
            pNode->pSiblingNext->pParent = GBL_NULL;
            pNode->pSiblingNext->pSiblingNext = GBL_NULL;
            pNode->pSiblingNext = GBL_NULL;
        }
    }
    return GBL_NULL;
}

GBL_INLINE GblNaryTreeNode* GblNaryTree_removeChildAt(GBL_SELF, GblSize index) GBL_NOEXCEPT {
    if(!index) return GblNaryTree_removeChildFront(pSelf);
    else {
        GblSize i = 0;
        for(GblNaryTreeNode* pNode = pSelf->pChildFirst;
            pNode;
            pNode = pNode->pSiblingNext)
        {
            if(++i == index) {
                GblNaryTreeNode* pRemoved = pNode->pSiblingNext;
                pNode->pSiblingNext = pRemoved->pSiblingNext;
                pRemoved->pSiblingNext = GBL_NULL;
                pRemoved->pParent = GBL_NULL;
                return pRemoved;
            }
        }
    }
    return GBL_NULL;
}

GBL_INLINE void GblNaryTree_replaceChild(GBL_SELF, GblNaryTreeNode* pOld, GblNaryTreeNode* pNew) GBL_NOEXCEPT {
    GblNaryTreeNode* pPrev = GBL_NULL;
    GblNaryTreeNode* pNode = pSelf->pChildFirst;
    while(pNode) {
        if(pNode == pOld) {
            if(!pPrev) pSelf->pChildFirst = pNew;
            else pPrev->pSiblingNext = pNew;

            pNew->pSiblingNext = pOld->pSiblingNext;
            pNew->pParent = pSelf;
            return;
        }
        pPrev = pNode;
        pNode = pNode->pSiblingNext;
    }
}

GBL_INLINE void GblNaryTree_replaceChildAt(GBL_SELF, GblSize index, GblNaryTreeNode* pNewChild) GBL_NOEXCEPT {
    GblSize i = 0;
    GblNaryTreeNode* pNode = pSelf->pChildFirst;
    if(!index) {
        pNewChild->pSiblingNext = pSelf->pChildFirst? pSelf->pChildFirst->pSiblingNext: GBL_NULL;
        pSelf->pChildFirst = pNewChild;
        pNewChild->pParent = pSelf;
    } else {
        GblNaryTreeNode* pPrev = GBL_NULL;
        while(pNode) {
            if(i++ == index) {
                pPrev->pSiblingNext = pNewChild;
                pNewChild->pSiblingNext = pNode->pSiblingNext;
                pNewChild->pParent = pSelf;
                break;
            }
            pPrev = pNode;
            pNode = pNode->pSiblingNext;
        }
    }
}


GBL_INLINE void GblNaryTree_swapChildren(GBL_SELF,
                                         GblNaryTreeNode* pChild1,
                                         GblNaryTreeNode* pChild2) GBL_NOEXCEPT
{
    GblNaryTreeNode* pPrev1 = GblNaryTree_childBefore(pSelf, pChild1);
    GblNaryTreeNode* pPrev2 = GblNaryTree_childBefore(pSelf, pChild2);
    GblNaryTreeNode* pTemp;

    if(pPrev1) {
        pPrev1->pSiblingNext = pChild2;
    } else {
        pSelf->pChildFirst = pChild2;
    }

    if(pPrev2) {
        pPrev2->pSiblingNext = pChild1;
    } else {
        pSelf->pChildFirst = pChild1;
    }

    pTemp = pChild1->pSiblingNext;
    pChild1->pSiblingNext = pChild2->pSiblingNext;
    pChild2->pSiblingNext = pTemp;

}

GBL_INLINE void GblNaryTree_swapChildrenAt(GBL_SELF, GblSize index1, GblSize index2) GBL_NOEXCEPT {
    GblNaryTree_swapChildren(pSelf, GblNaryTree_childAt(pSelf, index1), GblNaryTree_childAt(pSelf, index2));
}

GBL_INLINE void GblNaryTree_reverseChildren(GBL_SELF) GBL_NOEXCEPT {
    GblNaryTreeNode* pPrev = GBL_NULL;
    GblNaryTreeNode* pNext = GBL_NULL;
    GblNaryTreeNode* pNode = pSelf->pChildFirst;

    while(pNode) {
        pNext = pNode->pSiblingNext;
        pNode->pSiblingNext = pPrev;
        pPrev = pNode;
        pNode = pNext;
    }

    pSelf->pChildFirst = pPrev;
}

GBL_INLINE GblSize GblNaryTree_depth(GBL_CSELF) GBL_NOEXCEPT {
    GblSize depth = 0;
    const GblNaryTreeNode* pNode = pSelf;
    while(pNode->pParent) GBL_LIKELY {
        ++depth;
        pNode = pNode->pParent;
    }
    return depth;
}

GBL_INLINE GblNaryTreeNode* GblNaryTree_base(GBL_SELF, GblSize depth) GBL_NOEXCEPT {
    GblSize d = GblNaryTree_depth(pSelf) - depth;
    return d? GblNaryTree_ancestor(pSelf, d - 1) : pSelf;
}

GBL_INLINE GblNaryTreeNode* GblNaryTree_root(GBL_SELF) GBL_NOEXCEPT {
    GblNaryTreeNode* pNode = pSelf;
    while(pNode->pParent) pNode = pNode->pParent;
    return pNode;
}

GBL_INLINE GblNaryTreeNode* GblNaryTree_ancestor(GBL_CSELF, GblSize height) GBL_NOEXCEPT {
    GblSize i = 0;
    GblNaryTreeNode* pNode = pSelf->pParent;
    while(pNode && i <= height) GBL_LIKELY {
        if(i++ == height) {
            return pNode;
        }
        pNode = pNode->pParent;
    }
    return GBL_NULL;
}

GBL_INLINE GblSize GblNaryTree_ancestorHeight(GBL_CSELF, const GblNaryTreeNode* pParent) GBL_NOEXCEPT {
    GblSize i = 0;
    const GblNaryTreeNode* pNode = pSelf->pParent;
    while(pNode) {
        if(pNode == pParent) {
            return i;
        }
        ++i;
        pNode = pNode->pParent;
    }
    return GBL_NARY_TREE_NPOS;
}

GBL_INLINE GblBool GblNaryTree_empty(GBL_CSELF) GBL_NOEXCEPT {
    return pSelf->pChildFirst? GBL_FALSE : GBL_TRUE;
}

GBL_INLINE GblNaryTreeNode* GblNaryTree_siblingAt(GBL_CSELF, GblSize index) GBL_NOEXCEPT {
    GblSize i = 0;
    GblNaryTreeNode* pNode = pSelf->pParent->pChildFirst;
    while(pNode) {
        if(pNode != pSelf && i++ == index) return pNode;
        pNode = pNode->pSiblingNext;
    }
    return GBL_NULL;
}

GBL_INLINE GblSize GblNaryTree_siblingIndex(GBL_CSELF, const GblNaryTreeNode* pSibling) GBL_NOEXCEPT {
    GblSize index = 0;
    const GblNaryTreeNode* pNode = pSelf->pParent->pChildFirst;
    while(pNode) {
        if(pNode != pSelf) {
            if(pNode != pSibling) ++index;
            else return index;
        }
        pNode = pNode->pSiblingNext;
    }
    return GBL_NARY_TREE_NPOS;
}

GBL_INLINE GblSize GblNaryTree_siblingCount(GBL_CSELF) GBL_NOEXCEPT {
    return pSelf->pParent? GblNaryTree_childCount(pSelf->pParent) - 1 : 0;
}

GBL_INLINE GblNaryTreeNode* GblNaryTree_siblingLast(GBL_CSELF) GBL_NOEXCEPT {
    GblNaryTreeNode* pLast = GBL_NULL;
    GblNaryTreeNode* pNode = pSelf->pSiblingNext;
    if(pSelf->pSiblingNext) {
        while(pNode) {
            pLast = pNode;
            pNode = pNode->pSiblingNext;
        }
    } else if(pSelf->pParent) {
        pLast = GblNaryTree_childBefore(pSelf->pParent, pSelf);
    }
    return pLast;
}

GBL_INLINE GblNaryTreeNode* GblNaryTree_siblingBefore(GBL_CSELF) GBL_NOEXCEPT {
    if(pSelf->pParent) {
        GblNaryTreeNode* pNode = pSelf->pParent->pChildFirst;
        while(pNode) {
           if(pNode->pSiblingNext == pSelf) return pNode;
           pNode = pNode->pSiblingNext;
        }
    }
    return GBL_NULL;
}

GBL_INLINE GblNaryTreeNode* GblNaryTree_siblingFirst(GBL_CSELF) GBL_NOEXCEPT {
    if(pSelf->pParent) {
        if(pSelf->pParent->pChildFirst != pSelf)
            return pSelf->pParent->pChildFirst;
        else
            return pSelf->pSiblingNext;
    }
    return GBL_NULL;
}


GBL_INLINE void GblNaryTree_disconnect(GBL_SELF) GBL_NOEXCEPT {
    if(pSelf->pParent) {
        GblNaryTree_removeChild(pSelf->pParent, pSelf);
    }

    while(pSelf->pChildFirst) {
        GblNaryTree_removeChildFront(pSelf);
    }
}

GBL_INLINE GblBool GblNaryTree_traversePreOrder(GBL_CSELF, GblFlags mask, GblNaryTreeIterFn pFnIter, void* pUd) GBL_NOEXCEPT {
    GblBool retVal = GBL_FALSE;

    if(GblNaryTree_flags(pSelf) & mask) {
        retVal = pFnIter(pSelf, pUd);
        if(retVal) return GBL_TRUE;
    }

    for(GblNaryTreeNode* pIt = pSelf->pChildFirst;
        pIt;
        pIt = pIt->pSiblingNext)
    {
        retVal = GblNaryTree_traversePreOrder(pIt, mask, pFnIter, pUd);
        if(retVal) return GBL_TRUE;
    }
    return GBL_FALSE;
}

GBL_INLINE GblBool GblNaryTree_traverseInOrder(GBL_CSELF, GblFlags mask, GblNaryTreeIterFn pFnIter, void* pUd) GBL_NOEXCEPT {
    GblBool retVal = GBL_FALSE;
    GblNaryTreeNode* pIt;

    for(pIt = pSelf->pChildFirst;
        pIt && pIt->pSiblingNext;
        pIt = pIt->pSiblingNext)
    {
        retVal = GblNaryTree_traverseInOrder(pIt, mask, pFnIter, pUd);
        if(retVal) return GBL_TRUE;
    }

    if(GblNaryTree_flags(pSelf) & mask) {
        retVal = pFnIter(pSelf, pUd);
        if(retVal) return GBL_TRUE;
    }

    if(pIt) {
        retVal = GblNaryTree_traverseInOrder(pIt, mask, pFnIter, pUd);
        if(retVal) return GBL_TRUE;
    }

    return GBL_FALSE;
}

GBL_INLINE GblBool GblNaryTree_traversePostOrder(GBL_CSELF, GblFlags mask, GblNaryTreeIterFn pFnIter, void* pUd) GBL_NOEXCEPT {

    GblBool retVal = GBL_FALSE;
    for(GblNaryTreeNode* pIt = pSelf->pChildFirst;
        pIt;
        pIt = pIt->pSiblingNext)
    {
        retVal = GblNaryTree_traversePostOrder(pIt, mask, pFnIter, pUd);
        if(retVal) return GBL_TRUE;
    }

    if(GblNaryTree_flags(pSelf) & mask) {
        retVal = pFnIter(pSelf, pUd);
        if(retVal) return GBL_TRUE;
    }

    return GBL_FALSE;
}

GBL_INLINE GblBool GblNaryTree_traverse(GBL_CSELF, GblFlags mask, GblNaryTreeIterFn pFnIter, void* pUd) GBL_NOEXCEPT {
    switch((mask >> 3)) {
    case GBL_NARY_TREE_TRAVERSAL_ORDER_PRE:
        return GblNaryTree_traversePreOrder(pSelf, mask, pFnIter, pUd);
    case GBL_NARY_TREE_TRAVERSAL_ORDER_POST:
        return GblNaryTree_traversePostOrder(pSelf, mask, pFnIter, pUd);
    case GBL_NARY_TREE_TRAVERSAL_ORDER_IN:
        return GblNaryTree_traverseInOrder(pSelf, mask, pFnIter, pUd);
    default:
    case GBL_NARY_TREE_TRAVERSAL_ORDER_LEVEL:
        GBL_ASSERT(GBL_FALSE);
        return GBL_FALSE;
    }
}


GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_NARY_TREE_H

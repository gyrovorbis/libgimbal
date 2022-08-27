/*! \file
 *  \brief GblNaryTreeNode structure and related functions
 *  \todo Finish implementing commented out API operations
 *  \ingroup containers
 */

#ifndef GIMBAL_NARY_TREE_H
#define GIMBAL_NARY_TREE_H

#include "../core/gimbal_typedefs.h"

#define GBL_NARY_TREE_NPOS                           GBL_NPOS
#define GBL_NARY_TREE_ENTRY(node, structure, field)  GBL_CONTAINER_OF(node, structure, field)
#define GBL_NARY_TREE_TRAVERSAL_MASK(order, flags)   ((order << 0x2) && flags)

#define GBL_SELF  GblNaryTreeNode* pSelf
#define GBL_CSELF const GBL_SELF

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblNaryTreeNode);

typedef GblBool (*GblNaryTreeIterFn)(GBL_CSELF, const GblNaryTreeNode*, void*);

typedef enum GBL_NARY_TREE_NODE_FLAGS {
    GBL_NARY_TREE_NODE_FLAG_ROOT     = 0x1,
    GBL_NARY_TREE_NODE_FLAG_INTERNAL = 0x2,
    GBL_NARY_TREE_NODE_FLAG_LEAF     = 0x4
} GBL_NARY_TREE_NODE_FLAGS;

typedef enum GBL_NARY_TREE_TRAVERSAL_ORDER {
    GBL_NARY_TREE_TRAVERSAL_ORDER_PRE,
    GBL_NARY_TREE_TRAVERSAL_ORDER_IN,
    GBL_NARY_TREE_TRAVERSAL_ORDER_POST,
    GBL_NARY_TREE_TRAVERSAL_ORDER_LEVEL
} GBL_NARY_TREE_TRAVERSAL_ORDER;

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

//GBL_INLINE GblNaryTreeNode* GblNaryTree_lowestCommonAncestor(GBL_CSELF, const GblNaryTreeNode* pOther)                     GBL_NOEXCEPT;
//GBL_INLINE GblSize          GblNaryTree_distance            (GBL_CSELF, const GblNaryTreeNode* pOther)                     GBL_NOEXCEPT;
//GBL_INLINE GblBool          GblNaryTree_traverse            (GBL_CSELF, GblFlags mask, GblNaryTreeIterFn pFnIt, void* pUd) GBL_NOEXCEPT;

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

// ===== DO BETTER =====
GBL_INLINE void GblNaryTree_moveChildFront(GBL_SELF, GblNaryTreeNode* pChild) GBL_NOEXCEPT {
    GblNaryTree_removeChild(pSelf, pChild);
    GblNaryTree_addChildFront(pSelf, pChild);
}

GBL_INLINE void GblNaryTree_moveChildBack(GBL_SELF, GblNaryTreeNode* pChild) GBL_NOEXCEPT {
    GblNaryTree_removeChild(pSelf, pChild);
    GblNaryTree_addChildBack(pSelf, pChild);
}

GBL_INLINE void GblNaryTree_moveChildTo(GBL_SELF, GblSize index, GblNaryTreeNode* pChild) GBL_NOEXCEPT {
    GblNaryTreeNode* pPrev = index? GblNaryTree_childAt(pSelf, index-1) : NULL;

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
    GblNaryTreeNode* pPrev = NULL;
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
        pNewChild->pSiblingNext = pSelf->pChildFirst? pSelf->pChildFirst->pSiblingNext: NULL;
        pSelf->pChildFirst = pNewChild;
        pNewChild->pParent = pSelf;
    } else {
        GblNaryTreeNode* pPrev = NULL;
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
    GblNaryTreeNode* pNext = NULL;
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
    GblNaryTreeNode* pLast = NULL;
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
    return NULL;
}

GBL_INLINE GblNaryTreeNode* GblNaryTree_siblingFirst(GBL_CSELF) GBL_NOEXCEPT {
    if(pSelf->pParent) {
        if(pSelf->pParent->pChildFirst != pSelf)
            return pSelf->pParent->pChildFirst;
        else
            return pSelf->pSiblingNext;
    }
    return NULL;
}


GBL_INLINE GblBool GblNaryTree_traverse(GBL_CSELF,
                                        GblEnum order,
                                        GblNaryTreeIterFn pFnIter,
                                        void* pUserdata) GBL_NOEXCEPT {
    return GBL_FALSE;
}


// ===== / DO BETTER =====


GBL_DECLS_END

#endif // GIMBAL_NARY_TREE_H

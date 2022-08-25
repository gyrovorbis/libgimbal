/*! \file
 *  \brief GblNaryTreeNode structure and related functions
 *  \ingroup containers
 */

#ifndef GIMBAL_NARY_TREE_H
#define GIMBAL_NARY_TREE_H

#include "../core/gimbal_typedefs.h"

#define GBL_NARY_TREE_NPOS                           GBL_NPOS
#define GBL_NARY_TREE_ENTRY(node, structure, field)  GBL_CONTAINER_OF(node, structure, field)

#define GBL_SELF  GblNaryTreeNode* pSelf
#define GBL_CSELF const GBL_SELF

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblNaryTreeNode);

typedef GblBool (*GblNaryTreeIterFn)(GBL_CSELF, const GblNaryTreeNode*, void*);

typedef enum GBL_NARY_TREE_TRAVERSAL_ORDER {
    GBL_NARY_TREE_TRAVERSAL_ORDER_PRE,
    GBL_NARY_TREE_TRAVERSAL_ORDER_IN,
    GBL_NARY_TREE_TRAVERSAL_ORDER_POST,
    GBL_NARY_TREE_TRAVERSAL_ORDER_LEVEL
} GBL_NARY_TREE_TRAVERSAL_ORDER;

typedef struct GblNaryTreeNode {
    struct GblNaryTreeNode* pParent;
    struct GblNaryTreeNode* pChildFirst;
    struct GblNaryTreeNode* pSiblingNext;
} GblNaryTreeNode;

GBL_INLINE GblSize          GblNaryTree_childCount         (GBL_CSELF)                                        GBL_NOEXCEPT;
GBL_INLINE GblNaryTreeNode* GblNaryTree_childLast          (GBL_CSELF)                                        GBL_NOEXCEPT;
GBL_INLINE GblNaryTreeNode* GblNaryTree_childBefore        (GBL_CSELF, const GblNaryTreeNode* pChild)         GBL_NOEXCEPT;
GBL_INLINE GblNaryTreeNode* GblNaryTree_childByIndex       (GBL_CSELF, GblSize index)                         GBL_NOEXCEPT;
GBL_INLINE GblSize          GblNaryTree_childIndex         (GBL_CSELF, const GblNaryTreeNode* pChild)         GBL_NOEXCEPT;

GBL_INLINE void             GblNaryTree_addChildFront      (GBL_SELF, GblNaryTreeNode* pChild)                GBL_NOEXCEPT;
GBL_INLINE void             GblNaryTree_addChildBack       (GBL_SELF, GblNaryTreeNode* pChild)                GBL_NOEXCEPT;
GBL_INLINE GblBool          GblNaryTree_addChildByIndex    (GBL_SELF, GblSize index, GblNaryTreeNode* pChild) GBL_NOEXCEPT;

GBL_INLINE void             GblNaryTree_addChildBefore     (GBL_SELF,
                                                            GblNaryTreeNode* pBefore,
                                                            GblNaryTreeNode* pChild)                          GBL_NOEXCEPT;

GBL_INLINE void             GblNaryTree_addChildAfter      (GBL_SELF,
                                                            GblNaryTreeNode* pAfter,
                                                            GblNaryTreeNode* pChild)                          GBL_NOEXCEPT;

GBL_INLINE void             GblNaryTree_moveChildFront     (GBL_SELF, GblNaryTreeNode* pChild)                GBL_NOEXCEPT;
GBL_INLINE void             GblNaryTree_moveChildBack      (GBL_SELF, GblNaryTreeNode* pChild)                GBL_NOEXCEPT;
GBL_INLINE void             GblNaryTree_moveChildByIndex   (GBL_SELF, GblSize index, GblNaryTreeNode* pChild) GBL_NOEXCEPT;

GBL_INLINE GblNaryTreeNode* GblNaryTree_removeChild        (GBL_SELF, GblNaryTreeNode* pChild)                GBL_NOEXCEPT;
GBL_INLINE GblNaryTreeNode* GblNaryTree_removeChildFront   (GBL_SELF)                                         GBL_NOEXCEPT;
GBL_INLINE GblNaryTreeNode* GblNaryTree_removeChildBack    (GBL_SELF)                                         GBL_NOEXCEPT;
GBL_INLINE GblNaryTreeNode* GblNaryTree_removeChildByIndex (GBL_SELF, GblSize index)                          GBL_NOEXCEPT;

GBL_INLINE void             GblNaryTree_replaceChild       (GBL_SELF,
                                                            GblNaryTreeNode* pChild,
                                                            GblNaryTreeNode* pNewChild)                       GBL_NOEXCEPT;

GBL_INLINE void             GblNaryTree_replaceChildByIndex(GBL_SELF,
                                                            GblSize index,
                                                            GblNaryTreeNode* pNewChild)                       GBL_NOEXCEPT;

GBL_INLINE void             GblNaryTree_swapChildren       (GBL_SELF,
                                                            GblNaryTreeNode* pChild1,
                                                            GblNaryTreeNode* pChild2)                         GBL_NOEXCEPT;

GBL_INLINE void             GblNaryTree_swapChildrenByIndex(GBL_SELF, GblSize index1, GblSize index2)         GBL_NOEXCEPT;

GBL_INLINE void             GblNaryTree_reverseChildren    (GBL_SELF)                                         GBL_NOEXCEPT;

GBL_INLINE GblSize          GblNaryTree_depth              (GBL_CSELF)                                        GBL_NOEXCEPT;
GBL_INLINE GblNaryTreeNode* GblNaryTree_root               (GBL_CSELF)                                        GBL_NOEXCEPT;
GBL_INLINE GblNaryTreeNode* GblNaryTree_ancestor           (GBL_CSELF, int relativePos)                       GBL_NOEXCEPT;
GBL_INLINE GblSize          GblNaryTree_ancestorIndex      (GBL_CSELF, const GblNaryTreeNode* pParent)        GBL_NOEXCEPT;

GBL_INLINE GblBool          GblNaryTree_traverse           (GBL_CSELF,
                                                            GblEnum order,
                                                            GblNaryTreeIterFn pFnIter,
                                                            void* pUserdata)                                  GBL_NOEXCEPT;


// ===== IMPL ====

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
            if(pNode->pSiblingNext == pChild) break;
    return pNode;
}

GBL_INLINE GblNaryTreeNode* GblNaryTree_childByIndex(GBL_CSELF, GblSize index) GBL_NOEXCEPT {
    for(GblNaryTreeNode* pNode = pSelf->pChildFirst;
        pNode;
        pNode = pNode->pSiblingNext)
            if(index-- == 0) return pNode;
    return NULL;
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
    pChild->pSiblingNext = NULL;
}

GBL_INLINE GblBool GblNaryTree_addChildByIndex(GBL_SELF, GblSize index, GblNaryTreeNode* pChild) GBL_NOEXCEPT {
    if(!index) { //insert beginning
        pChild->pSiblingNext = pSelf->pChildFirst;
        pSelf->pChildFirst = pChild;
        pChild->pParent = pSelf;
        return GBL_TRUE;
    } else {
        GblSize i = 0;
        GblNaryTreeNode* pPrev = NULL;
        for(GblNaryTreeNode* pNode = pSelf->pChildFirst;
            pNode;
            pNode = pNode->pSiblingNext)
        {
            if(i++ == index) { // insert middle
                pChild->pSiblingNext = pNode;
                if(pPrev) pPrev->pSiblingNext = pNode;
                pChild->pParent = pSelf;
                return GBL_TRUE;
            }
        }
        if(i == index) { // insert end
            if(pPrev) pPrev->pSiblingNext = pChild;
            pChild->pSiblingNext = NULL;
            pChild->pParent = pSelf;
        }
    }
    return GBL_FALSE;
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

GBL_INLINE void GblNaryTree_moveChildByIndex(GBL_SELF, GblSize index, GblNaryTreeNode* pChild) GBL_NOEXCEPT {

}


GBL_INLINE GblNaryTreeNode* GblNaryTree_removeChild(GBL_SELF, GblNaryTreeNode* pChild) GBL_NOEXCEPT {
    GblNaryTreeNode* pBefore = GblNaryTree_childBefore(pSelf, pChild);
    if(pBefore) pBefore = pChild->pSiblingNext;
    else pSelf->pChildFirst = NULL;
    pChild->pParent = NULL;
    pChild->pSiblingNext = NULL;
    return NULL;
}

GBL_INLINE GblNaryTreeNode* GblNaryTree_removeChildFront(GBL_SELF) GBL_NOEXCEPT {
    GblNaryTreeNode* pFirst = pSelf->pChildFirst;
    pSelf->pChildFirst = pSelf->pChildFirst->pSiblingNext;
    pFirst->pSiblingNext = NULL;
    pFirst->pParent = NULL;
    return NULL;
}

GBL_INLINE GblNaryTreeNode* GblNaryTree_removeChildBack(GBL_SELF) GBL_NOEXCEPT {
    if(pSelf->pChildFirst) {
        if(!pSelf->pChildFirst->pSiblingNext) {
            pSelf->pChildFirst->pParent = NULL;
            pSelf->pChildFirst->pSiblingNext = NULL;
            pSelf->pChildFirst = NULL;
        } else {
            GblNaryTreeNode* pNode = pSelf->pChildFirst;
            for(;
                pNode->pSiblingNext->pSiblingNext;
                pNode = pNode->pSiblingNext)
                ;
                pNode->pSiblingNext->pParent = NULL;
                pNode->pSiblingNext->pSiblingNext = NULL;
                pNode->pSiblingNext = NULL;
        }
    }
    return NULL;
}

GBL_INLINE GblNaryTreeNode* GblNaryTree_removeChildByIndex(GBL_SELF, GblSize index) GBL_NOEXCEPT {
    return NULL;
}

// ===== / DO BETTER =====


GBL_DECLS_END

#endif // GIMBAL_NARY_TREE_H

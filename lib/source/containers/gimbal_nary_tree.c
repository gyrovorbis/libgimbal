#include <gimbal/containers/gimbal_nary_tree.h>


GBL_EXPORT GblBool GblNaryTree_isLeaf(const GblNaryTreeNode* pSelf) {
    return !GblNaryTree_childCount(pSelf)? GBL_TRUE : GBL_FALSE;
}

GBL_EXPORT GblBool GblNaryTree_isInternal(const GblNaryTreeNode* pSelf) {
    return !GblNaryTree_isLeaf(pSelf);
}

GBL_EXPORT GblBool GblNaryTree_isRoot(const GblNaryTreeNode* pSelf) {
    return pSelf->pParent? GBL_FALSE : GBL_TRUE;
}

GBL_EXPORT GblBool GblNaryTree_isConnected(const GblNaryTreeNode* pSelf) {
    return pSelf->pParent || pSelf->pChildFirst || pSelf->pSiblingNext;
}

GBL_EXPORT GblBool GblNaryTree_isParent(const GblNaryTreeNode* pSelf, const GblNaryTreeNode* pOther) {
    return pSelf->pParent == pOther? GBL_TRUE : GBL_FALSE;
}

GBL_EXPORT GblBool GblNaryTree_isAncestor(const GblNaryTreeNode* pSelf, const GblNaryTreeNode* pOther) {
    return GblNaryTree_ancestorHeight(pSelf, pOther) != GBL_NARY_TREE_NPOS? GBL_TRUE : GBL_FALSE;
}

GBL_EXPORT GblBool GblNaryTree_isSibling(const GblNaryTreeNode* pSelf, const GblNaryTreeNode* pOther) {
    if(pSelf->pParent && pSelf->pParent == pOther->pParent && pSelf != pOther) {
        return GBL_TRUE;
    }
    return GBL_FALSE;
}

GBL_EXPORT GblBool GblNaryTree_isChild(const GblNaryTreeNode* pSelf, const GblNaryTreeNode* pOther) {
    return GblNaryTree_childIndex(pSelf, pOther) != GBL_NARY_TREE_NPOS? GBL_TRUE : GBL_FALSE;
}

GBL_EXPORT GblBool GblNaryTree_isDescendent(const GblNaryTreeNode* pSelf, const GblNaryTreeNode* pOther) {
    return GblNaryTree_isAncestor(pOther, pSelf);
}

GBL_EXPORT GblFlags GblNaryTree_flags(const GblNaryTreeNode* pSelf) {
    GblFlags flags = GblNaryTree_isRoot(pSelf)? GBL_NARY_TREE_NODE_FLAG_ROOT : 0;
    if(GblNaryTree_isInternal(pSelf)) flags |= GBL_NARY_TREE_NODE_FLAG_INTERNAL;
    else flags |= GBL_NARY_TREE_NODE_FLAG_LEAF;
    return flags;
}

GBL_EXPORT size_t  GblNaryTree_childCount(const GblNaryTreeNode* pSelf) {
    size_t  size = 0;
    for(const GblNaryTreeNode* pNode = pSelf->pChildFirst;
        pNode;
        pNode = pNode->pSiblingNext)
            ++size;
    return size;
}

GBL_EXPORT GblNaryTreeNode* GblNaryTree_childLast(const GblNaryTreeNode* pSelf) {
    GblNaryTreeNode* pNode = pSelf->pChildFirst;
    for(;
        pNode && pNode->pSiblingNext;
        pNode = pNode->pSiblingNext);
    return pNode;
}

GBL_EXPORT GblNaryTreeNode* GblNaryTree_childBefore(const GblNaryTreeNode* pSelf, const GblNaryTreeNode* pChild) {
    GblNaryTreeNode* pNode = pSelf->pChildFirst;
    for(;
        pNode;
        pNode = pNode->pSiblingNext)
            if(pNode->pSiblingNext == pChild)
                return pNode;
    return GBL_NULL;
}

GBL_EXPORT GblNaryTreeNode* GblNaryTree_childAt(const GblNaryTreeNode* pSelf, size_t  index) {
    for(GblNaryTreeNode* pNode = pSelf->pChildFirst;
        pNode;
        pNode = pNode->pSiblingNext)
            if(index-- == 0) return pNode;
    return GBL_NULL;
}

GBL_EXPORT size_t  GblNaryTree_childIndex(const GblNaryTreeNode* pSelf, const GblNaryTreeNode* pChild) {
    size_t  i = 0;
    for(GblNaryTreeNode* pNode = pSelf->pChildFirst;
        pNode;
        pNode = pNode->pSiblingNext)
    {
        if(pNode == pChild) return i;
        ++i;
    }
    return GBL_NARY_TREE_NPOS;
}

GBL_EXPORT void GblNaryTree_addChildFront(GblNaryTreeNode* pSelf, GblNaryTreeNode* pChild) {
    pChild->pParent = pSelf;
    pChild->pSiblingNext = pSelf->pChildFirst;
    pSelf->pChildFirst = pChild;
}

GBL_EXPORT void GblNaryTree_addChildBack(GblNaryTreeNode* pSelf, GblNaryTreeNode* pChild)  {
    GblNaryTreeNode* pBack = GblNaryTree_childLast(pSelf);
    if(pBack) pBack->pSiblingNext = pChild;
    else pSelf->pChildFirst = pChild;
    pChild->pParent = pSelf;
    pChild->pSiblingNext = GBL_NULL;
}

GBL_EXPORT void GblNaryTree_addChildTo(GblNaryTreeNode* pSelf, size_t  index, GblNaryTreeNode* pChild) {
    if(!index) { //insert beginning
        pChild->pSiblingNext = pSelf->pChildFirst;
        pSelf->pChildFirst = pChild;
        pChild->pParent = pSelf;
    } else {
        size_t  i = 0;
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

GBL_EXPORT void GblNaryTree_addChildBefore(GblNaryTreeNode* pSelf, GblNaryTreeNode* pBefore, GblNaryTreeNode* pChild) {
    GblNaryTreeNode* pBeforeBefore = GblNaryTree_childBefore(pSelf, pBefore);
    if(pBeforeBefore) {
        pBeforeBefore->pSiblingNext = pChild;
    } else {
        pSelf->pChildFirst = pChild;
    }
    pChild->pSiblingNext = pBefore;
    pChild->pParent = pSelf;
}

GBL_EXPORT void GblNaryTree_addChildAfter(GblNaryTreeNode* pSelf, GblNaryTreeNode* pAfter, GblNaryTreeNode* pChild) {
    pChild->pSiblingNext = pAfter->pSiblingNext;
    pAfter->pSiblingNext = pChild;
    pChild->pParent = pSelf;
}

GBL_EXPORT void GblNaryTree_moveChildFront(GblNaryTreeNode* pSelf, GblNaryTreeNode* pChild) {
    GblNaryTree_removeChild(pSelf, pChild);
    GblNaryTree_addChildFront(pSelf, pChild);
}

GBL_EXPORT void GblNaryTree_moveChildBack(GblNaryTreeNode* pSelf, GblNaryTreeNode* pChild) {
    GblNaryTree_removeChild(pSelf, pChild);
    GblNaryTree_addChildBack(pSelf, pChild);
}

GBL_EXPORT void GblNaryTree_moveChildTo(GblNaryTreeNode* pSelf, size_t  index, GblNaryTreeNode* pChild) {
    GblNaryTreeNode* pPrev = index? GblNaryTree_childAt(pSelf, index-1) : GBL_NULL;

    GblNaryTree_removeChild(pSelf, pChild);
    if(pPrev)
        GblNaryTree_addChildAfter(pSelf, pPrev, pChild);
    else
        GblNaryTree_addChildFront(pSelf, pChild);
}


GBL_EXPORT GblNaryTreeNode* GblNaryTree_removeChild(GblNaryTreeNode* pSelf, GblNaryTreeNode* pChild) {
    GblNaryTreeNode* pBefore = GblNaryTree_childBefore(pSelf, pChild);
    if(pBefore) pBefore->pSiblingNext = pChild->pSiblingNext;
    else pSelf->pChildFirst = pChild->pSiblingNext;
    pChild->pParent = GBL_NULL;
    pChild->pSiblingNext = GBL_NULL;
    return GBL_NULL;
}

GBL_EXPORT GblNaryTreeNode* GblNaryTree_removeChildFront(GblNaryTreeNode* pSelf) {
    GblNaryTreeNode* pFirst = pSelf->pChildFirst;
    pSelf->pChildFirst = pSelf->pChildFirst->pSiblingNext;
    pFirst->pSiblingNext = GBL_NULL;
    pFirst->pParent = GBL_NULL;
    return GBL_NULL;
}

GBL_EXPORT GblNaryTreeNode* GblNaryTree_removeChildBack(GblNaryTreeNode* pSelf) {
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

GBL_EXPORT GblNaryTreeNode* GblNaryTree_removeChildAt(GblNaryTreeNode* pSelf, size_t  index) {
    if(!index) return GblNaryTree_removeChildFront(pSelf);
    else {
        size_t  i = 0;
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

GBL_EXPORT void GblNaryTree_replaceChild(GblNaryTreeNode* pSelf, GblNaryTreeNode* pOld, GblNaryTreeNode* pNew) {
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

GBL_EXPORT void GblNaryTree_replaceChildAt(GblNaryTreeNode* pSelf, size_t  index, GblNaryTreeNode* pNewChild) {
    size_t  i = 0;
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


GBL_EXPORT void GblNaryTree_swapChildren(GblNaryTreeNode* pSelf,
                                         GblNaryTreeNode* pChild1,
                                         GblNaryTreeNode* pChild2)
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

GBL_EXPORT void GblNaryTree_swapChildrenAt(GblNaryTreeNode* pSelf, size_t  index1, size_t  index2)  {
    GblNaryTree_swapChildren(pSelf, GblNaryTree_childAt(pSelf, index1), GblNaryTree_childAt(pSelf, index2));
}

GBL_EXPORT void GblNaryTree_reverseChildren(GblNaryTreeNode* pSelf) {
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

GBL_EXPORT size_t  GblNaryTree_depth(const GblNaryTreeNode* pSelf) {
    size_t  depth = 0;
    const GblNaryTreeNode* pNode = pSelf;
    while(pNode->pParent) GBL_LIKELY {
        ++depth;
        pNode = pNode->pParent;
    }
    return depth;
}

GBL_EXPORT GblNaryTreeNode* GblNaryTree_base(GblNaryTreeNode* pSelf, size_t  depth) {
    size_t  d = GblNaryTree_depth(pSelf) - depth;
    return d? GblNaryTree_ancestor(pSelf, d - 1) : pSelf;
}

GBL_EXPORT GblNaryTreeNode* GblNaryTree_root(GblNaryTreeNode* pSelf)  {
    GblNaryTreeNode* pNode = pSelf;
    while(pNode->pParent) pNode = pNode->pParent;
    return pNode;
}

GBL_EXPORT GblNaryTreeNode* GblNaryTree_ancestor(const GblNaryTreeNode* pSelf, size_t  height) {
    size_t  i = 0;
    GblNaryTreeNode* pNode = pSelf->pParent;
    while(pNode && i <= height) GBL_LIKELY {
        if(i++ == height) {
            return pNode;
        }
        pNode = pNode->pParent;
    }
    return GBL_NULL;
}

GBL_EXPORT size_t  GblNaryTree_ancestorHeight(const GblNaryTreeNode* pSelf, const GblNaryTreeNode* pParent) {
    size_t  i = 0;
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

GBL_EXPORT GblBool GblNaryTree_empty(const GblNaryTreeNode* pSelf) {
    return pSelf->pChildFirst? GBL_FALSE : GBL_TRUE;
}

GBL_EXPORT GblNaryTreeNode* GblNaryTree_siblingAt(const GblNaryTreeNode* pSelf, size_t  index) {
    size_t  i = 0;
    GblNaryTreeNode* pNode = pSelf->pParent->pChildFirst;
    while(pNode) {
        if(pNode != pSelf && i++ == index) return pNode;
        pNode = pNode->pSiblingNext;
    }
    return GBL_NULL;
}

GBL_EXPORT size_t  GblNaryTree_siblingIndex(const GblNaryTreeNode* pSelf, const GblNaryTreeNode* pSibling) {
    size_t  index = 0;
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

GBL_EXPORT size_t  GblNaryTree_siblingCount(const GblNaryTreeNode* pSelf) {
    return pSelf->pParent? GblNaryTree_childCount(pSelf->pParent) - 1 : 0;
}

GBL_EXPORT GblNaryTreeNode* GblNaryTree_siblingLast(const GblNaryTreeNode* pSelf) {
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

GBL_EXPORT GblNaryTreeNode* GblNaryTree_siblingBefore(const GblNaryTreeNode* pSelf) {
    if(pSelf->pParent) {
        GblNaryTreeNode* pNode = pSelf->pParent->pChildFirst;
        while(pNode) {
           if(pNode->pSiblingNext == pSelf) return pNode;
           pNode = pNode->pSiblingNext;
        }
    }
    return GBL_NULL;
}

GBL_EXPORT GblNaryTreeNode* GblNaryTree_siblingFirst(const GblNaryTreeNode* pSelf) {
    if(pSelf->pParent) {
        if(pSelf->pParent->pChildFirst != pSelf)
            return pSelf->pParent->pChildFirst;
        else
            return pSelf->pSiblingNext;
    }
    return GBL_NULL;
}


GBL_EXPORT void GblNaryTree_disconnect(GblNaryTreeNode* pSelf) {
    if(pSelf->pParent) {
        GblNaryTree_removeChild(pSelf->pParent, pSelf);
    }

    while(pSelf->pChildFirst) {
        GblNaryTree_removeChildFront(pSelf);
    }
}

GBL_EXPORT GblBool GblNaryTree_traversePreOrder(const GblNaryTreeNode* pSelf, GblFlags mask, GblNaryTreeIterFn pFnIter, void* pUd) {
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

GBL_EXPORT GblBool GblNaryTree_traverseInOrder(const GblNaryTreeNode* pSelf, GblFlags mask, GblNaryTreeIterFn pFnIter, void* pUd) {
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

GBL_EXPORT GblBool GblNaryTree_traversePostOrder(const GblNaryTreeNode* pSelf, GblFlags mask, GblNaryTreeIterFn pFnIter, void* pUd) {

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

GBL_EXPORT GblBool GblNaryTree_traverse(const GblNaryTreeNode* pSelf, GblFlags mask, GblNaryTreeIterFn pFnIter, void* pUd) {
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

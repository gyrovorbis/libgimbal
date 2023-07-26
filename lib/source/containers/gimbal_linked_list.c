#include <gimbal/containers/gimbal_linked_list.h>


GBL_EXPORT void GblLinkedList_init(GblLinkedListNode* pSelf)  {
    pSelf->pNext = pSelf;
}

GBL_EXPORT void GblLinkedList_move(GblLinkedListNode* pSelf, GblLinkedListNode* pHead) {
    GblLinkedListNode* pLast = GblLinkedList_back(pHead);
    if(pLast) {
        pSelf->pNext = pHead->pNext;
        pLast->pNext = pSelf;
        pHead->pNext = pHead;
    }
}

GBL_EXPORT void GblLinkedList_pushBack(GblLinkedListNode* pSelf, GblLinkedListNode* pNode) {
    GblLinkedListNode* pPrevIt = pSelf;
    for(GblLinkedListNode* pIt = pSelf->pNext;
        pIt != pSelf;
        pIt = pIt->pNext)
    {
        pPrevIt = pIt;
    }
    pPrevIt->pNext = pNode;
    pNode->pNext = pSelf;
}

GBL_EXPORT void GblLinkedList_pushFront(GblLinkedListNode* pSelf, GblLinkedListNode* pNode) {
    pNode->pNext = pSelf->pNext;
    pSelf->pNext = pNode;
}

GBL_EXPORT GblBool GblLinkedList_remove(GblLinkedListNode* pSelf, GblLinkedListNode* pNode) {
    GblBool found = GBL_FALSE;
    GblLinkedListNode* pPrevIt = pSelf;
    for(GblLinkedListNode* pIt = pSelf->pNext;
        pIt != pSelf;
        pIt = pIt->pNext)
    {
        if(pIt == pNode) {
            pPrevIt->pNext  = pNode->pNext;
            pNode->pNext    = pNode;
            found           = GBL_TRUE;
            break;
        }
        pPrevIt = pIt;
    }
    return found;
}

GBL_EXPORT GblLinkedListNode* GblLinkedList_erase(GblLinkedListNode* pSelf, size_t index) {
    size_t  count = 0;
    GblLinkedListNode* pPrevIt = pSelf;
    for(GblLinkedListNode* pIt = pSelf->pNext;
        pIt != pSelf;
        pIt = pIt->pNext)
    {
        if(count++ == index) {
            pPrevIt->pNext  = pIt->pNext;
            pIt->pNext      = pIt;
            return pIt;
        }
        pPrevIt = pIt;
    }
    return GBL_NULL;
}


GBL_EXPORT size_t GblLinkedList_find(const GblLinkedListNode* pSelf, GblLinkedListNode* pNode) {
    size_t  index = GBL_LINKED_LIST_NPOS;
    size_t  count = 0;
    for(GblLinkedListNode* pIt = pSelf->pNext;
        pIt != pSelf;
        pIt = pIt->pNext)
    {
        if(pIt == pNode) {
            index = count;
            break;
        }
        ++count;
    }
    return index;
}

GBL_EXPORT GblBool GblLinkedList_contains(const GblLinkedListNode* pSelf, GblLinkedListNode* pNode) {
    return GblLinkedList_find(pSelf, pNode) != GBL_LINKED_LIST_NPOS;
}

GBL_EXPORT GblBool GblLinkedList_empty(const GblLinkedListNode* pSelf)  {
    return pSelf->pNext == pSelf || pSelf->pNext == GBL_NULL;
}

GBL_EXPORT size_t  GblLinkedList_count(const GblLinkedListNode* pSelf)  {
    size_t  count = 0;
    for(GblLinkedListNode* pIt = pSelf->pNext;
        pIt != pSelf;
        pIt = pIt->pNext)
    {
        ++count;
    }
    return count;
}

GBL_EXPORT GblLinkedListNode* GblLinkedList_at(const GblLinkedListNode* pSelf, size_t index) {
    GblLinkedListNode* pNode = GBL_NULL;
    size_t  count = 0;
    for(GblLinkedListNode* pIt = pSelf->pNext;
        pIt != pSelf;
        pIt = pIt->pNext)
    {
        if(count++ == index) {
            pNode = pIt;
            break;
        }
    }
    return pNode;
}

GBL_EXPORT GblLinkedListNode* GblLinkedList_front(const GblLinkedListNode* pSelf) {
    return (pSelf->pNext != pSelf)? pSelf->pNext : GBL_NULL;
}

GBL_EXPORT GblLinkedListNode* GblLinkedList_back(const GblLinkedListNode* pSelf) {
    GblLinkedListNode* pPrevIt = (GblLinkedListNode*)pSelf;
    for(GblLinkedListNode* pIt = pSelf->pNext;
        pIt != pSelf;
        pIt = pIt->pNext)
    {
        pPrevIt = pIt;
    }
    return (pPrevIt != pSelf)? pPrevIt : GBL_NULL;
}

GBL_EXPORT GblLinkedListNode* GblLinkedList_beforeMiddle(const GblLinkedListNode* pSelf) {
    GblLinkedListNode* pFast = pSelf->pNext;
    GblLinkedListNode* pSlow = pSelf->pNext;
    //GblLinkedListNode* pPrevSlow = pSlow;

    while(pFast->pNext != pSelf &&
          pFast->pNext->pNext != pSelf)
    {
        pFast = pFast->pNext->pNext;
      //  pPrevSlow = pSlow;
        pSlow = pSlow->pNext;
    }

    //return pPrevSlow != pSelf? pPrevSlow : GBL_NULL;
    return pSlow != pSelf? pSlow : GBL_NULL;
}

GBL_EXPORT GblLinkedListNode* GblLinkedList_middle(const GblLinkedListNode* pSelf) {
    GblLinkedListNode* pBefore = GblLinkedList_beforeMiddle(pSelf);
    return (pBefore && pBefore->pNext != pSelf)? pBefore->pNext : GBL_NULL;
}

GBL_EXPORT void GblLinkedList_join_(GblLinkedListNode* pNewList,
                                    GblLinkedListNode* pPrev,
                                    GblLinkedListNode* pNext)
{
    GblLinkedListNode* pFirst   = pNewList->pNext;
    GblLinkedListNode *pLast    = GblLinkedList_back(pNewList);
    if(!pLast) pLast = pNewList;

    pPrev->pNext = pFirst;
    pLast->pNext = pNext;
}

GBL_EXPORT void GblLinkedList_splitAfter(GblLinkedListNode* pSelf, GblLinkedListNode* pNewHead, GblLinkedListNode* pAfter)  {
    GblLinkedListNode* pBack = GblLinkedList_back(pSelf);
    pBack->pNext = pNewHead;
    pNewHead->pNext = pAfter->pNext;
    pAfter->pNext = pSelf;
}

GBL_EXPORT void GblLinkedList_joinFront(GblLinkedListNode* pSelf, GblLinkedListNode* pList) {
    if(!GblLinkedList_empty(pList)) {
        GblLinkedList_join_(pList, pSelf, pSelf->pNext);
        GblLinkedList_init(pList);
    }
}


GBL_EXPORT void GblLinkedList_joinBack(GblLinkedListNode* pSelf, GblLinkedListNode* pList) {
    if(!GblLinkedList_empty(pList)) {
        GblLinkedListNode* pBack = GblLinkedList_back(pSelf);
        if(!pBack) pBack = pSelf;
        GblLinkedList_join_(pList, pBack, pSelf);
        GblLinkedList_init(pList);
    }
}

GBL_EXPORT void GblLinkedList_joinSorted(GblLinkedListNode* pSelf,
                                         GblLinkedListNode* pList,
                                         GblLinkedListCmpFn pCmpFn,
                                         void*              pCl)
{
    GBL_LINKED_LIST_NODE(dst);
    GblLinkedListNode** ppDst = &dst.pNext;
    GblLinkedListNode*  pSrc1 = pSelf->pNext;
    GblLinkedListNode*  pSrc2 = pList->pNext;

    while(1) {
        if(pSrc1 == pSelf) {
            *ppDst = &dst;
            pSelf->pNext = pSelf;
            GblLinkedList_joinBack(&dst, pList);
            break;
        } else if(pSrc2 == pList) {
            *ppDst = &dst;
            GblLinkedList_joinBack(&dst, pSelf);
            break;
        }

        if(pCmpFn(pSrc1, pSrc2, pCl) < 0) {
            pSelf->pNext = pSrc1->pNext;
            *ppDst = pSrc1;
            pSrc1 = *(ppDst = &(pSrc1->pNext));
            continue;
        } else {
            pList->pNext = pSrc2->pNext;
            *ppDst = pSrc2;
            pSrc2 = *(ppDst = &(pSrc2->pNext));
            continue;
        }
    }

    GblLinkedList_move(pSelf, &dst);
}

GBL_EXPORT void GblLinkedList_mergeSort(GblLinkedListNode* pSelf, GblLinkedListCmpFn pCmpFn, void* pClosure) {
    if(GblLinkedList_count(pSelf) < 2) {
        return;
    }

    GBL_LINKED_LIST_NODE(halfList);

    GblLinkedList_splitAfter(pSelf, &halfList, GblLinkedList_beforeMiddle(pSelf));

    GblLinkedList_mergeSort(pSelf, pCmpFn, pClosure);
    GblLinkedList_mergeSort(&halfList, pCmpFn, pClosure);

    GblLinkedList_joinSorted(pSelf, &halfList, pCmpFn, pClosure);
}

GBL_EXPORT void GblLinkedList_moveBack(GblLinkedListNode* pSelf, GblLinkedListNode* pNode) {
    for(GblLinkedListNode* pIt = pNode->pNext;
        pIt != pNode;
        pIt = pIt->pNext)
    {
        if(pIt->pNext == pNode) {
            pIt->pNext = pNode->pNext;
            break;
        }
    }
    GblLinkedList_pushBack(pSelf, pNode);
}

GBL_EXPORT void GblLinkedList_moveFront(GblLinkedListNode* pSelf, GblLinkedListNode* pNode) {
    for(GblLinkedListNode* pIt = pNode->pNext;
        pIt != pNode;
        pIt = pIt->pNext)
    {
        if(pIt->pNext == pNode) {
            pIt->pNext = pNode->pNext;
            break;
        }
    }
    GblLinkedList_pushFront(pSelf, pNode);
}

GBL_EXPORT void GblLinkedList_clear(GblLinkedListNode* pSelf) {
    GblLinkedListNode* pPrev = pSelf;
    GblLinkedListNode* pIt = pSelf->pNext;

    do {
        pPrev->pNext = pPrev;
        pPrev = pIt;
        pIt = pIt->pNext;
    } while(pIt != pSelf);
}

GBL_EXPORT GblLinkedListNode* GblLinkedList_popFront(GblLinkedListNode* pSelf) {
    GblLinkedListNode* pFront = GblLinkedList_front(pSelf);
    if(pFront) pSelf->pNext = pFront->pNext;
    return pFront;
}

GBL_EXPORT GblLinkedListNode* GblLinkedList_popBack(GblLinkedListNode* pSelf) {
    GblLinkedListNode* pBack = GBL_NULL;
    GblLinkedListNode* pPrev = pSelf;
    for(GblLinkedListNode* pIt = pSelf->pNext;
        pIt != pSelf;
        pIt = pIt->pNext)
    {
        if(pIt->pNext == pSelf) {
            pBack = pIt;
            pPrev->pNext = pSelf;
            break;
        }
        pPrev = pIt;
    }

    return pBack;
}

GBL_EXPORT void GblLinkedList_reverse(GblLinkedListNode* pSelf) {
    GblLinkedListNode* pPrev = pSelf;
    GblLinkedListNode* pNext = pSelf;
    for(GblLinkedListNode* pIt = pSelf->pNext;
        pIt != pSelf;
        pIt = pNext)
    {
        pNext = pIt->pNext;
        pIt->pNext = pPrev;
        pPrev = pIt;
    }
    pSelf->pNext = pPrev;
}

GBL_EXPORT void GblLinkedList_insertAfter(GblLinkedListNode* pNode1, GblLinkedListNode* pNode2) {
    pNode2->pNext = pNode1->pNext;
    pNode1->pNext = pNode2;
}

GBL_EXPORT GblBool GblLinkedList_insert(GblLinkedListNode* pSelf, GblLinkedListNode* pNode, size_t index) {
    size_t  count = 0;
    GblLinkedListNode* pPrev = pSelf;
    for(GblLinkedListNode* pIt = pSelf->pNext;
        pIt != pSelf;
        pIt = pIt->pNext)
    {
        //inserting in the middle
        if(count++ == index) {
            pNode->pNext = pPrev->pNext;
            pPrev->pNext = pNode;
            return GBL_TRUE;
        }
        pPrev = pIt;
    }
    // inserting at the end
    if(count == index) {
        GblLinkedList_pushBack(pSelf, pNode);
        return GBL_TRUE;
    }
    return GBL_FALSE;
}

GBL_EXPORT GblBool GblLinkedList_replace(GblLinkedListNode* pSelf,
                                         GblLinkedListNode* pExisting,
                                         GblLinkedListNode* pReplacement)
{
    GblBool replaced = GBL_FALSE;
    for(GblLinkedListNode* pIt = pReplacement->pNext;
        pIt != pReplacement;
        pIt = pIt->pNext)
    {
        if(pIt->pNext == pReplacement) {
            pIt->pNext = pReplacement->pNext;
            break;
        }
    }

    GblLinkedListNode* pPrevIt = pSelf;
    for(GblLinkedListNode* pIt = pSelf->pNext;
        pIt != pSelf;
        pIt = pIt->pNext)
    {
        if(pIt == pExisting) {
            pPrevIt->pNext      = pReplacement;
            pReplacement->pNext = pExisting->pNext;
            pExisting->pNext    = pExisting;
            replaced            = GBL_TRUE;
            break;
        }
        pPrevIt = pIt;
    }
    return replaced;
}

GBL_EXPORT GblBool GblLinkedList_swap(GblLinkedListNode* pSelf,
                                      GblLinkedListNode* pNode1,
                                      GblLinkedListNode* pNode2)
{
    GblBool found = GBL_TRUE;
    GblLinkedListNode* pOldNode2Next = pNode2->pNext;
    for(GblLinkedListNode* pIt = pSelf->pNext;
        pIt != pSelf;
        pIt = pIt->pNext)
    {
        if(pIt->pNext == pNode1) {
            pIt->pNext = pNode2;
            pNode2->pNext = pNode1->pNext;
            found = GBL_TRUE;
            break;
        }
    }
    if(found) {
        if(pOldNode2Next == pNode2) {
            pNode1->pNext = pNode1;
        } else {
            for(GblLinkedListNode* pIt = pOldNode2Next;
                pIt != pNode2;
                pIt = pIt->pNext)
            {
                if(pIt->pNext == pNode2) {
                    pIt->pNext = pNode1;
                    pNode1->pNext = pOldNode2Next;
                    break;
                }
            }
        }
    }
    return found;
}

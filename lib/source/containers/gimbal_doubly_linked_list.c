#include <gimbal/containers/gimbal_doubly_linked_list.h>

GBL_EXPORT void GblDoublyLinkedList_init(GblDoublyLinkedListNode* pSelf) {
    pSelf->pNext = pSelf->pPrev = pSelf;
}

GBL_EXPORT void GblDoublyLinkedList_move(GblDoublyLinkedListNode* pSelf, GblDoublyLinkedListNode* pHead) {
    if(!GblDoublyLinkedList_empty(pHead)) {
        pHead->pPrev->pNext = pSelf;
        pSelf->pPrev        = pHead->pPrev;
        pHead->pNext->pPrev = pSelf;
        pSelf->pNext        = pHead->pNext;
        pHead->pNext        = pHead;
        pHead->pPrev        = pHead;
    }
}

GBL_EXPORT void GblDoublyLinkedList_pushBack(GblDoublyLinkedListNode* pSelf, GblDoublyLinkedListNode* pNode) {
    GblDoublyLinkedListNode* pLast = pSelf->pPrev;
    pLast->pNext = pNode;
    pNode->pPrev = pLast;
    pNode->pNext = pSelf;
    pSelf->pPrev = pNode;
}

GBL_EXPORT void GblDoublyLinkedList_pushFront(GblDoublyLinkedListNode* pSelf, GblDoublyLinkedListNode* pNode) {
    pNode->pNext = pSelf->pNext;
    pSelf->pNext->pPrev = pNode;
    pNode->pPrev = pSelf;
    pSelf->pNext = pNode;
}

GBL_EXPORT size_t GblDoublyLinkedList_find(const GblDoublyLinkedListNode* pSelf, GblDoublyLinkedListNode* pNode) {
    return GblLinkedList_find(&pSelf->singleNode, &pNode->singleNode);
}

GBL_EXPORT GblBool GblDoublyLinkedList_contains(const GblDoublyLinkedListNode* pSelf, GblDoublyLinkedListNode* pNode) {
    return GblLinkedList_contains(&pSelf->singleNode, &pNode->singleNode);
}

GBL_EXPORT GblBool GblDoublyLinkedList_empty(const GblDoublyLinkedListNode* pSelf)  {
    return GblLinkedList_empty(&pSelf->singleNode) && (pSelf->pPrev == pSelf || pSelf->pPrev == NULL);
}

GBL_EXPORT size_t  GblDoublyLinkedList_count(const GblDoublyLinkedListNode* pSelf) {
    return GblLinkedList_count(&pSelf->singleNode);
}

GBL_EXPORT GblDoublyLinkedListNode* GblDoublyLinkedList_at(const GblDoublyLinkedListNode* pSelf, intptr_t index) {
    if(index >= 0) return (GblDoublyLinkedListNode*)GblLinkedList_at(&pSelf->singleNode, index);
    else {
        intptr_t count = index;
        for(GblDoublyLinkedListNode* pIt = pSelf->pPrev;
            pIt != pSelf;
            pIt = pIt->pPrev)
        {
            if(++count == 0) return pIt;
        }
        return GBL_NULL;
    }
}

GBL_EXPORT GblDoublyLinkedListNode* GblDoublyLinkedList_front(const GblDoublyLinkedListNode* pSelf) {
    return (GblDoublyLinkedListNode*)GblLinkedList_front(&pSelf->singleNode);
}

GBL_EXPORT GblDoublyLinkedListNode* GblDoublyLinkedList_back(const GblDoublyLinkedListNode* pSelf) {
    return pSelf->pPrev != pSelf? pSelf->pPrev : NULL;
}

GBL_EXPORT GblDoublyLinkedListNode* GblDoublyLinkedList_middle(const GblDoublyLinkedListNode* pSelf) {
    return (GblDoublyLinkedListNode*)GblLinkedList_middle(&pSelf->singleNode);
}

GBL_EXPORT GblDoublyLinkedListNode* GblDoublyLinkedList_beforeMiddle(const GblDoublyLinkedListNode* pSelf) {
    return (GblDoublyLinkedListNode*)GblLinkedList_beforeMiddle(&pSelf->singleNode);
}

GBL_EXPORT void GblDoublyLinkedList_remove(GblDoublyLinkedListNode* pNode) {
    pNode->pPrev->pNext = pNode->pNext;
    pNode->pNext->pPrev = pNode->pPrev;
    pNode->pNext        = pNode;
    pNode->pPrev        = pNode;
}

GBL_EXPORT GblBool GblDoublyLinkedList_replace(GblDoublyLinkedListNode* pExisting, GblDoublyLinkedListNode* pReplacement) {
    pReplacement->pPrev->pNext  = pReplacement->pNext;
    pReplacement->pNext->pPrev  = pReplacement->pPrev;

    pExisting->pPrev->pNext     = pReplacement;
    pExisting->pNext->pPrev     = pReplacement;

    pReplacement->pPrev         = pExisting->pPrev;
    pReplacement->pNext         = pExisting->pNext;

    pExisting->pPrev            = pExisting;
    pExisting->pNext            = pExisting;
    return GBL_TRUE;
}

GBL_EXPORT void GblDoublyLinkedList_splitAfter(GblDoublyLinkedListNode* pSelf,
                                               GblDoublyLinkedListNode* pNewHead,
                                               GblDoublyLinkedListNode* pAfter)
{
    GblDoublyLinkedListNode* pBack = GblDoublyLinkedList_back(pSelf);
    GBL_ASSERT(pBack != pAfter);
    pBack->pNext = pNewHead;
    pAfter->pNext->pPrev = pNewHead;
    pNewHead->pNext = pAfter->pNext;
    pNewHead->pPrev = pBack;
    pAfter->pNext = pSelf;
    pSelf->pPrev = pAfter;
}


GBL_EXPORT GblBool GblDoublyLinkedList_swap(GblDoublyLinkedListNode* pExisting, GblDoublyLinkedListNode* pReplacement) {
    pExisting->pPrev->pNext     = pReplacement;
    pExisting->pNext->pPrev     = pReplacement;
    pReplacement->pNext->pPrev  = pExisting;
    pReplacement->pPrev->pNext  = pExisting;

    GblDoublyLinkedListNode* pNode = pReplacement->pNext;
    pReplacement->pNext = pExisting->pNext;
    pExisting->pNext = pNode;
    pNode = pReplacement->pPrev;
    pReplacement->pPrev = pExisting->pPrev;
    pExisting->pPrev = pNode;
    return GBL_TRUE;
}

GBL_EXPORT void GblDoublyLinkedList_moveBack(GblDoublyLinkedListNode* pSelf, GblDoublyLinkedListNode* pNode) {
    pNode->pPrev->pNext = pNode->pNext;
    pNode->pNext->pPrev = pNode->pPrev;

    GblDoublyLinkedList_pushBack(pSelf, pNode);
}

GBL_EXPORT void GblDoublyLinkedList_moveFront(GblDoublyLinkedListNode* pSelf, GblDoublyLinkedListNode* pNode) {
    pNode->pPrev->pNext = pNode->pNext;
    pNode->pNext->pPrev = pNode->pPrev;

    GblDoublyLinkedList_pushFront(pSelf, pNode);
}

GBL_EXPORT GblDoublyLinkedListNode* GblDoublyLinkedList_erase(GblDoublyLinkedListNode* pSelf, intptr_t index) {
    GblDoublyLinkedListNode* pNode = GblDoublyLinkedList_at(pSelf, index);
    if(pNode) {
        GblDoublyLinkedList_remove(pNode);
        return pNode;
    } else return GBL_NULL;
}

GBL_EXPORT void GblDoublyLinkedList_clear(GblDoublyLinkedListNode* pSelf) {
    GblDoublyLinkedListNode* pPrev = pSelf;
    GblDoublyLinkedListNode* pIt = pSelf->pNext;

    do {
        pPrev->pNext = pPrev;
        pPrev->pPrev = pPrev;
        pPrev = pIt;
        pIt = pIt->pNext;

    } while(pIt != pSelf);
}

GBL_EXPORT void GblDoublyLinkedList_joinBack(GblDoublyLinkedListNode* pSelf, GblDoublyLinkedListNode* pList) {
    if(!GblDoublyLinkedList_empty(pList)) {
        pSelf->pPrev->pNext = pList->pNext;
        pList->pNext->pPrev = pSelf->pPrev;
        pList->pPrev->pNext = pSelf;
        pSelf->pPrev        = pList->pPrev;
        pList->pPrev        = pList;
        pList->pNext        = pList;
    }
}

GBL_EXPORT void GblDoublyLinkedList_joinFront(GblDoublyLinkedListNode* pSelf, GblDoublyLinkedListNode* pList) {
    if(!GblDoublyLinkedList_empty(pList)) {
        pSelf->pNext->pPrev = pList->pPrev;
        pList->pPrev->pNext = pSelf->pNext;
        pSelf->pNext        = pList->pNext;
        pList->pNext->pPrev = pSelf;
        pList->pNext        = pList;
        pList->pPrev        = pList;
    }
}

GBL_EXPORT void GblDoublyLinkedList_joinSorted(GblDoublyLinkedListNode* pSelf,
                                               GblDoublyLinkedListNode* pList,
                                               GblDoublyLinkedListCmpFn pCmpFn,
                                               void* pClosure)
{
    GBL_DOUBLY_LINKED_LIST_NODE(dstList);
    GblDoublyLinkedListNode*  pSrc1 = pSelf->pNext;
    GblDoublyLinkedListNode*  pSrc2 = pList->pNext;

    while(1) {

        if(pSrc1 == pSelf) {
            GblDoublyLinkedList_joinBack(&dstList, pList);
            break;
        } else if(pSrc2 == pList) {
            GblDoublyLinkedList_joinBack(&dstList, pSelf);
            break;
        }

        if(pCmpFn(pSrc1, pSrc2, pClosure) < 0) {
            GblDoublyLinkedListNode* pTemp = pSrc1;
            pSrc1 = pSrc1->pNext;
            GblDoublyLinkedList_remove(pTemp);
            GblDoublyLinkedList_pushBack(&dstList, pTemp);
            continue;
        } else {
            GblDoublyLinkedListNode* pTemp = pSrc2;
            pSrc2 = pSrc2->pNext;
            GblDoublyLinkedList_remove(pTemp);
            GblDoublyLinkedList_pushBack(&dstList, pTemp);
            continue;
        }
    }

    GBL_ASSERT(GblDoublyLinkedList_empty(pSelf));
    GBL_ASSERT(GblDoublyLinkedList_empty(pList));

    GblDoublyLinkedList_move(pSelf, &dstList);
}


GBL_EXPORT GblDoublyLinkedListNode* GblDoublyLinkedList_popBack(GblDoublyLinkedListNode* pSelf) {
    GblDoublyLinkedListNode* pBack = GblDoublyLinkedList_back(pSelf);
    if(pBack) {
        pBack->pPrev->pNext = pBack->pNext;
        pBack->pNext->pPrev = pBack->pPrev;
    }
    return pBack;
}

GBL_EXPORT GblDoublyLinkedListNode* GblDoublyLinkedList_popFront(GblDoublyLinkedListNode* pSelf) {
    GblDoublyLinkedListNode* pFront = GblDoublyLinkedList_front(pSelf);
    if(pFront) {
        pFront->pPrev->pNext = pFront->pNext;
        pFront->pNext->pPrev = pFront->pPrev;
    }
    return pFront;
}

GBL_EXPORT void GblDoublyLinkedList_reverse(GblDoublyLinkedListNode* pSelf) {
    GblDoublyLinkedListNode* pIt = pSelf;
    do {
        GblDoublyLinkedListNode* pTemp = pIt->pNext;
        pIt->pNext = pIt->pPrev;
        pIt->pPrev = pTemp;
        pIt = pTemp;
    } while(pIt != pSelf);
}

GBL_EXPORT void GblDoublyLinkedList_rotate(GblDoublyLinkedListNode* pSelf, intptr_t n) {
    if(n != 0) {
        GblDoublyLinkedListNode* pPivot = GblDoublyLinkedList_at(pSelf, n);
        GBL_ASSERT(pPivot);
        if(pPivot) {
            GblDoublyLinkedList_remove(pSelf);
            GblDoublyLinkedList_insertBefore(pPivot, pSelf);
        }
    }
}

GBL_EXPORT GblBool GblDoublyLinkedList_insert(GblDoublyLinkedListNode* pSelf, intptr_t idx, GblDoublyLinkedListNode* pNode) {
    if(idx >= 0) {
        intptr_t count = 0;
        for(GblDoublyLinkedListNode* pIt = pSelf->pNext;
            pIt != pSelf;
            pIt = pIt->pNext)
        {
            //inserting in the middle
            if(count++ == idx) {
                GblDoublyLinkedList_insertBefore(pIt, pNode);
                return GBL_TRUE;
            }
        }
        // inserting at the end
        if(count == idx) {
            GblDoublyLinkedList_pushBack(pSelf, pNode);
            return GBL_TRUE;
        }
    } else {
        intptr_t count = idx;
        for(GblDoublyLinkedListNode* pIt = pSelf->pPrev;
            pIt != pSelf;
            pIt = pIt->pPrev)
        {
            // inserting in middle
            if(++count == 0) {
                GblDoublyLinkedList_insertAfter(pIt, pNode);
                return GBL_TRUE;
            }
        }
        //inserting at front
        if(count+1 == 0) {
            GblDoublyLinkedList_pushFront(pSelf, pNode);
            return GBL_TRUE;
        }

    }
    return GBL_FALSE;
}

GBL_EXPORT void GblDoublyLinkedList_insertBefore(GblDoublyLinkedListNode* pNode1, GblDoublyLinkedListNode* pNode2) {
    pNode2->pPrev = pNode1->pPrev;
    pNode2->pPrev->pNext = pNode2;
    pNode1->pPrev = pNode2;
    pNode2->pNext = pNode1;
}

GBL_EXPORT void GblDoublyLinkedList_insertAfter(GblDoublyLinkedListNode* pNode1, GblDoublyLinkedListNode* pNode2) {
    pNode2->pNext = pNode1->pNext;
    pNode2->pNext->pPrev = pNode2;
    pNode1->pNext = pNode2;
    pNode2->pPrev = pNode1;
}

GBL_EXPORT GblDoublyLinkedListNode* GblDoublyLinkedList_removeBefore(GblDoublyLinkedListNode* pNode) {
    GblDoublyLinkedListNode* pPrev = pNode->pPrev;
    GblDoublyLinkedList_remove(pPrev);
    return pPrev;
}

GBL_EXPORT GblDoublyLinkedListNode* GblDoublyLinkedList_removeAfter(GblDoublyLinkedListNode* pNode) {
    GblDoublyLinkedListNode* pNext = pNode->pNext;
    GblDoublyLinkedList_remove(pNext);
    return pNext;
}

GBL_EXPORT GblBool GblDoublyLinkedList_join (GblDoublyLinkedListNode* pSelf, intptr_t idx, GblDoublyLinkedListNode* pList) {
    GblBool success = GBL_FALSE;
    GblDoublyLinkedListNode* pNode = GblDoublyLinkedList_at(pSelf, idx);

    if(!GblDoublyLinkedList_empty(pList)) {
        if(pNode) {
            if(idx >= 0) {
                pNode->pPrev->pNext = pList->pNext;
                pList->pNext->pPrev = pNode->pPrev;
                pNode->pPrev        = pList->pPrev;
                pList->pPrev->pNext = pNode;
            } else {
                pNode->pNext->pPrev = pList->pPrev;
                pList->pPrev->pNext = pNode->pNext;
                pNode->pNext        = pList->pNext;
                pList->pNext->pPrev = pNode;
            }
        } else {
            const intptr_t count = GblDoublyLinkedList_count(pSelf);
            if(idx == count) GblDoublyLinkedList_joinBack(pSelf, pList);
            else if(idx == -(count+1)) GblDoublyLinkedList_joinFront(pSelf, pList);
            else return GBL_FALSE;
        }

        GblDoublyLinkedList_init(pList);
        success = GBL_TRUE;
    }

    return success;
}

GBL_EXPORT void GblDoublyLinkedList_mergeSort(GblDoublyLinkedListNode* pSelf, GblDoublyLinkedListCmpFn pCmpFn, void* pClosure) {
    if(GblDoublyLinkedList_count(pSelf) < 2) {
        return;
    }

    GBL_DOUBLY_LINKED_LIST_NODE(halfList);

    GblDoublyLinkedList_splitAfter(pSelf, &halfList, GblDoublyLinkedList_beforeMiddle(pSelf));

    GblDoublyLinkedList_mergeSort(pSelf, pCmpFn, pClosure);
    GblDoublyLinkedList_mergeSort(&halfList, pCmpFn, pClosure);

    GblDoublyLinkedList_joinSorted(pSelf, &halfList, pCmpFn, pClosure);
}

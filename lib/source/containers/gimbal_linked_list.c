#include <gimbal/containers/gimbal_linked_list.h>


GBL_EXPORT GblBool GblLinkedList_replace(GblLinkedListNode* pSelf,
                                         GblLinkedListNode* pExisting,
                                         GblLinkedListNode* pReplacement) GBL_NOEXCEPT
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
                                      GblLinkedListNode* pNode2) GBL_NOEXCEPT
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

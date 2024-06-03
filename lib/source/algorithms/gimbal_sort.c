#include <gimbal/algorithms/gimbal_sort.h>

GBL_EXPORT void gblSortMerge(void* pArray, size_t count, size_t elemSize, GblSortComparatorFn pFnCmp) {
    uint8_t* pFirstHalf;
    uint8_t* pEofFirstHalf;
    uint8_t* pSecondHalf;
    uint8_t* pEofSecondHalf;
    uint8_t* pResultBuffer, *pResult;
    size_t   halfSize;

    if(count <= 1) return;

    halfSize = count / 2;
    pFirstHalf = (uint8_t*)pArray;
    pSecondHalf = (uint8_t*)pArray + (halfSize * elemSize);

    gblSortMerge(pFirstHalf, halfSize, elemSize, pFnCmp);
    gblSortMerge(pSecondHalf, count-halfSize, elemSize, pFnCmp);

    pEofFirstHalf = pSecondHalf;
    pEofSecondHalf = (uint8_t*)pArray + (count * elemSize);
    pResultBuffer = (uint8_t*)GBL_ALLOCA(count * elemSize);
    pResult = pResultBuffer;

    while(pFirstHalf < pEofFirstHalf && pSecondHalf < pEofSecondHalf) {
        if((*pFnCmp)(pFirstHalf, pSecondHalf) < 0) {
            memcpy(pResult, pFirstHalf, elemSize);
            pFirstHalf += elemSize;
        } else {
            memcpy(pResult, pSecondHalf, elemSize);
            pSecondHalf += elemSize;
        }
        pResult += elemSize;
    }

    while(pFirstHalf < pEofFirstHalf) {
        memcpy(pResult, pFirstHalf, elemSize);
        pFirstHalf += elemSize;
        pResult += elemSize;
    }

    while(pSecondHalf < pEofSecondHalf) {
        memcpy(pResult, pSecondHalf, elemSize);
        pSecondHalf += elemSize;
        pResult += elemSize;
    }

    memcpy(pArray, pResultBuffer, count * elemSize);
}


GBL_EXPORT void gblSortSelection(void* pArray, size_t  count, size_t  elemSize, GblSortComparatorFn pFnCmp) {
    void* pTemp = GBL_ALLOCA(elemSize);

    for(size_t  i = 0; i < count; ++i) {
        size_t  min = i;

        for(size_t  j = i+1; j < count; ++j) {
            if(pFnCmp((char*)pArray + elemSize*j, (char*)pArray + elemSize*min) < 0)
                min = j;
        }

        memcpy(pTemp, (char*)pArray + min*elemSize, elemSize);
        memcpy((char*)pArray + min*elemSize, (char*)pArray + i*elemSize, elemSize);
        memcpy((char*)pArray + i*elemSize, pTemp, elemSize);
    }
}

GBL_EXPORT void gblSortQuick(void* pArray, size_t  count, size_t  elemSize, GblSortComparatorFn pFnCmp) {
    qsort(pArray, count, elemSize, pFnCmp);
}

GBL_EXPORT void gblSortInsertion(void* pArray, size_t count, size_t  elemSize, GblSortComparatorFn pFnCmp) {
    void* pTemp = GBL_ALLOCA(elemSize);

    for(int i = 1; i < count; ++i) {
        memcpy(pTemp, (char*)pArray + i*elemSize, elemSize);
        int  j = i - 1;
        while(j >= 0 && pFnCmp(((char*)pArray + j*elemSize), pTemp) > 0) {
            memcpy((char*)pArray + (j+1)*elemSize, (char*)pArray + j*elemSize, elemSize);
            --j;
        }
        memcpy((char*)pArray + (j+1)*elemSize, pTemp, elemSize);
    }
}

GBL_EXPORT void gblSortShell(void* pArray, size_t  count, size_t elemSize, GblSortComparatorFn pFnCmp) {
    void* pTemp     = GBL_ALLOCA(elemSize);
    char* pItems    = (char*)pArray;
    for (int i = (int)count / 2; i > 0; i = i / 2) {
       for (int j = i; j < (int)count; j++) {
           for(int k = j - i; k >= 0; k -= i) {
               if(pFnCmp(pItems + (k+i)*elemSize, pItems + k*elemSize) >= 0)
                   break;
               else {
                   memcpy(pTemp,                    pItems + k*elemSize,        elemSize);
                   memcpy(pItems + k*elemSize,      pItems + (k+i)*elemSize,    elemSize);
                   memcpy(pItems + (i+k)*elemSize,  pTemp,                      elemSize);
               }
           }
       }
   }
}

GBL_EXPORT void gblSortBubble(void* pArray, size_t  count, size_t  elemSize, GblSortComparatorFn pFnCmp) {
    void* pTemp     = GBL_ALLOCA(elemSize);
    char* pItems    = (char*)pArray;
    for(size_t  i = count; i >= 1; --i) {
        for(size_t  j = 1; j < i; ++j) {
            if(pFnCmp(pItems + (j-1)*elemSize, pItems + j*elemSize) > 0) {
                memcpy(pTemp,                   pItems + (j-1)*elemSize,    elemSize);
                memcpy(pItems + (j-1)*elemSize, pItems + j*elemSize,        elemSize);
                memcpy(pItems + j*elemSize,     pTemp,                      elemSize);
            }
        }
    }
}

GBL_EXPORT void gblSortComb(void* pArray, size_t count, size_t elemSize, GblSortComparatorFn pFnCmp) {
    // Initialize gap
    int gap = count;

    // Initialize swapped as true to make sure that
    // loop runs
    GblBool swapped = GBL_TRUE;

    uint8_t* pTemp = GBL_ALLOCA(elemSize);

    // Keep running while gap is more than 1 and last
    // iteration caused a swap
    while (gap != 1 || swapped == GBL_TRUE)
    {
        // Find next gap
        // To find gap between elements
        gap = (gap * 10) / 13;

        if(gap < 1)
            gap = 1;

        // Initialize swapped as false so that we can
        // check if swap happened or not
        swapped = GBL_FALSE;

        // Compare all elements with current gap
        for (int i = 0; i < count - gap; ++i)
        {
            uint8_t* pA = GBL_PTR_OFFSET(pArray, i * elemSize);
            uint8_t* pB = GBL_PTR_OFFSET(pArray, (i + gap) * elemSize);

            if(pFnCmp(pA, pB) > 0) {
                memcpy(pTemp, pA, elemSize);
                memcpy(pA, pB, elemSize);
                memcpy(pB, pTemp, elemSize);
                swapped = GBL_TRUE;
            }
        }
    }
}

GBL_INLINE void heapify_(void* pArray, size_t n, int i, size_t elemSize, GblSortComparatorFn pFnCmp) {
    // Find largest among root, left child and right child
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && pFnCmp(GBL_PTR_OFFSET(pArray, left * elemSize), GBL_PTR_OFFSET(pArray, largest * elemSize)) > 0)
      largest = left;

    if (right < n && pFnCmp(GBL_PTR_OFFSET(pArray, right * elemSize), GBL_PTR_OFFSET(pArray, largest * elemSize)) > 0)
      largest = right;

    // Swap and continue heapifying if root is not largest
    if (largest != i) {
        void* pTemp = GBL_ALLOCA(elemSize);
        memcpy(pTemp, GBL_PTR_OFFSET(pArray, i * elemSize), elemSize);
        memcpy(GBL_PTR_OFFSET(pArray, i * elemSize), GBL_PTR_OFFSET(pArray, largest * elemSize), elemSize);
        memcpy(GBL_PTR_OFFSET(pArray, largest * elemSize), pTemp, elemSize);
        heapify_(pArray, n, largest, elemSize, pFnCmp);
    }
  }

GBL_EXPORT void gblSortHeap(void* pArray, size_t count, size_t elemSize, GblSortComparatorFn pFnCmp) {
        // Build max heap
        for(int i = count / 2 - 1; i >= 0; i--)
          heapify_(pArray, count, i, elemSize, pFnCmp);

        void* pTemp = GBL_ALLOCA(elemSize);

        // Heap sort
        for(int i = count - 1; i >= 0; i--) {
            memcpy(pTemp, pArray, elemSize);
            memcpy(pArray, GBL_PTR_OFFSET(pArray, i * elemSize), elemSize);
            memcpy(GBL_PTR_OFFSET(pArray, i * elemSize), pTemp, elemSize);

          // Heapify root element to get highest element at root again
          heapify_(pArray, i, 0, elemSize, pFnCmp);
        }
}

GBL_EXPORT size_t gblSearchBinary(void* pArray, size_t  elemSize, int l, int r, void* pTarget, GblSortComparatorFn pFnCmp) {
    if(r >= l) {
        int mid = l + (r - l) / 2;
        void* pMid = (char*)pArray + elemSize*mid;

        if(pFnCmp(pMid, pTarget) == 0)
            return mid;

        if(pFnCmp(pMid, pTarget) > 0)
            return gblSearchBinary(pArray, elemSize, l, mid - 1, pTarget, pFnCmp);

        return gblSearchBinary(pArray, elemSize, mid + 1, r, pTarget, pFnCmp);
    }
    return GBL_NPOS;
}

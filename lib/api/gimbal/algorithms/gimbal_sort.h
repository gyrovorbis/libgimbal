/*! \addtogroup algorithms
 * @{
 */
/*! \file
 *  \brief Generialized array sorting algorithms
 */

#ifndef GIMBAL_SORT_H
#define GIMBAL_SORT_H

#include "../core/gimbal_typedefs.h"

GBL_DECLS_BEGIN


typedef GblInt  (*GblSortComparatorFn)  (const void*, const void*);
typedef void    (*GblSortFn)            (void*, GblSize, GblSize, GblSortComparatorFn);

GBL_INLINE void gblSortSelection(void* pArray, GblSize count, GblSize elemSize, GblSortComparatorFn pFnCmp) GBL_NOEXCEPT;
GBL_INLINE void gblSortQuick    (void* pArray, GblSize count, GblSize elemSize, GblSortComparatorFn pFnCmp) GBL_NOEXCEPT;
GBL_INLINE void gblSortInsertion(void* pArray, GblSize count, GblSize elemSize, GblSortComparatorFn pFnCmp) GBL_NOEXCEPT;
GBL_INLINE void gblSortShell    (void* pArray, GblSize count, GblSize elemSize, GblSortComparatorFn pFnCmp) GBL_NOEXCEPT;
GBL_INLINE void gblSortBubble   (void* pArray, GblSize count, GblSize elemSize, GblSortComparatorFn pFnCmp) GBL_NOEXCEPT;

GBL_INLINE GblSize gblSearchBinary(void* pArray, GblSize elemSize, GblInt l, GblInt r, void* pTarget, GblSortComparatorFn pFnCmp) GBL_NOEXCEPT;


/// @}





// ======== INLINE IMPLEMENTATION ========


GBL_INLINE
void gblSortSelection(void* pArray, GblSize count, GblSize elemSize, GblSortComparatorFn pFnCmp) GBL_NOEXCEPT {
    void* pTemp = GBL_ALLOCA(elemSize);
    for(GblSize i = 0; i < count; ++i) {
        GblSize min = i;
        for(GblSize j = i+1; j < count; ++j) {
            if(pFnCmp((char*)pArray + elemSize*j, (char*)pArray + elemSize*min) < 0)
                min = j;
         }
        memcpy(pTemp, (char*)pArray + min*elemSize, elemSize);
        memcpy((char*)pArray + min*elemSize, (char*)pArray + i*elemSize, elemSize);
        memcpy((char*)pArray + i*elemSize, pTemp, elemSize);
    }
}

GBL_INLINE
void gblSortQuick(void* pArray, GblSize count, GblSize elemSize, GblSortComparatorFn pFnCmp) GBL_NOEXCEPT {
    qsort(pArray, count, elemSize, pFnCmp);
}

GBL_INLINE
void gblSortInsertion(void* pArray, GblSize count, GblSize elemSize, GblSortComparatorFn pFnCmp) GBL_NOEXCEPT {
    void* pTemp = GBL_ALLOCA(elemSize);
    for(GblSize i = 0; i < count; ++i) {
        memcpy(pTemp, (char*)pArray + i*elemSize, elemSize);
        GblSize j = i;
        while(pFnCmp(((char*)pArray + (j-1)*elemSize), pTemp) > 0) {
            memcpy((char*)pArray + j*elemSize, (char*)pArray + (j-1)*elemSize, elemSize);
            --j;
        }
        memcpy((char*)pArray + j*elemSize, pTemp, elemSize);
    }
}

GBL_INLINE
void gblSortShell(void* pArray, GblSize count, GblSize elemSize, GblSortComparatorFn pFnCmp) GBL_NOEXCEPT {
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

GBL_INLINE
void gblSortBubble(void* pArray, GblSize count, GblSize elemSize, GblSortComparatorFn pFnCmp) GBL_NOEXCEPT {
    void* pTemp     = GBL_ALLOCA(elemSize);
    char* pItems    = (char*)pArray;
    for(GblSize i = count; i >= 1; --i) {
        for(GblSize j = 1; j < i; ++j) {
            if(pFnCmp(pItems + (j-1)*elemSize, pItems + j*elemSize) > 0) {
                memcpy(pTemp,                   pItems + (j-1)*elemSize,    elemSize);
                memcpy(pItems + (j-1)*elemSize, pItems + j*elemSize,        elemSize);
                memcpy(pItems + j*elemSize,     pTemp,                      elemSize);
            }
        }
    }
}

GBL_INLINE
GblSize gblSearchBinary(void* pArray, GblSize elemSize, GblInt l, GblInt r, void* pTarget, GblSortComparatorFn pFnCmp) GBL_NOEXCEPT {
    if(r >= l) {
        GblInt mid = l + (r - l) / 2;
        void* pMid = (char*)pArray + elemSize*mid;

        if(pFnCmp(pMid, pTarget) == 0)
            return mid;

        if(pFnCmp(pMid, pTarget) > 0)
            return gblSearchBinary(pArray, elemSize, l, mid - 1, pTarget, pFnCmp);

        return gblSearchBinary(pArray, elemSize, mid + 1, r, pTarget, pFnCmp);
    }
    return GBL_NPOS;
}

GBL_DECLS_END

#endif // GIMBAL_SORT_H

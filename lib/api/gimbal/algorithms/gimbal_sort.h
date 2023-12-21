/*! \file
 *  \ingroup sorting
 *  \brief Generialized array sorting algorithms
 *
 *  This file contains a collection of sorting algorithms,
 *  each with the same function prototype so that they can
 *  be passed around generically. Each is expected to
 *  operate on an array of data, accepting a custom
 *  comparator callback.
 *
 *  \todo
 *      - gblSortRadix()
 *
 *  \author     2023 Falco Girgis
 *  \copyright  MIT License
 */

#ifndef GIMBAL_SORT_H
#define GIMBAL_SORT_H

#include "../core/gimbal_decls.h"

GBL_DECLS_BEGIN

//! Function taking two elements and returning their numeric difference as an integer
typedef int  (*GblSortComparatorFn) (const void*, const void*);
//! Generic function pointer type for containing one of the array sorting algorithms
typedef void (*GblSortFn)           (void*, size_t, size_t, GblSortComparatorFn);

/*! \defgroup sorting Sorting
 *  \ingroup algorithms
 *  \brief Collection of sorting algorithms
 * @{
 */
//! Performs a Selection Sort over the given array with \p count \p elemSize elements, using \p pFnCmp to compare them
GBL_EXPORT void gblSortSelection (void* pArray, size_t count, size_t elemSize, GblSortComparatorFn pFnCmp) GBL_NOEXCEPT;
//! Performs a Quick Sort over the given array with \p count \p elemSize elements, using \p pFnCmp to compare them
GBL_EXPORT void gblSortQuick     (void* pArray, size_t count, size_t elemSize, GblSortComparatorFn pFnCmp) GBL_NOEXCEPT;
//! Performs an Insertion Sort over the given array with \p count \p elemSize elements, using \p pFnCmp to compare them
GBL_EXPORT void gblSortInsertion (void* pArray, size_t count, size_t elemSize, GblSortComparatorFn pFnCmp) GBL_NOEXCEPT;
//! Performs a Shell Sort over the given array with \p count \p elemSize elements, using \p pFnCmp to compare them
GBL_EXPORT void gblSortShell     (void* pArray, size_t count, size_t elemSize, GblSortComparatorFn pFnCmp) GBL_NOEXCEPT;
//! Performs a Merge Sort over the given array with \p count \p elemSize elements, using \p pFnCmp to compare them
GBL_EXPORT void gblSortMerge     (void* pArray, size_t count, size_t elemSize, GblSortComparatorFn pFnCmp) GBL_NOEXCEPT;
//! Performs a Comb Sort over the given array with \p count \p elemSize elements, using \p pFnCmp to compare them
GBL_EXPORT void gblSortComb      (void* pArray, size_t count, size_t elemSize, GblSortComparatorFn pFnCmp) GBL_NOEXCEPT;
//! Performs a Bubble Sort over the given array with \p count \p elemSize elements, using \p pFnCmp to compare them
GBL_EXPORT void gblSortBubble    (void* pArray, size_t count, size_t elemSize, GblSortComparatorFn pFnCmp) GBL_NOEXCEPT;
//! Performs a Heap Sort over the given array with \p count \p elemSize elements, using \p pFnCmp to compare them
GBL_EXPORT void gblSortHeap      (void* pArray, size_t count, size_t elemSize, GblSortComparatorFn pFnCmp) GBL_NOEXCEPT;
//! @}

//! Performs a Binary Search over the given array, from index \p l to index \p r, returning its position
GBL_EXPORT size_t gblSearchBinary (void* pSrc, size_t elemSize, int l, int r, void* pDst, GblSortComparatorFn pFnCmp) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_SORT_H

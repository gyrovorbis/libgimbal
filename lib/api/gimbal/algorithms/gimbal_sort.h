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

typedef int  (*GblSortComparatorFn) (const void*, const void*);
typedef void (*GblSortFn)           (void*, size_t, size_t, GblSortComparatorFn);

/*! \defgroup sorting Sorting
 *  \ingroup algorithms
 *  \brief Collection of sorting algorithms
 * @{
 */
GBL_EXPORT void gblSortSelection (void* pArray, size_t count, size_t elemSize, GblSortComparatorFn pFnCmp) GBL_NOEXCEPT;
GBL_EXPORT void gblSortQuick     (void* pArray, size_t count, size_t elemSize, GblSortComparatorFn pFnCmp) GBL_NOEXCEPT;
GBL_EXPORT void gblSortInsertion (void* pArray, size_t count, size_t elemSize, GblSortComparatorFn pFnCmp) GBL_NOEXCEPT;
GBL_EXPORT void gblSortShell     (void* pArray, size_t count, size_t elemSize, GblSortComparatorFn pFnCmp) GBL_NOEXCEPT;
GBL_EXPORT void gblSortMerge     (void* pArray, size_t count, size_t elemSize, GblSortComparatorFn pFnCmp) GBL_NOEXCEPT;
GBL_EXPORT void gblSortComb      (void* pArray, size_t count, size_t elemSize, GblSortComparatorFn pFnCmp) GBL_NOEXCEPT;
GBL_EXPORT void gblSortBubble    (void* pArray, size_t count, size_t elemSize, GblSortComparatorFn pFnCmp) GBL_NOEXCEPT;
GBL_EXPORT void gblSortHeap      (void* pArray, size_t count, size_t elemSize, GblSortComparatorFn pFnCmp) GBL_NOEXCEPT;
//! @}

GBL_EXPORT size_t gblSearchBinary (void* pSrc, size_t  elemSize, int l, int r, void* pDst, GblSortComparatorFn pFnCmp) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_SORT_H

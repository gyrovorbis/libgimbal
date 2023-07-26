/*! \file
 *  \brief GblArrayHeap container and related functions
 *  \ingroup containers
 *  \copydoc GblArrayHeap
 *
 *  \author Falco Girgis
 */
#ifndef GIMBAL_ARRAY_HEAP_H
#define GIMBAL_ARRAY_HEAP_H

#include "gimbal_array_list.h"

#define GBL_SELF_TYPE GblArrayHeap

GBL_DECLS_BEGIN

typedef int (*GblArrayHeapCmpFn)(const void* pEntry1, const void* pEntry2);


/*! \brief Array-based binary heap implementing priority queue
 *
 *  GblArrayHeap is a dynamic array-backed (GblArrayList)
 *  binary heap, providing a priority queue implementation.
 *
 *  \note
 *  As GblArrayMap is backed by GblArrayList, it also can be
 *  over-allocated to provide additional storage for the
 *  underlying data before bleeding over an allocating
 *  storage from the heap.
 *  \sa GblArrayList
 *  \ingroup containers
 */
typedef struct GblArrayHeap {           // Size (32-bit / 64-bit)
    GBL_PRIVATE_BEGIN
        GblArrayHeapCmpFn   pFnCmp;     // 4/8      bytes
        GblArrayList        list;       // 20/36    bytes
    GBL_PRIVATE_END
} GblArrayHeap;                         // 24/42    total

// ===== Public Methods ======
GBL_EXPORT GBL_RESULT  GblArrayHeap_construct   (GBL_SELF,
                                                 size_t            elemSize,
                                                 GblArrayHeapCmpFn pFnCmp,
                                                 size_t            structSize,
                                                 GblContext*       pCtx)              GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblArrayHeap_acquire     (GBL_SELF,
                                                 void*   pData,
                                                 size_t  size,
                                                 size_t  capacity)                    GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblArrayHeap_release     (GBL_SELF,
                                                 void**   ppData,
                                                 size_t * pSize,
                                                 size_t * pCapacity)                  GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblArrayHeap_destruct    (GBL_SELF)                            GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblArrayHeap_copy        (GBL_SELF, const GBL_SELF_TYPE* pRhs) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblArrayHeap_move        (GBL_SELF, GBL_SELF_TYPE* pRhs)       GBL_NOEXCEPT;

GBL_EXPORT GblContext* GblArrayHeap_context     (GBL_CSELF)                           GBL_NOEXCEPT;
GBL_EXPORT size_t      GblArrayHeap_elementSize (GBL_CSELF)                           GBL_NOEXCEPT;
GBL_EXPORT size_t      GblArrayHeap_size        (GBL_CSELF)                           GBL_NOEXCEPT;
GBL_EXPORT size_t      GblArrayHeap_capacity    (GBL_CSELF)                           GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblArrayHeap_empty       (GBL_CSELF)                           GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblArrayHeap_stack       (GBL_CSELF)                           GBL_NOEXCEPT;
GBL_EXPORT void*       GblArrayHeap_data        (GBL_CSELF)                           GBL_NOEXCEPT;

GBL_EXPORT void*       GblArrayHeap_peek        (GBL_CSELF)                           GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblArrayHeap_pop         (GBL_SELF, void* pEntryOut)           GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblArrayHeap_push        (GBL_SELF, const void* pEntry)        GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblArrayHeap_clear       (GBL_SELF)                            GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblArrayHeap_reserve     (GBL_SELF, size_t  capacity)          GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblArrayHeap_shrinkToFit (GBL_SELF)                            GBL_NOEXCEPT;

GBL_DECLS_END

//! \cond
#define GblArrayHeap_construct(...) \
    GblArrayHeap_constructDefault_(__VA_ARGS__)
#define GblArrayHeap_constructDefault_(...) \
    GblArrayHeap_constructDefault__(__VA_ARGS__, sizeof(GblArrayHeap), GBL_NULL)
#define GblArrayHeap_constructDefault__(self, elemSize, cmp, structSize, ctx, ...) \
    (GblArrayHeap_construct)(self, elemSize, cmp, structSize, ctx)
//! \endcond

#undef GBL_SELF_TYPE

#endif // GIMBAL_ARRAY_HEAP_H

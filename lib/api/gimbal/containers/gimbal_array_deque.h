/*! \file
 *  \brief GblArrayDeque container and related functions
 *  \ingroup containers
 *  \copydoc GblArrayDeque
 *
 *  \todo
 *    - GblArrayDeque_erase()
 *
 *  \author Falco Girgis
 */

#ifndef GIMBAL_ARRAY_DEQUE_H
#define GIMBAL_ARRAY_DEQUE_H

#include "gimbal_ring_buffer.h"

#ifndef GBL_ARRAY_DEQUE_FORCE_POW2
#   define GBL_ARRAY_DEQUE_FORCE_POW2 1
#endif

#define GBL_SELF_TYPE GblArrayDeque

GBL_DECLS_BEGIN

/*! \brief Circular array-based double-ended queue container
 *
 *  GblArrayDeque is a very flexible container type which
 *  combines the fast random access and cache coherency
 *  of a contiguous array with the semantics of a double-ended
 *  queue. This means that not only are random
 *  accesses extremely efficient, but also adding and
 *  removing entries from either end as well.
 *
 *  This container is well-suited for data sets which can
 *  grow or shrink from either end, but which are typically
 *  accessed as an array.
 *
 *  Operation                         |Time Complexity
 *  ----------------------------------|------------------
 *  iteration                         | O(N)
 *  insertion (back or front)         | O(1)
 *  insertion (middle)                | O(N)
 *  removal (back or front)           | O(1)
 *  removal (middle) (WIP)            | O(N)
 *  access (front or back)            | O(1)
 *  random access (middle)            | O(1)
 *
 *  For reference, the following are the results of the
 *  profiler unit tests for some common operations shared
 *  between GblArrayList (comparable to std::vector) and
 *  GblArrayDeque (1024 pointer-sized entries):
 *
 *  Test      |GblArrayList|GblArrayDeque
 *  ----------|------------|-------------
 *  at        |0.004 ms    |0.008 ms
 *  pushBack  |0.030 ms    |0.030 ms
 *  pushFront |0.116 ms    |0.027 ms
 *
 *  \note
 *  This container can be seen as a slightly more specific,
 *  more optimized version of C++'s std::deque. A std::deque
 *  also allows for efficient insertion and removal to and
 *  from the middle of the array; however, this flexibility
 *  comes at the price of increased indirection overhead
 *  for random access and increased memory utilization.
 *  While GblArrayDeque does support random insertions and
 *  deletions, these are more costly than doing so from the
 *  front or back, as they require the shuffling around of
 *  surrounding elements in memory. We have decided that
 *  this trade-off is worth it, since we still support
 *  the operation, but fast lookups and lower memory
 *  utilization are much higher priorities.
 *  \note
 *  Internally this structure is implemented as an extension
 *  of the GblRingBuffer container, which is able to push
 *  from the front, pop from the back, and dynamically resize
 *  itself as it reaches its capacity.
 *  \note
 *  When the GBL_ARRAY_DEQUE_FORCE_POW2 macro is defined as 1
 *  (default) behavior), the capacity of the deque will always
 *  be rounded to the next power-of-two. This means any direct
 *  requests to GblArrayDeque_reserve() or
 *  GblArrayDeque_shrinkToFit() will not resize the container
 *  to the exact value specified. As a consequence of this,
 *  lookup times can be optimized by another 15-30% by reducing
 *  a modulo operation to a decrement + bitmask.
 *
 *  \sa GblRingBuffer, GblArrayList
 *  \ingroup containers
 */
typedef struct GblArrayDeque {      // Size (32/64-bit)
    GBL_PRIVATE()
        GblRingBuffer ringBuffer;
    GBL_PRIVATE_END                 // 22/42    total
} GblArrayDeque;

// ===== Public methods =====
GBL_EXPORT GBL_RESULT  GblArrayDeque_construct    (GBL_SELF,
                                                   uint16_t    elementSize,
                                                   size_t      capacity,
                                                   size_t      initialSize,
                                                   const void* pInitialData,
                                                   GblContext* pCtx)                                        GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT  GblArrayDeque_destruct     (GBL_SELF)                                                GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT  GblArrayDeque_copy         (GBL_SELF, const GblArrayDeque* pOther)                   GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT  GblArrayDeque_move         (GBL_SELF, GblArrayDeque* pOther)                         GBL_NOEXCEPT;

GBL_INLINE GblContext* GblArrayDeque_context      (GBL_CSELF)                                               GBL_NOEXCEPT;
GBL_INLINE size_t      GblArrayDeque_capacity     (GBL_CSELF)                                               GBL_NOEXCEPT;
GBL_INLINE size_t      GblArrayDeque_size         (GBL_CSELF)                                               GBL_NOEXCEPT;
GBL_INLINE size_t      GblArrayDeque_elementSize  (GBL_CSELF)                                               GBL_NOEXCEPT;

GBL_INLINE GblBool     GblArrayDeque_empty        (GBL_CSELF)                                               GBL_NOEXCEPT;
GBL_INLINE GblBool     GblArrayDeque_full         (GBL_CSELF)                                               GBL_NOEXCEPT;

GBL_INLINE void*       GblArrayDeque_at           (GBL_CSELF, size_t index)                                 GBL_NOEXCEPT;
GBL_INLINE void*       GblArrayDeque_front        (GBL_CSELF)                                               GBL_NOEXCEPT;
GBL_INLINE void*       GblArrayDeque_back         (GBL_CSELF)                                               GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT  GblArrayDeque_pushBack     (GBL_SELF, const void* pData)                             GBL_NOEXCEPT;
GBL_EXPORT void*       GblArrayDeque_emplaceBack  (GBL_SELF)                                                GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblArrayDeque_pushFront    (GBL_SELF, const void* pData)                             GBL_NOEXCEPT;
GBL_EXPORT void*       GblArrayDeque_emplaceFront (GBL_SELF)                                                GBL_NOEXCEPT;

GBL_EXPORT void*       GblArrayDeque_popBack      (GBL_SELF)                                                GBL_NOEXCEPT;
GBL_INLINE void*       GblArrayDeque_popFront     (GBL_SELF)                                                GBL_NOEXCEPT;

GBL_EXPORT void*       GblArrayDeque_insert       (GBL_SELF, size_t pos, const void* pData, size_t count)   GBL_NOEXCEPT;
GBL_EXPORT void*       GblArrayDeque_emplace      (GBL_SELF, size_t pos)                                    GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblArrayDeque_erase        (GBL_SELF, size_t pos, size_t count )                     GBL_NOEXCEPT;

GBL_EXPORT void        GblArrayDeque_clear        (GBL_SELF)                                                GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblArrayDeque_reserve      (GBL_SELF, size_t capacity)                               GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblArrayDeque_resize       (GBL_SELF, size_t size)                                   GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblArrayDeque_shrinkToFit  (GBL_SELF)                                                GBL_NOEXCEPT;

// ===== Public macros =====
#define GblArrayDeque_construct(...)              GblArrayDeque_constructDefault_(__VA_ARGS__)
#define GblArrayDeque_insert(...)                 GblArrayDeque_insertDefault_(__VA_ARGS__)
#define GblArrayDeque_erase(...)                  GblArrayDeque_eraseDefault_(__VA_ARGS__)

// ===== IMPL =====
///\cond
#define GBL_RING_SELF_(dequeSelf)      (&GBL_PRIV_REF(dequeSelf).ringBuffer)
#define GBL_RING_PRIV_REF_(dequeSelf)  (GBL_PRIV_REF(GBL_RING_SELF_(dequeSelf)))

#define GblArrayDeque_constructDefault_(...) \
    GblArrayDeque_constructDefault__(__VA_ARGS__, 0, 0, GBL_NULL, GBL_NULL)
#define GblArrayDeque_constructDefault__(self, elem, cap, size, data, ctx, ...) \
    (GblArrayDeque_construct)(self, elem, cap, size, data, ctx)

#define GblArrayDeque_insertDefault_(...) \
    GblArrayDeque_insertDefault__(__VA_ARGS__, 1)
#define GblArrayDeque_insertDefault__(self, pos, data, count, ...) \
    (GblArrayDeque_insert(self, pos, data, count))

#define GblArrayDeque_eraseDefault_(...) \
    GblArrayDeque_eraseDefault__(__VA_ARGS__, 1)
#define GblArrayDeque_eraseDefault__(self, pos, count, ...) \
    (GblArrayDeque_eraseDefault(self, pos, count))
///\endcond

GBL_INLINE GBL_RESULT GblArrayDeque_destruct(GBL_SELF) GBL_NOEXCEPT {
    return GblRingBuffer_destruct(GBL_RING_SELF_(pSelf));
}

GBL_INLINE GBL_RESULT GblArrayDeque_copy(GBL_SELF, const GblArrayDeque* pOther) GBL_NOEXCEPT {
    return GblRingBuffer_copy(GBL_RING_SELF_(pSelf), GBL_RING_SELF_(pOther));
}

GBL_INLINE GBL_RESULT GblArrayDeque_move(GBL_SELF, GblArrayDeque* pOther) GBL_NOEXCEPT {
    return GblRingBuffer_move(GBL_RING_SELF_(pSelf), GBL_RING_SELF_(pOther));
}

GBL_INLINE GblContext* GblArrayDeque_context(GBL_CSELF) GBL_NOEXCEPT {
    return GblRingBuffer_context(GBL_RING_SELF_(pSelf));
}

GBL_INLINE size_t  GblArrayDeque_capacity(GBL_CSELF) GBL_NOEXCEPT {
    return GblRingBuffer_capacity(GBL_RING_SELF_(pSelf));
}

GBL_INLINE size_t  GblArrayDeque_size(GBL_CSELF) GBL_NOEXCEPT {
    return GblRingBuffer_size(GBL_RING_SELF_(pSelf));
}

GBL_INLINE size_t  GblArrayDeque_elementSize(GBL_CSELF) GBL_NOEXCEPT {
    return GblRingBuffer_elementSize(GBL_RING_SELF_(pSelf));
}

GBL_INLINE GblBool GblArrayDeque_empty(GBL_CSELF) GBL_NOEXCEPT {
    return GblRingBuffer_empty(GBL_RING_SELF_(pSelf));
}

GBL_INLINE GblBool GblArrayDeque_full(GBL_CSELF) GBL_NOEXCEPT {
    return GblRingBuffer_full(GBL_RING_SELF_(pSelf));
}

GBL_INLINE void* GblArrayDeque_at(GBL_CSELF, size_t  index) GBL_NOEXCEPT {
    void* pData = GBL_NULL;

    if(index >= GBL_RING_PRIV_REF_(pSelf).size) GBL_UNLIKELY {
        GBL_CTX_BEGIN(GBL_RING_PRIV_REF_(pSelf).pCtx);
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_OUT_OF_RANGE);
        GBL_CTX_END_BLOCK();
    } else GBL_LIKELY {
#if GBL_ARRAY_DEQUE_FORCE_POW2 == 1
        const size_t  slot = (GBL_RING_PRIV_REF_(pSelf).frontPos + index)
                           & (GBL_RING_PRIV_REF_(pSelf).capacity-1);
#else
        const size_t  slot = (GBL_RING_PRIV_REF_(pSelf).frontPos + index)
                           % (GBL_RING_PRIV_REF_(pSelf).capacity);
#endif
        pData = &GBL_RING_PRIV_REF_(pSelf).pData[slot * GBL_RING_PRIV_REF_(pSelf).elementSize];
    }

    return pData;
}

GBL_INLINE void* GblArrayDeque_front(GBL_CSELF) GBL_NOEXCEPT {
    return GblRingBuffer_front(GBL_RING_SELF_(pSelf));
}

GBL_INLINE void* GblArrayDeque_back(GBL_CSELF) GBL_NOEXCEPT {
    return GblRingBuffer_back(GBL_RING_SELF_(pSelf));
}

GBL_INLINE GBL_RESULT GblArrayDeque_pushBack(GBL_SELF, const void* pData) GBL_NOEXCEPT {
    void* pSlot = GblArrayDeque_emplaceBack(pSelf);
    if(!pSlot) return GblThd_callRecord(GBL_NULL)->result;
    else {
        GBL_ASSERT(pData);
        memcpy(pSlot, pData, GblArrayDeque_elementSize(pSelf));
    }
    return GBL_RESULT_SUCCESS;
}

GBL_INLINE void* GblArrayDeque_popFront(GBL_SELF) GBL_NOEXCEPT {
    return GblRingBuffer_popFront(GBL_RING_SELF_(pSelf));
}

#undef GBL_RING_SELF_
#undef GBL_RING_PRIV_REF_

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_ARRAY_DEQUE_H

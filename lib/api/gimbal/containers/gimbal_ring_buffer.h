/*! \file
 *  \brief GblRingBuffer container and related functions
 *  \ingroup containers
 *  \copydoc GblRingBuffer
 */

#ifndef GIMBAL_RING_BUFFER_H
#define GIMBAL_RING_BUFFER_H

#include "../core/gimbal_typedefs.h"
#include "../core/gimbal_ctx.h"
#include <stdlib.h>

#define GBL_SELF_TYPE GblRingBuffer

GBL_DECLS_BEGIN

/*! \brief Contiguous, array-based circular/ring buffer with queue semantics
 *
 *  GblRingBuffer is a generic container of fixed capacity,
 *  to which elements may be enqueued or dequeued without requiring any
 *  shuffling around in memory. Rather than resizing when the capacity is
 *  reached, any new values simply overrwrite the oldest values.
 *  Insertions and removals are performed semantically like a queue:
 *  pushing to the back and popping from the front; however, since the
 *  container is implemented as a contiguous array, it is also randomly
 *  accessible.
 *
 *  Operation                         |Time Complexity
 *  ----------------------------------|------------------
 *  iteration                         | O(N)
 *  insertion (back)                  | O(1)
 *  removal (front)                   | O(1)
 *  access (front or back)            | O(1)
 *  random access (middle)            | O(1)
 *
 *  These properties make this structure well-suited for FIFO or queue
 *  style structures of fixed sizes as well as for representing stream
 *  buffers in memory, where a producer is pushing new entries while a
 *  consumer pops them.
 *
 *  \ingroup containers
 *  \sa GblDeque, GblArrayList
 */
typedef struct GblRingBuffer {      // Size (32/64-bit)
    GBL_PRIVATE()
        GblContext* pCtx;           // 4/8      bytes
        uint8_t*    pData;          // 4/8      bytes
        GblSize     size;           // 4/8      bytes
        GblSize     capacity;       // 4/8      bytes
        GblSize     frontPos;       // 4/8      bytes
        uint16_t    elementSize;    // 2        bytes
    GBL_PRIVATE_END
} GblRingBuffer;                    // 22/42    total

GBL_EXPORT GBL_RESULT  GblRingBuffer_construct_6(GBL_SELF,
                                                 uint16_t    elementSize,
                                                 GblSize     capacity,
                                                 GblSize     initialSize,
                                                 const void* pInitialData,
                                                 GblContext* pCtx)                            GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT  GblRingBuffer_construct_5(GBL_SELF,
                                                 uint16_t    elementSize,
                                                 GblSize     capacity,
                                                 GblSize     initialSize,
                                                 const void* pInitialData)                    GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT  GblRingBuffer_construct_4(GBL_SELF,
                                                 uint16_t   elementSize,
                                                 GblSize    capacity,
                                                 GblSize    initialSize)                      GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT  GblRingBuffer_construct_3(GBL_SELF,
                                                 uint16_t   elementSize,
                                                 GblSize    capacity)                         GBL_NOEXCEPT;

#define                GblRingBuffer_construct(...)  GBL_VA_OVERLOAD_CALL(GblRingBuffer_construct,       \
                                                                          GBL_VA_OVERLOAD_SUFFIXER_ARGC, \
                                                                          __VA_ARGS__)

GBL_EXPORT GBL_RESULT  GblRingBuffer_destruct    (GBL_SELF)                                   GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblRingBuffer_copy        (GBL_SELF, const GblRingBuffer* pOther)      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblRingBuffer_move        (GBL_SELF, GblRingBuffer* pOther)            GBL_NOEXCEPT;

GBL_INLINE GblContext* GblRingBuffer_context     (GBL_CSELF)                                  GBL_NOEXCEPT;
GBL_INLINE GblSize     GblRingBuffer_capacity    (GBL_CSELF)                                  GBL_NOEXCEPT;
GBL_INLINE GblSize     GblRingBuffer_size        (GBL_CSELF)                                  GBL_NOEXCEPT;
GBL_INLINE GblSize     GblRingBuffer_elementSize (GBL_CSELF)                                  GBL_NOEXCEPT;

GBL_INLINE GblBool     GblRingBuffer_empty       (GBL_CSELF)                                  GBL_NOEXCEPT;
GBL_INLINE GblBool     GblRingBuffer_full        (GBL_CSELF)                                  GBL_NOEXCEPT;

GBL_INLINE void*       GblRingBuffer_at          (GBL_CSELF, GblSize index)                   GBL_NOEXCEPT;
GBL_INLINE void*       GblRingBuffer_front       (GBL_CSELF)                                  GBL_NOEXCEPT;
GBL_INLINE void*       GblRingBuffer_back        (GBL_CSELF)                                  GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT  GblRingBuffer_pushBack    (GBL_SELF, const void* pData)                GBL_NOEXCEPT;
GBL_INLINE void*       GblRingBuffer_emplaceBack (GBL_SELF)                                   GBL_NOEXCEPT;

GBL_INLINE void*       GblRingBuffer_popFront    (GBL_SELF)                                   GBL_NOEXCEPT;
GBL_INLINE void        GblRingBuffer_clear       (GBL_SELF)                                   GBL_NOEXCEPT;

// ===== IMPL =====

/// \cond

GBL_INLINE GblSize GblRingBuffer_mappedIndex_(GBL_CSELF, GblSize index) {
    return (GBL_PRIV_REF(pSelf).frontPos + index) % GBL_PRIV_REF(pSelf).capacity;
}

GBL_INLINE void GblRingBuffer_advance_(GBL_SELF) GBL_NOEXCEPT {
    if(GblRingBuffer_full(pSelf)) {
        ++GBL_PRIV_REF(pSelf).frontPos;
        GBL_PRIV_REF(pSelf).frontPos %= GBL_PRIV_REF(pSelf).capacity;
    } else {
        ++GBL_PRIV_REF(pSelf).size;
    }
}
/// \endcond

GBL_INLINE GBL_RESULT GblRingBuffer_construct_5(GBL_SELF,
                                                uint16_t    elementSize,
                                                GblSize     capacity,
                                                GblSize     initialSize,
                                                const void* pInitialData) GBL_NOEXCEPT
{
    return GblRingBuffer_construct_6(pSelf, elementSize, capacity, initialSize, pInitialData, GBL_NULL);
}

GBL_INLINE GBL_RESULT GblRingBuffer_construct_4(GBL_SELF,
                                                uint16_t   elementSize,
                                                GblSize    capacity,
                                                GblSize    initialSize) GBL_NOEXCEPT
{
    return GblRingBuffer_construct_5(pSelf, elementSize, capacity, initialSize, GBL_NULL);
}

GBL_INLINE GBL_RESULT GblRingBuffer_construct_3(GBL_SELF,
                                                uint16_t   elementSize,
                                                GblSize    capacity)        GBL_NOEXCEPT
{
    return GblRingBuffer_construct_4(pSelf, elementSize, capacity, 0);
}

GBL_INLINE GblContext* GblRingBuffer_context(GBL_CSELF) GBL_NOEXCEPT {
    return GBL_PRIV_REF(pSelf).pCtx;
}

GBL_INLINE GblSize GblRingBuffer_capacity(GBL_CSELF) GBL_NOEXCEPT {
    return GBL_PRIV_REF(pSelf).capacity;
}

GBL_INLINE GblSize GblRingBuffer_size(GBL_CSELF) GBL_NOEXCEPT {
    return GBL_PRIV_REF(pSelf).size;
}

GBL_INLINE GblSize GblRingBuffer_elementSize(GBL_CSELF) GBL_NOEXCEPT {
    return GBL_PRIV_REF(pSelf).elementSize;
}

GBL_INLINE GblBool GblRingBuffer_empty(GBL_CSELF) GBL_NOEXCEPT {
    return GblRingBuffer_size(pSelf) == 0? GBL_TRUE : GBL_FALSE;
}

GBL_INLINE GblBool GblRingBuffer_full(GBL_CSELF) GBL_NOEXCEPT {
    return GblRingBuffer_size(pSelf) == GBL_PRIV_REF(pSelf).capacity? GBL_TRUE : GBL_FALSE;
}

GBL_INLINE void* GblRingBuffer_at(GBL_CSELF, GblSize index) GBL_NOEXCEPT {
    const GblSize size = GblRingBuffer_size(pSelf);
    void* pData = GBL_NULL;

    if(index >= size) GBL_UNLIKELY {
        GBL_CTX_BEGIN(GBL_PRIV_REF(pSelf).pCtx);
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_OUT_OF_RANGE);
        GBL_CTX_END_BLOCK();
    } else GBL_LIKELY {
        pData = &GBL_PRIV_REF(pSelf).pData[GblRingBuffer_mappedIndex_(pSelf, index) *
                                           GBL_PRIV_REF(pSelf).elementSize];
    }

    return pData;
}

GBL_INLINE void* GblRingBuffer_front(GBL_CSELF) GBL_NOEXCEPT {
    return GblRingBuffer_at(pSelf, 0);
}

GBL_INLINE void* GblRingBuffer_back(GBL_CSELF) GBL_NOEXCEPT {
    return GblRingBuffer_at(pSelf, GBL_PRIV_REF(pSelf).size-1);
}


GBL_INLINE void* GblRingBuffer_emplaceBack(GBL_SELF) GBL_NOEXCEPT {
    GblRingBuffer_advance_(pSelf);
    return GblRingBuffer_back(pSelf);
}

GBL_INLINE GBL_RESULT GblRingBuffer_pushBack(GBL_SELF, const void* pData) GBL_NOEXCEPT {
    GBL_RESULT result = GBL_RESULT_SUCCESS;
    void* pBuffer = GblRingBuffer_emplaceBack(pSelf);
    if(pBuffer) GBL_LIKELY memcpy(pBuffer, pData, GBL_PRIV_REF(pSelf).elementSize);
    else GBL_UNLIKELY result = GblThread_callRecord(GBL_NULL)->result;
    return result;
}

GBL_INLINE void* GblRingBuffer_popFront(GBL_SELF) GBL_NOEXCEPT {
    void* pFront = GblRingBuffer_front(pSelf);
    if(pFront) {
        --GBL_PRIV_REF(pSelf).size;
        ++GBL_PRIV_REF(pSelf).frontPos;
        if(GBL_PRIV_REF(pSelf).frontPos >= GBL_PRIV_REF(pSelf).capacity)
            GBL_PRIV_REF(pSelf).frontPos = 0;
    } else {
        GBL_CTX_BEGIN(GBL_PRIV_REF(pSelf).pCtx);
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_OUT_OF_RANGE);
        GBL_CTX_END_BLOCK();
    }
    return pFront;
}

GBL_INLINE void GblRingBuffer_clear(GBL_SELF) GBL_NOEXCEPT {
    GBL_PRIV_REF(pSelf).frontPos = 0;
    GBL_PRIV_REF(pSelf).size     = 0;
}

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_RING_BUFFER_H

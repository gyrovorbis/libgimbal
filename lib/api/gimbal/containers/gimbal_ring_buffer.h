/*! \file
 *  \brief GblRingBuffer container and related functions
 *  \ingroup containers
 *  \copydoc GblRingBuffer
 *
 *  \todo
 *      - since nothing is public, make it an opaque struct
 *        with its buffer appended to the end of the same
 *        allocation
 *
 *  \author Falco Girgis
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
    GBL_PRIVATE_BEGIN
        GblContext* pCtx;           // 4/8      bytes
        uint8_t*    pData;          // 4/8      bytes
        size_t      size;           // 4/8      bytes
        size_t      capacity;       // 4/8      bytes
        size_t      frontPos;       // 4/8      bytes
        uint16_t    elementSize;    // 2        bytes
    GBL_PRIVATE_END
} GblRingBuffer;                    // 22/42    total

GBL_EXPORT GBL_RESULT  GblRingBuffer_construct_6(GBL_SELF,
                                                 uint16_t    elementSize,
                                                 size_t      capacity,
                                                 size_t      initialSize,
                                                 const void* pInitialData,
                                                 GblContext* pCtx)                            GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblRingBuffer_construct_5(GBL_SELF,
                                                 uint16_t    elementSize,
                                                 size_t      capacity,
                                                 size_t      initialSize,
                                                 const void* pInitialData)                    GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblRingBuffer_construct_4(GBL_SELF,
                                                 uint16_t   elementSize,
                                                 size_t     capacity,
                                                 size_t     initialSize)                      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblRingBuffer_construct_3(GBL_SELF,
                                                 uint16_t   elementSize,
                                                 size_t     capacity)                         GBL_NOEXCEPT;

#define                GblRingBuffer_construct(...)  GBL_VA_OVERLOAD_CALL(GblRingBuffer_construct,       \
                                                                          GBL_VA_OVERLOAD_SUFFIXER_ARGC, \
                                                                          __VA_ARGS__)

GBL_EXPORT GBL_RESULT  GblRingBuffer_destruct    (GBL_SELF)                                   GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblRingBuffer_copy        (GBL_SELF, const GblRingBuffer* pOther)      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblRingBuffer_move        (GBL_SELF, GblRingBuffer* pOther)            GBL_NOEXCEPT;

GBL_EXPORT GblContext* GblRingBuffer_context     (GBL_CSELF)                                  GBL_NOEXCEPT;
GBL_EXPORT size_t      GblRingBuffer_capacity    (GBL_CSELF)                                  GBL_NOEXCEPT;
GBL_EXPORT size_t      GblRingBuffer_size        (GBL_CSELF)                                  GBL_NOEXCEPT;
GBL_EXPORT size_t      GblRingBuffer_elementSize (GBL_CSELF)                                  GBL_NOEXCEPT;

GBL_EXPORT GblBool     GblRingBuffer_empty       (GBL_CSELF)                                  GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblRingBuffer_full        (GBL_CSELF)                                  GBL_NOEXCEPT;

GBL_EXPORT void*       GblRingBuffer_at          (GBL_CSELF, size_t  index)                   GBL_NOEXCEPT;
GBL_EXPORT void*       GblRingBuffer_front       (GBL_CSELF)                                  GBL_NOEXCEPT;
GBL_EXPORT void*       GblRingBuffer_back        (GBL_CSELF)                                  GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblRingBuffer_pushBack    (GBL_SELF, const void* pData)                GBL_NOEXCEPT;
GBL_EXPORT void*       GblRingBuffer_emplaceBack (GBL_SELF)                                   GBL_NOEXCEPT;

GBL_EXPORT void*       GblRingBuffer_popFront    (GBL_SELF)                                   GBL_NOEXCEPT;
GBL_EXPORT void        GblRingBuffer_clear       (GBL_SELF)                                   GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_RING_BUFFER_H

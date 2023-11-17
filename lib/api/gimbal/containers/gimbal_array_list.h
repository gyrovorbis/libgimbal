/*! \file
 *  \brief GblArrayList structure and related functions
 *  \ingroup containers
 *
 *  \author Falco Girgis
 */

#ifndef GIMBAL_ARRAY_LIST_H
#define GIMBAL_ARRAY_LIST_H

#include "../core/gimbal_ctx.h"

#define GBL_SELF_TYPE GblArrayList

GBL_DECLS_BEGIN

/*! \brief Contiguous, array-based abstract list container with C++-style STL API
 *
 *  GblArrayList is a C++ STL-style std::vector replacement as
 *  a general-purpose dynamic array with a few extra features:
 *
 *  - Custom allocator support
 *  - Optional additional stack-based storage before overflowing to the heap
 *  - Optional zero termination
 *
 *  \ingroup containers
 *  \sa GblRingBuffer, GblArrayDeque
 */
typedef struct GblArrayList {               // Size (32-bit / 64-bit)
    GBL_PRIVATE_BEGIN
        GblContext*     pCtx;               // 4/8      bytes
        uint8_t*        pData;              // 4/8      bytes
        size_t          size;               // 4/8      bytes
        size_t          capacity;           // 4/8      bytes
        uint16_t        elementSize;        // 2        bytes
        uint16_t        zeroTerminated: 1;
        uint16_t        stackCapacity:  15; // 2        bytes
    GBL_PRIVATE_END
} GblArrayList;                             // 20/36    bytes

GBL_EXPORT GBL_RESULT  GblArrayList_construct_7  (GBL_SELF,
                                                  uint16_t     elementSize,
                                                  size_t       elementCount,
                                                  const void*  pInitialData,
                                                  size_t       structSize,
                                                  GblBool      zeroTerminated,
                                                  GblContext*  pCtx)                                           GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblArrayList_construct_6  (GBL_SELF,
                                                  uint16_t     elementSize,
                                                  size_t       elementCount,
                                                  const void*  pInitialData,
                                                  size_t       structSize,
                                                  GblBool      zeroTerminated)                                 GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblArrayList_construct_5  (GBL_SELF,
                                                  uint16_t     elementSize,
                                                  size_t       elementCount,
                                                  const void*  pInitialData,
                                                  size_t       structSize)                                     GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblArrayList_construct_4  (GBL_SELF,
                                                  uint16_t     elementSize,
                                                  size_t       elementCount,
                                                  const void*  pInitialData)                                   GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblArrayList_construct_3  (GBL_SELF,
                                                  uint16_t     elementSize,
                                                  size_t       elementCount)                                   GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblArrayList_construct_2  (GBL_SELF,
                                                  uint16_t     elementSize)                                    GBL_NOEXCEPT;
#define                GblArrayList_construct(...) \
                           GBL_VA_OVERLOAD_CALL_ARGC(GblArrayList_construct, __VA_ARGS__)
#define                GblArrayList_alloca(...) \
                            GBL_VA_OVERLOAD_CALL_ARGC(GBL_ARRAY_LIST_ALLOCA, __VA_ARGS__)

GBL_EXPORT GBL_RESULT  GblArrayList_destruct     (GBL_SELF)                                                    GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblArrayList_assign       (GBL_SELF, const void* pData, size_t  elementCount)           GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblArrayList_release      (GBL_SELF, void** ppData, size_t * pSize, size_t * pCapacity) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblArrayList_acquire      (GBL_SELF, void* pData, size_t  size, size_t  capacity)       GBL_NOEXCEPT;
//GBL_EXPORT GBL_RESULT GblArrayList_swap(GBL_SELF, GblArrayList* pOther) GBL_NOEXCEPT;

GBL_EXPORT GblContext* GblArrayList_context      (GBL_CSELF)                                                   GBL_NOEXCEPT;
GBL_EXPORT uint16_t    GblArrayList_stackBytes   (GBL_CSELF)                                                   GBL_NOEXCEPT;
GBL_EXPORT void*       GblArrayList_stackBuffer  (GBL_CSELF)                                                   GBL_NOEXCEPT;
GBL_EXPORT size_t      GblArrayList_size         (GBL_CSELF)                                                   GBL_NOEXCEPT;
GBL_EXPORT size_t      GblArrayList_capacity     (GBL_CSELF)                                                   GBL_NOEXCEPT;
GBL_EXPORT uint16_t    GblArrayList_elementSize  (GBL_CSELF)                                                   GBL_NOEXCEPT;
GBL_EXPORT void*       GblArrayList_data         (GBL_CSELF)                                                   GBL_NOEXCEPT;

GBL_EXPORT GblBool     GblArrayList_empty        (GBL_CSELF)                                                   GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblArrayList_stack        (GBL_CSELF)                                                   GBL_NOEXCEPT;

GBL_EXPORT void*       GblArrayList_at           (GBL_CSELF, size_t  index)                                    GBL_NOEXCEPT;
GBL_EXPORT void*       GblArrayList_front        (GBL_CSELF)                                                   GBL_NOEXCEPT;
GBL_EXPORT void*       GblArrayList_back         (GBL_CSELF)                                                   GBL_NOEXCEPT;

GBL_EXPORT void*       GblArrayList_insert       (GBL_SELF, size_t  index, size_t  count, const void* pData)   GBL_NOEXCEPT;
GBL_EXPORT void*       GblArrayList_emplace      (GBL_SELF, size_t  index)                                     GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblArrayList_pushFront    (GBL_SELF, const void* pData)                                 GBL_NOEXCEPT;
GBL_EXPORT void*       GblArrayList_emplaceFront (GBL_SELF)                                                    GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblArrayList_pushBack     (GBL_SELF, const void* pData)                                 GBL_NOEXCEPT;
GBL_EXPORT void*       GblArrayList_emplaceBack  (GBL_SELF)                                                    GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblArrayList_append       (GBL_SELF, const void* pData, size_t  elementCount)           GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblArrayList_prepend      (GBL_SELF, const void* pData, size_t  elementCount)           GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblArrayList_popFront     (GBL_SELF, void* pOut)                                        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblArrayList_popBack      (GBL_SELF, void* pOut)                                        GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblArrayList_erase        (GBL_SELF, size_t  begin, size_t  count)                      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblArrayList_clear        (GBL_SELF)                                                    GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblArrayList_reserve      (GBL_SELF, size_t  capacity)                                  GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblArrayList_resize       (GBL_SELF, size_t  size)                                      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblArrayList_shrinkToFit  (GBL_SELF)                                                    GBL_NOEXCEPT;

// ========== IMPL ==========

/// \cond
#define GBL_ARRAY_LIST_ALLOCA_6(elemSize, elemCount, pData, size, zeroTerminated, pCtx) \
    GblArrayList_createInPlace_(GBL_ALLOCA(size), elemSize, elemCount, pData, size, zeroTerminated, pCtx)

#define GBL_ARRAY_LIST_ALLOCA_5(elemSize, elemCount, pData, size, zeroTerminated) \
    GBL_ARRAY_LIST_ALLOCA_6(elemSize, elemCount, pData, size, zeroterminated, NULL)

#define GBL_ARRAY_LIST_ALLOCA_4(elemSize, elemCount, pData, size) \
    GBL_ARRAY_LIST_ALLOCA_5(elemSize, elemCount, pData, size, GBL_FALSE)

#define GBL_ARRAY_LIST_ALLOCA_3(elemSize, elemCount, pData) \
    GBL_ARRAY_LIST_ALLOCA_4(elemSize, elemCount, pData, 0)

#define GBL_ARRAY_LIST_ALLOCA_2(elemSize, elemCount) \
    GBL_ARRAY_LIST_ALLOCA_3(elemSize, elemCount, NULL)

#define GBL_ARRAY_LIST_ALLOCA_1(elemSize) \
    GBL_ARRAY_LIST_ALLOCA_2(elemSize, 0)
/// \endcond

#undef GBL_SELF_TYPE

GBL_DECLS_END

#endif // GIMBAL_ARRAY_LIST_H

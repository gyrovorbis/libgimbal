/*! \file
 *  \brief GblVector structure and related functions
 *  \ingroup containers
 */

#ifndef GIMBAL_VECTOR_H
#define GIMBAL_VECTOR_H

#include "../core/gimbal_api_frame.h"
#include "../algorithms/gimbal_numeric.h"

#define SELF    GblVector* pSelf
#define CSELF   const SELF

GBL_DECLS_BEGIN

/*! \brief Contiguous array based abstract list container with C++-style STL API
 *  \ingroup containers
 */
typedef struct GblVector {          // Size (32-bit / 64-bit)
    GblContext*     pCtx;           // 4/8      bytes
    uint8_t*        pData;          // 4/8      bytes
    GblSize         size;           // 4/8      bytes
    GblSize         capacity;       // 4/8      bytes
    uint16_t        elementSize;    // 2        bytes
    uint16_t        stackSize;      // 2        bytes
} GblVector;                        // 20/36    bytes

GBL_EXPORT GBL_RESULT   GblVector_construct_6   (SELF,
                                                 uint16_t     elementSize,
                                                 GblSize      elementCount,
                                                 const void*  pInitialData,
                                                 GblSize      structSize,
                                                 GblContext*  pCtx)                                           GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVector_construct_5   (SELF,
                                                 uint16_t     elementSize,
                                                 GblSize      elementCount,
                                                 const void*  pInitialData,
                                                 GblSize      structSize)                                     GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVector_construct_4   (SELF,
                                                 uint16_t     elementSize,
                                                 GblSize      elementCount,
                                                 const void*  pInitialData)                                   GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVector_construct_3   (SELF,
                                                 uint16_t     elementSize,
                                                 GblSize      elementCount)                                   GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVector_construct_2   (SELF,
                                                 uint16_t     elementSize)                                    GBL_NOEXCEPT;
#define                 GblVector_construct(...) \
                            GBL_VA_OVERLOAD_SELECT(GblVector_construct, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)(__VA_ARGS__)
#define                 GblVector_alloca(...) \
                            GBL_VA_OVERLOAD_SELECT(GBL_VECTOR_ALLOCA, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)(__VA_ARGS__)

GBL_EXPORT GBL_RESULT   GblVector_destruct      (SELF)                                                        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblVector_assign        (SELF, const void* pData, GblSize elementCount)               GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblVector_release       (SELF, void** ppVecPtr, GblSize* pSize, GblSize* pCapacity)   GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblVector_acquire       (SELF, void* pData, GblSize size, GblSize capacity)           GBL_NOEXCEPT;

GBL_INLINE GblContext*  GblVector_context       (CSELF)                                                       GBL_NOEXCEPT;
GBL_INLINE uint16_t     GblVector_stackBytes    (CSELF)                                                       GBL_NOEXCEPT;
GBL_INLINE void*        GblVector_stackBuffer   (CSELF)                                                       GBL_NOEXCEPT;
GBL_INLINE GblSize      GblVector_size          (CSELF)                                                       GBL_NOEXCEPT;
GBL_INLINE GblSize      GblVector_capacity      (CSELF)                                                       GBL_NOEXCEPT;
GBL_INLINE uint16_t     GblVector_elementSize   (CSELF)                                                       GBL_NOEXCEPT;
GBL_INLINE void*        GblVector_data          (CSELF)                                                       GBL_NOEXCEPT;

GBL_INLINE GblBool      GblVector_empty         (CSELF)                                                       GBL_NOEXCEPT;
GBL_INLINE GblBool      GblVector_stack         (CSELF)                                                       GBL_NOEXCEPT;

GBL_INLINE void*        GblVector_at            (CSELF, GblSize index)                                        GBL_NOEXCEPT;
GBL_INLINE void*        GblVector_front         (CSELF)                                                       GBL_NOEXCEPT;
GBL_INLINE void*        GblVector_back          (CSELF)                                                       GBL_NOEXCEPT;

GBL_EXPORT void*        GblVector_insert        (SELF, GblSize index, GblSize count, const void* pData)       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVector_pushFront     (SELF, const void* pData)                                     GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVector_pushBack      (SELF, const void* pData)                                     GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblVector_append        (SELF, const void* pData, GblSize elementCount)               GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblVector_prepend       (SELF, const void* pData, GblSize elementCount)               GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblVector_popFront      (SELF, void* pOut)                                            GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblVector_popBack       (SELF, void* pOut)                                            GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblVector_erase         (SELF, GblSize begin, GblSize count)                          GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblVector_clear         (SELF)                                                        GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblVector_reserve       (SELF, GblSize capacity)                                      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblVector_resize        (SELF, GblSize size)                                          GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblVector_shrinkToFit   (SELF)                                                        GBL_NOEXCEPT;

// ========== IMPL ==========

/// \cond
#define GBL_VECTOR_ALLOCA_5(elemSize, elemCount, pData, size, pCtx) \
    GblVector_createInPlace_(GBL_ALLOCA(size), elemSize, elemCount, pData, size, pCtx)

#define GBL_VECTOR_ALLOCA_4(elemSize, elemCount, pData, size) \
    GBL_VECTOR_ALLOCA_5(elemSize, elemCount, pData, size, NULL)

#define GBL_VECTOR_ALLOCA_3(elemSize, elemCount, pData) \
    GBL_VECTOR_ALLOCA_4(elemSize, elemCount, pData, 0)

#define GBL_VECTOR_ALLOCA_2(elemSize, elemCount) \
    GBL_VECTOR_ALLOCA_3(elemSize, elemCount, NULL)

#define GBL_VECTOR_ALLOCA_1(elemSize) \
    GBL_VECTOR_ALLOCA_2(elemSize, 0)

GBL_INLINE GblSize GBL_VECTOR_CAPACITY_FROM_SIZE_(GblSize size) GBL_NOEXCEPT {
    return gblPow2Next(size);
}
/// \endcond

GBL_INLINE void* GblVector_stackBuffer(CSELF) GBL_NOEXCEPT {
    return (uint8_t*)((pSelf && pSelf->stackSize)? (pSelf + 1) : NULL);
}

GBL_INLINE GblBool GblVector_empty(CSELF) GBL_NOEXCEPT  {
    return (pSelf && pSelf->size)? GBL_FALSE : GBL_TRUE;
}

GBL_INLINE GblBool GblVector_stack(CSELF) GBL_NOEXCEPT {
    return (pSelf && (pSelf->pData == GblVector_stackBuffer(pSelf)))? GBL_TRUE : GBL_FALSE;
}

GBL_INLINE GBL_RESULT GblVector_construct_5(SELF, uint16_t elementSize, GblSize elementCount, const void* pInitialData, GblSize structSize) GBL_NOEXCEPT {
    return GblVector_construct_6(pSelf, elementSize, elementCount, pInitialData, structSize, NULL);
}

GBL_INLINE GBL_RESULT GblVector_construct_4(SELF, uint16_t elementSize, GblSize elementCount, const void* pInitialData) GBL_NOEXCEPT {
    return GblVector_construct_5(pSelf, elementSize, elementCount, pInitialData, sizeof(GblVector));
}

GBL_INLINE GBL_RESULT GblVector_construct_3(SELF, uint16_t elementSize, GblSize elementCount) GBL_NOEXCEPT {
    return GblVector_construct_4(pSelf, elementSize, elementCount, NULL);
}

GBL_INLINE GBL_RESULT GblVector_construct_2(SELF, uint16_t elementSize) GBL_NOEXCEPT {
    return GblVector_construct_3(pSelf, elementSize, 0);
}

GBL_INLINE GblContext* GblVector_context(CSELF) GBL_NOEXCEPT {
    return (pSelf)? pSelf->pCtx : NULL;
}

GBL_INLINE uint16_t GblVector_stackBytes(CSELF) GBL_NOEXCEPT {
    return (pSelf)? pSelf->stackSize : 0;
}

GBL_INLINE GblSize GblVector_size(CSELF) GBL_NOEXCEPT {
    return (pSelf)? pSelf->size : 0;
}

GBL_INLINE GblSize GblVector_capacity(CSELF) GBL_NOEXCEPT {
    return (pSelf)? pSelf->capacity : 0;
}

GBL_INLINE uint16_t GblVector_elementSize(CSELF) GBL_NOEXCEPT {
    return (pSelf)? pSelf->elementSize : 0;
}

GBL_INLINE void* GblVector_data(CSELF) GBL_NOEXCEPT {
    return (pSelf)? pSelf->pData : NULL;
}

GBL_INLINE void* GblVector_at(CSELF, GblSize index) GBL_NOEXCEPT {
    void* pData = NULL;
    GBL_API_BEGIN(pSelf->pCtx);
    GBL_API_VERIFY(index < pSelf->size,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    pData = &pSelf->pData[index * pSelf->elementSize];
    GBL_API_END_BLOCK();
    return pData;
}

GBL_INLINE void* GblVector_front(CSELF) GBL_NOEXCEPT {
    return GblVector_at(pSelf, 0);
}

GBL_INLINE void* GblVector_back(CSELF) GBL_NOEXCEPT {
    return GblVector_at(pSelf, pSelf->size-1);
}

GBL_INLINE GBL_RESULT GblVector_pushBack(SELF, const void* pData) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->pCtx);
    GblVector_insert(pSelf, GblVector_size(pSelf), 1, pData);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblVector_pushFront(SELF, const void* pData) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->pCtx);
    GblVector_insert(pSelf, 0, 1, pData);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END();
}

GBL_INLINE GblVector* GblVector_createInPlace (SELF,
                                               uint16_t    elementSize,
                                               GblSize     elementCount,
                                               const void* pInitialData,
                                               GblSize     structSize,
                                               GblContext* pCtx) GBL_NOEXCEPT
{
    GblVector* pOut = NULL;
    GBL_API_BEGIN(pCtx);
    GBL_API_VERIFY_CALL(GblVector_construct_6(pSelf, elementSize, elementCount, pInitialData, structSize, pCtx));
    pOut = pSelf;
    GBL_API_END_BLOCK();
    return pOut;
}

#undef CSELF
#undef SELF

GBL_DECLS_END

#endif // GIMBAL_VECTOR_H

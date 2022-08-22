/*! \file
 *  \brief GblArrayList structure and related functions
 *  \ingroup containers
 */

#ifndef GIMBAL_ARRAY_LIST_H
#define GIMBAL_ARRAY_LIST_H

#include "../core/gimbal_api_frame.h"
#include "../algorithms/gimbal_numeric.h"

#define GBL_SELF    GblArrayList* pSelf
#define GBL_CSELF   const GBL_SELF

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
 */
typedef struct GblArrayList {               // Size (32-bit / 64-bit)
    GBL_PRIVATE()
        GblContext*     pCtx;               // 4/8      bytes
        uint8_t*        pData;              // 4/8      bytes
        GblSize         size;               // 4/8      bytes
        GblSize         capacity;           // 4/8      bytes
        uint16_t        elementSize;        // 2        bytes
        uint16_t        zeroTerminated: 1;
        uint16_t        stackCapacity:  15; // 2        bytes
    GBL_PRIVATE_END
} GblArrayList;                             // 20/36    bytes

GBL_EXPORT GBL_RESULT   GblArrayList_construct_7 (GBL_SELF,
                                                  uint16_t     elementSize,
                                                  GblSize      elementCount,
                                                  const void*  pInitialData,
                                                  GblSize      structSize,
                                                  GblBool      zeroTerminated,
                                                  GblContext*  pCtx)                                             GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblArrayList_construct_6 (GBL_SELF,
                                                  uint16_t     elementSize,
                                                  GblSize      elementCount,
                                                  const void*  pInitialData,
                                                  GblSize      structSize,
                                                  GblBool      zeroTerminated)                                   GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblArrayList_construct_5 (GBL_SELF,
                                                  uint16_t     elementSize,
                                                  GblSize      elementCount,
                                                  const void*  pInitialData,
                                                  GblSize      structSize)                                       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblArrayList_construct_4 (GBL_SELF,
                                                  uint16_t     elementSize,
                                                  GblSize      elementCount,
                                                  const void*  pInitialData)                                     GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblArrayList_construct_3 (GBL_SELF,
                                                  uint16_t     elementSize,
                                                  GblSize      elementCount)                                     GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblArrayList_construct_2 (GBL_SELF,
                                                  uint16_t     elementSize)                                      GBL_NOEXCEPT;
#define                 GblArrayList_construct(...) \
                            GBL_VA_OVERLOAD_SELECT(GblArrayList_construct, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)(__VA_ARGS__)
#define                 GblArrayList_alloca(...) \
                            GBL_VA_OVERLOAD_SELECT(GBL_ARRAY_LIST_ALLOCA, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)(__VA_ARGS__)

GBL_EXPORT GBL_RESULT   GblArrayList_destruct    (GBL_SELF)                                                      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblArrayList_assign      (GBL_SELF, const void* pData, GblSize elementCount)             GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblArrayList_release     (GBL_SELF, void** ppVecPtr, GblSize* pSize, GblSize* pCapacity) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblArrayList_acquire     (GBL_SELF, void* pData, GblSize size, GblSize capacity)         GBL_NOEXCEPT;

GBL_INLINE GblContext*  GblArrayList_context     (GBL_CSELF)                                                     GBL_NOEXCEPT;
GBL_INLINE uint16_t     GblArrayList_stackBytes  (GBL_CSELF)                                                     GBL_NOEXCEPT;
GBL_INLINE void*        GblArrayList_stackBuffer (GBL_CSELF)                                                     GBL_NOEXCEPT;
GBL_INLINE GblSize      GblArrayList_size        (GBL_CSELF)                                                     GBL_NOEXCEPT;
GBL_INLINE GblSize      GblArrayList_capacity    (GBL_CSELF)                                                     GBL_NOEXCEPT;
GBL_INLINE uint16_t     GblArrayList_elementSize (GBL_CSELF)                                                     GBL_NOEXCEPT;
GBL_INLINE void*        GblArrayList_data        (GBL_CSELF)                                                     GBL_NOEXCEPT;

GBL_INLINE GblBool      GblArrayList_empty       (GBL_CSELF)                                                     GBL_NOEXCEPT;
GBL_INLINE GblBool      GblArrayList_stack       (GBL_CSELF)                                                     GBL_NOEXCEPT;

GBL_INLINE void*        GblArrayList_at          (GBL_CSELF, GblSize index)                                      GBL_NOEXCEPT;
GBL_INLINE void*        GblArrayList_front       (GBL_CSELF)                                                     GBL_NOEXCEPT;
GBL_INLINE void*        GblArrayList_back        (GBL_CSELF)                                                     GBL_NOEXCEPT;

GBL_EXPORT void*        GblArrayList_insert      (GBL_SELF, GblSize index, GblSize count, const void* pData)     GBL_NOEXCEPT;
GBL_INLINE void*        GblArrayList_emplace     (GBL_SELF, GblSize index)                                       GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT   GblArrayList_pushFront   (GBL_SELF, const void* pData)                                   GBL_NOEXCEPT;
GBL_INLINE void*        GblArrayList_emplaceFront(GBL_SELF)                                                      GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT   GblArrayList_pushBack    (GBL_SELF, const void* pData)                                   GBL_NOEXCEPT;
GBL_INLINE void*        GblArrayList_emplaceBack (GBL_SELF)                                                      GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT   GblArrayList_append      (GBL_SELF, const void* pData, GblSize elementCount)             GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblArrayList_prepend     (GBL_SELF, const void* pData, GblSize elementCount)             GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblArrayList_popFront    (GBL_SELF, void* pOut)                                          GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblArrayList_popBack     (GBL_SELF, void* pOut)                                          GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblArrayList_erase       (GBL_SELF, GblSize begin, GblSize count)                        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblArrayList_clear       (GBL_SELF)                                                      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblArrayList_reserve     (GBL_SELF, GblSize capacity)                                    GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblArrayList_resize      (GBL_SELF, GblSize size)                                        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblArrayList_shrinkToFit (GBL_SELF)                                                      GBL_NOEXCEPT;

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

GBL_INLINE void* GblArrayList_stackBuffer(GBL_CSELF) GBL_NOEXCEPT {
    return (uint8_t*)((pSelf && GBL_PRIV_REF(pSelf).stackCapacity)? (pSelf + 1) : NULL);
}

GBL_INLINE GblBool GblArrayList_empty(GBL_CSELF) GBL_NOEXCEPT  {
    return (pSelf && GBL_PRIV_REF(pSelf).size)? GBL_FALSE : GBL_TRUE;
}

GBL_INLINE GblBool GblArrayList_stack(GBL_CSELF) GBL_NOEXCEPT {
    return (pSelf && (GBL_PRIV_REF(pSelf).pData == GblArrayList_stackBuffer(pSelf)))? GBL_TRUE : GBL_FALSE;
}

GBL_INLINE GBL_RESULT GblArrayList_construct_6(GBL_SELF, uint16_t elementSize, GblSize elementCount, const void* pInitialData, GblSize structSize, GblBool zeroTerminated) GBL_NOEXCEPT {
    return GblArrayList_construct_7(pSelf, elementSize, elementCount, pInitialData, structSize, zeroTerminated, NULL);
}

GBL_INLINE GBL_RESULT GblArrayList_construct_5(GBL_SELF, uint16_t elementSize, GblSize elementCount, const void* pInitialData, GblSize structSize) GBL_NOEXCEPT {
    return GblArrayList_construct_6(pSelf, elementSize, elementCount, pInitialData, structSize, GBL_FALSE);
}

GBL_INLINE GBL_RESULT GblArrayList_construct_4(GBL_SELF, uint16_t elementSize, GblSize elementCount, const void* pInitialData) GBL_NOEXCEPT {
    return GblArrayList_construct_5(pSelf, elementSize, elementCount, pInitialData, sizeof(GblArrayList));
}

GBL_INLINE GBL_RESULT GblArrayList_construct_3(GBL_SELF, uint16_t elementSize, GblSize elementCount) GBL_NOEXCEPT {
    return GblArrayList_construct_4(pSelf, elementSize, elementCount, NULL);
}

GBL_INLINE GBL_RESULT GblArrayList_construct_2(GBL_SELF, uint16_t elementSize) GBL_NOEXCEPT {
    return GblArrayList_construct_3(pSelf, elementSize, 0);
}

GBL_INLINE GblContext* GblArrayList_context(GBL_CSELF) GBL_NOEXCEPT {
    return (pSelf)? GBL_PRIV_REF(pSelf).pCtx : NULL;
}

GBL_INLINE uint16_t GblArrayList_stackBytes(GBL_CSELF) GBL_NOEXCEPT {
    return (pSelf)? GBL_PRIV_REF(pSelf).stackCapacity * GBL_PRIV_REF(pSelf).elementSize : 0;
}

GBL_INLINE GblSize GblArrayList_size(GBL_CSELF) GBL_NOEXCEPT {
    return (pSelf)? GBL_PRIV_REF(pSelf).size : 0;
}

GBL_INLINE GblSize GblArrayList_capacity(GBL_CSELF) GBL_NOEXCEPT {
    return (pSelf)? GBL_PRIV_REF(pSelf).capacity : 0;
}

GBL_INLINE uint16_t GblArrayList_elementSize(GBL_CSELF) GBL_NOEXCEPT {
    return (pSelf)? GBL_PRIV_REF(pSelf).elementSize : 0;
}

GBL_INLINE void* GblArrayList_data(GBL_CSELF) GBL_NOEXCEPT {
    return (pSelf)? GBL_PRIV_REF(pSelf).pData : NULL;
}

GBL_INLINE void* GblArrayList_at(GBL_CSELF, GblSize index) GBL_NOEXCEPT {
    void* pData = NULL;

    if(!pSelf || index >= GBL_PRIV_REF(pSelf).size) GBL_UNLIKELY {

        GBL_API_BEGIN(GBL_PRIV_REF(pSelf).pCtx);
        GBL_API_VERIFY(GBL_FALSE,
                       GBL_RESULT_ERROR_OUT_OF_RANGE);
        GBL_API_END_BLOCK();

    } else GBL_LIKELY {
        pData = &GBL_PRIV_REF(pSelf).pData[index * GBL_PRIV_REF(pSelf).elementSize];
    }

    return pData;
}

GBL_INLINE void* GblArrayList_front(GBL_CSELF) GBL_NOEXCEPT {
    return GblArrayList_at(pSelf, 0);
}

GBL_INLINE void* GblArrayList_back(GBL_CSELF) GBL_NOEXCEPT {
    return GblArrayList_at(pSelf, GBL_PRIV_REF(pSelf).size-1);
}

GBL_INLINE GBL_RESULT GblArrayList_pushBack(GBL_SELF, const void* pData) GBL_NOEXCEPT {
    return !GblArrayList_insert(pSelf, GblArrayList_size(pSelf), 1, pData)?
                GblThread_callRecord(NULL)->result :
                GBL_RESULT_SUCCESS;
}

GBL_INLINE void* GblArrayList_emplaceBack(GBL_SELF) GBL_NOEXCEPT {
    return GblArrayList_insert(pSelf, GblArrayList_size(pSelf), 1, NULL);
}

GBL_INLINE GBL_RESULT GblArrayList_pushFront(GBL_SELF, const void* pData) GBL_NOEXCEPT {
    return !GblArrayList_insert(pSelf, 0, 1, pData)?
                GblThread_callRecord(NULL)->result :
                GBL_RESULT_SUCCESS;
}

GBL_INLINE void* GblArrayList_emplaceFront(GBL_SELF) GBL_NOEXCEPT {
    return GblArrayList_insert(pSelf, 0, 1, NULL);
}

GBL_INLINE GBL_RESULT GblArrayList_append(GblArrayList* pSelf, const void* pData, GblSize elementCount) GBL_NOEXCEPT {
    return !GblArrayList_insert(pSelf, GblArrayList_size(pSelf), elementCount, pData)?
                GblThread_callRecord(NULL)->result :
                GBL_RESULT_SUCCESS;
}

GBL_INLINE GBL_RESULT GblArrayList_prepend(GblArrayList* pSelf, const void* pData, GblSize elementCount) GBL_NOEXCEPT {
    return !GblArrayList_insert(pSelf, 0, elementCount, pData)?
                GblThread_callRecord(NULL)->result :
                GBL_RESULT_SUCCESS;
}

GBL_INLINE GblArrayList* GblArrayList_createInPlace(GBL_SELF,
                                                    uint16_t    elementSize,
                                                    GblSize     elementCount,
                                                    const void* pInitialData,
                                                    GblSize     structSize,
                                                    GblBool     zeroTerminated,
                                                    GblContext* pCtx) GBL_NOEXCEPT
{
    GblArrayList* pOut = NULL;
    GBL_API_BEGIN(pCtx);
    GBL_API_VERIFY_CALL(GblArrayList_construct_7(pSelf, elementSize, elementCount, pInitialData, structSize, zeroTerminated, pCtx));
    pOut = pSelf;
    GBL_API_END_BLOCK();
    return pOut;
}

GBL_INLINE void* GblArrayList_emplace(GBL_SELF, GblSize index) GBL_NOEXCEPT {
    return GblArrayList_insert(pSelf, index, 1, NULL);
}

#undef GBL_CSELF
#undef GBL_SELF

GBL_DECLS_END

#endif // GIMBAL_ARRAY_LIST_H

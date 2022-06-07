#ifndef GIMBAL_VECTOR_H
#define GIMBAL_VECTOR_H

#include "../core/gimbal_api_frame.h"
#include "../algorithms/gimbal_numeric.h"

#define GBL_VECTOR_ALLOCA_5(elemSize, elemCount, pData, extraSize, pCtx) \
    GblVector_createInPlace(GBL_ALLOCA(sizeof(GblVector)+extraSize), elemSize, elemCount, pData, pCtx)

#define GBL_VECTOR_ALLOCA_4(elemSize, elemCount, pData, extraSize) \
    GBL_VECTOR_ALLOCA_5(elemSize, elemCount, pData, extraSize, NULL)

#define GBL_VECTOR_ALLOCA_3(elemSize, elemCount, pData) \
    GBL_VECTOR_ALLOCA_4(elemSize, elemCount, pData, 0)

#define GBL_VECTOR_ALLOCA_2(elemSize, elemCount) \
    GBL_VECTOR_ALLOCA_3(elemSize, elemCount, NULL)

#define GBL_VECTOR_ALLOCA_1(elemSize) \
    GBL_VECTOR_ALLOCA_2(elemSize, 0)

#define GBL_VECTOR_ALLOCA(...) \
     GBL_VA_OVERLOAD_SELECT(GBL_VECTOR_ALLOCA, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)(__VA_ARGS__)


#define SELF    GblVector* pSelf
#define CSELF   const SELF

GBL_DECLS_BEGIN

typedef struct GblVector {          // Size (32-bit / 64-bit)
    GblContext*     pCtx;           // 4/8      bytes
    uint8_t*        pData;          // 4/8      bytes
    GblSize         size;           // 4/8      bytes
    GblSize         capacity;       // 4/8      bytes
    uint16_t        elementSize;    // 2        bytes
    uint16_t        stackSize;      // 2        bytes
    //element destructor?
} GblVector;                        // 20/36    bytes

GBL_INLINE GBL_RESULT   GblVector_construct_6 (SELF,
                                               uint16_t     elementSize,
                                               GblSize      elementCount,
                                               const void*  pInitialData,
                                               GblSize      structSize,
                                               GblContext*  pCtx)                                           GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVector_construct_5 (SELF,
                                               uint16_t     elementSize,
                                               GblSize      elementCount,
                                               const void*  pInitialData,
                                               GblSize      structSize)                                     GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVector_construct_4 (SELF,
                                               uint16_t     elementSize,
                                               GblSize      elementCount,
                                               const void*  pInitialData)                                   GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVector_construct_3 (SELF,
                                               uint16_t elementSize,
                                               GblSize  elementCount)                                       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVector_construct_2 (SELF,
                                               uint16_t elementSize)                                        GBL_NOEXCEPT;
#define                 GblVector_construct(...) \
                            GBL_VA_OVERLOAD_SELECT(GblVector_construct, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)(__VA_ARGS__)
GBL_INLINE GBL_RESULT   GblVector_destruct    (SELF)                                                        GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVector_assign      (SELF, const void* pData, GblSize elementCount)               GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVector_take        (SELF, void** ppVecPtr, GblSize* pSize, GblSize* pCapacity)   GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVector_give        (SELF, void* pData, GblSize size, GblSize capacity)           GBL_NOEXCEPT;

GBL_INLINE GblContext*  GblVector_context     (CSELF)                                                       GBL_NOEXCEPT;
GBL_INLINE uint16_t     GblVector_stackBytes  (CSELF)                                                       GBL_NOEXCEPT;
GBL_INLINE void*        GblVector_stackBuffer (CSELF)                                                       GBL_NOEXCEPT;
GBL_INLINE GblSize      GblVector_size        (CSELF)                                                       GBL_NOEXCEPT;
GBL_INLINE GblSize      GblVector_capacity    (CSELF)                                                       GBL_NOEXCEPT;
GBL_INLINE uint16_t     GblVector_elementSize (CSELF)                                                       GBL_NOEXCEPT;
GBL_INLINE void*        GblVector_data        (CSELF)                                                       GBL_NOEXCEPT;

GBL_INLINE GblBool      GblVector_empty       (CSELF)                                                       GBL_NOEXCEPT;
GBL_INLINE GblBool      GblVector_stack       (CSELF)                                                       GBL_NOEXCEPT;

GBL_INLINE void*        GblVector_at          (CSELF, GblSize index)                                        GBL_NOEXCEPT;
GBL_INLINE void*        GblVector_front       (CSELF)                                                       GBL_NOEXCEPT;
GBL_INLINE void*        GblVector_back        (CSELF)                                                       GBL_NOEXCEPT;

GBL_INLINE void*        GblVector_insert      (SELF, GblSize index, GblSize count, const void* pData)       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVector_pushFront   (SELF, const void* pData)                                     GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVector_pushBack    (SELF, const void* pData)                                     GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVector_append      (SELF, const void* pData, GblSize elementCount)               GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVector_prepend     (SELF, const void* pData, GblSize elementCount)               GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT   GblVector_erase       (SELF, GblSize begin, GblSize count)                          GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVector_clear       (SELF)                                                        GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT   GblVector_reserve     (SELF, GblSize capacity)                                      GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVector_resize      (SELF, GblSize size)                                          GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblVector_shrinkToFit (SELF)                                                        GBL_NOEXCEPT;

// ========== IMPL ==========

GBL_INLINE GblSize GBL_VECTOR_CAPACITY_FROM_SIZE_(GblSize size) GBL_NOEXCEPT {
    return gblPow2Next(size);
}

GBL_INLINE void* GblVector_stackBuffer(CSELF) GBL_NOEXCEPT {
    return (uint8_t*)((pSelf && pSelf->stackSize)? (pSelf + 1) : NULL);
}

GBL_INLINE GblBool GblVector_empty(CSELF) GBL_NOEXCEPT  {
    return (pSelf && pSelf->size)? GBL_FALSE : GBL_TRUE;
}

GBL_INLINE GblBool GblVector_stack(CSELF) GBL_NOEXCEPT {
    return (pSelf && (pSelf->pData == GblVector_stackBuffer(pSelf)))? GBL_TRUE : GBL_FALSE;
}

GBL_INLINE void GblVector_initialize_(SELF) GBL_NOEXCEPT {
    GBL_ASSERT(pSelf->elementSize);
    pSelf->capacity = pSelf->stackSize / pSelf->elementSize;
    pSelf->pData = (uint8_t*)GblVector_stackBuffer(pSelf);
    pSelf->size = 0;
    if(pSelf->pData) pSelf->pData[0] = 0;
}

GBL_INLINE GBL_RESULT GblVector_destruct(SELF) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->pCtx);
   // Check if we have a buffer to free
   if(pSelf->pData && pSelf->pData != GblVector_stackBuffer(pSelf)) {
       if(pSelf->pCtx) GBL_API_FREE(pSelf->pData);
       else free(pSelf->pData);
       pSelf->pData = (uint8_t*)GblVector_stackBuffer(pSelf);
   }
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblVector_clear(SELF) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->pCtx);
    GBL_API_CALL(GblVector_destruct(pSelf));
    GblVector_initialize_(pSelf);
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblVector_alloc_(SELF, GblSize capacity) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->pCtx);

    if(capacity * pSelf->elementSize <= pSelf->stackSize) {
        pSelf->pData = (uint8_t*)GblVector_stackBuffer(pSelf);
        pSelf->capacity = pSelf->stackSize / pSelf->elementSize;
    } else {
        GblSize allocSize = capacity * pSelf->elementSize;
        if(allocSize < GBL_ALIGNOF(GBL_MAX_ALIGN_T)) {
            allocSize = gblPow2Next(GBL_ALIGNOF(GBL_MAX_ALIGN_T));
        }
        pSelf->capacity = allocSize / pSelf->elementSize;
        pSelf->pData = pSelf->pCtx != GBL_NULL?
                    (uint8_t*)GBL_API_MALLOC(allocSize) :
                    (uint8_t*)malloc(allocSize);
    }
    pSelf->pData[0] = 0;
    pSelf->size = 0;
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblVector_assign(SELF, const void* pData, GblSize elementCount) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->pCtx);
    if(pSelf->capacity < elementCount + 1) {
        GBL_API_CALL(GblVector_clear(pSelf));
        if(elementCount) GBL_API_CALL(GblVector_alloc_(pSelf, GBL_VECTOR_CAPACITY_FROM_SIZE_(pSelf->elementSize * elementCount + 1)));
    }
    if(elementCount) {
        if(pData) memcpy(pSelf->pData, pData, elementCount * pSelf->elementSize);
        else memset(pSelf->pData, 0, elementCount * pSelf->elementSize);
    }
    pSelf->size = elementCount;
    if(pSelf->pData) pSelf->pData[pSelf->size * pSelf->elementSize] = 0;
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblVector_construct_6(SELF, uint16_t elementSize, GblSize elementCount, const void* pInitialData, GblSize structSize, GblContext* pCtx) GBL_NOEXCEPT {
    GBL_API_BEGIN(pCtx);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_ARG(structSize >= sizeof(GblVector));
    memset(pSelf, 0, structSize);
    pSelf->pCtx           = pCtx;
    pSelf->stackSize      = structSize - sizeof(GblVector);
    pSelf->elementSize    = elementSize;
    GblVector_initialize_(pSelf);
    GBL_API_CALL(GblVector_assign(pSelf, pInitialData, elementCount));
    GBL_API_END();
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

GBL_INLINE GBL_RESULT GblVector_take(SELF, void** ppVecPtr, GblSize* pSize, GblSize* pCapacity) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->pCtx);
    GblBool stack = GBL_FALSE;
    GBL_API_VERIFY_POINTER(ppVecPtr);
    GBL_API_VERIFY_POINTER(pCapacity);
    GBL_API_VERIFY_POINTER(pSize);
    stack = GblVector_stack(pSelf);
    GBL_API_VERIFY(!stack, GBL_RESULT_ERROR_INVALID_OPERATION, "Cannot Take Stack Buffer!");
    *ppVecPtr = pSelf->pData;
    *pCapacity = pSelf->capacity;
    *pSize = pSelf->size;
    GblVector_initialize_(pSelf);
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblVector_give(SELF, void* pData, GblSize size, GblSize capacity) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->pCtx);
    GBL_API_VERIFY_POINTER(pData);
    GBL_API_VERIFY_ARG(capacity);
    GBL_API_VERIFY_ARG(size);
    GBL_API_CALL(GblVector_destruct(pSelf));
    pSelf->pData = (uint8_t*)pData;
    pSelf->capacity = capacity;
    pSelf->size = size;
    GBL_API_END();
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

GBL_INLINE GBL_RESULT GblVector_reserve(SELF, GblSize capacity) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->pCtx);
    if(pSelf->capacity < capacity) {
        const GblBool stack = GblVector_stack(pSelf);
        if(stack) {
            const GblSize oldSize = pSelf->size;
            GBL_API_CALL(GblVector_alloc_(pSelf, GBL_VECTOR_CAPACITY_FROM_SIZE_(capacity * pSelf->elementSize + 1)));
            memcpy(pSelf->pData, GblVector_stackBuffer(pSelf), oldSize * pSelf->elementSize);
            pSelf->size = oldSize;
        } else {
            pSelf->pData = pSelf->pCtx != GBL_NULL?
                        (uint8_t*)GBL_API_REALLOC(pSelf->pData, capacity * pSelf->elementSize) :
                        (uint8_t*)realloc(pSelf->pData, capacity * pSelf->elementSize);
            pSelf->capacity = capacity;
        }
        pSelf->pData[pSelf->size * pSelf->elementSize] = 0;
    }
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblVector_resize(SELF, GblSize size) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->pCtx);
    if(size > pSelf->size) {

        if(size+1 > pSelf->capacity) {
            GBL_API_CALL(GblVector_reserve(pSelf, GBL_VECTOR_CAPACITY_FROM_SIZE_(size + 1)));
        }

        //pSelf->data.pData[pSelf->data.length] = ' '; remove old nullcharacter so strlen isn't wrong!
    }

    pSelf->pData[size * pSelf->elementSize] = 0;
    pSelf->size = size;
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblVector_shrinkToFit(SELF) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->pCtx);
    const GblBool stack = GblVector_stack(pSelf);
    if(!stack && pSelf->capacity > pSelf->size) {
        pSelf->pData = pSelf->pCtx != GBL_NULL?
                    (uint8_t*)GBL_API_REALLOC(pSelf->pData, pSelf->size * pSelf->elementSize) :
                    (uint8_t*)realloc(pSelf->pData, pSelf->size * pSelf->elementSize);
        pSelf->capacity = pSelf->size;
    }
    GBL_API_END();
}


GBL_INLINE GBL_RESULT GblVector_append(SELF, const void* pData, GblSize elementCount) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->pCtx);
    GblSize expectedSize = 0;
    GBL_API_VERIFY_POINTER(pData);
    GBL_API_VERIFY_ARG(elementCount);
    expectedSize = GblVector_size(pSelf);
    expectedSize += elementCount;
    GBL_API_CALL(GblVector_reserve(pSelf, expectedSize + 1));
    memcpy(&pSelf->pData[pSelf->size * pSelf->elementSize], pData, elementCount * pSelf->elementSize);
    pSelf->size = expectedSize;
    pSelf->pData[pSelf->size * pSelf->elementSize] = 0;
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblVector_prepend(SELF, const void* pData, GblSize elementCount) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->pCtx);
    GblSize expectedSize = 0;
    GBL_API_VERIFY_POINTER(pData);
    GBL_API_VERIFY_ARG(elementCount);
    expectedSize = GblVector_size(pSelf);
    expectedSize += elementCount;
    GBL_API_CALL(GblVector_reserve(pSelf, expectedSize + 1));
    memcpy(pSelf->pData + elementCount*pSelf->elementSize, pSelf->pData, elementCount*pSelf->elementSize);
    memcpy(pSelf->pData, pData, elementCount*pSelf->elementSize);
    pSelf->size = expectedSize;
    pSelf->pData[pSelf->size * pSelf->elementSize] = 0;
    GBL_API_END();
}

GBL_INLINE void* GblVector_at(CSELF, GblSize index) GBL_NOEXCEPT {
    void* pData = NULL;
    GBL_API_BEGIN(pSelf->pCtx);
    GBL_API_VERIFY_ARG(index < pSelf->size);
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


// HANDLES DYNAMIC REALLOCATION
GBL_INLINE void* GblVector_insert(SELF, GblSize index, GblSize count, const void* pData) GBL_NOEXCEPT {
    uintptr_t insertionPoint = 0;
    GblSize slideSize = 0;
    void* pDataOut = NULL;
    GBL_API_BEGIN(pSelf->pCtx);
    GBL_API_VERIFY_ARG(index <= pSelf->size);
    GBL_API_CALL(GblVector_resize(pSelf, pSelf->size + count));
    slideSize = (pSelf->size-1) - index;
    insertionPoint = (uintptr_t)pSelf->pData + index * pSelf->elementSize;
    if(slideSize) memmove((uint8_t*)insertionPoint + pSelf->elementSize * count,
                          (const uint8_t*)insertionPoint,
                          slideSize * pSelf->elementSize);
    if(pData) {
        memcpy((void*)insertionPoint, pData, pSelf->elementSize * count);
    }
    pDataOut = (void*)insertionPoint;
    GBL_API_END_BLOCK();
    return pDataOut;
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

GBL_INLINE GBL_RESULT GblVector_erase(SELF, GblSize begin, GblSize count) GBL_NOEXCEPT {
    GblSize lastIndex = 0;
    GblSize remainderCount  = 0;
    GBL_API_BEGIN(pSelf->pCtx);
    GBL_API_VERIFY_EXPRESSION(pSelf->size > 0);
    GBL_API_VERIFY_ARG(begin < pSelf->size);
    lastIndex = begin + count - 1;
    GBL_API_VERIFY_ARG(lastIndex < pSelf->size);
    GBL_API_VERIFY_ARG(begin <= lastIndex);
    GBL_API_VERIFY_EXPRESSION(count > 0 && count <= pSelf->size);
    remainderCount = pSelf->size - 1 - lastIndex;
    if(remainderCount) memmove(&pSelf->pData[begin * pSelf->elementSize],
                               &pSelf->pData[(lastIndex+1) * pSelf->elementSize],
                               remainderCount * pSelf->elementSize);
    GBL_API_CALL(GblVector_resize(pSelf, pSelf->size - count));
    GBL_API_END();
}

#undef CSELF
#undef SELF

GBL_DECLS_END

#endif // GIMBAL_VECTOR_H

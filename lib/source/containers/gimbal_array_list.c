#include <gimbal/containers/gimbal_array_list.h>

GBL_INLINE void GblArrayList_initialize_(GblArrayList* pSelf) GBL_NOEXCEPT {
    GBL_ASSERT(GBL_PRIV_REF(pSelf).elementSize);

    GBL_PRIV_REF(pSelf).capacity = GBL_PRIV_REF(pSelf).stackCapacity;
    GBL_PRIV_REF(pSelf).pData = (uint8_t*)GblArrayList_stackBuffer(pSelf);
    GBL_PRIV_REF(pSelf).size = 0;

    if GBL_UNLIKELY(GBL_PRIV_REF(pSelf).pData && GBL_PRIV_REF(pSelf).zeroTerminated)
        memset(&GBL_PRIV_REF(pSelf).pData[GBL_PRIV_REF(pSelf).size * GBL_PRIV_REF(pSelf).elementSize],
               0,
               GBL_PRIV_REF(pSelf).elementSize);
}


static GBL_RESULT GblArrayList_alloc_(GblArrayList* pSelf, size_t  size) GBL_NOEXCEPT {
    GBL_RESULT result = GBL_RESULT_SUCCESS;

    if GBL_UNLIKELY(!pSelf) return GBL_RESULT_ERROR_INVALID_POINTER;

    if GBL_UNLIKELY(GBL_PRIV_REF(pSelf).zeroTerminated) ++size;;

    if(GBL_PRIV_REF(pSelf).stackCapacity && size <= GBL_PRIV_REF(pSelf).stackCapacity) {
        GBL_PRIV_REF(pSelf).pData = (uint8_t*)GblArrayList_stackBuffer(pSelf);
        GBL_PRIV_REF(pSelf).capacity = GBL_PRIV_REF(pSelf).stackCapacity;

    } else {

        size_t  allocSize = size * GBL_PRIV_REF(pSelf).elementSize;
        if(allocSize < GBL_ALIGNOF(GBL_MAX_ALIGN_T)) {
            allocSize = gblPow2Next(GBL_ALIGNOF(GBL_MAX_ALIGN_T));
        }
        GBL_PRIV_REF(pSelf).capacity = allocSize / GBL_PRIV_REF(pSelf).elementSize;
        GBL_CTX_BEGIN(GBL_PRIV_REF(pSelf).pCtx);
        GBL_PRIV_REF(pSelf).pData = GBL_CTX_MALLOC(allocSize);
        GBL_CTX_END_BLOCK();
    }

    if GBL_UNLIKELY(GBL_PRIV_REF(pSelf).pData && GBL_PRIV_REF(pSelf).zeroTerminated)
        memset(&GBL_PRIV_REF(pSelf).pData[GBL_PRIV_REF(pSelf).size * GBL_PRIV_REF(pSelf).elementSize], 0, GBL_PRIV_REF(pSelf).elementSize);

    GBL_PRIV_REF(pSelf).size = 0;
    return result;
}

GBL_EXPORT GBL_RESULT GblArrayList_destruct(GblArrayList* pSelf) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_PRIV_REF(pSelf).pCtx);
   // Check if we have a buffer to free
   if(GBL_PRIV_REF(pSelf).pData && GBL_PRIV_REF(pSelf).pData != GblArrayList_stackBuffer(pSelf)) {
       GBL_CTX_FREE(GBL_PRIV_REF(pSelf).pData);
       GBL_PRIV_REF(pSelf).pData = (uint8_t*)GblArrayList_stackBuffer(pSelf);
   }
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblArrayList_clear(GblArrayList* pSelf) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_PRIV_REF(pSelf).pCtx);
    GBL_CTX_CALL(GblArrayList_destruct(pSelf));
    GblArrayList_initialize_(pSelf);
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblArrayList_assign(GblArrayList* pSelf, const void* pData, size_t  elementCount) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_PRIV_REF(pSelf).pCtx);
    if(GBL_PRIV_REF(pSelf).capacity < elementCount + (GBL_PRIV_REF(pSelf).zeroTerminated? 1 : 0)) {
        GBL_CTX_CALL(GblArrayList_clear(pSelf));
        if(elementCount) GBL_CTX_CALL(GblArrayList_alloc_(pSelf, elementCount));
    }
    if(elementCount) {
        if(pData) memcpy(GBL_PRIV_REF(pSelf).pData, pData, elementCount * GBL_PRIV_REF(pSelf).elementSize);
        else memset(GBL_PRIV_REF(pSelf).pData, 0, elementCount * GBL_PRIV_REF(pSelf).elementSize);
    }
    GBL_PRIV_REF(pSelf).size = elementCount;
    if GBL_UNLIKELY(GBL_PRIV_REF(pSelf).pData && GBL_PRIV_REF(pSelf).zeroTerminated)
        memset(&GBL_PRIV_REF(pSelf).pData[GBL_PRIV_REF(pSelf).size * GBL_PRIV_REF(pSelf).elementSize], 0, GBL_PRIV_REF(pSelf).elementSize);

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblArrayList_construct_7(GblArrayList* pSelf, uint16_t elementSize, size_t  elementCount, const void* pInitialData, size_t  structSize, GblBool zeroTerminated, GblContext* pCtx) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(pCtx);
    GBL_CTX_VERIFY_POINTER(pSelf);
    GBL_CTX_VERIFY_ARG(structSize >= sizeof(GblArrayList));
    GBL_CTX_VERIFY_ARG(elementSize);
    GBL_PRIV_REF(pSelf).pCtx           = pCtx;
    GBL_PRIV_REF(pSelf).stackCapacity  = (structSize - sizeof(GblArrayList)) / elementSize;
    GBL_PRIV_REF(pSelf).elementSize    = elementSize;
    GBL_PRIV_REF(pSelf).zeroTerminated = zeroTerminated;
    GblArrayList_initialize_(pSelf);
    GBL_CTX_CALL(GblArrayList_assign(pSelf, pInitialData, elementCount));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblArrayList_release(GblArrayList* pSelf, void** ppVecPtr, size_t * pSize, size_t * pCapacity) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_PRIV_REF(pSelf).pCtx);
    GblBool stack = GBL_FALSE;
    GBL_CTX_VERIFY_POINTER(ppVecPtr);
    GBL_CTX_VERIFY_POINTER(pCapacity);
    GBL_CTX_VERIFY_POINTER(pSize);
    stack = GblArrayList_stack(pSelf);
    GBL_CTX_VERIFY(!stack, GBL_RESULT_ERROR_INVALID_OPERATION, "Cannot Take Stack Buffer!");
    *ppVecPtr = GBL_PRIV_REF(pSelf).pData;
    *pCapacity = GBL_PRIV_REF(pSelf).capacity;
    *pSize = GBL_PRIV_REF(pSelf).size;
    GblArrayList_initialize_(pSelf);
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblArrayList_acquire(GblArrayList* pSelf, void* pData, size_t  size, size_t  capacity) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GBL_PRIV_REF(pSelf).pCtx);
    GBL_CTX_VERIFY_POINTER(pData);
    GBL_CTX_VERIFY_ARG(capacity);
    GBL_CTX_VERIFY_ARG(size);
    GBL_CTX_CALL(GblArrayList_destruct(pSelf));
    GBL_PRIV_REF(pSelf).pData = (uint8_t*)pData;
    GBL_PRIV_REF(pSelf).capacity = capacity;
    GBL_PRIV_REF(pSelf).size = size;
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblArrayList_reserve(GblArrayList* pSelf, size_t  capacity) GBL_NOEXCEPT {
    GBL_RESULT result = GBL_RESULT_SUCCESS;

    if GBL_UNLIKELY(!pSelf)
        return GBL_RESULT_ERROR_INVALID_POINTER;

    if GBL_UNLIKELY(GBL_PRIV_REF(pSelf).capacity < capacity) {
        if(capacity > 2 && capacity % 2) capacity = gblPow2Next(capacity);
        const GblBool stack = GblArrayList_stack(pSelf);
        if(stack) {
            const size_t  oldSize = GBL_PRIV_REF(pSelf).size;
            result = GblArrayList_alloc_(pSelf, capacity);
            memcpy(GBL_PRIV_REF(pSelf).pData, GblArrayList_stackBuffer(pSelf), oldSize * GBL_PRIV_REF(pSelf).elementSize);
            GBL_PRIV_REF(pSelf).size = oldSize;
        } else {
            GBL_CTX_BEGIN(GBL_PRIV_REF(pSelf).pCtx);
            GBL_PRIV_REF(pSelf).pData = GBL_CTX_REALLOC(GBL_PRIV_REF(pSelf).pData,
                                           (capacity + (GBL_PRIV_REF(pSelf).zeroTerminated? 1 : 0)) * GBL_PRIV_REF(pSelf).elementSize);
            GBL_CTX_END_BLOCK();
            GBL_PRIV_REF(pSelf).capacity = capacity;
        }
        if GBL_UNLIKELY(GBL_PRIV_REF(pSelf).zeroTerminated)
                memset(&GBL_PRIV_REF(pSelf).pData[GBL_PRIV_REF(pSelf).size * GBL_PRIV_REF(pSelf).elementSize], 0, GBL_PRIV_REF(pSelf).elementSize);
    }

    return result;
}

GBL_EXPORT GBL_RESULT GblArrayList_resize(GblArrayList* pSelf, size_t  size) GBL_NOEXCEPT {
    GBL_RESULT result = GBL_RESULT_SUCCESS;

    if GBL_UNLIKELY(!pSelf) {
        GBL_CTX_BEGIN(NULL);
        GBL_CTX_END();
    } else {

        if GBL_UNLIKELY(size > GBL_PRIV_REF(pSelf).size) {
            result = GblArrayList_reserve(pSelf, size);
        }

        if GBL_UNLIKELY (GBL_PRIV_REF(pSelf).zeroTerminated)
            memset(&GBL_PRIV_REF(pSelf).pData[size * GBL_PRIV_REF(pSelf).elementSize], 0, GBL_PRIV_REF(pSelf).elementSize);

        GBL_PRIV_REF(pSelf).size = size;
    }
    return result;
}

GBL_EXPORT GBL_RESULT GblArrayList_shrinkToFit(GblArrayList* pSelf) GBL_NOEXCEPT {
    GBL_RESULT result = GBL_RESULT_SUCCESS;

    if GBL_UNLIKELY(!pSelf)
        return GBL_RESULT_ERROR_INVALID_POINTER;

    const GblBool stack = GblArrayList_stack(pSelf);

    if(!stack && GBL_PRIV_REF(pSelf).capacity > GBL_PRIV_REF(pSelf).size) {
        const size_t fitSize = GBL_PRIV_REF(pSelf).size + ((GBL_PRIV_REF(pSelf).zeroTerminated)? 1 : 0);
        const size_t bytes = fitSize * GBL_PRIV_REF(pSelf).elementSize;

        GBL_CTX_BEGIN(GBL_PRIV_REF(pSelf).pCtx);

        if(fitSize <= GBL_PRIV_REF(pSelf).stackCapacity) {
            memcpy(GblArrayList_stackBuffer(pSelf), GBL_PRIV_REF(pSelf).pData, bytes);
            GBL_CTX_FREE(GBL_PRIV_REF(pSelf).pData);
            GBL_PRIV_REF(pSelf).pData = GblArrayList_stackBuffer(pSelf);
        } else {
            GBL_PRIV_REF(pSelf).pData = GBL_CTX_REALLOC(GBL_PRIV_REF(pSelf).pData, bytes);
        }

        GBL_CTX_END_BLOCK();
        GBL_PRIV_REF(pSelf).capacity = GBL_PRIV_REF(pSelf).size;
    }

    return result;
}

GBL_EXPORT void* GblArrayList_insert(GblArrayList* pSelf, size_t  index, size_t  count, const void* pData) GBL_NOEXCEPT {
    uintptr_t insertionPoint = 0;
    size_t  slideSize = 0;
    void* pDataOut = NULL;

    if GBL_UNLIKELY(!pSelf || !count || index > GBL_PRIV_REF(pSelf).size) {
        GBL_CTX_BEGIN(pSelf? (GblObject*)GBL_PRIV_REF(pSelf).pCtx : NULL);
        GBL_CTX_VERIFY_ARG(count);
        GBL_CTX_VERIFY(index <= GBL_PRIV_REF(pSelf).size,
                       GBL_RESULT_ERROR_OUT_OF_RANGE);
        GBL_CTX_END_BLOCK();

    } else {
        slideSize = GBL_PRIV_REF(pSelf).size - index;
        GBL_RESULT result = GblArrayList_resize(pSelf, GBL_PRIV_REF(pSelf).size + count);

        if GBL_LIKELY(GBL_RESULT_SUCCESS(result)) {

            insertionPoint = (uintptr_t)GBL_PRIV_REF(pSelf).pData + index * GBL_PRIV_REF(pSelf).elementSize;
            if(slideSize) memmove((uint8_t*)insertionPoint + GBL_PRIV_REF(pSelf).elementSize * count,
                                  (const uint8_t*)insertionPoint,
                                  slideSize * GBL_PRIV_REF(pSelf).elementSize);
            if(pData) {
                memcpy((void*)insertionPoint, pData, GBL_PRIV_REF(pSelf).elementSize * count);
            }
            pDataOut = (void*)insertionPoint;
        }
    }
    return pDataOut;
}


GBL_EXPORT GBL_RESULT GblArrayList_erase(GblArrayList* pSelf, size_t  begin, size_t  count) GBL_NOEXCEPT {
    size_t  lastIndex = 0;
    size_t  remainderCount  = 0;

    GBL_CTX_BEGIN(GBL_PRIV_REF(pSelf).pCtx);
    GBL_CTX_VERIFY(GBL_PRIV_REF(pSelf).size > 0, GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_VERIFY(begin < GBL_PRIV_REF(pSelf).size, GBL_RESULT_ERROR_OUT_OF_RANGE);
    lastIndex = begin + count - 1;
    GBL_CTX_VERIFY(lastIndex < GBL_PRIV_REF(pSelf).size, GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_VERIFY(begin <= lastIndex, GBL_RESULT_ERROR_OUT_OF_RANGE);
    if(!count) GBL_CTX_DONE();
    GBL_CTX_VERIFY(count > 0 && count <= GBL_PRIV_REF(pSelf).size, GBL_RESULT_ERROR_OUT_OF_RANGE);
    remainderCount = GBL_PRIV_REF(pSelf).size - 1 - lastIndex;
    if(remainderCount) memmove(&GBL_PRIV_REF(pSelf).pData[begin * GBL_PRIV_REF(pSelf).elementSize],
                               &GBL_PRIV_REF(pSelf).pData[(lastIndex+1) * GBL_PRIV_REF(pSelf).elementSize],
                               remainderCount * GBL_PRIV_REF(pSelf).elementSize);
    GBL_CTX_CALL(GblArrayList_resize(pSelf, GBL_PRIV_REF(pSelf).size - count));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblArrayList_popFront(GblArrayList* pSelf, void* pOut) {
    GBL_CTX_BEGIN(GBL_PRIV_REF(pSelf).pCtx);
    if(pOut) memcpy(pOut, GblArrayList_at(pSelf, 0), GblArrayList_elementSize(pSelf));
    GBL_CTX_VERIFY_CALL(GblArrayList_erase(pSelf, 0, 1));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblArrayList_popBack(GblArrayList* pSelf, void* pOut) {
    GBL_CTX_BEGIN(GBL_PRIV_REF(pSelf).pCtx);
    const size_t  index = GblArrayList_size(pSelf)-1;
    if(pOut) memcpy(pOut, GblArrayList_at(pSelf, index), GblArrayList_elementSize(pSelf));
    GBL_CTX_VERIFY_CALL(GblArrayList_erase(pSelf, index, 1));
    GBL_CTX_END();
}

GBL_EXPORT void* GblArrayList_stackBuffer(const GblArrayList* pSelf) {
    return (uint8_t*)((pSelf && GBL_PRIV_REF(pSelf).stackCapacity)? (pSelf + 1) : NULL);
}

GBL_EXPORT GblBool GblArrayList_empty(const GblArrayList* pSelf) {
    return (pSelf && GBL_PRIV_REF(pSelf).size)? GBL_FALSE : GBL_TRUE;
}

GBL_EXPORT GblBool GblArrayList_stack(const GblArrayList* pSelf) {
    return (pSelf && (GBL_PRIV_REF(pSelf).pData == GblArrayList_stackBuffer(pSelf)))? GBL_TRUE : GBL_FALSE;
}

GBL_EXPORT GBL_RESULT GblArrayList_construct_6(GblArrayList* pSelf, uint16_t elementSize, size_t  elementCount, const void* pInitialData, size_t  structSize, GblBool zeroTerminated) {
    return GblArrayList_construct_7(pSelf, elementSize, elementCount, pInitialData, structSize, zeroTerminated, NULL);
}

GBL_EXPORT GBL_RESULT GblArrayList_construct_5(GblArrayList* pSelf, uint16_t elementSize, size_t  elementCount, const void* pInitialData, size_t  structSize) {
    return GblArrayList_construct_6(pSelf, elementSize, elementCount, pInitialData, structSize, GBL_FALSE);
}

GBL_EXPORT GBL_RESULT GblArrayList_construct_4(GblArrayList* pSelf, uint16_t elementSize, size_t  elementCount, const void* pInitialData) {
    return GblArrayList_construct_5(pSelf, elementSize, elementCount, pInitialData, sizeof(GblArrayList));
}

GBL_EXPORT GBL_RESULT GblArrayList_construct_3(GblArrayList* pSelf, uint16_t elementSize, size_t  elementCount) {
    return GblArrayList_construct_4(pSelf, elementSize, elementCount, NULL);
}

GBL_EXPORT GBL_RESULT GblArrayList_construct_2(GblArrayList* pSelf, uint16_t elementSize) {
    return GblArrayList_construct_3(pSelf, elementSize, 0);
}

GBL_EXPORT GblContext* GblArrayList_context(const GblArrayList* pSelf) {
    return (pSelf)? GBL_PRIV_REF(pSelf).pCtx : NULL;
}

GBL_EXPORT uint16_t GblArrayList_stackBytes(const GblArrayList* pSelf) {
    return (pSelf)? GBL_PRIV_REF(pSelf).stackCapacity * GBL_PRIV_REF(pSelf).elementSize : 0;
}

GBL_EXPORT size_t  GblArrayList_size(const GblArrayList* pSelf) {
    return (pSelf)? GBL_PRIV_REF(pSelf).size : 0;
}

GBL_EXPORT size_t  GblArrayList_capacity(const GblArrayList* pSelf) {
    return (pSelf)? GBL_PRIV_REF(pSelf).capacity : 0;
}

GBL_EXPORT uint16_t GblArrayList_elementSize(const GblArrayList* pSelf) {
    return (pSelf)? GBL_PRIV_REF(pSelf).elementSize : 0;
}

GBL_EXPORT void* GblArrayList_data(const GblArrayList* pSelf) {
    return (pSelf)? GBL_PRIV_REF(pSelf).pData : NULL;
}

GBL_EXPORT void* GblArrayList_at(const GblArrayList* pSelf, size_t  index) {
    void* pData = NULL;

    if GBL_UNLIKELY(index >= GBL_PRIV_REF(pSelf).size) {
        GBL_CTX_BEGIN(GBL_PRIV_REF(pSelf).pCtx);
        GBL_CTX_VERIFY(GBL_FALSE,
                       GBL_RESULT_ERROR_OUT_OF_RANGE);
        GBL_CTX_END_BLOCK();

    } else {
        pData = &GBL_PRIV_REF(pSelf).pData[index * GBL_PRIV_REF(pSelf).elementSize];
    }

    return pData;
}

GBL_EXPORT void* GblArrayList_front(const GblArrayList* pSelf) {
    return GblArrayList_at(pSelf, 0);
}

GBL_EXPORT void* GblArrayList_back(const GblArrayList* pSelf) {
    return GblArrayList_at(pSelf, GBL_PRIV_REF(pSelf).size-1);
}

GBL_EXPORT GBL_RESULT GblArrayList_pushBack(GblArrayList* pSelf, const void* pData) {
    return !GblArrayList_insert(pSelf, GblArrayList_size(pSelf), 1, pData)?
                GblThd_callRecord(NULL)->result :
                GBL_RESULT_SUCCESS;
}

GBL_EXPORT void* GblArrayList_emplaceBack(GblArrayList* pSelf) {
    return GblArrayList_insert(pSelf, GblArrayList_size(pSelf), 1, NULL);
}

GBL_EXPORT GBL_RESULT GblArrayList_pushFront(GblArrayList* pSelf, const void* pData) {
    return !GblArrayList_insert(pSelf, 0, 1, pData)?
                GblThd_callRecord(NULL)->result :
                GBL_RESULT_SUCCESS;
}

GBL_EXPORT void* GblArrayList_emplaceFront(GblArrayList* pSelf) {
    return GblArrayList_insert(pSelf, 0, 1, NULL);
}

GBL_EXPORT GBL_RESULT GblArrayList_append(GblArrayList* pSelf, const void* pData, size_t elementCount) {
    return !GblArrayList_insert(pSelf, GblArrayList_size(pSelf), elementCount, pData)?
                GblThd_callRecord(NULL)->result :
                GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT GblArrayList_prepend(GblArrayList* pSelf, const void* pData, size_t elementCount) {
    return !GblArrayList_insert(pSelf, 0, elementCount, pData)?
                GblThd_callRecord(NULL)->result :
                GBL_RESULT_SUCCESS;
}

GBL_EXPORT GblArrayList* GblArrayList_createInPlace(GblArrayList* pSelf,
                                                    uint16_t    elementSize,
                                                    size_t      elementCount,
                                                    const void* pInitialData,
                                                    size_t      structSize,
                                                    GblBool     zeroTerminated,
                                                    GblContext* pCtx)
{
    GblArrayList* pOut = NULL;
    GBL_CTX_BEGIN(pCtx);
    GBL_CTX_VERIFY_CALL(GblArrayList_construct_7(pSelf, elementSize, elementCount, pInitialData, structSize, zeroTerminated, pCtx));
    pOut = pSelf;
    GBL_CTX_END_BLOCK();
    return pOut;
}

GBL_EXPORT void* GblArrayList_emplace(GblArrayList* pSelf, size_t  index) {
    return GblArrayList_insert(pSelf, index, 1, NULL);
}

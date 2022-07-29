#include <gimbal/containers/gimbal_array_list.h>

GBL_INLINE void GblArrayList_initialize_(GblArrayList* pSelf) GBL_NOEXCEPT {
    GBL_ASSERT(pSelf->elementSize);
    pSelf->capacity = pSelf->stackCapacity;
    pSelf->pData = (uint8_t*)GblArrayList_stackBuffer(pSelf);
    pSelf->size = 0;
    if(pSelf->pData && pSelf->zeroTerminated) memset(&pSelf->pData[pSelf->size * pSelf->elementSize], 0, pSelf->elementSize);
}


static GBL_RESULT GblArrayList_alloc_(GblArrayList* pSelf, GblSize size) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->pCtx);

    if(pSelf->zeroTerminated) ++size;;

    if(pSelf->stackCapacity && size <= pSelf->stackCapacity) {
        pSelf->pData = (uint8_t*)GblArrayList_stackBuffer(pSelf);
        pSelf->capacity = pSelf->stackCapacity;
    } else {

        GblSize allocSize = size * pSelf->elementSize;
        if(allocSize < GBL_ALIGNOF(GBL_MAX_ALIGN_T)) {
            allocSize = gblPow2Next(GBL_ALIGNOF(GBL_MAX_ALIGN_T));
        }
        pSelf->capacity = allocSize / pSelf->elementSize;
        pSelf->pData = GBL_API_MALLOC(allocSize);
    }
    if(pSelf->pData && pSelf->zeroTerminated) memset(&pSelf->pData[pSelf->size * pSelf->elementSize], 0, pSelf->elementSize);
    pSelf->size = 0;
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblArrayList_destruct(GblArrayList* pSelf) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->pCtx);
   // Check if we have a buffer to free
   if(pSelf->pData && pSelf->pData != GblArrayList_stackBuffer(pSelf)) {
       GBL_API_FREE(pSelf->pData);
       pSelf->pData = (uint8_t*)GblArrayList_stackBuffer(pSelf);
   }
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblArrayList_clear(GblArrayList* pSelf) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->pCtx);
    GBL_API_CALL(GblArrayList_destruct(pSelf));
    GblArrayList_initialize_(pSelf);
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblArrayList_assign(GblArrayList* pSelf, const void* pData, GblSize elementCount) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->pCtx);
    if(pSelf->capacity < elementCount + (pSelf->zeroTerminated? 1 : 0)) {
        GBL_API_CALL(GblArrayList_clear(pSelf));
        if(elementCount) GBL_API_CALL(GblArrayList_alloc_(pSelf, elementCount));
    }
    if(elementCount) {
        if(pData) memcpy(pSelf->pData, pData, elementCount * pSelf->elementSize);
        else memset(pSelf->pData, 0, elementCount * pSelf->elementSize);
    }
    pSelf->size = elementCount;
    if(pSelf->pData && pSelf->zeroTerminated)
        memset(&pSelf->pData[pSelf->size * pSelf->elementSize], 0, pSelf->elementSize);
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblArrayList_construct_6(GblArrayList* pSelf, uint16_t elementSize, GblSize elementCount, const void* pInitialData, GblSize structSize, GblContext* pCtx) GBL_NOEXCEPT {
    GBL_API_BEGIN(pCtx);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_ARG(structSize >= sizeof(GblArrayList));
    GBL_API_VERIFY_ARG(elementSize);
    //memset(pSelf, 0, structSize);
    pSelf->pCtx           = pCtx;
    pSelf->stackCapacity  = (structSize - sizeof(GblArrayList)) / elementSize;
    pSelf->elementSize    = elementSize;
    GblArrayList_initialize_(pSelf);
    GBL_API_CALL(GblArrayList_assign(pSelf, pInitialData, elementCount));
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblArrayList_release(GblArrayList* pSelf, void** ppVecPtr, GblSize* pSize, GblSize* pCapacity) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->pCtx);
    GblBool stack = GBL_FALSE;
    GBL_API_VERIFY_POINTER(ppVecPtr);
    GBL_API_VERIFY_POINTER(pCapacity);
    GBL_API_VERIFY_POINTER(pSize);
    stack = GblArrayList_stack(pSelf);
    GBL_API_VERIFY(!stack, GBL_RESULT_ERROR_INVALID_OPERATION, "Cannot Take Stack Buffer!");
    *ppVecPtr = pSelf->pData;
    *pCapacity = pSelf->capacity;
    *pSize = pSelf->size;
    GblArrayList_initialize_(pSelf);
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblArrayList_acquire(GblArrayList* pSelf, void* pData, GblSize size, GblSize capacity) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->pCtx);
    GBL_API_VERIFY_POINTER(pData);
    GBL_API_VERIFY_ARG(capacity);
    GBL_API_VERIFY_ARG(size);
    GBL_API_CALL(GblArrayList_destruct(pSelf));
    pSelf->pData = (uint8_t*)pData;
    pSelf->capacity = capacity;
    pSelf->size = size;
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblArrayList_reserve(GblArrayList* pSelf, GblSize capacity) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->pCtx);

    if(pSelf->capacity < capacity) {
        if(capacity > 2 && capacity % 2) capacity = gblPow2Next(capacity);
        const GblBool stack = GblArrayList_stack(pSelf);
        if(stack) {
            const GblSize oldSize = pSelf->size;
            GBL_API_CALL(GblArrayList_alloc_(pSelf, capacity));
            memcpy(pSelf->pData, GblArrayList_stackBuffer(pSelf), oldSize * pSelf->elementSize);
            pSelf->size = oldSize;
        } else {
            pSelf->pData = GBL_API_REALLOC(pSelf->pData,
                                           (capacity + (pSelf->zeroTerminated? 1 : 0)) * pSelf->elementSize);
            pSelf->capacity = capacity;
        }
        if(pSelf->zeroTerminated) memset(&pSelf->pData[pSelf->size * pSelf->elementSize], 0, pSelf->elementSize);
    }
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblArrayList_resize(GblArrayList* pSelf, GblSize size) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->pCtx);
    if(size > pSelf->size) {

        //if(size+1 > pSelf->capacity) {
            GBL_API_CALL(GblArrayList_reserve(pSelf, size));
        //}

        //pSelf->data.pData[pSelf->data.length] = ' '; remove old nullcharacter so strlen isn't wrong!
    }

    if(pSelf->zeroTerminated)
        memset(&pSelf->pData[size * pSelf->elementSize], 0, pSelf->elementSize);

    pSelf->size = size;
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblArrayList_shrinkToFit(GblArrayList* pSelf) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->pCtx);
    const GblBool stack = GblArrayList_stack(pSelf);
    if(!stack && pSelf->capacity > pSelf->size) {
        const GblSize fitSize = pSelf->size + ((pSelf->zeroTerminated)? 1 : 0);
        const GblSize bytes = fitSize * pSelf->elementSize;
        if(fitSize <= pSelf->stackCapacity) {
            memcpy(GblArrayList_stackBuffer(pSelf), pSelf->pData, bytes);
            GBL_API_FREE(pSelf->pData);
            pSelf->pData = GblArrayList_stackBuffer(pSelf);
        } else {
            pSelf->pData = GBL_API_REALLOC(pSelf->pData, bytes);
        }
        pSelf->capacity = pSelf->size;
    }
    GBL_API_END();
}


GBL_EXPORT GBL_RESULT GblArrayList_append(GblArrayList* pSelf, const void* pData, GblSize elementCount) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->pCtx);
    GblArrayList_insert(pSelf, GblArrayList_size(pSelf), elementCount, pData);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblArrayList_prepend(GblArrayList* pSelf, const void* pData, GblSize elementCount) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->pCtx);
    GblArrayList_insert(pSelf, 0, elementCount, pData);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END();
}


GBL_EXPORT void* GblArrayList_insert(GblArrayList* pSelf, GblSize index, GblSize count, const void* pData) GBL_NOEXCEPT {
    uintptr_t insertionPoint = 0;
    GblSize slideSize = 0;
    void* pDataOut = NULL;
    GBL_API_BEGIN(pSelf->pCtx);
    GBL_API_VERIFY_ARG(count);
    GBL_API_VERIFY(index <= pSelf->size,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    slideSize = pSelf->size - index;
    GBL_API_CALL(GblArrayList_resize(pSelf, pSelf->size + count));
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


GBL_EXPORT GBL_RESULT GblArrayList_erase(GblArrayList* pSelf, GblSize begin, GblSize count) GBL_NOEXCEPT {
    GblSize lastIndex = 0;
    GblSize remainderCount  = 0;

    GBL_API_BEGIN(pSelf->pCtx);
    GBL_API_VERIFY(pSelf->size > 0, GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_API_VERIFY(begin < pSelf->size, GBL_RESULT_ERROR_OUT_OF_RANGE);
    lastIndex = begin + count - 1;
    GBL_API_VERIFY(lastIndex < pSelf->size, GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_API_VERIFY(begin <= lastIndex, GBL_RESULT_ERROR_OUT_OF_RANGE);
    if(!count) GBL_API_DONE();
    GBL_API_VERIFY(count > 0 && count <= pSelf->size, GBL_RESULT_ERROR_OUT_OF_RANGE);
    remainderCount = pSelf->size - 1 - lastIndex;
    if(remainderCount) memmove(&pSelf->pData[begin * pSelf->elementSize],
                               &pSelf->pData[(lastIndex+1) * pSelf->elementSize],
                               remainderCount * pSelf->elementSize);
    GBL_API_CALL(GblArrayList_resize(pSelf, pSelf->size - count));
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblArrayList_popFront(GblArrayList* pSelf, void* pOut) {
    GBL_API_BEGIN(pSelf->pCtx);
    if(pOut) memcpy(pOut, GblArrayList_at(pSelf, 0), GblArrayList_elementSize(pSelf));
    GBL_API_VERIFY_CALL(GblArrayList_erase(pSelf, 0, 1));
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblArrayList_popBack(GblArrayList* pSelf, void* pOut) {
    GBL_API_BEGIN(pSelf->pCtx);
    const GblSize index = GblArrayList_size(pSelf)-1;
    if(pOut) memcpy(pOut, GblArrayList_at(pSelf, index), GblArrayList_elementSize(pSelf));
    GBL_API_VERIFY_CALL(GblArrayList_erase(pSelf, index, 1));
    GBL_API_END();
}

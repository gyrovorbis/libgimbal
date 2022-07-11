#include <gimbal/containers/gimbal_vector.h>

GBL_INLINE void GblVector_initialize_(GblVector* pSelf) GBL_NOEXCEPT {
    GBL_ASSERT(pSelf->elementSize);
    pSelf->capacity = pSelf->stackSize / pSelf->elementSize;
    pSelf->pData = (uint8_t*)GblVector_stackBuffer(pSelf);
    pSelf->size = 0;
    if(pSelf->pData) pSelf->pData[0] = 0;
}


static GBL_RESULT GblVector_alloc_(GblVector* pSelf, GblSize capacity) GBL_NOEXCEPT {
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

GBL_EXPORT GBL_RESULT GblVector_destruct(GblVector* pSelf) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->pCtx);
   // Check if we have a buffer to free
   if(pSelf->pData && pSelf->pData != GblVector_stackBuffer(pSelf)) {
       if(pSelf->pCtx) GBL_API_FREE(pSelf->pData);
       else free(pSelf->pData);
       pSelf->pData = (uint8_t*)GblVector_stackBuffer(pSelf);
   }
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblVector_clear(GblVector* pSelf) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->pCtx);
    GBL_API_CALL(GblVector_destruct(pSelf));
    GblVector_initialize_(pSelf);
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblVector_assign(GblVector* pSelf, const void* pData, GblSize elementCount) GBL_NOEXCEPT {
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

GBL_EXPORT GBL_RESULT GblVector_construct_6(GblVector* pSelf, uint16_t elementSize, GblSize elementCount, const void* pInitialData, GblSize structSize, GblContext* pCtx) GBL_NOEXCEPT {
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

GBL_EXPORT GBL_RESULT GblVector_release(GblVector* pSelf, void** ppVecPtr, GblSize* pSize, GblSize* pCapacity) GBL_NOEXCEPT {
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

GBL_EXPORT GBL_RESULT GblVector_acquire(GblVector* pSelf, void* pData, GblSize size, GblSize capacity) GBL_NOEXCEPT {
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

GBL_EXPORT GBL_RESULT GblVector_reserve(GblVector* pSelf, GblSize capacity) GBL_NOEXCEPT {
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

GBL_EXPORT GBL_RESULT GblVector_resize(GblVector* pSelf, GblSize size) GBL_NOEXCEPT {
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

GBL_EXPORT GBL_RESULT GblVector_shrinkToFit(GblVector* pSelf) GBL_NOEXCEPT {
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


GBL_EXPORT GBL_RESULT GblVector_append(GblVector* pSelf, const void* pData, GblSize elementCount) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->pCtx);
    GblVector_insert(pSelf, GblVector_size(pSelf), elementCount, pData);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblVector_prepend(GblVector* pSelf, const void* pData, GblSize elementCount) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->pCtx);
    GblVector_insert(pSelf, 0, elementCount, pData);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END();
}


GBL_EXPORT void* GblVector_insert(GblVector* pSelf, GblSize index, GblSize count, const void* pData) GBL_NOEXCEPT {
    uintptr_t insertionPoint = 0;
    GblSize slideSize = 0;
    void* pDataOut = NULL;
    GBL_API_BEGIN(pSelf->pCtx);
    GBL_API_VERIFY_POINTER(pData);
    GBL_API_VERIFY_ARG(count);
    GBL_API_VERIFY(index <= pSelf->size,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
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


GBL_EXPORT GBL_RESULT GblVector_erase(GblVector* pSelf, GblSize begin, GblSize count) GBL_NOEXCEPT {
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
    GBL_API_CALL(GblVector_resize(pSelf, pSelf->size - count));
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblVector_popFront(GblVector* pSelf, void* pOut) {
    GBL_API_BEGIN(pSelf->pCtx);
    if(pOut) memcpy(pOut, GblVector_at(pSelf, 0), GblVector_elementSize(pSelf));
    GBL_API_VERIFY_CALL(GblVector_erase(pSelf, 0, 1));
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblVector_popBack(GblVector* pSelf, void* pOut) {
    GBL_API_BEGIN(pSelf->pCtx);
    const GblSize index = GblVector_size(pSelf)-1;
    if(pOut) memcpy(pOut, GblVector_at(pSelf, index), GblVector_elementSize(pSelf));
    GBL_API_VERIFY_CALL(GblVector_erase(pSelf, index, 1));
    GBL_API_END();
}

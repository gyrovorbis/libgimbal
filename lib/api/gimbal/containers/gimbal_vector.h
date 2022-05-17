#ifndef GIMBAL_VECTOR_H
#define GIMBAL_VECTOR_H

#include "../core/gimbal_api_frame.h"
//#include "../types/gimbal_buffer.h"
#include "../algorithms/gimbal_numeric.h"

#define SELF    GblVector* pSelf
#define CSELF   const SELF

GBL_DECLS_BEGIN


#define GBL_VECTOR_DEFAULT_VALUE ({ GBL_HANDLE_INVALID, NULL, 0, 0, 0, 0, {0} })

/*
 * use header trick to put metadata above vector allocation
 * return void* which points to actual buffer data
 *
 * make API stop returning result and push errors onto Context Lastresult
 */

typedef struct GblVector {
    GblContext*     pCtx;
    char*           pBuffer;
    uint16_t        size;
    uint16_t        capacity;
    uint16_t        elementSize;
    uint16_t        stackSize;
    char            stackBuffer[1]; // delete me eventually
} GblVector;


GBL_INLINE GBL_API GblVector_construct_6 (SELF, GblContext* pCtx, GblSize elementSize,
                                         GblSize structSize, const void* pInitialData,
                                         GblSize elementCount)                                              GBL_NOEXCEPT;
GBL_INLINE GBL_API GblVector_construct_4 (SELF, GblContext* pCtx, GblSize elementSize, GblSize structSize)  GBL_NOEXCEPT;
GBL_INLINE GBL_API GblVector_construct_3 (SELF, GblContext* pCtx, GblSize elementSize)                      GBL_NOEXCEPT;
#define            GblVector_construct(...) \
                        GBL_VA_OVERLOAD_SELECT(GblVector_construct, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)(__VA_ARGS__)
GBL_INLINE GBL_API GblVector_destruct    (SELF)                                                             GBL_NOEXCEPT;
GBL_INLINE GBL_API GblVector_assign      (SELF, const void* pData, GblSize elementCount)                    GBL_NOEXCEPT;
GBL_INLINE GBL_API GblVector_take        (SELF, void** ppVecPtr, GblSize* pSize, GblSize* pCapacity)        GBL_NOEXCEPT;
GBL_INLINE GBL_API GblVector_give        (SELF, void* pData, GblSize size, GblSize capacity)                GBL_NOEXCEPT;

GBL_INLINE GBL_API GblVector_context     (CSELF, GblContext** pCtx)                                         GBL_NOEXCEPT;
GBL_INLINE GBL_API GblVector_stackBytes  (CSELF, GblSize* pSize)                                            GBL_NOEXCEPT;
GBL_INLINE GBL_API GblVector_size        (CSELF, GblSize* pSize)                                            GBL_NOEXCEPT;
GBL_INLINE GBL_API GblVector_capacity    (CSELF, GblSize* pCapacity)                                        GBL_NOEXCEPT;
GBL_INLINE GBL_API GblVector_elementSize (CSELF, GblSize* pElemSize)                                        GBL_NOEXCEPT;
GBL_INLINE GBL_API GblVector_data        (CSELF, const void** ppData)                                       GBL_NOEXCEPT;

GBL_INLINE GBL_API GblVector_empty       (CSELF, GblBool* pResult)                                          GBL_NOEXCEPT;
GBL_INLINE GBL_API GblVector_stack       (CSELF, GblBool* pResult)                                          GBL_NOEXCEPT;

GBL_INLINE GBL_API GblVector_at          (CSELF, GblSize index, void** ppEntry)                             GBL_NOEXCEPT;
GBL_INLINE GBL_API GblVector_front       (CSELF, void** ppEntry)                                            GBL_NOEXCEPT;
GBL_INLINE GBL_API GblVector_back        (CSELF, void** ppEntry)                                            GBL_NOEXCEPT;

GBL_INLINE GBL_API GblVector_insert      (SELF, GblSize index, GblSize count,
                                         const void* pDataIn, void** ppDataOut)                             GBL_NOEXCEPT;
GBL_INLINE GBL_API GblVector_pushFront   (SELF, const void* pData)                                          GBL_NOEXCEPT;
GBL_INLINE GBL_API GblVector_pushBack    (SELF, const void* pData)                                          GBL_NOEXCEPT;
GBL_INLINE GBL_API GblVector_append      (SELF, const void* pData, GblSize elementCount)                    GBL_NOEXCEPT;

GBL_INLINE GBL_API GblVector_erase       (SELF, GblSize begin, GblSize count)                               GBL_NOEXCEPT;
GBL_INLINE GBL_API GblVector_clear       (SELF)                                                             GBL_NOEXCEPT;

GBL_INLINE GBL_API GblVector_reserve     (SELF, GblSize capacity)                                           GBL_NOEXCEPT;
GBL_INLINE GBL_API GblVector_resize      (SELF, GblSize size)                                               GBL_NOEXCEPT;
GBL_INLINE GBL_API GblVector_shrinkToFit (SELF)                                                             GBL_NOEXCEPT;








// ========== INLINE IMPLEMENTATION ==========

//IMPLEMENT ME FOR CUSTOM ALLOCATION SCHEMES
GBL_INLINE GblSize GBL_VECTOR_CAPACITY_FROM_SIZE_(GblSize size) GBL_NOEXCEPT {
    return gblPow2Next(size);
}

GBL_INLINE GBL_API GblVector_empty(const GblVector* pVec, GblBool* pResult) GBL_NOEXCEPT  {
    GBL_ASSERT(pVec);
    GBL_ASSERT(pResult);
    *pResult = (!pVec->size);
    return GBL_RESULT_SUCCESS;
}

GBL_INLINE GBL_API GblVector_stack(const GblVector* pVec, GblBool* pResult) GBL_NOEXCEPT {
    GBL_ASSERT(pVec);
    GBL_ASSERT(pResult);
    *pResult = (pVec->pBuffer == pVec->stackBuffer);
    return GBL_RESULT_SUCCESS;
}

GBL_INLINE void gblVectorInitialize_(GblVector* pVec) GBL_NOEXCEPT {
    GBL_ASSERT(pVec->elementSize);
    pVec->capacity = pVec->stackSize / pVec->elementSize;
    pVec->pBuffer = pVec->stackBuffer;
    pVec->size = 0;
    pVec->stackBuffer[0] = 0;
}

GBL_INLINE GBL_API GblVector_destruct(GblVector* pVec) GBL_NOEXCEPT {
    GBL_API_BEGIN(pVec->pCtx);
   // Check if we have a buffer to free
   if(pVec->pBuffer && pVec->pBuffer != pVec->stackBuffer) {
       if(pVec->pCtx) GBL_API_FREE(pVec->pBuffer);
       else free(pVec->pBuffer);
       pVec->pBuffer = pVec->stackBuffer;
   }
    GBL_API_END();
}

GBL_INLINE GBL_API GblVector_clear(GblVector* pVec) GBL_NOEXCEPT {
    GBL_API_BEGIN(pVec->pCtx);
    GBL_API_CALL(GblVector_destruct(pVec));
    gblVectorInitialize_(pVec);
    GBL_API_END();
}
#if 0
GBL_INLINE GBL_API GblVector_clear(GblVector* pVec) GBL_NOEXCEPT {
    GBL_TEST_CASE_API_BLOCKZ(pVec->pCtx, ({
        GBL_API_CALL(GblVector_destruct(pVec));
        gblVectorInitialize_(pVec);
    }));
}
#endif

GBL_INLINE GBL_API gblVectorAlloc_(GblVector* pVec, GblSize capacity) GBL_NOEXCEPT {
    GBL_API_BEGIN(pVec->pCtx);

    if(capacity * pVec->elementSize <= pVec->stackSize) {
        pVec->pBuffer = pVec->stackBuffer;
        pVec->capacity = pVec->stackSize / pVec->elementSize;
    } else {
        GblSize allocSize = capacity * pVec->elementSize;
        if(allocSize < GBL_ALIGNOF(GBL_MAX_ALIGN_T)) {
            allocSize = gblPow2Next(GBL_ALIGNOF(GBL_MAX_ALIGN_T));
        }
        pVec->capacity = allocSize / pVec->elementSize;
        pVec->pBuffer = pVec->pCtx != GBL_NULL?
                    (char*)GBL_API_MALLOC(allocSize) :
                    (char*)malloc(allocSize);
    }
    pVec->pBuffer[0] = 0;
    pVec->size = 0;
    GBL_API_END();
}

GBL_INLINE GBL_API GblVector_assign(GblVector* pVec, const void* pData, GblSize elementCount) GBL_NOEXCEPT {
    GBL_API_BEGIN(pVec->pCtx);
    if(pVec->capacity < elementCount + 1) {
        GBL_API_CALL(GblVector_clear(pVec));
        if(elementCount) GBL_API_CALL(gblVectorAlloc_(pVec, GBL_VECTOR_CAPACITY_FROM_SIZE_(pVec->elementSize * elementCount + 1)));
    }

    if(pData && elementCount) {
        memcpy(pVec->pBuffer, pData, elementCount * pVec->elementSize);
    }
    pVec->size = elementCount;
    pVec->pBuffer[pVec->size * pVec->elementSize] = 0;
    GBL_API_END();
}

GBL_INLINE GBL_API GblVector_construct_6(GblVector* pVector, GblContext* pCtx, GblSize elementSize, GblSize structSize, const void* pInitialData, GblSize elementCount) GBL_NOEXCEPT {
    GBL_API_BEGIN(pCtx);
    GBL_API_VERIFY_POINTER(pVector);
    GBL_API_VERIFY_ARG(structSize >= sizeof(GblVector));
    memset(pVector, 0, structSize);
    pVector->pCtx           = pCtx;
    pVector->stackSize      = structSize - sizeof(GblVector) + GBL_VECTOR_STACK_BUFFER_DEFAULT_SIZE;
    pVector->elementSize    = elementSize;
    gblVectorInitialize_(pVector);
    GBL_API_CALL(GblVector_assign(pVector, pInitialData, elementCount));
    GBL_API_END();
}

GBL_INLINE GBL_API GblVector_construct_4(GblVector* pVector, GblContext* pCtx, GblSize elementSize, GblSize structSize) GBL_NOEXCEPT {
    return GblVector_construct_6(pVector, pCtx, elementSize, structSize, NULL, 0);
}

GBL_INLINE GBL_API GblVector_construct_3(GblVector* pVector, GblContext* pCtx, GblSize elementSize) GBL_NOEXCEPT {
    return GblVector_construct_4(pVector, pCtx, elementSize, sizeof(GblVector));
}

GBL_INLINE GBL_API GblVector_take(GblVector* pVec, void** ppVecPtr, GblSize* pSize, GblSize* pCapacity) GBL_NOEXCEPT {
    GBL_API_BEGIN(pVec->pCtx);
    GblBool stack = GBL_FALSE;
    GBL_API_VERIFY_POINTER(ppVecPtr);
    GBL_API_VERIFY_POINTER(pCapacity);
    GBL_API_VERIFY_POINTER(pSize);
    GBL_API_CALL(GblVector_stack(pVec, &stack));
    GBL_API_VERIFY(!stack, GBL_RESULT_ERROR_INVALID_OPERATION, "Cannot Take Stack Buffer!");
    *ppVecPtr = pVec->pBuffer;
    *pCapacity = pVec->capacity;
    *pSize = pVec->size;
    gblVectorInitialize_(pVec);
    GBL_API_END();
}

GBL_INLINE GBL_API GblVector_give(GblVector* pVec, void* pData, GblSize size, GblSize capacity) GBL_NOEXCEPT {
    GBL_API_BEGIN(pVec->pCtx);
    GBL_API_VERIFY_POINTER(pData);
    GBL_API_VERIFY_ARG(capacity);
    GBL_API_VERIFY_ARG(size);
    GblVector_destruct(pVec);
    pVec->pBuffer = (char*)pData;
    pVec->capacity = capacity;
    pVec->size = size;
    GBL_API_END();
}

GBL_INLINE GBL_API GblVector_context(const GblVector* pVec, GblContext** pCtx) GBL_NOEXCEPT {
    GBL_API_BEGIN(pVec->pCtx);
    GBL_API_VERIFY_POINTER(pCtx);
    *pCtx = pVec->pCtx;
    GBL_API_END();
}

GBL_INLINE GBL_API GblVector_stackBytes(const GblVector* pVec, GblSize* pSize) GBL_NOEXCEPT {
    GBL_API_BEGIN(pVec->pCtx);
    GBL_API_VERIFY_POINTER(pSize);
    *pSize = pVec->stackSize;
    GBL_API_END();
}

GBL_INLINE GBL_API GblVector_size(const GblVector* pVec, GblSize* pSize) GBL_NOEXCEPT {
    GBL_API_BEGIN(pVec->pCtx);
    GBL_API_VERIFY_POINTER(pSize);
    *pSize = pVec->size;
    GBL_API_END();
}

GBL_INLINE GBL_API GblVector_capacity(const GblVector* pVec, GblSize* pCapacity) GBL_NOEXCEPT {
    GBL_API_BEGIN(pVec->pCtx);
    GBL_API_VERIFY_POINTER(pCapacity);
    *pCapacity = pVec->capacity;
    GBL_API_END();
}

GBL_INLINE GBL_API GblVector_elementSize(const GblVector* pVec, GblSize* pElemSize) GBL_NOEXCEPT {
    GBL_API_BEGIN(pVec->pCtx);
    GBL_API_VERIFY_POINTER(pElemSize);
    *pElemSize = pVec->elementSize;
    GBL_API_END();
}

GBL_INLINE GBL_API GblVector_data(const GblVector* pVec, const void** ppData) GBL_NOEXCEPT {
    GBL_API_BEGIN(pVec->pCtx);
    GBL_API_VERIFY_POINTER(ppData);
    *ppData = pVec->pBuffer;
    GBL_API_END();
}

GBL_INLINE GBL_API GblVector_reserve(GblVector* pVec, GblSize capacity) GBL_NOEXCEPT {
    GBL_API_BEGIN(pVec->pCtx);
    if(pVec->capacity < capacity) {
        GblBool stack = GBL_FALSE;
        GBL_API_CALL(GblVector_stack(pVec, &stack));
        if(stack) {
            const GblSize oldSize = pVec->size;
            GBL_API_CALL(gblVectorAlloc_(pVec, GBL_VECTOR_CAPACITY_FROM_SIZE_(capacity * pVec->elementSize + 1)));
            memcpy(pVec->pBuffer, pVec->stackBuffer, oldSize * pVec->elementSize);
            pVec->size = oldSize;
        } else {
            pVec->pBuffer = pVec->pCtx != GBL_NULL?
                        (char*)GBL_API_REALLOC(pVec->pBuffer, capacity * pVec->elementSize) :
                        (char*)realloc(pVec->pBuffer, capacity * pVec->elementSize);
            pVec->capacity = capacity;
        }
        pVec->pBuffer[pVec->size * pVec->elementSize] = 0;
    }
    GBL_API_END();
}

GBL_INLINE GBL_API GblVector_resize(GblVector* pVec, GblSize size) GBL_NOEXCEPT {
    GBL_API_BEGIN(pVec->pCtx);
    if(size > pVec->size) {

        if(size+1 > pVec->capacity) {
            GBL_API_CALL(GblVector_reserve(pVec, GBL_VECTOR_CAPACITY_FROM_SIZE_(size + 1)));
        }

        //pVec->data.pBuffer[pVec->data.length] = ' '; remove old nullcharacter so strlen isn't wrong!
    }

    pVec->pBuffer[size * pVec->elementSize] = 0;
    pVec->size = size;
    GBL_API_END();
}

GBL_INLINE GBL_API GblVector_shrinkToFit(GblVector* pVec) GBL_NOEXCEPT {
    GBL_API_BEGIN(pVec->pCtx);
    GblBool stack = GBL_FALSE;
    GBL_API_CALL(GblVector_stack(pVec, &stack));
    if(!stack && pVec->capacity > pVec->size) {
        pVec->pBuffer = pVec->pCtx != GBL_NULL?
                    (char*)GBL_API_REALLOC(pVec->pBuffer, pVec->size * pVec->elementSize) :
                    (char*)realloc(pVec->pBuffer, pVec->size * pVec->elementSize);
        pVec->capacity = pVec->size;
    }
    GBL_API_END();
}


GBL_INLINE GBL_API GblVector_append(GblVector* pVec, const void* pData, GblSize elementCount) GBL_NOEXCEPT {
    GBL_API_BEGIN(pVec->pCtx);
    GblSize expectedSize = 0;
    GBL_API_VERIFY_POINTER(pData);
    GBL_API_VERIFY_ARG(elementCount);
    GBL_API_CALL(GblVector_size(pVec, &expectedSize));
    expectedSize += elementCount;
    GBL_API_CALL(GblVector_reserve(pVec, expectedSize + 1));
    memcpy(&pVec->pBuffer[pVec->size * pVec->elementSize], pData, elementCount * pVec->elementSize);
    pVec->size = expectedSize;
    pVec->pBuffer[pVec->size * pVec->elementSize] = 0;
    GBL_API_END();
}

GBL_INLINE GBL_API GblVector_at(const GblVector* pVector, GblSize index, void** ppEntry) GBL_NOEXCEPT {
    GBL_API_BEGIN(pVector->pCtx);
    GBL_API_VERIFY_POINTER(ppEntry);
    GBL_API_VERIFY_ARG(index < pVector->size);
    *ppEntry = &pVector->pBuffer[index * pVector->elementSize];
    GBL_API_END();
}

GBL_INLINE GBL_API GblVector_front(const GblVector* pVector, void** ppEntry) GBL_NOEXCEPT {
    return GblVector_at(pVector, 0, ppEntry);
}

GBL_INLINE GBL_API GblVector_back(const GblVector* pVector, void** ppEntry) GBL_NOEXCEPT {
    return GblVector_at(pVector, pVector->size-1, ppEntry);
}


// HANDLES DYNAMIC REALLOCATION
GBL_INLINE GBL_API GblVector_insert(GblVector* pVector, GblSize index, GblSize count, const void* pDataIn, void** ppDataOut) GBL_NOEXCEPT {
    uintptr_t insertionPoint = 0;
    GblSize slideSize = 0;
    GBL_API_BEGIN(pVector->pCtx);
    GBL_API_VERIFY_ARG(index <= pVector->size);
    GBL_API_CALL(GblVector_resize(pVector, pVector->size + count));
    slideSize = (pVector->size-1) - index;
    insertionPoint = (uintptr_t)pVector->pBuffer + index * pVector->elementSize;
    if(slideSize) memmove((char*)insertionPoint + pVector->elementSize * count, (const char*)insertionPoint, slideSize * pVector->elementSize);
    if(pDataIn) {
        memcpy((void*)insertionPoint, pDataIn, pVector->elementSize * count);
    }
    if(ppDataOut) {
        *ppDataOut = (void*)insertionPoint;
    }
    GBL_API_END();
}

// SHOULD BE THE SAME THING AS INSERT
GBL_INLINE GBL_API GblVector_pushBack(GblVector* pVector, const void* pData) GBL_NOEXCEPT {
    GblSize size = 0;
    GBL_API_BEGIN(pVector->pCtx);
    GBL_API_CALL(GblVector_size(pVector, &size));
    GBL_API_CALL(GblVector_insert(pVector, size, 1, pData, NULL));
    GBL_API_END();
}

GBL_INLINE GBL_API GblVector_pushFront(GblVector* pVector, const void* pData) GBL_NOEXCEPT {
    GBL_API_BEGIN(pVector->pCtx);
    GBL_API_CALL(GblVector_insert(pVector, 0, 1, pData, NULL));
    GBL_API_END();
}

GBL_INLINE GBL_API GblVector_erase(GblVector* pVector, GblSize begin, GblSize count) GBL_NOEXCEPT {
    GblSize lastIndex = 0;
    GblSize remainderCount  = 0;
    GBL_API_BEGIN(pVector->pCtx);
    GBL_API_VERIFY_EXPRESSION(pVector->size > 0);
    GBL_API_VERIFY_ARG(begin < pVector->size);
    lastIndex = begin + count - 1;
    GBL_API_VERIFY_ARG(lastIndex < pVector->size);
    GBL_API_VERIFY_ARG(begin <= lastIndex);
    GBL_API_VERIFY_EXPRESSION(count > 0 && count <= pVector->size);
    remainderCount = pVector->size - 1 - lastIndex;
    if(remainderCount) memmove(&pVector->pBuffer[begin * pVector->elementSize], &pVector->pBuffer[(lastIndex+1) * pVector->elementSize], remainderCount * pVector->elementSize);
    GBL_API_CALL(GblVector_resize(pVector, pVector->size - count));
    GBL_API_END();
}

#if 0
GBL_INLINE GBL_API gblVectorSort(GblVector* pVector, GblSortFn pFnSorter) {
    pSorter();
}
#endif

/*
#if 0
GBL_API_INLINE_HELPER((GblSize, 0), VECTOR_SIZE, (const GblVector* pVector), ({
    GBL_API_CALL(GblVector_size(pVector, &GBL_API_INLINE_RETVAL()));
}));

GBL_API_INLINE_HELPER((GblSize, 0), VECTOR_CAPACITY, (const GblVector* pVector), {
    GBL_API_CALL(GblVector_capacity(pVector, &GBL_API_INLINE_RETVAL()));
});

GBL_API_INLINE_HELPER((const void*, NULL), VECTOR_DATA, (const GblVector* pVector), {
    GBL_API_CALL(GblVector_data(pVector, &GBL_API_INLINE_RETVAL()));
});

GBL_API_INLINE_HELPER((void*, NULL), VECTOR_AT, (const GblVector* pVector, GblSize index), {
    GBL_API_CALL(GblVector_at(pVector, index, &GBL_API_INLINE_RETVAL()));
});

GBL_API_INLINE_HELPER((void*, NULL), VECTOR_FRONT, (const GblVector* pVector), {
    GBL_API_CALL(GblVector_front(pVector, &GBL_API_INLINE_RETVAL()));
});

GBL_API_INLINE_HELPER((void*, NULL), VECTOR_BACK, (const GblVector* pVector), {
    GBL_API_CALL(GblVector_back(pVector, &GBL_API_INLINE_RETVAL()));
});

GBL_API_INLINE_HELPER((GblBool, GBL_FALSE), VECTOR_PUSH_BACK, (GblVector* pVector, const void* pValue), {
    GBL_API_CALL(GblVector_pushBack(pVector, pValue));
    GBL_API_INLINE_RETVAL() = GBL_RESULT_SUCCESS(GBL_API_RESULT());
});

GBL_API_INLINE_HELPER((GblBool, GBL_FALSE), VECTOR_PUSH_FRONT, (GblVector* pVector, const void* pValue), {
    GBL_API_CALL(GblVector_pushFront(pVector, pValue));
});

GBL_API_INLINE_HELPER((GblSize, 0), VECTOR_ELEMENT_SIZE, (const GblVector* pVector), {
    GBL_API_CALL(GblVector_elementSize(pVector, &GBL_API_INLINE_RETVAL()));
});

GBL_API_INLINE_HELPER((GblSize, 0), VECTOR_STACK_BYTES, (const GblVector* pVector), {
    GBL_API_CALL(GblVector_stackBytes(pVector, &GBL_API_INLINE_RETVAL()));
});

GBL_API_INLINE_HELPER((GblBool, GBL_TRUE), VECTOR_IS_STACK, (const GblVector* pVector), {
    GBL_API_CALL(GblVector_stack(pVector, &GBL_API_INLINE_RETVAL()));
});

GBL_API_INLINE_HELPER((GblBool, GBL_TRUE), VECTOR_IS_EMPTY, (const GblVector* pVector), {
    GBL_API_CALL(GblVector_empty(pVector, &GBL_API_INLINE_RETVAL()));
});

GBL_API_INLINE_HELPER((GblContext, GBL_HANDLE_INVALID), VECTOR_CONTEXT, (const GblVector* pVector), {
    GBL_API_CALL(GblVector_context(pVector, &GBL_API_INLINE_RETVAL()));
});

// COPY + MOVE + ASSIGN

GBL_API_INLINE_HELPER((GblVector, {0), VECTOR_CREATE, (GblContext pCtx, GblSize elementSize, GblSize structSize, const void* pInitialData, GblSize elementCount), {
    GBL_API_CALL(GblVector_construct_6(&GBL_API_INLINE_RETVAL(), pCtx, elementSize, structSize, pInitialData, elementCount));
});

GBL_API_INLINE_HELPER((GblBool, GBL_FALSE), VECTOR_DESTROY, (GblVector* pVector), {
    GBL_API_CALL(GblVector_destruct(pVector));
    GBL_API_INLINE_RETVAL() = GBL_RESULT_SUCCESS(GBL_API_RESULT());
});

#endif */


#undef CSELF
#undef SELF

GBL_DECLS_END

#endif // GIMBAL_VECTOR_H

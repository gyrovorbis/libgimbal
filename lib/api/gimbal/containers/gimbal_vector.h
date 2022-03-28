#ifndef GIMBAL_VECTOR_H
#define GIMBAL_VECTOR_H

#include "../core/gimbal_api_frame.h"
//#include "../types/gimbal_buffer.h"
#include "../algorithms/gimbal_numeric.h"

#define SELF    GblVector* pSelf
#define CSELF   const SELF

GBL_DECLS_BEGIN


#define GBL_VECTOR_DEFAULT_VALUE ({ GBL_HANDLE_INVALID, NULL, 0, 0, 0, 0, {0} })

typedef struct GblVector {
    GblContext      hCtx;
    char*           pBuffer;
    GblSize         size;
    GblSize         capacity;
    GblSize         elementSize;
    GblSize         stackSize;
    char            stackBuffer[GBL_VECTOR_STACK_BUFFER_DEFAULT_SIZE];
} GblVector;


GBL_INLINE GBL_API gblVectorConstruct_6 (SELF, GblContext hCtx, GblSize elementSize,
                                         GblSize structSize, const void* pInitialData,
                                         GblSize elementCount)                                              GBL_NOEXCEPT;
GBL_INLINE GBL_API gblVectorConstruct_4 (SELF, GblContext hCtx, GblSize elementSize, GblSize structSize)    GBL_NOEXCEPT;
GBL_INLINE GBL_API gblVectorConstruct_3 (SELF, GblContext hCtx, GblSize elementSize) GBL_NOEXCEPT;
#define            gblVectorConstruct(...) \
                        GBL_VA_OVERLOAD_SELECT(gblVectorConstruct, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)(__VA_ARGS__)
GBL_INLINE GBL_API gblVectorDestruct    (SELF)                                                              GBL_NOEXCEPT;
GBL_INLINE GBL_API gblVectorAssign      (SELF, const void* pData, GblSize elementCount)                     GBL_NOEXCEPT;
GBL_INLINE GBL_API gblVectorTake        (SELF, void** ppVecPtr, GblSize* pSize, GblSize* pCapacity)         GBL_NOEXCEPT;
GBL_INLINE GBL_API gblVectorGive        (SELF, void* pData, GblSize size, GblSize capacity)                 GBL_NOEXCEPT;

GBL_INLINE GBL_API gblVectorContext     (CSELF, GblContext* pCtx)                                           GBL_NOEXCEPT;
GBL_INLINE GBL_API gblVectorStackBytes  (CSELF, GblSize* pSize)                                             GBL_NOEXCEPT;
GBL_INLINE GBL_API gblVectorSize        (CSELF, GblSize* pSize)                                             GBL_NOEXCEPT;
GBL_INLINE GBL_API gblVectorCapacity    (CSELF, GblSize* pCapacity)                                         GBL_NOEXCEPT;
GBL_INLINE GBL_API gblVectorElementSize (CSELF, GblSize* pElemSize)                                         GBL_NOEXCEPT;
GBL_INLINE GBL_API gblVectorData        (CSELF, const void** ppData)                                        GBL_NOEXCEPT;

GBL_INLINE GBL_API gblVectorIsEmpty     (CSELF, GblBool* pResult)                                           GBL_NOEXCEPT;
GBL_INLINE GBL_API gblVectorIsStack     (CSELF, GblBool* pResult)                                           GBL_NOEXCEPT;

GBL_INLINE GBL_API gblVectorAt          (CSELF, GblSize index, void** ppEntry)                              GBL_NOEXCEPT;
GBL_INLINE GBL_API gblVectorFront       (CSELF, void** ppEntry)                                             GBL_NOEXCEPT;
GBL_INLINE GBL_API gblVectorBack        (CSELF, void** ppEntry)                                             GBL_NOEXCEPT;

GBL_INLINE GBL_API gblVectorInsert      (SELF, GblSize index, GblSize count,
                                         const void* pDataIn, void** ppDataOut)                             GBL_NOEXCEPT;
GBL_INLINE GBL_API gblVectorPushFront   (SELF, const void* pData)                                           GBL_NOEXCEPT;
GBL_INLINE GBL_API gblVectorPushBack    (SELF, const void* pData)                                           GBL_NOEXCEPT;
GBL_INLINE GBL_API gblVectorConcat      (SELF, const void* pData, GblSize elementCount)                     GBL_NOEXCEPT;

GBL_INLINE GBL_API gblVectorErase       (SELF, GblSize begin, GblSize count)                                GBL_NOEXCEPT;
GBL_INLINE GBL_API gblVectorClear       (SELF)                                                              GBL_NOEXCEPT;

GBL_INLINE GBL_API gblVectorReserve     (SELF, GblSize capacity)                                            GBL_NOEXCEPT;
GBL_INLINE GBL_API gblVectorResize      (SELF, GblSize size)                                                GBL_NOEXCEPT;
GBL_INLINE GBL_API gblVectorShrinkToFit (SELF)                                                              GBL_NOEXCEPT;








// ========== INLINE IMPLEMENTATION ==========

//IMPLEMENT ME FOR CUSTOM ALLOCATION SCHEMES
GBL_INLINE GblSize GBL_VECTOR_CAPACITY_FROM_SIZE_(GblSize size) GBL_NOEXCEPT {
    return gblPow2Next(size);
}

GBL_INLINE GBL_API gblVectorIsEmpty(const GblVector* pVec, GblBool* pResult) GBL_NOEXCEPT  {
    GBL_ASSERT(pVec);
    GBL_ASSERT(pResult);
    *pResult = (!pVec->size);
    return GBL_RESULT_SUCCESS;
}

GBL_INLINE GBL_API gblVectorIsStack(const GblVector* pVec, GblBool* pResult) GBL_NOEXCEPT {
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

GBL_INLINE GBL_API gblVectorDestruct(GblVector* pVec) GBL_NOEXCEPT {
    GBL_API_FUNCTION(pVec->hCtx, ({
       // Check if we have a buffer to free
       if(pVec->pBuffer && pVec->pBuffer != pVec->stackBuffer) {
           if(pVec->hCtx) GBL_API_FREE(pVec->pBuffer);
           else free(pVec->pBuffer);
           pVec->pBuffer = pVec->stackBuffer;
       }
    }));
}

GBL_INLINE GBL_API gblVectorClear(GblVector* pVec) GBL_NOEXCEPT {
    GBL_API_BEGIN(pVec->hCtx);
    GBL_API_CALL(gblVectorDestruct(pVec));
    gblVectorInitialize_(pVec);
    GBL_API_END();
}
#if 0
GBL_INLINE GBL_API gblVectorClear(GblVector* pVec) GBL_NOEXCEPT {
    GBL_TEST_CASE_API_BLOCKZ(pVec->hCtx, ({
        GBL_API_CALL(gblVectorDestruct(pVec));
        gblVectorInitialize_(pVec);
    }));
}
#endif

GBL_INLINE GBL_API gblVectorAlloc_(GblVector* pVec, GblSize capacity) GBL_NOEXCEPT {
    GBL_API_BEGIN(pVec->hCtx);

    if(capacity * pVec->elementSize <= pVec->stackSize) {
        pVec->pBuffer = pVec->stackBuffer;
        pVec->capacity = pVec->stackSize / pVec->elementSize;
    } else {
        GblSize allocSize = capacity * pVec->elementSize;
        if(allocSize < GBL_ALIGNOF(max_align_t)) {
            allocSize = gblPow2Next(GBL_ALIGNOF(max_align_t));
        }
        pVec->capacity = allocSize / pVec->elementSize;
        pVec->pBuffer = pVec->hCtx != GBL_HANDLE_INVALID?
                    (char*)GBL_API_MALLOC(allocSize) :
                    (char*)malloc(allocSize);
    }
    pVec->pBuffer[0] = 0;
    pVec->size = 0;
    GBL_API_END();
}

GBL_INLINE GBL_API gblVectorAssign(GblVector* pVec, const void* pData, GblSize elementCount) GBL_NOEXCEPT {
    GBL_API_BEGIN(pVec->hCtx);
    if(pVec->capacity < elementCount + 1) {
        GBL_API_CALL(gblVectorClear(pVec));
        if(elementCount) GBL_API_CALL(gblVectorAlloc_(pVec, GBL_VECTOR_CAPACITY_FROM_SIZE_(pVec->elementSize * elementCount + 1)));
    }

    if(pData && elementCount) {
        memcpy(pVec->pBuffer, pData, elementCount * pVec->elementSize);
    }
    pVec->size = elementCount;
    pVec->pBuffer[pVec->size * pVec->elementSize] = 0;
    GBL_API_END();
}

GBL_INLINE GBL_API gblVectorConstruct_6(GblVector* pVector, GblContext hCtx, GblSize elementSize, GblSize structSize, const void* pInitialData, GblSize elementCount) GBL_NOEXCEPT {
    GBL_API_BEGIN(hCtx);
    GBL_API_VERIFY_POINTER(pVector);
    GBL_API_VERIFY_ARG(structSize >= sizeof(GblVector));
    memset(pVector, 0, structSize);
    pVector->hCtx           = hCtx;
    pVector->stackSize      = structSize - sizeof(GblVector) + GBL_VECTOR_STACK_BUFFER_DEFAULT_SIZE;
    pVector->elementSize    = elementSize;
    gblVectorInitialize_(pVector);
    GBL_API_CALL(gblVectorAssign(pVector, pInitialData, elementCount));
    GBL_API_END();
}

GBL_INLINE GBL_API gblVectorConstruct_4(GblVector* pVector, GblContext hCtx, GblSize elementSize, GblSize structSize) GBL_NOEXCEPT {
    return gblVectorConstruct_6(pVector, hCtx, elementSize, structSize, NULL, 0);
}

GBL_INLINE GBL_API gblVectorConstruct_3(GblVector* pVector, GblContext hCtx, GblSize elementSize) GBL_NOEXCEPT {
    return gblVectorConstruct_4(pVector, hCtx, elementSize, sizeof(GblVector));
}

GBL_INLINE GBL_API gblVectorTake(GblVector* pVec, void** ppVecPtr, GblSize* pSize, GblSize* pCapacity) GBL_NOEXCEPT {
    GBL_API_BEGIN(pVec->hCtx);
    GblBool stack = GBL_FALSE;
    GBL_API_VERIFY_POINTER(ppVecPtr);
    GBL_API_VERIFY_POINTER(pCapacity);
    GBL_API_VERIFY_POINTER(pSize);
    GBL_API_CALL(gblVectorIsStack(pVec, &stack));
    GBL_API_VERIFY(!stack, GBL_RESULT_ERROR_INVALID_OPERATION, "Cannot Take Stack Buffer!");
    *ppVecPtr = pVec->pBuffer;
    *pCapacity = pVec->capacity;
    *pSize = pVec->size;
    gblVectorInitialize_(pVec);
    GBL_API_END();
}

GBL_INLINE GBL_API gblVectorGive(GblVector* pVec, void* pData, GblSize size, GblSize capacity) GBL_NOEXCEPT {
    GBL_API_BEGIN(pVec->hCtx);
    GBL_API_VERIFY_POINTER(pData);
    GBL_API_VERIFY_ARG(capacity);
    GBL_API_VERIFY_ARG(size);
    gblVectorDestruct(pVec);
    pVec->pBuffer = (char*)pData;
    pVec->capacity = capacity;
    pVec->size = size;
    GBL_API_END();
}

GBL_INLINE GBL_API gblVectorContext(const GblVector* pVec, GblContext* pCtx) GBL_NOEXCEPT {
    GBL_API_FUNCTION(pVec->hCtx, ({
        GBL_API_VERIFY_POINTER(pCtx);
        *pCtx = pVec->hCtx;
    }));
}

GBL_INLINE GBL_API gblVectorStackBytes(const GblVector* pVec, GblSize* pSize) GBL_NOEXCEPT {
    GBL_API_BEGIN(pVec->hCtx);
    GBL_API_VERIFY_POINTER(pSize);
    *pSize = pVec->stackSize;
    GBL_API_END();
}

GBL_INLINE GBL_API gblVectorSize(const GblVector* pVec, GblSize* pSize) GBL_NOEXCEPT {
    GBL_API_BEGIN(pVec->hCtx);
    GBL_API_VERIFY_POINTER(pSize);
    *pSize = pVec->size;
    GBL_API_END();
}

GBL_INLINE GBL_API gblVectorCapacity(const GblVector* pVec, GblSize* pCapacity) GBL_NOEXCEPT {
    GBL_API_BEGIN(pVec->hCtx);
    GBL_API_VERIFY_POINTER(pCapacity);
    *pCapacity = pVec->capacity;
    GBL_API_END();
}

GBL_INLINE GBL_API gblVectorElementSize(const GblVector* pVec, GblSize* pElemSize) GBL_NOEXCEPT {
    GBL_API_FUNCTION(pVec->hCtx, ({
        GBL_API_VERIFY_POINTER(pElemSize);
        *pElemSize = pVec->elementSize;
    }));
}

GBL_INLINE GBL_API gblVectorData(const GblVector* pVec, const void** ppData) GBL_NOEXCEPT {
    GBL_API_BEGIN(pVec->hCtx);
    GBL_API_VERIFY_POINTER(ppData);
    *ppData = pVec->pBuffer;
    GBL_API_END();
}

GBL_INLINE GBL_API gblVectorReserve(GblVector* pVec, GblSize capacity) GBL_NOEXCEPT {
    GBL_API_BEGIN(pVec->hCtx);
    if(pVec->capacity < capacity) {
        GblBool stack = GBL_FALSE;
        GBL_API_CALL(gblVectorIsStack(pVec, &stack));
        if(stack) {
            const GblSize oldSize = pVec->size;
            GBL_API_CALL(gblVectorAlloc_(pVec, GBL_VECTOR_CAPACITY_FROM_SIZE_(capacity * pVec->elementSize + 1)));
            memcpy(pVec->pBuffer, pVec->stackBuffer, oldSize * pVec->elementSize);
            pVec->size = oldSize;
        } else {
            pVec->pBuffer = pVec->hCtx != GBL_HANDLE_INVALID?
                        (char*)GBL_API_REALLOC(pVec->pBuffer, capacity * pVec->elementSize) :
                        (char*)realloc(pVec->pBuffer, capacity * pVec->elementSize);
            pVec->capacity = capacity;
        }
        pVec->pBuffer[pVec->size * pVec->elementSize] = 0;
    }
    GBL_API_END();
}

GBL_INLINE GBL_API gblVectorResize(GblVector* pVec, GblSize size) GBL_NOEXCEPT {
    GBL_API_BEGIN(pVec->hCtx);
    if(size > pVec->size) {

        if(size+1 > pVec->capacity) {
            GBL_API_CALL(gblVectorReserve(pVec, GBL_VECTOR_CAPACITY_FROM_SIZE_(size + 1)));
        }

        //pVec->data.pBuffer[pVec->data.length] = ' '; remove old nullcharacter so strlen isn't wrong!
    }

    pVec->pBuffer[size * pVec->elementSize] = 0;
    pVec->size = size;
    GBL_API_END();
}

GBL_INLINE GBL_API gblVectorShrinkToFit(GblVector* pVec) GBL_NOEXCEPT {
    GBL_API_BEGIN(pVec->hCtx);
    GblBool stack = GBL_FALSE;
    GBL_API_CALL(gblVectorIsStack(pVec, &stack));
    if(!stack && pVec->capacity > pVec->size) {
        pVec->pBuffer = pVec->hCtx != GBL_HANDLE_INVALID?
                    (char*)GBL_API_REALLOC(pVec->pBuffer, pVec->size * pVec->elementSize) :
                    (char*)realloc(pVec->pBuffer, pVec->size * pVec->elementSize);
        pVec->capacity = pVec->size;
    }
    GBL_API_END();
}


GBL_INLINE GBL_API gblVectorConcat(GblVector* pVec, const void* pData, GblSize elementCount) GBL_NOEXCEPT {
    GBL_API_BEGIN(pVec->hCtx);
    GblSize expectedSize = 0;
    GBL_API_VERIFY_POINTER(pData);
    GBL_API_VERIFY_ARG(elementCount);
    GBL_API_CALL(gblVectorSize(pVec, &expectedSize));
    expectedSize += elementCount;
    GBL_API_CALL(gblVectorReserve(pVec, expectedSize + 1));
    memcpy(&pVec->pBuffer[pVec->size * pVec->elementSize], pData, elementCount * pVec->elementSize);
    pVec->size = expectedSize;
    pVec->pBuffer[pVec->size * pVec->elementSize] = 0;
    GBL_API_END();
}

GBL_INLINE GBL_API gblVectorAt(const GblVector* pVector, GblSize index, void** ppEntry) GBL_NOEXCEPT {
    GBL_API_BEGIN(pVector->hCtx);
    GBL_API_VERIFY_POINTER(ppEntry);
    GBL_API_VERIFY_ARG(index < pVector->size);
    *ppEntry = &pVector->pBuffer[index * pVector->elementSize];
    GBL_API_END();
}

GBL_INLINE GBL_API gblVectorFront(const GblVector* pVector, void** ppEntry) GBL_NOEXCEPT {
    return gblVectorAt(pVector, 0, ppEntry);
}

GBL_INLINE GBL_API gblVectorBack(const GblVector* pVector, void** ppEntry) GBL_NOEXCEPT {
    return gblVectorAt(pVector, pVector->size-1, ppEntry);
}


// HANDLES DYNAMIC REALLOCATION
GBL_INLINE GBL_API gblVectorInsert(GblVector* pVector, GblSize index, GblSize count, const void* pDataIn, void** ppDataOut) GBL_NOEXCEPT {
    uintptr_t insertionPoint = 0;
    GblSize slideSize = 0;
    GBL_API_BEGIN(pVector->hCtx);
    GBL_API_VERIFY_ARG(index <= pVector->size);
    GBL_API_CALL(gblVectorResize(pVector, pVector->size + count));
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
GBL_INLINE GBL_API gblVectorPushBack(GblVector* pVector, const void* pData) GBL_NOEXCEPT {
    GblSize size = 0;
    GBL_API_BEGIN(pVector->hCtx);
    GBL_API_CALL(gblVectorSize(pVector, &size));
    GBL_API_CALL(gblVectorInsert(pVector, size, 1, pData, NULL));
    GBL_API_END();
}

GBL_INLINE GBL_API gblVectorPushFront(GblVector* pVector, const void* pData) GBL_NOEXCEPT {
    GBL_API_BEGIN(pVector->hCtx);
    GBL_API_CALL(gblVectorInsert(pVector, 0, 1, pData, NULL));
    GBL_API_END();
}

GBL_INLINE GBL_API gblVectorErase(GblVector* pVector, GblSize begin, GblSize count) GBL_NOEXCEPT {
    GblSize lastIndex = 0;
    GblSize remainderCount  = 0;
    GBL_API_BEGIN(pVector->hCtx);
    GBL_API_VERIFY_EXPRESSION(pVector->size > 0);
    GBL_API_VERIFY_ARG(begin < pVector->size);
    lastIndex = begin + count - 1;
    GBL_API_VERIFY_ARG(lastIndex < pVector->size);
    GBL_API_VERIFY_ARG(begin <= lastIndex);
    GBL_API_VERIFY_EXPRESSION(count > 0 && count <= pVector->size);
    remainderCount = pVector->size - 1 - lastIndex;
    if(remainderCount) memmove(&pVector->pBuffer[begin * pVector->elementSize], &pVector->pBuffer[(lastIndex+1) * pVector->elementSize], remainderCount * pVector->elementSize);
    GBL_API_CALL(gblVectorResize(pVector, pVector->size - count));
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
    GBL_API_CALL(gblVectorSize(pVector, &GBL_API_INLINE_RETVAL()));
}));

GBL_API_INLINE_HELPER((GblSize, 0), VECTOR_CAPACITY, (const GblVector* pVector), {
    GBL_API_CALL(gblVectorCapacity(pVector, &GBL_API_INLINE_RETVAL()));
});

GBL_API_INLINE_HELPER((const void*, NULL), VECTOR_DATA, (const GblVector* pVector), {
    GBL_API_CALL(gblVectorData(pVector, &GBL_API_INLINE_RETVAL()));
});

GBL_API_INLINE_HELPER((void*, NULL), VECTOR_AT, (const GblVector* pVector, GblSize index), {
    GBL_API_CALL(gblVectorAt(pVector, index, &GBL_API_INLINE_RETVAL()));
});

GBL_API_INLINE_HELPER((void*, NULL), VECTOR_FRONT, (const GblVector* pVector), {
    GBL_API_CALL(gblVectorFront(pVector, &GBL_API_INLINE_RETVAL()));
});

GBL_API_INLINE_HELPER((void*, NULL), VECTOR_BACK, (const GblVector* pVector), {
    GBL_API_CALL(gblVectorBack(pVector, &GBL_API_INLINE_RETVAL()));
});

GBL_API_INLINE_HELPER((GblBool, GBL_FALSE), VECTOR_PUSH_BACK, (GblVector* pVector, const void* pValue), {
    GBL_API_CALL(gblVectorPushBack(pVector, pValue));
    GBL_API_INLINE_RETVAL() = GBL_RESULT_SUCCESS(GBL_API_RESULT());
});

GBL_API_INLINE_HELPER((GblBool, GBL_FALSE), VECTOR_PUSH_FRONT, (GblVector* pVector, const void* pValue), {
    GBL_API_CALL(gblVectorPushFront(pVector, pValue));
});

GBL_API_INLINE_HELPER((GblSize, 0), VECTOR_ELEMENT_SIZE, (const GblVector* pVector), {
    GBL_API_CALL(gblVectorElementSize(pVector, &GBL_API_INLINE_RETVAL()));
});

GBL_API_INLINE_HELPER((GblSize, 0), VECTOR_STACK_BYTES, (const GblVector* pVector), {
    GBL_API_CALL(gblVectorStackBytes(pVector, &GBL_API_INLINE_RETVAL()));
});

GBL_API_INLINE_HELPER((GblBool, GBL_TRUE), VECTOR_IS_STACK, (const GblVector* pVector), {
    GBL_API_CALL(gblVectorIsStack(pVector, &GBL_API_INLINE_RETVAL()));
});

GBL_API_INLINE_HELPER((GblBool, GBL_TRUE), VECTOR_IS_EMPTY, (const GblVector* pVector), {
    GBL_API_CALL(gblVectorIsEmpty(pVector, &GBL_API_INLINE_RETVAL()));
});

GBL_API_INLINE_HELPER((GblContext, GBL_HANDLE_INVALID), VECTOR_CONTEXT, (const GblVector* pVector), {
    GBL_API_CALL(gblVectorContext(pVector, &GBL_API_INLINE_RETVAL()));
});

// COPY + MOVE + ASSIGN

GBL_API_INLINE_HELPER((GblVector, {0), VECTOR_CREATE, (GblContext hCtx, GblSize elementSize, GblSize structSize, const void* pInitialData, GblSize elementCount), {
    GBL_API_CALL(gblVectorConstruct_6(&GBL_API_INLINE_RETVAL(), hCtx, elementSize, structSize, pInitialData, elementCount));
});

GBL_API_INLINE_HELPER((GblBool, GBL_FALSE), VECTOR_DESTROY, (GblVector* pVector), {
    GBL_API_CALL(gblVectorDestruct(pVector));
    GBL_API_INLINE_RETVAL() = GBL_RESULT_SUCCESS(GBL_API_RESULT());
});

#endif */


#undef CSELF
#undef SELF

GBL_DECLS_END

#endif // GIMBAL_VECTOR_H

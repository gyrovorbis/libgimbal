#ifndef GIMBAL_CONTAINER_H
#define GIMBAL_CONTAINER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "gimbal_types.h"

#if 0
/* 1) Do it like string with static, c standard, and context-based allocation
 * 2) Try to be generic as fuck to reuse code for different types of containers
 * 3) Create adapters in a way that make them easily representable, iterable, accessible in C++
 *
 */

///CONTAINER TYPES
/// 1) random access (array, map, etc)
/// 2) forward iterable (pretty much everything)
/// 3)


struct GblContainer;
struct GblContainerIterator;

typedef GBL_RESULT (*GblContainerVTableSize)(const struct GblContainer* pCont, GblSize* pSize);
//typedef GBL_RESULT (*GblContainerVTableNext)(const GblContainer* pCont, GblContainerIterator* pIterator);
typedef GBL_RESULT (*GblContainerVTableGet)(const struct GblContainer* pCont, const void* pKey, void** pValue);
typedef GBL_RESULT (*GblContainerVTableSet)(const struct GblContainer* pCont, const void* pKey, const void* pValue);

typedef GBL_RESULT (*GblContainerVTableIteratorBegin)(const struct GblContainer* pCont, struct GblContainerIterator* pIterator);
typedef GBL_RESULT (*GblContainerVTableIteratorEnd)(const struct GblContainer* pCont, struct GblContainerIterator* pIterator);
typedef GBL_RESULT (*GblContainerVTableIteratorIncrement)(const struct GblContainer* pCont, struct GblContainerIterator* pIterator);
typedef GBL_RESULT (*GblContainerVTableIteratorCompare)(const struct GblContainer* pCont, const struct GblContainerIterator* pIterator1, const struct GblContainerIterator* pIterator2);

typedef GBL_RESULT (*GblContainerVTableIteratorData)(const struct GblContainer* pCont, const struct GblContainerIterator* pIterator,
                                                     void** pKey, void** pVal);

typedef GBL_RESULT (*GblContainerVTableAppend)(const struct GblContainer* pCont, const void* pValue);
typedef GBL_RESULT (*GblContainerVTableRemove)(const struct GblContainer* pCont, const void* pKey);


//typedef GBL_RESULT (*GblContainer)








GBL_RESULT gblIteratorValue(const GblIterator* pIt, void* pValue);
GBL_RESULT gblIteratorKey(const GblIterator* pIt, void* pKey);


// Iteration
// Begin: gblContainerNext(pCont, nullptr, &next) => GblIterator;
// End:   gblContainerNext(pCont, &current, &next) => NULL;
// Next:  gblContainerNext(pCont, &current, &next) => GblIterator
GBL_RESULT gblContainerConstruct(GblContainer* pCont, GblContext* pCtx, GblSize valueSize);
GBL_RESULT gblContainerDestruct(GblContainer* pCont);

GBL_RESULT gblContainerNext(const GblContainer* pCont, const GblIterator* pCurIt, GblIterator* pNextIt);
//GBL_RESULT gblContainerRemove(GblContainer* pCont, const GblIterator* pIt);


GBL_RESULT gblContainerClear(GblContainer* pCont);
GBL_RESULT gblContainerData(GblContainer* pCont, void** ppData);
GBL_RESULT gblContainerContext(GblContainer* pCont, GblContext* pCtx);

GBL_RESULT gblContainerSize(const GblContainer* pCont, GblSize* pSize);
GBL_RESULT gblContainerCapacity(const GblContainer* pCont, GblSize* pCapacity);


GBL_RESULT gblContainerSize(GblContainer* pCont, GblSize* pSize);
GBL_RESULT gblContainerClear(GblContainer* pCont);


#define GBL_VECTOR_INDEX_INVALID GBL_SIZE_MAX
#endif


/*A. Sequence containers:
 * 1) vector
 * 2) forward list
 *
 * B. Associative containers:
 * 1) map
 * 2) hash
 *
 */

// JUST FOCUS ON VECTOR + MAP FOR NOW, UNIFY STRING + VECTOR



/*
#define GBL_META_OBJECT_VECTOR_TABLE (                                                               \
        ( GblVector, Vector, "Dynamic Array Container"),     \
        (                                              
            (gblVectorClear, clear, (void)


\
            (GBL_META_TYPE_VTABLE_TO,           0x0,    To,          "To"),                        \
            (GBL_META_TYPE_VTABLE_COMPARE,      0x1,    Compare,     "Compare"),                   \
            (GBL_META_TYPE_VTABLE_DESTRUCTOR,   0x2,    Destructor,  "Destructor"),                     \
            (GBL_META_TYPE_VTABLE_COUNT,        0x3,    BuiltinCount, "Builtin virtual method count")  \
        )                                                                                          \
    )
*/

typedef struct GblVector {
    GblContext      hCtx;
    char*           pBuffer;
    GblSize         size;
    GblSize         capacity;
    GblSize         elementSize;
    GblSize         stackSize;
    char            stackBuffer[GBL_VECTOR_STACK_BUFFER_DEFAULT_SIZE];
} GblVector;


//IMPLEMENT ME FOR CUSTOM ALLOCATION SCHEMES
GBL_INLINE GblSize GBL_VECTOR_CAPACITY_FROM_SIZE_(GblSize size) {
    return GBL_POW2_NEXT(size);
}

GBL_INLINE GBL_API gblVectorIsEmpty(const GblVector* pVec, GblBool* pResult) {
    GBL_ASSERT(pVec);
    GBL_ASSERT(pResult);
    *pResult = (!pVec->size);
    return GBL_RESULT_SUCCESS;
}

GBL_INLINE GBL_API gblVectorIsStack(const GblVector* pVec, GblBool* pResult) {
    GBL_ASSERT(pVec);
    GBL_ASSERT(pResult);
    *pResult = (pVec->pBuffer == pVec->stackBuffer);
    return GBL_RESULT_SUCCESS;
}

GBL_INLINE void gblVectorInitialize_(GblVector* pVec) {
    pVec->capacity = pVec->stackSize / pVec->elementSize;
    pVec->pBuffer = pVec->stackBuffer;
    pVec->size = 0;
    pVec->stackBuffer[0] = 0;
}

GBL_INLINE GBL_API gblVectorDestruct(GblVector* pVec) {
    GBL_API_BEGIN(pVec->hCtx);
    // Check if we have a buffer to free
    if(pVec->pBuffer && pVec->pBuffer != pVec->stackBuffer) {
        if(pVec->hCtx) GBL_API_FREE(pVec->pBuffer);
        else free(pVec->pBuffer);
    }
    GBL_API_END();
}

GBL_INLINE GBL_API gblVectorClear(GblVector* pVec) {
    GBL_API_BEGIN(pVec->hCtx);
    GBL_API_CALL(gblVectorDestruct(pVec));
    gblVectorInitialize_(pVec);
    GBL_API_END();
}

GBL_INLINE GBL_API gblVectorAlloc_(GblVector* pVec, GblSize capacity) {
    GBL_API_BEGIN(pVec->hCtx);

    if(capacity * pVec->elementSize <= pVec->stackSize) {
        pVec->pBuffer = pVec->stackBuffer;
        pVec->capacity = pVec->stackSize / pVec->elementSize;
    } else {
        pVec->capacity = capacity;
        pVec->pBuffer = pVec->hCtx != GBL_HANDLE_INVALID?
                    (char*)GBL_API_MALLOC(pVec->capacity * pVec->elementSize) :
                    (char*)malloc(pVec->capacity * pVec->elementSize);
    }
    pVec->pBuffer[0] = 0;
    pVec->size = 0;
    GBL_API_END();
}

GBL_INLINE GBL_API gblVectorAssign(GblVector* pVec, const void* pData, GblSize elementCount) {
    GBL_API_BEGIN(pVec->hCtx);
    if(pVec->capacity < elementCount) {
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

GBL_INLINE GBL_API gblVectorConstruct_6(GblVector* pVector, GblContext hCtx, GblSize elementSize, GblSize structSize, const void* pInitialData, GblSize elementCount) {
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

GBL_INLINE GBL_API gblVectorConstruct_4(GblVector* pVector, GblContext hCtx, GblSize elementSize, GblSize structSize) {
    return gblVectorConstruct_6(pVector, hCtx, elementSize, structSize, NULL, 0);
}

GBL_INLINE GBL_API gblVectorConstruct_3(GblVector* pVector, GblContext hCtx, GblSize elementSize) {
    return gblVectorConstruct_4(pVector, hCtx, elementSize, sizeof(GblVector));
}

#define gblVectorConstruct(...) \
        GBL_VA_OVERLOAD_SELECT(gblVectorConstruct, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)(__VA_ARGS__)

GBL_INLINE GBL_API gblVectorTake(GblVector* pVec, void** ppVecPtr, GblSize* pSize, GblSize* pCapacity) {
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

GBL_INLINE GBL_API gblVectorGive(GblVector* pVec, void* pData, GblSize size, GblSize capacity) {
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

GBL_INLINE GBL_API gblVectorContext(const GblVector* pVec, GblContext* pCtx) {
    GBL_API_BEGIN(pVec->hCtx);
    GBL_API_VERIFY_POINTER(pCtx);
    *pCtx = pVec->hCtx;
    GBL_API_END();
}

GBL_INLINE GBL_API gblVectorStackBytes(const GblVector* pVec, GblSize* pSize) {
    GBL_API_BEGIN(pVec->hCtx);
    GBL_API_VERIFY_POINTER(pSize);
    *pSize = pVec->stackSize;
    GBL_API_END();
}

GBL_INLINE GBL_API gblVectorSize(const GblVector* pVec, GblSize* pSize) {
    GBL_API_BEGIN(pVec->hCtx);
    GBL_API_VERIFY_POINTER(pSize);
    *pSize = pVec->size;
    GBL_API_END();
}

GBL_INLINE GBL_API gblVectorCapacity(const GblVector* pVec, GblSize* pCapacity) {
    GBL_API_BEGIN(pVec->hCtx);
    GBL_API_VERIFY_POINTER(pCapacity);
    *pCapacity = pVec->capacity;
    GBL_API_END();
}

GBL_INLINE GBL_API gblVectorElementSize(const GblVector* pVec, GblSize* pElemSize) {
    GBL_API_BEGIN(pVec->hCtx);
    GBL_API_VERIFY_POINTER(pElemSize);
    *pElemSize = pVec->elementSize;
    GBL_API_END();
}

GBL_INLINE GBL_API gblVectorData(const GblVector* pVec, const void** ppData) {
    GBL_API_BEGIN(pVec->hCtx);
    GBL_API_VERIFY_POINTER(ppData);
    *ppData = pVec->pBuffer;
    GBL_API_END();
}

GBL_INLINE GBL_API gblVectorReserve(GblVector* pVec, GblSize capacity) {
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

GBL_INLINE GBL_API gblVectorResize(GblVector* pVec, GblSize size) {
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

GBL_INLINE GBL_API gblVectorShrinkToFit(GblVector* pVec) {
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


GBL_INLINE GBL_API gblVectorConcat(GblVector* pVec, const void* pData, GblSize elementCount) {
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

GBL_INLINE GBL_API gblVectorAt(const GblVector* pVector, GblSize index, void** ppEntry) {
    GBL_API_BEGIN(pVector->hCtx);
    GBL_API_VERIFY_POINTER(ppEntry);
    GBL_API_VERIFY_ARG(index < pVector->size);
    *ppEntry = &pVector->pBuffer[index * pVector->elementSize];
    GBL_API_END();
}

GBL_INLINE GBL_API gblVectorFront(const GblVector* pVector, void** ppEntry) {
    return gblVectorAt(pVector, 0, ppEntry);
}

GBL_INLINE GBL_API gblVectorBack(const GblVector* pVector, void** ppEntry) {
    return gblVectorAt(pVector, pVector->size-1, ppEntry);
}


// HANDLES DYNAMIC REALLOCATION
GBL_INLINE GBL_API gblVectorInsert(GblVector* pVector, GblSize index, GblSize count, const void* pDataIn, void** ppDataOut) {
    uintptr_t insertionPoint = 0;
    GblSize slideSize = 0;
    GBL_API_BEGIN(pVector->hCtx);
    GBL_API_VERIFY_ARG(index <= pVector->size);
    GBL_API_CALL(gblVectorResize(pVector, pVector->size + count));
    slideSize = pVector->size - index;
    insertionPoint = (uintptr_t)pVector->pBuffer + index * pVector->elementSize;
    memmove((char*)insertionPoint + pVector->elementSize * count, (const char*)insertionPoint, slideSize * pVector->elementSize);
    if(pDataIn) {
        memcpy((void*)insertionPoint, pDataIn, pVector->elementSize * count);
    }
    if(ppDataOut) {
        *ppDataOut = (void*)insertionPoint;
    }
    GBL_API_END();
}

// SHOULD BE THE SAME THING AS INSERT
GBL_INLINE GBL_API gblVectorPushBack(GblVector* pVector, const void* pData) {
    GblSize size = 0;
    GBL_API_BEGIN(pVector->hCtx);
    GBL_API_CALL(gblVectorSize(pVector, &size));
    GBL_API_CALL(gblVectorInsert(pVector, size, 1, pData, NULL));
    GBL_API_END();
}

GBL_INLINE GBL_API gblVectorPushFront(GblVector* pVector, const void* pData) {
    GBL_API_BEGIN(pVector->hCtx);
    GBL_API_CALL(gblVectorInsert(pVector, 0, 1, pData, NULL));
    GBL_API_END();
}

GBL_INLINE GBL_API gblVectorErase(GblVector* pVector, GblSize begin, GblSize count) {
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







/* STRING SHIT THAT CAN GO HERE:
 * context
 * isEmpty()
 * isStack()
 * clear()
 * take()/give()
 * stackSize()
 * capacity()
 * resize()/reserve()
 * concatenate()
*/
// set comparator for for find?



#if 0


typedef struct GblAssociativeContainer {
    GblContainer        containerBase;
    GblSize             keySize;

} GblAssociativeContainer;

GBL_RESULT gblMapConstruct(GblMap* pMap, GblContext* pCtx, GblSize keySize, GblSize valueSize, Comparator*);
GBL_RESULT gblMapDestruct(GblMap* pMap);
GBL_RESULT gblMapAt(GblMap* pMap, void* pKey, void** ppEntry);
GBL_RESULT gblMapNext(GblMap* pMap, void* pKeyCurrent, void* pKeyNext);
GBL_RESULT gblMapInsert(GblMap* pMap, const void* pKey, const void* pValue);
GBL_RESULT gblMapErase(GblMap* pMap, const void* pKey);



#endif


#ifdef __cplusplus
}
#endif

#endif // GIMBAL_CONTAINER_H

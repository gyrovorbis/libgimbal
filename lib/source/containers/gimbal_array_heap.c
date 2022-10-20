#include <gimbal/containers/gimbal_array_heap.h>

#define GBL_ARRAY_HEAP_LIST_(heap)      (&GBL_PRIV_REF(heap).list)
#define GBL_ARRAY_HEAP_PARENT_(npos)    ((intptr_t)(((npos) - 1) / 2))
#define GBL_ARRAY_HEAP_LEFT_(npos)      (((npos) * 2) + 1)
#define GBL_ARRAY_HEAP_RIGHT_(npos)     (((npos) * 2) + 2)



GBL_EXPORT GBL_RESULT (GblArrayHeap_construct)(GblArrayHeap*     pSelf,
                                               GblSize           elemSize,
                                               GblArrayHeapCmpFn pFnCmp,
                                               GblSize           structSize,
                                               GblContext*       pCtx)
{
    GBL_CTX_BEGIN(pCtx);
    GBL_CTX_VERIFY_POINTER(pFnCmp);
    GBL_CTX_VERIFY_CALL(GblArrayList_construct(GBL_ARRAY_HEAP_LIST_(pSelf),
                                               elemSize,
                                               0,
                                               NULL,
                                               structSize-(sizeof(GblArrayHeap)-sizeof(GblArrayList)),
                                               GBL_FALSE,
                                               pCtx));
    GBL_PRIV_REF(pSelf).pFnCmp = pFnCmp;
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblArrayHeap_destruct(GblArrayHeap* pSelf) {
    return GblArrayList_destruct(GBL_ARRAY_HEAP_LIST_(pSelf));
}

GBL_EXPORT GBL_RESULT GblArrayHeap_copy(GblArrayHeap* pSelf, const GblArrayHeap* pRhs) {
    GblArrayList_destruct(GBL_ARRAY_HEAP_LIST_(pSelf));

    const GBL_RESULT result = GblArrayList_assign(GBL_ARRAY_HEAP_LIST_(pSelf),
                                                  GblArrayList_data(GBL_ARRAY_HEAP_LIST_(pRhs)),
                                                  GblArrayList_size(GBL_ARRAY_HEAP_LIST_(pRhs)));
    GBL_PRIV_REF(pSelf).pFnCmp = GBL_PRIV_REF(pRhs).pFnCmp;
    return result;
}

GBL_EXPORT GBL_RESULT GblArrayHeap_move(GblArrayHeap* pSelf, GblArrayHeap* pRhs) {
    void* pData;
    GblSize size;
    GblSize capacity;

    GblArrayList_destruct(GBL_ARRAY_HEAP_LIST_(pSelf));

    GBL_RESULT result = GblArrayList_release(GBL_ARRAY_HEAP_LIST_(pRhs), &pData, &size, &capacity);
    if(!GBL_RESULT_SUCCESS(result)) return result;
    result = GblArrayList_acquire(GBL_ARRAY_HEAP_LIST_(pSelf), pData, size, capacity);

    GBL_PRIV_REF(pSelf).pFnCmp = GBL_PRIV_REF(pRhs).pFnCmp;
    return result;
}

GBL_EXPORT GBL_RESULT GblArrayHeap_push(GblArrayHeap* pSelf, const void* pEntry) {
    GblArrayList* pList = GBL_ARRAY_HEAP_LIST_(pSelf);
    GBL_CTX_BEGIN(GblArrayHeap_context(pSelf));
    GBL_CTX_VERIFY_CALL(GblArrayHeap_reserve(pSelf, GblArrayHeap_capacity(pSelf)+1));

    GBL_CTX_VERIFY_CALL(GblArrayList_pushBack(pList, pEntry));

    const GblSize elementSize = GblArrayHeap_elementSize(pSelf);
    intptr_t ipos = (intptr_t)GblArrayHeap_size(pSelf);
    intptr_t ppos = GBL_ARRAY_HEAP_PARENT_(ipos);

    void* pTempEntry = GBL_ALLOCA(elementSize);

    while(ipos > 0) {
        void* pPposEntry = GblArrayList_at(pList, ppos);
        void* pIposEntry = GblArrayList_at(pList, ipos);
        if(GBL_PRIV_REF(pSelf).pFnCmp(GblArrayList_at(pList, ppos),
                                      GblArrayList_at(pList, ipos)) >= 0)
            break;

        memcpy(pTempEntry, pPposEntry, elementSize);
        memcpy(pPposEntry, pIposEntry, elementSize);
        memcpy(pIposEntry, pTempEntry, elementSize);

        ipos = ppos;
        ppos = GBL_ARRAY_HEAP_PARENT_(ipos);
    }

    ++GBL_PRIV_REF(pList).size;

    GBL_CTX_END();
}

GBL_INLINE GBL_RESULT GblArrayHeap_pop(GblArrayHeap* pSelf, void* pEntryOut) {
    if(!GblArrayHeap_size(pSelf)) return GBL_RESULT_ERROR_OUT_OF_RANGE;

    GblArrayList* pList = GBL_ARRAY_HEAP_LIST_(pSelf);
    GBL_RESULT result = GblArrayList_popFront(pList, pEntryOut);
    if(!GBL_RESULT_SUCCESS(result)) return result;

    intptr_t mpos = 0;
    intptr_t ipos = 0;
    intptr_t lpos = GBL_ARRAY_HEAP_LEFT_(ipos);
    intptr_t rpos = GBL_ARRAY_HEAP_RIGHT_(ipos);
    void* pTemp   = GBL_ALLOCA(GblArrayHeap_elementSize(pSelf));

    while(1) {
        lpos = GBL_ARRAY_HEAP_LEFT_(ipos);
        rpos = GBL_ARRAY_HEAP_RIGHT_(ipos);

        if(lpos < (intptr_t)GblArrayHeap_size(pSelf) &&
           GBL_PRIV_REF(pSelf).pFnCmp(GblArrayList_at(pList, lpos),
                                      GblArrayList_at(pList, ipos)) > 0) {
            mpos = lpos;
        } else {
            mpos = ipos;
        }

        if(lpos < (intptr_t)GblArrayHeap_size(pSelf) &&
           GBL_PRIV_REF(pSelf).pFnCmp(GblArrayList_at(pList, rpos),
                                      GblArrayList_at(pList, mpos)) > 0) {
            mpos = rpos;
        }

        if(mpos == ipos) break;

        memcpy(pTemp, GblArrayList_at(pList, mpos), GblArrayHeap_elementSize(pSelf));
        memcpy(GblArrayList_at(pList, mpos), GblArrayList_at(pList, ipos), GblArrayHeap_elementSize(pSelf));
        memcpy(GblArrayList_at(pList, ipos), pTemp, GblArrayHeap_elementSize(pSelf));

        ipos = mpos;
    }
    return result;
}

#include <gimbal/allocators/gimbal_allocation_tracker.h>
#include <gimbal/core/gimbal_ctx.h>
#include <gimbal/containers/gimbal_hash_set.h>
#include <gimbal/algorithms/gimbal_hash.h>

#define GBL_ALLOCATION_TRACKER_INITIAL_CAPACITY_    512

typedef struct GblAllocationTracker_ {
    GblAllocationTracker base;
    GblHashSet           activeSet;
    GblBool              recursing;
} GblAllocationTracker_;

typedef struct GblAllocationEntry_ {
    const void*         pPointer;
    size_t              size;
    size_t              alignment;
    const char*         pDebugStr;
    GblSourceLocation   sourceLocation;
} GblAllocationEntry_;

GblHash GblAllocationTracker_hash_(const GblHashSet* pSet, const void* pEntry) {
    GBL_UNUSED(pSet);
    return gblHash(pEntry, sizeof(void*));
}

GblBool GblAllocationTracker_compare_(const GblHashSet* pSet,
                                      const void* pEntry1,
                                      const void* pEntry2)
{
    GBL_UNUSED(pSet);
    return *(const void**)pEntry1 == *(const void**)pEntry2?
                GBL_TRUE : GBL_FALSE;
}

GBL_EXPORT GblAllocationTracker* GblAllocationTracker_create(GblContext* pCtx) {
    GblAllocationTracker_* pTracker = NULL;

    GBL_CTX_BEGIN(pCtx);

    pTracker = GBL_CTX_MALLOC(sizeof(GblAllocationTracker_));
    memset(pTracker, 0, sizeof(GblAllocationTracker_));

    pTracker->recursing = GBL_TRUE;
    GBL_CTX_VERIFY_CALL(GblHashSet_construct(&pTracker->activeSet,
                                             sizeof(GblAllocationEntry_),
                                             GblAllocationTracker_hash_,
                                             GblAllocationTracker_compare_,
                                             NULL,
                                             GBL_ALLOCATION_TRACKER_INITIAL_CAPACITY_,
                                             pCtx,
                                             pTracker));
    GBL_CTX_END_BLOCK();
    pTracker->recursing = GBL_FALSE;
    return &pTracker->base;
}

GBL_EXPORT GBL_RESULT GblAllocationTracker_destroy(GblAllocationTracker* pSelf) {
    GblAllocationTracker_* pSelf_ = (GblAllocationTracker_*)pSelf;
    GBL_CTX_BEGIN(GblHashSet_context(&pSelf_->activeSet));
    pSelf_->recursing = GBL_TRUE;
    GBL_CTX_VERIFY_CALL(GblHashSet_destruct(&pSelf_->activeSet));
    pSelf_->recursing = GBL_FALSE;
    GBL_CTX_FREE(pSelf_);
    GBL_CTX_END_BLOCK();
    return GBL_CTX_RESULT();
}

GBL_EXPORT GBL_RESULT GblAllocationTracker_allocEvent(GblAllocationTracker* pSelf,
                                                      const void*           pPtr,
                                                      size_t                size,
                                                      size_t                align,
                                                      const char*           pDbg,
                                                      GblSourceLocation     srcLoc)
{
    const GblAllocationEntry_ entry = {
        .pPointer       = pPtr,
        .size           = size,
        .alignment      = align,
        .pDebugStr      = pDbg,
        .sourceLocation = srcLoc
    };

    GblAllocationTracker_* pSelf_ = (GblAllocationTracker_*)pSelf;

    GBL_CTX_BEGIN(GblHashSet_context(&pSelf_->activeSet));
    if(pSelf_->recursing) GBL_CTX_DONE();

    pSelf_->recursing = GBL_TRUE;
    GBL_CTX_VERIFY(GblHashSet_insert(&pSelf_->activeSet, &entry),
                   GBL_RESULT_ERROR_MEM_ALLOC,
                   "[Allocation Tracker] Attemping to allocate existing allocation: [%p]", pPtr);

    pSelf_->base.counters.bytesActive += size;
    pSelf_->base.counters.bytesAllocated += size;
    ++pSelf_->base.counters.allocEvents;
    ++pSelf_->base.counters.allocsActive;

    if(pSelf_->base.counters.allocsActive > pSelf_->base.maxAllocations) {
        pSelf_->base.maxAllocations = pSelf_->base.counters.allocsActive;
    }

    if(pSelf_->base.counters.bytesActive > pSelf_->base.maxBytes) {
        pSelf_->base.maxBytes = pSelf_->base.counters.bytesActive;
    }

    if(size > pSelf_->base.maxAllocationSize)
        pSelf_->base.maxAllocationSize = size;

    GBL_CTX_END_BLOCK();
    pSelf_->recursing = GBL_FALSE;
    return GBL_CTX_RESULT();
}

GBL_EXPORT GBL_RESULT GblAllocationTracker_reallocEvent(GblAllocationTracker*   pSelf,
                                                        const void*             pExisting,
                                                        const void*             pNew,
                                                        size_t                  newSize,
                                                        size_t                  newAlign,
                                                        GblSourceLocation       srcLoc)
{
    GblAllocationEntry_ entry = {
        .pPointer       = pExisting,
        .size           = newSize,
        .alignment      = newAlign,
        .pDebugStr      = NULL,
        .sourceLocation = srcLoc
    };

    GblAllocationTracker_* pSelf_ = (GblAllocationTracker_*)pSelf;
    GBL_CTX_BEGIN(GblHashSet_context(&pSelf_->activeSet));
    if(pSelf_->recursing) GBL_CTX_DONE();

    pSelf_->recursing = GBL_TRUE;
    GblAllocationEntry_* pExistingEntry = GblHashSet_extract(&pSelf_->activeSet, &entry);

    GBL_CTX_VERIFY(pExistingEntry,
                   GBL_RESULT_ERROR_MEM_REALLOC,
                   "[Allocation Tracker] Attempt to realloc unknown pointer: [%p]", pExisting);

    entry.pPointer = pNew;
    pSelf_->base.counters.bytesActive -= pExistingEntry->size;
    pSelf_->base.counters.bytesAllocated -= pExistingEntry->size;

    GBL_CTX_VERIFY(GblHashSet_insert(&pSelf_->activeSet, &entry),
                   GBL_RESULT_ERROR_MEM_REALLOC,
                   "[Allocation Tracker] Failed to insert entry for reallocated pointer: [%p]", pNew);

    pSelf_->base.counters.bytesActive += newSize;
    pSelf_->base.counters.bytesAllocated += newSize;
    ++pSelf_->base.counters.reallocEvents;

    if(pSelf_->base.counters.bytesActive > pSelf_->base.maxBytes) {
        pSelf_->base.maxBytes = pSelf_->base.counters.bytesActive;
    }

    if(newSize > pSelf_->base.maxAllocationSize)
        pSelf_->base.maxAllocationSize = newSize;

    GBL_CTX_END_BLOCK();
    pSelf_->recursing = GBL_FALSE;
    return GBL_CTX_RESULT();
}

GBL_EXPORT GBL_RESULT GblAllocationTracker_freeEvent(GblAllocationTracker* pSelf, const void* pPtr, GblSourceLocation srcLoc) {
    GblAllocationTracker_* pSelf_ = (GblAllocationTracker_*)pSelf;
    GBL_CTX_BEGIN(GblHashSet_context(&pSelf_->activeSet));
    if(pSelf_->recursing) GBL_CTX_DONE();

    const GblAllocationEntry_ entry = {
        .pPointer   = pPtr
    };

    pSelf_->recursing = GBL_TRUE;
    GblAllocationEntry_* pExisting = GblHashSet_extract(&pSelf_->activeSet, &entry);

    if(!pExisting) {
        GBL_CTX_WARN("[Allocation Tracker] Attempt to free unknown pointer: [%p]", pPtr);
        GBL_CTX_DONE();
    }

    pSelf_->base.counters.bytesActive -= pExisting->size;
    pSelf_->base.counters.bytesFreed += pExisting->size;

    GBL_CTX_END_BLOCK();
    ++pSelf_->base.counters.freeEvents;
    --pSelf_->base.counters.allocsActive;

    pSelf_->recursing = GBL_FALSE;
    return GBL_CTX_RESULT();
}

GBL_EXPORT GblBool GblAllocationTracker_validatePointer(const GblAllocationTracker* pSelf, const void* pPtr) {
    GblAllocationTracker_* pSelf_ = (GblAllocationTracker_*)pSelf;
    const GblAllocationEntry_ entry = {
        .pPointer   = pPtr
    };

    GblHashSetIter it = GblHashSet_find(&pSelf_->activeSet, &entry);
    return GblHashSetIter_valid(&it);
}

GBL_EXPORT GBL_RESULT GblAllocationTracker_logActive(const GblAllocationTracker* pSelf) {
    GblAllocationTracker_* pSelf_ = (GblAllocationTracker_*)pSelf;
    GBL_CTX_BEGIN(GblHashSet_context(&pSelf_->activeSet));
    GBL_CTX_INFO("[Allocation Tracker] Dumping Active Allocations:");
    GBL_CTX_PUSH();
    size_t  count = 0;
    for(GblHashSetIter it = GblHashSet_next(&pSelf_->activeSet, NULL);
        GblHashSetIter_valid(&it);
        it = GblHashSet_next(&pSelf_->activeSet, &it), count++)
    {
        const GblAllocationEntry_* pEntry = GblHashSetIter_value(&it);

        GBL_CTX_INFO("[%u]: %p", count, pEntry->pPointer);
        GBL_CTX_PUSH();
        GBL_CTX_INFO("%-20s: %20u", "Size",     pEntry->size);
        GBL_CTX_INFO("%-20s: %20s", "Marker",   pEntry->pDebugStr);
        GBL_CTX_INFO("%-20s: %20s", "File",     pEntry->sourceLocation.pFile);
        GBL_CTX_INFO("%-20s: %20s", "Function", pEntry->sourceLocation.pFunc);
        GBL_CTX_INFO("%-20s: %20u", "Line",     pEntry->sourceLocation.line);
        GBL_CTX_POP(1);
    }

    GBL_CTX_POP(1);
    GBL_CTX_END();

}

void GblAllocationTracker_captureCounters(const GblAllocationTracker* pSelf,
                                          GblAllocationCounters*      pCounters) {

    memcpy(pCounters, &pSelf->counters, sizeof(GblAllocationCounters));
}

void GblAllocationTracker_diffCounters(const GblAllocationTracker* pSelf,
                                       const GblAllocationCounters* pSrc,
                                       GblAllocationCounters*       pDst) {

    pDst->allocEvents    = pSelf->counters.allocEvents    - pSrc->allocEvents;
    pDst->reallocEvents  = pSelf->counters.reallocEvents  - pSrc->reallocEvents;
    pDst->freeEvents     = pSelf->counters.freeEvents     - pSrc->freeEvents;

    pDst->bytesAllocated = pSelf->counters.bytesAllocated - pSrc->bytesAllocated;
    pDst->bytesFreed     = pSelf->counters.bytesFreed     - pSrc->bytesFreed;
    pDst->bytesActive    = pSelf->counters.bytesActive    - pSrc->bytesActive;

    pDst->allocsActive   = pSelf->counters.allocsActive   - pSrc->allocsActive;

}






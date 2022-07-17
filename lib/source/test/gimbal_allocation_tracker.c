#include <gimbal/test/gimbal_allocation_tracker.h>
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/containers/gimbal_hash_set.h>
#include <gimbal/algorithms/gimbal_hash.h>

#define GBL_ALLOCATION_TRACKER_INITIAL_CAPACITY_    512

typedef struct GblAllocationTracker_ {
    GblAllocationTracker    base;
    GblHashSet              activeSet;
    GblBool                 recursing;
} GblAllocationTracker_;

typedef struct GblAllocationEntry_ {
    const void*         pPointer;
    GblSize             size;
    GblSize             alignment;
    const char*         pDebugStr;
    GblSourceLocation   sourceLocation;
} GblAllocationEntry_;

GblHash GblAllocationTracker_hash_(const GblHashSet* pSet, const void* pEntry) {
    GBL_UNUSED(pSet);
    return gblHashMurmur(pEntry, sizeof(void*));
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

    GBL_API_BEGIN(pCtx);

    pTracker = GBL_API_MALLOC(sizeof(GblAllocationTracker_));
    memset(pTracker, 0, sizeof(GblAllocationTracker_));

    pTracker->recursing = GBL_TRUE;
    GBL_API_VERIFY_CALL(GblHashSet_construct(&pTracker->activeSet,
                                             sizeof(GblAllocationEntry_),
                                             GblAllocationTracker_hash_,
                                             GblAllocationTracker_compare_,
                                             NULL,
                                             GBL_ALLOCATION_TRACKER_INITIAL_CAPACITY_,
                                             pCtx,
                                             pTracker));
    GBL_API_END_BLOCK();
    pTracker->recursing = GBL_FALSE;
    return &pTracker->base;
}

GBL_EXPORT GBL_RESULT GblAllocationTracker_destroy(GblAllocationTracker* pSelf) {
    GblAllocationTracker_* pSelf_ = (GblAllocationTracker_*)pSelf;
    GBL_API_BEGIN(GblHashSet_context(&pSelf_->activeSet));
    pSelf_->recursing = GBL_TRUE;
    GBL_API_VERIFY_CALL(GblHashSet_destruct(&pSelf_->activeSet));
    GBL_API_FREE(pSelf_);
    GBL_API_END_BLOCK();
    pSelf_->recursing = GBL_FALSE;
    return GBL_API_RESULT();
}

GBL_EXPORT GBL_RESULT GblAllocationTracker_allocEvent(GblAllocationTracker* pSelf,
                                                      const void*           pPtr,
                                                      GblSize               size,
                                                      GblSize               align,
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

    GBL_API_BEGIN(GblHashSet_context(&pSelf_->activeSet));
    if(pSelf_->recursing) GBL_API_DONE();

    pSelf_->recursing = GBL_TRUE;
    GBL_API_VERIFY(GblHashSet_insert(&pSelf_->activeSet, &entry),
                   GBL_RESULT_ERROR_MEM_ALLOC,
                   "[Allocation Tracker] Attemping to allocate existing allocation: [%p]", pPtr);

    pSelf_->base.activeBytes += size;
    ++pSelf_->base.totalAllocations;
    pSelf_->base.totalBytes += size;
    const GblSize totalActive = pSelf_->base.totalAllocations - pSelf_->base.totalFrees;
    if(totalActive > pSelf_->base.maxAllocations) {
        pSelf_->base.maxAllocations = totalActive;
    }
    if(pSelf_->base.activeBytes > pSelf_->base.maxBytes) {
        pSelf_->base.maxBytes = pSelf_->base.activeBytes;
    }
    if(size > pSelf_->base.maxAllocationSize) pSelf_->base.maxAllocationSize = size;

    GBL_API_END_BLOCK();
    pSelf_->recursing = GBL_FALSE;
    return GBL_API_RESULT();
}

GBL_EXPORT GBL_RESULT GblAllocationTracker_reallocEvent(GblAllocationTracker*   pSelf,
                                                        const void*             pExisting,
                                                        const void*             pNew,
                                                        GblSize                 newSize,
                                                        GblSize                 newAlign,
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
    GBL_API_BEGIN(GblHashSet_context(&pSelf_->activeSet));
    if(pSelf_->recursing) GBL_API_DONE();

    pSelf_->recursing = GBL_TRUE;
    GblAllocationEntry_* pExistingEntry = GblHashSet_extract(&pSelf_->activeSet, &entry);

    GBL_API_VERIFY(pExistingEntry,
                   GBL_RESULT_ERROR_MEM_REALLOC,
                   "[Allocation Tracker] Attempt to realloc unknown pointer: [%p]", pExisting);

    entry.pPointer = pNew;
    pSelf_->base.activeBytes -= pExistingEntry->size;
    pSelf_->base.totalBytes -= pExistingEntry->size;

    GBL_API_VERIFY(GblHashSet_insert(&pSelf_->activeSet, &entry),
                   GBL_RESULT_ERROR_MEM_REALLOC,
                   "[Allocation Tracker] Failed to insert entry for reallocated pointer: [%p]", pNew);

    pSelf_->base.activeBytes += newSize;
    pSelf_->base.totalBytes += newSize;
    ++pSelf_->base.totalReallocations;
    if(pSelf_->base.activeBytes > pSelf_->base.maxBytes) {
        pSelf_->base.maxBytes = pSelf_->base.activeBytes;
    }
    if(newSize > pSelf_->base.maxAllocationSize) pSelf_->base.maxAllocationSize = newSize;
    GBL_API_END_BLOCK();
    pSelf_->recursing = GBL_FALSE;
    return GBL_API_RESULT();
}

GBL_EXPORT GBL_RESULT GblAllocationTracker_freeEvent(GblAllocationTracker* pSelf, const void* pPtr, GblSourceLocation srcLoc) {
    GblAllocationTracker_* pSelf_ = (GblAllocationTracker_*)pSelf;
    GBL_API_BEGIN(GblHashSet_context(&pSelf_->activeSet));
    if(pSelf_->recursing) GBL_API_DONE();

    const GblAllocationEntry_ entry = {
        .pPointer   = pPtr
    };

    pSelf_->recursing = GBL_TRUE;
    GblAllocationEntry_* pExisting = GblHashSet_extract(&pSelf_->activeSet, &entry);

    GBL_API_VERIFY(pExisting,
                   GBL_RESULT_ERROR_MEM_FREE,
                   "[Allocation Tracker] Attempt to free unknown pointer: [%p]", pPtr);

    pSelf_->base.activeBytes -= pExisting->size;
    ++pSelf_->base.totalFrees;

    GBL_API_END_BLOCK();
    pSelf_->recursing = GBL_FALSE;
    return GBL_API_RESULT();
}

GBL_EXPORT GBL_RESULT GblAllocationTracker_validatePointer(const GblAllocationTracker* pSelf, const void* pPtr) {
    GblAllocationTracker_* pSelf_ = (GblAllocationTracker_*)pSelf;
    const GblAllocationEntry_ entry = {
        .pPointer   = pPtr
    };

    GBL_API_BEGIN(GblHashSet_context(&pSelf_->activeSet));
    GblHashSetIterator it = GblHashSet_find(&pSelf_->activeSet, &entry);
    GBL_API_VERIFY(GblHashSetIterator_valid(&it),
                   GBL_RESULT_ERROR_INVALID_POINTER);

    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblAllocationTracker_logActive(const GblAllocationTracker* pSelf) GBL_NOEXCEPT {
    GblAllocationTracker_* pSelf_ = (GblAllocationTracker_*)pSelf;
    GBL_API_BEGIN(GblHashSet_context(&pSelf_->activeSet));
    GBL_API_INFO("[Allocation Tracker] Dumping Active Allocations:");
    GBL_API_PUSH();
    GblSize count = 0;
    for(GblHashSetIterator it = GblHashSet_next(&pSelf_->activeSet, NULL);
        GblHashSetIterator_valid(&it);
        it = GblHashSet_next(&pSelf_->activeSet, &it), count++)
    {
        const GblAllocationEntry_* pEntry = GblHashSetIterator_value(&it);

        GBL_API_INFO("[%u]: %p", count, pEntry->pPointer);
        GBL_API_PUSH();
        GBL_API_INFO("%-20s: %20u", "Size",     pEntry->size);
        GBL_API_INFO("%-20s: %20s", "Marker",   pEntry->pDebugStr);
        GBL_API_INFO("%-20s: %20s", "File",     pEntry->sourceLocation.pFile);
        GBL_API_INFO("%-20s: %20s", "Function", pEntry->sourceLocation.pFunc);
        GBL_API_INFO("%-20s: %20u", "Line",     pEntry->sourceLocation.line);
        GBL_API_POP(1);


    }
    GBL_API_POP(1);
    GBL_API_END();

}

GBL_EXPORT GblSize GblAllocationTracker_activeCount(const GblAllocationTracker* pSelf) GBL_NOEXCEPT {
    GblAllocationTracker_* pSelf_ = (GblAllocationTracker_*)pSelf;
    return pSelf_? GblHashSet_size(&pSelf_->activeSet) : 0;
}

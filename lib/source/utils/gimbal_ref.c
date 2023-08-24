#include <gimbal/utils/gimbal_ref.h>
#include <gimbal/core/gimbal_atomics.h>

typedef struct GblRef_ {
    GblContext*         pCtx;
    GBL_ATOMIC_INT16    refCount;
} GblRef_;

static GBL_ATOMIC_INT16 activeCount_ = 0;

GBL_INLINE  GblRef_* GblRef_header_(const void* pData) GBL_NOEXCEPT {
    return (GblRef_*)(pData? (GblRef_*)pData - 1 : NULL);
}

GBL_EXPORT GBL_RESULT GblRef_reinit(void) {
    GBL_CTX_BEGIN(NULL);
    const GblRefCount refCount = GblRef_activeCount();
    if(refCount > 0) {
        GBL_CTX_RECORD_SET(GBL_RESULT_PARTIAL,
                           "GblRef_reinit(): %u remaining references!",
                           refCount);
    }
    GBL_ATOMIC_INT16_STORE(activeCount_, 0);
    GBL_CTX_END();
}

GBL_EXPORT GblRefCount GblRef_activeCount(void) {
    return (GblRefCount)GBL_ATOMIC_INT16_LOAD(activeCount_);
}

GBL_EXPORT void* GblRef_allocWithContext(size_t      size,
                                         GblContext* pCtx)
{
    void* pPtr = NULL;
    const size_t  totalSize = size + sizeof(GblRef_);
    GblRef_* pMemory = NULL;
    GBL_CTX_BEGIN(pCtx);
    GBL_CTX_VERIFY_ARG(size);
    pMemory = (GblRef_*)GBL_CTX_MALLOC(totalSize);
    if(pMemory) {
        memset(pMemory, 0, totalSize);
        pMemory->pCtx = pCtx;
        GBL_ATOMIC_INT16_INIT(pMemory->refCount, 1);
        GBL_ATOMIC_INT16_INC(activeCount_);
        pPtr = (pMemory + 1);
    }
    GBL_CTX_END_BLOCK();
    return pPtr;
}

GBL_EXPORT GblRefCount GblRef_releaseWithDtor(const void* pData,
                                              GblRefDestructFn pFnDtor)
{
    GblRefCount refCount = 0;
    if(pData) {
        GblRef_* pHeader = GblRef_header_(pData);
        GBL_CTX_BEGIN(pHeader->pCtx);
        if(!(refCount = (GBL_ATOMIC_INT16_DEC(pHeader->refCount) - 1))) {
            GBL_CTX_VERIFY(GblRef_activeCount(),
                           GBL_RESULT_ERROR_INVALID_HANDLE,
                           "Attempting to release a reference with 0 active refs!");
            if(pFnDtor) {
                GBL_RESULT dtorResult = pFnDtor(pData);
                if(!GBL_RESULT_SUCCESS(dtorResult)) {
                    GBL_CTX_WARN("Failed to destruct last reference, leaking!");
                    refCount = GBL_ATOMIC_INT16_INC(pHeader->refCount) + 1;
                    GBL_CTX_RECORD_SET(dtorResult, "GblRef_release(): Destructor failed!");
                    GBL_CTX_DONE();
                }
            }
            GBL_CTX_FREE(pHeader);
            GBL_ATOMIC_INT16_DEC(activeCount_);
        }
        GBL_CTX_END_BLOCK();
    }
    return refCount;
}

GBL_EXPORT void* GblRef_alloc(size_t  size) GBL_NOEXCEPT {
    return GblRef_allocWithContext(size, NULL);
}

GBL_EXPORT void* GblRef_acquire(const void* pSelf) GBL_NOEXCEPT
{
    void* pRetData = NULL;
    if(pSelf) {
        GblRef_* pHeader = GblRef_header_(pSelf);
        GBL_ATOMIC_INT16_INC(pHeader->refCount);
        pRetData = pSelf;
    }
    return pRetData;
}

GBL_EXPORT GblRefCount GblRef_release(const void* pSelf) GBL_NOEXCEPT {
    return GblRef_releaseWithDtor(pSelf, NULL);
}

GBL_EXPORT GblRefCount GblRef_refCount(const void* pSelf) GBL_NOEXCEPT {
    GblRefCount refCount = 0;
    if(pSelf) {
        GblRef_* pHeader = GblRef_header_(pSelf);
        refCount = GBL_ATOMIC_INT16_LOAD(pHeader->refCount);
    }
    return refCount;
}

GBL_EXPORT GblContext* GblRef_context(const void* pSelf) GBL_NOEXCEPT {
    GblContext* pCtx = NULL;
    if(pSelf) {
        GblRef_* pHeader = GblRef_header_(pSelf);
        pCtx = pHeader->pCtx;
    }
    return pCtx;
}


#include <gimbal/containers/gimbal_ring_buffer.h>


GBL_INLINE size_t GblRingBuffer_mappedIndex_(const GblRingBuffer* pSelf, size_t  index) {
    return (GBL_PRIV_REF(pSelf).frontPos + index) % GBL_PRIV_REF(pSelf).capacity;
}

GBL_INLINE void GblRingBuffer_advance_(GblRingBuffer* pSelf) GBL_NOEXCEPT {
    if(GblRingBuffer_full(pSelf)) {
        ++GBL_PRIV_REF(pSelf).frontPos;
        GBL_PRIV_REF(pSelf).frontPos %= GBL_PRIV_REF(pSelf).capacity;
    } else {
        ++GBL_PRIV_REF(pSelf).size;
    }
}
/// \endcond

GBL_EXPORT GBL_RESULT GblRingBuffer_construct_5(GblRingBuffer* pSelf,
                                                uint16_t    elementSize,
                                                size_t      capacity,
                                                size_t      initialSize,
                                                const void* pInitialData) GBL_NOEXCEPT
{
    return GblRingBuffer_construct_6(pSelf, elementSize, capacity, initialSize, pInitialData, GBL_NULL);
}

GBL_EXPORT GBL_RESULT GblRingBuffer_construct_4(GblRingBuffer* pSelf,
                                                uint16_t   elementSize,
                                                size_t     capacity,
                                                size_t     initialSize) GBL_NOEXCEPT
{
    return GblRingBuffer_construct_5(pSelf, elementSize, capacity, initialSize, GBL_NULL);
}

GBL_EXPORT GBL_RESULT GblRingBuffer_construct_3(GblRingBuffer* pSelf,
                                                uint16_t   elementSize,
                                                size_t     capacity)        GBL_NOEXCEPT
{
    return GblRingBuffer_construct_4(pSelf, elementSize, capacity, 0);
}

GBL_EXPORT GblContext* GblRingBuffer_context(const GblRingBuffer* pSelf) GBL_NOEXCEPT {
    return GBL_PRIV_REF(pSelf).pCtx;
}

GBL_EXPORT size_t  GblRingBuffer_capacity(const GblRingBuffer* pSelf) GBL_NOEXCEPT {
    return GBL_PRIV_REF(pSelf).capacity;
}

GBL_EXPORT size_t  GblRingBuffer_size(const GblRingBuffer* pSelf) GBL_NOEXCEPT {
    return GBL_PRIV_REF(pSelf).size;
}

GBL_EXPORT size_t  GblRingBuffer_elementSize(const GblRingBuffer* pSelf) GBL_NOEXCEPT {
    return GBL_PRIV_REF(pSelf).elementSize;
}

GBL_EXPORT GblBool GblRingBuffer_empty(const GblRingBuffer* pSelf) GBL_NOEXCEPT {
    return GblRingBuffer_size(pSelf) == 0? GBL_TRUE : GBL_FALSE;
}

GBL_EXPORT GblBool GblRingBuffer_full(const GblRingBuffer* pSelf) GBL_NOEXCEPT {
    return GblRingBuffer_size(pSelf) == GBL_PRIV_REF(pSelf).capacity? GBL_TRUE : GBL_FALSE;
}

GBL_EXPORT void* GblRingBuffer_at(const GblRingBuffer* pSelf, size_t  index) GBL_NOEXCEPT {
    const size_t  size = GblRingBuffer_size(pSelf);
    void* pData = GBL_NULL;

    if GBL_UNLIKELY(index >= size) {
        GBL_CTX_BEGIN(GBL_PRIV_REF(pSelf).pCtx);
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_OUT_OF_RANGE);
        GBL_CTX_END_BLOCK();
    } else {
        pData = &GBL_PRIV_REF(pSelf).pData[GblRingBuffer_mappedIndex_(pSelf, index) *
                                           GBL_PRIV_REF(pSelf).elementSize];
    }

    return pData;
}

GBL_EXPORT void* GblRingBuffer_front(const GblRingBuffer* pSelf) GBL_NOEXCEPT {
    return GblRingBuffer_at(pSelf, 0);
}

GBL_EXPORT void* GblRingBuffer_back(const GblRingBuffer* pSelf) GBL_NOEXCEPT {
    return GblRingBuffer_at(pSelf, GBL_PRIV_REF(pSelf).size-1);
}


GBL_EXPORT void* GblRingBuffer_emplaceBack(GblRingBuffer* pSelf) GBL_NOEXCEPT {
    GblRingBuffer_advance_(pSelf);
    return GblRingBuffer_back(pSelf);
}

GBL_EXPORT GBL_RESULT GblRingBuffer_pushBack(GblRingBuffer* pSelf, const void* pData) GBL_NOEXCEPT {
    GBL_RESULT result = GBL_RESULT_SUCCESS;
    void* pBuffer = GblRingBuffer_emplaceBack(pSelf);

    if GBL_LIKELY(pBuffer)
        memcpy(pBuffer, pData, GBL_PRIV_REF(pSelf).elementSize);
    else
        result = GblThd_callRecord(GBL_NULL)->result;

    return result;
}

GBL_EXPORT void* GblRingBuffer_popFront(GblRingBuffer* pSelf) GBL_NOEXCEPT {
    void* pFront = GblRingBuffer_front(pSelf);
    if(pFront) {
        --GBL_PRIV_REF(pSelf).size;
        ++GBL_PRIV_REF(pSelf).frontPos;
        if(GBL_PRIV_REF(pSelf).frontPos >= GBL_PRIV_REF(pSelf).capacity)
            GBL_PRIV_REF(pSelf).frontPos = 0;
    } else {
        GBL_CTX_BEGIN(GBL_PRIV_REF(pSelf).pCtx);
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_OUT_OF_RANGE);
        GBL_CTX_END_BLOCK();
    }
    return pFront;
}

GBL_EXPORT void GblRingBuffer_clear(GblRingBuffer* pSelf) GBL_NOEXCEPT {
    GBL_PRIV_REF(pSelf).frontPos = 0;
    GBL_PRIV_REF(pSelf).size     = 0;
}

GBL_EXPORT GBL_RESULT GblRingBuffer_construct_6(GblRingBuffer* pSelf,
                                                uint16_t       elementSize,
                                                size_t         capacity,
                                                size_t         initialSize,
                                                const void*    pInitialData,
                                                GblContext*    pCtx)
{
    GBL_CTX_BEGIN(pCtx);
    GBL_CTX_VERIFY_POINTER(pSelf);
    GBL_CTX_VERIFY_ARG(elementSize > 0);
    //GBL_CTX_VERIFY_ARG(capacity > 0);

    memset(pSelf, 0, sizeof(GblRingBuffer));

    GBL_PRIV_REF(pSelf).pCtx        = pCtx;
    GBL_PRIV_REF(pSelf).elementSize = elementSize;
    GBL_PRIV_REF(pSelf).capacity    = capacity;

    if(capacity)
        GBL_PRIV_REF(pSelf).pData       = GBL_CTX_MALLOC(capacity * elementSize, 0, "GblRingBuffer");

    for(size_t  i = 0; i < initialSize; ++i) {
        GBL_CTX_VERIFY_CALL(GblRingBuffer_pushBack(pSelf, (void*)((uintptr_t)pInitialData + (elementSize * i))));
    }
    GBL_CTX_END();
}


GBL_EXPORT GBL_RESULT GblRingBuffer_destruct(GblRingBuffer* pSelf) {
    GBL_CTX_BEGIN(GBL_PRIV_REF(pSelf).pCtx);
    GblRingBuffer_clear(pSelf);
    GBL_CTX_FREE(GBL_PRIV_REF(pSelf).pData);
    GBL_PRIV_REF(pSelf).pData = NULL;
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblRingBuffer_copy(GblRingBuffer* pSelf,
                                         const GblRingBuffer* pOther)
{
    GBL_CTX_BEGIN(GBL_PRIV_REF(pOther).pCtx);
    GBL_CTX_VERIFY_CALL(GblRingBuffer_destruct(pSelf));

    GBL_CTX_VERIFY_CALL(GblRingBuffer_construct_6(pSelf,
                                                  GBL_PRIV_REF(pOther).elementSize,
                                                  GBL_PRIV_REF(pOther).capacity,
                                                  0,
                                                  NULL,
                                                  GBL_PRIV_REF(pOther).pCtx));

    memcpy(GBL_PRIV_REF(pSelf).pData,
           GBL_PRIV_REF(pOther).pData,
           GBL_PRIV_REF(pSelf).elementSize * GBL_PRIV_REF(pSelf).capacity);

    GBL_PRIV_REF(pSelf).size = GBL_PRIV_REF(pOther).size;

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblRingBuffer_move(GblRingBuffer* pSelf,
                                         GblRingBuffer* pOther)
{
    GBL_CTX_BEGIN(GBL_PRIV_REF(pOther).pCtx);
    GBL_CTX_VERIFY_CALL(GblRingBuffer_destruct(pSelf));

    memcpy(pSelf, pOther, sizeof(GblRingBuffer));
    GBL_PRIV_REF(pOther).pData = NULL;
    GBL_PRIV_REF(pOther).size = 0;
    GBL_PRIV_REF(pOther).capacity = 0;

    GBL_CTX_END();
}

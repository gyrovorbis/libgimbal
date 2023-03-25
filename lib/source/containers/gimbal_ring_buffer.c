#include <gimbal/containers/gimbal_ring_buffer.h>

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

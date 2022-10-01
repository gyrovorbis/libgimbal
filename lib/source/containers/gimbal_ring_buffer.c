#include <gimbal/containers/gimbal_ring_buffer.h>

GBL_EXPORT GBL_RESULT GblRingBuffer_construct_6(GblRingBuffer* pSelf,
                                                uint16_t       elementSize,
                                                GblSize        capacity,
                                                GblSize        initialSize,
                                                const void*    pInitialData,
                                                GblContext*    pCtx)
{
    GBL_API_BEGIN(pCtx);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_ARG(elementSize > 0);
    GBL_API_VERIFY_ARG(capacity > 0);

    memset(pSelf, 0, sizeof(GblRingBuffer));

    GBL_PRIV_REF(pSelf).pCtx        = pCtx;
    GBL_PRIV_REF(pSelf).elementSize = elementSize;
    GBL_PRIV_REF(pSelf).capacity    = capacity;

    GBL_PRIV_REF(pSelf).pData       = GBL_API_MALLOC(capacity * elementSize, 0, "GblRingBuffer");

    for(GblSize i = 0; i < initialSize; ++i) {
        GBL_API_VERIFY_CALL(GblRingBuffer_pushBack(pSelf, pInitialData + (elementSize * i)));
    }
    GBL_API_END();
}


GBL_EXPORT GBL_RESULT GblRingBuffer_destruct(GblRingBuffer* pSelf) {
    GBL_API_BEGIN(GBL_PRIV_REF(pSelf).pCtx);
    GblRingBuffer_clear(pSelf);
    GBL_API_FREE(GBL_PRIV_REF(pSelf).pData);
    GBL_PRIV_REF(pSelf).pData = NULL;
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblRingBuffer_copy(GblRingBuffer* pSelf,
                                         const GblRingBuffer* pOther)
{
    GBL_API_BEGIN(GBL_PRIV_REF(pOther).pCtx);
    GBL_API_VERIFY_CALL(GblRingBuffer_destruct(pSelf));

    GBL_API_VERIFY_CALL(GblRingBuffer_construct_6(pSelf,
                                                  GBL_PRIV_REF(pOther).elementSize,
                                                  GBL_PRIV_REF(pOther).capacity,
                                                  0,
                                                  NULL,
                                                  GBL_PRIV_REF(pOther).pCtx));

    memcpy(GBL_PRIV_REF(pSelf).pData,
           GBL_PRIV_REF(pOther).pData,
           GBL_PRIV_REF(pSelf).elementSize * GBL_PRIV_REF(pSelf).capacity);

    GBL_PRIV_REF(pSelf).size = GBL_PRIV_REF(pOther).size;

    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblRingBuffer_move(GblRingBuffer* pSelf,
                                         GblRingBuffer* pOther)
{
    GBL_API_BEGIN(GBL_PRIV_REF(pOther).pCtx);
    GBL_API_VERIFY_CALL(GblRingBuffer_destruct(pSelf));

    memcpy(pSelf, pOther, sizeof(GblRingBuffer));
    memset(pOther, 0, sizeof(GblRingBuffer));

    GBL_API_END();
}
#if 0
GBL_EXPORT GBL_RESULT GblRingBuffer_reserve(GblRingBuffer* pSelf,
                                            GblSize        capacity)
{
    GBL_API_BEGIN(GBL_PRIV_REF(pSelf).pCtx);

    if(GBL_PRIV_REF(pSelf).capacity < capacity) {
        const GblSize newCapacityBytes = capacity * GBL_PRIV_REF(pSelf).elementSize;
        const GblSize oldSize = GblRingBuffer_size(pSelf);

        GBL_PRIV_REF(pSelf).capacity = capacity;

        if(GBL_PRIV_REF(pSelf).pData) {
            GBL_PRIV_REF(pSelf).pData = GBL_API_REALLOC(GBL_PRIV_REF(pSelf).pData, newCapacityBytes);

            if(GBL_PRIV_REF(pSelf).backPos < GBL_PRIV_REF(pSelf).frontPos) {

                GblSize newChunkSize = oldSize + GBL_PRIV_REF(pSelf).backPos;
                const GblSize frontChunkSize = newChunkSize % capacity;
                if(newChunkSize > capacity) newChunkSize = capacity;

                GBL_PRIV_REF(pSelf).backPos = frontChunkSize;

                memcpy(&GBL_PRIV_REF(pSelf).pData[oldSize + GBL_PRIV_REF(pSelf).elementSize],
                       GBL_PRIV_REF(pSelf).pData,
                       newChunkSize * GBL_PRIV_REF(pSelf).elementSize);

                if(frontChunkSize) {
                    const GblSize frontBytes = frontChunkSize * GBL_PRIV_REF(pSelf).elementSize;

                    memcpy(GBL_PRIV_REF(pSelf).pData,
                           &GBL_PRIV_REF(pSelf).pData[frontBytes],
                           frontBytes);
                }

            }
        } else {
            GBL_PRIV_REF(pSelf).pData = GBL_API_MALLOC(newCapacityBytes, 0, "GblRingBuffer");
        }

    }

    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblRingBuffer_resize(GblRingBuffer* pSelf, GblSize size) {
    const GblSize oldSize = GblRingBuffer_size(pSelf);

    if(size != oldSize) {
        GBL_API_BEGIN(GBL_PRIV_REF(pSelf).pCtx);
        if(size > GBL_PRIV_REF(pSelf).capacity) {
            GBL_API_VERIFY_CALL(GblRingBuffer_reserve(pSelf, size));

        } else if(size > oldSize) {
            for(GblSize i = 0; i < (size - oldSize); ++i)
                GblRingBuffer_advance_(pSelf);

        } else if(size < oldSize) {
            GBL_PRIV_REF(pSelf).size -= (oldSize - size);
            GBL_PRIV_REF(pSelf).frontPos += (oldSize - size);
            GBL_PRIV_REF(pSelf).frontPos %= GBL_PRIV_REF(pSelf).capacity;
        }

        GBL_API_END();

    } else return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT GblRingBuffer_shrinkToFit(GblRingBuffer* pSelf) {
    const GblSize oldSize = GblRingBuffer_size(pSelf);

    if(oldSize < GBL_PRIV_REF(pSelf).capacity) {
        GBL_API_BEGIN(GBL_PRIV_REF(pSelf).pCtx);


        GBL_API_END();

    } else return GBL_RESULT_SUCCESS;
}
#endif

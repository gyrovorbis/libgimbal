#include <gimbal/containers/gimbal_array_deque.h>

#define GBL_RING_SELF_(dequeSelf)      (&GBL_PRIV_REF(dequeSelf).ringBuffer)
#define GBL_RING_PRIV_REF_(dequeSelf)  (GBL_PRIV_REF(GBL_RING_SELF_(dequeSelf)))

static GblSize GblArrayDeque_endSize_(const GblArrayDeque* pSelf) {
    return GBL_RING_PRIV_REF_(pSelf).frontPos?
                (GBL_RING_PRIV_REF_(pSelf).capacity - GBL_RING_PRIV_REF_(pSelf).frontPos) : 0;
}

static GBL_RESULT GblArrayDeque_reserve_(GblArrayDeque* pSelf,
                                         GblSize       capacity,
                                         GblBool       exact)
{
    if(GBL_RING_PRIV_REF_(pSelf).capacity < capacity) {
        GBL_API_BEGIN(GBL_RING_PRIV_REF_(pSelf).pCtx);

        if(!exact && capacity > 2)
            capacity = gblPow2Next(capacity);
        const GblSize newCapacityBytes = capacity * GBL_RING_PRIV_REF_(pSelf).elementSize;

        if(GBL_RING_PRIV_REF_(pSelf).pData) {
            GBL_RING_PRIV_REF_(pSelf).pData = GBL_API_REALLOC(GBL_RING_PRIV_REF_(pSelf).pData,
                                                              newCapacityBytes);

            // Have to move "end" chunk to end of new allocation
            const GblSize endSize = GblArrayDeque_endSize_(pSelf);
            if(endSize) {
                const GblSize newFrontPos = capacity - endSize;
                memcpy((void*)((uintptr_t)GBL_RING_PRIV_REF_(pSelf).pData +
                                (newFrontPos * GblArrayDeque_elementSize(pSelf))),
                        GblArrayDeque_front(pSelf),
                        endSize * GblArrayDeque_elementSize(pSelf));

                GBL_RING_PRIV_REF_(pSelf).frontPos = newFrontPos;
            }

        } else {
            GBL_RING_PRIV_REF_(pSelf).pData = GBL_API_MALLOC(newCapacityBytes, 0, "GblArrayDeque");
        }

        GBL_RING_PRIV_REF_(pSelf).capacity = capacity;

        GBL_API_END();
    }
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT (GblArrayDeque_construct)(GblArrayDeque* pSelf,
                                                uint16_t       elementSize,
                                                GblSize        capacity,
                                                GblSize        initialSize,
                                                const void*    pInitialData,
                                                GblContext*    pCtx) GBL_NOEXCEPT
{
    if(capacity < initialSize) capacity = initialSize;

    return GblRingBuffer_construct(GBL_RING_SELF_(pSelf),
                                   elementSize,
                                   capacity,
                                   initialSize,
                                   pInitialData,
                                   pCtx);
}


GBL_EXPORT GBL_RESULT GblArrayDeque_pushFront(GblArrayDeque* pSelf, const void* pData) {
    void* pEntry = GblArrayDeque_emplaceFront(pSelf);
    if(pEntry) {
        GBL_ASSERT(pData);
        memcpy(pEntry, pData, GblArrayDeque_elementSize(pSelf));
        return GBL_RESULT_SUCCESS;
    } else return GblThread_callRecord(NULL)->result;
}

GBL_EXPORT void* GblArrayDeque_emplaceFront(GblArrayDeque* pSelf) {
    GblArrayDeque_reserve_(pSelf, GblArrayDeque_size(pSelf) + 1, GBL_FALSE);
    if(GBL_RING_PRIV_REF_(pSelf).frontPos == 0)
        GBL_RING_PRIV_REF_(pSelf).frontPos = GBL_RING_PRIV_REF_(pSelf).capacity - 1;
    else
        --GBL_RING_PRIV_REF_(pSelf).frontPos;
    ++GBL_RING_PRIV_REF_(pSelf).size;
    return GblArrayDeque_front(pSelf);
}

GBL_EXPORT void* GblArrayDeque_popBack(GblArrayDeque* pSelf) {
    void* pSlot = NULL;
    if(GblArrayDeque_empty(pSelf)) {
        GBL_API_BEGIN(NULL);
        GBL_API_RECORD_SET(GBL_RESULT_ERROR_OUT_OF_RANGE);
        GBL_API_END_BLOCK();
    } else {
        pSlot = GblArrayDeque_back(pSelf);
        --GBL_RING_PRIV_REF_(pSelf).size;
    }
    return pSlot;
}

GBL_EXPORT void* (GblArrayDeque_insert)(GblArrayDeque* pSelf, GblSize pos, const void* pData, GblSize count) {
    void* pSlot = NULL;
    GBL_RESULT result = GblArrayDeque_reserve_(pSelf, GblArrayDeque_size(pSelf) + count, GBL_FALSE);
    if(GBL_RESULT_SUCCESS(result)) {
       /* if(pos == 0) { // insert into front
            for(GblSize i = 0; i < count; ++i) {
                GblArrayDeque_pushFront(pSelf, (void*)((uintptr_t)pData + GblArrayDeque_elementSize(pSelf)*i));
            }
        } else if(pos == GblArrayDeque_size(pSelf) - 1) { // insert into back
            for(GblSize i = 0; i < count; ++i) {
                GblArrayDeque_pushBack(pSelf, (void*)((uintptr_t)pData + GblArrayDeque_elementSize(pSelf)*i));
            }
        } else */if(pos <= GblArrayDeque_size(pSelf)){ // insert into middle

            const GblSize wrapIndex = GblArrayDeque_endSize_(pSelf);
            const GblSize oldSize = GblArrayDeque_size(pSelf);
            GblArrayDeque_resize(pSelf, oldSize + count);
            if(pos >= wrapIndex) {  // inserting into begin segment

                if(pos < oldSize)
                    memmove(GblArrayDeque_at(pSelf, pos+count),
                            GblArrayDeque_at(pSelf, pos),
                            GblArrayDeque_elementSize(pSelf) * count);


            } else { // inserting into end segment

                memmove((void*)(((uintptr_t)GBL_RING_PRIV_REF_(pSelf).pData +
                        (GBL_RING_PRIV_REF_(pSelf).frontPos - count) * GBL_RING_PRIV_REF_(pSelf).elementSize)),
                        GblArrayDeque_front(pSelf),
                        GblArrayDeque_elementSize(pSelf) * (pos+count));

                GBL_RING_PRIV_REF_(pSelf).frontPos -= count;

            }

            pSlot = GblArrayDeque_at(pSelf, pos);

            memcpy(pSlot,
                   pData,
                   GblArrayDeque_elementSize(pSelf) * count);

        } else {
            GBL_API_BEGIN(GBL_RING_PRIV_REF_(pSelf).pCtx);
            GBL_API_RECORD_SET(GBL_RESULT_ERROR_OUT_OF_RANGE);
            GBL_API_END_BLOCK();
        }
    }
    return pSlot;
}

GBL_EXPORT void* GblArrayDeque_emplace(GblArrayDeque* pSelf, GblSize pos) {

}

GBL_EXPORT GBL_RESULT (GblArrayDeque_erase)(GblArrayDeque* pSelf, GblSize pos, GblSize count) {

}

GBL_EXPORT void* GblArrayDeque_emplaceBack(GblArrayDeque* pSelf) GBL_NOEXCEPT {
    void* pSlot = GBL_NULL;
    GBL_RESULT result = GblArrayDeque_reserve_(pSelf, GblArrayDeque_size(pSelf) + 1, GBL_FALSE);
    if(GBL_RESULT_SUCCESS(result)) {
        pSlot = GblRingBuffer_emplaceBack(GBL_RING_SELF_(pSelf));
    }
    return pSlot;
}

GBL_EXPORT GBL_RESULT GblArrayDeque_reserve(GblArrayDeque* pSelf,
                                            GblSize        capacity)
{
    return GblArrayDeque_reserve_(pSelf, capacity, GBL_TRUE);

}

GBL_EXPORT GBL_RESULT GblArrayDeque_resize(GblArrayDeque* pSelf, GblSize size) {
    const GblSize oldSize = GblArrayDeque_size(pSelf);

    if(size != oldSize) {
        GBL_API_BEGIN(GBL_RING_PRIV_REF_(pSelf).pCtx);
        if(size > GBL_RING_PRIV_REF_(pSelf).capacity) {
            GBL_API_VERIFY_CALL(GblArrayDeque_reserve_(pSelf, size, GBL_FALSE));

        } else if(size > oldSize) {
           GBL_RING_PRIV_REF_(pSelf).size += (size - oldSize);

        } else if(size < oldSize) {
            GBL_RING_PRIV_REF_(pSelf).size -= (oldSize - size);
        }

        GBL_API_END();

    } else return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT GblArrayDeque_shrinkToFit(GblArrayDeque* pSelf) {
    const GblSize oldSize = GblArrayDeque_size(pSelf);

    if(oldSize < GBL_RING_PRIV_REF_(pSelf).capacity) {
        GBL_API_BEGIN(GBL_RING_PRIV_REF_(pSelf).pCtx);
        const GblSize endSize = GblArrayDeque_endSize_(pSelf);

        if(endSize) {
            const GblSize newFront = GBL_RING_PRIV_REF_(pSelf).frontPos -
                    (GblArrayDeque_capacity(pSelf) - GblArrayDeque_size(pSelf));

            memmove(GBL_RING_PRIV_REF_(pSelf).pData + (newFront * GBL_RING_PRIV_REF_(pSelf).elementSize),
                    GblArrayDeque_front(pSelf),
                    endSize * GBL_RING_PRIV_REF_(pSelf).elementSize);

            GBL_RING_PRIV_REF_(pSelf).frontPos = newFront;

            GBL_RING_PRIV_REF_(pSelf).pData = GBL_API_REALLOC(GBL_RING_PRIV_REF_(pSelf).pData,
                                                              GBL_RING_PRIV_REF_(pSelf).size * GBL_RING_PRIV_REF_(pSelf).elementSize);

            GBL_RING_PRIV_REF_(pSelf).capacity = GBL_RING_PRIV_REF_(pSelf).size;
        }

        GBL_API_END();

    } else return GBL_RESULT_SUCCESS;
}

GBL_EXPORT void GblArrayDeque_clear(GblArrayDeque* pSelf) GBL_NOEXCEPT {
    GBL_API_BEGIN(GBL_RING_PRIV_REF_(pSelf).pCtx);
    GblRingBuffer_clear(GBL_RING_SELF_(pSelf));
    GBL_API_FREE(GBL_RING_PRIV_REF_(pSelf).pData);
    GBL_RING_PRIV_REF_(pSelf).pData = NULL;
    GBL_RING_PRIV_REF_(pSelf).capacity = 0;
    GBL_API_END_BLOCK();
}


#include <gimbal/containers/gimbal_array_deque.h>

#define GBL_RING_SELF_(dequeSelf)      (&GBL_PRIV_REF(dequeSelf).ringBuffer)
#define GBL_RING_PRIV_REF_(dequeSelf)  (GBL_PRIV_REF(GBL_RING_SELF_(dequeSelf)))

static size_t  GblArrayDeque_endSize_(const GblArrayDeque* pSelf) {
    return GBL_RING_PRIV_REF_(pSelf).frontPos?
                (GBL_RING_PRIV_REF_(pSelf).capacity - GBL_RING_PRIV_REF_(pSelf).frontPos) : 0;
}

static GBL_RESULT GblArrayDeque_reserve_(GblArrayDeque* pSelf,
                                         size_t        capacity,
                                         GblBool       exact)
{
    if(GBL_RING_PRIV_REF_(pSelf).capacity < capacity) {
        GBL_CTX_BEGIN(GBL_RING_PRIV_REF_(pSelf).pCtx);

        if(!exact && capacity > 2)
            capacity = gblPow2Next(capacity);
        const size_t  newCapacityBytes = capacity * GBL_RING_PRIV_REF_(pSelf).elementSize;

        if(GBL_RING_PRIV_REF_(pSelf).pData) {
            GBL_RING_PRIV_REF_(pSelf).pData = GBL_CTX_REALLOC(GBL_RING_PRIV_REF_(pSelf).pData,
                                                              newCapacityBytes);

            // Have to move "end" chunk to end of new allocation
            const size_t  endSize = GblArrayDeque_endSize_(pSelf);
            if(endSize) {
                const size_t  newFrontPos = capacity - endSize;
                memmove((void*)((uintptr_t)GBL_RING_PRIV_REF_(pSelf).pData +
                                (newFrontPos * GblArrayDeque_elementSize(pSelf))),
                        GblArrayDeque_front(pSelf),
                        endSize * GblArrayDeque_elementSize(pSelf));

                GBL_RING_PRIV_REF_(pSelf).frontPos = newFrontPos;
            }

        } else {
            GBL_RING_PRIV_REF_(pSelf).pData = GBL_CTX_MALLOC(newCapacityBytes, 0, "GblArrayDeque");
        }

        GBL_RING_PRIV_REF_(pSelf).capacity = capacity;

        GBL_CTX_END();
    }
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT (GblArrayDeque_construct)(GblArrayDeque* pSelf,
                                                uint16_t       elementSize,
                                                size_t         capacity,
                                                size_t         initialSize,
                                                const void*    pInitialData,
                                                GblContext*    pCtx)
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
    } else return GblThd_callRecord(NULL)->result;
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
        GBL_CTX_BEGIN(NULL);
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_OUT_OF_RANGE);
        GBL_CTX_END_BLOCK();
    } else {
        pSlot = GblArrayDeque_back(pSelf);
        --GBL_RING_PRIV_REF_(pSelf).size;
    }
    return pSlot;
}

GBL_EXPORT void* (GblArrayDeque_insert)(GblArrayDeque* pSelf, size_t  pos, const void* pData, size_t  count) {
    void* pSlot = NULL;
    GBL_ASSERT(count);
    GBL_RESULT result = GblArrayDeque_reserve_(pSelf, GblArrayDeque_size(pSelf) + count, GBL_FALSE);
    if(GBL_RESULT_SUCCESS(result)) {

        if(pos <= GblArrayDeque_size(pSelf)){ // insert into middle

            const size_t  wrapIndex = GblArrayDeque_endSize_(pSelf);
            const size_t  oldSize = GblArrayDeque_size(pSelf);
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
            if(pData) {
                memcpy(pSlot,
                       pData,
                       GblArrayDeque_elementSize(pSelf) * count);
            }

        } else {
            GBL_CTX_BEGIN(GBL_RING_PRIV_REF_(pSelf).pCtx);
            GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_OUT_OF_RANGE);
            GBL_CTX_END_BLOCK();
        }
    }
    return pSlot;
}

GBL_EXPORT void* GblArrayDeque_emplace(GblArrayDeque* pSelf, size_t  pos) {
    return GblArrayDeque_insert(pSelf, pos, NULL);
}

GBL_EXPORT GBL_RESULT (GblArrayDeque_erase)(GblArrayDeque* pSelf, size_t  pos, size_t  count) {
    return GBL_RESULT_UNIMPLEMENTED;
#if 0
    GBL_CTX_BEGIN(GBL_RING_PRIV_REF_(pSelf).pCtx);

    GBL_CTX_VERIFY_ARG(count);
    GBL_CTX_VERIFY_ARG(pos+count <= GblArrayDeque_size(pSelf));

    const size_t  wrapIndex = GblArrayDeque_endSize_(pSelf);
    const size_t  oldSize = GblArrayDeque_size(pSelf);

    if(pos < wrapIndex) {
        size_t  endBegin = pos;
        if(endBegin > wrapIndex) endBegin = wrapIndex;
        size_t  endEnd = pos + count;
        if(endEnd > wrapIndex) endEnd = wrapIndex;
        const GblBool endRemoveChunk = endEnd - endBegin;

        // check if we have anything to shave off of the end
        const size_t  endRemovalSize = endEnd - endBegin;
        if(endRemovalSize) {
            const size_t  endSize = GblArrayDeque_endSize_(pSelf);

            // scoot over remaining entries
            if(endSize > endRemovalSize) {



            }

            GBL_RING_PRIV_REF_(pSelf).frontPos =
        }



        memmove((void*)(((uintptr_t)GBL_RING_PRIV_REF_(pSelf).pData +
                (GBL_RING_PRIV_REF_(pSelf).frontPos - count) * GBL_RING_PRIV_REF_(pSelf).elementSize)),
                GblArrayDeque_front(pSelf),
                GblArrayDeque_elementSize(pSelf) * (pos+count));


    }






    GBL_CTX_END();
#endif
}

GBL_EXPORT void* GblArrayDeque_emplaceBack(GblArrayDeque* pSelf)  {
    void* pSlot = GBL_NULL;
    GBL_RESULT result = GblArrayDeque_reserve_(pSelf, GblArrayDeque_size(pSelf) + 1, GBL_FALSE);
    if(GBL_RESULT_SUCCESS(result)) {
        pSlot = GblRingBuffer_emplaceBack(GBL_RING_SELF_(pSelf));
    }
    return pSlot;
}

GBL_EXPORT GBL_RESULT GblArrayDeque_reserve(GblArrayDeque* pSelf,
                                            size_t         capacity)
{
#if GBL_ARRAY_DEQUE_FORCE_POW2 == 1
    return GblArrayDeque_reserve_(pSelf, capacity, GBL_FALSE);
#else
    return GblArrayDeque_reserve_(pSelf, capacity, GBL_TRUE);
#endif

}

GBL_EXPORT GBL_RESULT GblArrayDeque_resize(GblArrayDeque* pSelf, size_t  size) {
    const size_t  oldSize = GblArrayDeque_size(pSelf);

    if(size != oldSize) {
        GBL_CTX_BEGIN(GBL_RING_PRIV_REF_(pSelf).pCtx);
        if(size > GBL_RING_PRIV_REF_(pSelf).capacity) {
            GBL_CTX_VERIFY_CALL(GblArrayDeque_reserve_(pSelf, size, GBL_FALSE));

        } else if(size > oldSize) {
           GBL_RING_PRIV_REF_(pSelf).size += (size - oldSize);

        } else if(size < oldSize) {
            GBL_RING_PRIV_REF_(pSelf).size -= (oldSize - size);
        }

        GBL_CTX_END();

    } else return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT GblArrayDeque_shrinkToFit(GblArrayDeque* pSelf) {
#if GBL_ARRAY_DEQUE_FORCE_POW2 == 1
    const size_t  newSize = gblPow2Next(GblArrayDeque_size(pSelf));
#else
    const size_t  newSize = GblArrayDeque_size(pSelf);
#endif

    if(newSize < GBL_RING_PRIV_REF_(pSelf).capacity) {
        GBL_CTX_BEGIN(GBL_RING_PRIV_REF_(pSelf).pCtx);
        const size_t  endSize = GblArrayDeque_endSize_(pSelf);

        if(endSize) {

            const size_t  newFront = GBL_RING_PRIV_REF_(pSelf).frontPos -
                    (GblArrayDeque_capacity(pSelf) - newSize);

            memmove(GBL_RING_PRIV_REF_(pSelf).pData + (newFront * GBL_RING_PRIV_REF_(pSelf).elementSize),
                    GblArrayDeque_front(pSelf),
                    endSize * GBL_RING_PRIV_REF_(pSelf).elementSize);

            GBL_RING_PRIV_REF_(pSelf).frontPos = newFront;

            GBL_RING_PRIV_REF_(pSelf).pData = GBL_CTX_REALLOC(GBL_RING_PRIV_REF_(pSelf).pData,
                                                              newSize * GBL_RING_PRIV_REF_(pSelf).elementSize);

            GBL_RING_PRIV_REF_(pSelf).capacity = newSize;
        }

        GBL_CTX_END();

    } else return GBL_RESULT_SUCCESS;
}

GBL_EXPORT void GblArrayDeque_clear(GblArrayDeque* pSelf)  {
    GBL_CTX_BEGIN(GBL_RING_PRIV_REF_(pSelf).pCtx);
    GblRingBuffer_clear(GBL_RING_SELF_(pSelf));
    GBL_CTX_FREE(GBL_RING_PRIV_REF_(pSelf).pData);
    GBL_RING_PRIV_REF_(pSelf).pData = NULL;
    GBL_RING_PRIV_REF_(pSelf).capacity = 0;
    GBL_CTX_END_BLOCK();
}

GBL_EXPORT GBL_RESULT GblArrayDeque_destruct(GblArrayDeque* pSelf) {
    return GblRingBuffer_destruct(GBL_RING_SELF_(pSelf));
}

GBL_EXPORT GBL_RESULT GblArrayDeque_copy(GblArrayDeque* pSelf, const GblArrayDeque* pOther) {
    return GblRingBuffer_copy(GBL_RING_SELF_(pSelf), GBL_RING_SELF_(pOther));
}

GBL_EXPORT GBL_RESULT GblArrayDeque_move(GblArrayDeque* pSelf, GblArrayDeque* pOther) {
    return GblRingBuffer_move(GBL_RING_SELF_(pSelf), GBL_RING_SELF_(pOther));
}

GBL_EXPORT GblContext* GblArrayDeque_context(const GblArrayDeque* pSelf) {
    return GblRingBuffer_context(GBL_RING_SELF_(pSelf));
}

GBL_EXPORT size_t  GblArrayDeque_capacity(const GblArrayDeque* pSelf) {
    return GblRingBuffer_capacity(GBL_RING_SELF_(pSelf));
}

GBL_EXPORT size_t  GblArrayDeque_size(const GblArrayDeque* pSelf) {
    return GblRingBuffer_size(GBL_RING_SELF_(pSelf));
}

GBL_EXPORT size_t  GblArrayDeque_elementSize(const GblArrayDeque* pSelf) {
    return GblRingBuffer_elementSize(GBL_RING_SELF_(pSelf));
}

GBL_EXPORT GblBool GblArrayDeque_empty(const GblArrayDeque* pSelf) {
    return GblRingBuffer_empty(GBL_RING_SELF_(pSelf));
}

GBL_EXPORT GblBool GblArrayDeque_full(const GblArrayDeque* pSelf) {
    return GblRingBuffer_full(GBL_RING_SELF_(pSelf));
}

GBL_EXPORT void* GblArrayDeque_at(const GblArrayDeque* pSelf, size_t  index) {
    void* pData = GBL_NULL;

    if(index >= GBL_RING_PRIV_REF_(pSelf).size) GBL_UNLIKELY {
        GBL_CTX_BEGIN(GBL_RING_PRIV_REF_(pSelf).pCtx);
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_OUT_OF_RANGE);
        GBL_CTX_END_BLOCK();
    } else GBL_LIKELY {
#if GBL_ARRAY_DEQUE_FORCE_POW2 == 1
        const size_t  slot = (GBL_RING_PRIV_REF_(pSelf).frontPos + index)
                           & (GBL_RING_PRIV_REF_(pSelf).capacity-1);
#else
        const size_t  slot = (GBL_RING_PRIV_REF_(pSelf).frontPos + index)
                           % (GBL_RING_PRIV_REF_(pSelf).capacity);
#endif
        pData = &GBL_RING_PRIV_REF_(pSelf).pData[slot * GBL_RING_PRIV_REF_(pSelf).elementSize];
    }

    return pData;
}

GBL_EXPORT void* GblArrayDeque_front(const GblArrayDeque* pSelf) {
    return GblRingBuffer_front(GBL_RING_SELF_(pSelf));
}

GBL_EXPORT void* GblArrayDeque_back(const GblArrayDeque* pSelf) {
    return GblRingBuffer_back(GBL_RING_SELF_(pSelf));
}

GBL_EXPORT GBL_RESULT GblArrayDeque_pushBack(GblArrayDeque* pSelf, const void* pData) {
    void* pSlot = GblArrayDeque_emplaceBack(pSelf);
    if(!pSlot) return GblThd_callRecord(GBL_NULL)->result;
    else {
        GBL_ASSERT(pData);
        memcpy(pSlot, pData, GblArrayDeque_elementSize(pSelf));
    }
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT void* GblArrayDeque_popFront(GblArrayDeque* pSelf) {
    return GblRingBuffer_popFront(GBL_RING_SELF_(pSelf));
}



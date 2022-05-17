#ifndef GIMBAL_BYTE_ARRAY_H
#define GIMBAL_BYTE_ARRAY_H

#include "../types/gimbal_typedefs.h"
#include "../core/gimbal_api_frame.h"

#define SELF                GblByteArray*          pSelf
#define CSELF               const GblByteArray*    pSelf

GBL_DECLS_BEGIN

typedef struct GblByteArray {
    GblSize             size;
    void*               pData;
} GblByteArray;

GBL_INLINE GBL_RESULT   GblByteArray_construct_1(SELF)                                                      GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblByteArray_construct_2(SELF, GblSize bytes)                                       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblByteArray_construct_3(SELF, GblSize bytes, const void* pData)                    GBL_NOEXCEPT;
#define GblByteArray_construct(...) \
        GBL_VA_OVERLOAD_SELECT(GblByteArray_construct, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)(__VA_ARGS__)

GBL_INLINE GBL_RESULT   GblByteArray_destruct  (SELF)                                                       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblByteArray_copy      (SELF, const GblByteArray* pOther)                           GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblByteArray_move      (SELF, GblByteArray* pOther)                                 GBL_NOEXCEPT;
GBL_INLINE GblCmpResult GblByteArray_compare   (CSELF, const GblByteArray* pRhs)                            GBL_NOEXCEPT;

GBL_INLINE GblSize      GblByteArray_size      (CSELF)                                                      GBL_NOEXCEPT;
GBL_INLINE GblContext*  GblByteArray_context   (CSELF)                                                      GBL_NOEXCEPT;
GBL_INLINE GblBool      GblByteArray_isEmpty   (CSELF)                                                      GBL_NOEXCEPT;

GBL_INLINE void*        GblByteArray_data      (CSELF)                                                       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblByteArray_dataRead  (CSELF, GblSize offset, GblSize bytes, void* pDataOut)        GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblByteArray_dataWrite (SELF, GblSize offset, GblSize bytes, const void* pDataIn)    GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT   GblByteArray_clear     (SELF)                                                       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblByteArray_acquire   (SELF, GblSize bytes, void* pData)                           GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblByteArray_release   (SELF, GblSize* pSize, void** ppData)                        GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblByteArray_resize    (SELF, GblSize bytes)                                        GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblByteArray_grow      (SELF, GblSize bytes)                                        GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblByteArray_shrink    (SELF, GblSize bytes)                                        GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblByteArray_erase     (SELF, GblSize offset, GblSize bytes)                        GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblByteArray_insert    (SELF, GblSize offset, GblSize bytes, const void* pDataIn)   GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblByteArray_append    (SELF, GblSize bytes, const void* pData)                     GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblByteArray_prepend   (SELF, GblSize bytes, const void* pData)                     GBL_NOEXCEPT;

// ===== IMPL =====

GBL_INLINE GblSize GblByteArray_size(CSELF) GBL_NOEXCEPT {
    return pSelf->size;
}

GBL_INLINE void* GblByteArray_data(CSELF) GBL_NOEXCEPT {
    return pSelf->pData;
}

GBL_INLINE GblBool GblByteArray_isEmpty(CSELF) GBL_NOEXCEPT {
    if(!pSelf)
        return GBL_TRUE;
    if(!pSelf->pData || ! pSelf->size)
        return GBL_TRUE;
    else
        return GBL_FALSE;
}

GBL_INLINE GBL_RESULT GblByteArray_construct_1(SELF) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GBL_API_CALL(GblByteArray_construct_2(pSelf, 0));
    GBL_API_END();
}

GBL_MAYBE_UNUSED GBL_RESULT GblByteArray_construct_2(SELF, GblSize bytes) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GBL_API_CALL(GblByteArray_construct_3(pSelf, bytes, NULL));
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblByteArray_construct_3(SELF, GblSize bytes, const void* pData) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_ARG(!(bytes == 0 && pData != NULL),
                        "Cannot copy buffer of unknown size!");
    pSelf->size     = 0;
    pSelf->pData    = NULL;
    if(bytes) {
        GBL_API_CALL(GblByteArray_resize(pSelf, bytes));
        if(pData) {
            memcpy(GblByteArray_data(pSelf), pData, bytes);
        }
    }
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblByteArray_destruct(SELF) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_CALL(GblByteArray_clear(pSelf));
    GBL_API_END();
}

GBL_INLINE GblCmpResult GblByteArray_compare(CSELF, const GblByteArray* pOther) GBL_NOEXCEPT {
    GblCmpResult result;
    GblCmpResult* pResult = &result;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_POINTER(pOther);
    GBL_API_VERIFY_POINTER(pResult);
    if(GblByteArray_size(pSelf) > GblByteArray_size(pOther))
        *pResult = INT_MAX;
    else if(GblByteArray_size(pSelf) < GblByteArray_size(pOther))
        *pResult = INT_MIN;
    else if(GblByteArray_data(pSelf) == GblByteArray_data(pOther))
        *pResult = 0;
    else if(GblByteArray_data(pSelf) && GblByteArray_data(pOther))
        *pResult =  memcmp(GblByteArray_data(pSelf),
                           GblByteArray_data(pOther),
                           GblByteArray_size(pSelf));
    else if(GblByteArray_data(pSelf))
        *pResult = INT_MAX;
    else
        *pResult = INT_MIN;
    GBL_API_END_BLOCK();
    return result;
}

GBL_INLINE GBL_RESULT GblByteArray_copy(SELF, const GblByteArray* pOther) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_POINTER(pOther);
    GBL_API_CALL(GblByteArray_resize(pSelf, GblByteArray_size(pOther)));
    memcpy(GblByteArray_data(pSelf), GblByteArray_data(pOther), GblByteArray_size(pOther));
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblByteArray_move(SELF, GblByteArray* pOther) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_POINTER(pOther);

    GBL_API_CALL(GblByteArray_clear(pSelf));
    pSelf->size     = pOther->size;
    pSelf->pData    = pOther->pData;
    pOther->size    = 0;
    pOther->pData   = NULL;

    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblByteArray_clear(SELF) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);
    if(pSelf->pData) GBL_API_FREE(pSelf->pData);
    pSelf->pData = NULL;
    pSelf->size = 0;
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblByteArray_resize(SELF, GblSize bytes) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);
    if(!bytes) GBL_API_CALL(GblByteArray_clear(pSelf));
    else {
        if(!pSelf->pData) {
            pSelf->pData = GBL_API_MALLOC(bytes);
        } else {
            pSelf->pData = GBL_API_REALLOC(pSelf->pData, bytes);
        }

        if(bytes > GblByteArray_size(pSelf)) {
            memset((uint8_t*)pSelf->pData + GblByteArray_size(pSelf), 0, bytes - GblByteArray_size(pSelf));
        }

        pSelf->size = bytes;
    }
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblByteArray_acquire(SELF, GblSize bytes, void* pData) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_ARG(bytes > 0);
    GBL_API_VERIFY_POINTER(pData);
    GBL_API_CALL(GblByteArray_clear(pSelf));
    pSelf->size = bytes;
    pSelf->pData = pData;
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblByteArray_release(SELF, GblSize* pSize, void** ppData) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSize);
    GBL_API_VERIFY_POINTER(ppData);
    *pSize  = pSelf->size;
    *ppData = pSelf->pData;
    pSelf->size = 0;
    pSelf->pData = NULL;
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblByteArray_grow(SELF, GblSize bytes) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GblByteArray_resize(pSelf, GblByteArray_size(pSelf) + bytes);
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblByteArray_shrink(SELF, GblSize bytes) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GBL_API_CALL(GblByteArray_resize(pSelf, GblByteArray_size(pSelf) - bytes));
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblByteArray_insert(SELF, GblSize offset, GblSize bytes, const void* pDataIn) GBL_NOEXCEPT {
    uintptr_t insertionPoint = 0;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_ARG(offset <= GblByteArray_size(pSelf));
    GBL_API_CALL(GblByteArray_resize(pSelf, GblByteArray_size(pSelf) + bytes));
    {
        GblInt      slideSize       = GblByteArray_size(pSelf) - offset;
                    insertionPoint  = (uintptr_t)GblByteArray_data(pSelf) + offset;
        memmove((uint8_t*)insertionPoint + bytes, (const uint8_t*)insertionPoint, slideSize);
        if(pDataIn)         memcpy((void*)insertionPoint, pDataIn, bytes);
    }
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblByteArray_append(SELF, GblSize bytes, const void* pData) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_ARG(bytes > 0);
    GBL_API_VERIFY_POINTER(pData);
    {
        GblSize oldSize = GblByteArray_size(pSelf);
        GBL_API_CALL(GblByteArray_grow(pSelf, bytes));
        memcpy((uint8_t*)pSelf->pData + oldSize, pData, bytes - oldSize);
    }
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblByteArray_prepend(SELF, GblSize bytes, const void* pData) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GblByteArray_insert(pSelf, 0, bytes, pData);
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblByteArray_erase(SELF, GblSize offset, GblSize bytes) GBL_NOEXCEPT {
    GblSize lastPos = 0;
    GblSize remainderSize  = 0;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_EXPRESSION(GblByteArray_size(pSelf) > 0);
    GBL_API_VERIFY_ARG(offset < GblByteArray_size(pSelf));
    lastPos = offset + bytes - 1;
    GBL_API_VERIFY_ARG(lastPos < GblByteArray_size(pSelf));
    GBL_API_VERIFY_ARG(offset <= lastPos);
    GBL_API_VERIFY_EXPRESSION(bytes > 0 && bytes <= GblByteArray_size(pSelf));
    remainderSize = GblByteArray_size(pSelf) - 1 - lastPos;
    if(remainderSize) memmove((uint8_t*)GblByteArray_data(pSelf)+offset,
                              (const uint8_t*)GblByteArray_data(pSelf)+lastPos+1,
                              remainderSize);
    GBL_API_CALL(GblByteArray_shrink(pSelf, bytes));
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblByteArray_dataRead(CSELF, GblSize offset, GblSize bytes, void* pDataOut) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY(offset < pSelf->size, GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_API_VERIFY(offset + bytes <= pSelf->size, GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_API_VERIFY_POINTER(pDataOut);
    memcpy(pDataOut, (char*)pSelf->pData + offset, bytes);
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblByteArray_dataWrite(SELF, GblSize offset, GblSize bytes, const void* pDataIn) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY(offset < pSelf->size, GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_API_VERIFY(offset + bytes <= pSelf->size, GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_API_VERIFY_POINTER(pDataIn);
    memcpy((char*)pSelf->pData + offset, pDataIn, bytes);
    GBL_API_END();
}

#undef CSELF
#undef SELF

GBL_DECLS_END


#endif // GIMBAL_BUFFER_H

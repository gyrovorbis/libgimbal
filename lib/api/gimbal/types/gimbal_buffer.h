#ifndef GIMBAL_BUFFER_H
#define GIMBAL_BUFFER_H

#include "../core/gimbal_api_frame.h"
#include "../meta/gimbal_object.h"

#define SELF                GblBuffer*          pSelf
#define CSELF               const GblBuffer*    pSelf
#define GBL_BUFFER_TYPE     (GblBuffer_type())

GBL_DECLS_BEGIN

typedef struct GblBufferClass {
    GblObjectClass base;
} GblBufferClass;

typedef struct GblBuffer {
    union {
        GblBufferClass* pClass;
        GblObject       base;
    };
    GblContext          hCtx;
    GblSize             size;
    void*               pData;
} GblBuffer;


GBL_EXPORT GblType      GblBuffer_type      (void);

GBL_INLINE GBL_RESULT   GblBuffer_construct_1(SELF)                                                     GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblBuffer_construct_2(SELF, GblSize bytes)                                      GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblBuffer_construct_3(SELF, GblSize bytes, const void* pData)                   GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblBuffer_construct_4(SELF, GblSize bytes, const void* pData, GblContext hCtx)  GBL_NOEXCEPT;
#define GblBuffer_construct(...) \
        GBL_VA_OVERLOAD_SELECT(GblBuffer_construct, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)(__VA_ARGS__)

GBL_INLINE GBL_RESULT   GblBuffer_destruct  (SELF)                                                      GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblBuffer_copy      (SELF, const GblBuffer* pOther)                             GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblBuffer_move      (SELF, GblBuffer* pOther)                                   GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblBuffer_compare   (CSELF, const GblBuffer* pRhs, GblCmpResult* pResult)       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblBuffer_convert   (CSELF, GblVariant* pVariant)                               GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblBuffer_debug     (CSELF, GblString* pStr)                                    GBL_NOEXCEPT;

GBL_INLINE GblSize      GblBuffer_size      (CSELF)                                                     GBL_NOEXCEPT;
GBL_INLINE void*        GblBuffer_data      (CSELF)                                                     GBL_NOEXCEPT;
GBL_INLINE GblContext   GblBuffer_context   (CSELF)                                                     GBL_NOEXCEPT;
GBL_INLINE GblBool      GblBuffer_empty     (CSELF)                                                     GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT   GblBuffer_valueRead (CSELF, GblSize offset, GblSize bytes, void* pDataOut)      GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblBuffer_valueWrite(SELF, GblSize offset, GblSize bytes, const void* pDataIn)  GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT   GblBuffer_clear     (SELF)                                                      GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblBuffer_acquire   (SELF, GblSize bytes, void* pData)                          GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblBuffer_release   (SELF, GblSize* pSize, void** ppData)                       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblBuffer_resize    (SELF, GblSize bytes)                                       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblBuffer_grow      (SELF, GblSize bytes)                                       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblBuffer_shrink    (SELF, GblSize bytes)                                       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblBuffer_erase     (SELF, GblSize offset, GblSize bytes)                       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblBuffer_insert    (SELF, GblSize offset, GblSize bytes, const void* pDataIn)  GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblBuffer_append    (SELF, GblSize bytes, const void* pData)                    GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT   GblBuffer_prepend   (SELF, GblSize bytes, const void* pData)                    GBL_NOEXCEPT;





// ===== INLINE IMPLEMENTATION =====

GBL_MAYBE_UNUSED GBL_INLINE
GblSize GblBuffer_size(CSELF) GBL_NOEXCEPT {
    return pSelf->size;
}

GBL_MAYBE_UNUSED GBL_INLINE
void* GblBuffer_data(CSELF) GBL_NOEXCEPT {
    return pSelf->pData;
}

GBL_MAYBE_UNUSED GBL_INLINE
GblContext GblBuffer_context(CSELF) GBL_NOEXCEPT {
    return pSelf->hCtx;
}

GBL_MAYBE_UNUSED GBL_INLINE
GblBool GblBuffer_empty(CSELF) GBL_NOEXCEPT {
    if(!pSelf)
        return GBL_TRUE;
    if(!pSelf->pData || ! pSelf->size)
        return GBL_TRUE;
    else
        return GBL_FALSE;
}

GBL_MAYBE_UNUSED GBL_INLINE
GBL_RESULT GblBuffer_construct_1(SELF) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->hCtx);
    GBL_API_CALL(GblBuffer_construct_2(pSelf, 0));
    GBL_API_END();
}

GBL_MAYBE_UNUSED GBL_INLINE
GBL_RESULT GblBuffer_construct_2(SELF, GblSize bytes) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->hCtx);
    GBL_API_CALL(GblBuffer_construct_3(pSelf, bytes, NULL));
    GBL_API_END();
}

GBL_MAYBE_UNUSED GBL_INLINE
GBL_RESULT GblBuffer_construct_3(SELF, GblSize bytes, const void* pData) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->hCtx);
    GBL_API_CALL(GblBuffer_construct_4(pSelf, bytes, pData, NULL));
    GBL_API_END();
}

GBL_MAYBE_UNUSED
GBL_INLINE GBL_RESULT   GblBuffer_construct_4(SELF, GblSize bytes, const void* pData, GblContext hCtx)  GBL_NOEXCEPT {
    GBL_API_BEGIN(hCtx);
    GBL_API_VERIFY_EXPRESSION(!(bytes == 0 && pData != NULL),
                              "Cannot copy buffer of unknown size!");
    pSelf->size     = 0;
    pSelf->pData    = NULL;
    pSelf->hCtx     = GBL_API_CONTEXT();
    if(bytes) {
        GBL_API_CALL(GblBuffer_resize(pSelf, bytes));
        if(pData) {
            memcpy(GblBuffer_data(pSelf), pData, bytes);
        }
    }
    GBL_API_END();
}

GBL_MAYBE_UNUSED GBL_INLINE
GBL_RESULT GblBuffer_destruct(SELF) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->hCtx);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_CALL(GblBuffer_clear(pSelf));
    GBL_API_END();
}

GBL_MAYBE_UNUSED GBL_INLINE
GBL_RESULT GblBuffer_compare(CSELF, const GblBuffer* pOther, GblCmpResult* pResult) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->hCtx);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_POINTER(pOther);
    GBL_API_VERIFY_POINTER(pResult);
    if(GblBuffer_size(pSelf) > GblBuffer_size(pOther))
        *pResult = INT_MAX;
    else if(GblBuffer_size(pSelf) < GblBuffer_size(pOther))
        *pResult = INT_MIN;
    else if(GblBuffer_data(pSelf) == GblBuffer_data(pOther))
        *pResult = 0;
    else if(GblBuffer_data(pSelf) && GblBuffer_data(pOther))
        *pResult =  memcmp(GblBuffer_data(pSelf),
                           GblBuffer_data(pOther),
                           GblBuffer_size(pSelf));
    else if(GblBuffer_data(pSelf))
        *pResult = INT_MAX;
    else
        *pResult = INT_MIN;
    GBL_API_END();
}

GBL_MAYBE_UNUSED GBL_INLINE
GBL_RESULT GblBuffer_copy(SELF, const GblBuffer* pOther) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->hCtx);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_POINTER(pOther);
    GBL_API_CALL(GblBuffer_resize(pSelf, GblBuffer_size(pOther)));
    memcpy(GblBuffer_data(pSelf), GblBuffer_data(pOther), GblBuffer_size(pOther));
    GBL_API_END();
}

GBL_MAYBE_UNUSED GBL_INLINE
GBL_RESULT GblBuffer_move(SELF, GblBuffer* pOther) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->hCtx);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_POINTER(pOther);

    if(pSelf->hCtx == pOther->hCtx) {
        GBL_API_CALL(GblBuffer_clear(pSelf));
        pSelf->size     = pOther->size;
        pSelf->pData    = pOther->pData;
        pOther->size    = 0;
        pOther->pData   = NULL;
    } else {
        GBL_API_CALL(GblBuffer_copy(pSelf, pOther));
    }

    GBL_API_END();
}

GBL_MAYBE_UNUSED GBL_INLINE
GBL_RESULT GblBuffer_clear(SELF) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->hCtx);
    GBL_API_VERIFY_POINTER(pSelf);
    if(pSelf->pData) GBL_API_FREE(pSelf->pData);
    pSelf->pData = NULL;
    pSelf->size = 0;
    GBL_API_END();
}

GBL_MAYBE_UNUSED GBL_INLINE
GBL_RESULT GblBuffer_resize(SELF, GblSize bytes) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->hCtx);
    GBL_API_VERIFY_POINTER(pSelf);
    if(!bytes) GBL_API_CALL(GblBuffer_clear(pSelf));
    else {
        if(!pSelf->pData) {
            pSelf->pData = GBL_API_MALLOC(bytes);
        } else {
            pSelf->pData = GBL_API_REALLOC(pSelf->pData, bytes);
        }

        if(bytes > GblBuffer_size(pSelf)) {
            memset((uint8_t*)pSelf->pData + GblBuffer_size(pSelf), 0, bytes - GblBuffer_size(pSelf));
        }

        pSelf->size = bytes;
    }
    GBL_API_END();
}

GBL_MAYBE_UNUSED GBL_INLINE
GBL_RESULT GblBuffer_acquire(SELF, GblSize bytes, void* pData) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->hCtx);
    GBL_API_VERIFY_ARG(bytes > 0);
    GBL_API_VERIFY_POINTER(pData);
    GBL_API_CALL(GblBuffer_clear(pSelf));
    pSelf->size = bytes;
    pSelf->pData = pData;
    GBL_API_END();
}

GBL_MAYBE_UNUSED GBL_INLINE
GBL_RESULT GblBuffer_release(SELF, GblSize* pSize, void** ppData) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->hCtx);
    GBL_API_VERIFY_POINTER(pSize);
    GBL_API_VERIFY_POINTER(ppData);
    *pSize  = pSelf->size;
    *ppData = pSelf->pData;
    pSelf->size = 0;
    pSelf->pData = NULL;
    GBL_API_END();
}

GBL_MAYBE_UNUSED GBL_INLINE
GBL_RESULT GblBuffer_grow(SELF, GblSize bytes) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->hCtx);
    GblBuffer_resize(pSelf, GblBuffer_size(pSelf) + bytes);
    GBL_API_END();
}

GBL_MAYBE_UNUSED GBL_INLINE
GBL_RESULT GblBuffer_shrink(SELF, GblSize bytes) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->hCtx);
    GBL_API_CALL(GblBuffer_resize(pSelf, GblBuffer_size(pSelf) - bytes));
    GBL_API_END();
}

GBL_MAYBE_UNUSED GBL_INLINE
GBL_RESULT GblBuffer_insert(SELF, GblSize offset, GblSize bytes, const void* pDataIn) GBL_NOEXCEPT {
    uintptr_t insertionPoint = 0;
    GBL_API_BEGIN(pSelf->hCtx);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_ARG(offset <= GblBuffer_size(pSelf));
    GBL_API_CALL(GblBuffer_resize(pSelf, GblBuffer_size(pSelf) + bytes));
    {
        GblInt      slideSize       = GblBuffer_size(pSelf) - offset;
                    insertionPoint  = (uintptr_t)GblBuffer_data(pSelf) + offset;
        memmove((uint8_t*)insertionPoint + bytes, (const uint8_t*)insertionPoint, slideSize);
        if(pDataIn)         memcpy((void*)insertionPoint, pDataIn, bytes);
    }
    GBL_API_END();
}

GBL_MAYBE_UNUSED GBL_INLINE
GBL_RESULT GblBuffer_append(SELF, GblSize bytes, const void* pData) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->hCtx);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_ARG(bytes > 0);
    GBL_API_VERIFY_POINTER(pData);
    {
        GblSize oldSize = GblBuffer_size(pSelf);
        GBL_API_CALL(GblBuffer_grow(pSelf, bytes));
        memcpy((uint8_t*)pSelf->pData + oldSize, pData, bytes - oldSize);
    }
    GBL_API_END();
}

GBL_MAYBE_UNUSED GBL_INLINE
GBL_RESULT GblBuffer_prepend(SELF, GblSize bytes, const void* pData) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->hCtx);
    GblBuffer_insert(pSelf, 0, bytes, pData);
    GBL_API_END();
}

GBL_MAYBE_UNUSED GBL_INLINE
GBL_RESULT GblBuffer_erase(SELF, GblSize offset, GblSize bytes) GBL_NOEXCEPT {
    GblSize lastPos = 0;
    GblSize remainderSize  = 0;
    GBL_API_BEGIN(pSelf->hCtx);
    GBL_API_VERIFY_EXPRESSION(GblBuffer_size(pSelf) > 0);
    GBL_API_VERIFY_ARG(offset < GblBuffer_size(pSelf));
    lastPos = offset + bytes - 1;
    GBL_API_VERIFY_ARG(lastPos < GblBuffer_size(pSelf));
    GBL_API_VERIFY_ARG(offset <= lastPos);
    GBL_API_VERIFY_EXPRESSION(bytes > 0 && bytes <= GblBuffer_size(pSelf));
    remainderSize = GblBuffer_size(pSelf) - 1 - lastPos;
    if(remainderSize) memmove((uint8_t*)GblBuffer_data(pSelf)+offset,
                              (const uint8_t*)GblBuffer_data(pSelf)+lastPos+1,
                              remainderSize);
    GBL_API_CALL(GblBuffer_shrink(pSelf, bytes));
    GBL_API_END();
}

GBL_MAYBE_UNUSED
GBL_INLINE GBL_RESULT GblBuffer_valueRead (CSELF, GblSize offset, GblSize bytes, void* pDataOut) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->hCtx);
    GBL_API_VERIFY(offset < pSelf->size, GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_API_VERIFY(offset + bytes <= pSelf->size, GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_API_VERIFY_POINTER(pDataOut);
    memcpy(pDataOut, (char*)pSelf->pData + offset, bytes);
    GBL_API_END();
}

GBL_MAYBE_UNUSED
GBL_INLINE GBL_RESULT GblBuffer_valueWrite(SELF, GblSize offset, GblSize bytes, const void* pDataIn) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->hCtx);
    GBL_API_VERIFY(offset < pSelf->size, GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_API_VERIFY(offset + bytes <= pSelf->size, GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_API_VERIFY_POINTER(pDataIn);
    memcpy((char*)pSelf->pData + offset, pDataIn, bytes);
    GBL_API_END();
}

GBL_MAYBE_UNUSED
GBL_INLINE GBL_RESULT GblBuffer_debug(CSELF, GblString* pStr) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->hCtx);
#if 0
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_POINTER(pStr);
    GBL_API_CALL(gblStringSnprintf(pStr,
                                  "GblBuffer [size: %u, data: %p]",
                                  GblBuffer_size(pSelf),
                                  GblBuffer_data(pSelf)));
#endif
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblBuffer_convert(CSELF, GblVariant* pVariant) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSelf->hCtx);
    //convert to: nil, bool, pointer, string
  #if 0
    switch(gblVariantType(pVariant)) {
    case GBL_STRING_TYPE: {
        GblString* pStr = NULL;
        GblStringView view { GblBuffer_data(pSelf), GblBuffer_size(pSelf) };
        GBL_API_CALL(gblVariantStringGet(pVariant, &pStr));
        GBL_API_VERIFY_EXPRESION(pstr);
        GBL_API_CALL(gblStringAssign(pStr, &view));
    }
    default:
        GBL_API_RECORD_SET(GBL_RESULT_ERROR_TYPE_MISMATCH);

    }
#endif
    GBL_API_END();


}

#undef CSELF
#undef SELF

GBL_DECLS_END


#endif // GIMBAL_BUFFER_H

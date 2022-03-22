#ifndef GIMBAL_BUFFER_H
#define GIMBAL_BUFFER_H

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
    GblSize             size;
    void*               pData;
} GblBuffer;


GBL_EXPORT GblType      GblBuffer_type      (void);

GBL_INLINE GBL_RESULT   GblBuffer_construct_1(SELF);
GBL_INLINE GBL_RESULT   GblBuffer_construct_2(SELF, GblSize size);
GBL_INLINE GBL_RESULT   GblBuffer_construct_3(SELF, GblSize size, const void* pData);
#define GblBuffer_Construct(...) \
        GBL_VA_OVERLOAD_SELECT(GblBuffer_construct, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)(__VA_ARGS__)

GBL_INLINE GBL_RESULT   GblBuffer_destruct  (SELF);
GBL_INLINE GBL_RESULT   GblBuffer_copy      (SELF, const GblBuffer* pOther);
GBL_INLINE GBL_RESULT   GblBuffer_move      (SELF, GblBuffer* pOther);
GBL_INLINE GBL_RESULT   GblBuffer_compare   (CSELF, const GblBuffer* pRhs, GblCmpResult* pResult);
GBL_INLINE GBL_RESULT   GblBuffer_convert   (CSELF, GblVariant* pVariant);
GBL_INLINE GBL_RESULT   GblBuffer_debug     (CSELF, GblString* pStr);

GBL_INLINE GblSize      GblBuffer_size      (CSELF);
GBL_INLINE void*        GblBuffer_data      (CSELF);
GBL_INLINE GblBool      GblBuffer_isEmpty   (CSELF);

GBL_INLINE GBL_RESULT   GblBuffer_reset     (SELF);
GBL_INLINE GBL_RESULT   GblBuffer_acquire   (SELF, GblSize size, const void* pData);
GBL_INLINE GBL_RESULT   GblBuffer_release   (SELF, GblSize* pSize, const void** ppData);
GBL_INLINE GBL_RESULT   GblBuffer_resize    (SELF, GblSize size);
GBL_INLINE GBL_RESULT   GblBuffer_grow      (SELF, GblSize size);
GBL_INLINE GBL_RESULT   GblBuffer_shrink    (SELF, GblSize size);
GBL_INLINE GBL_RESULT   GblBuffer_erase     (SELF, GblSize pos, GblSize size);
GBL_INLINE void*        GblBuffer_insert    (SELF, GblSize pos, GblSize size, const void* pDataIn);
GBL_INLINE GBL_RESULT   GblBuffer_append    (SELF, GblSize size, const void* pData);
GBL_INLINE GBL_RESULT   GblBuffer_prepend   (SELF, GblSize size, const void* pData);

// ===== INLINE IMPLEMENTATION =====

GBL_MAYBE_UNUSED GBL_INLINE
GblSize GblBuffer_size(CSELF) {
    return pSelf->size;
}

GBL_MAYBE_UNUSED GBL_INLINE
void* GblBuffer_data(CSELF) {
    return pSelf->pData;
}

GBL_MAYBE_UNUSED GBL_INLINE
GblBool GblBuffer_isEmpty(CSELF) {
    if(!pSelf)
        return GBL_TRUE;
    if(!pSelf->pData || ! pSelf->size)
        return GBL_TRUE;
    else
        return GBL_FALSE;
}

GBL_MAYBE_UNUSED GBL_INLINE
GBL_RESULT GblBuffer_construct_1(SELF) {
    GBL_API_BEGIN(NULL);
    GBL_API_CALL(GblBuffer_construct_2(pSelf, 0));
    GBL_API_END();
}

GBL_MAYBE_UNUSED GBL_INLINE
GBL_RESULT GblBuffer_construct_2(SELF, GblSize size) {
    GBL_API_BEGIN(NULL);
    GBL_API_CALL(GblBuffer_construct_3(pSelf, size, NULL));
    GBL_API_END();
}

GBL_MAYBE_UNUSED GBL_INLINE
GBL_RESULT GblBuffer_construct_3(SELF, GblSize size, const void* pData) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_EXPRESSION(!(size == 0 && pData != NULL),
                              "Cannot copy buffer of unknown size!");
    pSelf->size     = 0;
    pSelf->pData    = NULL;
    if(size) {
        GBL_API_CALL(GblBuffer_resize(pSelf, size));
        if(pData) {
            memcpy(GblBuffer_data(pSelf), pData, size);
        }
    }
    GBL_API_END();
}

GBL_MAYBE_UNUSED GBL_INLINE
GBL_RESULT GblBuffer_destruct(SELF) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_CALL(GblBuffer_reset(pSelf));
    GBL_API_END();
}

GBL_MAYBE_UNUSED GBL_INLINE
GBL_RESULT GblBuffer_compare(CSELF, const GblBuffer* pOther, GblCmpResult* pResult) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_POINTER(pOther);
    GBL_API_VERIFY_POINTER(pResult);
    if(GblBuffer_size(pSelf) > GblBuffer_size(pOther))
        *pResult = INT_MAX;
    else if(GblBuffer_size(pSelf) < GblBuffer_size(pOther))
        *pResult = INT_MIN;
    else
        *pResult =  memcmp(GblBuffer_data(pSelf),
                           GblBuffer_data(pOther),
                           GblSize(pSelf));
    GBL_API_END();
}

GBL_MAYBE_UNUSED GBL_INLINE
GBL_RESULT GblBuffer_copy(SELF, const GblBuffer* pOther) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_POINTER(pOther);
    GBL_API_CALL(GblBuffer_resize(pSelf, GblBuffer_size(pOther)));
    memcpy(GblBuffer_data(pSelf), GblBuffer_data(pOther), GblBuffer_size(pOther));
    GBL_API_END();
}

GBL_MAYBE_UNUSED GBL_INLINE
GBL_RESULT GblBuffer_move(SELF, GblBuffer* pOther) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_POINTER(pOther);
    pSelf->size     = pOther->size;
    pSelf->pData    = pOther->pData;
    GBL_API_CALL(GblBuffer_reset(pOther));
    GBL_API_END();
}

GBL_MAYBE_UNUSED GBL_INLINE
GBL_RESULT GblBuffer_reset(SELF) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_FREE(pSelf->pData);
    pSelf->size = 0;
    GBL_API_END();
}

GBL_MAYBE_UNUSED GBL_INLINE
GBL_RESULT GblBuffer_resize(SELF, GblSize size) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);
    if(!size) GBL_API_CALL(GblBuffer_reset(pSelf));
    else {
        if(!pSelf->pData) {
            pSelf->pData = GBL_API_MALLOC(size);
        } else {
            pSelf->pData = GBL_API_REALLOC(pSelf->pData, size);
        }

        if(size > GblBuffer_size(pSelf)) {
            memset((uint8_t*)pSelf->pData + GblBuffer_size(pSelf), 0, size - GblBuffer_size(pSelf));
        }

        pSelf->size = size;
    }
    GBL_API_END();
}

GBL_MAYBE_UNUSED GBL_INLINE
GBL_RESULT GblBuffer_acquire(SELF, GblSize size, const void* pData) {
    GBL_API_BEGIN(NULL);
    if(!size) {
        if(!pData) {
            GBL_API_CALL(GblBuffer_reset(pSelf));
        } else GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_ARG,
                                  "GblBuffer_acquire(self, 0, NON-NULL-DATA)");
    } else {
        if(!pData) GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_ARG,
                                      "GblBuffer_acquire(self, %u, NULL-DATA)", size);
        GBL_API_CALL(GblBuffer_resize(pSelf, size));
        memcpy(pSelf->pData, pData, size);
    }
    GBL_API_END();
}

GBL_MAYBE_UNUSED GBL_INLINE
GBL_RESULT GblBuffer_grow(SELF, GblSize size) {
    GBL_API_BEGIN(NULL);
    GblBuffer_resize(pSelf, GblBuffer_size(pSelf) + size);
    GBL_API_END();
}

GBL_MAYBE_UNUSED GBL_INLINE
GBL_RESULT GblBuffer_shrink(SELF, GblSize size) {
    GBL_API_BEGIN(NULL);
    GBL_API_CALL(GblBuffer_resize(pSelf, GblBuffer_size(pSelf) - size));
    GBL_API_END();
}

GBL_MAYBE_UNUSED GBL_INLINE
void* GblBuffer_insert(SELF, GblSize pos, GblSize size, const void* pDataIn) {
    uintptr_t insertionPoint = 0;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_ARG(pos <= GblBuffer_size(pSelf));
    GBL_API_CALL(GblBuffer_resize(pSelf, GblBuffer_size(pSelf) + size));
    {
        GblInt      slideSize       = GblBuffer_size(pSelf) - pos;
                    insertionPoint  = (uintptr_t)GblBuffer_data(pSelf) + pos;
        memmove((uint8_t*)insertionPoint + size, (const uint8_t*)insertionPoint, slideSize);
        if(pDataIn)         memcpy((void*)insertionPoint, pDataIn, size);
    }
    GBL_API_END_BLOCK();
    return (void*)insertionPoint;
}

GBL_MAYBE_UNUSED GBL_INLINE
GBL_RESULT GblBuffer_append(SELF, GblSize size, const void* pData) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_ARG(size > 0);
    GBL_API_VERIFY_POINTER(pData);
    {
        GblSize oldSize = GblBuffer_size(pSelf);
        GBL_API_CALL(GblBuffer_grow(pSelf, size));
        memcpy((uint8_t*)pSelf->pData + oldSize, pData, size - oldSize);
    }
    GBL_API_END();
}

GBL_MAYBE_UNUSED GBL_INLINE
GBL_RESULT GblBuffer_prepend(SELF, GblSize size, const void* pData) {
    GBL_API_BEGIN(NULL);
    GblBuffer_insert(pSelf, 0, size, pData);
    GBL_API_END();
}

GBL_MAYBE_UNUSED GBL_INLINE
GBL_RESULT GblBuffer_erase(SELF, GblSize pos, GblSize size) {
    GblSize lastPos = 0;
    GblSize remainderSize  = 0;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_EXPRESSION(GblBuffer_size(pSelf) > 0);
    GBL_API_VERIFY_ARG(pos < GblBuffer_size(pSelf));
    lastPos = pos + size - 1;
    GBL_API_VERIFY_ARG(lastPos < GblBuffer_size(pSelf));
    GBL_API_VERIFY_ARG(pos <= lastPos);
    GBL_API_VERIFY_EXPRESSION(size > 0 && size <= GblBuffer_size(pSelf));
    remainderSize = GblBuffer_size(pSelf) - 1 - lastPos;
    if(remainderSize) memmove((uint8_t*)GblBuffer_data(pSelf)+pos,
                              (const uint8_t*)GblBuffer_data(pSelf)+lastPos+1,
                              remainderSize);
    GBL_API_CALL(GblBuffer_shrink(pSelf, size));
    GBL_API_END();
}

GBL_MAYBE_UNUSED
GBL_INLINE GBL_RESULT GblBuffer_debug(CSELF, GblString* pStr) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_POINTER(pStr);
    GBL_API_CALL(gblStringSprintf(pStr,
                                  "GblBuffer [size: %u, data: %p]",
                                  GblBuffer_size(pSelf),
                                  GblBuffer_data(pSelf)));
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblBuffer_convert(CSELF, GblVariant* pVariant) {
    GBL_API_BEGIN(NULL);
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

    GBL_API_END();
#endif

}

#undef CSELF
#undef SELF

GBL_DECLS_END


#endif // GIMBAL_BUFFER_H

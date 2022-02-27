#ifndef GIMBAL_BUFFER_H
#define GIMBAL_BUFFER_H

#include "gimbal_typedefs.h"
#include "../meta/gimbal_type.h"

#define GBL_IBUFFER_TYPE  (GblIBuffer_type())
#define GBL_BUFFER_TYPE   (GblBuffer_type())

#ifdef __cplusplus
extern "C" {
#endif



struct GblBuffer;

struct GblBuffer_vtable {
    struct GblIBuffer_vtable   IBuffer;
};

typedef struct GblBuffer {
    union {
        const struct GblBuffer_vtable*  pVptr;
        GblIBuffer                      IBuffer;
    };
    GblSize                             bytes;
    void*                               pData;
} GblBuffer;

GBL_EXPORT GblType GblBuffer_type(void);




GBL_API GblBuffer_init(GblBuffer* pSelf, const struct GblBuffer_vtable* pClass) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_POINTER(pClass);
    memset(pSelf, 0, sizeof(GblBuffer));
    pSelf->pVptr = pClass;
    GBL_API_END_BLOCK();
}

GBL_API GblBuffer_reset(GblBuffer* pSelf) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_FREE(pSelf->pData);
    pSelf->bytes = 0;
    GBL_API_END_BLOCK();
}

GBL_API GblBuffer_resize(GblBuffer* pSelf, GblSize size) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);
    if(!size) GBL_API_CALL(GblBuffer_reset(pSelf));
    else {
        if(!pSelf->pData) {
            pSelf->pData = GBL_API_MALLOC(size);
        } else {
            pSelf->pData = GBL_API_REALLOC(pSelf->pData, size);
        }

        if(size > pSelf->bytes) {
            memset((uint8_t*)pSelf->pData + pSelf->bytes, 0, size - pSelf->bytes);
        }

        pSelf->bytes = size;
    }
    GBL_API_END_BLOCK();
}

GBL_API GblBuffer_set(GblBuffer* pSelf, GblSize size, const void* pData) {
    GBL_API_BEGIN(NULL);
    if(!size) {
        if(!pData) {
            GBL_API_CALL(GblBuffer_reset(pSelf));
        } else GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_ARG, "GblBuffer_set(self, 0, NON-NULL-DATA)");
    } else {
        if(!pData) GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_ARG, "GblBuffer_set(self, %u, NULL-DATA)", size);
        GBL_API_CALL(GblBuffer_resize(pSelf, size));
        memcpy(pSelf->pData, pData, size);
    }
    GBL_API_END_BLOCK();
}

GBL_API GblBuffer_grow(GblBuffer* pSelf, GblSize size) {
    GblBuffer_resize(pSelf, pSelf->bytes + size);
}

GBL_API GblBuffer_shrink(GblBuffer* pSelf, GblSize size) {
    GblBuffer_resize(pSelf, pSelf->bytes - size);
}

GBL_API GblBuffer_append(GblBuffer* pSelf, GblSize size, const void* pData) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_ARG(size > 0);
    GBL_API_VERIFY_POINTER(pData);
    {
        GblSize oldSize = pSelf->bytes;
        GBL_API_CALL(GblBuffer_grow(pSelf, size));
        memcpy((uint8_t*)pSelf->pData + oldSize, pData, size - oldSize);
    }
    GBL_API_END();
}

#if 0
GBL_API GblBuffer_bytesRead(const GblBuffer* pSelf, GblSize pos, GblSize bytes, void* pBuffer) {
    uint8_t byte = 0;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY(index < pSelf->bytes, GBL_RESULT_ERROR_OUT_OF_RANGE);
    byte = *((uint8_t*)pSelf->pData + index);
    GBL_API_END();
    return byte;
}

GBL_API GblBuffer_bytesWrite(const GblBuffer* pSelf, GblSize pos, GblSize bytes, void* pBuffer) {
    uint8_t byte = 0;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY(index < pSelf->bytes, GBL_RESULT_ERROR_OUT_OF_RANGE);
    *((uint8_t*)pSelf->pData + index) = value;
    GBL_API_END();
    return byte;
}
#endif




#ifdef __cplusplus
}
#endif


#endif // GIMBAL_BUFFER_H

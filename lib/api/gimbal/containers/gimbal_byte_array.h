/*! \file
 *  \brief GblByteArray structure and related functions
 *  \ingroup containers
 */

#ifndef GIMBAL_BYTE_ARRAY_H
#define GIMBAL_BYTE_ARRAY_H

#include "../core/gimbal_ref.h"
#include "../strings/gimbal_string_view.h"

#define GBL_SELF    GblByteArray*          pSelf
#define GBL_CSELF   const GBL_SELF

GBL_DECLS_BEGIN

/*! \brief Reference-counted resizable array of bytes
 *  \ingroup containers
 */
typedef struct GblByteArray {
    GblSize             size;
    uint8_t*            pData;
} GblByteArray;

GBL_INLINE GblByteArray*   GblByteArray_create_0   (void)                                                           GBL_NOEXCEPT;
GBL_INLINE GblByteArray*   GblByteArray_create_1   (GblSize bytes)                                                  GBL_NOEXCEPT;
GBL_INLINE GblByteArray*   GblByteArray_create_2   (GblSize bytes, const void* pData)                               GBL_NOEXCEPT;
GBL_EXPORT GblByteArray*   GblByteArray_create_3   (GblSize bytes, const void* pData, GblContext* pCtx)             GBL_NOEXCEPT;
#define GblByteArray_create(...) \
        GBL_VA_OVERLOAD_CALL(GblByteArray_create, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)

GBL_INLINE GblByteArray*    GblByteArray_ref       (GBL_SELF)                                                       GBL_NOEXCEPT;
GBL_EXPORT GblRefCount      GblByteArray_unref     (GBL_SELF)                                                       GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT       GblByteArray_copy      (GBL_SELF, const GblByteArray* pOther)                           GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblByteArray_move      (GBL_SELF, GblByteArray* pOther)                                 GBL_NOEXCEPT;
GBL_EXPORT GblInt           GblByteArray_compare   (GBL_CSELF, const GblByteArray* pRhs)                            GBL_NOEXCEPT;
GBL_INLINE GblBool          GblByteArray_equals    (GBL_CSELF, const GblByteArray* pRhs)                            GBL_NOEXCEPT;

GBL_INLINE GblRefCount      GblByteArray_refCount  (GBL_CSELF)                                                      GBL_NOEXCEPT;
GBL_INLINE GblContext*      GblByteArray_context   (GBL_CSELF)                                                      GBL_NOEXCEPT;
GBL_INLINE GblSize          GblByteArray_size      (GBL_CSELF)                                                      GBL_NOEXCEPT;
GBL_INLINE GblBool          GblByteArray_empty     (GBL_CSELF)                                                      GBL_NOEXCEPT;

GBL_INLINE uint8_t          GblByteArray_at        (GBL_CSELF, GblSize index)                                       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblByteArray_set       (GBL_SELF, GblSize bytes, const void* pData)                     GBL_NOEXCEPT;

GBL_INLINE void*            GblByteArray_data      (GBL_CSELF)                                                      GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblByteArray_read      (GBL_CSELF, GblSize offset, GblSize bytes, void* pDataOut)       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblByteArray_write     (GBL_SELF, GblSize offset, GblSize bytes, const void* pDataIn)   GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT       GblByteArray_clear     (GBL_SELF)                                                       GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblByteArray_acquire   (GBL_SELF, GblSize bytes, void* pData)                           GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblByteArray_release   (GBL_SELF, GblSize* pSize, void** ppData)                        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblByteArray_resize    (GBL_SELF, GblSize bytes)                                        GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblByteArray_grow      (GBL_SELF, GblSize bytes)                                        GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblByteArray_shrink    (GBL_SELF, GblSize bytes)                                        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblByteArray_erase     (GBL_SELF, GblSize offset, GblSize bytes)                        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT       GblByteArray_insert    (GBL_SELF, GblSize offset, GblSize bytes, const void* pDataIn)   GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblByteArray_append    (GBL_SELF, GblSize bytes, const void* pData)                     GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblByteArray_prepend   (GBL_SELF, GblSize bytes, const void* pData)                     GBL_NOEXCEPT;

GBL_EXPORT GblHash          GblByteArray_hash       (GBL_CSELF)                                                     GBL_NOEXCEPT;
GBL_INLINE const char*      GblByteArray_cString    (GBL_CSELF)                                                     GBL_NOEXCEPT;
GBL_INLINE GblStringView    GblByteArray_stringView (GBL_CSELF)                                                     GBL_NOEXCEPT;

// =========== IMPL ===========

GBL_INLINE GblBool GblByteArray_empty(GBL_CSELF) GBL_NOEXCEPT {
    if(!pSelf)
        return GBL_TRUE;
    if(!pSelf->pData || ! pSelf->size)
        return GBL_TRUE;
    else
        return GBL_FALSE;
}

GBL_INLINE GblByteArray* GblByteArray_create_0(void) GBL_NOEXCEPT {
    return GblByteArray_create_1(0);
}

GBL_INLINE GblByteArray* GblByteArray_create_1(GblSize bytes) GBL_NOEXCEPT {
    return GblByteArray_create_2(bytes, NULL);
}

GBL_INLINE GblByteArray* GblByteArray_create_2(GblSize bytes, const void* pData) GBL_NOEXCEPT {
    return GblByteArray_create_3(bytes, pData, NULL);
}

GBL_INLINE GblByteArray* GblByteArray_ref(GBL_SELF) GBL_NOEXCEPT {
    return (GblByteArray*)GblRef_acquire(pSelf);
}

GBL_INLINE GblRefCount GblByteArray_refCount(GBL_CSELF) GBL_NOEXCEPT {
    return GblRef_refCount(pSelf);
}

GBL_INLINE GblContext* GblByteArray_context(GBL_CSELF) GBL_NOEXCEPT {
    return GblRef_context(pSelf);
}

GBL_INLINE GblSize GblByteArray_size(GBL_CSELF) GBL_NOEXCEPT {
    return pSelf->size;
}

GBL_INLINE void* GblByteArray_data(GBL_CSELF) GBL_NOEXCEPT {
    return pSelf->pData;
}

GBL_INLINE GBL_RESULT GblByteArray_set(GBL_SELF, GblSize bytes, const void* pData) GBL_NOEXCEPT {
    GBL_API_BEGIN(GblByteArray_context(pSelf));
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_CALL(GblByteArray_resize(pSelf, bytes));
    if(bytes && pData) memcpy(GblByteArray_data(pSelf), pData, bytes);
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblByteArray_copy(GBL_SELF, const GblByteArray* pOther) GBL_NOEXCEPT {
    GBL_API_BEGIN(GblByteArray_context(pSelf));
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_POINTER(pOther);
    GBL_API_CALL(GblByteArray_set(pSelf, GblByteArray_size(pOther), GblByteArray_data(pOther)));
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblByteArray_move(GBL_SELF, GblByteArray* pOther) GBL_NOEXCEPT {
    GBL_API_BEGIN(GblByteArray_context(pSelf));
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_POINTER(pOther);

    GBL_API_CALL(GblByteArray_clear(pSelf));
    pSelf->size     = pOther->size;
    pSelf->pData    = pOther->pData;
    pOther->size    = 0;
    pOther->pData   = NULL;
    GBL_API_END();
}

GBL_INLINE GblBool GblByteArray_equals(GBL_CSELF, const GblByteArray* pRhs) GBL_NOEXCEPT {
    return GblByteArray_compare(pSelf, pRhs) == 0;
}

GBL_INLINE GBL_RESULT GblByteArray_clear(GBL_SELF) GBL_NOEXCEPT {
    GBL_API_BEGIN(GblByteArray_context(pSelf));
    GBL_API_VERIFY_POINTER(pSelf);
    if(pSelf->pData) GBL_API_FREE(pSelf->pData);
    pSelf->pData = NULL;
    pSelf->size = 0;
    GBL_API_END();
}


GBL_INLINE GBL_RESULT GblByteArray_acquire(GBL_SELF, GblSize bytes, void* pData) GBL_NOEXCEPT {
    GBL_API_BEGIN(GblByteArray_context(pSelf));
    GBL_API_VERIFY_ARG(bytes > 0);
    GBL_API_VERIFY_POINTER(pData);
    GBL_API_CALL(GblByteArray_clear(pSelf));
    pSelf->size = bytes;
    pSelf->pData = (uint8_t*)pData;
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblByteArray_release(GBL_SELF, GblSize* pSize, void** ppData) GBL_NOEXCEPT {
    GBL_API_BEGIN(GblByteArray_context(pSelf));
    GBL_API_VERIFY_POINTER(pSize);
    GBL_API_VERIFY_POINTER(ppData);
    *pSize  = pSelf->size;
    *ppData = pSelf->pData;
    pSelf->size = 0;
    pSelf->pData = NULL;
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblByteArray_grow(GBL_SELF, GblSize bytes) GBL_NOEXCEPT {
    GBL_API_BEGIN(GblByteArray_context(pSelf));
    GblByteArray_resize(pSelf, GblByteArray_size(pSelf) + bytes);
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblByteArray_shrink(GBL_SELF, GblSize bytes) GBL_NOEXCEPT {
    GBL_API_BEGIN(GblByteArray_context(pSelf));
    GBL_API_CALL(GblByteArray_resize(pSelf, GblByteArray_size(pSelf) - bytes));
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblByteArray_append(GBL_SELF, GblSize bytes, const void* pData) GBL_NOEXCEPT {
    GBL_API_BEGIN(GblByteArray_context(pSelf));
    GBL_API_VERIFY_POINTER(pSelf);
    GBL_API_VERIFY_ARG(bytes > 0);
    GBL_API_VERIFY_POINTER(pData);
    {
        GblSize oldSize = GblByteArray_size(pSelf);
        GBL_API_CALL(GblByteArray_grow(pSelf, bytes));
        memcpy((uint8_t*)pSelf->pData + oldSize, pData, bytes);
    }
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblByteArray_prepend(GBL_SELF, GblSize bytes, const void* pData) GBL_NOEXCEPT {
    GBL_API_BEGIN(GblByteArray_context(pSelf));
    GblByteArray_insert(pSelf, 0, bytes, pData);
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblByteArray_read(GBL_CSELF, GblSize offset, GblSize bytes, void* pDataOut) GBL_NOEXCEPT {
    GBL_API_BEGIN(GblByteArray_context(pSelf));
    GBL_API_VERIFY(offset < pSelf->size, GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_API_VERIFY(offset + bytes <= pSelf->size, GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_API_VERIFY_POINTER(pDataOut);
    memcpy(pDataOut, (char*)pSelf->pData + offset, bytes);
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblByteArray_write(GBL_SELF, GblSize offset, GblSize bytes, const void* pDataIn) GBL_NOEXCEPT {
    GBL_API_BEGIN(GblByteArray_context(pSelf));
    GBL_API_VERIFY(offset < pSelf->size, GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_API_VERIFY(offset + bytes <= pSelf->size, GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_API_VERIFY_POINTER(pDataIn);
    memcpy((char*)pSelf->pData + offset, pDataIn, bytes);
    GBL_API_END();
}

GBL_INLINE GblStringView GblByteArray_stringView(GBL_CSELF) GBL_NOEXCEPT {
    return GblStringView_fromStringSized((const char*)pSelf->pData, pSelf->size);
}

GBL_INLINE const char* GblByteArray_cString(GBL_CSELF) GBL_NOEXCEPT {
    return pSelf->pData[pSelf->size-1] == '\0'? (const char*)pSelf->pData : GBL_NULL;
}
GBL_INLINE uint8_t GblByteArray_at(GBL_CSELF, GblSize index) GBL_NOEXCEPT {
    uint8_t byte = 0;
    GBL_API_BEGIN(GblByteArray_context(pSelf));
    GBL_API_VERIFY(index < pSelf->size,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    byte = pSelf->pData[index];
    GBL_API_END_BLOCK();
    return byte;
}


#undef GBL_CSELF
#undef GBL_SELF

GBL_DECLS_END


#endif // GIMBAL_BUFFER_H

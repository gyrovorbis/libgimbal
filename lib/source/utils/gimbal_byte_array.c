#include <gimbal/utils/gimbal_byte_array.h>
#include <gimbal/algorithms/gimbal_hash.h>
#include <gimbal/utils/gimbal_ref.h>

GBL_INLINE GBL_RESULT GblByteArray_destruct(void* pRef) {
    GblByteArray* pSelf = (GblByteArray*)pRef;
    GBL_CTX_BEGIN(GblByteArray_context(pSelf));
    GBL_CTX_VERIFY_CALL(GblByteArray_clear(pSelf));
    GBL_CTX_END();
}

GBL_EXPORT GblRefCount GblByteArray_unref(GblByteArray* pSelf) {
    return GblRef_releaseWithDtor(pSelf, GblByteArray_destruct);
}

GBL_EXPORT GblByteArray* (GblByteArray_create)(size_t bytes, const void* pData, GblContext* pCtx) GBL_NOEXCEPT {
    GblByteArray* pSelf = NULL;
    GBL_CTX_BEGIN(pCtx);
    GBL_CTX_VERIFY_ARG(!(bytes == 0 && pData != NULL),
                        "Cannot copy buffer of unknown size!");

    pSelf = (GblByteArray*)GblRef_allocWithContext(sizeof(GblByteArray), pCtx);

    pSelf->size     = 0;
    pSelf->pData    = NULL;
    if(bytes) {
        GBL_CTX_VERIFY_CALL(GblByteArray_resize(pSelf, bytes));
        if(pData) {
            memcpy(GblByteArray_data(pSelf), pData, bytes);
        }
    }
    GBL_CTX_END_BLOCK();
    return pSelf;
}

GBL_EXPORT GBL_RESULT GblByteArray_resize(GblByteArray* pSelf, size_t  bytes) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(GblByteArray_context(pSelf));
    GBL_CTX_VERIFY_POINTER(pSelf);
    if(!bytes) GBL_CTX_CALL(GblByteArray_clear(pSelf));
    else {
        if(!pSelf->pData) {
            pSelf->pData = (uint8_t*)GBL_CTX_MALLOC(bytes);
        } else {
            pSelf->pData = (uint8_t*)GBL_CTX_REALLOC(pSelf->pData, bytes);
        }

        if(bytes > GblByteArray_size(pSelf)) {
            memset((uint8_t*)pSelf->pData + GblByteArray_size(pSelf), 0, bytes - GblByteArray_size(pSelf));
        }

        pSelf->size = bytes;
    }
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblByteArray_insert(GblByteArray* pSelf, size_t  offset, size_t  bytes, const void* pDataIn) GBL_NOEXCEPT {
    uintptr_t insertionPoint = 0;
    GBL_CTX_BEGIN(GblByteArray_context(pSelf));
    GBL_CTX_VERIFY_POINTER(pSelf);
    GBL_CTX_VERIFY_ARG(offset <= GblByteArray_size(pSelf));
    size_t  oldSize = GblByteArray_size(pSelf);
    GBL_CTX_CALL(GblByteArray_resize(pSelf, GblByteArray_size(pSelf) + bytes));
    {
        int      slideSize       = oldSize - offset;
                    insertionPoint  = (uintptr_t)GblByteArray_data(pSelf) + offset;
        memmove((uint8_t*)insertionPoint + bytes, (const uint8_t*)insertionPoint, slideSize);
        if(pDataIn)         memcpy((void*)insertionPoint, pDataIn, bytes);
    }
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblByteArray_erase(GblByteArray* pSelf, size_t  offset, size_t  bytes) GBL_NOEXCEPT {
    size_t  lastPos = 0;
    size_t  remainderSize  = 0;
    GBL_CTX_BEGIN(GblByteArray_context(pSelf));
    GBL_CTX_VERIFY_EXPRESSION(GblByteArray_size(pSelf) > 0);
    GBL_CTX_VERIFY_ARG(offset < GblByteArray_size(pSelf));
    lastPos = offset + bytes - 1;
    GBL_CTX_VERIFY_ARG(lastPos < GblByteArray_size(pSelf));
    GBL_CTX_VERIFY_ARG(offset <= lastPos);
    GBL_CTX_VERIFY_EXPRESSION(bytes > 0 && bytes <= GblByteArray_size(pSelf));
    remainderSize = GblByteArray_size(pSelf) - 1 - lastPos;
    if(remainderSize) memmove((uint8_t*)GblByteArray_data(pSelf)+offset,
                              (const uint8_t*)GblByteArray_data(pSelf)+lastPos+1,
                              remainderSize);
    GBL_CTX_CALL(GblByteArray_shrink(pSelf, bytes));
    GBL_CTX_END();
}

GBL_EXPORT int GblByteArray_compare(const GblByteArray* pSelf, const GblByteArray* pOther) GBL_NOEXCEPT {
    int result = INT_MAX;
    int* pResult = &result;
    GBL_CTX_BEGIN(GblByteArray_context(pSelf));
    GBL_CTX_VERIFY_POINTER(pSelf);
    GBL_CTX_VERIFY_POINTER(pOther);
    GBL_CTX_VERIFY_POINTER(pResult);
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
    GBL_CTX_END_BLOCK();
    return result;
}

GBL_EXPORT GblHash GblByteArray_hash(const GblByteArray* pSelf) {
    return gblHash(pSelf->pData, pSelf->size);
}


GBL_EXPORT GblBool GblByteArray_empty(const GblByteArray* pSelf) {
    if(!pSelf)
        return GBL_TRUE;
    if(!pSelf->pData || ! pSelf->size)
        return GBL_TRUE;
    else
        return GBL_FALSE;
}

GBL_EXPORT GblByteArray* GblByteArray_ref(GblByteArray* pSelf) {
    return (GblByteArray*)GblRef_acquire(pSelf);
}

GBL_EXPORT GblRefCount GblByteArray_refCount(const GblByteArray* pSelf) {
    return GblRef_refCount(pSelf);
}

GBL_EXPORT GblContext* GblByteArray_context(const GblByteArray* pSelf) {
    return GblRef_context(pSelf);
}

GBL_EXPORT size_t  GblByteArray_size(const GblByteArray* pSelf) {
    return pSelf->size;
}

GBL_EXPORT void* GblByteArray_data(const GblByteArray* pSelf) {
    return pSelf->pData;
}

GBL_EXPORT GBL_RESULT GblByteArray_set(GblByteArray* pSelf, size_t bytes, const void* pData) {
    GBL_CTX_BEGIN(GblByteArray_context(pSelf));
    GBL_CTX_VERIFY_POINTER(pSelf);
    GBL_CTX_CALL(GblByteArray_resize(pSelf, bytes));
    if(bytes && pData) memcpy(GblByteArray_data(pSelf), pData, bytes);
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblByteArray_copy(GblByteArray* pSelf, const GblByteArray* pOther) {
    GBL_CTX_BEGIN(GblByteArray_context(pSelf));
    GBL_CTX_VERIFY_POINTER(pSelf);
    GBL_CTX_VERIFY_POINTER(pOther);
    GBL_CTX_CALL(GblByteArray_set(pSelf, GblByteArray_size(pOther), GblByteArray_data(pOther)));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblByteArray_move(GblByteArray* pSelf, GblByteArray* pOther) {
    GBL_CTX_BEGIN(GblByteArray_context(pSelf));
    GBL_CTX_VERIFY_POINTER(pSelf);
    GBL_CTX_VERIFY_POINTER(pOther);

    GBL_CTX_CALL(GblByteArray_clear(pSelf));
    pSelf->size     = pOther->size;
    pSelf->pData    = pOther->pData;
    pOther->size    = 0;
    pOther->pData   = NULL;
    GBL_CTX_END();
}

GBL_EXPORT GblBool GblByteArray_equals(const GblByteArray* pSelf, const GblByteArray* pRhs) {
    return GblByteArray_compare(pSelf, pRhs) == 0;
}

GBL_EXPORT GBL_RESULT GblByteArray_clear(GblByteArray* pSelf) {
    GBL_CTX_BEGIN(GblByteArray_context(pSelf));
    GBL_CTX_VERIFY_POINTER(pSelf);
    if(pSelf->pData) GBL_CTX_FREE(pSelf->pData);
    pSelf->pData = NULL;
    pSelf->size = 0;
    GBL_CTX_END();
}


GBL_EXPORT GBL_RESULT GblByteArray_acquire(GblByteArray* pSelf, size_t  bytes, void* pData) {
    GBL_CTX_BEGIN(GblByteArray_context(pSelf));
    GBL_CTX_VERIFY_ARG(bytes > 0);
    GBL_CTX_VERIFY_POINTER(pData);
    GBL_CTX_CALL(GblByteArray_clear(pSelf));
    pSelf->size = bytes;
    pSelf->pData = (uint8_t*)pData;
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblByteArray_release(GblByteArray* pSelf, size_t * pSize, void** ppData) {
    GBL_CTX_BEGIN(GblByteArray_context(pSelf));
    GBL_CTX_VERIFY_POINTER(pSize);
    GBL_CTX_VERIFY_POINTER(ppData);
    *pSize  = pSelf->size;
    *ppData = pSelf->pData;
    pSelf->size = 0;
    pSelf->pData = NULL;
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblByteArray_grow(GblByteArray* pSelf, size_t  bytes) {
    GBL_CTX_BEGIN(GblByteArray_context(pSelf));
    GblByteArray_resize(pSelf, GblByteArray_size(pSelf) + bytes);
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblByteArray_shrink(GblByteArray* pSelf, size_t  bytes) {
    GBL_CTX_BEGIN(GblByteArray_context(pSelf));
    GBL_CTX_CALL(GblByteArray_resize(pSelf, GblByteArray_size(pSelf) - bytes));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblByteArray_append(GblByteArray* pSelf, size_t  bytes, const void* pData) {
    GBL_CTX_BEGIN(GblByteArray_context(pSelf));
    GBL_CTX_VERIFY_POINTER(pSelf);
    GBL_CTX_VERIFY_ARG(bytes > 0);
    GBL_CTX_VERIFY_POINTER(pData);
    {
        size_t  oldSize = GblByteArray_size(pSelf);
        GBL_CTX_CALL(GblByteArray_grow(pSelf, bytes));
        memcpy((uint8_t*)pSelf->pData + oldSize, pData, bytes);
    }
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblByteArray_prepend(GblByteArray* pSelf, size_t  bytes, const void* pData) {
    GBL_CTX_BEGIN(GblByteArray_context(pSelf));
    GblByteArray_insert(pSelf, 0, bytes, pData);
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblByteArray_read(const GblByteArray* pSelf, size_t  offset, size_t  bytes, void* pDataOut) {
    GBL_CTX_BEGIN(GblByteArray_context(pSelf));
    GBL_CTX_VERIFY(offset < pSelf->size, GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_VERIFY(offset + bytes <= pSelf->size, GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_VERIFY_POINTER(pDataOut);
    memcpy(pDataOut, (char*)pSelf->pData + offset, bytes);
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblByteArray_write(GblByteArray* pSelf, size_t  offset, size_t  bytes, const void* pDataIn) {
    GBL_CTX_BEGIN(GblByteArray_context(pSelf));
    GBL_CTX_VERIFY(offset < pSelf->size, GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_VERIFY(offset + bytes <= pSelf->size, GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_VERIFY_POINTER(pDataIn);
    memcpy((char*)pSelf->pData + offset, pDataIn, bytes);
    GBL_CTX_END();
}

GBL_EXPORT GblStringView GblByteArray_stringView(const GblByteArray* pSelf) {
    return GblStringView_fromStringSized((const char*)pSelf->pData, pSelf->size);
}

GBL_EXPORT const char* GblByteArray_cString(const GblByteArray* pSelf) {
    return pSelf->pData[pSelf->size-1] == '\0'? (const char*)pSelf->pData : GBL_NULL;
}

GBL_EXPORT uint8_t GblByteArray_at(const GblByteArray* pSelf, size_t  index) {
    uint8_t byte = 0;
    GBL_CTX_BEGIN(GblByteArray_context(pSelf));
    GBL_CTX_VERIFY(index < pSelf->size,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    byte = pSelf->pData[index];
    GBL_CTX_END_BLOCK();
    return byte;
}

#include <gimbal/containers/gimbal_byte_array.h>
#include <gimbal/algorithms/gimbal_hash.h>

GBL_INLINE GBL_RESULT GblByteArray_destruct(void* pRef) {
    GblByteArray* pSelf = (GblByteArray*)pRef;
    GBL_CTX_BEGIN(GblByteArray_context(pSelf));
    GBL_CTX_VERIFY_CALL(GblByteArray_clear(pSelf));
    GBL_CTX_END();
}

GBL_EXPORT GblRefCount GblByteArray_unref(GblByteArray* pSelf) {
    return GblRef_releaseWithDtor(pSelf, GblByteArray_destruct);
}

GBL_EXPORT GblByteArray* GblByteArray_create_3(GblSize bytes, const void* pData, GblContext* pCtx) GBL_NOEXCEPT {
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

GBL_EXPORT GBL_RESULT GblByteArray_resize(GblByteArray* pSelf, GblSize bytes) GBL_NOEXCEPT {
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

GBL_EXPORT GBL_RESULT GblByteArray_insert(GblByteArray* pSelf, GblSize offset, GblSize bytes, const void* pDataIn) GBL_NOEXCEPT {
    uintptr_t insertionPoint = 0;
    GBL_CTX_BEGIN(GblByteArray_context(pSelf));
    GBL_CTX_VERIFY_POINTER(pSelf);
    GBL_CTX_VERIFY_ARG(offset <= GblByteArray_size(pSelf));
    GblSize oldSize = GblByteArray_size(pSelf);
    GBL_CTX_CALL(GblByteArray_resize(pSelf, GblByteArray_size(pSelf) + bytes));
    {
        int      slideSize       = oldSize - offset;
                    insertionPoint  = (uintptr_t)GblByteArray_data(pSelf) + offset;
        memmove((uint8_t*)insertionPoint + bytes, (const uint8_t*)insertionPoint, slideSize);
        if(pDataIn)         memcpy((void*)insertionPoint, pDataIn, bytes);
    }
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblByteArray_erase(GblByteArray* pSelf, GblSize offset, GblSize bytes) GBL_NOEXCEPT {
    GblSize lastPos = 0;
    GblSize remainderSize  = 0;
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
    return gblHashFnv1(pSelf->pData, pSelf->size);
}

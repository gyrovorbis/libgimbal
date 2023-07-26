#include <gimbal/strings/gimbal_string_ref.h>
#include <gimbal/algorithms/gimbal_hash.h>

typedef struct GblStringRef_ {
    size_t      length;
    char        data[1];
} GblStringRef_;

GBL_INLINE GblStringRef_* GblStringRef_header_(const GblStringRef* pString) GBL_NOEXCEPT {
    GblStringRef_* pHeader = (GblStringRef_*)(pString - offsetof(GblStringRef_, data));
    return pHeader;
}

GBL_EXPORT GblStringRef* GblStringRef_create(const char* pString) GBL_NOEXCEPT {
    return GblStringRef_createWithContext(pString, NULL);
}

GBL_EXPORT GblStringRef* GblStringRef_createWithContext(const char* pString, GblContext* pCtx) GBL_NOEXCEPT {
    return GblStringRef_createFromViewWithContext(GBL_STRING_VIEW(pString), pCtx);
}

GBL_EXPORT GblStringRef* GblStringRef_createFromView(GblStringView view) GBL_NOEXCEPT {
    return GblStringRef_createFromViewWithContext(view, NULL);
}

GBL_EXPORT GblContext* GblStringRef_context(const GblStringRef* pSelf) GBL_NOEXCEPT {
    GblContext* pCtx = NULL;
    if(pSelf) {
        GblStringRef_* pStrHeader = GblStringRef_header_(pSelf);
        pCtx = GblRef_context(pStrHeader);
    }
    return pCtx;
}

GBL_EXPORT GblRefCount GblStringRef_refCount(const GblStringRef* pSelf) GBL_NOEXCEPT {
    GblRefCount refCount = 0;
    if(pSelf) {
        GblStringRef_* pStrHeader = GblStringRef_header_(pSelf);
        refCount = GblRef_refCount(pStrHeader);
    }
    return refCount;
}

GBL_EXPORT size_t  GblStringRef_length(const GblStringRef* pSelf) GBL_NOEXCEPT {
    size_t  length = 0;
    if(pSelf) {
        GblStringRef_* pStrHeader = GblStringRef_header_(pSelf);
        length = pStrHeader->length;
    }
    return length;
}

GBL_EXPORT GblBool GblStringRef_empty(const GblStringRef* pSelf) GBL_NOEXCEPT {
    return GblStringRef_length(pSelf) == 0;
}

GBL_EXPORT GblBool GblStringRef_valid(const GblStringRef* pSelf) GBL_NOEXCEPT {
    return pSelf? GBL_TRUE : GBL_FALSE;
}

GBL_EXPORT GblStringView GblStringRef_view(const GblStringRef* pSelf) GBL_NOEXCEPT {
    GblStringView view = GBL_STRING_VIEW(pSelf, GblStringRef_length(pSelf));
    view.nullTerminated = 1;
    return view;
}

GBL_EXPORT GblStringRef* GblStringRef_createFromViewWithContext(GblStringView view,
                                                                GblContext* pCtx)
{
    GblStringRef* pStr = NULL;
    GBL_CTX_BEGIN(pCtx);
    //GBL_CTX_VERIFY_ARG(view.length);
    //GBL_CTX_VERIFY_POINTER(view.pData);
    GblStringRef_* pStrHeader =
            (GblStringRef_*)GblRef_allocWithContext(sizeof(GblStringRef_) + view.length,
                                                    pCtx);
    GBL_CTX_VERIFY_LAST_RECORD();
    if(view.pData && view.length)
        strncpy(pStrHeader->data, view.pData, view.length);
    else
        pStrHeader->data[0] = '\0';
    pStrHeader->length = view.length;
    pStr = pStrHeader->data;
    GBL_CTX_END_BLOCK();
    return pStr;
}

GBL_EXPORT GblStringRef* GblStringRef_acquire(const GblStringRef* pSelf) {
    GBL_CTX_BEGIN(NULL);
    GblStringRef* pOut = NULL;
    if(pSelf) {
        GblStringRef_* pStrHeader = GblStringRef_header_(pSelf);
        pStrHeader = (GblStringRef_*)GblRef_acquire(pStrHeader);
        pOut = pStrHeader->data;
        GBL_CTX_VERBOSE("[GblStringRef]: [%s]++ = %u",
                        pSelf,
                        GblStringRef_refCount(pSelf));
    }
    GBL_CTX_END_BLOCK();
    return pOut;
}

GBL_EXPORT GblRefCount GblStringRef_release(const GblStringRef* pSelf) {
    GBL_CTX_BEGIN(NULL);
    GblRefCount count = 0;
    if(pSelf) {
        GblStringRef_* pStrHeader = GblStringRef_header_(pSelf);
        GBL_CTX_VERBOSE("[GblStringRef]: [%s]-- = %u",
                        pSelf,
                        GblStringRef_refCount(pSelf)-1);
        count = GblRef_release(pStrHeader);
    }
    GBL_CTX_END_BLOCK();
    return count;
}

GBL_EXPORT char GblStringRef_at(const GblStringRef* pSelf, size_t  index) {
    char value = '\0';
    GBL_CTX_BEGIN(GblStringRef_context(pSelf));
    GBL_CTX_VERIFY(index < GblStringRef_length(pSelf),
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    value = pSelf[index];
    GBL_CTX_END_BLOCK();
    return value;
}

GBL_EXPORT GblHash GblStringRef_hash(const GblStringRef* pSelf) {
    return gblHash(pSelf, GblStringRef_length(pSelf));
}

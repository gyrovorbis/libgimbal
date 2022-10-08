#include <gimbal/strings/gimbal_string_ref.h>


GBL_EXPORT GblStringRef* GblStringRef_createFromViewWithContext(GblStringView view,
                                                                GblContext* pCtx) GBL_NOEXCEPT
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
    pStrHeader->length = view.length;
    pStr = pStrHeader->data;
    GBL_CTX_END_BLOCK();
    return pStr;
}

GBL_EXPORT GblStringRef* GblStringRef_acquire(const GblStringRef* pSelf) GBL_NOEXCEPT {
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

GBL_EXPORT GblRefCount GblStringRef_release(const GblStringRef* pSelf) GBL_NOEXCEPT {
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

GBL_EXPORT char GblStringRef_at(const GblStringRef* pSelf, GblSize index) GBL_NOEXCEPT {
    char value = '\0';
    GBL_CTX_BEGIN(GblStringRef_context(pSelf));
    GBL_CTX_VERIFY(index < GblStringRef_length(pSelf),
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    value = pSelf[index];
    GBL_CTX_END_BLOCK();
    return value;
}

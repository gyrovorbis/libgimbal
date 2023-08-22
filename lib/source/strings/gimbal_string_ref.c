#include <gimbal/strings/gimbal_string_ref.h>
#include <gimbal/utils/gimbal_ref.h>
#include <gimbal/algorithms/gimbal_hash.h>

typedef struct GblStringRef_ {
    size_t      length;
    char        data[1];
} GblStringRef_;

GBL_INLINE GblStringRef_* GblStringRef_header_(const GblStringRef* pString) {
    return (GblStringRef_*)(pString - offsetof(GblStringRef_, data));
}

GBL_EXPORT const GblStringRef* (GblStringRef_create)(const char* pString,
                                                     size_t      length,
                                                     GblContext* pCtx)
{
    GblStringRef* pStr = NULL;
    GBL_CTX_BEGIN(pCtx);

    if(pString && !length)
        length = strlen(pString);

    GblStringRef_* pStrHeader =
            (GblStringRef_*)GblRef_allocWithContext(sizeof(GblStringRef_) + length,
                                                    pCtx);

    GBL_CTX_VERIFY_LAST_RECORD();
    if(pString && length)
        strncpy(pStrHeader->data, pString, length);
    else
        pStrHeader->data[0] = '\0';

    pStrHeader->length = length;
    pStr = pStrHeader->data;

    GBL_CTX_END_BLOCK();
    return pStr;
}

GBL_EXPORT GblContext* GblStringRef_context(const GblStringRef* pSelf) {
    GblContext* pCtx = NULL;

    if(pSelf) {
        GblStringRef_* pStrHeader = GblStringRef_header_(pSelf);
        pCtx = GblRef_context(pStrHeader);
    }

    return pCtx;
}

GBL_EXPORT GblRefCount GblStringRef_refCount(const GblStringRef* pSelf) {
    GblRefCount refCount = 0;

    if(pSelf) {
        GblStringRef_* pStrHeader = GblStringRef_header_(pSelf);
        refCount = GblRef_refCount(pStrHeader);
    }

    return refCount;
}

GBL_EXPORT size_t  GblStringRef_length(const GblStringRef* pSelf) {
    size_t  length = 0;

    if(pSelf) {
        GblStringRef_* pStrHeader = GblStringRef_header_(pSelf);
        length = pStrHeader->length;
    }

    return length;
}

GBL_EXPORT GblBool GblStringRef_blank(const GblStringRef* pSelf) {
    return pSelf?
                GblStringView_blank(GblStringView_fromString(pSelf, GblStringRef_length(pSelf))) :
                GBL_TRUE;
}

GBL_EXPORT GblBool GblStringRef_empty(const GblStringRef* pSelf) {
    return GblStringRef_length(pSelf);
}

GBL_EXPORT GblBool GblStringRef_valid(const GblStringRef* pSelf) {
    return !!pSelf;
}

GBL_EXPORT GblStringView (GblStringRef_view)(const GblStringRef* pSelf, size_t offset, size_t length) {
    const size_t strLen = GblStringRef_length(pSelf);

    if(!length)
        length = strLen;

    const size_t end = offset + length;
    if(end > strLen) {
        GBL_CTX_BEGIN(NULL);
        GBL_CTX_VERIFY(GBL_FALSE,
                       GBL_RESULT_ERROR_OUT_OF_RANGE,
                       "[GblStringRef_view(%s, %zu, %zu): Out of Range! [Length: %zu]",
                       pSelf? pSelf : NULL,
                       offset,
                       length,
                       strLen);
        GBL_CTX_END_BLOCK();
        return (GblStringView) { 0 };
    }

    return (GblStringView) {
        .pData  = pSelf + offset,
        .length = length,
        .nullTerminated = (length == end)? 1 : 0
    };
}



GBL_EXPORT GblStringRef* GblStringRef_ref(const GblStringRef* pSelf) {
    GblStringRef* pOut = NULL;

    if(pSelf) {
        GblStringRef_* pStrHeader = GblStringRef_header_(pSelf);
        pStrHeader = (GblStringRef_*)GblRef_acquire(pStrHeader);
        pOut = pStrHeader->data;

        GBL_CTX_VERBOSE("[GblStringRef]: [%s]++ = %u",
                        pSelf,
                        GblStringRef_refCount(pSelf));
    }

    return pOut;
}

GBL_EXPORT GblRefCount GblStringRef_unref(const GblStringRef* pSelf) {
    GblRefCount count = 0;

    if(pSelf) {
        GblStringRef_* pStrHeader = GblStringRef_header_(pSelf);

        GBL_CTX_VERBOSE("[GblStringRef]: [%s]-- = %u",
                        pSelf,
                        GblStringRef_refCount(pSelf)-1);
        count = GblRef_release(pStrHeader);
    }

    return count;
}

GBL_EXPORT char GblStringRef_at(const GblStringRef* pSelf, size_t index) {
    char value = '\0';

    if(index >= GblStringRef_length(pSelf)) {
        GBL_CTX_BEGIN(GblStringRef_context(pSelf));
        GBL_CTX_VERIFY(GBL_FALSE,
                       GBL_RESULT_ERROR_OUT_OF_RANGE,
                       "GblStringRef_at(%s, %zu): Out of range!",
                       pSelf? pSelf : "",
                       index);
        GBL_CTX_END_BLOCK();

    } else value = pSelf[index];

    return value;
}

GBL_EXPORT GblHash GblStringRef_hash(const GblStringRef* pSelf) {
    return gblHash(pSelf, GblStringRef_length(pSelf));
}

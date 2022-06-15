#ifndef GIMBAL_STRING_REF_H
#define GIMBAL_STRING_REF_H

#include "../core/gimbal_api_frame.h"
#include "../types/gimbal_typedefs.h"
#include "../types/gimbal_ref.h"
#include "gimbal_string_view.h"

#define SELF    GblStringRef* pSelf
#define CSELF   const SELF

GBL_DECLS_BEGIN

typedef const char GblStringRef;

GBL_INLINE GblStringRef*    GblStringRef_create                     (const char* pString)   GBL_NOEXCEPT;
GBL_INLINE GblStringRef*    GblStringRef_createWithContext          (const char* pString,
                                                                     GblContext* pCtx)      GBL_NOEXCEPT;
GBL_INLINE GblStringRef*    GblStringRef_createFromView             (GblStringView view)    GBL_NOEXCEPT;
GBL_INLINE GblStringRef*    GblStringRef_createFromViewWithContext  (GblStringView view,
                                                                     GblContext*   pCtx)    GBL_NOEXCEPT;

GBL_INLINE GblStringRef*    GblStringRef_acquire                    (CSELF)                 GBL_NOEXCEPT;
GBL_INLINE GblRefCount      GblStringRef_release                    (CSELF)                 GBL_NOEXCEPT;

GBL_INLINE char             GblStringRef_at                         (CSELF, GblSize index)  GBL_NOEXCEPT;

GBL_INLINE GblContext*      GblStringRef_context                    (CSELF)                 GBL_NOEXCEPT;
GBL_INLINE GblRefCount      GblStringRef_refCount                   (CSELF)                 GBL_NOEXCEPT;
GBL_INLINE GblSize          GblStringRef_length                     (CSELF)                 GBL_NOEXCEPT;
GBL_INLINE GblBool          GblStringRef_empty                      (CSELF)                 GBL_NOEXCEPT;
GBL_INLINE GblBool          GblStringRef_valid                      (CSELF)                 GBL_NOEXCEPT;
GBL_INLINE GblStringView    GblStringRef_view                       (CSELF)                 GBL_NOEXCEPT;

// ========== IMPL ==========

typedef struct GblStringRef_ {
    GblSize     length;
    char        data[1];
} GblStringRef_;

GBL_INLINE GblStringRef_* GblStringRef_header_(const GblStringRef* pString) GBL_NOEXCEPT {
    GblStringRef_* pHeader = (GblStringRef_*)(pString - offsetof(GblStringRef_, data));
    return pHeader;
}

GBL_INLINE GblStringRef* GblStringRef_create(const char* pString) GBL_NOEXCEPT {
    return GblStringRef_createWithContext(pString, NULL);
}

GBL_INLINE GblStringRef* GblStringRef_createWithContext(const char* pString, GblContext* pCtx) GBL_NOEXCEPT {
    return GblStringRef_createFromViewWithContext(GBL_STRING_VIEW(pString), pCtx);
}

GBL_INLINE GblStringRef* GblStringRef_createFromView(GblStringView view) GBL_NOEXCEPT {
    return GblStringRef_createFromViewWithContext(view, NULL);
}

GBL_INLINE GblStringRef* GblStringRef_createFromViewWithContext(GblStringView view,
                                                                GblContext* pCtx) GBL_NOEXCEPT
{
    GblStringRef* pStr = NULL;
    GBL_API_BEGIN(pCtx);
    //GBL_API_VERIFY_ARG(view.length);
    //GBL_API_VERIFY_POINTER(view.pData);
    GblStringRef_* pStrHeader =
            (GblStringRef_*)GblRef_allocWithContext(sizeof(GblStringRef_) + view.length,
                                                    pCtx);
    GBL_API_VERIFY_LAST_RECORD();
    if(view.pData && view.length)
        strncpy(pStrHeader->data, view.pData, view.length);
    pStrHeader->length = view.length;
    pStr = pStrHeader->data;
    GBL_API_END_BLOCK();
    return pStr;
}

GBL_INLINE GblStringRef* GblStringRef_acquire(CSELF) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GblStringRef* pOut = NULL;
    if(pSelf) {
        GblStringRef_* pStrHeader = GblStringRef_header_(pSelf);
        pStrHeader = (GblStringRef_*)GblRef_acquire(pStrHeader);
        pOut = pStrHeader->data;
        GBL_API_VERBOSE("[GblStringRef]: [%s]++ = %u",
                        pSelf,
                        GblStringRef_refCount(pSelf));
    }
    GBL_API_END_BLOCK();
    return pOut;
}

GBL_INLINE GblRefCount GblStringRef_release(CSELF) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GblRefCount count = 0;
    if(pSelf) {
        GblStringRef_* pStrHeader = GblStringRef_header_(pSelf);
        GBL_API_VERBOSE("[GblStringRef]: [%s]-- = %u",
                        pSelf,
                        GblStringRef_refCount(pSelf)-1);
        count = GblRef_release(pStrHeader);
    }
    GBL_API_END_BLOCK();
    return count;
}

GBL_INLINE char GblStringRef_at(CSELF, GblSize index) GBL_NOEXCEPT {
    char value = '\0';
    GBL_API_BEGIN(GblStringRef_context(pSelf));
    GBL_API_VERIFY(index < GblStringRef_length(pSelf),
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    value = pSelf[index];
    GBL_API_END_BLOCK();
    return value;
}

GBL_INLINE GblContext* GblStringRef_context(CSELF) GBL_NOEXCEPT {
    GblContext* pCtx = NULL;
    if(pSelf) {
        GblStringRef_* pStrHeader = GblStringRef_header_(pSelf);
        pCtx = GblRef_context(pStrHeader);
    }
    return pCtx;
}

GBL_INLINE GblRefCount GblStringRef_refCount(CSELF) GBL_NOEXCEPT {
    GblRefCount refCount = 0;
    if(pSelf) {
        GblStringRef_* pStrHeader = GblStringRef_header_(pSelf);
        refCount = GblRef_refCount(pStrHeader);
    }
    return refCount;
}

GBL_INLINE GblSize GblStringRef_length(CSELF) GBL_NOEXCEPT {
    GblSize length = 0;
    if(pSelf) {
        GblStringRef_* pStrHeader = GblStringRef_header_(pSelf);
        length = pStrHeader->length;
    }
    return length;
}

GBL_INLINE GblBool GblStringRef_empty(CSELF) GBL_NOEXCEPT {
    return GblStringRef_length(pSelf) == 0;
}

GBL_INLINE GblBool GblStringRef_valid(CSELF) GBL_NOEXCEPT {
    return pSelf? GBL_TRUE : GBL_FALSE;
}

GBL_INLINE GblStringView GblStringRef_view(CSELF) GBL_NOEXCEPT {
    return GBL_STRING_VIEW(pSelf, GblStringRef_length(pSelf));
}


GBL_DECLS_END

#undef CSELF
#undef SELF

#endif // GIMBAL_STRING_REF_H

/*! \file
 *  \brief ::GblStringRef type and related functions
 */


#ifndef GIMBAL_STRING_REF_H
#define GIMBAL_STRING_REF_H

#include "../core/gimbal_api_frame.h"
#include "../types/gimbal_typedefs.h"
#include "../types/gimbal_ref.h"
#include "gimbal_string_view.h"

#define SELF    GblStringRef* pSelf
#define CSELF   const SELF

GBL_DECLS_BEGIN

/*! \brief Reference-counted string type
 *  \ingroup strings
 */
typedef char GblStringRef;

GBL_INLINE GblStringRef*    GblStringRef_create                     (const char* pString)   GBL_NOEXCEPT;
GBL_INLINE GblStringRef*    GblStringRef_createWithContext          (const char* pString,
                                                                     GblContext* pCtx)      GBL_NOEXCEPT;
GBL_INLINE GblStringRef*    GblStringRef_createFromView             (GblStringView view)    GBL_NOEXCEPT;
GBL_EXPORT GblStringRef*    GblStringRef_createFromViewWithContext  (GblStringView view,
                                                                     GblContext*   pCtx)    GBL_NOEXCEPT;

GBL_EXPORT GblStringRef*    GblStringRef_acquire                    (CSELF)                 GBL_NOEXCEPT;
GBL_EXPORT GblRefCount      GblStringRef_release                    (CSELF)                 GBL_NOEXCEPT;

GBL_EXPORT char             GblStringRef_at                         (CSELF, GblSize index)  GBL_NOEXCEPT;

GBL_INLINE GblContext*      GblStringRef_context                    (CSELF)                 GBL_NOEXCEPT;
GBL_INLINE GblRefCount      GblStringRef_refCount                   (CSELF)                 GBL_NOEXCEPT;
GBL_INLINE GblSize          GblStringRef_length                     (CSELF)                 GBL_NOEXCEPT;
GBL_INLINE GblBool          GblStringRef_empty                      (CSELF)                 GBL_NOEXCEPT;
GBL_INLINE GblBool          GblStringRef_valid                      (CSELF)                 GBL_NOEXCEPT;
GBL_INLINE GblStringView    GblStringRef_view                       (CSELF)                 GBL_NOEXCEPT;

// ========== IMPL ==========

/// \cond
typedef struct GblStringRef_ {
    GblSize     length;
    char        data[1];
} GblStringRef_;

GBL_INLINE GblStringRef_* GblStringRef_header_(const GblStringRef* pString) GBL_NOEXCEPT {
    GblStringRef_* pHeader = (GblStringRef_*)(pString - offsetof(GblStringRef_, data));
    return pHeader;
}
/// \endcond

GBL_INLINE GblStringRef* GblStringRef_create(const char* pString) GBL_NOEXCEPT {
    return GblStringRef_createWithContext(pString, NULL);
}

GBL_INLINE GblStringRef* GblStringRef_createWithContext(const char* pString, GblContext* pCtx) GBL_NOEXCEPT {
    return GblStringRef_createFromViewWithContext(GBL_STRING_VIEW(pString), pCtx);
}

GBL_INLINE GblStringRef* GblStringRef_createFromView(GblStringView view) GBL_NOEXCEPT {
    return GblStringRef_createFromViewWithContext(view, NULL);
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

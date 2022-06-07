#ifndef GIMBAL_MEMORY_REF_H
#define GIMBAL_MEMORY_REF_H

#include "../preprocessor/gimbal_atomics.h"
#include "../core/gimbal_api_frame.h"
#include "gimbal_typedefs.h"

#define SELF    GblRef* pSelf
#define CSELF   const SELF

GBL_DECLS_BEGIN

typedef void            GblRef;
typedef GBL_RESULT      (*GblRefDestructFn)     (SELF);

GBL_EXPORT GBL_RESULT   GblRef_reinit           (void)                      GBL_NOEXCEPT;
GBL_EXPORT GblRefCount  GblRef_activeCount      (void)                      GBL_NOEXCEPT;

GBL_INLINE void*        GblRef_alloc            (GblSize size)              GBL_NOEXCEPT;
GBL_EXPORT void*        GblRef_allocWithContext (GblSize size,
                                                 GblContext* pCtx)          GBL_NOEXCEPT;

GBL_INLINE void*        GblRef_acquire          (SELF)                      GBL_NOEXCEPT;
GBL_INLINE GblRefCount  GblRef_release          (SELF)                      GBL_NOEXCEPT;
GBL_EXPORT GblRefCount  GblRef_releaseWithDtor  (SELF,
                                                 GblRefDestructFn pFnDtor)  GBL_NOEXCEPT;

GBL_INLINE GblRefCount  GblRef_refCount         (CSELF)                     GBL_NOEXCEPT;
GBL_INLINE GblContext*  GblRef_context          (CSELF)                     GBL_NOEXCEPT;

// ========== IMPL ==========

typedef struct GblRef_ {
    GblContext*         pCtx;
    GBL_ATOMIC_INT16    refCount;
} GblRef_;

GBL_INLINE GblRef_* GblRef_header_(const void* pData) GBL_NOEXCEPT {
    return (GblRef_*)(pData? (GblRef_*)pData - 1 : NULL);
}

GBL_INLINE void* GblRef_alloc(GblSize size) GBL_NOEXCEPT {
    return GblRef_allocWithContext(size, NULL);
}

GBL_INLINE void* GblRef_acquire(SELF) GBL_NOEXCEPT
{
    void* pRetData = NULL;
    if(pSelf) {
        GblRef_* pHeader = GblRef_header_(pSelf);
        GBL_ATOMIC_INT16_INC(pHeader->refCount);
        pRetData = pSelf;
    }
    return pRetData;
}

GBL_INLINE GblRefCount GblRef_release(SELF) GBL_NOEXCEPT {
    return GblRef_releaseWithDtor(pSelf, NULL);
}

GBL_INLINE GblRefCount GblRef_refCount(CSELF) GBL_NOEXCEPT {
    GblRefCount refCount = 0;
    if(pSelf) {
        GblRef_* pHeader = GblRef_header_(pSelf);
        refCount = GBL_ATOMIC_INT16_LOAD(pHeader->refCount);
    }
    return refCount;
}

GBL_INLINE GblContext* GblRef_context(CSELF) GBL_NOEXCEPT {
    GblContext* pCtx = NULL;
    if(pSelf) {
        GblRef_* pHeader = GblRef_header_(pSelf);
        pCtx = pHeader->pCtx;
    }
    return pCtx;
}

GBL_DECLS_END

#undef CSELF
#undef SELF

#endif // GIMBAL_MEMORY_REF_H

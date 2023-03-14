/*! \file
 *  \brief Generic, atomic reference-counted data types
 *  \ingroup utils
 */

#ifndef GIMBAL_MEMORY_REF_H
#define GIMBAL_MEMORY_REF_H

#include "../core/gimbal_atomics.h"
#include "../core/gimbal_ctx.h"

#define GBL_SELF_TYPE GblRef

GBL_DECLS_BEGIN

typedef void            GblRef;

typedef GBL_RESULT      (*GblRefDestructFn)     (GBL_SELF);

GBL_EXPORT GBL_RESULT   GblRef_reinit           (void)                               GBL_NOEXCEPT;
GBL_EXPORT GblRefCount  GblRef_activeCount      (void)                               GBL_NOEXCEPT;

GBL_INLINE void*        GblRef_alloc            (GblSize size)                       GBL_NOEXCEPT;
GBL_EXPORT void*        GblRef_allocWithContext (GblSize size, GblContext* pCtx)     GBL_NOEXCEPT;

GBL_INLINE void*        GblRef_acquire          (GBL_SELF)                           GBL_NOEXCEPT;
GBL_INLINE GblRefCount  GblRef_release          (GBL_SELF)                           GBL_NOEXCEPT;
GBL_EXPORT GblRefCount  GblRef_releaseWithDtor  (GBL_SELF, GblRefDestructFn pFnDtor) GBL_NOEXCEPT;

GBL_INLINE GblRefCount  GblRef_refCount         (GBL_CSELF)                          GBL_NOEXCEPT;
GBL_INLINE GblContext*  GblRef_context          (GBL_CSELF)                          GBL_NOEXCEPT;

#define GblRef_create(...)  GBL_VA_OVERLOAD_CALL(GblRef_create_, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)
#define GblRef_ref(ref)     GblRef_acquire(ref)
#define GblRef_unref(...)   GBL_VA_OVERLOAD_CALL(GblRef_unref_, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)

// ========== IMPL ==========

/// \cond

#define GblRef_create__1(size)      GblRef_alloc(size)
#define GblRef_create__2(size, ctx) GblRef_allocWithContext(size, ctx)

#define GblRef_unref__1(ref)        GblRef_release(ref)
#define GblRef_unref__2(ref, dtor)  GblRef_releaseWithDtor(ref, dtor)

typedef struct GblRef_ {
    GblContext*         pCtx;
    GBL_ATOMIC_INT16    refCount;
} GblRef_;

GBL_INLINE GblRef_* GblRef_header_(const void* pData) GBL_NOEXCEPT {
    return (GblRef_*)(pData? (GblRef_*)pData - 1 : NULL);
}
/// \endcond

GBL_INLINE void* GblRef_alloc(GblSize size) GBL_NOEXCEPT {
    return GblRef_allocWithContext(size, NULL);
}

GBL_INLINE void* GblRef_acquire(GBL_SELF) GBL_NOEXCEPT
{
    void* pRetData = NULL;
    if(pSelf) {
        GblRef_* pHeader = GblRef_header_(pSelf);
        GBL_ATOMIC_INT16_INC(pHeader->refCount);
        pRetData = pSelf;
    }
    return pRetData;
}

GBL_INLINE GblRefCount GblRef_release(GBL_SELF) GBL_NOEXCEPT {
    return GblRef_releaseWithDtor(pSelf, NULL);
}

GBL_INLINE GblRefCount GblRef_refCount(GBL_CSELF) GBL_NOEXCEPT {
    GblRefCount refCount = 0;
    if(pSelf) {
        GblRef_* pHeader = GblRef_header_(pSelf);
        refCount = GBL_ATOMIC_INT16_LOAD(pHeader->refCount);
    }
    return refCount;
}

GBL_INLINE GblContext* GblRef_context(GBL_CSELF) GBL_NOEXCEPT {
    GblContext* pCtx = NULL;
    if(pSelf) {
        GblRef_* pHeader = GblRef_header_(pSelf);
        pCtx = pHeader->pCtx;
    }
    return pCtx;
}

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_MEMORY_REF_H

/*! \file
 *  \brief Generic reference-counted pointers
 *  \ingroup utils
 *
 *  This file contains GblRef, an atomic reference-counted
 *  shared pointer type, as well as its associated API.
 *
 *  \note
 *  The reference-counting metadata for a GblRef is stored
 *  within a header above the returned pointer, meaning
 *  that a client can use a GblRef as though it were a
 *  regular void* type, other than managing its lifetime.
 *
 *  \author     2023 Falco Girgis
 *  \copyright  MIT License
 */
#ifndef GIMBAL_REF_H
#define GIMBAL_REF_H

#include "../core/gimbal_ctx.h"

#define GBL_SELF_TYPE GblRef

GBL_DECLS_BEGIN

/*! Reference-counted shared pointer type
 *  \ingroup utils
 */
typedef void GblRef;

typedef GBL_RESULT      (*GblRefDestructFn)     (GBL_SELF);

GBL_EXPORT GBL_RESULT   GblRef_reinit           (void)                               GBL_NOEXCEPT;
GBL_EXPORT GblRefCount  GblRef_activeCount      (void)                               GBL_NOEXCEPT;

GBL_EXPORT void*        GblRef_alloc            (size_t size)                       GBL_NOEXCEPT;
GBL_EXPORT void*        GblRef_allocWithContext (size_t size, GblContext* pCtx)     GBL_NOEXCEPT;

GBL_EXPORT void*        GblRef_acquire          (GBL_CSELF)                           GBL_NOEXCEPT;
GBL_EXPORT GblRefCount  GblRef_release          (GBL_CSELF)                          GBL_NOEXCEPT;
GBL_EXPORT GblRefCount  GblRef_releaseWithDtor  (GBL_CSELF, GblRefDestructFn pFnDtor) GBL_NOEXCEPT;

GBL_EXPORT GblRefCount  GblRef_refCount         (GBL_CSELF)                          GBL_NOEXCEPT;
GBL_EXPORT GblContext*  GblRef_context          (GBL_CSELF)                          GBL_NOEXCEPT;

GBL_DECLS_END

#define GblRef_create(...)  GBL_VA_OVERLOAD_CALL_ARGC(GblRef_create_, __VA_ARGS__)
#define GblRef_ref(ref)     GblRef_acquire(ref)
#define GblRef_unref(...)   GBL_VA_OVERLOAD_CALL_ARGC(GblRef_unref_, __VA_ARGS__)

#define GblRef_create__1(size)      GblRef_alloc(size)
#define GblRef_create__2(size, ctx) GblRef_allocWithContext(size, ctx)

#define GblRef_unref__1(ref)        GblRef_release(ref)
#define GblRef_unref__2(ref, dtor)  GblRef_releaseWithDtor(ref, dtor)

#undef GBL_SELF_TYPE

#endif // GIMBAL_REF_H

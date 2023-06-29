/*! \file
 *  \brief GblCClosure base instance and API
 *  \copydoc GblCClosure
 *  \ingroup signals
 *
 *  \author Falco Girgis
 */


#ifndef GIMBAL_C_CLOSURE_H
#define GIMBAL_C_CLOSURE_H

#include "gimbal_closure.h"

#define GBL_C_CLOSURE_TYPE                  (GBL_TYPEOF(GblCClosure))

#define GBL_C_CLOSURE(instance)             (GBL_INSTANCE_CAST(instance, GblCClosure))
#define GBL_C_CLOSURE_CLASS(klass)          (GBL_CLASS_CAST(klass, GblCClosure))
#define GBL_C_CLOSURE_GET_CLASS(instance)   (GBL_INSTANCE_GET_CLASS(instance, GblCClosure))

#define GBL_SELF_TYPE GblCClosure

GBL_DECLS_BEGIN

/*! \struct  GblCClosureClass
 *  \brief   GblClass VTable for GblCClosure
 *  \extends GblClosureClass
 *
 *  No public members.
 *
 *  \sa GblCClosure
 */
GBL_CLASS_DERIVE_EMPTY(GblCClosure, GblClosure)

/*! \struct  GblCClosure
 *  \brief   Clsoure type for invoking C functions
 *  \extends GblClosure
 *  \ingroup signals
 *
 *  GblCClosure is a GblClosure which is used to
 *  marshal generic arguments into a C function
 *  pointer and marshal out the return value.
 */
GBL_INSTANCE_DERIVE(GblCClosure, GblClosure)
    GBL_PRIVATE()
        GblFnPtr    pFnCallback;
    GBL_PRIVATE_END
GBL_INSTANCE_END

GBL_EXPORT GblType      GblCClosure_type        (void)                 GBL_NOEXCEPT;

GBL_EXPORT GblCClosure* GblCClosure_create      (GblFnPtr pFnCallback,
                                                 void*    pUserdata)   GBL_NOEXCEPT;

GBL_INLINE void         GblCClosure_setCallback (GBL_SELF,
                                                 GblFnPtr pFnCallback) GBL_NOEXCEPT;

// ===== IMPL =====

GBL_INLINE void GblCClosure_setCallback(GBL_SELF, GblFnPtr pFnCallback) GBL_NOEXCEPT {
    GBL_PRIV_REF(pSelf).pFnCallback = pFnCallback;
}

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_C_CLOSURE_H

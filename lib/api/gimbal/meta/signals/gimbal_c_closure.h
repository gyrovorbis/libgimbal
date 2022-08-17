#ifndef GIMBAL_C_CLOSURE_H
#define GIMBAL_C_CLOSURE_H

#include "gimbal_closure.h"

#define GBL_C_CLOSURE_TYPE                  (GblCClosure_type())

#define GBL_C_CLOSURE(instance)             (GBL_INSTANCE_CAST(instance, GBL_C_CLOSURE_TYPE, GblCClosure))
#define GBL_C_CLOSURE_CLASS(klass)          (GBL_CLASS_CAST(klass, GBL_C_CLOSURE_TYPE, GblCClosureClass))
#define GBL_C_CLOSURE_GET_CLASS(instance)   (GBL_INSTANCE_GET_CLASS(instance, GBL_C_CLOSURE_TYPE, GblCClosureClass))

#define SELF    GblCClosure* pSelf
#define CSELF   const SELF

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblCClosureClass, GblClosureClass)

GBL_INSTANCE_DERIVE(GblCClosure, GblClosure)
    GBL_PRIVATE()
        GblFnPtr    pFnCallback;
    GBL_PRIVATE_END
GBL_INSTANCE_END

GBL_EXPORT GblType      GblCClosure_type       (void)                 GBL_NOEXCEPT;

GBL_EXPORT GblCClosure* GblCClosure_create     (GblFnPtr pFnCallback,
                                                void*    pUserdata)   GBL_NOEXCEPT;

GBL_INLINE void         GblCClosure_setCallback(SELF,
                                                GblFnPtr pFnCallback) GBL_NOEXCEPT;

// ===== IMPL =====

GBL_INLINE void GblCClosure_setCallback(SELF, GblFnPtr pFnCallback) GBL_NOEXCEPT {
    GBL_PRIV_REF(pSelf).pFnCallback = pFnCallback;
}

GBL_DECLS_END

#undef CSELF
#undef SELF

#endif // GIMBAL_C_CLOSURE_H

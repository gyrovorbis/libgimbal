/*! \file
 *  \brief GblClassClosure base instance and API
 *  \copydoc GblClassClosure
 *  \ingroup signals
 */


#ifndef GIMBAL_CLASS_CLOSURE_H
#define GIMBAL_CLASS_CLOSURE_H

#include "gimbal_closure.h"

#define GBL_CLASS_CLOSURE_TYPE                  (GBL_TYPEOF(GblClassClosure))

#define GBL_CLASS_CLOSURE(instance)             (GBL_INSTANCE_CAST(instance, GblClassClosure))
#define GBL_CLASS_CLOSURE_CLASS(klass)          (GBL_CLASS_CAST(klass, GblClassClosure))
#define GBL_CLASS_CLOSURE_GET_CLASS(instance)   (GBL_INSTANCE_GET_CLASS(instance, GblClassClosure))

#define GBL_SELF_TYPE GblClassClosure

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblClassClosure, GblClosure)

GBL_INSTANCE_DERIVE(GblClassClosure, GblClosure)
    GBL_PRIVATE()
        GblType         classType;
        GblSize         offset;
        GblInstance*    pInstance;
    GBL_PRIVATE_END
GBL_INSTANCE_END

GBL_EXPORT GblType          GblClassClosure_type        (void)                   GBL_NOEXCEPT;

GBL_EXPORT GblClassClosure* GblClassClosure_create      (GblType      classType,
                                                         GblSize      offset,
                                                         GblInstance* pInstance,
                                                         void*        pUserdata) GBL_NOEXCEPT;

GBL_INLINE void             GblClassClosure_setMethod   (GBL_SELF,
                                                         GblType classType,
                                                         GblSize offset)         GBL_NOEXCEPT;

GBL_INLINE void             GblClassClosure_setInstance (GBL_SELF,
                                                         GblInstance* pInstance) GBL_NOEXCEPT;

// ===== IMPL ======


GBL_INLINE void GblClassClosure_setMethod(GBL_SELF, GblType classType, GblSize offset) GBL_NOEXCEPT {
    GBL_PRIV_REF(pSelf).classType = classType;
    GBL_PRIV_REF(pSelf).offset = offset;
}

GBL_INLINE void GblClassClosure_setInstance(GBL_SELF, GblInstance* pInstance) GBL_NOEXCEPT {
    GBL_PRIV_REF(pSelf).pInstance = pInstance;
}


GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_CLASS_CLOSURE_H

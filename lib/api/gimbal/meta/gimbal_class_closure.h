#ifndef GIMBAL_CLASS_CLOSURE_H
#define GIMBAL_CLASS_CLOSURE_H

#include "gimbal_closure.h"

#define GBL_CLASS_CLOSURE_TYPE                  (GblClassClosure_type())

#define GBL_CLASS_CLOSURE(instance)             (GBL_INSTANCE_CAST(instance, GBL_CLASS_CLOSURE_TYPE, GblClassClosure))
#define GBL_CLASS_CLOSURE_CLASS(klass)          (GBL_CLASS_CAST(klass, GBL_CLASS_CLOSURE_TYPE, GblClassClosureClass))
#define GBL_CLASS_CLOSURE_GET_CLASS(instance)   (GBL_INSTANCE_GET_CLASS(instance), GBL_CLASS_CLOSURE_TYPE, GblClassClosureClass)

#define SELF    GblClassClosure* pSelf
#define CSELF   const SELF

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE(GblClassClosureClass, GblClosureClass)
GBL_CLASS_END

GBL_INSTANCE_DERIVE(GblClassClosure, GblClosure)
    GBL_PRIVATE()
        GblType         classType;
        GblSize         offset;
        GblInstance*    pInstance;
    GBL_PRIVATE_END
GBL_INSTANCE_END

GBL_EXPORT GblType GblClassClosure_type       (void)                   GBL_NOEXCEPT;

GBL_INLINE void    GblClassClosure_setMethod  (SELF,
                                               GblType classType,
                                               GblSize offset)         GBL_NOEXCEPT;

GBL_INLINE void    GblClassClosure_setInstance(SELF,
                                               GblInstance* pInstance) GBL_NOEXCEPT;

// ===== IMPL ======


GBL_INLINE void GblClassClosure_setMethod(SELF, GblType classType, GblSize offset) GBL_NOEXCEPT {
    GBL_PRIV_REF(pSelf).classType = classType;
    GBL_PRIV_REF(pSelf).offset = offset;
}

GBL_INLINE void GblClassClsoure_setInstance(SELF, GblInstance* pInstance) GBL_NOEXCEPT {
    GBL_PRIV_REF(pSelf).pInstance = pInstance;
}


GBL_DECLS_END

#undef CSELF
#undef SELF

#endif // GIMBAL_CLASS_CLOSURE_H

#ifndef GIMBAL_CLOSURE_H
#define GIMBAL_CLOSURE_H

#include "../instances/gimbal_box.h"
#include "gimbal_marshal.h"

#define GBL_CLOSURE_TYPE                (GblClosure_type())

#define GBL_CLOSURE(instance)           (GBL_INSTANCE_CAST(instance, GBL_CLOSURE_TYPE, GblClosure))
#define GBL_CLOSURE_CLASS(klass)        (GBL_CLASS_CAST(klass, GBL_CLOSURE_TYPE, GblClosureClass))
#define GBL_CLOSURE_GET_CLASS(instance) (GBL_INSTANCE_GET_CLASS(instance, GBL_CLOSURE_TYPE, GblClosureClass))

#define GBL_CALLBACK(fn)                    ((GblFnPtr)fn)

#define GBL_SELF    GblClosure* pSelf
#define GBL_CSELF   const GBL_SELF

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE(GblClosureClass, GblBoxClass)
    GblMarshalFn    pFnMetaMarshal;
GBL_CLASS_END

GBL_INSTANCE_DERIVE(GblClosure, GblBox)
    GBL_PRIVATE()
        GblMarshalFn    pFnMarshal;
    GBL_PRIVATE_END
GBL_INSTANCE_END

GBL_EXPORT GblType     GblClosure_type          (void)                          GBL_NOEXCEPT;
GBL_EXPORT GblClosure* GblClosure_current       (void)                          GBL_NOEXCEPT;

GBL_EXPORT GblClosure* GblClosure_create        (GblType           derivedType,
                                                 GblSize           size,
                                                 void*             pUserdata,
                                                 GblArrayMapDtorFn pFnDtor)     GBL_NOEXCEPT;

GBL_INLINE void        GblClosure_setMarshal    (GBL_SELF,
                                                 GblMarshalFn pFnMarshal)       GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblClosure_setMetaMarshal(GBL_SELF,
                                                 GblMarshalFn pFnMeta)          GBL_NOEXCEPT;

GBL_INLINE GblBool     GblClosure_hasMarshal    (GBL_CSELF)                     GBL_NOEXCEPT;
GBL_INLINE GblBool     GblClosure_hasMetaMarshal(GBL_CSELF)                     GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblClosure_invoke        (GBL_SELF,
                                                 GblVariant* pRetValue,
                                                 GblSize     argCount,
                                                 GblVariant* pArgValues)        GBL_NOEXCEPT;

// ===== IMPL =====

GBL_INLINE GblBool GblClosure_hasMarshal(GBL_CSELF) GBL_NOEXCEPT {
    return GBL_PRIV_REF(pSelf).pFnMarshal? GBL_TRUE : GBL_FALSE;
}

GBL_INLINE GblBool GblClosure_hasMetaMarshal(GBL_CSELF) GBL_NOEXCEPT {
    return GBL_CLOSURE_GET_CLASS(pSelf)->pFnMetaMarshal? GBL_TRUE : GBL_FALSE;
}

GBL_INLINE void GblClosure_setMarshal(GBL_SELF, GblMarshalFn pFnMarshal) GBL_NOEXCEPT {
    GBL_PRIV_REF(pSelf).pFnMarshal = pFnMarshal;
}

GBL_DECLS_END

#undef GBL_CSELF
#undef GBL_SELF

#endif // GIMBAL_CLOSURE_H

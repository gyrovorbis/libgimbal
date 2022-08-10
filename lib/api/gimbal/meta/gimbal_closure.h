#ifndef GIMBAL_CLOSURE_H
#define GIMBAL_CLOSURE_H

#include "gimbal_record.h"
#include "../types/gimbal_marshal.h"

#define GBL_CLOSURE_TYPE                (GblClosure_type())

#define GBL_CLOSURE(instance)           (GBL_INSTANCE_CAST(instance, GBL_CLOSURE_TYPE, GblClosure))
#define GBL_CLOSURE_CLASS(klass)        (GBL_CLASS_CAST(klass, GBL_CLOSURE_TYPE, GblClosureClass))
#define GBL_CLOSURE_GET_CLASS(instance) (GBL_INSTANCE_GET_CLASS(instance, GBL_CLOSURE_TYPE, GblClosureClass))

#define SELF    GblClosure* pSelf
#define CSELF   const SELF

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE(GblClosureClass, GblRecordClass)
    GblMarshalFn    pFnMetaMarshal;
GBL_CLASS_END

GBL_INSTANCE_DERIVE(GblClosure, GblRecord)
    GBL_PRIVATE()
        GblMarshalFn    pFnMarshal;
    GBL_PRIVATE_END
GBL_INSTANCE_END

GBL_EXPORT GblType     GblClosure_type      (void)                    GBL_NOEXCEPT;
GBL_EXPORT GblClosure* GblClosure_current   (void)                    GBL_NOEXCEPT;

GBL_EXPORT void        GblClosure_setMarshal(SELF,
                                             GblMarshalFn pFnMarshal) GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblClosure_invoke    (SELF,
                                             GblVariant* pRetValue,
                                             GblSize     argCount,
                                             GblVariant* pArgValues)  GBL_NOEXCEPT;
GBL_DECLS_END

#undef CSELF
#undef SELF

#endif // GIMBAL_CLOSURE_H

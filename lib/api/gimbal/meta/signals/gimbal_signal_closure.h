#ifndef GIMBAL_SIGNAL_CLOSURE_H
#define GIMBAL_SIGNAL_CLOSURE_H

#include "gimbal_closure.h"

#define GBL_SIGNAL_CLOSURE_TYPE                 (GblSignalClosure_type())

#define GBL_SIGNAL_CLOSURE(instance)            (GBL_INSTANCE_CAST(instance, GBL_SIGNAL_CLOSURE_TYPE, GblSignalClosure))
#define GBL_SIGNAL_CLOSURE_CLASS(klass)         (GBL_CLASS_CAST(klass, GBL_SIGNAL_CLOSURE_TYPE, GblSignalClosureClass))
#define GBL_SIGNAL_CLOSURE_GET_CLASS(instance)  (GBL_INSTANCE_GET_CLASS(instance, GBL_SIGNAL_CLOSURE_TYPE, GblSignalClosureClass))

#define SELF    GblSignalClosure* pSelf
#define CSELF   const SELF

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblSignalClosureClass, GblClosureClass)

GBL_INSTANCE_DERIVE(GblSignalClosure, GblClosure)
    GBL_PRIVATE()
        GblQuark        signalName;
    GBL_PRIVATE_END
GBL_INSTANCE_END

GBL_EXPORT GblType           GblSignalClosure_type     (void)                     GBL_NOEXCEPT;

GBL_EXPORT GblSignalClosure* GblSignalClosure_create   (const char*  pSignalName,
                                                        void*        pUserdata)   GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT        GblSignalClosure_setSignal(SELF,
                                                        const char*  pSignalName) GBL_NOEXCEPT;
GBL_DECLS_END

#undef CSELF
#undef SELF

#endif // GIMBAL_SIGNAL_CLOSURE_H

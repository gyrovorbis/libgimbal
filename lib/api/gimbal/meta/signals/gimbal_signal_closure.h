/*! \file
 *  \brief GblSignalClosure base instance and API
 *  \copydoc GblSignalClosure
 *  \ingroup signals
 *
 *  \author Falco Girgis
 */

#ifndef GIMBAL_SIGNAL_CLOSURE_H
#define GIMBAL_SIGNAL_CLOSURE_H

#include "gimbal_closure.h"

#define GBL_SIGNAL_CLOSURE_TYPE                 (GBL_TYPEOF(GblSignalClosure))

#define GBL_SIGNAL_CLOSURE(instance)            (GBL_INSTANCE_CAST(instance, GblSignalClosure))
#define GBL_SIGNAL_CLOSURE_CLASS(klass)         (GBL_CLASS_CAST(klass, GblSignalClosure))
#define GBL_SIGNAL_CLOSURE_GET_CLASS(instance)  (GBL_INSTANCE_GET_CLASS(instance, GblSignalClosure))

#define GBL_SELF_TYPE GblSignalClosure

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblSignalClosure, GblClosure)

GBL_INSTANCE_DERIVE(GblSignalClosure, GblClosure)
    GBL_PRIVATE()
        GblQuark        signalName;
    GBL_PRIVATE_END
GBL_INSTANCE_END

GBL_EXPORT GblType           GblSignalClosure_type      (void)                     GBL_NOEXCEPT;

GBL_EXPORT GblSignalClosure* GblSignalClosure_create    (const char*  pSignalName,
                                                         void*        pUserdata)   GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT        GblSignalClosure_setSignal (GBL_SELF,
                                                         const char*  pSignalName) GBL_NOEXCEPT;
GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_SIGNAL_CLOSURE_H

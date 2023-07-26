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

#define GBL_SIGNAL_CLOSURE_TYPE             (GBL_TYPEID(GblSignalClosure))

#define GBL_SIGNAL_CLOSURE(self)            (GBL_CAST(GblSignalClosure, self))
#define GBL_SIGNAL_CLOSURE_CLASS(klass)     (GBL_CLASS_CAST(GblSignalClosure, klass))
#define GBL_SIGNAL_CLOSURE_GET_CLASS(self)  (GBL_CLASSOF(GblSignalClosure, self))

#define GBL_SELF_TYPE GblSignalClosure

GBL_DECLS_BEGIN

/*! \struct  GblSignalClosureClass
 *  \extends GblClosureClass
 *  \brief   GblClass type for GblSignalClosure
 *
 *  No public members.
 *
 *  \sa GblSignalClosure
 */
GBL_CLASS_DERIVE_EMPTY(GblSignalClosure, GblClosure)

/*! \struct  GblSignalClosure
 *  \extends GblClosure
 *  \ingroup signals
 *  \brief   Closure type for invoking signals
 *
 *  GblSignalClosure is a closure type which marshals its
 *  arguments into a signal invocation. The first argument
 *  of the invoke function is expected to be the instance
 *  which emits the signal.
 */
GBL_INSTANCE_DERIVE(GblSignalClosure, GblClosure)
    GBL_PRIVATE_BEGIN
        GblQuark signalName;
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

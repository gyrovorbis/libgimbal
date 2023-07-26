/*! \file
 *  \ingroup signals
 *  \brief   GblSignalClosure, signal-emitting closurure, and API
 *
 *  This file contains the GblSignalClosure type and its
 *  associated methods. It is used to forward a generic function
 *  invocation to a signal emission, which can then be received
 *  again by any GblClosure instances connected to the signal.
 *
 *  \author    2023 Falco Girgis
 *  \copyright MIT License
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
        GblQuark signalName; //!< PRIVATE: Name of the signal to forward arguments to
    GBL_PRIVATE_END
GBL_INSTANCE_END

//! Returns the GblType UUID associated with GblSignalClosure
GBL_EXPORT GblType           GblSignalClosure_type   (void)                    GBL_NOEXCEPT;

//! Creates a new GblSignalClosure, with the given \p pSignalName, capturing the given \p pUserdata
GBL_EXPORT GblSignalClosure* GblSignalClosure_create (const char* pSignalName,
                                                      void*       pUserdata)   GBL_NOEXCEPT;

/*! \name  Accessor Method(s)
 *  \brief Setter methods for GblSignalClosure
 *  \relatesalso GblSignalClosure
 *  @{
 */
//! Sets the signal for the closure to invoke to the given \p pSignalName
GBL_EXPORT GBL_RESULT GblSignalClosure_setSignal (GBL_SELF, const char* pSignalName) GBL_NOEXCEPT;
//! @}

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_SIGNAL_CLOSURE_H

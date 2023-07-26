/*! \file
 *  \ingroup signals
 *  \brief   GblCClosure, C function callback closure, and API
 *
 *  This file contains the definition of and API around
 *  GblCClosure, which is the plain C function implementation
 *  of the GblClosure type.
 *
 *  It simply stores the value of a generic C function pointer,
 *  expecting to have either a marshal or meta marshal which
 *  converts arguments into plain C types, casts the callback
 *  to its correct type, invokes it, passing the arguments,
 *  and stores its return value as a GblVariant.
 *
 *  \author     2023 Falco Girgis
 *  \copyright  MIT License
 */
#ifndef GIMBAL_C_CLOSURE_H
#define GIMBAL_C_CLOSURE_H

#include "gimbal_closure.h"

/*! \name Type System
 *  \brief Type UUID and cast operators
 *  @{
 */
#define GBL_C_CLOSURE_TYPE            (GBL_TYPEID(GblCClosure))            //!< Type UUID of GblCClosure
#define GBL_C_CLOSURE(self)           (GBL_CAST(GblCClosure, self))        //!< Function-style GblInstance cast
#define GBL_C_CLOSURE_CLASS(klass)    (GBL_CLASS_CAST(GblCClosure, klass)) //!< Function-style GblClass cast
#define GBL_C_CLOSURE_GET_CLASS(self) (GBL_CLASSOF(GblCClosure, self))     //!< Get GblCClosureClass from GblInstance
//! @}

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
 *  \brief   Closure type for invoking C functions
 *  \extends GblClosure
 *  \ingroup signals
 *
 *  GblCClosure is a GblClosure which is used to
 *  marshal generic arguments into a C function
 *  pointer and marshal out the return value.
 *
 *  \sa GblClosure
 */
GBL_INSTANCE_DERIVE(GblCClosure, GblClosure)
    GBL_PRIVATE_BEGIN
        GblFnPtr    pFnCallback; //!< PRIVATE: C Function pointer to invoke as a callback
    GBL_PRIVATE_END
GBL_INSTANCE_END

//! Returns the GblType UUID for GblCClosure
GBL_EXPORT GblType GblCClosure_type (void) GBL_NOEXCEPT;

/*! \name Methods
 *  \brief General methods for GblCClosure
 *  \relatesalso GblCClosure
 *  @{
 */
//! Creates a GblCClosure instance with the given callback and userdata, returning a pointer to it
GBL_EXPORT GblCClosure* GblCClosure_create      (GblFnPtr pFnCallback,
                                                 void*    pUserdata)   GBL_NOEXCEPT;
//! Sets the C callback function of the given GblCClosure instance
GBL_EXPORT void         GblCClosure_setCallback (GBL_SELF,
                                                 GblFnPtr pFnCallback) GBL_NOEXCEPT;
//! @}

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_C_CLOSURE_H

/*! \file
 *  \brief GblClosure base instance and API
 *  \copydoc GblClosure
 *  \ingroup signals
 *
 *  \author Falco Girgis
 */
#ifndef GIMBAL_CLOSURE_H
#define GIMBAL_CLOSURE_H

#include "../instances/gimbal_box.h"
#include "gimbal_marshal.h"

/*! \name Type System
 *  \brief Type UUID and cast operators
 *  @{
 */
#define GBL_CLOSURE_TYPE                (GBL_TYPEID(GblClosure))
#define GBL_CLOSURE(self)               (GBL_CAST(GblClosure, self))
#define GBL_CLOSURE_CLASS(klass)        (GBL_CLASS_CAST(GblClosure, klass))
#define GBL_CLOSURE_GET_CLASS(self)     (GBL_CLASSOF(GblClosure, self))
//! @}

//! Casts a C function pointer to a generic callback type
#ifndef __cplusplus
#   define GBL_CALLBACK(fn)  ((GblFnPtr)fn)
#else
    #define GBL_CALLBACK(fn) ((GblFnPtr)+fn)
#endif

#define GBL_SELF_TYPE GblClosure

GBL_DECLS_BEGIN

/*! \struct  GblClosureClass
 *  \brief   GblClass VTable for GblClosure
 *  \extends GblBoxClass
 *
 *  Provides an overridable virtual method for marshalling
 *  generic arguments to an actual function and marshalling
 *  its return value into a generic value.
 *
 *  \sa GblBox, GblMarshalFn
 */
GBL_CLASS_DERIVE(GblClosure, GblBox)
    GblMarshalFn pFnMetaMarshal; //!< Primary entry point when marshalling a function call
GBL_CLASS_END

/*! \struct GblClosure
 *  \brief Base instance for all closure types
 *  \ingroup signals
 *  \extends GblBox
 *
 * A GblClosure represents an abstract  generic
 * callback object. It is analogous to a stateful C++
 * function object, or functor, in that it can
 * contain both a callback method as  well as stateful
 * data to operate upon.
 *
 * All closures have a single entry-point for
 * having their callback logic called, which is
 * GblClosure_invoke(). This calls the closure's
 * "marshal" function, which is responsible for
 * - validating arguments and return type
 * - converting arguments into expected format for callback
 * - calling actual callback or logic
 * - converting return value back into a GblVariant
 *
 * As the abstract base closure, there is no underlying
 * language-specific callback state within this class.
 * For calling back into C function pointers, see
 * GblCClosure.
 *
 * \note As GblClosure inherits GblBox, it can
 * contain arbitrary language-specific or
 * binding-specific userdata and has reference
 * count semantics.
 *
 * \sa GblCClosure, GblMarshalFn, gimbal_signal.h
 */
GBL_INSTANCE_DERIVE(GblClosure, GblBox)
    GBL_PRIVATE_BEGIN
        GblMarshalFn pFnMarshal; //!< Per-instance marshal function, private
    GBL_PRIVATE_END
GBL_INSTANCE_END

//! Returns the GblType UUID for GblClosure
GBL_EXPORT GblType GblClosure_type (void) GBL_NOEXCEPT;

/*! \name Current Closure
 *  \brief Methods for querying the active GblClosure
 *  @{
 */
//! Returns a pointer to the inner-most currently executing GblClosure instance
GBL_EXPORT GblClosure* GblClosure_current         (void) GBL_NOEXCEPT;
//! Returns a pointer to the userdatea of the inner-most currently executing GblClosure instance
GBL_EXPORT void*       GblClosure_currentUserdata (void) GBL_NOEXCEPT;
//! @}

//! Creates a GblClosure-derived instance with the given attributes, returning a pointer to it
GBL_EXPORT GblClosure* GblClosure_create (GblType           derivedType,
                                          size_t            size,
                                          void*             pUserdata,
                                          GblArrayMapDtorFn pFnDtor) GBL_NOEXCEPT;

/*! \name Accessor Methods
 *  \brief Methods for reading/writing GblClosure fields
 *  \relatesalso GblClosure
 *  @{
 */
//! Sets the closure's marshal to the function pointed to by \p pFnMarshal
GBL_EXPORT void       GblClosure_setMarshal     (GBL_SELF,
                                                 GblMarshalFn pFnMarshal) GBL_NOEXCEPT;
//! Sets the closure's class's meta marshal to the function pointed to by \p pFnMeta, swizzling its class
GBL_EXPORT GBL_RESULT GblClosure_setMetaMarshal (GBL_SELF,
                                                 GblMarshalFn pFnMeta)    GBL_NOEXCEPT;
//! Returns whether the GblClosure has an instance marshal or not
GBL_EXPORT GblBool    GblClosure_hasMarshal     (GBL_CSELF)               GBL_NOEXCEPT;
//! Returns whether the GblClosure's class has a meta marshal or not
GBL_EXPORT GblBool    GblClosure_hasMetaMarshal (GBL_CSELF)               GBL_NOEXCEPT;
//! @}

/*! \name Invocation Method(s)
 *  \brief Methods used to invoke the closure
 *  \relatesalso GblClosure
 */
//! Invokes the given GblClosure, marshalling args in and a value out as variants
GBL_EXPORT GBL_RESULT GblClosure_invoke (GBL_SELF,
                                         GblVariant* pRetValue,
                                         size_t      argCount,
                                         GblVariant* pArgValues) GBL_NOEXCEPT;
//! @}

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_CLOSURE_H

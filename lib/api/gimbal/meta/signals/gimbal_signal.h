/*! \file
 *  \brief   Signals, connections, and related API.
 *  \ingroup signals
 *  \sa      signals
 *
 *  This file contains the routines used for creating, managing, and
 *  emitting signals. For detailed information on how to use signals,
 *  refer to the \ref signals overview.
 *
 *  \author    2023, 2025 Falco Girgis
 *  \copyright MIT License
 *
 *  \todo
 *      - GBL_SIGNALS() DSL
 *      - GblSignal_next() for iteration
 *      - GblSignal_uninstallAll(GblType type)
 *      - thread-safety
 */
#ifndef GIMBAL_SIGNAL_H
#define GIMBAL_SIGNAL_H

#include "../types/gimbal_type.h"
#include "gimbal_marshal.h"

/*! \name  DSL Macros
 *  \brief Helper macros for declaring and managing signals.
 *  @{
 */
//! Declares a list of signals to be associated with the given instanceStruct.
#define GBL_SIGNALS(instanceStruct, /* signals */...)
//! Registers the list of signals which has been associated with the given instanceStruct.
#define GBL_SIGNALS_REGISTER(instanceStruct, /* marshals */...)
//! Emits a signal from the given emitter with the given name and arguments.
#define GBL_EMIT(emitter, /* name, */...)      (GblSignal_emit(GBL_INSTANCE(emitter), __VA_ARGS__))
//! Connects the signal with the given name from the given emitter to a receiver with the given callback function and optional userdata.
#define GBL_CONNECT(/* emitter, name*/...)     (GBL_VA_OVERLOAD_CALL_ARGC(GBL_CONNECT, __VA_ARGS__))
//! @}

/*! \defgroup signals Signals
 *	\brief    Signal emission mechanism and its types and API.
 *  \ingroup  meta
 *
 *  # Overview
 *  libGimbal's signal system implements a powerful cross-language
 *  event callback mechanism which decouples event emitting types
 *  from those types which receive or listen for events. It is based
 *  upon both GTk's "signals" and Qt's "signals and slots" and is
 *  expecially powerful for GUI/widget programming.
 *
 *  # Declaration
 *  It is a good practice to use DSL macros to predeclare any signals
 *  which are associated with a GblType as part of their public API
 *  declaration within their respective header file. As an example,
 *  the signals associated with the GblOptionGroup are declared by:
 *
 *  \code{.c}
 *      GBL_SIGNALS(GblOptionGroup,
 *          (parsePrePass,  (GBL_POINTER_TYPE, stringList),
 *          (parsePostPass, (GBL_POINTER_TYPE, stringList),
 *          (parseError,    (GBL_ENUM_TYPE,    errorCode)
 *      )
 *  \endcode
 *
 *  This declares GblOptionGroup as being able to emit 3 different signals,
 *  with `parsePrePass` and `parsePostPass` sending pointers to GblStringList
 *  as their parameters, and `parseError` passing along an error code as a
 *  GblEnum as its parameter.
 *
 *  # Registration
 *  The first thing that must happen before signals can be used is that they
 *  get registered at runtime with an actual type. This is typically done
 *  within the GblTypeInfo::pFnClassInit routine for a given type, only when
 *  its reference count is 0, meaning it is being instantiated for the first
 *  time:
 *
 *  \code{.c}
 *      static GBL_RESULT GblOptionGroupClass_init_(GblClass* pClass, const void* pData) {
 *          if(!GblType_classRefCount(GBL_OPTION_GROUP_TYPE))
 *              GBL_SIGNALS_REGISTER(GblOptionGroup);
 *          // ...
 *      }
 *  \endcode
 *
 *  # Basic Usage
 *  In order to get notified when a signal gets fired from a particular
 *  GblInstance subtype, we must first connect a closure to it, so that it
 *  can be invoked later when the signal fires. The simplest way is to use a
 *  C function pointer as the callback:
 *
 *  \code{.c}
 *      // C callback to be called when a GblOptionGroupClass::parseError signal is fired.
 *      static void GblOptionGroup_onParseError_(GblInstance* pReciever, GblEnum error) {
 *          // Retrieve our emission counter from the associated closure's userdata pointer.
 *          size_t* pEmittedCounter = (size_t*)GblClosure_currentUserdata();
 *          // Increment our counter.
 *          *pEmittedCounter++;
 *
 *          // Retrieve a pointer to the instance that emitted the current signal.
 *          GblInstance* pEmitter = GblSignal_emitter();
 *
 *          GBL_LOG_ERROR("signal_test", "%s emitted parseError %u to %s!",
 *                        GblObject_name(GBL_OBJECT(pEmitter)),
 *                        error,
 *                        GblObject_name(GBL_OBJECT(pReceiver)));
 *      }
 *
 *      int main(int argc, const char* argv[]) {
 *          // Create the emitting object.
 *          GblOptionGroup* pSender = GBL_NEW(GblOptionGroupClass, "name", "emitter");
 *          // Create the receiving object.
 *          GblObject* pReceiver = GBL_NEW(GblObject, "name", "recipient");
 *          // Counter for number of times signal was emitted
 *          size_t emittedCounter = 0;
 *
 *          // Connect the receiver's callback to the emitter's signal, plus give it a counter pointer.
 *          GBL_CONNECT(pSender,
 *                      "parseError",
 *                      pReceiver,
 *                      GblOptionGroup_onParseError_,
 *                      &emittedCounter);
 *
 *          // Manually fire the signal ourselves (usually done internally)
 *          GBL_EMIT(pSender, "parseError", GBL_RESULT_ERROR_INVALID_TYPE);
 *
 *          // Counter should've been incremented from the signal handler!
 *          GBL_ASSERT(emittedCounter == 1);
 *          return 0;
 *      }
 *  \endcode
 */

GBL_DECLS_BEGIN

/*! \name  Installing
 *  \brief Methods for installing and uninstalling signals from a GblType.
 *  @{
 */
//! Installs a named signal onto the given type, taking a number of arguments and an associated list of GblTypes representing each, along with a marshal which can handle forwarding such an invocation to a C callback function.
GBL_EXPORT GBL_RESULT GblSignal_install      (GblType      instanceType,
                                              const char*  pName,
                                              GblMarshalFn pFnCMarshal,
                                              size_t       argCount,
                                              /*GblType,*/ ...)          GBL_NOEXCEPT;
//! Uninstalls the signal with the given name which was previously installed onto the given \p instanceType.
GBL_EXPORT GBL_RESULT GblSignal_uninstall    (GblType      instanceType,
                                              const char*  pName)        GBL_NOEXCEPT;
//! Uninstalls all signals which have been installed onto the associated \p instanceType.
GBL_EXPORT GBL_RESULT GblSignal_uninstallAll (GblType      instanceType) GBL_NOEXCEPT;
//! @}

/*! \name  Connecting
 *  \brief Methods for connecting/disconnecting signals and closures.
 *  @{
 */
//! Connects the given C callback function, \p pFnCCallback, to the given \p pSignalName, which is emitted from the given \p pEmitter. \p pUserdata gets added to the associated closure and can be retrieved from within the callback with GblClosure_currentUserdata().
GBL_EXPORT GBL_RESULT GblSignal_connect        (GblInstance* pEmitter,
                                                const char*  pSignalName,
                                                GblInstance* pReceiver,
                                                GblFnPtr     pFnCCallback,
                                                void*        pUserdata)      GBL_NOEXCEPT;
//! Forwards the signal, \p pSignalName on \p pEmitter, to the virtual function at \p methodOffset located on the GblClass structure on \p classType.
GBL_EXPORT GBL_RESULT GblSignal_connectClass   (GblInstance* pEmitter,
                                                const char*  pSignalName,
                                                GblInstance* pReceiver,
                                                GblType      classType,
                                                size_t       methodOffset)   GBL_NOEXCEPT;
//! Creates a forwarding signal by reemitting the \p pSignalName coming from \p pEmitter as \p pDstSignalName coming from \p pDstEmitter.
GBL_EXPORT GBL_RESULT GblSignal_connectSignal  (GblInstance* pEmitter,
                                                const char*  pSignalName,
                                                GblInstance* pDstEmitter,
                                                const char*  pDstSignalName) GBL_NOEXCEPT;
//! Connects the generic closure instance, \p GblClosure, on the given \p pReceiver, from the given \p pSignalName on the given \p pemitter.
GBL_EXPORT GBL_RESULT GblSignal_connectClosure (GblInstance* pEmitter,
                                                const char*  pSignalName,
                                                GblInstance* pReceiver,
                                                GblClosure*  pClosure)       GBL_NOEXCEPT;
//! Disconnects the given \p pClosure on the given \p pReceiver from the given \p pSignalName on the given \p pEmitter, returning the number of closure which were uninstalled.
GBL_EXPORT size_t     GblSignal_disconnect     (GblInstance* pEmitter,
                                                const char*  pSignalName,
                                                GblInstance* pReceiver,
                                                GblClosure*  pClosure)       GBL_NOEXCEPT;
//! @}

/*! \name  Blocking
 *  \brief Routines for temporarily disabling and enabling signal emission.
 *  @{
 */
//! Blocks \p pSignalName from being emitted from \p pInstance if \p blocked is `true`, otherwise enables its emission.
GBL_EXPORT GblBool GblSignal_block    (GblInstance* pInstance,
                                       const char*  pSignalName,
                                       GblBool      blocked)     GBL_NOEXCEPT;
//! Blocks all signals from being emitted from \p pinstance if \p block is `true`, otherwise enables them.
GBL_EXPORT GblBool GblSignal_blockAll (GblInstance* pInstance,
                                       GblBool      blocked)     GBL_NOEXCEPT;
//! Returns true if the signal is blocked on the given instance or if all signals are blocked, if no signal name is provided.
GBL_EXPORT GblBool GblSignal_blocked  (GblInstance* pInstance,
                                       const char*  pSignalName) GBL_NOEXCEPT;
//! @}


/*! \name  Emitting
 *  \brief Routines for firing signals to associated closures.
 *  @{
 */
//! Fires \p pSignalName from \p pEmitter, with the given variadic argument list.
GBL_EXPORT GBL_RESULT GblSignal_emit         (GblInstance* pEmitter,
                                              const char*  pSignalName,
                                              ...)                      GBL_NOEXCEPT;
//! Equivalent to GblSignal_emit(), except taking arguments as a va_list* rather than variadic args.
GBL_EXPORT GBL_RESULT GblSignal_emitVaList   (GblInstance* pEmitter,
                                              const char*  pSignalName,
                                              va_list*     pVarArgs)    GBL_NOEXCEPT;
//! Equivalent to GblSignal_emit(), except taking arguments as an array of GblVariants rather than variadic args.
GBL_EXPORT GBL_RESULT GblSignal_emitVariants (GblInstance* pEmitter,
                                              const char*  pSignalName,
                                              GblVariant*  pArgs)       GBL_NOEXCEPT;
//! @}

/*! \name  Querying
 *  \brief Routines for retrieving signal state information.
 *   @{
 */
//! Returns the number of connections which have been registered for \p pSignalName on \p pInstance.
GBL_EXPORT size_t       GblSignal_connectionCount (GblInstance* pInstance,
                                                   const char*  pSignalName) GBL_NOEXCEPT;
//! Returns a  pointer to the GblInstance which emitted the acive signal or `NULL` if there isn't an active one.
GBL_EXPORT GblInstance* GblSignal_emitter         (void)                     GBL_NOEXCEPT;
//! Returns a pointer to the GblInstance which received the active signal or `NULL` if there isn't an active one.
GBL_EXPORT GblInstance* GblSignal_receiver        (void)                     GBL_NOEXCEPT;
//! @}

#define                 GblSignal_connect(...)    GBL_VA_OVERLOAD_CALL_ARGC(GblSignal_connect, __VA_ARGS__)

// ===== IMPLEMENTATION =====

///\cond
#define GblSignal_connect_3(emitter, signal, callback) \
    (GblSignal_connect_4(emitter, signal, emitter, callback))
#define GblSignal_connect_4(emitter, signal, receiver, callback) \
    (GblSignal_connect_5(emitter, signal, receiver, callback, GBL_NULL))
#define GblSignal_connect_5(emitter, signal, receiver, callback, userdata) \
    ((GblSignal_connect)(emitter, signal, receiver, callback, userdata))

#define GBL_CONNECT_3(emitter, signal, callback) \
    (GBL_CONNECT_4(emitter, signal, emitter, callback))
#define GBL_CONNECT_4(emitter, signal, receiver, callback) \
    (GBL_CONNECT_5(emitter, signal, receiver, callback, GBL_NULL))
#define GBL_CONNECT_5(emitter, signal, receiver, callback, userdata) \
    ((GblSignal_connect)(GBL_INSTANCE(emitter), signal, GBL_INSTANCE(receiver), GBL_CALLBACK(callback), (void*)userdata))

#define GBL_SIGNALS_(instance, ...) \
    GBL_INLINE GBL_RESULT instance##_registerSignals_(instance* pSelf, GblMarshalFn* pMarshals) { \
        GBL_UNUSED(pSelf, pMarshals); \
        GBL_CTX_BEGIN(NULL); \
        GBL_TUPLE_FOREACH(GBL_SIGNAL_INSTALL_, instance, (__VA_ARGS__)) \
        GBL_CTX_END(); \
    }


#define GBL_SIGNAL_INSTALL_(instance, signal)

#if 0
GBL_CTX_VERIFY_CALL(GblSignal_install(GBL_TYPEID(instance), \
                                          GBL_STRINGIFY(GBL_TUPLE_FIRST signal), \
                                          pMarshals++, \
                                          GBL_NARG signal - 1, \
                                          GBL_TUPLE_FOREACH(GBL_SIGNAL_ARG_TYPE_, instance, GBL_TUPLE_REST(signal) )));
#endif

#define GBL_SIGNAL_ARG_TYPE_(instance, pair) \
    GBL_TUPLE_FIRST(pair),

#define GBL_SIGNALS_REGISTER_(instance, marshals) \
    instance##_registerSignals_(instance, marshals)
///\endcond

GBL_DECLS_END

#endif // GIMBAL_SIGNAL_H

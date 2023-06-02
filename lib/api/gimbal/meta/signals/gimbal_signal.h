/*! \file
 *  \brief Signals, connections, and related API
 *  \ingroup signals
 *  \todo
 *      - GblSignal_next() for iteration
 *
 *  \author Falco Girgis
 */

#ifndef GIMBAL_SIGNAL_H
#define GIMBAL_SIGNAL_H

#include "../types/gimbal_type.h"
#include "../../core/gimbal_typedefs.h"
#include "../../preprocessor/gimbal_macro_composition.h"
#include "gimbal_closure.h"

#define GBL_SIGNALS(instanceStruct, /* signals */...)               GBL_SIGNALS_(instanceStruct, __VA_ARGS__)
#define GBL_SIGNALS_REGISTER(instanceStruct, /* marshals */...)
#define GBL_EMIT(emitter, ...)                                      (GblSignal_emit(GBL_INSTANCE(emitter), __VA_ARGS__))
#define GBL_CONNECT(...)                                            (GBL_VA_OVERLOAD_CALL_ARGC(GBL_CONNECT, __VA_ARGS__))

GBL_DECLS_BEGIN

GBL_EXPORT GBL_RESULT   GblSignal_install         (GblType        ownerType,
                                                   const char*    pName,
                                                   GblMarshalFn   pFnCMarshal,
                                                   size_t         argCount,
                                                   ...)                           GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblSignal_uninstall       (GblType        ownerType,
                                                   const char*    pName)          GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblSignal_connect         (GblInstance*   pEmitter,
                                                   const char*    pSignalName,
                                                   GblInstance*   pReceiver,
                                                   GblFnPtr       pFnCCallback,
                                                   void*          pUserdata)      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblSignal_connectClass    (GblInstance*   pEmitter,
                                                   const char*    pSignalName,
                                                   GblInstance*   pReceiver,
                                                   GblType        classType,
                                                   size_t         methodOffset)   GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblSignal_connectSignal   (GblInstance*   pEmitter,
                                                   const char*    pSignalName,
                                                   GblInstance*   pDstEmitter,
                                                   const char*    pDstSignalName) GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblSignal_connectClosure  (GblInstance*   pEmitter,
                                                   const char*    pSignalName,
                                                   GblInstance*   pReceiver,
                                                   GblClosure*    pClosure)       GBL_NOEXCEPT;

GBL_EXPORT size_t       GblSignal_disconnect      (GblInstance*   pEmitter,
                                                   const char*    pSignalName,
                                                   GblInstance*   pReceiver,
                                                   GblClosure*    pClosure)       GBL_NOEXCEPT;

GBL_EXPORT GblBool      GblSignal_block           (GblInstance*   pInstance,
                                                   const char*    pSignalName,
                                                   GblBool        blocked)        GBL_NOEXCEPT;

GBL_EXPORT GblBool      GblSignal_blockAll        (GblInstance*   pInstance,
                                                   GblBool        blocked)        GBL_NOEXCEPT;

GBL_EXPORT size_t       GblSignal_connectionCount (GblInstance*   pInstance,
                                                   const char*    pSignalName)    GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblSignal_emit            (GblInstance*   pEmitter,
                                                   const char*    pSignalName,
                                                   ...)                           GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblSignal_emitVaList      (GblInstance*   pEmitter,
                                                   const char*    pSignalName,
                                                   va_list*       pVarArgs)       GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblSignal_emitVariants    (GblInstance*   pEmitter,
                                                   const char*    pSignalName,
                                                   GblVariant*    pArgs)          GBL_NOEXCEPT;

GBL_EXPORT GblInstance* GblSignal_emitter         (void)                          GBL_NOEXCEPT;

GBL_EXPORT GblInstance* GblSignal_receiver        (void)                          GBL_NOEXCEPT;

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
GBL_CTX_VERIFY_CALL(GblSignal_install(GBL_TYPEOF(instance), \
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

/*! \todo GblSignal_next() for iterating over signals for a type
 *       - will allow for auto signal cleanup upon type registration
 */
/*! \file
 *  \brief Signals, connections, and related API
 *  \ingroup signals
 */

#ifndef GIMBAL_SIGNAL_H
#define GIMBAL_SIGNAL_H

#include "../types/gimbal_type.h"
#include "../../core/gimbal_typedefs.h"
#include "../../preprocessor/gimbal_macro_composition.h"
#include "gimbal_closure.h"

#define GBL_SIGNALS(instanceStruct, /* signals */...) GBL_SIGNALS_(instanceStruct, __VA_ARGS__)
#define GBL_SIGNALS_REGISTER(instanceStruct, /* marshals */...)

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
                                                   GblFnPtr       pFnCCallback)   GBL_NOEXCEPT;

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

// ===== IMPLEMENTATION =====

///\cond

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

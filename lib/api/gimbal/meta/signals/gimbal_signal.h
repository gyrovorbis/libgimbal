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
#include "gimbal_closure.h"

GBL_DECLS_BEGIN

GBL_EXPORT GBL_RESULT   GblSignal_install         (GblType        ownerType,
                                                   const char*    pName,
                                                   GblMarshalFn   pFnCMarshal,
                                                   GblSize        argCount,
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
                                                   GblSize        methodOffset)   GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblSignal_connectSignal   (GblInstance*   pEmitter,
                                                   const char*    pSignalName,
                                                   GblInstance*   pDstEmitter,
                                                   const char*    pDstSignalName) GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblSignal_connectClosure  (GblInstance*   pEmitter,
                                                   const char*    pSignalName,
                                                   GblInstance*   pReceiver,
                                                   GblClosure*    pClosure)       GBL_NOEXCEPT;

GBL_EXPORT GblSize      GblSignal_disconnect      (GblInstance*   pEmitter,
                                                   const char*    pSignalName,
                                                   GblInstance*   pReceiver,
                                                   GblClosure*    pClosure)       GBL_NOEXCEPT;

GBL_EXPORT GblBool      GblSignal_block           (GblInstance*   pInstance,
                                                   const char*    pSignalName,
                                                   GblBool        blocked)        GBL_NOEXCEPT;

GBL_EXPORT GblBool      GblSignal_blockAll        (GblInstance*   pInstance,
                                                   GblBool        blocked)        GBL_NOEXCEPT;

GBL_EXPORT GblSize      GblSignal_connectionCount (GblInstance*   pInstance,
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

GBL_DECLS_END

#endif // GIMBAL_SIGNAL_H

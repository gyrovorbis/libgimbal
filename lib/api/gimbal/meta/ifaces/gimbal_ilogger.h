/*! \file
 *  \brief GblILogger abstract logger interface
 *  \ingroup interfaces
 */


#ifndef GIMBAL_ILOGGER_H
#define GIMBAL_ILOGGER_H

#include "gimbal_interface.h"
#include "../../core/gimbal_api_frame.h"

#define GBL_ILOGGER_TYPE                (GblILogger_type())
#define GBL_ILOGGER_STRUCT              GblILogger
#define GBL_ILOGGER_CLASS_STRUCT        GblILoggerClass

#define GBL_ILOGGER(inst)               (GBL_INSTANCE_CAST_PREFIX       (inst,  GBL_ILOGGER))
#define GBL_ILOGGER_CHECK(inst)         (GBL_INSTANCE_CHECK_PREFIX      (inst,  GBL_ILOGGER))
#define GBL_ILOGGER_TRY(inst)           (GBL_INSTANCE_TRY_PREFIX        (inst,  GBL_ILOGGER))

#define GBL_ILOGGER_IFACE(klass)        (GBL_CLASS_CAST_PREFIX          (klass, GBL_ILOGGER))
#define GBL_ILOGGER_IFACE_CHECK(klass)  (GBL_CLASS_CHECK_PREFIX         (klass, GBL_ILOGGER))
#define GBL_ILOGGER_IFACE_TRY(klass)    (GBL_CLASS_TRY_PREFIX           (klass, GBL_ILOGGER))

#define GBL_ILOGGER_GET_IFACE(inst)     (GBL_INSTANCE_GET_CLASS_PREFIX (inst,  GBL_ILOGGER))
#define GBL_ILOGGER_TRY_IFACE(inst)     (GBL_INSTANCE_TRY_CLASS_PREFIX  (inst,  GBL_ILOGGER))

#define GBL_SELF    GblILogger* pSelf
#define GBL_CSELF   const GBL_SELF

GBL_DECLS_BEGIN

GBL_INTERFACE_DERIVE(GblILogger)
    GBL_RESULT (*pFnWrite)  (GBL_SELF, const GblStackFrame* pFrame, GBL_LOG_LEVEL level, const char* pFmt, va_list varArgs);
    GBL_RESULT (*pFnPush)   (GBL_SELF, const GblStackFrame* pFrame);
    GBL_RESULT (*pFnPop)    (GBL_SELF, const GblStackFrame* pFrame, uint32_t count);
GBL_INTERFACE_END

GBL_EXPORT GblType GblILogger_type(void)                    GBL_NOEXCEPT;

GBL_API GblILogger_write(GBL_SELF,
                         const GblStackFrame*   pFrame,
                         GBL_LOG_LEVEL          level,
                         const char*            pFmt,
                         va_list                varArgs)    GBL_NOEXCEPT;

GBL_API GblILogger_push (GBL_SELF,
                         const GblStackFrame* pFrame)       GBL_NOEXCEPT;

GBL_API GblILogger_pop  (GBL_SELF,
                         const GblStackFrame* pFrame,
                          uint32_t            count)        GBL_NOEXCEPT;



GBL_DECLS_END

#undef GBL_CSELF
#undef GBL_SELF




#endif // GIMBAL_ILOGGER_H

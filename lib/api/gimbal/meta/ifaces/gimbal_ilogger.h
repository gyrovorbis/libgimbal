/*! \file
 *  \brief GblILogger abstract logger interface
 *  \ingroup interfaces
 */


#ifndef GIMBAL_ILOGGER_H
#define GIMBAL_ILOGGER_H

#include "gimbal_interface.h"
#include "../../core/gimbal_ctx.h"

#define GBL_ILOGGER_TYPE                (GBL_TYPEOF(GblILogger))
#define GBL_ILOGGER(instance)           (GBL_INSTANCE_CAST(instance, GblILogger))
#define GBL_ILOGGER_CLASS(klass)        (GBL_CLASS_CAST(klass, GblILogger))
#define GBL_ILOGGER_GET_IFACE(instance) (GBL_INSTANCE_GET_CLASS(instamce, GblILogger))

#define GBL_SELF_TYPE GblILogger

GBL_DECLS_BEGIN

GBL_INTERFACE_DERIVE(GblILogger)
    GBL_RESULT (*pFnWrite)(GBL_SELF, const GblStackFrame* pFrame, GBL_LOG_LEVEL level, const char* pFmt, va_list varArgs);
    GBL_RESULT (*pFnPush) (GBL_SELF, const GblStackFrame* pFrame);
    GBL_RESULT (*pFnPop)  (GBL_SELF, const GblStackFrame* pFrame, uint32_t count);
GBL_INTERFACE_END

GBL_EXPORT GblType    GblILogger_type  (void)                           GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblILogger_write (GBL_SELF,
                                        const GblStackFrame*   pFrame,
                                        GBL_LOG_LEVEL          level,
                                        const char*            pFmt,
                                        va_list                varArgs) GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblILogger_push  (GBL_SELF,
                                        const GblStackFrame*   pFrame)  GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblILogger_pop   (GBL_SELF,
                                        const GblStackFrame*   pFrame,
                                        uint32_t               count)   GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE




#endif // GIMBAL_ILOGGER_H

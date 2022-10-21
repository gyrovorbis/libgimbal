/*! \file
 *  \brief GblILogger abstract logger interface
 *  \ingroup interfaces
 */


#ifndef GIMBAL_ILOGGER_H
#define GIMBAL_ILOGGER_H

#include "gimbal_interface.h"
#include <stdarg.h>

#define GBL_ILOGGER_TYPE                (GBL_TYPEOF(GblILogger))
#define GBL_ILOGGER(instance)           (GBL_INSTANCE_CAST(instance, GblILogger))
#define GBL_ILOGGER_CLASS(klass)        (GBL_CLASS_CAST(klass, GblILogger))
#define GBL_ILOGGER_GET_IFACE(instance) (GBL_INSTANCE_GET_CLASS(instamce, GblILogger))

#define GBL_SELF_TYPE GblILogger

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblStackFrame);

#define GBL_META_LOG_LEVEL_TABLE (                                                      \
        ( GBL_LOG_LEVEL, LogLevel, "Log Message Severity Level", gblLogLevelString),    \
        (                                                                               \
            (GBL_LOG_LEVEL_DEBUG,       0x1,    Debug,      "Debug"),                   \
            (GBL_LOG_LEVEL_VERBOSE,     0x2,    Verbose,    "Verbose"),                 \
            (GBL_LOG_LEVEL_INFO,        0x4,    Info,       "Info"),                    \
            (GBL_LOG_LEVEL_WARNING,     0x8,    Warning,    "Warning"),                 \
            (GBL_LOG_LEVEL_ERROR,       0x10,   Error,      "Error"),                   \
            (GBL_LOG_LEVEL_COUNT,       0x5,    Count,      "# of Levels")              \
        )                                                                               \
    )

GBL_ENUM_TABLE_DECLARE(GBL_META_LOG_LEVEL_TABLE);

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

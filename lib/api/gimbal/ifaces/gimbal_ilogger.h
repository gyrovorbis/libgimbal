#ifndef GIMBAL_ILOGGER_H
#define GIMBAL_ILOGGER_H

#include "gimbal_icomponent.h"

#define GBL_META_LOG_LEVEL_TABLE (                                                          \
        ( GBL_LOG_LEVEL, LogLevel, "Log Message Severity Level", gblLogLevelString),        \
        (                                                                                   \
            (GBL_LOG_LEVEL_DEBUG,       0x0,    Debug,      "Debug"),                       \
            (GBL_LOG_LEVEL_VERBOSE,     0x1,    Verbose,    "Verbose"),                     \
            (GBL_LOG_LEVEL_INFO,        0x2,    Info,       "Info"),                        \
            (GBL_LOG_LEVEL_WARNING,     0x3,    Warning,    "Warning"),                     \
            (GBL_LOG_LEVEL_ERROR,       0x4,    Error,      "Error"),                       \
            (GBL_LOG_LEVEL_COUNT,       0x5,    Count,      "# of Levels")                  \
        )                                                                                   \
    )

GBL_ENUM_TABLE_DECLARE(GBL_META_LOG_LEVEL_TABLE);

// MAKE #defines FOR THIS SHIT
typedef struct GblLogDomain {
    const char* pModule;
} GbLogDomain;

#define ILogger_IFACE                                                                                                     \
    vfunc(GBL_RESULT, write, VSelf, const GblStackFrame* pFrame, GBL_LOG_LEVEL level, const char* pFmt, va_list varArgs)  \
    vfunc(GBL_RESULT, push,  VSelf, const GblStackFrame* pFrame)                                                          \
    vfunc(GBL_RESULT, pop,   VSelf, const GblStackFrame* pFrame, uint32_t count)

#define ILogger_EXTENDS (IComponent)

interface(ILogger);

#endif // GIMBAL_ILOGGER_H

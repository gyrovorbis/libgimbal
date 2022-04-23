#ifndef GIMBAL_ILOGGER_H
#define GIMBAL_ILOGGER_H


#include "../meta/gimbal_interface.h"
#include "../core/gimbal_api_frame.h"

#define GBL_ILOGGER_TYPE                GBL_TYPE_ILOGGER
#define GBL_ILOGGER_STRUCT              GblILogger
#define GBL_ILOGGER_CLASS_STRUCT        GblILoggerIFace
#define GBL_ILOGGER(inst)               (GBL_TYPE_CAST_INSTANCE_PREFIX  (inst,  GBL_ILOGGER))
#define GBL_ILOGGER_CHECK(inst)         (GBL_TYPE_CHECK_INSTANCE_PREFIX (inst,  GBL_ILOGGER))
#define GBL_ILOGGER_IFACE(klass)        (GBL_TYPE_CAST_CLASS_PREFIX     (klass, GBL_ILOGGER))
#define GBL_ILOGGER_IFACE_CHECK(klass)  (GBL_TYPE_CHECK_CLASS_PREFIX    (klass, GBL_ILOGGER))
#define GBL_ILOGGER_GET_IFACE(inst)     (GBL_TYPE_CAST_GET_CLASS_PREFIX (inst,  GBL_ILOGGER))

#define SELF    GblILogger* pSelf
#define CSELF   const SELF

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblILogger);


typedef struct GblILoggerIFace {
    GblInterface base;
    GBL_RESULT (*pFnWrite)  (SELF, const GblStackFrame* pFrame, GBL_LOG_LEVEL level, const char* pFmt, va_list varArgs);
    GBL_RESULT (*pFnPush)   (SELF, const GblStackFrame* pFrame);
    GBL_RESULT (*pFnPop)    (SELF, const GblStackFrame* pFrame, uint32_t count);
} GblILoggerIFace;


GBL_API GblILogger_write(SELF,
                         const GblStackFrame*   pFrame,
                         GBL_LOG_LEVEL          level,
                         const char*            pFmt,
                         va_list                varArgs)    GBL_NOEXCEPT;

GBL_API GblILogger_push(SELF,
                        const GblStackFrame* pFrame)        GBL_NOEXCEPT;

GBL_API GblILogger_pop(SELF,
                       const GblStackFrame* pFrame,
                       uint32_t             count)          GBL_NOEXCEPT;



GBL_DECLS_END

#undef CSELF
#undef SELF




#endif // GIMBAL_ILOGGER_H

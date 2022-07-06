#ifndef GIMBAL_ILOGGER_H
#define GIMBAL_ILOGGER_H


#include "../meta/gimbal_interface.h"
#include "../core/gimbal_api_frame.h"

#define GBL_ILOGGER_TYPE                (GBL_BUILTIN_TYPE(ILOGGER))
#define GBL_ILOGGER_STRUCT              GblILogger
#define GBL_ILOGGER_CLASS_STRUCT        GblILoggerIFace

#define GBL_ILOGGER(inst)               (GBL_INSTANCE_CAST_PREFIX       (inst,  GBL_ILOGGER))
#define GBL_ILOGGER_CHECK(inst)         (GBL_INSTANCE_CHECK_PREFIX      (inst,  GBL_ILOGGER))
#define GBL_ILOGGER_TRY(inst)           (GBL_INSTANCE_TRY_PREFIX        (inst,  GBL_ILOGGER))

#define GBL_ILOGGER_IFACE(klass)        (GBL_CLASS_CAST_PREFIX          (klass, GBL_ILOGGER))
#define GBL_ILOGGER_IFACE_CHECK(klass)  (GBL_CLASS_CHECK_PREFIX         (klass, GBL_ILOGGER))
#define GBL_ILOGGER_IFACE_TRY(klass)    (GBL_CLASS_TRY_PREFIX           (klass, GBL_ILOGGER))

#define GBL_ILOGGER_GET_IFACE(inst)     (GBL_INSTANCE_CAST_CLASS_PREFIX (inst,  GBL_ILOGGER))
#define GBL_ILOGGER_TRY_IFACE(inst)     (GBL_INSTANCE_TRY_CLASS_PREFIX  (inst,  GBL_ILOGGER))

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

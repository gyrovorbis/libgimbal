#include <gimbal/meta/ifaces/gimbal_ilogger.h>
#include <gimbal/meta/instances/gimbal_instance.h>
#include <gimbal/strings/gimbal_quark.h>
#include "../types/gimbal_type_.h"

static GBL_RESULT GblILoggerIFace_pop_(GblILogger* pILogger, const GblStackFrame* pFrame, uint32_t count) GBL_NOEXCEPT {
    GBL_UNUSED(pILogger, pFrame, count);
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GblILoggerIFace_push_(GblILogger* pILogger, const GblStackFrame* pFrame) GBL_NOEXCEPT {
    GBL_UNUSED(pILogger,  pFrame);
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_END();
}

static GBL_RESULT GblILoggerIFace_write_(GblILogger* pILogger, const GblStackFrame* pFrame, GBL_LOG_LEVEL level, const char* pFmt, va_list varArgs) {
    GBL_UNUSED(pILogger);
    GBL_CTX_BEGIN(pFrame->pContext);
    char buffer[GBL_VA_SNPRINTF_BUFFER_SIZE];
    FILE* const pFile = (level >= GBL_LOG_LEVEL_ERROR)?
                stderr : stdout;
    vsnprintf(buffer, sizeof(buffer), pFmt, varArgs);
    fprintf(pFile, "%s\n", buffer);
    fflush(pFile);
    GBL_CTX_END();
}


static GBL_RESULT GblILoggerIFace_init_(GblILoggerClass* pIFace, void* pData, GblContext* pCtx) GBL_NOEXCEPT {
    GBL_UNUSED(pData);
    GBL_CTX_BEGIN(pCtx);
    pIFace->pFnWrite    = GblILoggerIFace_write_;
    pIFace->pFnPush     = GblILoggerIFace_push_;
    pIFace->pFnPop      = GblILoggerIFace_pop_;
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblILogger_write(GblILogger* pSelf,
                         const GblStackFrame*   pFrame,
                         GBL_LOG_LEVEL          level,
                         const char*            pFmt,
                         va_list                varArgs) GBL_NOEXCEPT
{
    GBL_CTX_BEGIN(NULL);
    GBL_INSTANCE_VCALL(GblILogger, pFnWrite,
                       pSelf, pFrame, level, pFmt, varArgs);
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblILogger_push(GblILogger* pSelf,
                        const GblStackFrame* pFrame) GBL_NOEXCEPT
{
    GBL_CTX_BEGIN(NULL);
    GBL_INSTANCE_VCALL(GblILogger, pFnPush,
                       pSelf, pFrame);
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblILogger_pop(GblILogger* pSelf,
                       const GblStackFrame* pFrame,
                       uint32_t             count) GBL_NOEXCEPT
{
    GBL_CTX_BEGIN(NULL);
    GBL_INSTANCE_VCALL(GblILogger, pFnPop,
                       pSelf, pFrame, count);
    GBL_CTX_END();
}

GBL_EXPORT GblType GblILogger_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static const GblTypeInfo info =  {
        .pFnClassInit     = (GblTypeClassInitializeFn)GblILoggerIFace_init_,
        .classSize        = sizeof(GblILoggerClass)
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);
        type = GblType_registerStatic(GblQuark_internStringStatic("GblILogger"),
                                      GBL_INTERFACE_TYPE,
                                      &info,
                                     GBL_TYPE_FLAG_TYPEINFO_STATIC);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }
    return type;
}





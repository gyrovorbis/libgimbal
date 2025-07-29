#include <gimbal/meta/instances/gimbal_context.h>
#include "../types/gimbal_type_.h"
#include <gimbal/utils/gimbal_date_time.h>
#include <gimbal/strings/gimbal_string_buffer.h>
#include <stdlib.h>

static GBL_RESULT GblContext_IAllocator_alloc_(GblIAllocator* pIAllocator, const GblStackFrame* pFrame, size_t  size, size_t  align, const char* pDbgStr, void** ppData) GBL_NOEXCEPT {
    GblContext* pParentCtx = GblContext_parentContext((GblContext*)pIAllocator);
    GBL_CTX_BEGIN(pParentCtx);

    // chain up
    if(GBL_CTX_CONTEXT() != (GblContext*)pIAllocator) {
        GBL_CTX_VERIFY_CALL(GblContext_memAlloc_(GBL_CTX_CONTEXT(), pFrame, size, align, pDbgStr, ppData));
    } else {
        GBL_CTX_VERIFY_ARG(size);
        GBL_CTX_VERIFY_ARG(align <= size && align >= 0);
        GBL_CTX_VERIFY_POINTER(ppData);
        //GBL_CTX_DEBUG("Malloc(Size: %z, Align: %z)", size, align);
        GBL_CTX_ERRNO_CLEAR();

        *ppData = GBL_ALIGNED_ALLOC(align, size);

        GBL_CTX_PUSH();
        GBL_CTX_PERROR("Malloc Failed");
        GBL_CTX_VERIFY(*ppData, GBL_RESULT_ERROR_MEM_ALLOC);
    #if 0
        GBL_CTX_DEBUG("%-20s: %20p", "Address", *ppData);
        GBL_CTX_DEBUG("%-20s: %20s", "Debug Marker", pDbgStr? pDbgStr : "NULL");
        GBL_CTX_DEBUG("%-20s: %20s", "Function", pFrame->sourceCurrent.pFunc);
        GBL_CTX_DEBUG("%-20s: %20z", "Line", pFrame->sourceCurrent.line);
        GBL_CTX_DEBUG("%-20s: %20s", "File", pFrame->sourceCurrent.pFile);
    #endif
        GBL_CTX_POP(1);
    }
    GBL_CTX_END();
}

static GBL_RESULT GblContext_IAllocator_realloc_(GblIAllocator* pIAllocator, const GblStackFrame* pFrame, void* pData, size_t  newSize, size_t  newAlign, void** ppNewData) GBL_NOEXCEPT {
    GBL_UNUSED(pFrame);

    GblContext* pParentCtx = GblContext_parentContext((GblContext*)pIAllocator);
    GBL_CTX_BEGIN(pParentCtx);
    // chain up
    if(GBL_CTX_CONTEXT() != (GblContext*)pIAllocator) {
        GBL_CTX_VERIFY_CALL(GblContext_memRealloc_(GBL_CTX_CONTEXT(), pFrame, pData, newSize, newAlign, ppNewData));
    } else {

        GBL_CTX_VERIFY_ARG(newSize);
        GBL_CTX_VERIFY_ARG(newAlign <= newSize && newAlign >= 0);
        GBL_CTX_VERIFY_POINTER(pData);
        GBL_CTX_VERIFY_POINTER(ppNewData);
        const uintptr_t ptrVal = (uintptr_t)pData;
        GBL_UNUSED(ptrVal);
        //GBL_CTX_DEBUG("Realloc(Size: %z, Align: %z) %p", newSize, newAlign, ptrVal);

        *ppNewData = GBL_ALIGNED_REALLOC(pData, newAlign, newSize);

        GBL_CTX_VERIFY(*ppNewData, GBL_RESULT_ERROR_MEM_REALLOC);
    #if 0
        GBL_CTX_PUSH();
        GBL_CTX_DEBUG("%-20s: %20p", "Address", *ppNewData);
        GBL_CTX_DEBUG("%-20s: %20s", "Function", pFrame->sourceCurrent.pFunc);
        GBL_CTX_DEBUG("%-20s: %20z", "Line", pFrame->sourceCurrent.line);
        GBL_CTX_DEBUG("%-20s: %20s", "File", pFrame->sourceCurrent.pFile);
        GBL_CTX_POP(1);
    #endif
    }
    GBL_CTX_END();
}

static GBL_RESULT GblContext_IAllocator_free_(GblIAllocator* pIAllocator, const GblStackFrame* pFrame, void* pData) GBL_NOEXCEPT {
    GBL_UNUSED(pFrame);

    GblContext* pParentCtx = GblContext_parentContext((GblContext*)pIAllocator);
    GBL_CTX_BEGIN(pParentCtx);
    // chain up
    if(GBL_CTX_CONTEXT() != (GblContext*)pIAllocator) {
        GBL_CTX_VERIFY_CALL(GblContext_memFree_(GBL_CTX_CONTEXT(), pFrame, pData));
    } else {

        const uintptr_t ptrVal = (uintptr_t)pData;
        GBL_UNUSED(ptrVal);
        GBL_ALIGNED_FREE(pData);

    #if 0
        GBL_CTX_DEBUG("Free(%p)", ptrVal);
        GBL_CTX_PUSH();
        GBL_CTX_DEBUG("%-20s: %20s", "Function", pFrame->sourceCurrent.pFunc);
        GBL_CTX_DEBUG("%-20s: %20z", "Line", pFrame->sourceCurrent.line);
        GBL_CTX_DEBUG("%-20s: %20s", "File", pFrame->sourceCurrent.pFile);
        GBL_CTX_POP(1);
    #endif
    }
    GBL_CTX_END();
}

static GBL_RESULT GblContext_ILogger_write_(GblILogger* pILogger, const GblStackFrame* pFrame, GBL_LOG_LEVEL level, const char* pFmt, va_list varArgs) GBL_NOEXCEPT {
    GBL_RESULT result = GBL_RESULT_SUCCESS;
    GblContext* pSelf = (GblContext*)(pILogger);
    GblContext* pCtx = (GblContext*)pILogger;

    if(!(pCtx->logFilter & level)) return GBL_RESULT_SUCCESS;

    GBL_ASSERT(pFmt);
    GBL_ASSERT(level >= 0);

    char buffer[GBL_VA_SNPRINTF_BUFFER_SIZE];
    char tabBuff[GBL_VA_SNPRINTF_BUFFER_SIZE];
    FILE* const pFile = (level >= GBL_LOG_LEVEL_ERROR)?
                stderr : stdout;
    const char* pPrefix = NULL;

    switch(level) {
    case GBL_LOG_LEVEL_WARNING: pPrefix = "! "; break;
    case GBL_LOG_LEVEL_ERROR:   pPrefix = "X "; break;
    case GBL_LOG_LEVEL_DEBUG:   pPrefix = "# "; break;
    case GBL_LOG_LEVEL_INFO:    pPrefix = "* "; break;
    default:                    pPrefix = "";   break;
    }

    //replace me later
    const int vsnprintfBytes = vsnprintf(buffer, sizeof(buffer), pFmt, varArgs);
    if(vsnprintfBytes > (int)sizeof(buffer)) {
        pPrefix = "T - "; //Truncated prefix!
        result = GBL_RESULT_TRUNCATED;
    }

    //not per byte!
    if(pSelf) {
        size_t depth = GblThd_current()->logStackDepth;
        for(unsigned t = 0; t < depth*8; ++t) {
            tabBuff[t] = ' ';
        }
        tabBuff[depth*8] = '\0';
    } else {
        tabBuff[0] = '\0';
    }

    switch(level) {
    case GBL_LOG_LEVEL_WARNING:
    case GBL_LOG_LEVEL_ERROR: {

        if((fprintf(pFile, "%s%s%s\n%s        @ %s(..): %s:%u\n",
                            tabBuff, pPrefix, buffer, tabBuff,
                            pFrame->record.srcLocation.pFunc,
                            pFrame->record.srcLocation.pFile,
                            pFrame->record.srcLocation.line)
                    < 0)) result = GBL_RESULT_ERROR_FILE_WRITE;
        break;
    }
    default:
        if(((fprintf(pFile, "%s%s%s\n",
                            tabBuff, pPrefix, buffer)
                    >= 0))) result = GBL_RESULT_ERROR_FILE_WRITE;
        break;
    }

    if(fflush(pFile) == 0) result = GBL_RESULT_ERROR_FILE_WRITE;
    return result;
}

static GBL_RESULT GblContext_ILogger_pop_(GblILogger* pILogger, const GblStackFrame* pFrame, uint32_t count) GBL_NOEXCEPT {
    GBL_UNUSED(pFrame);
    GblContext* pSelf = (GblContext*)pILogger;
    if(pSelf->logStackDepth < count)
        return GBL_RESULT_ERROR_UNDERFLOW;
    pSelf->logStackDepth -= count;
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GblContext_ILogger_push_(GblILogger* pILogger, const GblStackFrame* pFrame) GBL_NOEXCEPT {
    GBL_UNUSED(pFrame);
    GblContext* pSelf = (GblContext*)pILogger;
    if(pSelf->logStackDepth + 1 <= pSelf->logStackDepth) return GBL_RESULT_ERROR_OVERFLOW;
    ++pSelf->logStackDepth;
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GblContext_constructor_(GblObject* pSelf) GBL_NOEXCEPT {
    GBL_PRIV(pSelf->base).contextType = 1;
    ((GblContext*)pSelf)->logFilter = 0xffffffff;
    GBL_CTX_BEGIN(pSelf);

    GblObjectClass* pObjClass = GBL_OBJECT_CLASS(GblClass_weakRefDefault(GBL_OBJECT_TYPE));

    GBL_CTX_VERIFY_CALL(pObjClass->pFnConstructor(pSelf));

    GBL_CTX_END();
}

static GBL_RESULT GblContextClass_init_(GblContextClass* pClass, void* pData) {
    GBL_UNUSED(pData);
    GBL_CTX_BEGIN(NULL);

    if(!GblType_classRefCount(GBL_CONTEXT_TYPE)) {
        GBL_PROPERTIES_REGISTER(GblContext);
    }

    GBL_IALLOCATOR_CLASS(pClass)->pFnAlloc       = GblContext_IAllocator_alloc_;
    GBL_IALLOCATOR_CLASS(pClass)->pFnRealloc     = GblContext_IAllocator_realloc_;
    GBL_IALLOCATOR_CLASS(pClass)->pFnFree        = GblContext_IAllocator_free_;
    GBL_ILOGGER_CLASS(pClass)   ->pFnWrite       = GblContext_ILogger_write_;
    GBL_ILOGGER_CLASS(pClass)   ->pFnPush        = GblContext_ILogger_push_;
    GBL_ILOGGER_CLASS(pClass)   ->pFnPop         = GblContext_ILogger_pop_;
    GBL_OBJECT_CLASS(pClass)    ->pFnConstructor = GblContext_constructor_;

    GBL_CTX_END();
}


static GblContextClass defaultClass = {
    .GblIAllocatorImpl.pFnAlloc    = GblContext_IAllocator_alloc_,
    .GblIAllocatorImpl.pFnRealloc  = GblContext_IAllocator_realloc_,
    .GblIAllocatorImpl.pFnFree     = GblContext_IAllocator_free_,
    .GblILoggerImpl.pFnWrite       = GblContext_ILogger_write_,
    .GblILoggerImpl.pFnPush        = GblContext_ILogger_push_,
    .GblILoggerImpl.pFnPop         = GblContext_ILogger_pop_
};

GblContext GblContext_default_ = {
    .base.base.pClass = (GblBoxClass*)&defaultClass,
    .base.base.private_.contextType = 1,
    .logFilter = (GBL_LOG_LEVEL_INFO|GBL_LOG_LEVEL_WARNING|GBL_LOG_LEVEL_ERROR)
    //.logFilter = 0xffffffff
};

GblContext* globalCtx_ = &GblContext_default_;

void GblContext_setGlobal(GblContext* pCtx) GBL_NOEXCEPT {
    globalCtx_ = pCtx;
}

GBL_EXPORT GblContext* GblContext_global(void) GBL_NOEXCEPT {
    return globalCtx_;
}

GBL_EXPORT GblContext* GblContext_parentContext(const GblContext* pSelf) GBL_NOEXCEPT {
    return GblObject_findContext(GblObject_parent((GblObject*)pSelf));
}

GBL_EXPORT const GblCallRecord* GblContext_lastIssue(const GblContext* pSelf) GBL_NOEXCEPT {
    return pSelf? &pSelf->lastIssue : NULL;
}

GBL_EXPORT GBL_RESULT GblContext_setLastIssue(GblContext* pSelf, const GblCallRecord* pRecord) GBL_NOEXCEPT {
    //GBL_CTX_BEGIN(pSelf);
    if(pRecord) memcpy(&pSelf->lastIssue, pRecord, sizeof(GblCallRecord));
    else memset(&pSelf->lastIssue, 0, sizeof(GblCallRecord));
    //GBL_CTX_END();

    //GblError* pError = GblError_create(GBL_ERROR_TYPE, GBL_ENUM_TYPE, pRecord->result, pRecord->message);
    //GblError_throw(pError);
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT GblContext_clearLastIssue(GblContext* pSelf) GBL_NOEXCEPT {
    return GblContext_setLastIssue(pSelf, NULL);
}

GBL_EXPORT GblBool GblContext_hasIssue(const GblContext* pSelf) GBL_NOEXCEPT {
    return pSelf? GBL_RESULT_ISSUE(pSelf->lastIssue.result) : GBL_FALSE;
}

GBL_EXPORT GblBool GblContext_hasError(const GblContext* pSelf) GBL_NOEXCEPT {
    return pSelf? GBL_RESULT_ERROR(pSelf->lastIssue.result) : GBL_FALSE;
}

GBL_EXPORT GBL_RESULT GblContext_lastIssueResult  (const GblContext* pSelf) GBL_NOEXCEPT {
    return pSelf? pSelf->lastIssue.result : GBL_RESULT_ERROR_INVALID_POINTER;
}

GBL_EXPORT const char*  GblContext_lastIssueMessage (const GblContext* pSelf) GBL_NOEXCEPT {
    return pSelf? pSelf->lastIssue.message : NULL;
}


GBL_EXPORT GBL_RESULT GblContext_memAlloc_     (GblContext* pSelf,
                                  const GblStackFrame*  pFrame,
                                  size_t                size,
                                  size_t                align,
                                  const char*           pDbgStr,
                                  void**                ppData)     GBL_NOEXCEPT
{
    return pSelf->pClass->GblIAllocatorImpl.pFnAlloc((GblIAllocator*)pSelf, pFrame, size, align, pDbgStr, ppData);
}
GBL_EXPORT GBL_RESULT GblContext_memRealloc_   (GblContext* pSelf,
                                  const GblStackFrame*  pFrame,
                                  void*                 pData,
                                  size_t                newSize,
                                  size_t                newAlign,
                                  void**                ppNewData)  GBL_NOEXCEPT
{
    return pSelf->pClass->GblIAllocatorImpl.pFnRealloc((GblIAllocator*)pSelf, pFrame, pData, newSize, newAlign, ppNewData);
}
GBL_EXPORT GBL_RESULT GblContext_memFree_      (GblContext* pSelf,
                                  const GblStackFrame*  pFrame,
                                  void*                 pData)      GBL_NOEXCEPT
{
    return !pData? GBL_RESULT_SUCCESS : pSelf->pClass->GblIAllocatorImpl.pFnFree((GblIAllocator*)pSelf, pFrame, pData);
}

GBL_EXPORT GBL_RESULT GblContext_logWrite_     (GblContext* pSelf,
                                  const GblStackFrame*  pFrame,
                                  GblFlags              level,
                                  const char*           pFmt,
                                  va_list               varArgs)    GBL_NOEXCEPT
{
    //GBL_CTX_BEGIN(pSelf);
    return pSelf->pClass->GblILoggerImpl.pFnWrite((GblILogger*)pSelf, pFrame, level, pFmt, varArgs);
    //GBL_CTX_END();
}
GBL_EXPORT GBL_RESULT GblContext_logPush_      (GblContext* pSelf,
                                  const GblStackFrame*  pFrame)     GBL_NOEXCEPT
{
    return pSelf->pClass->GblILoggerImpl.pFnPush((GblILogger*)pSelf, pFrame);
}
GBL_EXPORT GBL_RESULT GblContext_logPop_       (GblContext* pSelf,
                                  const GblStackFrame*  pFrame,
                                  uint32_t              count)      GBL_NOEXCEPT
{
    return pSelf->pClass->GblILoggerImpl.pFnPop((GblILogger*)pSelf, pFrame, count);
}
GBL_EXPORT GBL_RESULT GblContext_callRecordSet_(GblContext* pSelf,
                                  const GblStackFrame* pFrame,
                                  const GblCallRecord* pRecord)     GBL_NOEXCEPT
{
    GBL_UNUSED(pFrame);
    return GblContext_setLastIssue(pSelf, pRecord);
}

GBL_EXPORT void GblContext_setLogFilter(GblContext* pSelf, GblFlags mask) {
    pSelf->logFilter = mask;
}

GBL_EXPORT void GblContext_logBuildInfo(const GblContext* pSelf) {
    GBL_CTX_BEGIN(pSelf);

    GblDateTime dt;
    struct {
        GblStringBuffer buffer;
        char ext[256];
    } str;
    GblStringBuffer_construct(&str.buffer, NULL, 0, sizeof(str));
    GBL_CTX_INFO("%-20s: %-100.100s", "Local Time",
                 GblDateTime_format(GblDateTime_nowLocal(&dt),
                                    &str.buffer,
                                    "%A, %m/%d/%Y %I:%M:%S %p"));
    GblStringBuffer_destruct(&str.buffer);

    GBL_CTX_INFO("LibGimbal Info");
    GBL_CTX_PUSH();
    GBL_CTX_INFO("%-20s: %-100.100s", "Version", GIMBAL_VERSION);
    GBL_CTX_POP(1);

    GBL_CTX_INFO("Project Info");
    GBL_CTX_PUSH();
    GBL_CTX_INFO("%-20s: %-100.100s", "Name", GBL_PROJECT_NAME);
    GBL_CTX_INFO("%-20s: %-100.100s", "Version", GBL_PROJECT_VERSION);
    GBL_CTX_INFO("%-20s: %-100.100s", "URL", GBL_PROJECT_URL);
    GBL_CTX_INFO("%-20s: %-100.100s", "Description", GBL_PROJECT_DESCRIPTION);
    GBL_CTX_POP(1);

    GBL_CTX_INFO("Build Info");
    GBL_CTX_PUSH();
    GBL_CTX_INFO("%-20s: %-100.100s", "Timestamp", GBL_BUILD_TIMESTAMP);
    GBL_CTX_INFO("%-20s: %-100.100s", "Type", GBL_BUILD_TYPE);
    //GBL_CTX_INFO("%-20s: %-100.100s", "Config", GBL_BUILD_CONFIG);
    GBL_CTX_POP(1);


    GBL_CTX_INFO("CI Info");
    GBL_CTX_PUSH();
    #if GBL_BUILD_CI
    GBL_CTX_INFO("%-20s: %-100.100s", "Project", GBL_STRINGIFY(GBL_BUILD_CI_PROJECT_TITLE));

    GBL_CTX_INFO("Source Control Commit Info");
    GBL_CTX_PUSH();
    GBL_CTX_INFO("%-20s: %-100.100s", "Title", GBL_STRINGIFY_MULTI(GBL_BUILD_CI_COMMIT_TITLE));
    GBL_CTX_INFO("%-20s: %-100.100s", "Timestamp", GBL_STRINGIFY_MULTI(GBL_BUILD_CI_COMMIT_TIMESTAMP));
    GBL_CTX_INFO("%-20s: %-100.100s", "SHA", GBL_STRINGIFY_MULTI(GBL_BUILD_CI_COMMIT_SHA));
    GBL_CTX_INFO("%-20s: %-100.100s", "Tag", GBL_STRINGIFY_MULTI(GBL_BUILD_CI_COMMIT_TAG));
    GBL_CTX_INFO("%-20s: %-100.100s", "Ref", GBL_STRINGIFY_MULTI(GBL_BUILD_CI_COMMIT_BRANCH));
    GBL_CTX_POP(1);

    GBL_CTX_INFO("Job Info");
    GBL_CTX_PUSH();
    GBL_CTX_INFO("%-20s: %-100.100s", "ID", GBL_STRINGIFY_MULTI(GBL_BUILD_CI_JOB_ID));
    GBL_CTX_INFO("%-20s: %-100.100s", "Name", GBL_STRINGIFY_MULTI(GBL_BUILD_CI_JOB_NAME));
    GBL_CTX_INFO("%-20s: %-100.100s", "Stage", GBL_STRINGIFY_MULTI(GBL_BUILD_CI_JOB_STAGE));
    GBL_CTX_POP(1);

    GBL_CTX_INFO("User Info");
    GBL_CTX_PUSH();
    GBL_CTX_INFO("%-20s: %-100.100s", "Login", GBL_STRINGIFY_MULTI(GBL_BUILD_CI_USER_LOGIN));
    GBL_CTX_INFO("%-20s: %-100.100s", "Name", GBL_STRINGIFY_MULTI(GBL_BUILD_CI_USER_NAME));
    GBL_CTX_INFO("%-20s: %-100.100s", "Email Address", GBL_STRINGIFY_MULTI(GBL_BUILD_CI_USER_EMAIL));
    GBL_CTX_POP(1);

    GBL_CTX_INFO("Build Node Info");
    GBL_CTX_PUSH();
    GBL_CTX_INFO("%-20s: %-100.100s", "Description", GBL_STRINGIFY_MULTI(GBL_BUILD_CI_RUNNER_DESCRIPTION));
    //GBL_CTX_INFO("%-20s: %-100.100s", "Tags", GBL_STRINGIFY_MULTI(GBL_BUILD_CI_RUNNER_TAGS));
    GBL_CTX_INFO("%-20s: %-100.100s", "Architecture", GBL_STRINGIFY_MULTI(GBL_BUILD_CI_RUNNER_ARCH));
    GBL_CTX_POP(1);
    #else
    GBL_CTX_WARN("UNOFFICIAL LOCAL BUILD !");
    #endif
    GBL_CTX_POP(1);

    GBL_CTX_INFO("Compiler Info");
    GBL_CTX_PUSH();
    GBL_CTX_INFO("C");
    GBL_CTX_PUSH();
    GBL_CTX_INFO("%-20s: %-100.100s", "ID", GBL_BUILD_C_COMPILER_ID);
    GBL_CTX_INFO("%-20s: %-100.100s", "Version", GBL_BUILD_C_COMPILER_VERSION);
    GBL_CTX_INFO("%-20s: %-100.100s", "Target", GBL_BUILD_C_COMPILER_TARGET);
    GBL_CTX_INFO("%-20s: %-100.100s", "Toolchain", GBL_BUILD_C_COMPILER_TOOLCHAIN);
    GBL_CTX_INFO("%-20s: %-100.100s", "Language Standard", GBL_BUILD_C_STANDARD);
    GBL_CTX_INFO("%-20s: %-100.100s", "Language Extensions", GBL_BUILD_C_EXTENSIONS);
    GBL_CTX_POP(1);

    GBL_CTX_INFO("C++");
    GBL_CTX_PUSH();
    GBL_CTX_INFO("%-20s: %-100.100s", "ID", GBL_BUILD_CPP_COMPILER_ID);
    GBL_CTX_INFO("%-20s: %-100.100s", "Version", GBL_BUILD_CPP_COMPILER_VERSION);
    GBL_CTX_INFO("%-20s: %-100.100s", "Target", GBL_BUILD_CPP_COMPILER_TARGET);
    GBL_CTX_INFO("%-20s: %-100.100s", "Toolchain", GBL_BUILD_CPP_COMPILER_TOOLCHAIN);
    GBL_CTX_INFO("%-20s: %-100.100s", "Language Standard", GBL_BUILD_CPP_STANDARD);
    GBL_CTX_INFO("%-20s: %-100.100s", "Language Extensions", GBL_BUILD_CPP_EXTENSIONS);
    GBL_CTX_POP(1);
    GBL_CTX_POP(1);

    GBL_CTX_INFO("Environment Info");
    GBL_CTX_PUSH();
    GBL_CTX_INFO("Host");
    GBL_CTX_PUSH();
    GBL_CTX_INFO("%-20s: %-100.100s", "Host Name", GBL_BUILD_HOST_NAME);
    //GBL_CTX_VERBOSE("%-20s: %-100.100s", "Domain Name", GBL_BUILD_HOST_DOMAIN_NAME);
    GBL_CTX_INFO("%-20s: %-100.100s", "Operating System", GBL_BUILD_HOST_OS);
    GBL_CTX_INFO("%-20s: %-100.100s", "Architecture", GBL_BUILD_HOST_ARCH);
    GBL_CTX_INFO("Processor");
    GBL_CTX_PUSH();
    GBL_CTX_INFO("%-20s: %-100.100s", "Family", GBL_BUILD_HOST_PROCESSOR_NAME);
    GBL_CTX_INFO("%-20s: %-100.100s", "Description", GBL_BUILD_HOST_PROCESSOR_DESCRIPTION);
    GBL_CTX_INFO("%-20s: %-100.u", "Physical Cores", GBL_BUILD_HOST_PROCESSOR_CORES_PHYSICAL);
    GBL_CTX_INFO("%-20s: %-100.u", "Logical Cores", GBL_BUILD_HOST_PROCESSOR_CORES_LOGICAL);
    GBL_CTX_POP(1);
    GBL_CTX_INFO("Physical Memory");
    GBL_CTX_PUSH();
    GBL_CTX_INFO("%-20s: %-100u", "Total (MB)", GBL_BUILD_HOST_MEMORY_PHYSICAL_TOTAL);
    GBL_CTX_INFO("%-20s: %-100u", "Available (MB)", GBL_BUILD_HOST_MEMORY_PHYSICAL_AVAILABLE);
    GBL_CTX_POP(1);
    GBL_CTX_POP(1);
    GBL_CTX_INFO("Target");
    GBL_CTX_PUSH();
    GBL_CTX_INFO("%-20s: %-100.100s", "Operating System", GBL_BUILD_TARGET_OS);
    GBL_CTX_INFO("%-20s: %-100.100s", "Architecture", GBL_BUILD_TARGET_ARCH);
//    GBL_CTX_INFO("%-20s: %-100u", "Word Size", GBL_PTR_SIZE * 8);
    GBL_CTX_INFO("%-20s: %-100.100s", "Endianness", GBL_BIG_ENDIAN? "Big" : "Little");
    GBL_CTX_POP(2);

    GBL_CTX_END_BLOCK();
}
GBL_EXPORT GblType GblContext_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static GblInterfaceImpl ifaceEntries[2] = {
        {
            .classOffset   = offsetof(GblContextClass, GblIAllocatorImpl)
        }, {
            .classOffset   = offsetof(GblContextClass, GblILoggerImpl)
        }
    };

    static GblTypeInfo info = {
        .pFnClassInit     = (GblClassInitFn)GblContextClass_init_,
        .classSize        = sizeof(GblContextClass),
        .instanceSize     = sizeof(GblContext),
        .interfaceCount   = 2,
        .pInterfaceImpls    = ifaceEntries
    };

    if(type == GBL_INVALID_TYPE) GBL_UNLIKELY {

        ifaceEntries[0].interfaceType = GBL_IALLOCATOR_TYPE;
        ifaceEntries[1].interfaceType = GBL_ILOGGER_TYPE;

        type = GblType_register(GblQuark_internStatic("GblContext"),
                                GBL_OBJECT_TYPE,
                                &info,
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);

    }
    return type;
}


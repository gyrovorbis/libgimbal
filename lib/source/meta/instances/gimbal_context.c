#include <gimbal/meta/instances/gimbal_context.h>
#include "../types/gimbal_type_.h"

static GBL_RESULT GblContext_IAllocator_alloc_(GblIAllocator* pIAllocator, const GblStackFrame* pFrame, GblSize size, GblSize align, const char* pDbgStr, void** ppData) GBL_NOEXCEPT {
    GblContext* pParentCtx = GblContext_parentContext((GblContext*)pIAllocator);
    GBL_API_BEGIN(pParentCtx);

    // chain up
    if(GBL_API_CONTEXT() != (GblContext*)pIAllocator) {
        GBL_API_VERIFY_CALL(GblContext_memAlloc_(GBL_API_CONTEXT(), pFrame, size, align, pDbgStr, ppData));
    } else {
        GBL_API_VERIFY_ARG(size);
        GBL_API_VERIFY_ARG(align <= size && align >= 0);
        GBL_API_VERIFY_POINTER(ppData);
        //GBL_API_DEBUG("Malloc(Size: %" GBL_SIZE_FMT ", Align: %" GBL_SIZE_FMT ")", size, align);
        GBL_API_ERRNO_CLEAR();

        *ppData = GBL_ALIGNED_ALLOC(align, size);

        GBL_API_PUSH();
        GBL_API_PERROR("Malloc Failed");
        GBL_API_VERIFY(*ppData, GBL_RESULT_ERROR_MEM_ALLOC);
    #if 0
        GBL_API_DEBUG("%-20s: %20p", "Address", *ppData);
        GBL_API_DEBUG("%-20s: %20s", "Debug Marker", pDbgStr? pDbgStr : "NULL");
        GBL_API_DEBUG("%-20s: %20s", "Function", pFrame->sourceCurrent.pFunc);
        GBL_API_DEBUG("%-20s: %20"GBL_SIZE_FMT, "Line", pFrame->sourceCurrent.line);
        GBL_API_DEBUG("%-20s: %20s", "File", pFrame->sourceCurrent.pFile);
    #endif
        GBL_API_POP(1);
    }
    GBL_API_END();
}

static GBL_RESULT GblContext_IAllocator_realloc_(GblIAllocator* pIAllocator, const GblStackFrame* pFrame, void* pData, GblSize newSize, GblSize newAlign, void** ppNewData) GBL_NOEXCEPT {
    GBL_UNUSED(pFrame);

    GblContext* pParentCtx = GblContext_parentContext((GblContext*)pIAllocator);
    GBL_API_BEGIN(pParentCtx);
    // chain up
    if(GBL_API_CONTEXT() != (GblContext*)pIAllocator) {
        GBL_API_VERIFY_CALL(GblContext_memRealloc_(GBL_API_CONTEXT(), pFrame, pData, newSize, newAlign, ppNewData));
    } else {

        GBL_API_VERIFY_ARG(newSize);
        GBL_API_VERIFY_ARG(newAlign <= newSize && newAlign >= 0);
        GBL_API_VERIFY_POINTER(pData);
        GBL_API_VERIFY_POINTER(ppNewData);
        const uintptr_t ptrVal = (uintptr_t)pData;
        //GBL_API_DEBUG("Realloc(Size: %" GBL_SIZE_FMT ", Align: %" GBL_SIZE_FMT ") %p", newSize, newAlign, ptrVal);

        *ppNewData = GBL_ALIGNED_REALLOC(pData, newAlign, newSize);

        GBL_API_VERIFY(*ppNewData, GBL_RESULT_ERROR_MEM_REALLOC);
    #if 0
        GBL_API_PUSH();
        GBL_API_DEBUG("%-20s: %20p", "Address", *ppNewData);
        GBL_API_DEBUG("%-20s: %20s", "Function", pFrame->sourceCurrent.pFunc);
        GBL_API_DEBUG("%-20s: %20" GBL_SIZE_FMT, "Line", pFrame->sourceCurrent.line);
        GBL_API_DEBUG("%-20s: %20s", "File", pFrame->sourceCurrent.pFile);
        GBL_API_POP(1);
    #endif
    }
    GBL_API_END();
}

static GBL_RESULT GblContext_IAllocator_free_(GblIAllocator* pIAllocator, const GblStackFrame* pFrame, void* pData) GBL_NOEXCEPT {
    GBL_UNUSED(pFrame);

    GblContext* pParentCtx = GblContext_parentContext((GblContext*)pIAllocator);
    GBL_API_BEGIN(pParentCtx);
    // chain up
    if(GBL_API_CONTEXT() != (GblContext*)pIAllocator) {
        GBL_API_VERIFY_CALL(GblContext_memFree_(GBL_API_CONTEXT(), pFrame, pData));
    } else {

        const uintptr_t ptrVal = (uintptr_t)pData;

        GBL_ALIGNED_FREE(pData);

    #if 0
        GBL_API_DEBUG("Free(%p)", ptrVal);
        GBL_API_PUSH();
        GBL_API_DEBUG("%-20s: %20s", "Function", pFrame->sourceCurrent.pFunc);
        GBL_API_DEBUG("%-20s: %20" GBL_SIZE_FMT, "Line", pFrame->sourceCurrent.line);
        GBL_API_DEBUG("%-20s: %20s", "File", pFrame->sourceCurrent.pFile);
        GBL_API_POP(1);
    #endif
    }
    GBL_API_END();
}

static GBL_RESULT GblContext_ILogger_write_(GblILogger* pILogger, const GblStackFrame* pFrame, GBL_LOG_LEVEL level, const char* pFmt, va_list varArgs) GBL_NOEXCEPT {
    GblContext* pSelf = (GblContext*)(pILogger);
    GblContext* pCtx = (GblContext*)pILogger;

    if(!(pCtx->logFilter & level)) return GBL_RESULT_SUCCESS;

    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pFmt);
    GBL_API_VERIFY_ARG(level >= 0 /*&& level < GBL_LOG_LEVEL_COUNT*/); // or not to allow for user levels!

    char buffer[GBL_VA_SNPRINTF_BUFFER_SIZE] = { '\0' };
    char tabBuff[GBL_VA_SNPRINTF_BUFFER_SIZE];// = { '\t' };
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
        GBL_API_RECORD_SET(GBL_RESULT_TRUNCATED, "Log message truncated!");
    }

    //not per byte!
    if(pSelf) {
        for(unsigned t = 0; t < pSelf->logStackDepth*8; ++t) {
            tabBuff[t] = ' ';
        }
        tabBuff[pSelf->logStackDepth*8] = '\0';
    } else {
        tabBuff[0] = '\0';
    }

    switch(level) {
    case GBL_LOG_LEVEL_WARNING:
    case GBL_LOG_LEVEL_ERROR: {

        GBL_API_VERIFY((fprintf(pFile, "%s%s%s\n%s        @ %s(..): %s:%" GBL_SIZE_FMT"\n",
                            tabBuff, pPrefix, buffer, tabBuff,
                            pFrame->sourceCurrent.pFunc,
                            pFrame->sourceCurrent.pFile,
                            pFrame->sourceCurrent.line)
                    >= 0), GBL_RESULT_ERROR_FILE_WRITE);
        break;
    }
    default:
        GBL_API_VERIFY((fprintf(pFile, "%s%s%s\n",
                            tabBuff, pPrefix, buffer)
                    >= 0), GBL_RESULT_ERROR_FILE_WRITE);
        break;
    }

    GBL_API_VERIFY(fflush(pFile) == 0, GBL_RESULT_ERROR_FILE_WRITE);
    GBL_API_END();
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
    GBL_API_BEGIN(pSelf);
    GBL_API_VERIFY(pSelf->logStackDepth + 1 > pSelf->logStackDepth, GBL_RESULT_ERROR_OVERFLOW);
    ++pSelf->logStackDepth;
    GBL_API_END();
}

static GBL_RESULT GblContext_constructor_(GblObject* pSelf) GBL_NOEXCEPT {
    GBL_PRIV(pSelf->base).contextType = 1;
    ((GblContext*)pSelf)->logFilter = 0xffffffff;
    GBL_API_BEGIN(pSelf);

    GblObjectClass* pObjClass = GBL_OBJECT_CLASS(GblClass_weakRefDefault(GBL_OBJECT_TYPE));

    GBL_API_VERIFY_CALL(pObjClass->pFnConstructor(pSelf));

    GBL_API_END();
}

static GBL_RESULT GblContextClass_init_(GblContextClass* pClass, void* pData, GblContext* pCtx) GBL_NOEXCEPT {
    GBL_UNUSED(pData);
    GBL_API_BEGIN(pCtx);

    if(!GblType_classRefCount(GBL_CONTEXT_TYPE)) {
        GBL_PROPERTY_TABLE_REGISTER(GBL_CONTEXT, pClass);
    }

    pClass->iAllocatorIFace.pFnAlloc    = GblContext_IAllocator_alloc_;
    pClass->iAllocatorIFace.pFnRealloc  = GblContext_IAllocator_realloc_;
    pClass->iAllocatorIFace.pFnFree     = GblContext_IAllocator_free_;
    pClass->iLoggerIFace.pFnWrite       = GblContext_ILogger_write_;
    pClass->iLoggerIFace.pFnPush        = GblContext_ILogger_push_;
    pClass->iLoggerIFace.pFnPop         = GblContext_ILogger_pop_;
    pClass->base.pFnConstructor         = GblContext_constructor_;
    GBL_API_END();
}


static GblContextClass defaultClass = {
    .iAllocatorIFace.pFnAlloc    = GblContext_IAllocator_alloc_,
    .iAllocatorIFace.pFnRealloc  = GblContext_IAllocator_realloc_,
    .iAllocatorIFace.pFnFree     = GblContext_IAllocator_free_,
    .iLoggerIFace.pFnWrite       = GblContext_ILogger_write_,
    .iLoggerIFace.pFnPush        = GblContext_ILogger_push_,
    .iLoggerIFace.pFnPop         = GblContext_ILogger_pop_
};

static GblContext defaultCtx_ = {
    .base.base.pClass = (GblBoxClass*)&defaultClass,
    .base.base.private_.contextType = 1,
    .logFilter = 0xffffffff
};

static GblContext* globalCtx_ = &defaultCtx_;


void GblContext_globalSet(GblContext* pCtx) GBL_NOEXCEPT {
    globalCtx_ = pCtx;
}

GBL_EXPORT GblContext* GblContext_global(void) GBL_NOEXCEPT {
    return globalCtx_;
}

GBL_EXPORT GblContext* GblContext_parentContext(const GblContext* pSelf) GBL_NOEXCEPT {
    return GblObject_contextFind(GblObject_parent((GblObject*)pSelf));
}

GBL_EXPORT const GblCallRecord* GblContext_lastIssue(const GblContext* pSelf) GBL_NOEXCEPT {
    return pSelf? &pSelf->lastIssue : NULL;
}

GBL_API GblContext_lastIssueSet(GblContext* pSelf, const GblCallRecord* pRecord) GBL_NOEXCEPT {
    //GBL_API_BEGIN(pSelf);
    if(pRecord) memcpy(&pSelf->lastIssue, pRecord, sizeof(GblCallRecord));
    else memset(&pSelf->lastIssue, 0, sizeof(GblCallRecord));
    //GBL_API_END();
    return GBL_RESULT_SUCCESS;
}

GBL_API GblContext_lastIssueClear(GblContext* pSelf) GBL_NOEXCEPT {
    return GblContext_lastIssueSet(pSelf, NULL);
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


GBL_API GblContext_memAlloc_     (GblContext* pSelf,
                                  const GblStackFrame*  pFrame,
                                  GblSize               size,
                                  GblSize               align,
                                  const char*           pDbgStr,
                                  void**                ppData)     GBL_NOEXCEPT
{
    return pSelf->pClass->iAllocatorIFace.pFnAlloc((GblIAllocator*)pSelf, pFrame, size, align, pDbgStr, ppData);
}
GBL_API GblContext_memRealloc_   (GblContext* pSelf,
                                  const GblStackFrame*  pFrame,
                                  void*                 pData,
                                  GblSize               newSize,
                                  GblSize               newAlign,
                                  void**                ppNewData)  GBL_NOEXCEPT
{
    return pSelf->pClass->iAllocatorIFace.pFnRealloc((GblIAllocator*)pSelf, pFrame, pData, newSize, newAlign, ppNewData);
}
GBL_API GblContext_memFree_      (GblContext* pSelf,
                                  const GblStackFrame*  pFrame,
                                  void*                 pData)      GBL_NOEXCEPT
{
    return !pData? GBL_RESULT_SUCCESS : pSelf->pClass->iAllocatorIFace.pFnFree((GblIAllocator*)pSelf, pFrame, pData);
}

GBL_API GblContext_logWrite_     (GblContext* pSelf,
                                  const GblStackFrame*  pFrame,
                                  GBL_LOG_LEVEL         level,
                                  const char*           pFmt,
                                  va_list               varArgs)    GBL_NOEXCEPT
{
    //GBL_API_BEGIN(pSelf);
    return pSelf->pClass->iLoggerIFace.pFnWrite((GblILogger*)pSelf, pFrame, level, pFmt, varArgs);
    //GBL_API_END();
}
GBL_API GblContext_logPush_      (GblContext* pSelf,
                                  const GblStackFrame*  pFrame)     GBL_NOEXCEPT
{
    return pSelf->pClass->iLoggerIFace.pFnPush((GblILogger*)pSelf, pFrame);
}
GBL_API GblContext_logPop_       (GblContext* pSelf,
                                  const GblStackFrame*  pFrame,
                                  uint32_t              count)      GBL_NOEXCEPT
{
    return pSelf->pClass->iLoggerIFace.pFnPop((GblILogger*)pSelf, pFrame, count);
}
GBL_API GblContext_callRecordSet_(GblContext* pSelf,
                                  const GblStackFrame* pFrame,
                                  const GblCallRecord* pRecord)     GBL_NOEXCEPT
{
    GBL_UNUSED(pFrame);
    return GblContext_lastIssueSet(pSelf, pRecord);
}

GBL_EXPORT void GblContext_setLogFilter(GblContext* pSelf, GblFlags mask) {
    pSelf->logFilter = mask;
}



extern GBL_RESULT GblContext_typeRegister_(GblContext* pCtx) GBL_NOEXCEPT {
    GBL_API_BEGIN(pCtx);
    GblType_registerBuiltin_(GBL_TYPE_BUILTIN_INDEX_CONTEXT,
      GBL_OBJECT_TYPE,
      GblQuark_internStringStatic("Context"),
      &(const GblTypeInfo) {
          .pFnClassInit     = (GblTypeClassInitializeFn)GblContextClass_init_,
          .classSize        = sizeof(GblContextClass),
          .instanceSize     = sizeof(GblContext),
          .interfaceCount   = 2,
          .pInterfaceMap    = (const GblTypeInterfaceMapEntry[]) {
              {
                  .interfaceType = GBL_IALLOCATOR_TYPE,
                  .classOffset   = offsetof(GblContextClass, iAllocatorIFace)
              }, {
                  .interfaceType = GBL_ILOGGER_TYPE,
                  .classOffset   = offsetof(GblContextClass, iLoggerIFace)
              }
          }
      },
      GBL_TYPE_FLAGS_NONE);
    GBL_API_END();
}

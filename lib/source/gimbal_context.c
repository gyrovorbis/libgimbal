#include <gimbal/gimbal_context.h>
#include <gimbal/gimbal_types.h>
#include <gimbal/gimbal_api.h>
#include <gimbal/gimbal_config.h>
#include <string.h>



typedef struct GblContext_ {
    GblHandle_                  baseHandle;

    struct {
        GblContextExtLog        log;
        GblContextExtMem        mem;
        GblContextExtApi        api;
    }                           ext;
#if GBL_CONFIG_EXT_CONTEXT_DEFAULT_ENABLED
    uint32_t logStackDepth;
#endif
    //GblError               lastError;
    //GblContextCreateInfo   createInfo;
    //GblApiCookie*          pApiCookieTop;
} GblContext_;

static const GblContextCreateInfo createInfoDefault_ = {
    33,
    NULL,
    NULL,
    NULL,
    NULL
};

static GBL_API gblContextLogBuildInfo(GblContext hContext) {
    GBL_API_BEGIN(hContext, "swanky fucker: %d", 3);

    GBL_API_PUSH();
    void* pPtr = GBL_API_MALLOC(32);
    pPtr = GBL_API_REALLOC(pPtr, 34);
    GBL_API_FREE(pPtr);

    GBL_API_END();
}



GBL_API gblContextVersion           (GblVersion* pVersion) {
    GBL_ASSERT(pVersion, "NULL version output pointer!");
    *pVersion = GBL_VERSION_MAKE(GBL_PROJECT_VERSION_MAJOR, GBL_PROJECT_VERSION_MINOR, GBL_PROJECT_VERSION_PATCH);
    return GBL_RESULT_SUCCESS;
}


static GBL_API gblContextBuildInfoLog_(GblContext hCtx) {
    GBL_API_BEGIN(hCtx, "Build Info");

    GBL_API_PUSH("Project Info");
    GBL_API_VERBOSE("%-20s: %40s", "Name", GBL_PROJECT_NAME);
    GBL_API_VERBOSE("%-20s: %40s", "Version", GBL_PROJECT_VERSION);
    GBL_API_VERBOSE("%-20s: %40s", "URL", GBL_PROJECT_URL);
    GBL_API_VERBOSE("%-20s: %40s", "Description", GBL_PROJECT_DESCRIPTION);
    GBL_API_POP(1);

    GBL_API_PUSH("Build Info");
    GBL_API_VERBOSE("%-20s: %40s", "Type", GBL_BUILD_TYPE);
    GBL_API_VERBOSE("%-20s: %40s", "Config", GBL_BUILD_CONFIG);
    GBL_API_POP(1);

    GBL_API_PUSH("Compiler Info");
    GBL_API_VERBOSE("%-20s: %40s", "C", GBL_BUILD_C_COMPILER);
    GBL_API_VERBOSE("%-20s: %40s", "C++", GBL_BUILD_CPP_COMPILER);
    GBL_API_POP(1);

    GBL_API_PUSH("Environment Info");
    GBL_API_PUSH("Host");
    GBL_API_VERBOSE("%-20s: %40s", "Operating System", GBL_HOST_OS);
    GBL_API_VERBOSE("%-20s: %40s", "Architecture", GBL_HOST_ARCH);
    GBL_API_POP(1);
    GBL_API_PUSH("Target");
    GBL_API_VERBOSE("%-20s: %40s", "Operating System", GBL_TARGET_OS);
    GBL_API_VERBOSE("%-20s: %40s", "Architecture", GBL_TARGET_ARCH);
    GBL_API_POP(2);

    GBL_API_END();
}

GBL_API gblContextConstruct (GblContext hCtx, const GblContextCreateInfo* pInfo) {
    GBL_ASSERT(hCtx, "NULL Context Handle!");



    memset(hCtx, 0, sizeof(GblContext_));
    hCtx->baseHandle.pContext   = hCtx;
    hCtx->baseHandle.pParent    = pInfo->handleInfo.hParent;
    hCtx->baseHandle.pUserdata  = pInfo->handleInfo.pUserdata;
    memcpy(&hCtx->ext.log, pInfo->pExtLog, sizeof(GblContextExtLog));
    memcpy(&hCtx->ext.mem, pInfo->pExtMem, sizeof(GblContextExtMem));
   // memcpy(&hCtx->ext.api, pInfo->pExtApi, sizeof(GblContextExtApi));

    GBL_API_BEGIN(hCtx, "Creating Context");

    GblVersion verCur = 0;
    GBL_API_CALL(gblContextVersion(&verCur));
    GblVersionInfo verCurInfo = GBL_VERSION_EXTRACT(verCur);
    GblVersionInfo verMinInfo = GBL_VERSION_EXTRACT(pInfo->versionMin);
    char verCurString[GBL_VERSION_STRING_SIZE_MAX] = { '\0' };
    char verMinString[GBL_VERSION_STRING_SIZE_MAX] = { '\0' };

    GBL_API_CALL(gblVersionInfoString(&verCurInfo, verCurString, sizeof(verCurString)));
    GBL_API_CALL(gblVersionInfoString(&verMinInfo, verMinString, sizeof(verMinString)));

    GBL_API_VERBOSE("Version Minimum: %s", verMinString);
    GBL_API_VERBOSE("Version Current: %s", verCurString);

    GBL_API_VERIFY(verCur >= pInfo->versionMin, GBL_RESULT_VERSION_MISMATCH);

    GBL_API_CALL(gblContextBuildInfoLog_(hCtx));

    GBL_API_END();
}


GBL_API gblContextCreate            (GblContext* phCtx, const GblContextCreateInfo* pInfo) {

    *phCtx = malloc(sizeof(GblContext_));
    return gblContextConstruct(*phCtx, pInfo);



    //struct GblContext_* pCtx = *phCtx;
}

GBL_API gblContextDestroy(GblContext hCtx) {
    return gblHandleDestruct((GblHandle)hCtx);
}

static inline GblContext gblContextParent_(GblContext hCtx) {
    GBL_ASSERT(hCtx);
    GblContext hCurrentCtx = hCtx;
    GBL_RESULT result = GBL_RESULT_SUCCESS;
    while(hCurrentCtx == hCtx) {
        GblHandle hHandle = GBL_HANDLE_INVALID;
        result = gblHandleParentGet((GblHandle)hCurrentCtx, &hHandle);
        GBL_ASSERT(GBL_RESULT_SUCCESS(result));
        if(hHandle) {
            result = gblHandleContext(hHandle, &hCurrentCtx);
            GBL_ASSERT(GBL_RESULT_SUCCESS(result));
        } else {
            hCurrentCtx = GBL_HANDLE_INVALID;
        }
    }
    return hCurrentCtx;
}

#define GBL_CONTEXT_EXT_IMPL_DEFAULT_CALL_(name, result, ...) \
    if(result == GBL_RESULT_UNIMPLEMENTED || result == GBL_RESULT_INCOMPLETE || result == GBL_RESULT_UNSUPPORTED) \
        result = gblContext##name##Default_(__VA_ARGS__)

#define GBL_CONTEXT_EXT_IMPL_PARENT_CALL_(name, result, hCtx, ...) \
    if(result == GBL_RESULT_UNIMPLEMENTED || result == GBL_RESULT_INCOMPLETE || result == GBL_RESULT_UNSUPPORTED) { \
        GblContext hParent = gblContextParent_(hCtx);   \
        if(hParent) result = gblContext##name (hParent GBL_VA_ARGS(__VA_ARGS__));   \
    }

#define GBL_CONTEXT_EXT_DECL_(name, ...) \
    GBL_API gblContext##name (GblContext hCtx, const GblStackFrame* pFrame GBL_VA_ARGS(GBL_MAP_LIST(GBL_DECL_VAR_PAIR, __VA_ARGS__)))

#define GBL_CONTEXT_EXT_IMPL_(name, memberFn, ...) \
    GBL_CONTEXT_EXT_DECL_(name, __VA_ARGS__) { \
        GBL_ASSERT(hCtx && pFrame); \
        GBL_RESULT result = GBL_RESULT_UNIMPLEMENTED; \
        if(hCtx->ext. memberFn) { \
            result = hCtx->ext.memberFn(pFrame GBL_VA_ARGS(GBL_MAP_LIST(GBL_DECL_VAR_PAIR_NAME, __VA_ARGS__))); \
        } \
        GBL_MACRO_CONDITIONAL_CALL(GBL_CONFIG_EXT_CONTEXT_PARENT_ENABLED, GBL_CONTEXT_EXT_IMPL_PARENT_CALL_, \
                   name, result, hCtx, pFrame GBL_VA_ARGS(GBL_MAP_LIST(GBL_DECL_VAR_PAIR_NAME, __VA_ARGS__)));    \
        GBL_MACRO_CONDITIONAL_CALL(GBL_CONFIG_EXT_CONTEXT_DEFAULT_ENABLED, GBL_CONTEXT_EXT_IMPL_DEFAULT_CALL_, \
                       name, result, hCtx, pFrame GBL_VA_ARGS(GBL_MAP_LIST(GBL_DECL_VAR_PAIR_NAME, __VA_ARGS__))); \
        return result; \
    }

GBL_CONTEXT_EXT_DECL_(LogWriteDefault_, (GBL_LOG_LEVEL, level), (const char*, pFmt), (va_list, varArgs)) {
    GBL_API_BEGIN(hCtx);
    GBL_API_VERIFY_POINTER(pFmt);
    GBL_API_VERIFY_ARG(level >= 0 /*&& level < GBL_LOG_LEVEL_COUNT*/); // or not to allow for user levels!

    char buffer[GBL_VA_SNPRINTF_BUFFER_SIZE] = { '\0' };
    char tabBuff[GBL_VA_SNPRINTF_BUFFER_SIZE];// = { '\t' };
    FILE* const pFile = (level >= GBL_LOG_LEVEL_ERROR)?
                stderr : stdout;
    const char* pPrefix = NULL;

    switch(level) {
    case GBL_LOG_LEVEL_WARNING: pPrefix = "! - "; break;
    case GBL_LOG_LEVEL_ERROR:   pPrefix = "X - "; break;
    case GBL_LOG_LEVEL_DEBUG:   pPrefix = "@ - "; break;
    default:                    pPrefix = "";     break;
    }

    const int vsnprintfBytes = vsnprintf(buffer, sizeof(buffer), pFmt, varArgs);
    if(vsnprintfBytes > (int)sizeof(buffer)) {
        pPrefix = "T - "; //Truncated prefix!
        GBL_API_RESULT_SET(GBL_RESULT_TRUNCATED, "Log message truncated!");
    }

    //not per byte!
    for(unsigned t = 0; t < hCtx->logStackDepth; ++t) {
        tabBuff[t] = '\t';
    }
    tabBuff[hCtx->logStackDepth] = '\0';
#if 0
    GBL_API_VERIFY((fprintf(pFile, "%s%s%s [%s:" GBL_SIZE_FMT " %s]\n",
                            tabBuff, pPrefix, buffer,
                            pFrame->sourceCurrent.pFile,
                            pFrame->sourceCurrent.line,
                            pFrame->sourceCurrent.pFunc)
                    >= 0), GBL_RESULT_ERROR_FILE_WRITE);
#else
    GBL_UNUSED(pFrame);
    GBL_API_VERIFY((fprintf(pFile, "%s%s%s\n",
                            tabBuff, pPrefix, buffer)
                    >= 0), GBL_RESULT_ERROR_FILE_WRITE);
#endif
    GBL_API_VERIFY(fflush(pFile) == 0, GBL_RESULT_ERROR_FILE_WRITE);

    GBL_API_END();
}


GBL_CONTEXT_EXT_DECL_(LogPopDefault_, (uint32_t, count)) {
    GBL_UNUSED(pFrame);
    GBL_API_BEGIN(hCtx);
    GBL_API_VERIFY(hCtx->logStackDepth >= count, GBL_RESULT_ERROR_UNDERFLOW);
    hCtx->logStackDepth -= count;
    GBL_API_END();
}

GBL_API gblContextLogPushDefault_(GblContext hCtx, const GblStackFrame* pFrame) {
    GBL_UNUSED(pFrame);
    GBL_API_BEGIN(hCtx);
    GBL_API_VERIFY(hCtx->logStackDepth + 1 > hCtx->logStackDepth, GBL_RESULT_ERROR_OVERFLOW);
    ++hCtx->logStackDepth;
    GBL_API_END();
}

GBL_CONTEXT_EXT_DECL_(MemAllocDefault_, (GblSize, size), (GblSize, alignment), (void**, ppData)) {
    GBL_UNUSED(pFrame);
    GBL_API_BEGIN(hCtx);
    GBL_API_VERIFY_ARG(size);
    GBL_API_VERIFY_ARG(alignment <= size && alignment >= 0);
    GBL_API_VERIFY_POINTER(ppData);
    *ppData = malloc(size);
    GBL_API_VERIFY(*ppData, GBL_RESULT_ERROR_MEM_ALLOC);
    GBL_API_DEBUG("Malloc[Size: " GBL_SIZE_FMT ", Align: " GBL_SIZE_FMT "] => %p [%s:" GBL_SIZE_FMT " %s]",
                  size, alignment, *ppData,
                  pFrame->sourceCurrent.pFile, pFrame->sourceCurrent.line, pFrame->sourceCurrent.pFunc);
    GBL_API_END();
}

GBL_CONTEXT_EXT_DECL_(MemReallocDefault_, (void*, pData), (GblSize, newSize), (GblSize, newAlign), (void**, ppNewData)) {
    GBL_UNUSED(pFrame);
    GBL_API_BEGIN(hCtx);
    GBL_API_VERIFY_ARG(newSize);
    GBL_API_VERIFY_ARG(newAlign <= newSize && newAlign >= 0);
    GBL_API_VERIFY_POINTER(pData);
    GBL_API_VERIFY_POINTER(ppNewData);
    *ppNewData = realloc(pData, newSize);
    GBL_API_VERIFY(*ppNewData, GBL_RESULT_ERROR_MEM_REALLOC);
    GBL_API_DEBUG("Realloc[Size: " GBL_SIZE_FMT ", Align: " GBL_SIZE_FMT "] %p => %p [%s:" GBL_SIZE_FMT " %s]",
                  newSize, newAlign, (uintptr_t)pData, *ppNewData,
                  pFrame->sourceCurrent.pFile, pFrame->sourceCurrent.line, pFrame->sourceCurrent.pFunc);
    GBL_API_END();
}


GBL_CONTEXT_EXT_DECL_(MemFreeDefault_, (void*, pData)) {
    GBL_UNUSED(pFrame);
    GBL_API_BEGIN(hCtx);
    const uintptr_t ptrVal = (uintptr_t)pData;
    free(pData);
    GBL_API_DEBUG("Free %p [%s:" GBL_SIZE_FMT " %s]",
                  ptrVal,
                  pFrame->sourceCurrent.pFile, pFrame->sourceCurrent.line, pFrame->sourceCurrent.pFunc);
    GBL_API_END();
}

GBL_CONTEXT_EXT_IMPL_(LogWrite, log.pFnWrite, (GBL_LOG_LEVEL, level), (const char*, pFmt), (va_list, varArgs))
GBL_CONTEXT_EXT_IMPL_(LogPop, log.pFnPop, (uint32_t, count))

GBL_CONTEXT_EXT_IMPL_(MemAlloc, mem.pFnAlloc, (GblSize, size), (GblSize, alignment), (void**, ppData))
GBL_CONTEXT_EXT_IMPL_(MemRealloc, mem.pFnRealloc, (void*, pData), (GblSize, newSize), (GblSize, newAlign), (void**, ppNewData))
GBL_CONTEXT_EXT_IMPL_(MemFree, mem.pFnFree, (void*, pData))

GBL_API gblContextLogPush(GblContext hCtx, const GblStackFrame* pFrame) {
    GBL_ASSERT(hCtx && pFrame);
    GBL_RESULT result = GBL_RESULT_UNIMPLEMENTED;
    if(hCtx->ext.log.pFnPush) {
        result = hCtx->ext.log.pFnPush(pFrame);
    }
#if GBL_CONFIG_EXT_CONTEXT_PARENT_ENABLED
    if(result == GBL_RESULT_UNIMPLEMENTED || result == GBL_RESULT_INCOMPLETE || result == GBL_RESULT_UNSUPPORTED) {
        GblContext hParent = gblContextParent_(hCtx);
        if(hParent) result = gblContextLogPush(hParent, pFrame);
    }
#endif
#if GBL_CONFIG_EXT_CONTEXT_DEFAULT_ENABLED
    if(result == GBL_RESULT_UNIMPLEMENTED || result == GBL_RESULT_INCOMPLETE || result == GBL_RESULT_UNSUPPORTED) {
        result = gblContextLogPushDefault_(hCtx, pFrame);
    }
#endif
    return result;
}


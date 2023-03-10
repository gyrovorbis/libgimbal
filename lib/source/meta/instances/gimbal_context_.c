#include <gimbal/objects/gimbal_context.h>
#include <gimbal/types/gimbal_typedefs.h>
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/core/gimbal_config.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>



typedef struct GblContext_ {
    GblHandle_                  baseHandle;

    struct {
        GblContextExtLog        log;
        GblContextExtMem        mem;
        GblContextExtApi        api;
    }                           ext;
#if GBL_CONFIG_EXT_CONTEXT_DEFAULT_ENABLED
    uint32_t logStackDepth;
    GblCallRecord               record;
#endif
    //GblError               lastError;
    //GblContextCreateInfo   createInfo;
    //GblApiCookie*          pApiCookieTop;
} GblContext_;


static GblContext_ defaultCtx = {
    .ext.log.pFnPush = NULL,
    .ext.log.pFnPop = NULL,
    .ext.mem.pFnAlloc = NULL,
    .ext.mem.pFnFree = NULL
};

static GblContext hGlobalCtx_ = &defaultCtx;

GblContext GblContext_globalGet(void) {
    return hGlobalCtx_;
}

GBL_API GblContext_globalSet(GblContext hCtx) {
    hGlobalCtx_ = hCtx;
    return GBL_RESULT_SUCCESS;
}


GBL_API gblContextVersion           (GblVersion* pVersion, const char** ppString) {
    if(!pVersion) return GBL_RESULT_ERROR_INVALID_POINTER;
    *pVersion = GBL_VERSION_MAKE(GBL_PROJECT_VERSION_MAJOR, GBL_PROJECT_VERSION_MINOR, GBL_PROJECT_VERSION_PATCH);
    if(ppString) *ppString = GBL_PROJECT_VERSION;
    return GBL_RESULT_SUCCESS;
}

static GBL_API gblContextBuildInfoLog_(GblContext hCtx) {
    GBL_API_BEGIN(hCtx, "Build Info");

    GBL_API_PUSH_VERBOSE("Project Info");
    GBL_API_VERBOSE("%-20s: %-100.100s", "Name", GBL_PROJECT_NAME);
    GBL_API_VERBOSE("%-20s: %-100.100s", "Version", GBL_PROJECT_VERSION);
    GBL_API_VERBOSE("%-20s: %-100.100s", "URL", GBL_PROJECT_URL);
    GBL_API_VERBOSE("%-20s: %-100.100s", "Description", GBL_PROJECT_DESCRIPTION);
    GBL_API_POP(1);

    GBL_API_PUSH_VERBOSE("Build Info");
    GBL_API_VERBOSE("%-20s: %-100.100s", "Timestamp", GBL_BUILD_TIMESTAMP);
    GBL_API_VERBOSE("%-20s: %-100.100s", "Type", GBL_BUILD_TYPE);
    GBL_API_VERBOSE("%-20s: %-100.100s", "Config", GBL_BUILD_CONFIG);
    GBL_API_POP(1);

    GBL_API_PUSH_VERBOSE("CI Info");
#if GBL_BUILD_CI
    GBL_API_VERBOSE("%-20s: %-100.100s", "Project", GBL_BUILD_CI_PROJECT_TITLE);

    GBL_API_PUSH_VERBOSE("Source Control Commit Info");
    GBL_API_VERBOSE("%-20s: %-100.100s", "Title", GBL_BUILD_CI_COMMIT_TITLE);
    GBL_API_VERBOSE("%-20s: %-100.100s", "Timestamp", GBL_BUILD_CI_COMMIT_TIMESTAMP);
    GBL_API_VERBOSE("%-20s: %-100.100s", "SHA", GBL_BUILD_CI_COMMIT_SHA);
    GBL_API_VERBOSE("%-20s: %-100.100s", "Tag", GBL_BUILD_CI_COMMIT_TAG);
    GBL_API_VERBOSE("%-20s: %-100.100s", "Ref", GBL_BUILD_CI_COMMIT_BRANCH);
    GBL_API_POP(1);

    GBL_API_PUSH_VERBOSE("Job Info");
    GBL_API_VERBOSE("%-20s: %-100.100s", "ID", GBL_BUILD_CI_JOB_ID);
    GBL_API_VERBOSE("%-20s: %-100.100s", "Name", GBL_BUILD_CI_JOB_NAME);
    GBL_API_VERBOSE("%-20s: %-100.100s", "Stage", GBL_BUILD_CI_JOB_STAGE);
    GBL_API_POP(1);

    GBL_API_PUSH_VERBOSE("User Info");
    GBL_API_VERBOSE("%-20s: %-100.100s", "Login", GBL_BUILD_CI_USER_LOGIN);
    GBL_API_VERBOSE("%-20s: %-100.100s", "Name", GBL_BUILD_CI_USER_NAME);
    GBL_API_VERBOSE("%-20s: %-100.100s", "Email Address", GBL_BUILD_CI_USER_EMAIL);
    GBL_API_POP(1);

    GBL_API_PUSH_VERBOSE("Build Node Info");
    GBL_API_VERBOSE("%-20s: %-100.100s", "Description", GBL_BUILD_CI_RUNNER_DESCRIPTION);
    GBL_API_VERBOSE("%-20s: %-100.100s", "Tags", GBL_BUILD_CI_RUNNER_TAGS);
    GBL_API_VERBOSE("%-20s: %-100.100s", "Architecture", GBL_BUILD_CI_RUNNER_ARCH);
    GBL_API_POP(1);
#else
    GBL_API_WARN("UNOFFICIAL LOCAL BUILD !");
#endif
    GBL_API_POP(1);

    GBL_API_PUSH_VERBOSE("Compiler Info");
    GBL_API_PUSH_VERBOSE("C");
    GBL_API_VERBOSE("%-20s: %-100.100s", "ID", GBL_BUILD_C_COMPILER_ID);
    GBL_API_VERBOSE("%-20s: %-100.100s", "Version", GBL_BUILD_C_COMPILER_VERSION);
    GBL_API_VERBOSE("%-20s: %-100.100s", "Target", GBL_BUILD_C_COMPILER_TARGET);
    GBL_API_VERBOSE("%-20s: %-100.100s", "Toolchain", GBL_BUILD_C_COMPILER_TOOLCHAIN);
    GBL_API_VERBOSE("%-20s: %-100.100s", "Language Standard", GBL_BUILD_C_STANDARD);
    GBL_API_VERBOSE("%-20s: %-100.100s", "Language Extensions", GBL_BUILD_C_EXTENSIONS);
    GBL_API_POP(1);

    GBL_API_PUSH_VERBOSE("C++");
    GBL_API_VERBOSE("%-20s: %-100.100s", "ID", GBL_BUILD_CPP_COMPILER_ID);
    GBL_API_VERBOSE("%-20s: %-100.100s", "Version", GBL_BUILD_CPP_COMPILER_VERSION);
    GBL_API_VERBOSE("%-20s: %-100.100s", "Target", GBL_BUILD_CPP_COMPILER_TARGET);
    GBL_API_VERBOSE("%-20s: %-100.100s", "Toolchain", GBL_BUILD_CPP_COMPILER_TOOLCHAIN);
    GBL_API_VERBOSE("%-20s: %-100.100s", "Language Standard", GBL_BUILD_CPP_STANDARD);
    GBL_API_VERBOSE("%-20s: %-100.100s", "Language Extensions", GBL_BUILD_CPP_EXTENSIONS);
    GBL_API_POP(1);
    GBL_API_POP(1);

    GBL_API_PUSH_VERBOSE("Environment Info");
    GBL_API_PUSH_VERBOSE("Host");
    GBL_API_VERBOSE("%-20s: %-100.100s", "Host Name", GBL_BUILD_HOST_NAME);
    //GBL_API_VERBOSE("%-20s: %-100.100s", "Domain Name", GBL_BUILD_HOST_DOMAIN_NAME);
    GBL_API_VERBOSE("%-20s: %-100.100s", "Operating System", GBL_BUILD_HOST_OS);
    GBL_API_VERBOSE("%-20s: %-100.100s", "Architecture", GBL_BUILD_HOST_ARCH);
    GBL_API_PUSH_VERBOSE("Processor");
    GBL_API_VERBOSE("%-20s: %-100.100s", "Family", GBL_BUILD_HOST_PROCESSOR_NAME);
    GBL_API_VERBOSE("%-20s: %-100.100s", "Description", GBL_BUILD_HOST_PROCESSOR_DESCRIPTION);
    GBL_API_VERBOSE("%-20s: %-100.u", "Physical Cores", GBL_BUILD_HOST_PROCESSOR_CORES_PHYSICAL);
    GBL_API_VERBOSE("%-20s: %-100.u", "Logical Cores", GBL_BUILD_HOST_PROCESSOR_CORES_LOGICAL);
    GBL_API_POP(1);
    GBL_API_PUSH_VERBOSE("Physical Memory");
    GBL_API_VERBOSE("%-20s: %-100u", "Total (MB)", GBL_BUILD_HOST_MEMORY_PHYSICAL_TOTAL);
    GBL_API_VERBOSE("%-20s: %-100u", "Available (MB)", GBL_BUILD_HOST_MEMORY_PHYSICAL_AVAILABLE);
    GBL_API_POP(1);
    GBL_API_POP(1);
    GBL_API_PUSH_VERBOSE("Target");
    GBL_API_VERBOSE("%-20s: %-100.100s", "Operating System", GBL_BUILD_TARGET_OS);
    GBL_API_VERBOSE("%-20s: %-100.100s", "Architecture", GBL_BUILD_TARGET_ARCH);
    GBL_API_VERBOSE("%-20s: %-100u", "Word Size", GBL_PTR_SIZE * 8);
    GBL_API_VERBOSE("%-20s: %-100.100s", "Endianness", GBL_BIG_ENDIAN? "Big" : "Little");
    GBL_API_POP(2);

    GBL_API_END();
}

GBL_INLINE GBL_RESULT gblContextInitializer(GblContext hCtx, const GblContextCreateInfo* pInfo) {
    if(!hCtx) return GBL_RESULT_ERROR_INVALID_HANDLE;
    memset(hCtx, 0, sizeof(GblContext_));
    hCtx->baseHandle.hContext       = hCtx;
    hCtx->baseHandle.hParent        = pInfo? pInfo->handleInfo.hParent          : NULL;
    hCtx->baseHandle.pUserdata      = pInfo? pInfo->handleInfo.pUserdata        : NULL;
    //hCtx->baseHandle.pFnEventHandler = pInfo? pInfo->handleInfo.pFnEventHandler : NULL;
    if(pInfo) {
        if(pInfo->pExtLog) memcpy(&hCtx->ext.log, pInfo->pExtLog, sizeof(GblContextExtLog));
        if(pInfo->pExtMem) memcpy(&hCtx->ext.mem, pInfo->pExtMem, sizeof(GblContextExtMem));

    }
    return GBL_RESULT_SUCCESS;
}

GBL_API gblContextConstruct (GblContext hCtx, const GblContextCreateInfo* pInfo) {
    GBL_API_BEGIN(hCtx, "Constructing Context");
    GblVersion verCur = 0;
    GBL_API_CALL(gblContextVersion(&verCur, NULL));
    GblVersion verMin = pInfo? pInfo->versionMin : 0;
    GblVersionInfo verCurInfo = GBL_VERSION_EXTRACT(verCur);
    GblVersionInfo verMinInfo = GBL_VERSION_EXTRACT(verMin);
    char verCurString[GBL_VERSION_STRING_SIZE_MAX] = { '\0' };
    char verMinString[GBL_VERSION_STRING_SIZE_MAX] = { '\0' };

    GBL_API_CALL(gblVersionInfoString(&verCurInfo, verCurString, sizeof(verCurString)));
    GBL_API_CALL(gblVersionInfoString(&verMinInfo, verMinString, sizeof(verMinString)));

    GBL_API_VERBOSE("Version Minimum: %s", verMinString);
    GBL_API_VERBOSE("Version Current: %s", verCurString);

    GBL_API_VERIFY(verCur >= verMin, GBL_RESULT_VERSION_MISMATCH);

    GBL_API_CALL(gblContextBuildInfoLog_(hCtx));

    GBL_API_END();
}

GBL_API gblContextCreate(GblContext* phCtx, const GblContextCreateInfo* pInfo) {
    // Temporary stack object so we can use the macro API without having to allocate shit first
    GblContext_ stackCtx;
    GblMemAllocInfo mallocInfo = { sizeof(GblContext_), GBL_ALIGNOF(GblContext_), "GblContext" };


    // Initializer for GlObject => GlHandle parents + GlContext properties
    const GBL_RESULT result = gblContextInitializer(&stackCtx, pInfo);
    if(!GBL_RESULT_SUCCESS(result)) {
        return result;
    }

    // Use the temp stack context to get access to the malloc EXT macro
    {
        //GBL_MEM_ALLOC_INFO_APPEND(&mallocInfo, pInfo->handleInfo.pAllocExtraSpaceInfo);
        if(pInfo->handleInfo.objectInfo.pExtraSpaceInfo) {
            mallocInfo.extraBytes += pInfo->handleInfo.objectInfo.pExtraSpaceInfo->extraBytes;
            if(pInfo->handleInfo.objectInfo.pExtraSpaceInfo->align > mallocInfo.align)
                mallocInfo.align = pInfo->handleInfo.objectInfo.pExtraSpaceInfo->align;
        }

        GBL_API_BEGIN(&stackCtx, "Creating Context");
        *phCtx = GBL_API_MALLOC(mallocInfo.extraBytes, mallocInfo.align, mallocInfo.pDebugStr);
        if(!GBL_RESULT_SUCCESS(GBL_API_RESULT())) {
            return GBL_API_RESULT();
        }

        if(!phCtx) {
            return GBL_RESULT_ERROR_MEM_ALLOC;
        }
    }
    // Reset the shit
    memset(*phCtx, 0, mallocInfo.extraBytes);
    // Copy the temp stack context into the new heap address
    memcpy(*phCtx, &stackCtx, sizeof(GblContext_));
    // Fix up the inner Context handle!
    (*phCtx)->baseHandle.hContext   = *phCtx; // have to fixup the damn self pointer!!!

    // Continue with the rest of the constructor
    {
        GBL_API_BEGIN(*phCtx);
        GBL_API_CALL(gblContextConstruct(*phCtx, pInfo));
        GBL_API_END();
    }
}

GBL_API gblContextDestroy(GblContext hCtx) {
    if(hCtx == GBL_HANDLE_INVALID) {
        GBL_ASSERT(hCtx);
        return GBL_RESULT_ERROR_INVALID_HANDLE;
    }

    // Copy over to a temp so we can still use the API...
    GblContext_ stackCtx;
    memcpy(&stackCtx, hCtx, sizeof(GblContext_));
    stackCtx.baseHandle.hContext = &stackCtx;

    {
        GBL_API_BEGIN(&stackCtx);
        //const GBL_RESULT result = gblHandleDestroy((GblHandle)hCtx);
        //GBL_API_VERIFY(GBL_RESULT_SUCCESS(result), result);
        GBL_API_FREE(hCtx);
        GBL_API_END();
    }
}

static inline GblContext gblContextParent_(GblContext hCtx) {
    GBL_ASSERT(hCtx);
    GblContext hCurrentCtx = hCtx;
    GBL_RESULT result = GBL_RESULT_SUCCESS;
    while(hCurrentCtx == hCtx) {
        GblHandle hHandle = GBL_HANDLE_INVALID;
        GBL_MAYBE_UNUSED result = gblHandleParentGet((GblHandle)hCurrentCtx, &hHandle);
        GBL_ASSERT(GBL_RESULT_SUCCESS(result));
        if(hHandle) {
            GBL_MAYBE_UNUSED result = gblHandleContext(hHandle, &hCurrentCtx);
            GBL_ASSERT(GBL_RESULT_SUCCESS(result));
        } else {
            hCurrentCtx = GBL_HANDLE_INVALID;
        }
    }
    return hCurrentCtx;
}

#define GBL_CONTEXT_EXT_IMPL_DEFAULT_CALL_(name, result, ...) \
    if(GBL_RESULT_UNAVAILABLE(result)) \
        result = gblContext##name##Default_(GBL_EVAL(__VA_ARGS__))

#define GBL_CONTEXT_EXT_IMPL_PARENT_CALL_(name, result, hCtx, ...) \
    if(hCtx && GBL_RESULT_UNAVAILABLE(result)) { \
        GblContext hParent = gblContextParent_(hCtx);   \
        if(hParent) result = gblContext##name (hParent GBL_VA_ARGS(__VA_ARGS__));   \
    }

#define GBL_CONTEXT_EXT_DECL_(name, ...) \
    GBL_API gblContext##name (GblContext hCtx, const GblStackFrame* pFrame GBL_VA_ARGS(GBL_MAP_LIST(GBL_DECL_VAR_PAIR, __VA_ARGS__)))

#define GBL_CONTEXT_EXT_IMPL_(name, memberFn, ...) \
    GBL_CONTEXT_EXT_DECL_(name, __VA_ARGS__) { \
        GBL_RESULT result = GBL_RESULT_UNIMPLEMENTED; \
        if(hCtx && hCtx->ext. memberFn) { \
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
    case GBL_LOG_LEVEL_WARNING: pPrefix = "! "; break;
    case GBL_LOG_LEVEL_ERROR:   pPrefix = "X "; break;
    case GBL_LOG_LEVEL_DEBUG:   pPrefix = "~ "; break;
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
    if(hCtx) {
        for(unsigned t = 0; t < hCtx->logStackDepth; ++t) {
            tabBuff[t] = '\t';
        }
        tabBuff[hCtx->logStackDepth] = '\0';
    } else {
        tabBuff[0] = '\0';
    }
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

GBL_CONTEXT_EXT_DECL_(MemAllocDefault_, (GblSize, size), (GblSize, alignment), (const char*, pDebugInfoStr), (void**, ppData)) {
    GBL_UNUSED(pFrame);
    GBL_API_BEGIN(hCtx);
    GBL_API_VERIFY_ARG(size);
    GBL_API_VERIFY_ARG(alignment <= size && alignment >= 0);
    GBL_API_VERIFY_POINTER(ppData);
    GBL_API_DEBUG("Malloc(Size: %" GBL_SIZE_FMT ", Align: %" GBL_SIZE_FMT ")", size, alignment);
    GBL_API_ERRNO_CLEAR();
    *ppData = GBL_ALLOC_ALIGNED(alignment, size);
    GBL_API_PUSH();
    GBL_API_PERROR("Malloc Failed");
    GBL_API_VERIFY(*ppData, GBL_RESULT_ERROR_MEM_ALLOC);
    GBL_API_DEBUG("%-20s: %20p", "Address", *ppData);
    GBL_API_DEBUG("%-20s: %20s", "Debug Marker", pDebugInfoStr? pDebugInfoStr : "NULL");
    GBL_API_DEBUG("%-20s: %20s", "Function", pFrame->sourceCurrent.pFunc);
    GBL_API_DEBUG("%-20s: %20"GBL_SIZE_FMT, "Line", pFrame->sourceCurrent.line);
    GBL_API_DEBUG("%-20s: %20s", "File", pFrame->sourceCurrent.pFile);
    GBL_API_POP(1);
    GBL_API_END();
}

GBL_CONTEXT_EXT_DECL_(MemReallocDefault_, (void*, pData), (GblSize, newSize), (GblSize, newAlign), (void**, ppNewData)) {
    GBL_UNUSED(pFrame);
    GBL_API_BEGIN(hCtx);
    GBL_API_VERIFY_ARG(newSize);
    GBL_API_VERIFY_ARG(newAlign <= newSize && newAlign >= 0);
    GBL_API_VERIFY_POINTER(pData);
    GBL_API_VERIFY_POINTER(ppNewData);
    const uintptr_t ptrVal = (uintptr_t)pData;
    GBL_API_DEBUG("Realloc(Size: %" GBL_SIZE_FMT ", Align: %" GBL_SIZE_FMT ") %p", newSize, newAlign, ptrVal);
    *ppNewData = realloc(pData, newSize);
    GBL_API_VERIFY(*ppNewData, GBL_RESULT_ERROR_MEM_REALLOC);
    GBL_API_PUSH();
    GBL_API_DEBUG("%-20s: %20p", "Address", *ppNewData);
    GBL_API_DEBUG("%-20s: %20s", "Function", pFrame->sourceCurrent.pFunc);
    GBL_API_DEBUG("%-20s: %20" GBL_SIZE_FMT, "Line", pFrame->sourceCurrent.line);
    GBL_API_DEBUG("%-20s: %20s", "File", pFrame->sourceCurrent.pFile);
    GBL_API_POP(1);

    GBL_API_END();
}


GBL_CONTEXT_EXT_DECL_(MemFreeDefault_, (void*, pData)) {
    GBL_UNUSED(pFrame);
    GBL_API_BEGIN(hCtx);
    const uintptr_t ptrVal = (uintptr_t)pData;
    free(pData);
    GBL_API_DEBUG("Free(%p)", ptrVal);
    GBL_API_PUSH();
    GBL_API_DEBUG("%-20s: %20s", "Function", pFrame->sourceCurrent.pFunc);
    GBL_API_DEBUG("%-20s: %20" GBL_SIZE_FMT, "Line", pFrame->sourceCurrent.line);
    GBL_API_DEBUG("%-20s: %20s", "File", pFrame->sourceCurrent.pFile);
    GBL_API_POP(1);
    GBL_API_END();
}

GBL_CONTEXT_EXT_DECL_(EventDefault_, (const GblEvent*, pEvent)) {
    GBL_UNUSED(pFrame);
    GBL_API_BEGIN(hCtx);
    GBL_API_VERIFY_POINTER(pEvent);
    GBL_API_INFO("Unhandled Event: %u [%s]", pEvent->eventType, gblEventTypeString(pEvent->eventType));
    GBL_API_END();
}


GBL_CONTEXT_EXT_IMPL_(LogWrite, log.pFnWrite, (GBL_LOG_LEVEL, level), (const char*, pFmt), (va_list, varArgs))
GBL_CONTEXT_EXT_IMPL_(LogPop, log.pFnPop, (uint32_t, count))

GBL_CONTEXT_EXT_IMPL_(MemAlloc, mem.pFnAlloc, (GblSize, size), (GblSize, alignment), (const char*, pDebugInfoStr), (void**, ppData))
GBL_CONTEXT_EXT_IMPL_(MemRealloc, mem.pFnRealloc, (void*, pData), (GblSize, newSize), (GblSize, newAlign), (void**, ppNewData))
GBL_CONTEXT_EXT_IMPL_(MemFree, mem.pFnFree, (void*, pData))

GBL_API gblContextEvent(GblContext hCtx, const GblStackFrame* pFrame, const GblEvent* pEvent) {
    return gblContextEventDefault_(hCtx, pFrame, pEvent);
}

GBL_API gblContextLogPush(GblContext hCtx, const GblStackFrame* pFrame) {
    GBL_ASSERT(hCtx && pFrame);
    GBL_RESULT result = GBL_RESULT_UNIMPLEMENTED;
    if(hCtx->ext.log.pFnPush) {
        result = hCtx->ext.log.pFnPush(pFrame);
    }
#if GBL_CONFIG_EXT_CONTEXT_PARENT_ENABLED
    if(GBL_RESULT_UNAVAILABLE(result)) {
        GblContext hParent = gblContextParent_(hCtx);
        if(hParent) result = gblContextLogPush(hParent, pFrame);
    }
#endif
#if GBL_CONFIG_EXT_CONTEXT_DEFAULT_ENABLED
    if(GBL_RESULT_UNAVAILABLE(result)) {
        result = gblContextLogPushDefault_(hCtx, pFrame);
    }
#endif
    return result;
}

GBL_API gblContextCallRecordGet(GblContext hCtx, const GblCallRecord** ppRecord) {
    GBL_API_BEGIN(hCtx);
    GBL_API_VERIFY_POINTER(ppRecord);
    *ppRecord = &hCtx->record;
    GBL_API_END();
}

GBL_API gblContextCallRecordSet(GblContext hCtx, const GblCallRecord* pRecord) {
    GBL_ASSERT(hCtx);
    if(pRecord) {
        memcpy(&hCtx->record, pRecord, sizeof(GblCallRecord));
    } else {
        memset(&hCtx->record, 0, sizeof(GblCallRecord));
    }
    return GBL_RESULT_SUCCESS;
}


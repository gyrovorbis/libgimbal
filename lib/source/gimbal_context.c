#include <gimbal/gimbal_context.h>



GBL_API gblContextInit(GblContext* pCtx,
                       GblContextCreateInfo* pInfo);


GBL_API gblContextDeinit(GblContext* pCtx);

GBL_API gblContextUserdata(const GblContext* pCtx,
                           void** pUd)
{
    *pUd = pCtx->pUserdata;
}

#if 0
GBL_API gblContextFromUserdata(const GblContextUd* pUd,
                                                GblContext** pCtx)
{
    *pCtx = (GblContext*)(((uintptr_t)pUd) - GBL_CONTEXT_USERDATA_OFFSET);
}
#endif


GBL_API gblContextApiCookieTop(const GblContext* pCtx,
                                          GblApiCookie** ppCookie)
{
    *ppCookie = pCtx->pApiCookieTop;
}

GBL_API gblContextApiCookiePush(GblContext* pCtx,
                                              GblApiCookie* pCookie)
{
    pCookie->pParent = pCtx->pApiCookieTop;
    pCtx->pApiCookieTop = pCookie;
}

GBL_API gblContextApiCookiePop(GblContext* pCtx) {

    pCtx->pApiCookieTop = pCtx->pApiCookieTop->pParent;
}
#if 0
GBL_API gblContextEnvTable(const GblContext* pctX, GblTable** ppEnv);
#endif


GBL_API gblContextLastError(const GblContext* pCtx,
                                             const GblError** ppError);


#if 0
typedef struct CtxDefaultUserdata_ {
    uint32_t logStackDepth;
} CtxDefaultUserdata_;

CtxDefaultUserdata_* ctxDefaultUserdata_(const GblContext* pCtx) {
    CtxDefaultUserdata_* pUd = NULL;

    if(pCtx) {
        pUd = (CtxDefaultUserdata_*)&pCtx->createInfo.pUserdata;
        EVMU_RESULT_CTX_FOLD(pCtx, HANDLER, pUd, INVALID_HANDLE,  "Failed to retrieve default UD ptr from context!");
    }

    return pUd;
}

EVMU_API EVMU_RESULT GblContextInit(GblContext* pCtx, GblContextCreateInfo* pInfo) {
    EVMU_RESULT_CTX_FOLD(pCtx, VALIDATE_HANDLE);
    EVMU_RESULT result = EVMU_RESULT_SUCCESS;

    memset(pCtx, 0, sizeof(GblContext));

    if(pInfo) {
        memcpy(&pCtx->createInfo, pInfo, sizeof(GblContextCreateInfo));
    }

    if(!EVMU_CTX_UD(pCtx)) {
        void* pPtr = NULL;
        result = evmuMalloc_(pCtx, sizeof(CtxDefaultUserdata_), 1, &pPtr);
        if(EVMU_RESULT_SUCCESS(result)) {
            memset(pPtr, 0, sizeof(CtxDefaultUserdata_));
            EVMU_CTX_UD(pCtx) = pPtr;
        }
    }

    return result;
}

EVMU_API void GblContextDeinit(GblContext* pCtx) {
    if(pCtx) {
        evmuFree_(pCtx, pCtx->ppDevices);
        pCtx->deviceCount = 0;
    }
}

EVMU_API EVMU_RESULT GblContextDeviceIndex(const GblContext* pCtx, uint32_t index, EVMUDevice** ppDevice) {
    EVMU_RESULT_VALIDATE_HANDLE(pCtx);
    EVMU_RESULT_CTX_FOLD(pCtx, VALIDATE_ARG, ppDevice);

    *ppDevice = (index < pCtx->deviceCount)?
                pCtx->ppDevices[index] : NULL;

    return EVMU_RESULT_SUCCESS;
}

EVMU_API EVMU_RESULT evmuLogWriteDefault(const GblContext* pCtx,
                                const char* pFileName,
                                const char* pFunction,
                                uint64_t line,
                                EVMU_LOG_LEVEL level,
                                const char* pFormat,
                                ...)
{
    EVMU_RESULT_VALIDATE_HANDLE(pCtx);
    EVMU_RESULT_CTX_FOLD(pCtx, VALIDATE_ARG, pFormat);
    EVMU_RESULT_CTX_FOLD(pCtx, VALIDATE_ARG, level, level >= 0 && level <= EVMU_LOG_LEVEL_ERROR);

    FILE* const pFile = (level >= EVMU_LOG_LEVEL_ERROR)?
                stderr : stdout;

    const char* pPrefix = NULL;

    EVMU_RESULT result = EVMU_RESULT_SUCCESS;

    switch(level) {
    case EVMU_LOG_LEVEL_WARNING: pPrefix = "! - "; break;
    case EVMU_LOG_LEVEL_ERROR:   pPrefix = "X - "; break;
    case EVMU_LOG_LEVEL_DEBUG:   pPrefix = "@ - "; break;
    default:                     pPrefix = "";     break;
    }

    EVMU_VA_SNPRINTF(pFormat);
    if(vsnprintfBytes > (int)sizeof(buffer)) {
        pPrefix = "T - "; //Truncated prefix!
        result = EVMU_RESULT_INCOMPLETE;
    }

    char tabBuff[EVMU_LOG_WRITE_DEFAULT_BUFFER_SIZE] = { '\t' };

    CtxDefaultUserdata_* pUd = ctxDefaultUserdata_(pCtx);

    EVMU_RESULT_CTX_FOLD(pCtx,
                         ASSIGN_GOTO,
                            pUd,
                            INVALID_HANDLE,
                            done,
                         "Failed to retrieve default context userdata!");

    tabBuff[pUd->logStackDepth] = '\0';

    EVMU_RESULT_ASSIGN_GOTO(pCtx,
                            (fprintf(pFile, "%s%s%s [%s:%zu %s]\n", tabBuff, pPrefix, buffer, pFileName, line, pFunction) >= 0),
                            FILE_WRITE,
                            done);

    EVMU_RESULT_ASSIGN_GOTO(pCtx, fflush(pFile) == 0, FILE_WRITE, done);

done:
    return result;
}


EVMU_API EVMU_RESULT evmuLogWrite_(const GblContext* pCtx,
                                   const char* pFile, const char* pFunction, uint64_t line,
                                   EVMU_LOG_LEVEL level, const char* pFormat, ...)
{
    EVMU_RESULT_VALIDATE_HANDLE(pCtx);
    EVMU_RESULT_VALIDATE_ARG(pCtx, pFormat);
    EVMU_RESULT_VALIDATE_ARG(pCtx, level, level >= 0 && level <= EVMU_LOG_LEVEL_ERROR);
    EVMU_RESULT_VALIDATE_ARG(pCtx, pFile);
    EVMU_RESULT_VALIDATE_ARG(pCtx, pFunction);

    EVMU_VA_SNPRINTF(pFormat);
    const EVMULogWriteFn pWrite = pCtx->createInfo.logCallbacks.pWriteFn;

    if(pWrite) {
        return pWrite(EVMU_CTX_UD(pCtx),
                    pFile, pFunction, line,
                    level, buffer);
    } else {
        return evmuLogWriteDefault(pCtx, pFile, pFunction, line, level, buffer);
    }
}

EVMU_API EVMU_RESULT evmuLogPush_(const GblContext* pCtx)
{
    EVMU_RESULT_VALIDATE_HANDLE(pCtx);

    EVMU_RESULT result = EVMU_RESULT_SUCCESS;
    const EVMULogPushFn pPush = pCtx->createInfo.logCallbacks.pPushFn;

    if(pPush) {
        return pPush(EVMU_CTX_UD(pCtx));
    } else {        CtxDefaultUserdata_* pUd = ctxDefaultUserdata_(pCtx);

        EVMU_RESULT_ASSIGN_GOTO(pCtx,
                                pUd,
                                INVALID_HANDLE,
                                done,
                                "Failed to retrieve default context userdata!");

        const uint32_t newDepth = pUd->logStackDepth + 1;
        EVMU_RESULT_ASSIGN_GOTO(pCtx, (newDepth > pUd->logStackDepth), LOG_PUSH_OVERFLOW, done, "Overflowing log stack!");
        ++pUd->logStackDepth;
    }
done:
    return result;
}


EVMU_API EVMU_RESULT evmuLogPop_(const GblContext* pCtx,
                                    uint32_t count)
{
    EVMU_RESULT_VALIDATE_HANDLE(pCtx);
    EVMU_RESULT_VALIDATE_ARG(pCtx, count);

    EVMU_RESULT result = EVMU_RESULT_SUCCESS;
    const EVMULogPopFn pPop = pCtx->createInfo.logCallbacks.pPopFn;

    if(pPop) {
        return pPop(EVMU_CTX_UD(pCtx), count);
    } else {
        CtxDefaultUserdata_* pUd = ctxDefaultUserdata_(pCtx);

        EVMU_RESULT_ASSIGN_GOTO(pCtx,
                                pUd,
                                INVALID_HANDLE,
                                end);
        EVMU_RESULT_ASSIGN_GOTO(pCtx, pUd->logStackDepth == 0, LOG_POP_UNDERFLOW, end, "Underflowing log stack!");

        --pUd->logStackDepth;

    }
end:
    return result;
}


EVMU_API EVMU_RESULT evmuMalloc_(const GblContext* pCtx,
                                    size_t size,
                                    size_t alignment,
                                    void** ppData)
{
    EVMU_RESULT_VALIDATE_HANDLE(pCtx);
    EVMU_RESULT_VALIDATE_ARG(pCtx, ppData);
    EVMU_RESULT_VALIDATE_ARG(size);
    EVMU_RESULT_VALIDATE_ARG(alignment);

    const EVMUMallocFn pMalloc = pCtx->createInfo.allocCallbacks.pMallocFn;
    if(pMalloc) {
        return pMalloc(EVMU_CTX_UD(pCtx), size, alignment, ppData);
    } else {
        EVMU_LOG(WARNING, "Malloc %zu bytes ignoring alignment: %zu", size, alignment);
        *ppData = malloc(size);
        EVMU_RESULT_RETURN(pCtx, ppData, MEM_MALLOC);
    }
    return EVMU_RESULT_SUCCESS;
}

EVMU_API EVMU_RESULT evmuRealloc_(const GblContext* pCtx,
                                     void* pExistingData,
                                     size_t newSize,
                                     size_t newAlignment,
                                     void* ppNewData)
{
    EVMU_RESULT_VALIDATE_HANDLE(pCtx);
    EVMU_RESULT_VALIDATE_ARG(pCtx, pExistingData);
    EVMU_RESULT_VALIDATE_ARG(pCtx, newSize);
    EVMU_RESULT_VALIDATE_ARG(pCtx, newAlignment);
    EVMU_RESULT_VALIDATE_ARG(pCtx, ppNewData);

    const EVMUReallocFn pRealloc = pCtx->createInfo.allocCallbacks.pReallocFn;
    if(pRealloc) {
        return pRealloc(EVMU_CTX_UD(pCtx), pExistingData, newSize, newAlignment, ppNewData);
    } else {
        EVMU_LOG(WARNING, "Realloc %zu bytes ignoring alignment: %zu", newSize, newAlignment);
        *ppData = realloc(pExistingData, newSize);
        EVMU_RESULT_RETURN(pCtx, ppData, MEM_REALLOC);
    }
    return EVMU_RESULT_SUCCESS;
}


EVMU_API EVMU_RESULT evmuFree_(const GblContext* pCtx,
                                  void* pData)
{
    EVMU_RESULT_VALIDATE_HANDLE(pCtx);
    const EVMUFreeFn pFree = pCtx->createInfo.allocCallbacks.pFreeFn;
    if(pFree) {
        return pFree(EVMU_CTX_UD(pCtx), pData);
    } else {
        free(pData);
    }
    return EVMU_RESULT_SUCCESS;
}
#endif

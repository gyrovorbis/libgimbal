#ifndef GIMBAL_CONTEXT_H
#define GIMBAL_CONTEXT_H

#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>

#include "gimbal_types.h"
#include "gimbal_ext.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GBL_CONTEXT_USERDATA_OFFSET (offsetof(GblContext, createInfo) + offsetof(GblContextCreateInfo, pUserdata))

typedef struct GblContextCreateInfo {
    void* pUserdata;

    struct {
        GblExtLogWriteFn      pFnWrite;
        GblExtLogPushFn       pFnPush;
        GblExtLogPopFn        pFnPop;
    }                      logCallbacks;

    struct {
        GblExtMallocFn        pFnMalloc;
        GblExtReallocFn       pFnRealloc;
        GblExtFreeFn          pFnFree;
    }                      allocCallbacks;

    GblVersion             versionMin;
} GblContextCreateInfo;


#define GBL_THREAD_NAME_SIZE 256

struct GblState;

typedef struct GblThread {
    char threadName[GBL_THREAD_NAME_SIZE];
    struct GblState* pState;
    GblApiCookie* pApiCookieTop;
    GblError* pLastError;
} GblThread;

// Actual top-level shared static state for all child contexts
typedef struct GblState {
    uint32_t contextCount;
    GblContext* pContexts;
    GblThread* pThread;
    GblApiCookie* pApiCookieTop;
} GblState;


typedef struct GblContext {

    GblError               lastError;
    GblThread*             pgblThread;
    void*                  pUserdata;

    GblContextCreateInfo   createInfo;
    GblApiCookie*          pApiCookieTop;
#if 0
    GblTable                envTable;
#endif
} GblContext;


/* Use GBL_VERSION_STRING for compile-time version.
 * Use GBLVersionString() for run-time version.
 */
GBL_API gblContextVersion(GblVersionInfo** ppVersionInfo);

GBL_API gblContextInit(GblContext* pCtx,
                       GblContextCreateInfo* pInfo);


GBL_API gblContextDeinit(GblContext* pCtx);


GBL_API gblContextUserdata(const GblContext* pCtx,
                           void** ppUd);

GBL_API gblContextApiCookieTop(const GblContext* pCtx,
                               GblApiCookie** ppCookie);

GBL_API gblContextApiCookiePush(GblContext* pCtx,
                                GblApiCookie* pCookie);

GBL_API gblContextApiCookiePop(GblContext* pCtx);


GBL_API gblContextLastError(const GblContext* pCtx,
                            const GblError** ppError);

GBL_API gblContextParent(const GblContext* pCtx,
                         GblContext* pParent);

GBL_API gblContextChild(const GblContext* pCtx,
                        uint32_t index,
                        GblContext* pChild);



// gblContextLog
//gblContextMalloc
//gblContextEtc




#if 0

GBL_API gblContextUpdateLastError_(GblContext* pCtx,
                                                 GblDevice* pDevice,
                                                 GBL_RESULT resultCode,
                                                 GBL_LOG_LEVEL logLevel,
                                                 const char* pFile,
                                                 const char* pFunc,
                                                 GblSize_t line,
                                                 const char* pFormat,
                                                 ...)
{
#ifdef GBL_RESULT_CONTEXT_TRACK_LAST_ERROR
    if(!pCtx) return GBL_RESULT_ERROR_INVALID_HANDLE;
    if(!pFormat) pFormat = "";

    GBL_RESULT result = GBL_RESULT_SUCCESS;
    GBLError* pError = &pCtx->lastError;

    pError->result  = resultCode;
    pError->pDevice = pDevice;
    pError->pFile   = pFile;
    pError->pFunc   = pFunc;
    pError->line    = line;

    va_list argptr;
    va_start(argptr, pFormat);

    if(!GBL_RESULT_SUCCESS(result)) {
        va_list argptr;
        va_start(argptr, pFormat);

        const int retVal = vsnprintf(pError->msg, sizeof(pError->msg), pFormat, argptr);
        va_end(argptr);

        if(retVal < 0) {
            result = GBL_RESULT_ERROR_VSNPRINTF;
        } else if((unsigned long)retVal > sizeof(pError->msg)) {
            result = GBL_RESULT_TRUNCATED;
        }

    } else {
        pError->msg[0] = '\0';
    }

    return result;
#else
    return GBL_RESULT_UNIMPLEMENTED;
#endif
}


#ifdef __cplusplus
}
#endif
#endif



#endif // GBL_CONTEXT_H

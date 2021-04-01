#ifndef GIMBAL_CONTEXT_H
#define GIMBAL_CONTEXT_H

#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>

#include "gimbal_handle.h"
#include "gimbal_types.h"
#include "gimbal_ext.h"

typedef GBL_RESULT (*GblExtVariantMetaObject)(GblContext, GBL_VARIANT_TYPE, const GblVariantMetaType**);

#ifdef __cplusplus
extern "C" {
#endif

#define GBL_CONTEXT_USERDATA_OFFSET (offsetof(GblContext, createInfo) + offsetof(GblContextCreateInfo, pUserdata))

typedef struct GblContextExtLog {
    GblExtLogWriteFn      pFnWrite;
    GblExtLogPushFn       pFnPush;
    GblExtLogPopFn        pFnPop;
} GblContextExtLog;

typedef struct GblContextExtMem {
    GblExtMemMallocFn          pFnMalloc;
    GblExtMemReallocFn         pFnRealloc;
    GblExtMemFreeFn            pFnFree;
} GblContextExtMem;

typedef struct GblContextExtApi {
    GblExtApiBeginFn        pFnBegin;
    GblExtApiEndFn          pFnEnd;
    GblExtApiLastErrorFn    pFnLastError;
} GblContextExtApi;

typedef struct GblContextCreateInfo {
    GblVersion          versionMin;
    void*               pUserdata;
    GblContextExtLog*   pExtLog;
    GblContextExtMem*   pExtMem;
    GblContextExtApi*   pExtApi;
} GblContextCreateInfo;


GBL_DECLARE_HANDLE(GblContext);

struct GblState;

typedef struct GblContext_ {
    GblHandle_                  baseHandle;

    struct {
        GblContextExtLog        log;
        GblContextExtMem        mem;
        GblContextExtApi        api;
    }                           ext;

    //GblError               lastError;
    //GblContextCreateInfo   createInfo;
    //GblApiCookie*          pApiCookieTop;
} GblContext_;


/* Use GBL_VERSION_STRING for compile-time version.
 * Use GBLVersionString() for run-time version.
 */


GBL_API gblContextVersion           (GblVersion* pVersion); //hard-compiled
GBL_API gblContextCreate            (GblContext* phCtx, const GblContextCreateInfo* pInfo);
GBL_API gblContextDestroy           (GblContext* phCtx);

// VIRTUALS
GBL_API gblContextExtLogWrite       (GblContext hCtx, GBL_LOG_LEVEL level, const char* pFmt, va_list varArgs);
GBL_API gblContextExtLogPush        (GblContext hCtx);
GBL_API gblContextExtLogPop         (GblContext hCtx, uint32_t count);

GBL_API gblContextExtMemMalloc      (GblContext hCtx, GblSize size, GblSize alignment, void** ppData);
GBL_API gblContextExtMemRealloc     (GblContext hCtx, const void* pData, GblSize newSize, GblSize newAlign, void** pNewData);
GBL_API gblContextExtMemFree        (GblContext hCtx, void* pData);

GBL_API gblContextExtApiBegin       (GblHandle hHandle);
GBL_API gblContextExtApiEnd         (GblHandle hHandle);
GBL_API gblContextExtApiLastError   (GblHandle hHandle, GblError** ppError);


#if 0

//Polymorphic API calls for gblContext

GBL_API gblContextMetaType(GblContext* pCtx, void*);
GBL_API gblContextExtMalloc();
GBL_API gblContextExtWrite();

#endif



#if 0
GBL_API gblContextVersion(GblVersionInfo** ppVersionInfo,
                          GblContext* phCtx);


GBL_API gblContextApiCookieTop(const GblContext* pCtx,
                               GblApiCookie** ppCookie);

GBL_API gblContextApiCookiePush(GblContext* pCtx,
                                GblApiCookie* pCookie);

GBL_API gblContextApiCookiePop(GblContext* pCtx);

GBL_API gblContextLastError(const GblContext* pCtx,
                            const GblError** ppError);
#endif


/* Should be inherited as a handle
GBL_API gblContextUserdata(const GblContext* pCtx,
                           void** ppUd);


GBL_API gblContextParent(const GblContext* pCtx,
                         GblContext* pParent);

GBL_API gblContextChild(const GblContext* pCtx,
                        uint32_t index,
                        GblContext* pChild);
*/


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


#endif

#ifdef __cplusplus
}
#endif

#endif // GBL_CONTEXT_H

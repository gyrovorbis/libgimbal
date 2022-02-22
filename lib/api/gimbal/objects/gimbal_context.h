#ifndef GIMBAL_CONTEXT_H
#define GIMBAL_CONTEXT_H

#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>

#include "gimbal_handle.h"
#include "../core/gimbal_ext.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GBL_CONTEXT_INVALID GBL_HANDLE_INVALID
//typedef GBL_RESULT (*GblExtVariantMetaObject)(GblContext, GBL_VARIANT_TYPE, const GblVariantMetaType**);

#define GBL_CONTEXT_USERDATA_OFFSET (offsetof(GblContext, createInfo) + offsetof(GblContextCreateInfo, pUserdata))

typedef struct GblContextExtLog {
    GblExtLogWriteFn      pFnWrite;
    GblExtLogPushFn       pFnPush;
    GblExtLogPopFn        pFnPop;
} GblContextExtLog;

typedef struct GblContextExtMem {
    GblExtMemAllocFn           pFnAlloc;
    GblExtMemReallocFn         pFnRealloc;
    GblExtMemFreeFn            pFnFree;
} GblContextExtMem;

typedef struct GblContextExtApi {
    GblExtApiBeginFn        pFnBegin;
    GblExtApiEndFn          pFnEnd;
    GblExtApiLastErrorFn    pFnLastError;
} GblContextExtApi;

// Let Contexts have parent contexts
typedef struct GblContextCreateInfo {
    GblVersion                  versionMin;
    GblHandleCreateInfo         handleInfo;
    const GblContextExtLog*     pExtLog;
    const GblContextExtMem*     pExtMem;
    const GblContextExtApi*     pExtApi;
    const GblExtEventHandlerFn  pEventHandlerFn;
} GblContextCreateInfo;



GBL_API gblContextVersion         (GblVersion* pVersion, const char** ppString); //hard-compiled
GBL_API gblContextCreate          (GblContext* phCtx, const GblContextCreateInfo* pInfo);
//GBL_API gblContextBuildInfo
GBL_API gblContextDestroy         (GblContext phCtx);

/* these are not really actually public, really.... maybe hide them? */
GBL_API gblContextLogWrite       (GblContext hCtx, const GblStackFrame* pFrame, GBL_LOG_LEVEL level, const char* pFmt, va_list varArgs);
GBL_API gblContextLogPush        (GblContext hCtx, const GblStackFrame* pFrame);
GBL_API gblContextLogPop         (GblContext hCtx, const GblStackFrame* pFrame, uint32_t count);

GBL_API gblContextMemAlloc       (GblContext hCtx, const GblStackFrame* pFrame, GblSize size, GblSize alignment, const char* pDebugInfoStr, void** ppData);
GBL_API gblContextMemRealloc     (GblContext hCtx, const GblStackFrame* pFrame, void* pData, GblSize newSize, GblSize newAlign, void** pNewData);
GBL_API gblContextMemFree        (GblContext hCtx, const GblStackFrame* pFrame, void* pData);
GBL_API gblContextEventHandler   (GblContext hCtx, const GblStackFrame* pFrame, const GblEvent* pEvent);

GBL_API gblContextApiBegin       (GblHandle hHandle);
GBL_API gblContextApiEnd         (GblHandle hHandle);

GBL_API gblContextCallRecordGet(GblContext hCtx, const GblCallRecord** ppRecord);
GBL_API gblContextCallRecordSet(GblContext hCtx, const GblCallRecord* pRecord);


#ifdef __cplusplus
}
#endif

#endif // GBL_CONTEXT_H
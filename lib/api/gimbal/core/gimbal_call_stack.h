#ifndef GIMBAL_CALL_STACK_H
#define GIMBAL_CALL_STACK_H

#include "../types/gimbal_typedefs.h"
#include "gimbal_state.h"
#include "gimbal_thread.h"
#include "../objects/gimbal_handle.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GblSourceLocation {
    const char*     pFile;
    const char*     pFunc;
    GblSize         line;
    GblSize         column;
} GblSourceLocation;

static inline GblSourceLocation GBL_SOURCE_LOCATION(const char* pFile, const char* pFunc, GBL_SIZE line, GBL_SIZE column) {
    const GblSourceLocation location = {
        pFile, pFunc, line, column
    };
    return location;
}

typedef struct GblMemAllocInfo {
    GblSize     extraBytes;
    GblSize     align;
    const char* pDebugStr;
} GblMemAllocInfo;

GBL_INLINE void GBL_MEM_ALLOC_INFO_APPEND(GblMemAllocInfo* pInfo1, const GblMemAllocInfo* pInfo2) {
    GBL_ASSERT(pInfo1);
    if(pInfo2) {
        pInfo1->extraBytes += pInfo2->extraBytes;
        if(pInfo2->align > pInfo1->align) pInfo1->align = pInfo2->align;
        if(pInfo2->pDebugStr) pInfo1->pDebugStr = pInfo2->pDebugStr;
    }
}

typedef struct GblCallRecord {
    char                message[GBL_API_RESULT_MSG_BUFFER_SIZE];
    GblSourceLocation   srcLocation;
    struct GblHandle_*  hHandle;
    GBL_RESULT          result;
} GblCallRecord;

GBL_MAYBE_UNUSED GBL_INLINE void GBL_CALL_RECORD_CONSTRUCT(GblCallRecord* pRecord, struct GblHandle_* hHandle, GBL_RESULT resultCode, GblSourceLocation source, const char* pFmt, ...) {
    va_list varArgs;
    va_start(varArgs, pFmt);
    memset(pRecord, 0, sizeof(GblCallRecord));
    if(pFmt) vsnprintf(pRecord->message, sizeof(pRecord->message), pFmt, varArgs);
    va_end(varArgs);
    pRecord->srcLocation    = source;
    pRecord->hHandle        = hHandle;
    pRecord->result         = resultCode;
}

#define SRC_FILE    GBL_SOURCE_FILE
#define SRC_FN      GBL_SOURCE_FUNCTION
#define SRC_LN      GBL_SOURCE_LINE
#define SRC_COL     GBL_SOURCE_COLUMN
#define SRC_LOC     GBL_SOURCE_LOCATION
#define SrcLoc      GblSourceLocation

typedef struct GblStackFrame {
    GblSourceLocation       sourceEntry;
    GblSourceLocation       sourceCurrent;
    GblCallRecord           record;
    uint32_t                sourceCurrentCaptureDepth;
    GblHandle               hHandle;
    GblContext              hContext;
    void*                   pHandleUd;
    void*                   pContextUd;
    uint32_t                stackDepth;
    const GblThread*        pThread;
    GblStackFrame*          pPrevFrame;
} GblStackFrame;


GBL_MAYBE_UNUSED GBL_INLINE GBL_RESULT GBL_API_STACK_FRAME_CONSTRUCT(GblStackFrame* pFrame, GblHandle hHandle, GBL_RESULT initialResult, GblSourceLocation entryLoc) {
    GBL_RESULT result               = GBL_RESULT_SUCCESS;
    GblContext hContext             = GBL_NULL;
    void* pHandleUserdata           = GBL_NULL;
    void* pContextUserdata          = GBL_NULL;

    if(!hHandle) {
        result = gblStateContextGet(&hContext);
    }

    if(hHandle) {
        result = gblHandleContext(hHandle, &hContext);
        GBL_ASSERT(GBL_RESULT_SUCCESS(result));
        result = gblHandleUserdata(hHandle, &pHandleUserdata);
        GBL_ASSERT(GBL_RESULT_SUCCESS(result));
        result = gblHandleUserdata((GblHandle)hContext, &pContextUserdata);
        GBL_ASSERT(GBL_RESULT_SUCCESS(result));
    }

    memset(pFrame, 0, sizeof(GblStackFrame));
    GBL_CALL_RECORD_CONSTRUCT(&pFrame->record, hHandle, initialResult, entryLoc, gblResultString(initialResult));
    pFrame->sourceEntry     = entryLoc;
    pFrame->sourceCurrent   = entryLoc;
    pFrame->hHandle         = hHandle;
    pFrame->hContext        = hContext;
    pFrame->pContextUd      = pContextUserdata;
    pFrame->pHandleUd       = pHandleUserdata;
    return result;
}

static inline GblBool GBL_API_STACK_FRAME_SOURCE_PUSH(GblStackFrame* pStackFrame, GblSourceLocation current) {
    if(++pStackFrame->sourceCurrentCaptureDepth == 1) { //we care about the first entry point
        pStackFrame->sourceCurrent = current;
        return GBL_TRUE;
    }
    return GBL_FALSE;
}

static inline GblBool GBL_API_STACK_FRAME_SOURCE_POP(GblStackFrame* pStackFrame) {
    GBL_ASSERT(pStackFrame->sourceCurrentCaptureDepth);
    if(--pStackFrame->sourceCurrentCaptureDepth == 0) {
        pStackFrame->sourceCurrent = pStackFrame->sourceEntry;
        return GBL_TRUE;
    }
    return GBL_FALSE;
}

#ifdef __cplusplus
}
#endif


#endif // GIMBAL_CALL_STACK_H

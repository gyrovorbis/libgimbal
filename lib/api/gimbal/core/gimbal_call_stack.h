#ifndef GIMBAL_CALL_STACK_H
#define GIMBAL_CALL_STACK_H

#include "../types/gimbal_typedefs.h"
#include "gimbal_thread.h"

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
    struct GblObject*   pObject;
    GBL_RESULT          result;
} GblCallRecord;

GBL_MAYBE_UNUSED GBL_INLINE void GBL_CALL_RECORD_CONSTRUCT(GblCallRecord* pRecord, struct GblObject* pObject, GBL_RESULT resultCode, GblSourceLocation source, const char* pFmt, ...) {
    va_list varArgs;
    va_start(varArgs, pFmt);
    memset(pRecord, 0, sizeof(GblCallRecord));
    if(pFmt) vsnprintf(pRecord->message, sizeof(pRecord->message), pFmt, varArgs);
    va_end(varArgs);
    pRecord->srcLocation    = source;
    pRecord->pObject        = pObject;
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
    GblCallRecord           lastFailure;
    uint32_t                sourceCurrentCaptureDepth;
    GblObject*              pObject;
    GblContext*             pContext;
    void*                   pObjectUd;
    void*                   pContextUd;
    uint32_t                stackDepth;
    const GblThread*        pThread;
    GblStackFrame*          pPrevFrame;
} GblStackFrame;


GBL_RESULT GBL_API_STACK_FRAME_CONSTRUCT(GblStackFrame* pFrame, GblObject* pObject, GBL_RESULT initialResult, GblSourceLocation entryLoc);

GBL_INLINE GblBool GBL_API_STACK_FRAME_SOURCE_PUSH(GblStackFrame* pStackFrame, GblSourceLocation current) {
    if(++pStackFrame->sourceCurrentCaptureDepth == 1) { //we care about the first entry point
        pStackFrame->sourceCurrent = current;
        return GBL_TRUE;
    }
    return GBL_FALSE;
}

GBL_INLINE GblBool GBL_API_STACK_FRAME_SOURCE_POP(GblStackFrame* pStackFrame) {
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

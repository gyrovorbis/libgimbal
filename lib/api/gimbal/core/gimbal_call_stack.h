/*! \file
 *  \brief Stack frame, call record, source capture debug utilities
 *  \ingroup debug
 */

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

#define GBL_SOURCE_LOCATION(FILE, FUNCTION, LINE, COL) ((GblSourceLocation){FILE, FUNCTION, LINE, COL})

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
    struct GblObject*       pObject;
    GblContext*             pContext;
    void*                   pObjectUd;
    void*                   pContextUd;
    uint32_t                stackDepth;
    const GblThread*        pThread;
    GblStackFrame*          pPrevFrame;
} GblStackFrame;


GBL_RESULT GBL_API_STACK_FRAME_CONSTRUCT(GblStackFrame* pFrame, struct GblObject* pObject, GBL_RESULT initialResult, GblSourceLocation entryLoc);


#define GBL_API_STACK_FRAME_SOURCE_PUSH(pStackFrame, current) \
    if(++pStackFrame->sourceCurrentCaptureDepth == 1) pStackFrame->sourceCurrent = current;

#define GBL_API_STACK_FRAME_SOURCE_POP(pStackFrame)                 \
    GBL_STMT_START {                                                \
        GBL_ASSERT(pStackFrame->sourceCurrentCaptureDepth);         \
        if(--pStackFrame->sourceCurrentCaptureDepth == 0)           \
            pStackFrame->sourceCurrent = pStackFrame->sourceEntry;  \
    } GBL_STMT_END

#ifdef __cplusplus
}
#endif


#endif // GIMBAL_CALL_STACK_H

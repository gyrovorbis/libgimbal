/*! \file
 *  \brief Stack frame, call record, source capture debug utilities
 *  \ingroup debug
 */

#ifndef GIMBAL_CALL_STACK_H
#define GIMBAL_CALL_STACK_H

#include "../core/gimbal_typedefs.h"
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

typedef struct GblCallRecord {
    GBL_ALIGNAS(128)
    char                message[GBL_API_RESULT_MSG_BUFFER_SIZE];
    GblSourceLocation   srcLocation;
    GBL_RESULT          result;
} GblCallRecord;

GBL_INLINE void GBL_CALL_RECORD_CONSTRUCT(GblCallRecord* pRecord, struct GblObject* pObject, GBL_RESULT resultCode, GblSourceLocation source, const char* pFmt, ...) GBL_NOEXCEPT {
    va_list varArgs;
    va_start(varArgs, pFmt);
    pRecord->result = GBL_RESULT_UNKNOWN;
    pRecord->message[0] = '\0';
    pRecord->srcLocation.pFile = pRecord->srcLocation.pFunc = GBL_NULL;
    if(pFmt) GBL_UNLIKELY vsnprintf(pRecord->message, sizeof(pRecord->message), pFmt, varArgs);
    va_end(varArgs);
    pRecord->srcLocation    = source;
    pRecord->result         = resultCode;
}

typedef struct GblStackFrame {
    GBL_ALIGNAS(256)
    GblSourceLocation       sourceEntry;
    GblSourceLocation       sourceCurrent;
    GblCallRecord           record;
    uint32_t                sourceCurrentCaptureDepth;
    GblObject*              pObject;
    GblContext*             pContext;
    uint32_t                stackDepth;
    GblStackFrame*          pPrevFrame;
} GblStackFrame;

GBL_EXPORT GblContext* GblObject_findContext(GblObject* pSelf) GBL_NOEXCEPT;

// I think this can all become a tiny macro
GBL_INLINE GBL_RESULT GBL_API_STACK_FRAME_CONSTRUCT(GblStackFrame* pFrame, GblObject* pObject, GBL_RESULT initialResult, GblSourceLocation entryLoc) GBL_NOEXCEPT {
    GBL_RESULT result               = GBL_RESULT_SUCCESS;
    GblContext* pContext            = GBL_NULL;

    if(pObject) GBL_UNLIKELY {
        const GblStackFrame* pPrev = GblThread_stackFrameTop(NULL);
        if(pPrev && pPrev->pObject == pObject) GBL_LIKELY {
            pContext = pPrev->pContext;
        } else GBL_UNLIKELY {
            pContext = GblObject_findContext(pObject);
        }
    }

    if(!pContext) GBL_LIKELY {
        pContext = GblThread_context(NULL);
    }

    pFrame->record.srcLocation          = entryLoc;
    pFrame->record.result               = initialResult;
    pFrame->stackDepth                  = 0;
    pFrame->sourceCurrentCaptureDepth   = 0;
    pFrame->sourceEntry                 = entryLoc;
    pFrame->sourceCurrent               = entryLoc;
    pFrame->pObject                     = pObject;
    pFrame->pContext                    = pContext;
    pFrame->pPrevFrame                  = NULL;
    return result;
}

#ifdef __cplusplus
}
#endif


#endif // GIMBAL_CALL_STACK_H

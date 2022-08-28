#include <gimbal/core/gimbal_thread.h>
#include <gimbal/meta/instances/gimbal_context.h>
#include <gimbal/core/gimbal_call_stack.h>

typedef struct GblThread_ {
    GblCallRecord   callRecord;
    const char*     pName;
    uint32_t        logStackDepth;
    GblContext*     pContext;
    GblStackFrame*  pStackFrameTop;
} GblThread_;

static GBL_THREAD_LOCAL GblThread_ thread_ = {
    .callRecord = {
        .message        = { '\0' },
        .srcLocation    = {
            .pFile      = NULL,
            .pFunc      = NULL,
            .line       = 0,
            .column     = 0
        },
        .result         = GBL_RESULT_UNKNOWN
    },
    .pName              = "Untitled",
    .logStackDepth      = 0,
    .pStackFrameTop     = NULL
};


GBL_EXPORT GblThread* GblThread_current(void) {
    return &thread_;
}

GblContext* GblThread_context(const GblThread* pThread) {
    GBL_UNUSED(pThread);
    //if(!pThread) GblThread_current((GblThread**)&pThread);
    return GblContext_global();
}

GBL_EXPORT const GblCallRecord* GblThread_callRecord(const GblThread* pThread) {
    if(!pThread) {
        pThread = GblThread_current();
    }
    return &pThread->callRecord;
}

GBL_API GblThread_setCallRecord(GblThread* pThread, const GblCallRecord* pRecord) {
    if(!pThread) pThread = GblThread_current();
    if(pRecord) {
        memcpy(&pThread->callRecord, pRecord, sizeof(GblCallRecord));
    } else {
        //memset(&pThread->callRecord, 0, sizeof(GblCallRecord));
        pThread->callRecord.result = GBL_RESULT_UNKNOWN;
    }
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT const char* GblThread_name(const GblThread* pThread) {
    if(!pThread) pThread = GblThread_current();
    return pThread->pName;
}

GBL_API GblThread_setName(GblThread* pThread, const char* pName) {
    GBL_ASSERT(pName);
    if(!pThread) pThread = GblThread_current();
    pThread->pName = pName;
    return GBL_RESULT_SUCCESS;
}

GBL_API GblThread_logPush(GblThread* pThread) {
    if(!pThread) pThread = GblThread_current();
    ++pThread->logStackDepth;
    return GBL_RESULT_SUCCESS;
}

GBL_API GblThread_logPop(GblThread* pThread, uint32_t count) {
    if(!pThread) pThread = GblThread_current();
    GBL_ASSERT(pThread->logStackDepth >= count);
    pThread->logStackDepth -= count;
    return GBL_RESULT_SUCCESS;
}

GBL_API GblThread_stackFramePush(GblThread* pThread, GblStackFrame* pFrame) {
    if(!pThread) pThread = GblThread_current();
    if(pFrame) {
        pFrame->pPrevFrame      = pThread->pStackFrameTop;
        if(GBL_RESULT_ERROR(pThread->callRecord.result)) {
            GblThread_setCallRecord(pThread,NULL);
            GBL_API_BEGIN(GblContext_global());
            GBL_API_WARN("Pushing new stack frame from [%s] with unhandled error record!",
                         !pFrame->sourceCurrent.pFunc? "UNKNOWN" : pFrame->sourceCurrent.pFunc);
            GBL_API_END_BLOCK();
        }
    }
    pThread->pStackFrameTop = pFrame;
    return GBL_RESULT_SUCCESS;
}
GBL_API GblThread_stackFramePop(GblThread* pThread) {
    if(!pThread) pThread = GblThread_current();
    GBL_ASSERT(pThread->pStackFrameTop);
    /*if(pThread->pStackFrameTop->pPrevFrame) {
        if(GBL_RESULT_ERROR(pThread->pStackFrameTop->record.result)) {
            memcpy(&pThread->pStackFrameTop->pPrevFrame->lastFailure,
                   &pThread->pStackFrameTop->record,
                   sizeof(GblCallRecord));
        }
    }*/
    GblStackFrame* pPrevFrame = pThread->pStackFrameTop;
    pThread->pStackFrameTop = pThread->pStackFrameTop? pThread->pStackFrameTop->pPrevFrame : NULL;
    pPrevFrame->pPrevFrame = NULL;
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT const GblStackFrame* GblThread_stackFrameTop(const GblThread* pThread) {
    if(!pThread) pThread = GblThread_current();
    return  pThread->pStackFrameTop;
}

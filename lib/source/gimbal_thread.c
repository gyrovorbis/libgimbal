#include <gimbal/core/gimbal_thread.h>
#include <gimbal/core/gimbal_call_stack.h>
#include <threads.h>

typedef struct GblThread_ {
    GblCallRecord   callRecord;
    const char*     pName;
    uint32_t        logStackDepth;
    GblContext*     pContext;
    GblStackFrame*  pStackFrameTop;
} GblThread_;

static thread_local GblThread_ thread_ = {
    .callRecord = {
        .message        = { '\0' },
        .srcLocation    = {
            .pFile      = NULL,
            .pFunc      = NULL,
            .line       = 0,
            .column     = 0
        },
        .hHandle        = GBL_HANDLE_INVALID,
        .result         = GBL_RESULT_UNKNOWN
    },
    .pName              = "Untitled",
    .logStackDepth      = 0,
    .pStackFrameTop     = NULL
};

GBL_API gblThreadCurrent(GblThread** ppThread) {
    GBL_ASSERT(ppThread);
    *ppThread = &thread_;
    return GBL_RESULT_SUCCESS;
}

GBL_API gblThreadContext(GblThread* pThread) {

}

GBL_API gblThreadCallRecordGet(const GblThread* pThread, const GblCallRecord** ppRecord) {
    GBL_ASSERT(ppRecord);
    if(!pThread) {
        gblThreadCurrent((GblThread**)&pThread);
    }
    *ppRecord = &pThread->callRecord;
    return GBL_RESULT_SUCCESS;
}

GBL_API gblThreadCallRecordSet(GblThread* pThread, const GblCallRecord* pRecord) {
    if(!pThread) gblThreadCurrent(&pThread);
    if(pRecord) {
        memcpy(&pThread->callRecord, pRecord, sizeof(GblCallRecord));
    } else {
        memset(&pThread->callRecord, 0, sizeof(GblCallRecord));
    }
    return GBL_RESULT_SUCCESS;
}

GBL_API gblThreadNameGet(const GblThread* pThread, const char** ppName) {
    GBL_ASSERT(ppName);
    if(!pThread) gblThreadCurrent((GblThread**)&pThread);
    *ppName = pThread->pName;
    return GBL_RESULT_SUCCESS;
}

GBL_API gblThreadNameSet(GblThread* pThread, const char* pName) {
    GBL_ASSERT(pName);
    if(!pThread) gblThreadCurrent(&pThread);
    pThread->pName = pName;
    return GBL_RESULT_SUCCESS;
}

GBL_API gblThreadLogPush(GblThread* pThread) {
    if(!pThread) gblThreadCurrent(&pThread);
    ++pThread->logStackDepth;
    return GBL_RESULT_SUCCESS;
}

GBL_API gblThreadLogPop(GblThread* pThread, uint32_t count) {
    if(!pThread) gblThreadCurrent(&pThread);
    GBL_ASSERT(pThread->logStackDepth >= count);
    pThread->logStackDepth -= count;
    return GBL_RESULT_SUCCESS;
}

GBL_API gblThreadStackFramePush(GblThread* pThread, GblStackFrame* pFrame) {
    if(!pThread) gblThreadCurrent(&pThread);
    GBL_ASSERT(pFrame);
    pFrame->pPrevFrame      = pThread->pStackFrameTop;
    pFrame->pThread         = pThread;
    pThread->pStackFrameTop = pFrame;
    return GBL_RESULT_SUCCESS;
}
GBL_API gblThreadStackFramePop(GblThread* pThread) {
    if(!pThread) gblThreadCurrent(&pThread);
    GBL_ASSERT(pThread->pStackFrameTop);
    pThread->pStackFrameTop = pThread->pStackFrameTop->pPrevFrame;
    return GBL_RESULT_SUCCESS;
}

GBL_API gblThreadStackFrameTop(const GblThread* pThread, const GblStackFrame** ppFrame) {
    if(!pThread) gblThreadCurrent((GblThread**)&pThread);
    GBL_ASSERT(ppFrame);
    *ppFrame = pThread->pStackFrameTop;
    return GBL_RESULT_SUCCESS;
}

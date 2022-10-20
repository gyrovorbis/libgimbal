#include <gimbal/core/gimbal_thread.h>
#include <gimbal/meta/instances/gimbal_context.h>
#include <gimbal/core/gimbal_call_stack.h>


GBL_THREAD_LOCAL GblThread thread_ = {
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
};


GblContext* GblThread_context(const GblThread* pThread) {
    GBL_UNUSED(pThread);
    //if(!pThread) GblThread_current((GblThread**)&pThread);
    return GblContext_global();
}

GBL_EXPORT GBL_RESULT GblThread_setCallRecord(GblThread* pThread, const GblCallRecord* pRecord) {
    if(!pThread) pThread = GblThread_current();
    if(pRecord) {
        //pThread->callRecord.result = pRecord->result;
        //strcpy(pThread->callRecord.message, pRecord->message);
        //memcpy(&pThread->callRecord.srcLocation, &pRecord->srcLocation, sizeof(GblSourceLocation));
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

GBL_EXPORT GBL_RESULT GblThread_setName(GblThread* pThread, const char* pName) {
    GBL_ASSERT(pName);
    if(!pThread) pThread = GblThread_current();
    pThread->pName = pName;
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT GblThread_logPush(GblThread* pThread) {
    if(!pThread) pThread = GblThread_current();
    ++pThread->logStackDepth;
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT GblThread_logPop(GblThread* pThread, uint32_t count) {
    if(!pThread) pThread = GblThread_current();
    GBL_ASSERT(pThread->logStackDepth >= count);
    pThread->logStackDepth -= count;
    return GBL_RESULT_SUCCESS;
}




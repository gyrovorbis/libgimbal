#include <gimbal/core/gimbal_thd.h>
#include <gimbal/meta/instances/gimbal_context.h>
#include <gimbal/core/gimbal_stack_frame.h>
#include <gimbal/core/gimbal_tls.h>

extern GblContext GblContext_default_;

static GblStackFrame defaultStackFrame_ = {
    .pContext = &GblContext_default_
};

static GBL_TLS(GblThd, GblThd_current_, ((const GblThd) {
    .callRecord = {
        .message        = { '\0' },
        .srcLocation    = {
            .pFile      = NULL,
            .pFunc      = NULL,
            .line       = 0
        },
        .result         = GBL_RESULT_UNKNOWN
    },
    .pName              = "Untitled",
    .logStackDepth      = 0,
    .pStackFrameTop     = &defaultStackFrame_
}));

GBL_EXPORT GblThd* GblThd_current(void) {
    return GBL_TLS_LOAD(GblThd_current_);
}

GblContext* GblThd_context(const GblThd* pThread) {
    GBL_UNUSED(pThread);
    //if(!pThread) GblThd_current((GblThd**)&pThread);
    return GblContext_global();
}

GBL_EXPORT GBL_RESULT GblThd_setCallRecord(GblThd* pThread, const GblCallRecord* pRecord) {
    if(!pThread) pThread = GblThd_current();
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

GBL_EXPORT const char* GblThd_name(const GblThd* pThread) {
    if(!pThread) pThread = GblThd_current();
    return pThread->pName;
}

GBL_EXPORT GBL_RESULT GblThd_setName(GblThd* pThread, const char* pName) {
    GBL_ASSERT(pName);
    if(!pThread) pThread = GblThd_current();
    pThread->pName = pName;
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT GblThd_logPush(GblThd* pThread) {
    if(!pThread) pThread = GblThd_current();
    ++pThread->logStackDepth;
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT GblThd_logPop(GblThd* pThread, uint32_t count) {
    if(!pThread) pThread = GblThd_current();
    GBL_ASSERT(pThread->logStackDepth >= count);
    pThread->logStackDepth -= count;
    return GBL_RESULT_SUCCESS;
}




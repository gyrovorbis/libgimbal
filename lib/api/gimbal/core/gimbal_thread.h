/*! \file
 *  \brief GblThread, per-thread state and context management
 *  \ingroup debug
 */

#ifndef GIMBAL_THREAD_H
#define GIMBAL_THREAD_H

#include <gimbal/core/gimbal_typedefs.h>

#include "gimbal_call_stack.h"
#define GBL_SELF_TYPE GblThread

GBL_DECLS_BEGIN

typedef struct GblThread {
    GblCallRecord   callRecord;
    const char*     pName;
    uint32_t        logStackDepth;
    GblContext*     pContext;
    GblStackFrame   bottomFrame;
    GblStackFrame*  pStackFrameTop;
    GblBool         initialized;
} GblThread;

extern GBL_THREAD_LOCAL GblThread thread_;

GBL_EXPORT GblContext* GblContext_global(void) GBL_NOEXCEPT;

GBL_INLINE GblThread* GblThread_current(void) {
    if(!thread_.initialized) {
        thread_.pStackFrameTop = &thread_.bottomFrame;
        thread_.bottomFrame.pContext = GblContext_global();
        thread_.initialized = GBL_TRUE;
    }
    return &thread_;
}


GBL_EXPORT GblContext*  GblThread_context        (GBL_CSELF)                        GBL_NOEXCEPT;

GBL_EXPORT const char*  GblThread_name           (GBL_CSELF)                        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblThread_setName        (GBL_SELF, const char* pName)      GBL_NOEXCEPT;

GBL_INLINE GblCallRecord*
                        GblThread_callRecord     (GBL_SELF)                        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblThread_setCallRecord  (GBL_SELF,
                                                  const GblCallRecord* pRecord)     GBL_NOEXCEPT;
//private
GBL_EXPORT GBL_RESULT   GblThread_logPush        (GBL_SELF)                         GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblThread_logPop         (GBL_SELF, uint32_t count)         GBL_NOEXCEPT;

GBL_INLINE GblStackFrame* GblThread_stackFrameTop(const GblThread* pThread) GBL_NOEXCEPT {
    if(!pThread) pThread = GblThread_current();
    return  pThread->pStackFrameTop;
}
//private


GBL_INLINE GBL_RESULT GblThread_stackFramePush(GblThread* pThread, GblStackFrame* pFrame) GBL_NOEXCEPT {
    if(!pThread) pThread = GblThread_current();
    if(pFrame) {
        pFrame->pPrevFrame      = pThread->pStackFrameTop;
        if(GBL_RESULT_ERROR(pThread->callRecord.result)) {
            GblThread_setCallRecord(pThread,NULL);
            //GBL_CTX_BEGIN(GblContext_global());
            //GBL_CTX_WARN("Pushing new stack frame from [%s] with unhandled error record!",
             //            !pFrame->sourceCurrent.pFunc? "UNKNOWN" : pFrame->sourceCurrent.pFunc);
           // GBL_CTX_END_BLOCK();
        }
    }
    pThread->pStackFrameTop = pFrame;
    return GBL_RESULT_SUCCESS;
}
GBL_INLINE GBL_RESULT GblThread_stackFramePop(GblThread* pThread) GBL_NOEXCEPT {
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

GBL_INLINE GblCallRecord* GblThread_callRecord(GblThread* pThread) {
    if(!pThread) {
        pThread = GblThread_current();
    }
    return &pThread->callRecord;
}

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_THREAD_H

/*! \file
 *  \brief GblThread, per-thread state and context management
 *  \ingroup core
 */

#ifndef GIMBAL_THREAD_H
#define GIMBAL_THREAD_H

#include <gimbal/core/gimbal_typedefs.h>

#include "gimbal_stack_frame.h"

#define GBL_SELF_TYPE GblThread

GBL_DECLS_BEGIN

typedef struct GblThread {
    GblCallRecord   callRecord;
    const char*     pName;
    uint32_t        logStackDepth;
    GblContext*     pContext;
    GblStackFrame*  pStackFrameTop;
    GblBool         initialized;
} GblThread;

// ===== Public API =====
GBL_INLINE GblThread*     GblThread_current        (void)                                   GBL_NOEXCEPT;
GBL_EXPORT GblContext*    GblThread_context        (GBL_CSELF)                              GBL_NOEXCEPT;

GBL_EXPORT const char*    GblThread_name           (GBL_CSELF)                              GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT     GblThread_setName        (GBL_SELF, const char* pName)            GBL_NOEXCEPT;

GBL_INLINE GblCallRecord* GblThread_callRecord     (GBL_SELF)                               GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT     GblThread_setCallRecord  (GBL_SELF, const GblCallRecord* pRecord) GBL_NOEXCEPT;

GBL_INLINE GblStackFrame* GblThread_stackFrameTop  (GBL_CSELF)                              GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT     GblThread_stackFramePush (GBL_SELF, GblStackFrame* pFrame)        GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT     GblThread_stackFramePop  (GBL_SELF)                               GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT     GblThread_logPush        (GBL_SELF)                               GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT     GblThread_logPop         (GBL_SELF, uint32_t count)               GBL_NOEXCEPT;

// ===== Implementation =====
///\cond
extern GBL_THREAD_LOCAL GblThread GblThread_current_;

GBL_EXPORT GblContext* GblContext_global(void) GBL_NOEXCEPT;
///\endcond

GBL_INLINE GblThread* GblThread_current(void) GBL_NOEXCEPT {
    return &GblThread_current_;
}

GBL_INLINE GblStackFrame* GblThread_stackFrameTop(const GblThread* pThread) GBL_NOEXCEPT {
    if(!pThread) pThread = GblThread_current();
    return  pThread->pStackFrameTop;
}

GBL_INLINE GBL_RESULT GblThread_stackFramePush(GblThread* pThread, GblStackFrame* pFrame) GBL_NOEXCEPT {
    if(!pThread) pThread = GblThread_current();
    if(pFrame) {
        pFrame->pPrevFrame      = pThread->pStackFrameTop;
        if(GBL_RESULT_ERROR(pThread->callRecord.result)) {
            GblThread_setCallRecord(pThread, GBL_NULL);
        }
    }
    pThread->pStackFrameTop = pFrame;
    return GBL_RESULT_SUCCESS;
}

GBL_INLINE GBL_RESULT GblThread_stackFramePop(GblThread* pThread) GBL_NOEXCEPT {
    if(!pThread) pThread = GblThread_current();
    GBL_ASSERT(pThread->pStackFrameTop);
    GblStackFrame* pPrevFrame = pThread->pStackFrameTop;
    pThread->pStackFrameTop = pThread->pStackFrameTop? pThread->pStackFrameTop->pPrevFrame : GBL_NULL;
    pPrevFrame->pPrevFrame = GBL_NULL;
    return GBL_RESULT_SUCCESS;
}

GBL_INLINE GblCallRecord* GblThread_callRecord(GblThread* pThread) GBL_NOEXCEPT {
    if(!pThread) {
        pThread = GblThread_current();
    }
    return &pThread->callRecord;
}

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_THREAD_H

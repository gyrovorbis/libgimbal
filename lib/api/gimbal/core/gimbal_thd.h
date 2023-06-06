/*! \file
 *  \brief GblThd, per-thread state and context management
 *  \ingroup core
 *  \todo
 *      - get rid of me
 *
 *  \author Falco Girgis
 */

#ifndef GIMBAL_THD_H
#define GIMBAL_THD_H

#include <tinycthread.h>
#include <gimbal/core/gimbal_typedefs.h>
#include "gimbal_stack_frame.h"

#define GBL_SELF_TYPE GblThd

GBL_DECLS_BEGIN

typedef struct GblThd {
    GblCallRecord   callRecord;
    const char*     pName;
    uint32_t        logStackDepth;
    GblContext*     pContext;
    GblStackFrame*  pStackFrameTop;
    GblBool         initialized;
} GblThd;

// ===== Public API =====
GBL_EXPORT GblThd*        GblThd_current        (void)                                   GBL_NOEXCEPT;
GBL_EXPORT GblContext*    GblThd_context        (GBL_CSELF)                              GBL_NOEXCEPT;

GBL_EXPORT const char*    GblThd_name           (GBL_CSELF)                              GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT     GblThd_setName        (GBL_SELF, const char* pName)            GBL_NOEXCEPT;

GBL_INLINE GblCallRecord* GblThd_callRecord     (GBL_SELF)                               GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT     GblThd_setCallRecord  (GBL_SELF, const GblCallRecord* pRecord) GBL_NOEXCEPT;

GBL_INLINE GblStackFrame* GblThd_stackFrameTop  (GBL_CSELF)                              GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT     GblThd_stackFramePush (GBL_SELF, GblStackFrame* pFrame)        GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT     GblThd_stackFramePop  (GBL_SELF)                               GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT     GblThd_logPush        (GBL_SELF)                               GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT     GblThd_logPop         (GBL_SELF, uint32_t count)               GBL_NOEXCEPT;

// ===== Implementation =====
///\cond

GBL_EXPORT GblContext* GblContext_global(void) GBL_NOEXCEPT;
///\endcond

GBL_INLINE GblStackFrame* GblThd_stackFrameTop(const GblThd* pThread) GBL_NOEXCEPT {
    if(!pThread) pThread = GblThd_current();
    return  pThread->pStackFrameTop;
}

GBL_INLINE GBL_RESULT GblThd_stackFramePush(GblThd* pThread, GblStackFrame* pFrame) GBL_NOEXCEPT {
    if(!pThread) pThread = GblThd_current();
    if(pFrame) {
        pFrame->pPrevFrame      = pThread->pStackFrameTop;
        if(GBL_RESULT_ERROR(pThread->callRecord.result)) {
            GblThd_setCallRecord(pThread, GBL_NULL);
        }
    }
    pThread->pStackFrameTop = pFrame;
    return GBL_RESULT_SUCCESS;
}

GBL_INLINE GBL_RESULT GblThd_stackFramePop(GblThd* pThread) GBL_NOEXCEPT {
    if(!pThread) pThread = GblThd_current();
    GBL_ASSERT(pThread->pStackFrameTop);
    GblStackFrame* pPrevFrame = pThread->pStackFrameTop;
    pThread->pStackFrameTop = pThread->pStackFrameTop? pThread->pStackFrameTop->pPrevFrame : GBL_NULL;
    pPrevFrame->pPrevFrame = GBL_NULL;
    return GBL_RESULT_SUCCESS;
}

GBL_INLINE GblCallRecord* GblThd_callRecord(GblThd* pThread) GBL_NOEXCEPT {
    if(!pThread) {
        pThread = GblThd_current();
    }
    return &pThread->callRecord;
}

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_THD_H

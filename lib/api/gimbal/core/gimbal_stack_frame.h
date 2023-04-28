/*! \file
 *  \brief Stack frame, call record, source capture debug utilities
 *  \ingroup debug
 */

#ifndef GIMBAL_CALL_STACK_H
#define GIMBAL_CALL_STACK_H

#include "../core/gimbal_result.h"
#include <stdarg.h>
#include <stdio.h>

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblContext);
GBL_FORWARD_DECLARE_STRUCT(GblObject);

typedef struct GblSourceLocation {
    const char*     pFile;
    const char*     pFunc;
    size_t          line;
} GblSourceLocation;

typedef struct GblCallRecord {
    GBL_ALIGNAS(64)
    char                message[GBL_CTX_RESULT_MSG_BUFFER_SIZE];
    GblSourceLocation   srcLocation;
    GBL_RESULT          result;
} GblCallRecord;

typedef struct GblStackFrame {
    GBL_ALIGNAS(64)
    GblCallRecord           record;
    uint32_t                sourceCurrentCaptureDepth;
    GblObject*              pObject;
    GblContext*             pContext;
    uint32_t                stackDepth;
    struct GblStackFrame*   pPrevFrame;
} GblStackFrame;

// ===== Public API =====
GBL_INLINE void       GblCallRecord_construct (GblCallRecord*    pRecord,
                                               GBL_RESULT        resultCode,
                                               GblSourceLocation source,
                                               const char*       pFmt, ...)  GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT GblStackFrame_construct (GblStackFrame* pFrame,
                                               GblObject*     pObject,
                                               GBL_RESULT     initialResult) GBL_NOEXCEPT;

// ===== Implementation =====
///\cond
GBL_FORWARD_DECLARE_STRUCT(GblThd);
GBL_FORWARD_DECLARE_STRUCT(GblContext);
GBL_INLINE GblThd*     GblThd_current         (void)                            GBL_NOEXCEPT;
GBL_EXPORT GblContext*    GblThd_context         (const GblThd* pSelf)          GBL_NOEXCEPT;
GBL_INLINE GblStackFrame* GblThd_stackFrameTop   (const GblThd* pSelf)          GBL_NOEXCEPT;
GBL_EXPORT GblContext*    GblObject_findContext     (GblObject* pSelf)                GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT     GblContext_memAlloc_      (GblContext* pSelf,
                                                     const GblStackFrame*  pFrame,
                                                     size_t                size,
                                                     size_t                align,
                                                     const char*           pDbgStr,
                                                     void**                ppData)    GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT     GblContext_memRealloc_    (GblContext* pSelf,
                                                     const GblStackFrame*  pFrame,
                                                     void*                 pData,
                                                     size_t                newSize,
                                                     size_t                newAlign,
                                                     void**                ppNewData) GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT     GblContext_memFree_       (GblContext* pSelf,
                                                     const GblStackFrame*  pFrame,
                                                     void*                 pData)     GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT     GblContext_logWrite_      (GblContext* pSelf,
                                                     const GblStackFrame*  pFrame,
                                                     GblFlags              level,
                                                     const char*           pFmt,
                                                     va_list               varArgs)   GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT     GblContext_logPush_       (GblContext* pSelf,
                                                     const GblStackFrame*  pFrame)    GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT     GblContext_logPop_        (GblContext* pSelf,
                                                     const GblStackFrame*  pFrame,
                                                     uint32_t              count)     GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT     GblContext_callRecordSet_ (GblContext* pSelf,
                                                     const GblStackFrame* pFrame,
                                                     const GblCallRecord* pRecord)    GBL_NOEXCEPT;

///\endcond

GBL_INLINE void GblCallRecord_construct(GblCallRecord* pRecord, GBL_RESULT resultCode, GblSourceLocation source, const char* pFmt, ...) GBL_NOEXCEPT {
    va_list varArgs;
    va_start(varArgs, pFmt);
    pRecord->result = GBL_RESULT_UNKNOWN;
    pRecord->message[0] = '\0';
    pRecord->srcLocation.pFile = pRecord->srcLocation.pFunc = GBL_NULL;
    if(pFmt) GBL_UNLIKELY vsprintf(pRecord->message, pFmt, varArgs);
    va_end(varArgs);
    pRecord->srcLocation    = source;
    pRecord->result         = resultCode;
}

GBL_INLINE GBL_RESULT GblStackFrame_construct(GblStackFrame* pFrame, GblObject* pObject, GBL_RESULT initialResult) GBL_NOEXCEPT {
    GBL_RESULT result               = GBL_RESULT_SUCCESS;
    GblContext* pContext            = GBL_NULL;

    if(pObject) GBL_UNLIKELY {
        const GblStackFrame* pPrev = GblThd_stackFrameTop(NULL);
        if(pPrev && pPrev->pObject == pObject) GBL_LIKELY {
            pContext = pPrev->pContext;
        } else GBL_UNLIKELY {
            pContext = GblObject_findContext(pObject);
        }
    }

    if(!pContext) GBL_LIKELY {
        pContext = GblThd_context(NULL);
    }

    pFrame->record.srcLocation.pFile    = GBL_NULL;
    pFrame->record.srcLocation.pFunc    = GBL_NULL;
    pFrame->record.result               = initialResult;
    pFrame->stackDepth                  = 0;
    pFrame->sourceCurrentCaptureDepth   = 0;
    pFrame->pObject                     = pObject;
    pFrame->pContext                    = pContext;
    pFrame->pPrevFrame                  = NULL;
    return result;
}

GBL_DECLS_END

#endif // GIMBAL_CALL_STACK_H

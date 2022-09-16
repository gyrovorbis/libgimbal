/*! \file
 *  \brief GblContext API management instance type
 *  \ingroup metaBuiltinTypes
 */

#ifndef GIMBAL_CONTEXT_H
#define GIMBAL_CONTEXT_H

#include "gimbal_object.h"
#include "../ifaces/gimbal_iallocator.h"
#include "../ifaces/gimbal_ilogger.h"

/// \ingroup metaBuiltinTypes
#define GBL_CONTEXT_TYPE                (GBL_TYPEOF(GblContext))

#define GBL_CONTEXT(instance)           (GBL_INSTANCE_CAST(instance, GblContext))
#define GBL_CONTEXT_CLASS(klass)        (GBL_CLASS_CAST(klass, GblContext))
#define GBL_CONTEXT_GET_CLASS(instance) (GBL_INSTANCE_GET_CLASS(instance, GblContext))

#define GBL_SELF_TYPE GblContext

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GblContext,
                       GblObject, GblIAllocator, GblILogger)

GBL_INSTANCE_DERIVE(GblContext, GblObject)
    GblCallRecord   lastIssue;
    uint32_t        logStackDepth;
    GblFlags        logFilter;
GBL_INSTANCE_END

GBL_PROPERTIES(GblContext,
    (result,  GBL_GENERIC, (READ), GBL_UINT32_TYPE),
    (message, GBL_GENERIC, (READ), GBL_POINTER_TYPE)
)

GBL_EXPORT GblType     GblContext_type             (void)                                  GBL_NOEXCEPT;
GBL_EXPORT GblContext* GblContext_global           (void)                                  GBL_NOEXCEPT;
GBL_EXPORT void        GblContext_setGlobal        (GblContext* pCtx)                      GBL_NOEXCEPT;

GBL_EXPORT const GblCallRecord*
                       GblContext_lastIssue        (GBL_CSELF)                             GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblContext_setLastIssue     (GBL_SELF, const GblCallRecord* pIssue) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblContext_clearLastIssue   (GBL_SELF)                              GBL_NOEXCEPT;

GBL_EXPORT GblBool     GblContext_hasIssue         (GBL_CSELF)                             GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblContext_hasError         (GBL_CSELF)                             GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblContext_lastIssueResult  (GBL_CSELF)                             GBL_NOEXCEPT;
GBL_EXPORT const char* GblContext_lastIssueMessage (GBL_CSELF)                             GBL_NOEXCEPT;

GBL_EXPORT GblContext* GblContext_parentContext    (GBL_CSELF)                             GBL_NOEXCEPT;
GBL_EXPORT void        GblContext_setLogFilter     (GBL_SELF, GblFlags mask)               GBL_NOEXCEPT;
GBL_EXPORT void        GblContext_logBuildInfo     (GBL_CSELF)                             GBL_NOEXCEPT;


/* ========== PRIVATE ==========
 * Private methods, do not call directly.
 *
 * Call via API frame macros like GBL_API_MALLOC(), GBL_API_FREE(), GBL_API_LOG(), GBL_API_PUSH(), etc
 * after calling GBL_API_BEGIN(GblContext* pCtx) with the context.
 */
/// \cond
GBL_EXPORT GBL_RESULT  GblContext_memAlloc_        (GBL_SELF,
                                                    const GblStackFrame*  pFrame,
                                                    GblSize               size,
                                                    GblSize               align,
                                                    const char*           pDbgStr,
                                                    void**                ppData)          GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblContext_memRealloc_      (GBL_SELF,
                                                    const GblStackFrame*  pFrame,
                                                    void*                 pData,
                                                    GblSize               newSize,
                                                    GblSize               newAlign,
                                                    void**                ppNewData)       GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblContext_memFree_         (GBL_SELF,
                                                    const GblStackFrame*  pFrame,
                                                    void*                 pData)           GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblContext_logWrite_        (GBL_SELF,
                                                    const GblStackFrame*  pFrame,
                                                    GBL_LOG_LEVEL         level,
                                                    const char*           pFmt,
                                                    va_list               varArgs)         GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblContext_logPush_         (GBL_SELF,
                                                    const GblStackFrame*  pFrame)          GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblContext_logPop_          (GBL_SELF,
                                                    const GblStackFrame*  pFrame,
                                                    uint32_t              count)           GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblContext_callRecordSet_   (GBL_SELF,
                                                    const GblStackFrame* pFrame,
                                                    const GblCallRecord* pRecord)          GBL_NOEXCEPT;

/// \endcond
GBL_DECLS_END

#undef GBL_SELF_TYPE


#endif // GIMBAL_CONTEXT_H

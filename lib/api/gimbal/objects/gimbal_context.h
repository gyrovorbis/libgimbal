#ifndef GIMBAL_CONTEXT_H
#define GIMBAL_CONTEXT_H

#include "gimbal_object.h"
#include "../ifaces/gimbal_iallocator.h"
#include "../ifaces/gimbal_ilogger.h"

#define GBL_CONTEXT_TYPE                (gblTypeBuiltin(26))
#define GBL_CONTEXT_STRUCT              GblContext
#define GBL_CONTEXT_CLASS_STRUCT        GblContextClass
#define GBL_CONTEXT(inst)               GBL_TYPE_CAST_INSTANCE_PREFIX (inst,  GBL_CONTEXT)
#define GBL_CONTEXT_CHECK(inst)         GBL_TYPE_CHECK_INSTANCE_PREFIX(inst,  GBL_CONTEXT)
#define GBL_CONTEXT_CLASS(klass)        GBL_TYPE_CAST_CLASS_PREFIX    (klass, GBL_CONTEXT)
#define GBL_CONTEXT_CLASS_CHECK(klass)  GBL_TYPE_CHECK_CLASS_PREFIX   (klass, GBL_CONTEXT)
#define GBL_CONTEXT_GET_CLASS(inst)     GBL_TYPE_CAST_GET_CLASS_PREFIX(inst,  GBL_CONTEXT)

#define SELF    GblContext* pSelf
#define CSELF   const SELF

GBL_DECLS_BEGIN

typedef struct GblContextClass {
    GblObjectClass      base;
    GblIAllocatorIFace  iAllocatorIFace;
    GblILoggerIFace     iLoggerIFace;
} GblContextClass;


typedef struct GblContext {
    union {
        GblContextClass*    pClass;
        GblObject           base;
    };
    GblCallRecord           lastIssue;
    uint32_t                logStackDepth;
} GblContext;


GBL_EXPORT const GblCallRecord*
                        GblContext_lastIssue        (CSELF)                                 GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblContext_lastIssueSet     (SELF, const GblCallRecord* pRecord)    GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblContext_lastIssueClear   (SELF)                                  GBL_NOEXCEPT;

GBL_EXPORT GblBool      GblContext_hasIssue         (CSELF)                                 GBL_NOEXCEPT;
GBL_EXPORT GblBool      GblContext_hasError         (CSELF)                                 GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblContext_lastIssueResult  (CSELF)                                 GBL_NOEXCEPT;
GBL_EXPORT const char*  GblContext_lastIssueMessage (CSELF)                                 GBL_NOEXCEPT;

GblContext*             GblContext_global(void)                                             GBL_NOEXCEPT;
void                    GblContext_globalSet(GblContext* pCtx)                              GBL_NOEXCEPT;


/* Private methods, do not call directly.
 *
 * Call via API frame macros like GBL_API_MALLOC(), GBL_API_FREE(), GBL_API_LOG(), GBL_API_PUSH(), etc
 * after calling GBL_API_BEGIN(GblContext* pCtx) with the context.
 */
GBL_API GblContext_memAlloc_     (SELF,
                                  const GblStackFrame*  pFrame,
                                  GblSize               size,
                                  GblSize               align,
                                  const char*           pDbgStr,
                                  void**                ppData)     GBL_NOEXCEPT;
GBL_API GblContext_memRealloc_   (SELF,
                                  const GblStackFrame*  pFrame,
                                  void*                 pData,
                                  GblSize               newSize,
                                  GblSize               newAlign,
                                  void**                ppNewData)  GBL_NOEXCEPT;
GBL_API GblContext_memFree_      (SELF,
                                  const GblStackFrame*  pFrame,
                                  void*                 pData)      GBL_NOEXCEPT;

GBL_API GblContext_logWrite_     (SELF,
                                  const GblStackFrame*  pFrame,
                                  GBL_LOG_LEVEL         level,
                                  const char*           pFmt,
                                  va_list               varArgs)    GBL_NOEXCEPT;
GBL_API GblContext_logPush_      (SELF,
                                  const GblStackFrame*  pFrame)     GBL_NOEXCEPT;
GBL_API GblContext_logPop_       (SELF,
                                  const GblStackFrame*  pFrame,
                                  uint32_t              count)      GBL_NOEXCEPT;

GBL_API GblContext_callRecordSet_(SELF,
                                  const GblStackFrame* pFrame,
                                  const GblCallRecord* pRecord)     GBL_NOEXCEPT;


GBL_EXPORT


GBL_DECLS_END

#undef CSELF
#undef SELF


#endif // GIMBAL_CONTEXT_H

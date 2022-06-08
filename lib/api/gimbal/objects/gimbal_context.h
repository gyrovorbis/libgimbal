#ifndef GIMBAL_CONTEXT_H
#define GIMBAL_CONTEXT_H

#include "gimbal_object.h"
#include "../ifaces/gimbal_iallocator.h"
#include "../ifaces/gimbal_ilogger.h"

#define GBL_CONTEXT_TYPE                (GBL_BUILTIN_TYPE(CONTEXT))
#define GBL_CONTEXT_STRUCT              GblContext
#define GBL_CONTEXT_CLASS_STRUCT        GblContextClass
#define GBL_CONTEXT(inst)               GBL_INSTANCE_CAST_PREFIX (inst,  GBL_CONTEXT)
#define GBL_CONTEXT_CHECK(inst)         GBL_INSTANCE_CHECK_PREFIX(inst,  GBL_CONTEXT)
#define GBL_CONTEXT_CLASS(klass)        GBL_CLASS_CAST_PREFIX    (klass, GBL_CONTEXT)
#define GBL_CONTEXT_CLASS_CHECK(klass)  GBL_CLASS_CHECK_PREFIX   (klass, GBL_CONTEXT)
#define GBL_CONTEXT_GET_CLASS(inst)     GBL_INSTANCE_CAST_CLASS_PREFIX(inst,  GBL_CONTEXT)

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

GBL_DECLARE_ENUM(GBL_CONTEXT_PROPERTY_ID) {
    GBL_CONTEXT_PROPERTY_ID_FIRST               = GBL_OBJECT_PROPERTY_ID_COUNT,
    GBL_CONTEXT_PROPERTY_ID_LAST_ISSUE_RESULT   = GBL_CONTEXT_PROPERTY_ID_FIRST,
    GBL_CONTEXT_PROPERTY_ID_LAST_ISSUE_MESSSAGE,
    GBL_CONTEXT_PROPERTY_ID_COUNT
};

GBL_PROPERTY_TABLE_BEGIN(GBL_CONTEXT)
    GBL_PROPERTY_ENTRY("result",
                       "Result Code",
                       "Encoded result value for last issue",
                       GBL_CONTEXT_PROPERTY_ID_LAST_ISSUE_RESULT,
                       GBL_UINT32_TYPE,
                       GBL_PROPERTY_FLAGS_MASK(READ))
    GBL_PROPERTY_ENTRY("message",
                       "Message",
                       "String message describing last issue",
                       GBL_CONTEXT_PROPERTY_ID_LAST_ISSUE_MESSSAGE,
                       GBL_POINTER_TYPE,
                       GBL_PROPERTY_FLAGS_MASK(READ))
GBL_PROPERTY_TABLE_END()


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

/* ========== PRIVATE ==========
 * Private methods, do not call directly.
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

/*! \file
 *  \brief GblContext API management instance type
 *  \ingroup meta
 *
 *  \deprecated This whole construct will soon be removed!
 *
 *  \author Falco Girgis
 */

#ifndef GIMBAL_CONTEXT_H
#define GIMBAL_CONTEXT_H

#include "gimbal_object.h"
#include "../ifaces/gimbal_iallocator.h"
#include "../ifaces/gimbal_ilogger.h"

/// \ingroup metaBuiltinTypes
#define GBL_CONTEXT_TYPE                (GBL_TYPEID(GblContext))
#define GBL_CONTEXT(instance)           (GBL_CAST(GblContext, instance))
#define GBL_CONTEXT_CLASS(klass)        (GBL_CLASS_CAST(GblContext, klass))
#define GBL_CONTEXT_GET_CLASS(instance) (GBL_CLASSOF(GblContext, instance))

#define GBL_SELF_TYPE GblContext

GBL_DECLS_BEGIN

/*! \struct GblContextClass
 *  \extends GblObjectClass
 *  \implements GblIAllocatorClass
 *  \implements GblILoggerClass
 *  \brief GblClass structure for GblContext
 *
 *  \deprecated
 *  \todo
 *      - remove me
 *
 *  \sa GblContext
 */
GBL_CLASS_DERIVE_EMPTY(GblContext,
                       GblObject, GblIAllocator, GblILogger)

/*! \struct GblContext
 *  \extends GblObject
 *  \implements GblIAllocator
 *  \implements GblILogger
 *  \brief      Top-level context object
 *
 *  \deprecated
 *  \todo
 *      -remove me
 *  \sa GblContextClass
 */
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

GBL_DECLS_END

#undef GBL_SELF_TYPE


#endif // GIMBAL_CONTEXT_H

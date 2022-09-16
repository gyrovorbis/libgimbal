/*! \file
 *  \brief GblThread, per-thread state and context management
 *  \ingroup debug
 */

#ifndef GIMBAL_THREAD_H
#define GIMBAL_THREAD_H

#include <gimbal/core/gimbal_typedefs.h>

#define GBL_SELF_TYPE GblThread

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT_PRIVATE(GblThread);

GBL_EXPORT GblThread*   GblThread_current        (void)                             GBL_NOEXCEPT;

GBL_EXPORT GblContext*  GblThread_context        (GBL_CSELF)                        GBL_NOEXCEPT;

GBL_EXPORT const char*  GblThread_name           (GBL_CSELF)                        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblThread_setName        (GBL_SELF, const char* pName)      GBL_NOEXCEPT;

GBL_EXPORT const GblCallRecord*
                        GblThread_callRecord     (GBL_CSELF)                        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblThread_setCallRecord  (GBL_SELF,
                                                  const GblCallRecord* pRecord)     GBL_NOEXCEPT;
//private
GBL_EXPORT GBL_RESULT   GblThread_logPush        (GBL_SELF)                         GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblThread_logPop         (GBL_SELF, uint32_t count)         GBL_NOEXCEPT;

GBL_EXPORT const GblStackFrame*
                        GblThread_stackFrameTop  (GBL_CSELF)                        GBL_NOEXCEPT;
//private
GBL_EXPORT GBL_RESULT   GblThread_stackFramePush (GBL_SELF, GblStackFrame* pFrame)  GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblThread_stackFramePop  (GBL_SELF)                         GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_THREAD_H

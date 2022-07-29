/*! \file
 *  \brief GblThread, per-thread state and context management
 *  \ingroup debug
 */

#ifndef GIMBAL_THREAD_H
#define GIMBAL_THREAD_H

#include <gimbal/types/gimbal_typedefs.h>

#define SELF    GblThread* pSelf
#define CSELF   const SELF

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT_PRIVATE(GblThread);

GBL_EXPORT GblThread*   GblThread_current        (void)                         GBL_NOEXCEPT;

GBL_EXPORT GblContext*  GblThread_context        (CSELF)                        GBL_NOEXCEPT;

GBL_EXPORT const char*  GblThread_name           (CSELF)                        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblThread_nameSet        (SELF, const char* pName)      GBL_NOEXCEPT;

GBL_EXPORT const GblCallRecord*
                        GblThread_callRecord     (CSELF)                        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblThread_callRecordSet  (SELF,
                                                  const GblCallRecord* pRecord) GBL_NOEXCEPT;
//private
GBL_EXPORT GBL_RESULT   GblThread_logPush        (SELF)                         GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblThread_logPop         (SELF, uint32_t count)         GBL_NOEXCEPT;

GBL_EXPORT const GblStackFrame*
                        GblThread_stackFrameTop  (CSELF)                        GBL_NOEXCEPT;
//private
GBL_EXPORT GBL_RESULT   GblThread_stackFramePush (SELF, GblStackFrame* pFrame)  GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblThread_stackFramePop  (SELF)                         GBL_NOEXCEPT;

GBL_DECLS_END

#undef CSELF
#undef SELF

#endif // GIMBAL_THREAD_H

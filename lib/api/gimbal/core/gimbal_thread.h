#ifndef GIMBAL_THREAD_H
#define GIMBAL_THREAD_H

#include <gimbal/types/gimbal_typedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

GBL_FORWARD_DECLARE_STRUCT_PRIVATE(GblThread);

GBL_API         gblThreadCurrent        (GblThread** pThread);

GblContext      gblThreadContext        (const GblThread* pThread);

GBL_API         gblThreadNameGet        (const GblThread* pThread,  const char** ppName);
GBL_API         gblThreadNameSet        (GblThread* pThread,        const char* pName);

GBL_API         gblThreadCallRecordGet  (const GblThread* pThread,  const GblCallRecord** ppRecord);
GBL_API         gblThreadCallRecordSet  (GblThread* pThread,        const GblCallRecord* pRecord);

GBL_API         gblThreadLogPush        (GblThread* pThread);
GBL_API         gblThreadLogPop         (GblThread* pThread,        uint32_t count);

GBL_API         gblThreadStackFramePush (GblThread* pThread,        GblStackFrame* pFrame);
GBL_API         gblThreadStackFramePop  (GblThread* pThread);
GBL_API         gblThreadStackFrameTop  (const GblThread* pThread,  const GblStackFrame** ppFrame);


#ifdef __cplusplus
}
#endif


#endif // GIMBAL_THREAD_H


#if 0
#ifndef GIMBAL_THREAD_H
#define GIMBAL_THREAD_H

#include <gimbal/types/gimbal_typedefs.h>

GBL_DECLS_BEGIN

#define SELF    GblThread* pSelf
#define CSELF   const SELF

GBL_FORWARD_DECLARE_STRUCT_PRIVATE(GblThread);

typedef int(*ThreadStartFn)(void*);

GBL_EXPORT GblContext           GblThread_context(CSELF);

GBL_EXPORT const char*          GblThread_nameGet(CSELF);
GBL_EXPORT void                 GblThread_nameSet(SELF, const char* pName);

GBL_EXPORT GblBool              GblThread_hasError(CSELF);
GBL_EXPORT GBL_RESULT           GblThread_result(CSELF);
GBL_EXPORT GblCallRecord*       GblThread_callRecordGet(CSELF);
GBL_EXPORT void                 GblThread_callRecordSet(SELF, const GblCallRecord* pRecord);

GBL_EXPORT void                 GblThread_logPush(SELF);
GBL_EXPORT void                 GblThread_logPop(SELF, uint32_t count);

GBL_EXPORT void                 GblThread_stackFramePush(SELF, GblStackFrame* pFrame);
GBL_EXPORT void                 GblThread_stackFramePop(SELF);
GBL_EXPORT const GblStackFrame* GblThread_stackFrameTop(CSELF);

/*
 *
 */
GBL_EXPORT GblThread*           GblThread_current(void);

GBL_EXPORT GblThread*           GblThread_create(ThreadStartFn pStart, void* pArg);

GBL_EXPORT GblBool              GblThread_equal(CSELF, const GblThread* pRhs);
GBL_EXPORT GBL_RESULT           GblThread_sleep(CSELF,
                                                const struct timespec*  duration,
                                                struct timespec*        remaining);
GBL_EXPORT void                 GblThread_yield(CSELF);
GBL_EXPORT GBL_NORETURN void    GblThread_exit(int result);
GBL_EXPORT GBL_RESULT           GblThread_detach(CSELF);
GBL_EXPORT GBL_RESULT           GblThread_join(CSELF, int* pResult);


#undef CSELF
#undef SELF

GBL_DECLS_END


#endif // GIMBAL_THREAD_H

#endif

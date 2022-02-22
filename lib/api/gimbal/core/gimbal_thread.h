#ifndef GIMBAL_THREAD_H
#define GIMBAL_THREAD_H

#include <gimbal/types/gimbal_typedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

GBL_FORWARD_DECLARE_STRUCT_PRIVATE(GblThread);

GBL_API gblThreadCurrent        (GblThread** pThread);

GBL_API gblThreadNameGet        (const GblThread* pThread,  const char** ppName);
GBL_API gblThreadNameSet        (GblThread* pThread,        const char* pName);

GBL_API gblThreadCallRecordGet  (const GblThread* pThread,  const GblCallRecord** ppRecord);
GBL_API gblThreadCallRecordSet  (GblThread* pThread,        const GblCallRecord* pRecord);

GBL_API gblThreadLogPush        (GblThread* pThread);
GBL_API gblThreadLogPop         (GblThread* pThread,        uint32_t count);

GBL_API gblThreadStackFramePush (GblThread* pThread,        GblStackFrame* pFrame);
GBL_API gblThreadStackFramePop  (GblThread* pThread);
GBL_API gblThreadStackFrameTop  (const GblThread* pThread,  const GblStackFrame** ppFrame);


#ifdef __cplusplus
}
#endif


#endif // GIMBAL_THREAD_H

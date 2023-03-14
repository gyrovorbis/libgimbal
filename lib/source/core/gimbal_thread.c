#include <gimbal/core/gimbal_thread.h>

#ifdef _WIN32
#   define NOGDI
# else
#   define __USE_UNIX98
#endif
#include <tinycthread.h>

GBL_THREAD_LOCAL GblThread* pCurThread_ = NULL;

GBL_EXPORT GblThread* GblThread_current(void) {
    if(!pCurThread_) {
        pCurThread_ = GBL_OBJECT_NEW(GblThread);
    }
    return pCurThread_;
}



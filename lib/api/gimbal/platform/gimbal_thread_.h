#ifndef GIMBAL_THREAD__H
#define GIMBAL_THREAD__H

#include "../core/gimbal_thread.h"

#define GBL_SELF_TYPE uintptr_t

typedef uintptr_t GblThreadId;

GBL_EXPORT GblThreadId GblThread__current     (void)                                GBL_NOEXCEPT;

GBL_EXPORT GblThreadId GblThread__create      (GblThreadFn pFnRun, size_t stackSz)  GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblThread__destroy     (GBL_VSELF)                           GBL_NOEXCEPT;

GBL_EXPORT const char* GblThread__name        (GBL_VSELF)                           GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblThread__setName     (GBL_VSELF, const char* pName)        GBL_NOEXCEPT;

GBL_EXPORT GBL_THREAD_PRIORITY
                       GblThread__priority    (GBL_VSELF)                           GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblThread__setPriority (GBL_VSELF, GBL_THREAD_PRIORITY prio) GBL_NOEXCEPT;

GBL_EXPORT GblThreadAffinity
                       GblThread__affinity    (GBL_VSELF)                           GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblThread__setAffinity (GBL_VSELF, GblThreadAffinity affin)  GBL_NOEXCEPT;


#endif // GIMBAL_THREAD__H

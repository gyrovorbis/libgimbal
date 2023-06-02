/*! \file
 *  \brief GblThread and lowest-level concurrency managment
 *  \ingroup core
 *  \todo
 *   - properties
 *   - GblThread_setCallback() has to store within GblBox field
 *   - GblThread_setClosure() has to store within GblBox field WITH DTOR
 *   - GblArrayMap for underlying structure doesn't make sensse, use GblArrayList
 *   - need code for adding and removing to/from vector
 *   - checking for auto invocation upon constructed won't work--hasn't been set yet
 *
 *   \author Falco Girgis
 */
#ifndef GIMBAL_THREAD_H
#define GIMBAL_THREAD_H

#include "../meta/instances/gimbal_object.h"
#include "../meta/signals/gimbal_signal.h"
#include "gimbal_stack_frame.h"
#include "gimbal_main_loop.h"
#include "../allocators/gimbal_scope_allocator.h"
#include <signal.h>

#define GBL_THREAD_TYPE             (GBL_TYPEOF(GblThread))
#define GBL_THREAD(self)            (GBL_INSTANCE_CAST(self, GblThread))
#define GBL_THREAD_CLASS(klass)     (GBL_CLASS_CAST(klass, GblThread))
#define GBL_THREAD_GET_CLASS(self)  (GBL_INSTANCE_GET_CLASS(self, GblThread))

#define GBL_SELF_TYPE GblThread

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblTime);
GBL_FORWARD_DECLARE_STRUCT(GblThread);

typedef GBL_RESULT (*GblThreadFn)    (GBL_SELF);
typedef GblBool    (*GblThreadIterFn)(GBL_SELF, void* pClosure);
typedef uintptr_t   GblThreadAffinity;

GBL_DECLARE_ENUM(GBL_THREAD_PRIORITY) {
    GBL_THREAD_PRIORITY_LOW,
    GBL_THREAD_PRIORITY_MEDIUM,
    GBL_THREAD_PRIORITY_HIGH,
    GBL_THREAD_PRIORITY_REAL_TIME
};

GBL_DECLARE_ENUM(GBL_THREAD_STATE) {
    GBL_THREAD_STATE_UNKNOWN,
    GBL_THREAD_STATE_INITIALIZING,
    GBL_THREAD_STATE_READY,
    GBL_THREAD_STATE_RUNNING,
    GBL_THREAD_STATE_FINISHED
};

GBL_CLASS_DERIVE(GblThread, GblObject)
    GblThreadFn pFnRun;
    GBL_RESULT  (*pFnSignal)(GBL_SELF, int signal);
GBL_CLASS_END

GBL_INSTANCE_DERIVE(GblThread, GblObject)
    GblCallRecord         returnStatus;
    volatile sig_atomic_t signalStatus;
    GBL_THREAD_STATE      state;
GBL_INSTANCE_END

GBL_PROPERTIES(GblThread,
//    (name,      GBL_GENERIC, (READ, WRITE, OVERRIDE), GBL_STRING_TYPE),
    (result,    GBL_GENERIC, (READ                 ), GBL_ENUM_TYPE),
    (state,     GBL_GENERIC, (READ                 ), GBL_ENUM_TYPE),
    (signal,    GBL_GENERIC, (READ                 ), GBL_ENUM_TYPE),
    (joined,    GBL_GENERIC, (READ                 ), GBL_BOOL_TYPE),
    (priority,  GBL_GENERIC, (      WRITE          ), GBL_ENUM_TYPE),
    (affinity,  GBL_GENERIC, (      WRITE          ), GBL_FLAGS_TYPE),
    (closure,   GBL_GENERIC, (READ, WRITE          ), GBL_CLOSURE_TYPE),
    (callback,  GBL_GENERIC, (READ, WRITE          ), GBL_FUNCTION_TYPE)
)

GBL_SIGNALS(GblThread,
    (started,  (GBL_INSTANCE_TYPE, pReceiver)),
    (finished, (GBL_INSTANCE_TYPE, pReceiver), (GBL_ENUM_TYPE, result)),
    (signaled, (GBL_INSTANCE_TYPE, pReceiver), (GBL_ENUM_TYPE, signal))
)

// ===== Static Methods =====
GBL_EXPORT GblType     GblThread_type        (void)                           GBL_NOEXCEPT;
GBL_EXPORT size_t      GblThread_count       (void)                           GBL_NOEXCEPT;
GBL_EXPORT GblThread*  GblThread_find        (const char* pName)              GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblThread_foreach     (GblThreadIterFn pIt,
                                              void*           pCl/*=NULL*/)   GBL_NOEXCEPT;

// ===== Instance Methods for Parent Thread =====
GBL_EXPORT GblThread*  GblThread_create      (GblThreadFn pCallback,
                                              void*       pUserdata/*=NULL*/,
                                              GblBool     autoStart/*=0*/)    GBL_NOEXCEPT;

GBL_EXPORT GblRefCount GblThread_unref       (GBL_SELF)                       GBL_NOEXCEPT;

GBL_EXPORT GblBool     GblThread_isJoined    (GBL_CSELF)                      GBL_NOEXCEPT;

GBL_EXPORT const char* GblThread_name        (GBL_CSELF)                      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblThread_setName     (GBL_SELF, const char* pName)    GBL_NOEXCEPT;

GBL_EXPORT GblClosure* GblThread_closure     (GBL_CSELF)                      GBL_NOEXCEPT;
GBL_EXPORT void        GblThread_setClosure  (GBL_SELF, GblClosure* pClosure) GBL_NOEXCEPT;

GBL_EXPORT GblThreadFn GblThread_callback    (GBL_CSELF)                      GBL_NOEXCEPT;
GBL_EXPORT void        GblThread_setCallback (GBL_SELF, GblThreadFn pCb)      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblThread_setPriority (GBL_SELF,
                                              GBL_THREAD_PRIORITY priority)   GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblThread_setAffinity (GBL_SELF,
                                              GblThreadAffinity affinity)     GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblThread_start       (GBL_SELF)                       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblThread_join        (GBL_SELF)                       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblThread_spinWait    (GBL_SELF/*,
                                              const GblTimeSpec* pTimeout*/)  GBL_NOEXCEPT;
// ===== Instance Methods for Child Thread =====
GBL_EXPORT GblThread*  GblThread_current     (void)                           GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblThread_yield       (void)                           GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblThread_nanoSleep   (uint64_t  nsec,
                                              uint64_t* pRemainder/*=NULL*/)  GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblThread_exit        (GBL_RESULT  result,
                                              const char* pMessage/*=NULL*/,
                                              const char* pFile   /*=NULL*/,
                                              const char* pFunc   /*=NULL*/,
                                              size_t      line    /*=0*/)     GBL_NOEXCEPT;

// ===== Overloaded Function Macros =====
#define                GblThread_create(...)    GBL_VA_OVERLOAD_CALL_ARGC(GblThread_create_, __VA_ARGS__)
#define                GblThread_exit(...)      GBL_VA_OVERLOAD_CALL_ARGC(GblThread_exit_, __VA_ARGS__)
#define                GblThread_nanoSleep(...) GBL_VA_OVERLOAD_CALL_ARGC(GblThread_nanoSleep_, __VA_ARGS__)
#define                GblThread_foreach(...)   GBL_VA_OVERLOAD_CALL_ARGC(GblThread_foreach_, __VA_ARGS__)

// ===== Implementation =====
///\cond
#define GblThread_create__3(cb, ud, start)   ((GblThread_create)(cb, ud, start))
#define GblThread_create__2(cb, ud)          (GblThread_create__3(cb, ud, GBL_TRUE))
#define GblThread_create__1(cb)              (GblThread_create__2(cb, GBL_NULL))

#define GblThread_exit__2(result, msg) \
    (GblThread_exit(result, msg, __FILE__, __func__, __LINE__))
#define GblThread_exit__1(result) \
    (GblThread_exit(result, GBL_NULL, __FILE__, __func__, __LINE__))

#define GblThread_nanoSleep__2(nsec, pRem) \
    ((GblThread_nanoSleep)(nSec, pRem))
#define GblThread_nanoSleep__1(nsec) \
    (GblThread_nanoSleep__2(nsec, GBL_NULL))

#define GblThread_foreach__2(it, closure) \
    ((GblThread_foreach)(it, closure))
#define GblThread_foreach__1(it) \
    (GblThread_foreach__2(it, GBL_NULL))
///\endcond

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_THREAD_H

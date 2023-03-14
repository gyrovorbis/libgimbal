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
 */
#ifndef GIMBAL_THREAD_H
#define GIMBAL_THREAD_H

#include "../meta/instances/gimbal_object.h"
#include "../meta/signals/gimbal_signal.h"
#include "gimbal_stack_frame.h"
#include "gimbal_main_loop.h"
#include "../allocators/gimbal_scope_allocator.h"

#define GBL_THREAD_TYPE             (GBL_TYPEOF(GblThread))
#define GBL_THREAD(self)            (GBL_INSTANCE_CAST(self, GblThread))
#define GBL_THREAD_CLASS(klass)     (GBL_CLASS_CAST(klass, GblThread))
#define GBL_THREAD_GET_CLASS(self)  (GBL_INSTANCE_GET_CLASS(self, GblThread))

#define GBL_SELF_TYPE GblThread

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblThread);

typedef GBL_RESULT (*GblThreadFn)    (GBL_SELF);
typedef GblBool    (*GblThreadIterFn)(GBL_SELF, void* pClosure);
typedef uintptr_t   GblThreadId;
typedef uintptr_t   GblThreadAffinity;

GBL_DECLARE_ENUM(GBL_THREAD_PRIORITY) {
    GBL_THREAD_PRIORITY_LOW,
    GBL_THREAD_PRIORITY_MEDIUM,
    GBL_THREAD_PRIORITY_HIGH,
    GBL_THREAD_PRIORITY_REAL_TIME
};

GBL_DECLARE_ENUM(GBL_THREAD_STATE) {
    GBL_THREAD_STATE_CONSTRUCTING,
    GBL_THREAD_STATE_READY,
    GBL_THREAD_STATE_RUNNING,
    GBL_THREAD_STATE_FINISHED,
    GBL_THREAD_STATE_DESTRUCTING,
};

GBL_CLASS_DERIVE(GblThread, GblObject)
    GblThreadFn pFnRun;
GBL_CLASS_END

GBL_INSTANCE_DERIVE(GblThread, GblObject)
    GblCallRecord    returnStatus;
    GBL_THREAD_STATE state;
GBL_INSTANCE_END

GBL_PROPERTIES(GblThread,
    (name,     GBL_GENERIC, (READ, WRITE), GBL_CLOSURE_TYPE),
    (priority, GBL_GENERIC, (      WRITE), GBL_ENUM_TYPE),
    (affinity, GBL_GENERIC, (      WRITE), GBL_FLAGS_TYPE),
    (result,   GBL_GENERIC, (READ),        GBL_ENUM_TYPE),
    (state,    GBL_GENERIC, (READ),        GBL_ENUM_TYPE),
    (autoRun,  GBL_GENERIC, (READ, WRITE), GBL_BOOL_TYPE),
    (detached, GBL_GENERIC, (      WRITE), GBL_BOOL_TYPE),
    (closure,  GBL_GENERIC, (READ, WRITE), GBL_CLOSURE_TYPE),
    (callback, GBL_GENERIC, (READ, WRITE), GBL_POINTER_TYPE)
)

GBL_SIGNALS(GblThread,
    (started,  (GBL_INSTANCE_TYPE, pReceiver)),
    (finished, (GBL_INSTANCE_TYPE, pReceiver))
)

// ===== Static Methods =====
GBL_EXPORT GblType     GblThread_type        (void)                           GBL_NOEXCEPT;
GBL_EXPORT GblSize     GblThread_coreCount   (void)                           GBL_NOEXCEPT;
GBL_EXPORT GblSize     GblThread_count       (void)                           GBL_NOEXCEPT;
GBL_EXPORT GblThread*  GblThread_find        (const char* pName)              GBL_NOEXCEPT;
GBL_EXPORT GblThread*  GblThread_at          (GblSize index)                  GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblThread_foreach     (GblThreadIterFn pIt, void* pCl) GBL_NOEXCEPT;

GBL_EXPORT GblThread*  GblThread_current     (void)                           GBL_NOEXCEPT;

// ===== Instance Methods for Parent Thread =====
GBL_EXPORT GblThread*  GblThread_create      (GblThreadFn pCallback,
                                              void*       pUserdata,
                                              GblBool     autoRun);           GBL_NOEXCEPT;

GBL_EXPORT GblRefCount GblThread_unref       (GBL_SELF)                       GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblThread_setName     (GBL_SELF, const char* pName)    GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblThread_setPriority (GBL_SELF,
                                              GBL_THREAD_PRIORITY priority)   GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblThread_setAffinity (GBL_SELF,
                                              GblThreadAffinity affinity)     GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblThread_start       (GBL_SELF)                       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblThread_wait        (GBL_SELF)                       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblThread_detach      (GBL_SELF)                       GBL_NOEXCEPT;

// ===== Instance Methods for Child Thread =====
GBL_EXPORT GBL_RESULT  GblThread_yield       (GBL_SELF)                       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblThread_sleep       (GBL_SELF, unsigned msec)        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblThread_exit        (GBL_SELF, GBL_RESULT result)    GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_THREAD_H

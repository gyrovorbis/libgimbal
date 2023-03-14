/*! \file
 *  \brief GblThread and lowest-level concurrency managment
 *  \ingroup core
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

typedef GblBool (*GblThreadIterFn)(GBL_SELF, void* pClosure);
typedef uintptr_t GblThreadId;
typedef uintptr_t GblThreadAffinity;

GBL_DECLARE_ENUM(GBL_THREAD_PRIORITY) {
    GBL_THREAD_PRIORITY_LOW,
    GBL_THREAD_PRIORITY_MEDIUM,
    GBL_THREAD_PRIORITY_HIGH,
    GBL_THREAD_PRIORITY_REAL_TIME
};

GBL_DECLARE_ENUM(GBL_THREAD_STATUS) {
    GBL_THREAD_INIT,
    GBL_THREAD_ACTIVE,
    GBL_THREAD_DONE
};

GBL_CLASS_DERIVE(GblThread, GblObject)
    GBL_RESULT (*pFnRun)(GBL_SELF);
GBL_CLASS_END

GBL_INSTANCE_DERIVE(GblThread, GblObject)
    GblCallRecord      callRecord;
    GblClosure*        pClosure;
    GblScopeAllocator* pScopeAlloc;
GBL_INSTANCE_END

GBL_PROPERTIES(GblThread,
    (closure,     GBL_GENERIC, (CONSTRUCT), GBL_CLOSURE_TYPE),
    (autoStart,   GBL_GENERIC, (WRITE),     GBL_BOOL_TYPE),
    (priority,    GBL_GENERIC, (WRITE),     GBL_ENUM_TYPE),
    (cpuAffinity, GBL_GENERIC, (WRITE),     GBL_FLAGS_TYPE)
)

GBL_SIGNALS(GblThread,
    (started,  (GBL_INSTANCE_TYPE, pReceiver)),
    (finished, (GBL_INSTANCE_TYPE, pReceiver))
)

GBL_EXPORT GblType           GblThread_type        (void)                           GBL_NOEXCEPT;
GBL_EXPORT GblSize           GblThread_coreCount   (void)                           GBL_NOEXCEPT;
GBL_EXPORT GblThread*        GblThread_current     (void)                           GBL_NOEXCEPT;
GBL_EXPORT GblThread*        GblThread_findByName  (const char* pName)              GBL_NOEXCEPT;
GBL_EXPORT GblThread*        GblThread_findById    (uintptr_t id)                   GBL_NOEXCEPT;
GBL_EXPORT GblBool           GblThread_foreach     (GblThreadIterFn pIt, void* pCl) GBL_NOEXCEPT;

GBL_EXPORT GblThread*        GblThread_create      (GblClosure*     pClosure,
                                                    GblType         type,
                                                    GblSize         size,
                                                    GblThreadClass* pClass,
                                                    GblBool         autoStart)      GBL_NOEXCEPT;

GBL_EXPORT GblRefCount       GblThread_unref       (GBL_SELF)                       GBL_NOEXCEPT;

GBL_EXPORT GblThreadId       GblThread_id          (GBL_CSELF)                      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT        GblThread_setName     (GBL_SELF, const char* pName)    GBL_NOEXCEPT;
GBL_EXPORT GBL_THREAD_STATUS GblThread_status      (GBL_CSELF)                      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT        GblThread_result      (GBL_CSELF)                      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT        GblThread_setPriority (GBL_SELF,
                                                    GBL_THREAD_PRIORITY priority)   GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT        GblThread_setAffinity (GBL_SELF,
                                                    GblThreadAffinity affinity)     GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT        GblThread_start       (GBL_SELF)                       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT        GblThread_wait        (GBL_SELF)                       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT        GblThread_detach      (GBL_SELF)                       GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT        GblThread_yield       (GBL_SELF)                       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT        GblThread_sleep       (GBL_SELF, unsigned msec)        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT        GblThread_exit        (GBL_SELF, GBL_RESULT result)    GBL_NOEXCEPT;

// set/get call record
// set/get exception/errors
// manage scope allocations

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_THREAD_H

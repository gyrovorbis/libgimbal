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
 *   \author 2023 Falco Girgis
 *   \copyright MIT License
 */
#ifndef GIMBAL_THREAD_H
#define GIMBAL_THREAD_H

#include "../meta/instances/gimbal_object.h"
#include "../meta/signals/gimbal_signal.h"
#include "gimbal_stack_frame.h"
#include "gimbal_main_loop.h"
#include "../allocators/gimbal_scope_allocator.h"
#include <signal.h>

/*! \name Type System
 *  \brief Type UUID and cast operators
 *  @{
 */
#define GBL_THREAD_TYPE             (GBL_TYPEID(GblThread))            //!< Type UUID for GblThread
#define GBL_THREAD(self)            (GBL_CAST(GblThread, self))        //!< Function-style GblInstance cast
#define GBL_THREAD_CLASS(klass)     (GBL_CLASS_CAST(GblThread, klass)) //!< Function-style GblClass cast
#define GBL_THREAD_GET_CLASS(self)  (GBL_CLASSOF(GblThread, self))     //!< Get GblThreadClass from GblInstance
//! @}

#define GBL_SELF_TYPE GblThread

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblTime);
GBL_FORWARD_DECLARE_STRUCT(GblThread);

//! Function callback type to be used as the main thread's callback with \ref GblThread_setCallback().
typedef GBL_RESULT (*GblThreadFn)    (GBL_SELF);
//! Iterator function type to be used with \ref GblThread_foreach()
typedef GblBool    (*GblThreadIterFn)(GBL_SELF, void* pClosure);
//! Represents a CPU affinity bitmask, with each bit being affinity to a single core
typedef uintptr_t   GblThreadAffinity;

//! Priority levels for GblThread
GBL_DECLARE_ENUM(GBL_THREAD_PRIORITY) {
    GBL_THREAD_PRIORITY_LOW,        //!< Lowest
    GBL_THREAD_PRIORITY_MEDIUM,     //!< Medium
    GBL_THREAD_PRIORITY_HIGH,       //!< High
    GBL_THREAD_PRIORITY_REAL_TIME   //!< Highest
};

//! Lifetime states for a GblThread
GBL_DECLARE_ENUM(GBL_THREAD_STATE) {
    GBL_THREAD_STATE_UNKNOWN,       //!< Unknown
    GBL_THREAD_STATE_INITIALIZING,  //!< Initializing/Constructing
    GBL_THREAD_STATE_READY,         //!< Ready/Waiting
    GBL_THREAD_STATE_RUNNING,       //!< Running/Executing
    GBL_THREAD_STATE_FINISHED       //!< Finished/Completed
};
\
/*! \struct  GblThreadClass
 *  \extends GblObjectClass
 *  \brief   GblClass VTable structure for GblThread
 *  \sa      GblThread
 */
GBL_CLASS_DERIVE(GblThread, GblObject)
//! Main execution entry point for a given thread, calls callback + closure then signals
    GblThreadFn pFnRun;
//! Standard C signal handler for a given thread
    GBL_RESULT  (*pFnSignal)(GBL_SELF, int signal);
GBL_CLASS_END

/*! \struct  GblThread
 *  \extends GblObject
 *  \ingroup core
 *  \brief   Object representing a thread, its local storage, and logic
 *  \sa      GblThreadClass
 */
GBL_INSTANCE_DERIVE(GblThread, GblObject)
    GblCallRecord         returnStatus; //!< Return information from a completed thread
    volatile sig_atomic_t signalStatus; //!< Pending signal state for a given thread
    GBL_THREAD_STATE      state;        //!< Current state for a given thread MAKE ME ATOMIC
GBL_INSTANCE_END

//! \cond
GBL_PROPERTIES(GblThread,
//  (name,      GBL_GENERIC, (READ, WRITE, OVERRIDE), GBL_STRING_TYPE),
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
//! \endcond

/*! \name Static Methods
 *  \brief Searching, iterating, counting, etc.
 *  @{
 */
//! Returns the GblType UUID associated with GblThread
GBL_EXPORT GblType    GblThread_type    (void)                         GBL_NOEXCEPT;
//! Returns the current number of live threads (not necessarily all active)
GBL_EXPORT size_t     GblThread_count   (void)                         GBL_NOEXCEPT;
//! Searches (linearly) for a thread with the string name given by \p pName
GBL_EXPORT GblThread* GblThread_find    (const char* pName)            GBL_NOEXCEPT;
//! Iterates over all live threads, passing each thread and \p pCl back to the \p pIt callback
GBL_EXPORT GblBool    GblThread_foreach (GblThreadIterFn pIt,
                                         void*           pCl/*=NULL*/) GBL_NOEXCEPT;
//! @}

/*! \name Parent Thread Methods
 *  \relatesalso GblThread
 *  \brief Methods to be called from a parent thread
 *  @{
 */
//! Creates a GblThread instance with the given callback and userdata and optionally starts its execution immediately
GBL_EXPORT GblThread*  GblThread_create      (GblThreadFn pCallback,
                                              void*       pUserdata/*=NULL*/,
                                              GblBool     autoStart/*=0*/)    GBL_NOEXCEPT;
//! Returns a new reference to an existing thread, incrementing its reference count by 1
GBL_EXPORT GblThread*  GblThread_ref         (GBL_CSELF)                      GBL_NOEXCEPT;
//! Decrements the reference count of a GblThread instance, deleting it if it's the last one
GBL_EXPORT GblRefCount GblThread_unref       (GBL_SELF)                       GBL_NOEXCEPT;
//! Returns GBL_TRUE if the given thread has been joined with its parent and is done executing, GBL_FALSE otherwise
GBL_EXPORT GblBool     GblThread_isJoined    (GBL_CSELF)                      GBL_NOEXCEPT;
//! Returns the string name assigned to the given thread
GBL_EXPORT const char* GblThread_name        (GBL_CSELF)                      GBL_NOEXCEPT;
//! Sets the string name \p pName to be the name of the given thread
GBL_EXPORT GBL_RESULT  GblThread_setName     (GBL_SELF, const char* pName)    GBL_NOEXCEPT;
//! Returns the closure assigned to the given thread, or NULL if there isn't one
GBL_EXPORT GblClosure* GblThread_closure     (GBL_CSELF)                      GBL_NOEXCEPT;
//! Assigns the closure of the given thread to \p pClosure, freeing the previous closure if there was one
GBL_EXPORT void        GblThread_setClosure  (GBL_SELF, GblClosure* pClosure) GBL_NOEXCEPT;
//! Returns the C callback function assigned to the given thread, or NULL if there isn't one
GBL_EXPORT GblThreadFn GblThread_callback    (GBL_CSELF)                      GBL_NOEXCEPT;
//! Assigns the C callback function of the given thread to \p pCb, which will be executed with the thread
GBL_EXPORT void        GblThread_setCallback (GBL_SELF, GblThreadFn pCb)      GBL_NOEXCEPT;
//! Sets the priority of the given thread to \p priority
GBL_EXPORT GBL_RESULT  GblThread_setPriority (GBL_SELF,
                                              GBL_THREAD_PRIORITY priority)   GBL_NOEXCEPT;
//! Sets the CPU affinity of the given thread to \p affinity
GBL_EXPORT GBL_RESULT  GblThread_setAffinity (GBL_SELF,
                                              GblThreadAffinity affinity)     GBL_NOEXCEPT;
//! Immediately starts a thread which has not yet joined or finished executing
GBL_EXPORT GBL_RESULT  GblThread_start       (GBL_SELF)                       GBL_NOEXCEPT;
//! Blocks execution of the current thread, awaiting the completion of the given thread
GBL_EXPORT GBL_RESULT  GblThread_join        (GBL_SELF)                       GBL_NOEXCEPT;
//! Performs a non-blocking wait on the given thread, spinning on a timeout
GBL_EXPORT GBL_RESULT  GblThread_spinWait    (GBL_SELF/*,
                                              const GblTimeSpec* pTimeout*/)  GBL_NOEXCEPT;
//! @}

/*! \name Child Thread Methods
 *  \relatesalso GblThread
 *  \brief Methods to be called from the spawned child thread
 *  @{
 */
//! Returns a pointer to the GblThread instance associatedw with the current thread
GBL_EXPORT GblThread* GblThread_current   (void)                          GBL_NOEXCEPT;
//! Suspends execution of the current thread, allowing other threads to execute temporarily
GBL_EXPORT GBL_RESULT GblThread_yield     (void)                          GBL_NOEXCEPT;
//! Sleeps the current thread for \p nsec nanoseconds, returning the remaining time optionally within \p pRemainder
GBL_EXPORT GBL_RESULT GblThread_nanoSleep (uint64_t  nsec,
                                           uint64_t* pRemainder/*=NULL*/) GBL_NOEXCEPT;
//! Ends execution of the currently executing thread, returning \p result and storing the given source context
GBL_EXPORT GBL_RESULT GblThread_exit      (GBL_RESULT  result,
                                           const char* pMessage/*=NULL*/,
                                           const char* pFile   /*=NULL*/,
                                           const char* pFunc   /*=NULL*/,
                                           size_t      line    /*=0*/)    GBL_NOEXCEPT;
//! @}

/*! \name Function Overload Macros
 *  \brief Macros providing default arguments for methods
 *  @{
 */
//! Provides default argument handling for GblThread_create()
#define GblThread_create(...)    GBL_VA_OVERLOAD_CALL_ARGC(GblThread_create_, __VA_ARGS__)
//! Provides default argument handling for GblThread_exit()
#define GblThread_exit(...)      GBL_VA_OVERLOAD_CALL_ARGC(GblThread_exit_, __VA_ARGS__)
//! Provides default argument handling for GblThread_nanoSleep()
#define GblThread_nanoSleep(...) GBL_VA_OVERLOAD_CALL_ARGC(GblThread_nanoSleep_, __VA_ARGS__)
//! Provides default argument handling for GblThread_foreach()
#define GblThread_foreach(...)   GBL_VA_OVERLOAD_CALL_ARGC(GblThread_foreach_, __VA_ARGS__)
//! @}

GBL_DECLS_END

///\cond
#define GblThread_create__3(cb, ud, start)   ((GblThread_create)(cb, ud, start))
#define GblThread_create__2(cb, ud)          (GblThread_create__3(cb, ud, GBL_TRUE))
#define GblThread_create__1(cb)              (GblThread_create__2(cb, GBL_NULL))

#define GblThread_exit__2(result, msg) \
    (GblThread_exit(result, msg, __FILE__, __func__, __LINE__))
#define GblThread_exit__1(result) \
    (GblThread_exit(result, GBL_NULL, __FILE__, __func__, __LINE__))

#define GblThread_nanoSleep__2(nsec, pRem) \
    ((GblThread_nanoSleep)(nsec, pRem))
#define GblThread_nanoSleep__1(nsec) \
    (GblThread_nanoSleep__2(nsec, GBL_NULL))

#define GblThread_foreach__2(it, closure) \
    ((GblThread_foreach)(it, closure))
#define GblThread_foreach__1(it) \
    (GblThread_foreach__2(it, GBL_NULL))
///\endcond

#undef GBL_SELF_TYPE

#endif // GIMBAL_THREAD_H

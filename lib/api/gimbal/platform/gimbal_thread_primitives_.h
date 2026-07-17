#ifndef GIMBAL_THREAD_PRIMITIVES__H
#define GIMBAL_THREAD_PRIMITIVES__H

#if GBL_THREADS_ENABLED
    #include <tinycthread.h>
#else

typedef int     thrd_t;
typedef int     mtx_t;
typedef int     tss_t;
typedef int     once_flag;

#define ONCE_FLAG_INIT  0
#define thrd_success    0
#define thrd_error      1
#define mtx_plain       0
#define mtx_recursive   0
#define mtx_timed       0

#define thrd_current()              (0)
#define thrd_create(thrp, fn, arg)  ((*(thrp) = 0), (fn)(arg), 0)
#define thrd_join(thr, res)         (0)
#define thrd_detach(thr)            (0)
#define thrd_yield()                ((void)0)
#define thrd_sleep(ts, rem)         (0)
#define thrd_exit(res)              ((void)(res))

#define mtx_init(mtx, type)         (0)
#define mtx_lock(mtx)               (0)
#define mtx_trylock(mtx)            (0)
#define mtx_unlock(mtx)             (0)
#define mtx_destroy(mtx)            ((void)0)

#define call_once(flag, fn) \
  do {                      \
    if (!(*(flag))) {       \
      (fn)();               \
      *(flag) = 1;          \
    }                       \
  } while (0)

#define tss_create(key, dtor)       (0)
#define tss_get(key)                (NULL)
#define tss_set(key, val)           (0)
#define tss_delete(key)             ((void)0)

#endif // GBL_THREADS_ENABLED

#endif // GIMBAL_THREAD_PRIMITIVES__H
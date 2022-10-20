/*! \file
 *  \brief GblTimer simple system timer/stopwatch API
 *  \ingroup dataTypes
 */

#ifndef GIMBAL_TIMER_H
#define GIMBAL_TIMER_H

#include "../core/gimbal_typedefs.h"
#include <time.h>

#define GBL_TIMER_INIT      { 0, 0, 0 }
#define GBL_TIMER(name)     name = GBL_TIMER_INIT

#define GBL_SELF_TYPE GblTimer

GBL_DECLS_BEGIN

typedef struct GblTimer {
    clock_t startTime;
    clock_t stopTime;
    GblBool active;
} GblTimer;

GBL_INLINE void     GblTimer_start      (GBL_SELF)  GBL_NOEXCEPT;
GBL_INLINE void     GblTimer_stop       (GBL_SELF)  GBL_NOEXCEPT;
GBL_INLINE void     GblTimer_continue   (GBL_SELF)  GBL_NOEXCEPT;
GBL_INLINE double   GblTimer_elapsedMs  (GBL_CSELF) GBL_NOEXCEPT;

// ========== IMPL ==========

GBL_INLINE void GblTimer_start(GBL_SELF) GBL_NOEXCEPT {
    pSelf->startTime    = clock();
    pSelf->active       = GBL_TRUE;
}

GBL_INLINE void GblTimer_stop(GBL_SELF) GBL_NOEXCEPT {
    pSelf->stopTime = clock();
    pSelf->active   = GBL_FALSE;
}

GBL_INLINE void GblTimer_continue(GBL_SELF) GBL_NOEXCEPT {
    if(!pSelf->active) {
        clock_t elapsed = pSelf->stopTime - pSelf->startTime;
        pSelf->startTime = clock();
        pSelf->startTime -= elapsed;
        pSelf->active   = GBL_TRUE;
    }
}

GBL_INLINE double GblTimer_elapsedMs(GBL_CSELF) GBL_NOEXCEPT {
    return (double)(pSelf->stopTime - pSelf->startTime) * 1000.0 / (double)CLOCKS_PER_SEC;
}

GBL_DECLS_END

#undef GBL_SELF_TYPE


#endif // GIMBAL_TIMER_H

/*! \file
 *  \brief GblTimer simple system timer/stopwatch API
 *  \ingroup utils
 *  \todo
 *      - change whole model to use timespec nsec
 */

#ifndef GIMBAL_TIMER_H
#define GIMBAL_TIMER_H

#include "../core/gimbal_ctx.h"
#include "gimbal_date_time.h"

#define GBL_TIMER_INIT      { 0, 0, 0 }
#define GBL_TIMER(name)     name = GBL_TIMER_INIT

#define GBL_SELF_TYPE GblTimer

GBL_DECLS_BEGIN

typedef struct GblTimer {
    GblTimeSpec startTime;
    GblTimeSpec stopTime;
    GblTimeSpec elapsedTime;
    GblBool     active;
} GblTimer;

GBL_INLINE void     GblTimer_start       (GBL_SELF)  GBL_NOEXCEPT;
GBL_INLINE void     GblTimer_stop        (GBL_SELF)  GBL_NOEXCEPT;
GBL_INLINE void     GblTimer_continue    (GBL_SELF)  GBL_NOEXCEPT;

GBL_INLINE GblBool  GblTimer_isValid     (GBL_CSELF) GBL_NOEXCEPT;

GBL_INLINE double   GblTimer_elapsedSecs (GBL_CSELF) GBL_NOEXCEPT;
GBL_INLINE double   GblTimer_elapsedMs   (GBL_CSELF) GBL_NOEXCEPT;
GBL_INLINE double   GblTimer_elapsedUs   (GBL_CSELF) GBL_NOEXCEPT;
GBL_INLINE uint64_t GblTimer_elapsedNs   (GBL_CSELF) GBL_NOEXCEPT;

// ========== IMPL ==========

GBL_INLINE void GblTimer_start(GBL_SELF) GBL_NOEXCEPT {
    timespec_get(&pSelf->startTime, TIME_UTC);
    pSelf->active       = GBL_TRUE;
}

GBL_INLINE void GblTimer_stop(GBL_SELF) GBL_NOEXCEPT {
    timespec_get(&pSelf->stopTime, TIME_UTC);
    pSelf->active    = GBL_FALSE;
    pSelf->elapsedTime = GblTime_specDiff(&pSelf->stopTime, &pSelf->startTime);
}

GBL_INLINE void GblTimer_continue(GBL_SELF) GBL_NOEXCEPT {
    if(!pSelf->active) {
        const GblTimeSpec elapsed = GblTime_specDiff(&pSelf->stopTime, &pSelf->startTime);
        timespec_get(&pSelf->startTime, TIME_UTC);
        pSelf->startTime = GblTime_specDiff(&pSelf->startTime, &elapsed);
        pSelf->active   = GBL_TRUE;
    }
}

GBL_INLINE GblBool GblTimer_isValid(GBL_CSELF) GBL_NOEXCEPT {
    return !pSelf->active &&
            pSelf->elapsedTime.tv_sec >= 0 &&
            pSelf->elapsedTime.tv_sec >= 0;
}

GBL_INLINE uint64_t GblTimer_elapsedNs(GBL_CSELF) GBL_NOEXCEPT {
    GBL_ASSERT(!pSelf->active);
    return pSelf->elapsedTime.tv_nsec +
           pSelf->elapsedTime.tv_sec * GBL_TIME_NSECS_PER_SEC;
}

GBL_INLINE double GblTimer_elapsedUs(GBL_CSELF) GBL_NOEXCEPT {
    return (double)GblTimer_elapsedNs(pSelf) / 1000.0;
}

GBL_INLINE double GblTimer_elapsedMs(GBL_CSELF) GBL_NOEXCEPT {
    return (double)GblTimer_elapsedNs(pSelf) / 1000000.0;
}

GBL_INLINE double GblTimer_elapsedSecs(GBL_CSELF) GBL_NOEXCEPT {
    return (double)GblTimer_elapsedNs(pSelf) / (double)GBL_TIME_NSECS_PER_SEC;
}


GBL_DECLS_END

#undef GBL_SELF_TYPE


#endif // GIMBAL_TIMER_H

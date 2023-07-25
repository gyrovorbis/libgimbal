#include <gimbal/utils/gimbal_timer.h>


GBL_EXPORT void GblTimer_start(GblTimer* pSelf) GBL_NOEXCEPT {
    timespec_get(&pSelf->startTime, TIME_UTC);
    pSelf->active = GBL_TRUE;
}

GBL_EXPORT void GblTimer_stop(GblTimer* pSelf) GBL_NOEXCEPT {
    timespec_get(&pSelf->stopTime, TIME_UTC);
    pSelf->active      = GBL_FALSE;
    pSelf->elapsedTime = GblTime_specDiff(&pSelf->stopTime, &pSelf->startTime);
}

GBL_EXPORT void GblTimer_continue(GblTimer* pSelf) GBL_NOEXCEPT {
    if(!pSelf->active) {
        const GblTimeSpec elapsed = GblTime_specDiff(&pSelf->stopTime, &pSelf->startTime);
        timespec_get(&pSelf->startTime, TIME_UTC);
        pSelf->startTime = GblTime_specDiff(&pSelf->startTime, &elapsed);
        pSelf->active   = GBL_TRUE;
    }
}

GBL_EXPORT GblBool GblTimer_isValid(const GblTimer* pSelf) GBL_NOEXCEPT {
    return !pSelf->active &&
            pSelf->elapsedTime.tv_sec >= 0 &&
            pSelf->elapsedTime.tv_sec >= 0;
}

GBL_EXPORT uint64_t GblTimer_elapsedNs(const GblTimer* pSelf) GBL_NOEXCEPT {
    GBL_ASSERT(!pSelf->active);
    return pSelf->elapsedTime.tv_nsec +
           pSelf->elapsedTime.tv_sec * GBL_TIME_NSECS_PER_SEC;
}

GBL_EXPORT double GblTimer_elapsedUs(const GblTimer* pSelf) GBL_NOEXCEPT {
    return (double)GblTimer_elapsedNs(pSelf) / 1000.0;
}

GBL_EXPORT double GblTimer_elapsedMs(const GblTimer* pSelf) GBL_NOEXCEPT {
    return (double)GblTimer_elapsedNs(pSelf) / 1000000.0;
}

GBL_EXPORT double GblTimer_elapsedSecs(const GblTimer* pSelf) GBL_NOEXCEPT {
    return (double)GblTimer_elapsedNs(pSelf) / (double)GBL_TIME_NSECS_PER_SEC;
}

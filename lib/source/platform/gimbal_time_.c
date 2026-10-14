#include <gimbal/platform/gimbal_time_.h>

#if defined(__gamecube__)
#include <sys/time.h>
#endif

#ifdef GBL_NO_TIMESPEC_

int timespec_get(struct timespec* pSpec, int base) {
    if(base != TIME_UTC)
        return 0;

#if defined(__gamecube__)
    struct timeval now;
    if(gettimeofday(&now, NULL) == -1)
        return 0;
    pSpec->tv_sec  = now.tv_sec;
    pSpec->tv_nsec = now.tv_usec * 1000;
#else
    if(clock_gettime(CLOCK_REALTIME, pSpec) == -1)
        return 0;
#endif

    return TIME_UTC;
}

#endif

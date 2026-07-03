#include <gimbal/platform/gimbal_time_.h>

#ifdef GBL_NO_TIMESPEC_

int timespec_get(struct timespec* pSpec, int base) {
    if(base != TIME_UTC)
        return 0;

    if(clock_gettime(CLOCK_REALTIME, pSpec) == -1)
        return 0;

    return TIME_UTC;
}

#endif

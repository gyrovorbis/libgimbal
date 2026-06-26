#include <gimbal/platform/gimbal_time_.h>

#ifdef GBL_NO_TIMESPEC_
    #ifdef GBL_N64
        #include <libdragon.h>

        int clock_gettime(int clock_id, struct timespec *tp) {
            if (tp == NULL) {
                return -1;
            }

            unsigned long long ticks = timer_ticks();

            tp->tv_sec = ticks / TICKS_PER_SECOND;

            unsigned long long remainder = ticks % TICKS_PER_SECOND;
            tp->tv_nsec = (remainder * 1000000000ULL) / TICKS_PER_SECOND;

            return 0;
        }
    #endif // GBL_N64

    int timespec_get(struct timespec* pSpec, int base) {
        if(base != TIME_UTC)
            return 0;

        if(clock_gettime(CLOCK_REALTIME, pSpec) == -1)
            return 0;

        return TIME_UTC;
    }

#endif // GBL_NO_TIMESPEC_

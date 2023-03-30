#ifndef GIMBAL_TIME__H
#define GIMBAL_TIME__H

#include <time.h>

#define GBL_NO_TIMESPEC_

#ifndef GBL_NO_TIMESPEC_
#   if defined(__APPLE__) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(VITA)
#       define GBL_NO_TIMESPEC_
#   endif
#endif

#ifdef GBL_NO_TIMESPEC_

#define TIME_UTC 1

struct timespec {
    time_t tv_sec;
    long   tv_nsec;
};

int timespec_get(struct timespec* pSpec, int base);

#endif

#endif // GIMBAL_TIME__H

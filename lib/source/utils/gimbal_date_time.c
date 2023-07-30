#include <gimbal/utils/gimbal_date_time.h>
#include <gimbal/strings/gimbal_string_buffer.h>
#include <gimbal/strings/gimbal_string.h>
#include <gimbal/meta/classes/gimbal_opaque.h>
#include <wchar.h>
#include <math.h>

#define ABDAY_1_    0x00
#define DAY_1_      0x07
#define ABMON_1_    0x0e
#define MON_1_      0x1a
#define AM_STR_     0x26
#define PM_STR_     0x27

#define FAIL_RETURN_NULL_() \
    GBL_STMT_START { \
        GBL_CTX_BEGIN(NULL); \
        GBL_CTX_VERIFY(GBL_FALSE, GBL_RESULT_ERROR_INVALID_DATE_TIME); \
        GBL_CTX_END_BLOCK(); \
        return NULL; \
    } GBL_STMT_END

static const char c_time_strings_[] =
/* 0x00 */  "Sun\0" "Mon\0" "Tue\0" "Wed\0" "Thu\0" "Fri\0" "Sat\0"
/* 0x07 */  "Sunday\0" "Monday\0" "Tuesday\0" "Wednesday\0"
/* 0x0b */  "Thursday\0" "Friday\0" "Saturday\0"
/* 0x0e */  "Jan\0" "Feb\0" "Mar\0" "Apr\0" "May\0" "Jun\0"
/* 0x14 */  "Jul\0" "Aug\0" "Sep\0" "Oct\0" "Nov\0" "Dec\0"
/* 0x1a */  "January\0" "February\0" "March\0" "April\0"
/* 0x1e */  "May\0" "June\0" "July\0" "August\0"
/* 0x22 */  "September\0" "October\0" "November\0" "December\0"
/* 0x26 */  "AM\0" "PM\0";

static const char* get_ctime_string_(size_t  entry) {
    const char* pStr = c_time_strings_;
    for (size_t  idx = entry; idx; idx--, pStr++) for (; *pStr; pStr++);
    return pStr;
}

// Adapted and modified from musl implementation
static char* strptime_(const char *restrict s, const char *restrict f, struct tm *restrict tm, int* restrict utcOffset) {
    int i, w, neg, adj, min, range, *dest, dummy;
    const char *ex;
    size_t len;
    int want_century = 0, century = 0, relyear = 0;
    while (*f) {
        if (*f != '%') {
            if (isspace(*f)) for (; *s && isspace(*s); s++);
            else if (*s != *f) return 0;
            else s++;
            f++;
            continue;
        }
        f++;
        if (*f == '+') f++;
        if (isdigit(*f)) {
            char *new_f;
            w=strtoul(f, &new_f, 10);
            f = new_f;
        } else {
            w=-1;
        }
        adj=0;
        switch (*f++) {
        case 'A':
            dest = &tm->tm_wday;
            min = DAY_1_;
            range = 7;
            goto symbolic_range;
        case 'a':
            dest = &tm->tm_wday;
            min = ABDAY_1_;
            range = 7;
            goto symbolic_range;
        case 'B':
            dest = &tm->tm_mon;
            min = MON_1_;
            range = 12;
            goto symbolic_range;
        case 'b': case 'h':
            dest = &tm->tm_mon;
            min = ABMON_1_;
            range = 12;
            goto symbolic_range;
        case 'c':
            s = strptime_(s, "%a %b %e %T %Y", tm, utcOffset);
            if (!s) return 0;
            break;
        case 'C':
            dest = &century;
            if (w<0) w=2;
            want_century |= 2;
            goto numeric_digits;
        case 'd': case 'e':
            dest = &tm->tm_mday;
            min = 1;
            range = 31;
            goto numeric_range;
        case 'D':
            s = strptime_(s, "%m/%d/%y", tm, utcOffset);
            if (!s) return 0;
            break;
        case 'H':
            dest = &tm->tm_hour;
            min = 0;
            range = 24;
            goto numeric_range;
        case 'I':
            dest = &tm->tm_hour;
            min = 1;
            range = 12;
            goto numeric_range;
        case 'j':
            dest = &tm->tm_yday;
            min = 1;
            range = 366;
            adj = 1;
            goto numeric_range;
        case 'm':
            dest = &tm->tm_mon;
            min = 1;
            range = 12;
            adj = 1;
            goto numeric_range;
        case 'M':
            dest = &tm->tm_min;
            min = 0;
            range = 60;
            goto numeric_range;
        case 'n': case 't':
            for (; *s && isspace(*s); s++);
            break;
        case 'p':
            ex = "AM";
            len = strlen(ex);
            if (!gblStrnCaseCmp(s, ex, len)) {
                tm->tm_hour %= 12;
                s += len;
                break;
            }
            ex = "PM";
            len = strlen(ex);
            if (!gblStrnCaseCmp(s, ex, len)) {
                tm->tm_hour %= 12;
                tm->tm_hour += 12;
                s += len;
                break;
            }
            return 0;
        case 'r':
            s = strptime_(s, "%I:%M:%S %p", tm, utcOffset);
            if (!s) return 0;
            break;
        case 'R':
            s = strptime_(s, "%H:%M", tm, utcOffset);
            if (!s) return 0;
            break;
        case 'S':
            dest = &tm->tm_sec;
            min = 0;
            range = 61;
            goto numeric_range;
        case 'T':
            s = strptime_(s, "%H:%M:%S", tm, utcOffset);
            if (!s) return 0;
            break;
        case 'U':
        case 'W':
            /* Throw away result, for now. (FIXME?) */
            dest = &dummy;
            min = 0;
            range = 54;
            goto numeric_range;
        case 'w':
            dest = &tm->tm_wday;
            min = 0;
            range = 7;
            goto numeric_range;
        case 'x':
            s = strptime_(s, "%a %b %e %T %Y", tm, utcOffset);
            if (!s) return 0;
            break;
        case 'X':
            s = strptime_(s, "%H:%M:%S\0", tm, utcOffset);
            if (!s) return 0;
            break;
        case 'y':
            dest = &relyear;
            w = 2;
            want_century |= 1;
            goto numeric_digits;
        case 'Y':
            dest = &tm->tm_year;
            if (w<0) w=4;
            adj = 1900;
            want_century = 0;
            goto numeric_digits;
        case 'z':
            dest = utcOffset;
            w = 4;
            goto numeric_digits;
        case '%':
            if (*s++ != '%') return 0;
            break;
        default:
            return 0;
        numeric_range:
            if (!isdigit(*s)) return 0;
            *dest = 0;
            for (i=1; i<=min+range && isdigit(*s); i*=10)
                *dest = *dest * 10 + *s++ - '0';
            if (*dest - min >= range) return 0;
            *dest -= adj;
            switch((char *)dest - (char *)tm) {
            case offsetof(struct tm, tm_yday):
                ;
            }
            goto update;
        numeric_digits:
            neg = 0;
            if (*s == '+') s++;
            else if (*s == '-') neg=1, s++;
            if (!isdigit(*s)) return 0;
            for (*dest=i=0; i<w && isdigit(*s); i++)
                *dest = *dest * 10 + *s++ - '0';
            if (neg) *dest = -*dest;
            *dest -= adj;
            goto update;
        symbolic_range:
            for (i=2*range-1; i>=0; i--) {
                ex = get_ctime_string_(min+i);
                len = strlen(ex);
                if (gblStrnCaseCmp(s, ex, len)) continue;
                s += len;
                *dest = i % range;
                break;
            }
            if (i<0) return 0;
            goto update;
        update:
            //FIXME
            ;
        }
    }
    if (want_century) {
        tm->tm_year = relyear;
        if (want_century & 2) tm->tm_year += century * 100 - 1900;
        else if (tm->tm_year <= 68) tm->tm_year += 100;
    }
    // check if we need to adjust UTC offset that was read
    if(utcOffset && *utcOffset) {
        *utcOffset = (*utcOffset / 100) * 60 + (*utcOffset % 100);
    }
    return (char *)s;
}

static long long diff_tm_(struct tm *a, struct tm *b) {
    return a->tm_sec - b->tm_sec
            + 60LL*(a->tm_min - b->tm_min)
            + 3600LL*(a->tm_hour - b->tm_hour)
            + 86400LL*(a->tm_yday - b->tm_yday)
            + (a->tm_year-70) * 31536000LL
            - (a->tm_year-69) / 4 * 86400LL
            + (a->tm_year-1) / 100 * 86400LL
            - (a->tm_year+299) / 400 * 86400LL
            - (b->tm_year-70) * 31536000LL
            + (b->tm_year-69) / 4 * 86400LL
            - (b->tm_year-1) / 100 * 86400LL
            + (b->tm_year+299) / 400 * 86400LL;
}

/*
 * Derive the difference in seconds between the local timezone and
 * UTC time, which we can use as timezone offset for GblDateTime
 * using the standard C API.
 */
static long long timezone_delta_secs_(void) {
    // Cache results of operation for subsequent calls
    static GblBool cached = GBL_FALSE;
    static long long result = 0;

    // Don't bother with expensive calculation if we've done it before
    if(!cached) {
        long long prevResult;
        /* Repeat calculation until we've found the same value for two consecutive
         * iterations. This protects against the slim chance that the localtime and
         * UTC time queries happen on second boundaries and are performed on different
         * times.
         */
        do {
            // maintain result from previous iteration
            prevResult = result;

            // broken-down time for Linux epoch
            struct tm e0 = { .tm_year = 70, .tm_mday = 1 }, e1;

            // get LOCAL time_t epoch value for e0 (Jan. 1, 1970)
            time_t pseudo = mktime(&e0);

            // get GTMTIME time_t EPOCH value
            e1 = *gmtime(&pseudo);

            // return difference between the two (in seconds)
            result = diff_tm_(&e0, &e1);

        // Continue iterating if the calculated values didn't match
        } while(prevResult != result);

        cached = GBL_TRUE;
    }

    return result;
}

static struct tm* now_tm_(GblNanoSecond* pNSecs) {
    GblTimeSpec spec;
    timespec_get(&spec, TIME_UTC);
    if(pNSecs) *pNSecs = spec.tv_nsec;
    return localtime(&spec.tv_sec);
}

static time_t toBrokenDown_(const GblDateTime* pSelf, struct tm* pBrokenDown) {
    memset(pBrokenDown, 0, sizeof(struct tm));
    pBrokenDown->tm_isdst = -1;
    pBrokenDown->tm_year = pSelf->date.year - GBL_DATE_TIME_BROKEN_DOWN_YEAR_FIRST;
    pBrokenDown->tm_mon  = pSelf->date.month + GBL_DATE_TIME_BROKEN_DOWN_MONTH_OFFSET;
    pBrokenDown->tm_mday = pSelf->date.day;
    pBrokenDown->tm_hour = pSelf->time.hours;
    pBrokenDown->tm_min  = pSelf->time.minutes;
    pBrokenDown->tm_sec  = pSelf->time.seconds;
    return mktime(pBrokenDown);
}

GBL_EXPORT void fromBrokenDown_(GblDateTime* pSelf, const struct tm* pBrokenDown)  {
    pSelf->date.year     = pBrokenDown->tm_year + GBL_DATE_TIME_BROKEN_DOWN_YEAR_FIRST;
    pSelf->date.month    = pBrokenDown->tm_mon - GBL_DATE_TIME_BROKEN_DOWN_MONTH_OFFSET;
    pSelf->date.day      = pBrokenDown->tm_mday;
    pSelf->time.hours    = pBrokenDown->tm_hour;
    pSelf->time.minutes  = pBrokenDown->tm_min;
    pSelf->time.seconds  = pBrokenDown->tm_sec;
    pSelf->time.nSeconds = 0;
}

GBL_EXPORT const char* GblDate_monthStr(GblMonth month) {
    return (month >= GBL_MONTH_JANUARY && month <= GBL_MONTH_COUNT)?
                get_ctime_string_(MON_1_ + (month - 1)) : NULL;
}

GBL_EXPORT const char* GblDate_monthStrShort(GblMonth month) {
    return (month >= GBL_MONTH_JANUARY && month <= GBL_MONTH_COUNT)?
                get_ctime_string_(ABMON_1_ + (month - 1)) : NULL;
}

GBL_EXPORT const char* GblDate_weekDayStr(GblWeekDay weekDay) {
    return (weekDay >= GBL_WEEK_DAY_SUNDAY && weekDay < GBL_WEEK_DAY_COUNT)?
                get_ctime_string_(DAY_1_ + weekDay) : NULL;
}

GBL_EXPORT const char* GblDate_weekDayStrShort(GblWeekDay weekDay) {
    return (weekDay >= GBL_WEEK_DAY_SUNDAY && weekDay < GBL_WEEK_DAY_COUNT)?
                get_ctime_string_(ABDAY_1_ + weekDay) : NULL;
}

GBL_EXPORT GblDate* GblDate_fromOrdinal(GblDate* pSelf, GblYear year, GblDay day) {
    if(!pSelf) FAIL_RETURN_NULL_();

    GblDateTime dt;
    GblDateTime_set(&dt, year, GBL_MONTH_JANUARY, 1);

    GblDateTime_addDays(&dt, day-1);

    memcpy(pSelf, &dt.date, sizeof(GblDate));

    return pSelf;
}

GBL_EXPORT GblBool GblDate_isDst(const GblDate* pSelf) {
    struct tm local = {
        .tm_year  = pSelf->year,
        .tm_mon   = pSelf->month,
        .tm_mday  = pSelf->day,
        .tm_isdst = -1
    };

    const time_t epoch = mktime(&local);

    struct tm* pLocal = localtime(&epoch);

    return pLocal->tm_isdst;
}

GBL_EXPORT const char* GblTime_amPmStr(GblBool isPm) {
    return get_ctime_string_(isPm? PM_STR_ : AM_STR_);
}

GBL_EXPORT GblSecond GblTime_localUtcOffset(void) {
    return (GblSecond)timezone_delta_secs_();
}

GBL_EXPORT GblDateTime* GblDateTime_fromUnix(GblDateTime* pSelf, const time_t epoch) {
    struct tm* pLocal = gmtime(&epoch);

    if(!pLocal)
        FAIL_RETURN_NULL_();

    fromBrokenDown_(pSelf, pLocal);
    pSelf->time.nSeconds = 0;
    pSelf->utcOffset = 0.0;
    return pSelf;
}

GBL_EXPORT GblDateTime* GblDateTime_fromSpecUtc(GblDateTime* pSelf,
                                                const GblTimeSpec* pSpec)
{
    GblDateTime* pDt = GblDateTime_fromUnix(pSelf, pSpec->tv_sec);

    if(!pDt)
        FAIL_RETURN_NULL_();

    pDt->time.nSeconds = pSpec->tv_nsec;

    return pSelf;
}

GBL_EXPORT GblDateTime* GblDateTime_fromLocal(GblDateTime* pSelf, const struct tm* pLocal) {
    struct tm temp = *pLocal;

    if(mktime(&temp) == (time_t)-1)
        FAIL_RETURN_NULL_();

    fromBrokenDown_(pSelf, &temp);
    pSelf->time.nSeconds = 0;
    pSelf->utcOffset = GblTime_localUtcOffset();

    return pSelf;
}

GBL_EXPORT GblDateTime* GblDateTime_fromUtc(GblDateTime* pSelf, const struct tm* pUtc) {
    struct tm temp = *pUtc;

    if(mktime(&temp) == (time_t)-1)
        FAIL_RETURN_NULL_();

    fromBrokenDown_(pSelf, &temp);
    pSelf->utcOffset = 0.0;

    return pSelf;
}

GBL_EXPORT GblDateTime* GblDateTime_nowLocal(GblDateTime* pSelf) {
    return GblDateTime_fromLocal(pSelf, now_tm_(&pSelf->time.nSeconds));
}

GBL_EXPORT GblDateTime* GblDateTime_nowUtc(GblDateTime* pSelf) {
    struct tm bdTime = *now_tm_(&pSelf->time.nSeconds);
    bdTime.tm_sec -= GblTime_localUtcOffset();
    return GblDateTime_fromUtc(pSelf, &bdTime);
}

GBL_EXPORT time_t GblDateTime_toUnix(const GblDateTime* pSelf) {
    struct tm local;
    if(!pSelf) return (time_t)-1;
    return toBrokenDown_(pSelf, &local) + GblTime_localUtcOffset();
}

GBL_EXPORT GBL_RESULT GblDateTime_toLocal(const GblDateTime* pSelf, struct tm* pBrokenDown) {
    GblDateTime adjTime = *pSelf;
    adjTime.time.seconds += (adjTime.utcOffset - GblTime_localUtcOffset());

    if(toBrokenDown_(&adjTime, pBrokenDown) == (time_t)-1)
        return GBL_RESULT_ERROR_INVALID_DATE_TIME;

    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT GblDateTime_toUtc(const GblDateTime* pSelf, struct tm* pBrokenDown) {
    GblDateTime adjusted = *pSelf;
    adjusted.time.seconds -= adjusted.utcOffset;

    if(toBrokenDown_(pSelf, pBrokenDown) == (time_t)-1)
        return GBL_RESULT_ERROR_INVALID_DATE_TIME;

    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GblTimeSpec GblDateTime_toSpecUtc(const GblDateTime* pSelf) {
    GblTimeSpec spec = {
        .tv_sec  = GblDateTime_toUnix(pSelf),
        .tv_nsec = pSelf->time.nSeconds
    };

    return spec;
}

GBL_EXPORT const char* GblDateTime_toIso8601(const GblDateTime* pSelf, GblStringBuffer* pBuffer) {
    if(!pSelf || !pBuffer)
        FAIL_RETURN_NULL_();

    GblStringBuffer_reserve(pBuffer, GBL_DATE_TIME_ISO8601_STRING_SIZE);

    GblDateTime_format(pSelf, pBuffer, "%Y-%m-%dT%H:%M:%S");

    const unsigned ms = pSelf->time.nSeconds / 1000000;
    if(ms)
        GblStringBuffer_appendPrintf(pBuffer, ".%.3u", ms);

    if(pSelf->utcOffset >= 0)
        GblStringBuffer_append(pBuffer, GBL_STRV("+"));
    else
        GblStringBuffer_append(pBuffer, GBL_STRV("-"));

    const int mins = abs(pSelf->utcOffset) / 60;
    GblStringBuffer_appendPrintf(pBuffer, "%02d%02d", mins / 60, mins % 60);
    return GblStringBuffer_data(pBuffer);
}

GBL_EXPORT GblDateTime* GblDateTime_parse(GblDateTime* pSelf, const char* pString, const char* pFormat) {
    struct tm brokenDownTime = { 0 };
    int utcOffset = 0;
    strptime_(pString, pFormat, &brokenDownTime, &utcOffset);
    GblDateTime* pResult = GblDateTime_fromUtc(pSelf, &brokenDownTime);
    if(pResult) pResult->utcOffset = utcOffset;
    return pResult;
}

GBL_EXPORT const char* GblDateTime_format(const GblDateTime* pSelf, GblStringBuffer* pBuffer, const char* pFormat) {
    const char* pResult = NULL;

    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_POINTER(pBuffer);
    GBL_CTX_VERIFY_POINTER(pFormat);

    struct tm bTime;
    GBL_CTX_VERIFY(toBrokenDown_(pSelf, &bTime) != (time_t)-1,
                   GBL_RESULT_ERROR_INVALID_DATE_TIME);

    int multiplier = 1;

    do {
        GBL_CTX_VERIFY_CALL(GblStringBuffer_reserve(pBuffer, multiplier * GBL_DATE_TIME_FORMAT_BUFFER_SIZE_INCREMENT));
        ++multiplier;
        GBL_CTX_VERIFY(++multiplier <= GBL_DATE_TIME_FORMAT_BUFFER_SIZE_MULTIPLIER_MAX,
                       GBL_RESULT_ERROR_INVALID_EXPRESSION);
    } while(strftime(GblStringBuffer_data(pBuffer),
                     GblStringBuffer_capacity(pBuffer),
                     pFormat,
                     &bTime)
            == 0);

    pResult = GblStringBuffer_data(pBuffer);

    GBL_PRIV(pBuffer->data).size = strlen(pResult);

    GBL_CTX_END_BLOCK();
    return pResult;
}

GBL_EXPORT GblDateTime* GblDateTime_normalize(GblDateTime* pSelf) {
    struct tm bTime;

    if(!pSelf)
        goto fail;

    if(pSelf->date.month <= 0)
        pSelf->date.month = GBL_MONTH_JANUARY;
    if(pSelf->date.day <= 0)
        pSelf->date.day = 1;
    if(pSelf->date.year < GBL_DATE_TIME_BROKEN_DOWN_YEAR_MIN)
        pSelf->date.year = GBL_DATE_TIME_BROKEN_DOWN_YEAR_MIN;
    if(abs(pSelf->time.nSeconds) > GBL_TIME_NSECS_PER_SEC) {
        pSelf->time.seconds  += pSelf->time.nSeconds / GBL_TIME_NSECS_PER_SEC;
        pSelf->time.nSeconds %= GBL_TIME_NSECS_PER_SEC;
    }
    const GblNanoSecond ns = pSelf->time.nSeconds;

    if(toBrokenDown_(pSelf, &bTime) == (time_t)-1)
        goto fail;

    fromBrokenDown_(pSelf, &bTime);
    pSelf->time.nSeconds = ns;
    return pSelf;
fail:
    FAIL_RETURN_NULL_();
}

GBL_EXPORT GblTimeSpec GblDateTime_diff(const GblDateTime* pSelf, const GblDateTime* pRhs) {
    GblTimeSpec spec1 = {
        .tv_sec  = GblDateTime_toUnix(pSelf),
        .tv_nsec = pSelf->time.nSeconds
    };

    GblTimeSpec spec2 = {
        .tv_sec  = GblDateTime_toUnix(pRhs),
        .tv_nsec = pSelf->time.nSeconds
    };

    GblTimeSpec spec = { 0 };

    if(spec1.tv_sec == (time_t)-1 && spec2.tv_sec == (time_t)-1)
        return spec;

    if(spec1.tv_sec == (time_t)-1 || spec2.tv_sec == (time_t)-1) {
        spec.tv_sec = -1;
        return spec;
    }

    return GblTime_specDiff(&spec1, &spec2);
}

GBL_EXPORT GblDateTime* GblDateTime_add(GblDateTime* pSelf, const GblDateTime* pRhs) {
    if(!pSelf || !pRhs) FAIL_RETURN_NULL_();

    pSelf->date.year     += pRhs->date.year,
    pSelf->date.month    += pRhs->date.month,
    pSelf->date.day      += pRhs->date.day;
    pSelf->time.hours    += pRhs->time.hours,
    pSelf->time.minutes  += pRhs->time.minutes,
    pSelf->time.seconds  += pRhs->time.seconds;
    pSelf->time.nSeconds += pRhs->time.nSeconds;
    pSelf->utcOffset     += pRhs->utcOffset;

    return GblDateTime_normalize(pSelf);
}

GBL_EXPORT GblDateTime* GblDateTime_addDays(GblDateTime* pSelf, GblDay days) {
    const GblDateTime rhs = {
        .date = {
            .day = days
        }
    };
    return GblDateTime_add(pSelf, &rhs);
}

GBL_EXPORT GblDateTime* GblDateTime_addHours(GblDateTime* pSelf, GblHour hours) {
    const GblDateTime rhs = {
        .time = {
            .hours = hours
        }
    };
    return GblDateTime_add(pSelf, &rhs);
}

GBL_EXPORT GblDateTime* GblDateTime_addMinutes(GblDateTime* pSelf, GblMinute minutes) {
    const GblDateTime rhs = {
        .time = {
            .minutes = minutes
        }
    };
    return GblDateTime_add(pSelf, &rhs);
}

GBL_EXPORT GblDateTime* GblDateTime_addSeconds(GblDateTime* pSelf, GblSecond seconds) {
    const GblDateTime rhs = {
        .time = {
            .seconds = seconds
        }
    };
    return GblDateTime_add(pSelf, &rhs);
}

GBL_EXPORT GblDateTime* GblDateTime_addNanoSecs(GblDateTime* pSelf, GblNanoSecond nSecs) {
    const GblDateTime rhs = {
        .time = {
            .nSeconds = nSecs
        }
    };
    return GblDateTime_add(pSelf, &rhs);
}

GBL_EXPORT GblDateTime* GblDateTime_addWeeks(GblDateTime* pSelf, int weeks) {
    const GblDateTime rhs = {
        .date = {
            .day = weeks * 7
        }
    };
    return GblDateTime_add(pSelf, &rhs);
}

GBL_EXPORT GblDateTime* GblDateTime_addMonths(GblDateTime* pSelf, int months) {
    if(!pSelf) FAIL_RETURN_NULL_();

    pSelf->date.year += months / 12;
    pSelf->date.month += months % 12;

    if(pSelf->date.month < 1) {
        pSelf->date.month += 12;
        --pSelf->date.year;
    } else if(pSelf->date.month > 12) {
        pSelf->date.month -= 12;
        ++pSelf->date.year;
    }

    pSelf->date.day = GBL_MIN(pSelf->date.day,
                              GblDate_monthDays(pSelf->date.month,
                                                pSelf->date.year));

    return pSelf;
}

GBL_EXPORT GblDateTime* GblDateTime_addYears(GblDateTime* pSelf, GblYear years) {
    const GblDateTime rhs = {
        .date = {
            .year = years
        }
    };
    return GblDateTime_add(pSelf, &rhs);
}


GBL_EXPORT GblBool GblDate_isLeapYear(GblYear year) {
    GBL_ASSERT(year > GBL_DATE_GREGORIAN_YEAR_FIRST, "Must be a Gregorian year!");
    if (year % 4) {
        return GBL_FALSE;
    } else if (!(year % 400)) {
        return GBL_TRUE;
    } else if (!(year % 100)) {
        return GBL_FALSE;
    } else {
        return GBL_TRUE;
    }
}

GBL_EXPORT GblDay GblDate_monthDays(GblMonth month, GblYear year) {
    if(month == GBL_MONTH_FEBRUARY) {
        return GblDate_isLeapYear(year)? 29 : 28;
    } else return (month > GBL_MONTH_JULY)? ((month&1)? 30: 31) : ((month&1)? 31: 30);
}

GBL_EXPORT GblBool GblDate_isValid(const GblDate* pSelf) {
    if(pSelf->day <= 0)
        return GBL_FALSE;
    if(pSelf->month <= 0)
        return GBL_FALSE;
    if(pSelf->year < 0)
        return GBL_FALSE;
    if(pSelf->month > GBL_MONTH_COUNT)
        GBL_FALSE;
    if(pSelf->day > GblDate_monthDays(pSelf->month, pSelf->year))
        return GBL_FALSE;
    return GBL_TRUE;
}

// Should work with non-Gregorian times?
GBL_EXPORT GblDay GblDate_toJulian(const GblDate* pSelf) {
    const GblDay a = (14 - pSelf->month) / 12;
    const GblDay y = pSelf->year + 4800 - a;
    const GblDay m = pSelf->month + 12*a - 3;

    GblDay jdn = 0;

    if ((pSelf->year > 1582) || (pSelf->year == 1582 && pSelf->month > 10) ||
        (pSelf->year == 1582 && pSelf->month == 10 && pSelf->day < 15)) {

        jdn = pSelf->day + (153*m+2)/5 + 365*y + y/4 - y/100 + y/400 - 32045;
    } else {
        jdn = pSelf->day + (153*m+2)/5 + 365*y + y/4 - 32045;
    }

    return jdn;
}

GBL_EXPORT GblWeekDay GblDate_weekDay(const GblDate* pSelf) {
    GblDay d = pSelf->day;
    GblDay m = pSelf->month;
    GblDay y = pSelf->year;
    return (GblWeekDay)((d += m < 3 ? y-- : y - 2, 23*m/9 + d + 4 + y/4- y/100 + y/400)%7);
}

GBL_EXPORT GblDay GblDate_yearDay(const GblDate* pSelf) {
    static const GblDay days[2][13] = {
        {0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},
        {0, 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335}
    };
    return days[GblDate_isLeapYear(pSelf->year)]
               [pSelf->month]
            + pSelf->day;
}

GBL_EXPORT uint8_t GblDate_yearWeek(const GblDate* pSelf) {
    return GblDate_yearDay(pSelf)/7;
}

// MAYBE works with non-Gregorian times?
GBL_EXPORT GblDate* GblDate_fromJulian(GblDate* pSelf, GblDay julianDate) {
    int i, j, k, n, l;

    l = julianDate+68569;
    n = 4*l/146097;
    l = l-(146097*n+3)/4;
    i = 4000*(l+1)/1461001;
    l = l-1461*i/4+31;
    j = 80*l/2447;
    k = l-2447*j/80;
    l = j/11;
    j = j+2-12*l;
    i = 100*(n-49)+i+l;

    pSelf->year  = (GblYear)i;
    pSelf->month = (GblMonth)j;
    pSelf->day   = (GblDay)k;

    return pSelf;
}

GBL_EXPORT GblBool GblTime_isValid(const GblTime* pSelf) {
    if(pSelf->nSeconds < 0)
        return GBL_FALSE;
    if(pSelf->seconds < 0)
        return GBL_FALSE;
    if(pSelf->minutes < 0)
        return GBL_FALSE;
    if(pSelf->hours < 0)
        return GBL_FALSE;
    if(pSelf->nSeconds > GBL_TIME_NSECS_PER_SEC - 1)
        return GBL_FALSE;
    if(pSelf->seconds > 59)
        return GBL_FALSE;
    if(pSelf->minutes > 59)
        return GBL_FALSE;
    if(pSelf->hours > 23)
        return GBL_FALSE;
    return GBL_TRUE;
}

GBL_EXPORT GblTimeSpec GblTime_specDiff(const GblTimeSpec* pSrc1, const GblTimeSpec* pSrc2) {
    GblTimeSpec temp;

    if((pSrc1->tv_nsec - pSrc2->tv_nsec) < 0) {
        temp.tv_sec  = pSrc1->tv_sec - pSrc2->tv_sec - 1;
        temp.tv_nsec = GBL_TIME_NSECS_PER_SEC + pSrc1->tv_nsec - pSrc2->tv_nsec;
    } else {
        temp.tv_sec  = pSrc1->tv_sec - pSrc2->tv_sec;
        temp.tv_nsec = pSrc1->tv_nsec - pSrc2->tv_nsec;
    }

    return temp;
}

GBL_EXPORT GblBool GblDateTime_isUtc(const GblDateTime* pSelf) {
    return pSelf->utcOffset == 0.0;
}

GBL_EXPORT GblBool GblDateTime_isLocal(const GblDateTime* pSelf) {
    return pSelf->utcOffset == GblTime_localUtcOffset();
}

GBL_EXPORT void GblDate_set(GblDate* pSelf, GblYear year, GblMonth month, GblDay day) {
    pSelf->year = year; pSelf->month = month; pSelf->day = day;
}

GBL_EXPORT GblBool GblTime_isPm(const GblTime* pSelf) {
    return pSelf->hours >= 12;
}

GBL_EXPORT void (GblTime_set)(GblTime* pSelf, GblHour hours, GblMinute mins, GblSecond secs, GblNanoSecond ns) {
    pSelf->hours = hours; pSelf->minutes = mins; pSelf->seconds = secs; pSelf->nSeconds = ns;
}

GBL_EXPORT GblBool GblDateTime_isValid(const GblDateTime* pSelf) {
    return GblDate_isValid(&pSelf->date) && GblTime_isValid(&pSelf->time);
}

GBL_EXPORT GblDateTime* (GblDateTime_set)(GblDateTime* pSelf, GblYear year, GblMonth month, GblDay day, GblHour hours, GblMinute mins, GblSecond secs, GblNanoSecond ns, GblSecond tzOff) GBL_NOEXCEPT {
    GblDate_set(&pSelf->date, year, month, day);
    GblTime_set(&pSelf->time, hours, mins, secs, ns);
    pSelf->utcOffset = tzOff;
    return pSelf;
}

GBL_EXPORT void GblDateTime_setDate(GblDateTime* pSelf, const GblDate* pDate) {
    memcpy(&pSelf->date, pDate, sizeof(GblDate));
}

GBL_EXPORT void GblDateTime_setTime(GblDateTime* pSelf, const GblTime* pTime) {
    memcpy(&pSelf->time, pTime, sizeof(GblTime));
}

GBL_EXPORT int GblDateTime_compare(const GblDateTime* pSelf, const GblDateTime* pRhs) {
    GblTimeSpec ts = GblDateTime_diff(pSelf, pRhs);
    if(ts.tv_sec  > 0) return 1;
    if(ts.tv_sec  < 0) return -1;
    if(ts.tv_nsec > 0) return 1;
    if(ts.tv_nsec < 0) return -1;
    return 0;
}

GBL_EXPORT GblBool GblDateTime_equals(const GblDateTime* pSelf, const GblDateTime* pOther) {
    return GblDateTime_compare(pSelf, pOther) == 0;
}

GBL_EXPORT GblDateTime* GblDateTime_addMilliSecs(GblDateTime* pSelf, int mSecs) {
    return GblDateTime_addMicroSecs(pSelf, mSecs * 1000);
}

GBL_EXPORT GblDateTime* GblDateTime_addMicroSecs(GblDateTime* pSelf, int uSecs) {
    return GblDateTime_addNanoSecs(pSelf, uSecs * 1000);
}

GBL_EXPORT GblDateTime* (GblDateTime_create)(GblYear       year,
                                             GblMonth      month/*=1*/,
                                             GblDay        day  /*=1*/,
                                             GblHour       hours/*=0*/,
                                             GblMinute     mins /*=0*/,
                                             GblSecond     secs /*=0*/,
                                             GblNanoSecond ns   /*=0*/,
                                             GblSecond     tzOff/*=0*/)
{
    GblDateTime* pDt = GblRef_create(sizeof(GblDateTime));
    return GblDateTime_set(pDt,
                          year,
                          month,
                          day,
                          hours,
                          mins,
                          secs,
                          ns,
                          tzOff);

}

GBL_EXPORT GblDateTime* GblDateTime_ref(GblDateTime* pDt) {
    return GblRef_ref(pDt);
}

GBL_EXPORT GblRefCount GblDateTime_unref(GblDateTime* pDt) {
    return GblRef_unref(pDt);
}

static GBL_RESULT GblDateTime_convert_(const GblVariant* pVariant, GblVariant* pOther) {
    const GblType typeSrc = GblVariant_typeOf(pVariant);
    const GblType typeDst = GblVariant_typeOf(pOther);

    if(typeSrc == GBL_DATE_TIME_TYPE) {

        if(typeDst == GBL_STRING_TYPE) {
            struct {
                GblStringBuffer buff;
                char            stack[GBL_DATE_TIME_ISO8601_STRING_SIZE];
            } str;

            GblStringBuffer_construct(&str.buff, GBL_STRV(""), sizeof(str));
            //GblDateTime_fromUnix(&dt, pVariant->time);
            GblDateTime_toIso8601(pVariant->pDateTime, &str.buff);
            GblVariant_setString(pOther, GblStringBuffer_cString(&str.buff));
            GblStringBuffer_destruct(&str.buff);
        } else if(typeDst == GBL_UINT64_TYPE) {
            GblVariant_setUint64(pOther, GblDateTime_toUnix(pVariant->pDateTime));
        }

    } else if(typeSrc == GBL_STRING_TYPE) {
        GBL_ASSERT(typeDst == GBL_DATE_TIME_TYPE);

        GblDateTime dt;


    } else if(typeSrc == GBL_UINT64_TYPE) {
        GblDateTime_fromUnix(pOther->pDateTime, pVariant->u64);
    }

    return GBL_TRUE;
}

GBL_EXPORT GblType GblDateTime_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if(type == GBL_INVALID_TYPE) {
        type = GblOpaque_registerRef("GblDateTime");

        GblVariant_registerConverter(type, GBL_STRING_TYPE, GblDateTime_convert_);
        GblVariant_registerConverter(type, GBL_UINT64_TYPE, GblDateTime_convert_);
    }

    return type;

}


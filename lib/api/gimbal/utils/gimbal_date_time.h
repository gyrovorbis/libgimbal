/*! \file
 *  \brief Date and time conversions and utilities
 *  \ingroup utils
 *
 *  \todo
 *      - ISO8601 RFC
 *      - GblDateTime_parse can handle timezone delta
 *      - GblDateTime_format can handle timzeone delta
 *      - Renormalization function
 *      - timezone delta seconds might need a second/third try when values differ
 *      - double GblDateTime_toJulian(): higher res, with seconds
 */

#ifndef GIMBAL_DATE_TIME_H
#define GIMBAL_DATE_TIME_H

#include "../gimbal_core.h"
#include <time.h>

#define GBL_TIME_MSECS_PER_SEC                      1000
#define GBL_TIME_USECS_PER_SEC                      1000000
#define GBL_TIME_NSECS_PER_SEC                      1000000000

#define GBL_DATE_GREGORIAN_YEAR_FIRST               1582
#define GBL_DATE_UNIX_EPOCH_YEAR                    1970
#define GBL_DATE_UNIX_EPOCH_MONTH                   GBL_MONTH_JANUARY
#define GBL_DATE_UNIX_EPOCH_DAY                     1


#define GBL_DATE_TIME_BROKEN_DOWN_YEAR_FIRST        1900
#define GBL_DATE_TIME_BROKEN_DOWN_MONTH_OFFSET      -1
#define GBL_DATE_TIME_FORMAT_BUFFER_SIZE_INCREMENT  128

GBL_DECLS_BEGIN

GBL_DECLARE_ENUM(GblMonth) {
    GBL_MONTH_JANUARY = 1,
    GBL_MONTH_FEBRUARY,
    GBL_MONTH_MARCH,
    GBL_MONTH_APRIL,
    GBL_MONTH_MAY,
    GBL_MONTH_JUNE,
    GBL_MONTH_JULY,
    GBL_MONTH_AUGUST,
    GBL_MONTH_SEPTEMBER,
    GBL_MONTH_OCTOBER,
    GBL_MONTH_NOVEMBER,
    GBL_MONTH_DECEMBER,
    GBL_MONTH_COUNT = 12
};

GBL_DECLARE_ENUM(GblWeekDay) {
    GBL_WEEK_DAY_SUNDAY,
    GBL_WEEK_DAY_MONDAY,
    GBL_WEEK_DAY_TUESDAY,
    GBL_WEEK_DAY_WEDNESDAY,
    GBL_WEEK_DAY_THURSDAY,
    GBL_WEEK_DAY_FRIDAY,
    GBL_WEEK_DAY_SATURDAY,
    GBL_WEEK_DAY_COUNT
};

typedef int32_t GblYear;
typedef int32_t GblDay;
typedef double  GblSecond;
typedef int32_t GblMinute;
typedef int32_t GblHour;

typedef struct GblDate {
    GblYear  year;
    GblMonth month;
    GblDay   day;
} GblDate;

typedef struct GblTime {
    GblSecond seconds;
    GblMinute minutes;
    GblHour   hours;
} GblTime;

typedef struct GblDateTime {
    GblDate   date;
    GblTime   time;
    GblSecond utcOffset;
} GblDateTime;

GBL_INLINE GblBool     GblDate_isLeapYear      (GblYear year)             GBL_NOEXCEPT;
GBL_INLINE GblDay      GblDate_monthDays       (GblMonth month,
                                                GblYear year)             GBL_NOEXCEPT;
GBL_EXPORT const char* GblDate_monthStr        (GblMonth month)           GBL_NOEXCEPT;
GBL_EXPORT const char* GblDate_monthStrShort   (GblMonth month)           GBL_NOEXCEPT;
GBL_EXPORT const char* GblDate_weekDayStr      (GblWeekDay weekDay)       GBL_NOEXCEPT;
GBL_EXPORT const char* GblDate_weekDayStrShort (GblWeekDay weekDay)       GBL_NOEXCEPT;

GBL_INLINE GblBool     GblDate_isValid         (const GblDate* pSelf)     GBL_NOEXCEPT;
GBL_EXPORT GblBool     GblDate_isDst           (const GblDate* pSelf)     GBL_NOEXCEPT;
GBL_INLINE GblDay      GblDate_toJulian        (const GblDate* pSelf)     GBL_NOEXCEPT;
GBL_INLINE GblWeekDay  GblDate_weekDay         (const GblDate* pSelf)     GBL_NOEXCEPT;
GBL_INLINE GblDay      GblDate_yearDay         (const GblDate* pSelf)     GBL_NOEXCEPT;
GBL_INLINE uint8_t     GblDate_yearWeek        (const GblDate* pSelf)     GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT  GblDate_fromJulian      (GblDate* pSelf,
                                                GblDay julienDate)        GBL_NOEXCEPT;

GBL_INLINE void        GblDate_set             (GblDate* pSelf,
                                                GblYear  year,
                                                GblMonth month,
                                                GblDay   day)             GBL_NOEXCEPT;

GBL_EXPORT const char* GblTime_amPmStr         (GblBool isPm)             GBL_NOEXCEPT;
GBL_EXPORT GblSecond   GblTime_localUtcOffset  (void)                     GBL_NOEXCEPT;

GBL_INLINE GblBool     GblTime_isValid         (const GblTime* pSelf)     GBL_NOEXCEPT;
GBL_INLINE GblBool     GblTime_isPm            (const GblTime* pSelf)     GBL_NOEXCEPT;
GBL_EXPORT double      GblTime_milliSeconds    (const GblTime* pSelf)     GBL_NOEXCEPT;
GBL_EXPORT double      GblTime_microSeconds    (const GblTime* pSelf)     GBL_NOEXCEPT;
GBL_EXPORT double      GblTime_nanoSeconds     (const GblTime* pSelf)     GBL_NOEXCEPT;
GBL_INLINE void        GblTime_set             (GblTime*  pSelf,
                                                GblHour   hours,
                                                GblMinute mins,
                                                GblSecond secs)           GBL_NOEXCEPT;

GBL_INLINE GblBool     GblDateTime_isValid     (const GblDateTime* pSelf) GBL_NOEXCEPT;

GBL_INLINE GblBool     GblDateTime_isUtc       (const GblDateTime* pSelf) GBL_NOEXCEPT;
GBL_INLINE GblBool     GblDateTime_isLocal     (const GblDateTime* pSelf) GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblDateTime_fromUnix    (GblDateTime* pSelf,
                                                time_t epoch)             GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblDateTime_fromLocal   (GblDateTime* pSelf,
                                                const struct tm* pLocal)  GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblDateTime_fromUtc     (GblDateTime* pSelf,
                                                const struct tm* pUtc)    GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblDateTime_fromJulian  (GblDateTime* pSelf,
                                                double julianDate)        GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblDateTime_nowLocal    (GblDateTime* pSelf)       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblDateTime_nowUtc      (GblDateTime* pSelf)       GBL_NOEXCEPT;

GBL_EXPORT const char* GblDateTime_parse       (GblDateTime* pSelf,
                                                const char* pString,
                                                const char* pFormat)      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblDateTime_toUnix      (const GblDateTime* pSelf,
                                                time_t* pEpoch)           GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblDateTime_toLocal     (const GblDateTime* pSelf,
                                                struct tm* pBrokenDown)   GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblDateTime_toUtc       (const GblDateTime* pSelf,
                                                struct tm* pBrokenDown)   GBL_NOEXCEPT;

GBL_EXPORT double      GblDateTime_toJulian    (const GblDateTime* pSelf) GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblDateTime_format      (const GblDateTime* pSelf,
                                                GblStringBuffer* pBuffer,
                                                const char* pFormat)      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblDateTime_normalize   (GblDateTime* pSelf)       GBL_NOEXCEPT;

GBL_INLINE void        GblDateTime_set         (GblDateTime* pSelf,
                                                GblYear      year,
                                                GblMonth     month/*=1*/,
                                                GblDay       day  /*=1*/,
                                                GblHour      hours/*=0*/,
                                                GblMinute    mins /*=0*/,
                                                GblSecond    secs /*=0*/,
                                                GblSecond    tzOff/*=0*/) GBL_NOEXCEPT;

GBL_INLINE void        GblDateTime_setDate     (GblDateTime*   pSelf,
                                                const GblDate* pDate)     GBL_NOEXCEPT;

GBL_INLINE void        GblDateTime_setTime     (GblDateTime*   pSelf,
                                                const GblTime* pTime);    GBL_NOEXCEPT;

GBL_INLINE GblBool     GblDateTime_equals      (const GblDateTime* pSelf,
                                                const GblDateTime* pRhs)  GBL_NOEXCEPT;

GBL_EXPORT double      GblDateTime_diff        (const GblDateTime* pSelf,
                                                const GblDateTime* pRhs)  GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT  GblDateTime_add         (GblDateTime* pSelf,
                                                const GblDateTime* pRhs)  GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT  GblDateTime_addDays     (GblDateTime* pSelf,
                                                GblDay days)              GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT  GblDateTime_addHours    (GblDateTime* pSelf,
                                                GblHour hours)            GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT  GblDateTime_addMinutes  (GblDateTime* pSelf,
                                                GblMinute minutes)        GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT  GblDateTime_addSeconds  (GblDateTime* pSelf,
                                                GblSecond seconds)        GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT  GblDateTime_addWeeks    (GblDateTime* pSelf,
                                                int weeks)                GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT  GblDateTime_addMonths   (GblDateTime* pSelf,
                                                GblMonth months)          GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT  GblDateTime_addYears    (GblDateTime* pSelf,
                                                GblYear years)            GBL_NOEXCEPT;

// ===== IMPL =====
///\cond
#define GblDateTime_set_2(self, year) \
    (GblDateTime_set_3(self, year, GBL_MONTH_JANUARY))
#define GblDateTime_set_3(self, year, month) \
    (GblDatetime_set_4(self, year, month, 1))
#define GblDateTime_set_4(self, year, month, day) \
    (GblDateTime_set_5(self, year, month, day, 0))
#define GblDateTime_set_5(self, year, month, day, hour) \
    (GblDateTime_set_6(self, year, month, day, hour, 0)
#define GblDateTime_set_6(self, year, month, day, hour, min) \
    (GblDateTime_set_7(self, year, month, day, hour, min, 0.0))
#define GblDateTime_set_7(self, year, month, day, hour, min, sec) \
    (GblDateTime_set_8(self, year, month, day, hour, min, sec, 0.0)
#define GblDateTime_set_8(self, year, month, day, hour, min, sec, tz) \
    ((GblDateTime_set)(self, year, month, day, hour, min, sec, tz))
#define GblDate_time_set(...) \
    GBL_VA_OVERLOAD_CALL(GblDateTime_set, GBL_VA_OVERLOAD_SUFFIXER_ARGC), __VA_ARGS__)
///\endcond
///
GBL_INLINE GblBool GblDate_isLeapYear(GblYear year) GBL_NOEXCEPT {
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

GBL_INLINE GblDay GblDate_monthDays(GblMonth month, GblYear year) GBL_NOEXCEPT {
    if(month == GBL_MONTH_FEBRUARY) {
        return GblDate_isLeapYear(year)? 29 : 28;
    } else return (month > GBL_MONTH_JULY)? ((month&1)? 30: 31) : ((month&1)? 31: 30);
}

GBL_INLINE GblBool GblDate_isValid(const GblDate* pSelf) GBL_NOEXCEPT {
    if(pSelf->day <= 0)
        return GBL_FALSE;
    if(pSelf->month <= 0)
        return GBL_FALSE;
    //if(pSelf->year < 0)
    //    return GBL_FALSE;
    if(pSelf->month > GBL_MONTH_COUNT)
        GBL_FALSE;
    if(pSelf->day > GblDate_monthDays(pSelf->month, pSelf->year))
        return GBL_FALSE;
    return GBL_TRUE;
}

// Should work with non-Gregorian times?
GBL_INLINE GblDay GblDate_toJulian(const GblDate* pSelf) GBL_NOEXCEPT {
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

GBL_INLINE GblWeekDay GblDate_weekDay(const GblDate* pSelf) GBL_NOEXCEPT {
    GblDay d = pSelf->day;
    GblDay m = pSelf->month;
    GblDay y = pSelf->year;
    return (GblWeekDay)((d += m < 3 ? y-- : y - 2, 23*m/9 + d + 4 + y/4- y/100 + y/400)%7);
}

GBL_INLINE GblDay GblDate_yearDay(const GblDate* pSelf) GBL_NOEXCEPT {
    static const GblDay days[2][13] = {
        {0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},
        {0, 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335}
    };
    return days[GblDate_isLeapYear(pSelf->year)]
               [pSelf->month]
            + pSelf->day;
}

GBL_INLINE uint8_t GblDate_yearWeek(const GblDate* pSelf) GBL_NOEXCEPT {
    return GblDate_yearDay(pSelf)/7;
}

// MAYBE works with non-Gregorian times?
GBL_INLINE GBL_RESULT GblDate_fromJulian(GblDate* pSelf, GblDay julienDate) GBL_NOEXCEPT {
    int i, j, k, n, l;

    l = julienDate+68569;
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

    return GBL_RESULT_SUCCESS;
}

GBL_INLINE GblBool GblTime_isValid(const GblTime* pSelf) GBL_NOEXCEPT {
    if(pSelf->seconds < 0)
        return GBL_FALSE;
    if(pSelf->minutes < 0)
        return GBL_FALSE;
    if(pSelf->hours < 0)
        return GBL_FALSE;
    if(pSelf->seconds > 59)
        return GBL_FALSE;
    if(pSelf->minutes > 59)
        return GBL_FALSE;
    if(pSelf->hours > 23)
        return GBL_FALSE;
    return GBL_TRUE;
}

GBL_INLINE GblBool GblDateTime_isUtc(const GblDateTime* pSelf) GBL_NOEXCEPT {
    return pSelf->utcOffset == 0.0;
}

GBL_INLINE GblBool GblDateTime_isLocal(const GblDateTime* pSelf) GBL_NOEXCEPT {
    return pSelf->utcOffset == GblTime_localUtcOffset();
}

GBL_INLINE void GblDate_set(GblDate* pSelf, GblYear year, GblMonth month, GblDay day) GBL_NOEXCEPT {
    pSelf->year = year; pSelf->month = month; pSelf->day = day;
}

GBL_INLINE GblBool GblTime_isPm(const GblTime* pSelf) GBL_NOEXCEPT {
    return pSelf->hours >= 12;
}

GBL_INLINE void GblTime_set(GblTime* pSelf, GblHour hours, GblMinute mins, GblSecond secs) GBL_NOEXCEPT {
    pSelf->hours = hours; pSelf->minutes = mins; pSelf->seconds = secs;
}

GBL_INLINE GblBool GblDateTime_isValid(const GblDateTime* pSelf) GBL_NOEXCEPT {
    return GblDate_isValid(&pSelf->date) && GblTime_isValid(&pSelf->time);
}

GBL_INLINE void GblDateTime_set(GblDateTime* pSelf, GblYear year, GblMonth month, GblDay day, GblHour hours, GblMinute mins, GblSecond secs, GblSecond tzOff) GBL_NOEXCEPT {
    GblDate_set(&pSelf->date, year, month, day);
    GblTime_set(&pSelf->time, hours, mins, secs);
    pSelf->utcOffset = tzOff;
}

GBL_INLINE void GblDateTime_setDate(GblDateTime* pSelf, const GblDate* pDate) GBL_NOEXCEPT {
    memcpy(&pSelf->date, pDate, sizeof(GblDate));
}

GBL_INLINE void GblDateTime_setTime(GblDateTime* pSelf, const GblTime* pTime) GBL_NOEXCEPT {
    memcpy(&pSelf->time, pTime, sizeof(GblTime));
}

GBL_INLINE GblBool GblDateTime_equals(const GblDateTime* pSelf, const GblDateTime* pOther) GBL_NOEXCEPT {
    return (GblDateTime_diff(pSelf, pOther) == 0.0)? GBL_TRUE : GBL_FALSE;
}

GBL_DECLS_END


#endif // GIMBAL_DATE_TIME_H

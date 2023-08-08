#ifndef GIMBAL_DATE_TIME_HPP
#define GIMBAL_DATE_TIME_HPP

#include "gimbal_date_time.h"

namespace gbl {

enum class Month: GblEnum {
    January   = GBL_MONTH_JANUARY = 1,
    February  = GBL_MONTH_FEBRUARY,
    March     = GBL_MONTH_MARCH,
    April     = GBL_MONTH_APRIL,
    May       = GBL_MONTH_MAY,
    June      = GBL_MONTH_JUNE,
    July      = GBL_MONTH_JULY,
    August    = GBL_MONTH_AUGUST,
    September = GBL_MONTH_SEPTEMBER,
    October   = GBL_MONTH_OCTOBER,
    November  = GBL_MONTH_NOVEMBER,
    December  = GBL_MONTH_DECEMBER,
    Count     = GBL_MONTH_COUNT = 12
};

enum class WeekDay: GblEnum {
    Sunday    = GBL_WEEK_DAY_SUNDAY,
    Monday    = GBL_WEEK_DAY_MONDAY,
    Tuesday   = GBL_WEEK_DAY_TUESDAY,
    Wednesday = GBL_WEEK_DAY_WEDNESDAY,
    Thursday  = GBL_WEEK_DAY_THURSDAY,
    Friday    = GBL_WEEK_DAY_FRIDAY,
    Saturday  = GBL_WEEK_DAY_SATURDAY,
    Count     = GBL_WEEK_DAY_COUNT
};

using Year       = ::GblYear;
using Day        = ::GblDay;
using NanoSecond = ::GblNanoSecond;
using Second     = ::GblSecond;
using Minute     = ::GblMinute;
using Hour       = ::GblHour;

using TimeSpec   = ::GblTimeSpec;

TimeSpec  operator- (const TimeSpec& lhs, const TimeSpec& rhs);
TimeSpec& operator-=(TimeSpec& lhs, const TimeSpec& rhs);
TimeSpec  operator+ (const TimeSpec& lhs, const TimeSpec& rhs);
Timespec& operator+=(TimeSpec& lhs, const TimeSpec& rhs);

struct Date: public GblDate {
    static bool        isLeapYear(Year year);
    static Day         monthDays(Month month, Year year);
    static const char* monthStr(Month month);
    static const char* monthStrShort(Month month);
    static const char* weekDayStr(WeekDay weekday);
    static const char* weekDayStrShort(WeekDay weekday);

    static Date        fromJulian(Day julianDate);
    static Date        fromOrdinal(Year year, Day day);

    void               set(Year year, Month month, Day day);

    bool               isValid()  const;
    bool               isDst()    const;
    Day                toJulien() const;
    WeekDay            weekDay()  const;
    Day                yearDay()  const;
    uint8_t            yearWeek() const;
};

struct Time: public GblTime {
    static const char* amPmStr(bool isPm);
    static Second      localUtcOffset();

    void               set(Hour hours, Minute mins, Second secs, NanoSecond nsecs);

    bool               isValid() const;
    bool               isPm()    const;
};

struct DateTime: public GblDateTime {
    static DateTime fromUnix(std::time_t epoch);
    static DateTime fromSpecUtc(TimeSpec spec);
    static DateTime fromLocal(const std::tm& local);
    static DateTime fromUtc(const std::tm& utc);
    static DateTime nowLocal();
    static DateTime nowUtc();
    static DateTime parse(const char* pString, const char* pFormat);

    std::time_t toUnix() const;
    TimeSpec    toSpecUtc() const;
    void        toLocal(std::tm* pBrokenDown) const;
    void        toUtc(std::tm* pBrokenDown) const;
    std::string toIso8601(void) const;
    std::string format(const char* pFmt) const;

    bool        normalize();

    void        set(Year       year,
                    Month      month = Month::January,
                    Day        day   = 1,
                    Hour       hours = 0,
                    Minute     mins  = 0,
                    Second     secs  = 0,
                    NanoSecond ns    = 0,
                    Second     tzOff = 0);

    void        setDate(const Date& other);
    void        setTime(const Time& other);

    bool        isValid() const;
    bool        isUtc() const;
    bool        isLocal() const;

};

}


#endif // GIMBAL_DATE_TIME_HPP

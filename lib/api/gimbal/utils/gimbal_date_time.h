/*! \file
 *  \brief Date and time conversions and utilities
 *  \ingroup utils
 *  \todo
 *      - GblDateTime_fromIso8601()
 *      - double GblDateTime_toJulian(): higher res, with seconds
 *      - toIso8601() string corrupting with msec resolution
 *      - GblDateTime_format() support ms, us, ns
 *      - GblDateTime_parse() support ms, us, ns
 *      - are we misnaming GMT as UTC anywhere?
 *      - GstDateTime maintains whether fields have been set or not...
 *
 *  \author     2023 Falco Girgis
 *  \copyright  MIT License
 */
#ifndef GIMBAL_DATE_TIME_H
#define GIMBAL_DATE_TIME_H

#include "../gimbal_core.h"
#include "../platform/gimbal_time_.h"

#define GBL_DATE_TIME_TYPE          (GBL_TYPEID(GblDateTime))
#define GBL_DATE_TIME_CLASS(klass)  (GBL_CLASS_CAST(GblDateTime, klass))

/*! \name Second Conversions
 *  \brief Defines for second conversion factors
 *  @{
 */
#define GBL_TIME_MSECS_PER_SEC  1000        //!< Number of milliseconds per second
#define GBL_TIME_USECS_PER_SEC  1000000     //!< Number of microseconds per second
#define GBL_TIME_NSECS_PER_SEC  1000000000  //!< Number of nanoseconds per second
//! @}

/*! \name Start Dates
 *  \brief Initial dates of epochs
 *  @{
 */
#define GBL_DATE_GREGORIAN_YEAR_FIRST   1582                //!< First year in Gregorian calendar
#define GBL_DATE_UNIX_EPOCH_YEAR        1970                //!< Year of the Unix epoch
#define GBL_DATE_UNIX_EPOCH_MONTH       GBL_MONTH_JANUARY   //!< Month of the Unix epoch
#define GBL_DATE_UNIX_EPOCH_DAY         1                   //!< Day of the unix epoch
//! @}

/*! \name Broken-down Time
 *  \brief Defines for working with broken-down time
 *  @{
 */
#define GBL_DATE_TIME_BROKEN_DOWN_YEAR_FIRST    1900    //!< Year offset of a broken-down time's date
#define GBL_DATE_TIME_BROKEN_DOWN_YEAR_MIN      1970    //!< The oldest year a broken-down time can handle by spec
#define GBL_DATE_TIME_BROKEN_DOWN_MONTH_OFFSET  -1      //!< Month offset from GblMonth to a broken-down month
//! @}

/*! \name Formatting
 *  \brief Defines used when formatting and stringifying
 *  @{
 */
#define GBL_DATE_TIME_FORMAT_BUFFER_SIZE_INCREMENT      128 //!< Size increment when resizing format buffers
#define GBL_DATE_TIME_FORMAT_BUFFER_SIZE_MULTIPLIER_MAX 10  //!< Maximum number of size increments for format buffers
//! @}

#define GBL_DATE_TIME_ISO8601_STRING_SIZE   29 //!< Required buffer size for going to an ISO8601-formatted string

GBL_DECLS_BEGIN

//! Represents a month of the 12-month year
GBL_DECLARE_ENUM(GblMonth) {
    GBL_MONTH_JANUARY = 1,  //!< January
    GBL_MONTH_FEBRUARY,     //!< February
    GBL_MONTH_MARCH,        //!< March
    GBL_MONTH_APRIL,        //!< April
    GBL_MONTH_MAY,          //!< May
    GBL_MONTH_JUNE,         //!< June
    GBL_MONTH_JULY,         //!< July
    GBL_MONTH_AUGUST,       //!< August
    GBL_MONTH_SEPTEMBER,    //!< September
    GBL_MONTH_OCTOBER,      //!< October
    GBL_MONTH_NOVEMBER,     //!< November
    GBL_MONTH_DECEMBER,     //!< December
    GBL_MONTH_COUNT = 12    //!< Months/Year
};

//! Represents a day of the 7-day week
GBL_DECLARE_ENUM(GblWeekDay) {
    GBL_WEEK_DAY_SUNDAY,    //!< Sunday
    GBL_WEEK_DAY_MONDAY,    //!< Monday
    GBL_WEEK_DAY_TUESDAY,   //!< Tuesday
    GBL_WEEK_DAY_WEDNESDAY, //!< Wednesday
    GBL_WEEK_DAY_THURSDAY,  //!< Thursday
    GBL_WEEK_DAY_FRIDAY,    //!< Friday
    GBL_WEEK_DAY_SATURDAY,  //!< Saturday
    GBL_WEEK_DAY_COUNT      //!< Days/Week
};

typedef int32_t GblYear;        //!< Represents a calendar year
typedef int32_t GblDay;         //!< Represents a 24-hour day within a month (0-31)
typedef int32_t GblNanoSecond;  //!< Represents a nanosecond within a second (0-1000000000
typedef int64_t GblSecond;      //!< Represents a second in a 60-second minute (0-59)
typedef int32_t GblMinute;      //!< Represents a minute in a 60-minute hour (0-59)
typedef int32_t GblHour;        //!< Represents an hour within a 24-hour day (0-23)

//! Represents the difference between two GblTime instances
typedef struct timespec GblTimeSpec;

/*! \name  Type System
 *  \brief Type declarations for GblVariant time type
 *  @{
 */
GBL_DECLARE_TYPE(GblDateTime);
GBL_CLASS_DERIVE_EMPTY(GblDateTime, GblOpaque);
//! @}

/*! Represents a calendar date
 *  \ingroup utils
 */
typedef struct GblDate {
    GblYear  year;           //!< Year
    GblMonth month;          //!< Month of the year (1-12)
    GblDay   day;            //!< Day of the month (1-31)
} GblDate;                   // 12 Total bytes

/*! Represents a time-of-day
 *  \ingroup utils
 */
typedef struct GblTime {
    GblHour       hours;     //!< Hour within a day (0-23)
    GblMinute     minutes;   //!< Minutes within an hour (0-59)
    GblSecond     seconds;   //!< Seconds within a minute (0-59)
    GblNanoSecond nSeconds;  //!< Nanosconds within a second (0-1000000000)
} GblTime;                   // 20 Total bytes

/*! Represents a combined calendar date with time-of-day
 *  \ingroup utils
 */
typedef struct GblDateTime {
    GblDate   date;          //!< Date structure
    GblTime   time;          //!< Time structure
    int16_t   utcOffset;     //!< Timezone second offset from UTC
} GblDateTime;               // 34 Total bytes

/*! \name Date Static Methods
 *  \brief Non-member functions for date management
 *  @{
 */
//! Returns GBL_TRUE if the given year was a leap year, otherwise returns GBL_FALSE
GBL_EXPORT GblBool     GblDate_isLeapYear      (GblYear year)       GBL_NOEXCEPT;
//! Returns the number of days in the given \p month of the given \p year
GBL_EXPORT GblDay      GblDate_monthDays       (GblMonth month,
                                                GblYear  year)      GBL_NOEXCEPT;
//! Returns the string name of the given \p month
GBL_EXPORT const char* GblDate_monthStr        (GblMonth month)     GBL_NOEXCEPT;
//! Returns the short-handed string name of the given \p month
GBL_EXPORT const char* GblDate_monthStrShort   (GblMonth month)     GBL_NOEXCEPT;
//! Returns the string name of the given \p weekday
GBL_EXPORT const char* GblDate_weekDayStr      (GblWeekDay weekDay) GBL_NOEXCEPT;
//! Returns the short-handed string name of the given \p weekday
GBL_EXPORT const char* GblDate_weekDayStrShort (GblWeekDay weekDay) GBL_NOEXCEPT;
//! @}

/*! \name Date Methods
 *  \brief Member functions for GblDate
 *  \relatesalso GblDate
 *  @{
 */
//! Returns GBL_TRUE if the given structure represents a valid GblDate
GBL_EXPORT GblBool    GblDate_isValid     (const GblDate* pSelf) GBL_NOEXCEPT;
//! Returns GBL_TRUE if the given date is during daylight savings time
GBL_EXPORT GblBool    GblDate_isDst       (const GblDate* pSelf) GBL_NOEXCEPT;
//! Returns the Julian Day from the given date
GBL_EXPORT GblDay     GblDate_toJulian    (const GblDate* pSelf) GBL_NOEXCEPT;
//! Returns the day of the week for the given date
GBL_EXPORT GblWeekDay GblDate_weekDay     (const GblDate* pSelf) GBL_NOEXCEPT;
//! Returns the day-of-the-year (1-365) for the given date
GBL_EXPORT GblDay     GblDate_yearDay     (const GblDate* pSelf) GBL_NOEXCEPT;
//! Returns the week-of-the-year (1-52) for the given date
GBL_EXPORT uint8_t    GblDate_yearWeek    (const GblDate* pSelf) GBL_NOEXCEPT;
//! Converts the given julian date to a GblDate, storing it in \p pSelf and returning it
GBL_EXPORT GblDate*   GblDate_fromJulian  (GblDate* pSelf,
                                           GblDay   julianDate)  GBL_NOEXCEPT;
//! Converts the given ordinal date to a GblDate, storing it in \p pSelf and returning it
GBL_EXPORT GblDate*   GblDate_fromOrdinal (GblDate* pSelf,
                                           GblYear  year,
                                           GblDay   day)         GBL_NOEXCEPT;
//! Sets the given date structure, given its \p year, \p month, and \p day
GBL_EXPORT void       GblDate_set         (GblDate* pSelf,
                                           GblYear  year,
                                           GblMonth month,
                                           GblDay   day)         GBL_NOEXCEPT;
//! @}

/*! \name Time Static Methods
 *  \brief Non-member functions for time management
 *  @{
 */
//! Returns the string "PM" if \p isPm is GBL_TRUE, otherwise returns "AM"
GBL_EXPORT const char* GblTime_amPmStr        (GblBool isPm)             GBL_NOEXCEPT;
//! Returns the number of seconds the local time is offset from UTC time
GBL_EXPORT GblSecond   GblTime_localUtcOffset (void)                     GBL_NOEXCEPT;
//! Returns the difference between two NS-resolution GblTimeSpec values
GBL_EXPORT GblTimeSpec GblTime_specDiff       (const GblTimeSpec* pSrc1,
                                               const GblTimeSpec* pSrc2) GBL_NOEXCEPT;
//! @}

/*! \name Time Methods
 *  \brief Member functions for GblTime
 *  \relatesalso GblTime
 *  @{
 */
//! Returns GBL_TRUE if the given time structure represents a valid time
GBL_EXPORT GblBool GblTime_isValid (const GblTime* pSelf)     GBL_NOEXCEPT;
//! Returns GBL_TRUE if the given GblTime is PM, false if it's AM
GBL_EXPORT GblBool GblTime_isPm    (const GblTime* pSelf)     GBL_NOEXCEPT;
//! Sets the values of the given GblTime structure, with nsecs defaulting to 0
GBL_EXPORT void    GblTime_set     (GblTime*      pSelf,
                                    GblHour       hours,
                                    GblMinute     mins,
                                    GblSecond     secs,
                                    GblNanoSecond nsec/*=0*/) GBL_NOEXCEPT;
//! @}

/*! \name Read Accessors
 *  \brief Methods for getting values
 *  \relatesalso GblDateTime
 *  @{
 */
//! Returns GBL_TRUE if the given GblDateTime structure contains both a valid date and time
GBL_EXPORT GblBool GblDateTime_isValid (const GblDateTime* pSelf) GBL_NOEXCEPT;
//! Returns GBL_TRUE if the given date/time represents a date and time within the UTC timezone
GBL_EXPORT GblBool GblDateTime_isUtc   (const GblDateTime* pSelf) GBL_NOEXCEPT;
//! Returns GBL_TRUE if the given date/time represents a date and time in the local timezone
GBL_EXPORT GblBool GblDateTime_isLocal (const GblDateTime* pSelf) GBL_NOEXCEPT;
//! @}

/*! \name Dynamic Lifetime
 *  \brief Methods for managing shared references
 *  \relatesalso GblDateTime
 *  @{
 */
//! Creates and allocates a new shared reference to a GblDateTime
GBL_EXPORT GblDateTime* GblDateTime_create (GblYear       year,
                                            GblMonth      month/*=1*/,
                                            GblDay        day  /*=1*/,
                                            GblHour       hours/*=0*/,
                                            GblMinute     mins /*=0*/,
                                            GblSecond     secs /*=0*/,
                                            GblNanoSecond ns   /*=0*/,
                                            GblSecond     tzOff/*=0*/) GBL_NOEXCEPT;
//! Returns a newly acquired reference to the given GblDateTime structure
GBL_EXPORT GblDateTime* GblDateTime_ref    (GblDateTime* pDt)          GBL_NOEXCEPT;
//! Decrements the refCounter by 1, destroying the struct at 0, and returning the new refCount value
GBL_EXPORT GblRefCount  GblDateTime_unref  (GblDateTime* pDt)          GBL_NOEXCEPT;
//! @}

/*! \name Initialization
 *  \brief Methods for constructing and populating
 *  \relatesalso GblDateTime
 *  @{
 */
//! Sets the given date/time structure from a unix epoch time, returning a pointer to it
GBL_EXPORT GblDateTime* GblDateTime_fromUnix    (GblDateTime* pSelf,
                                                 time_t       epoch)       GBL_NOEXCEPT;
//! Sets the given date/time from a GblTimeSpec value, returning a pointer to it
GBL_EXPORT GblDateTime* GblDateTime_fromSpecUtc (GblDateTime*       pSelf,
                                                 const GblTimeSpec* pSpec) GBL_NOEXCEPT;
//! Sets the given date/time from a local broken-down time, returning a pointer to it
GBL_EXPORT GblDateTime* GblDateTime_fromLocal   (GblDateTime*     pSelf,
                                                 const struct tm* pLocal)  GBL_NOEXCEPT;
//! Sets the given date/time from a UTC broken-down time, returning a pointer to it
GBL_EXPORT GblDateTime* GblDateTime_fromUtc     (GblDateTime*     pSelf,
                                                 const struct tm* pUtc)    GBL_NOEXCEPT;
//! Sets the given date/time to the current local time, returning a pointer to it
GBL_EXPORT GblDateTime* GblDateTime_nowLocal    (GblDateTime* pSelf)       GBL_NOEXCEPT;
//! Sets the given date/time to the current UTC time, returning a pointer to it
GBL_EXPORT GblDateTime* GblDateTime_nowUtc      (GblDateTime* pSelf)       GBL_NOEXCEPT;
//! Parses the given string using the strptime() formatter, storing and returning \p pSelf
GBL_EXPORT GblDateTime* GblDateTime_parse       (GblDateTime* pSelf,
                                                 const char*  pString,
                                                 const char*  pFormat)     GBL_NOEXCEPT;
//! @}

/*! \name Conversions
 *  \brief Methods for converting to another format
 *  \relatesalso GblDateTime
 *  @{
 */
//! Converts the given GblDateTime to a unix epoch time
GBL_EXPORT time_t      GblDateTime_toUnix    (const GblDateTime* pSelf) GBL_NOEXCEPT;
//! Converts the given GblDateTime to a timespec UTC time
GBL_EXPORT GblTimeSpec GblDateTime_toSpecUtc (const GblDateTime* pSelf) GBL_NOEXCEPT;
//! Converts the given GblDateTime to a broken-down local time
GBL_EXPORT GBL_RESULT  GblDateTime_toLocal   (const GblDateTime* pSelf,
                                              struct tm*         pTm)   GBL_NOEXCEPT;
//! Converts the given GblDateTime to a broken-down UTC time
GBL_EXPORT GBL_RESULT  GblDateTime_toUtc     (const GblDateTime* pSelf,
                                              struct tm*         pTm)   GBL_NOEXCEPT;
//! Fills in the given buffer with the ISO8601 representation of the given date/time, return its C string
GBL_EXPORT const char* GblDateTime_toIso8601 (const GblDateTime* pSelf,
                                              GblStringBuffer*   pBuff) GBL_NOEXCEPT;
//! Fills in the given buffer with the strftime()-formatted GblDateTime, returning its C string
GBL_EXPORT const char* GblDateTime_format    (const GblDateTime* pSelf,
                                              GblStringBuffer*   pBuffer,
                                              const char*        pFmt)  GBL_NOEXCEPT;
//! @}

/*! \name Write Accessors
 *  \brief Methods for writing values
 *  \relatesalso GblDateTime
 *  @{
 */
//! Ensures all values within the given GblDateTime are within range and returns a pointer to it
GBL_EXPORT GblDateTime* GblDateTime_normalize (GblDateTime* pSelf)        GBL_NOEXCEPT;
//! Sets the GblDate value of the given GblDateTime to \p pDate
GBL_EXPORT void         GblDateTime_setDate   (GblDateTime*   pSelf,
                                               const GblDate* pDate)      GBL_NOEXCEPT;
//! Sets the GblTime value of the given GblDateTime to \p pTime
GBL_EXPORT void         GblDateTime_setTime   (GblDateTime*   pSelf,
                                               const GblTime* pTime)      GBL_NOEXCEPT;
//! Sets the values of the given GblDateTime, with default argument values provided
GBL_EXPORT GblDateTime* GblDateTime_set       (GblDateTime*  pSelf,
                                               GblYear       year,
                                               GblMonth      month/*=1*/,
                                               GblDay        day  /*=1*/,
                                               GblHour       hours/*=0*/,
                                               GblMinute     mins /*=0*/,
                                               GblSecond     secs /*=0*/,
                                               GblNanoSecond ns   /*=0*/,
                                               GblSecond     tzOff/*=0*/) GBL_NOEXCEPT;
//! @}

/*! \name Operators
 *  \brief Methods implementing operations
 *  \relatesalso GblDateTime
 *  @{
 */
//! Lexacographically compares the two GblDateTimes, returning 0, -1, or 1
GBL_EXPORT int          GblDateTime_compare      (const GblDateTime* pSelf,
                                                  const GblDateTime* pRhs) GBL_NOEXCEPT;
//! Returns GBL_TRUE if the two provided GblDateTimes have the same value
GBL_EXPORT GblBool      GblDateTime_equals       (const GblDateTime* pSelf,
                                                  const GblDateTime* pRhs) GBL_NOEXCEPT;
//! Returns the NS-resolution GblTimeSpec difference between two GblDateTime values
GBL_EXPORT GblTimeSpec  GblDateTime_diff         (const GblDateTime* pSelf,
                                                  const GblDateTime* pRhs) GBL_NOEXCEPT;
//! Adds the \p pRhs GblDateTime to \p pSelf, returning a pointer to \p pSelf
GBL_EXPORT GblDateTime* GblDateTime_add          (GblDateTime*       pSelf,
                                                  const GblDateTime* pRhs) GBL_NOEXCEPT;
//! Adds the given number of \p days to the given GblDateTime, returning a pointer to it
GBL_EXPORT GblDateTime* GblDateTime_addDays      (GblDateTime* pSelf,
                                                  GblDay       days)       GBL_NOEXCEPT;
//! Adds the given number of \p hours to the given GblDateTime, returning a pointer to it
GBL_EXPORT GblDateTime* GblDateTime_addHours     (GblDateTime* pSelf,
                                                  GblHour      hours)      GBL_NOEXCEPT;
//! Adds the given number of \p minutes to the given GblDateTime, returning a pointer to it
GBL_EXPORT GblDateTime* GblDateTime_addMinutes   (GblDateTime* pSelf,
                                                  GblMinute    minutes)    GBL_NOEXCEPT;
//! Adds the given number of \p seconds to the given GblDateTime, returning a pointer to it
GBL_EXPORT GblDateTime* GblDateTime_addSeconds   (GblDateTime* pSelf,
                                                  GblSecond    seconds)    GBL_NOEXCEPT;
//! Adds the given number of milliseconds to the given GblDateTime, returning a pointer to it
GBL_EXPORT GblDateTime* GblDateTime_addMilliSecs (GblDateTime* pSelf,
                                                  int          mSecs)      GBL_NOEXCEPT;
//! Adds the given number of microseconds to the given GblDateTime, returning a pointer to it
GBL_EXPORT GblDateTime* GblDateTime_addMicroSecs (GblDateTime* pSelf,
                                                  int          uSecs)      GBL_NOEXCEPT;
//! Adds the given number of nanoseconds s to the given GblDateTime, returning a pointer to it
GBL_EXPORT GblDateTime* GblDateTime_addNanoSecs  (GblDateTime* pSelf,
                                                  GblNanoSecond nSecs)     GBL_NOEXCEPT;
//! Adds the given number of \p weeks to the given GblDateTime, returning a pointer to it
GBL_EXPORT GblDateTime* GblDateTime_addWeeks     (GblDateTime* pSelf,
                                                  int          weeks)      GBL_NOEXCEPT;
//! Adds the given number of \p months to the given GblDateTime, returning a pointer to it
GBL_EXPORT GblDateTime* GblDateTime_addMonths    (GblDateTime* pSelf,
                                                  int          months)     GBL_NOEXCEPT;
//! Adds the given number of \p years to the given GblDateTime, returning a pointer to it
GBL_EXPORT GblDateTime* GblDateTime_addYears     (GblDateTime* pSelf,
                                                  GblYear      years)      GBL_NOEXCEPT;
//! @}

GBL_DECLS_END

///\cond
#define GblTime_set_5(self, hour, min, sec, ns) \
    (GblTime_set)(self, hour, min, sec, ns)
#define GblTime_set_4(self, hour, min, sec) \
    (GblTime_set_5(self, hour, min, sec, 0))
#define GblTime_set(...) \
    GBL_VA_OVERLOAD_CALL_ARGC(GblTime_set, __VA_ARGS__)

#define GblDateTime_create_1(year) \
    (GblDateTime_create_2(year, GBL_MONTH_JANUARY))
#define GblDateTime_create_2(year, month) \
    (GblDateTime_create_3(year, month, 1))
#define GblDateTime_create_3(year, month, day) \
    (GblDateTime_create_4(year, month, day, 0))
#define GblDateTime_create_4(year, month, day, hour) \
    (GblDateTime_create_5(year, month, day, hour, 0))
#define GblDateTime_create_5(year, month, day, hour, min) \
    (GblDateTime_create_6(year, month, day, hour, min, 0))
#define GblDateTime_create_6(year, month, day, hour, min, sec) \
    (GblDateTime_create_7(year, month, day, hour, min, sec, 0))
#define GblDateTime_create_7(year, month, day, hour, min, sec, ns) \
    (GblDateTime_create_8(year, month, day, hour, min, sec, ns, 0))
#define GblDateTime_create_8(year, month, day, hour, min, sec, ns, tz) \
    ((GblDateTime_create)(year, month, day, hour, min, sec, ns, tz))
#define GblDateTime_create(...) \
    GBL_VA_OVERLOAD_CALL_ARGC(GblDateTime_create, __VA_ARGS__)

#define GblDateTime_set_2(self, year) \
    (GblDateTime_set_3(self, year, GBL_MONTH_JANUARY))
#define GblDateTime_set_3(self, year, month) \
    (GblDatetime_set_4(self, year, month, 1))
#define GblDateTime_set_4(self, year, month, day) \
    (GblDateTime_set_5(self, year, month, day, 0))
#define GblDateTime_set_5(self, year, month, day, hour) \
    (GblDateTime_set_6(self, year, month, day, hour, 0))
#define GblDateTime_set_6(self, year, month, day, hour, min) \
    (GblDateTime_set_7(self, year, month, day, hour, min, 0))
#define GblDateTime_set_7(self, year, month, day, hour, min, sec) \
    (GblDateTime_set_8(self, year, month, day, hour, min, sec, 0))
#define GblDateTime_set_8(self, year, month, day, hour, min, sec, ns) \
    (GblDateTime_set_9(self, year, month, day, hour, min, sec, ns, 0))
#define GblDateTime_set_9(self, year, month, day, hour, min, sec, ns, tz) \
    ((GblDateTime_set)(self, year, month, day, hour, min, sec, ns, tz))
#define GblDateTime_set(...) \
    GBL_VA_OVERLOAD_CALL_ARGC(GblDateTime_set, __VA_ARGS__)
///\endcond

#endif // GIMBAL_DATE_TIME_H

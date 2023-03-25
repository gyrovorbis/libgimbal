#include "utils/gimbal_date_time_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/utils/gimbal_date_time.h>
#include <gimbal/strings/gimbal_string_buffer.h>
#include <time.h>

#define GBL_TEST_SUITE_SELF GblDateTimeTestSuite

GBL_TEST_FIXTURE {
    unsigned dummy;
};

GBL_TEST_INIT() {
    GBL_TEST_CASE_END;
}

GBL_TEST_FINAL() {
    GBL_TEST_CASE_END;
}

static GBL_RESULT verifyTime_(GblTestSuite* pSelf, const GblTime* pTime, GblHour hours, GblMinute mins, GblSecond secs, GblNanoSecond nsecs) {
    GBL_CTX_BEGIN(pSelf);
    GBL_TEST_COMPARE(pTime->hours, hours);
    GBL_TEST_COMPARE(pTime->minutes, mins);
    GBL_TEST_COMPARE(pTime->seconds, secs);
    GBL_TEST_COMPARE(pTime->nSeconds, nsecs);
    GBL_CTX_END();
}

static GBL_RESULT verifyDate_(GblTestSuite* pSelf, const GblDate* pDate, GblYear year, GblMonth month, GblDay day) {
    GBL_CTX_BEGIN(pSelf);
    GBL_TEST_COMPARE(pDate->year, year);
    GBL_TEST_COMPARE(pDate->month, month);
    GBL_TEST_COMPARE(pDate->day, day);
    GBL_CTX_END();
}

static GBL_RESULT verifyDateTime_(GblTestSuite* pSelf, const GblDateTime* pDateTime, GblYear year, GblMonth month, GblDay day, GblHour hours, GblMinute mins, GblSecond secs, GblNanoSecond nsecs, GblSecond utcOffset) {
    GBL_CTX_BEGIN(pSelf);
    GBL_TEST_CALL(verifyDate_(pSelf, &pDateTime->date, year, month, day));
    GBL_TEST_CALL(verifyTime_(pSelf, &pDateTime->time, hours, mins, secs, nsecs));
    GBL_TEST_COMPARE(pDateTime->utcOffset, utcOffset);
    GBL_CTX_END();
}

GBL_TEST_CASE(dateIsLeapYear) {
    GBL_TEST_VERIFY(!GblDate_isLeapYear(1583));
    GBL_TEST_VERIFY(GblDate_isLeapYear(1584));
    GBL_TEST_VERIFY(GblDate_isLeapYear(1904));
    GBL_TEST_VERIFY(!GblDate_isLeapYear(1903));
    GBL_TEST_VERIFY(!GblDate_isLeapYear(1630));
    GBL_TEST_VERIFY(GblDate_isLeapYear(1632));
    GBL_TEST_VERIFY(!GblDate_isLeapYear(2023));
    GBL_TEST_VERIFY(GblDate_isLeapYear(2024));
    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(dateMonthDays) {
    GBL_TEST_COMPARE(GblDate_monthDays(GBL_MONTH_JANUARY, 1583), 31); // normal year
    GBL_TEST_COMPARE(GblDate_monthDays(GBL_MONTH_JANUARY, 1584), 31); // leap year
    GBL_TEST_COMPARE(GblDate_monthDays(GBL_MONTH_FEBRUARY, 1583), 28);
    GBL_TEST_COMPARE(GblDate_monthDays(GBL_MONTH_FEBRUARY, 1584), 29);
    GBL_TEST_COMPARE(GblDate_monthDays(GBL_MONTH_MARCH, 1583), 31);
    GBL_TEST_COMPARE(GblDate_monthDays(GBL_MONTH_MARCH, 1584), 31);
    GBL_TEST_COMPARE(GblDate_monthDays(GBL_MONTH_APRIL, 1583), 30);
    GBL_TEST_COMPARE(GblDate_monthDays(GBL_MONTH_APRIL, 1584), 30);
    GBL_TEST_COMPARE(GblDate_monthDays(GBL_MONTH_MAY, 1583), 31);
    GBL_TEST_COMPARE(GblDate_monthDays(GBL_MONTH_MAY, 1584), 31);
    GBL_TEST_COMPARE(GblDate_monthDays(GBL_MONTH_JUNE, 1583), 30);
    GBL_TEST_COMPARE(GblDate_monthDays(GBL_MONTH_JUNE, 1584), 30);
    GBL_TEST_COMPARE(GblDate_monthDays(GBL_MONTH_JULY, 1583), 31);
    GBL_TEST_COMPARE(GblDate_monthDays(GBL_MONTH_JULY, 1584), 31);
    GBL_TEST_COMPARE(GblDate_monthDays(GBL_MONTH_AUGUST, 1583), 31);
    GBL_TEST_COMPARE(GblDate_monthDays(GBL_MONTH_AUGUST, 1584), 31);
    GBL_TEST_COMPARE(GblDate_monthDays(GBL_MONTH_SEPTEMBER, 1583), 30);
    GBL_TEST_COMPARE(GblDate_monthDays(GBL_MONTH_SEPTEMBER, 1584), 30);
    GBL_TEST_COMPARE(GblDate_monthDays(GBL_MONTH_OCTOBER, 1583), 31);
    GBL_TEST_COMPARE(GblDate_monthDays(GBL_MONTH_OCTOBER, 1584), 31);
    GBL_TEST_COMPARE(GblDate_monthDays(GBL_MONTH_NOVEMBER, 1583), 30);
    GBL_TEST_COMPARE(GblDate_monthDays(GBL_MONTH_NOVEMBER, 1584), 30);
    GBL_TEST_COMPARE(GblDate_monthDays(GBL_MONTH_DECEMBER, 1583), 31);
    GBL_TEST_COMPARE(GblDate_monthDays(GBL_MONTH_DECEMBER, 1584), 31);
    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(dateMonthString) {
    GBL_TEST_COMPARE(GblDate_monthStr(GBL_MONTH_JANUARY), "January");
    GBL_TEST_COMPARE(GblDate_monthStr(GBL_MONTH_DECEMBER), "December");

    GBL_TEST_COMPARE(GblDate_monthStrShort(GBL_MONTH_JANUARY), "Jan");
    GBL_TEST_COMPARE(GblDate_monthStrShort(GBL_MONTH_DECEMBER), "Dec");
    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(dateWeekDayString) {
    GBL_TEST_COMPARE(GblDate_weekDayStr(GBL_WEEK_DAY_SUNDAY), "Sunday");
    GBL_TEST_COMPARE(GblDate_weekDayStr(GBL_WEEK_DAY_SATURDAY), "Saturday");

    GBL_TEST_COMPARE(GblDate_weekDayStrShort(GBL_WEEK_DAY_SUNDAY), "Sun");
    GBL_TEST_COMPARE(GblDate_weekDayStrShort(GBL_WEEK_DAY_SATURDAY), "Sat");
    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(dateToJulian) {
    GblDate date;

    GblDate_set(&date, 1672, 6, 17);
    GBL_TEST_COMPARE(GblDate_toJulian(&date), 2331914);
    GblDate_set(&date, 1867, 11, 15);
    GBL_TEST_COMPARE(GblDate_toJulian(&date), 2403286);
    GblDate_set(&date, 1904, GBL_MONTH_FEBRUARY, 29);  // leap year date
    GBL_TEST_COMPARE(GblDate_toJulian(&date), 2416540);
    GblDate_set(&date, 2023, 1, 24);
    GBL_TEST_COMPARE(GblDate_toJulian(&date), 2459969);
    GblDate_set(&date, 2023, 1, 25);
    GBL_TEST_COMPARE(GblDate_toJulian(&date), 2459970);

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(dateWeekDay) {
    GblDate date;

    GblDate_set(&date, 2023, GBL_MONTH_JANUARY, 24);
    GBL_TEST_COMPARE(GblDate_weekDay(&date), GBL_WEEK_DAY_TUESDAY);

    GblDate_set(&date, 1904, GBL_MONTH_FEBRUARY, 29);  // leap year date
    GBL_TEST_COMPARE(GblDate_weekDay(&date), GBL_WEEK_DAY_MONDAY);

    GblDate_set(&date, 1672, 6, 17);
    GBL_TEST_COMPARE(GblDate_weekDay(&date), GBL_WEEK_DAY_FRIDAY);

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(dateYearDayAndWeek) {
    GblDate date;

    GblDate_set(&date, 2023, GBL_MONTH_JANUARY, 24);
    GBL_TEST_COMPARE(GblDate_yearDay(&date), 24);
    GBL_TEST_COMPARE(GblDate_yearWeek(&date), 24/7);

    GblDate_set(&date, 1904, GBL_MONTH_FEBRUARY, 29);  // leap year date
    GBL_TEST_COMPARE(GblDate_yearDay(&date), 60);
    GBL_TEST_COMPARE(GblDate_yearWeek(&date), 60/7);

    GblDate_set(&date, 1672, 6, 17);
    GBL_TEST_COMPARE(GblDate_yearDay(&date), 169);
    GBL_TEST_COMPARE(GblDate_yearWeek(&date), 169/7);

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(dateFromJulian) {
    GblDate date;

    GblDate_set(&date, 1672, 6, 17);
    GblDate_fromJulian(&date, GblDate_toJulian(&date));
    GBL_TEST_CALL(verifyDate_(pSelf, &date, 1672, 6, 17));

    GblDate_set(&date, 1867, 11, 15);
    GblDate_fromJulian(&date, GblDate_toJulian(&date));
    GBL_TEST_CALL(verifyDate_(pSelf, &date, 1867, 11, 15));

    GblDate_set(&date, 1904, GBL_MONTH_FEBRUARY, 29);  // leap year date
    GblDate_fromJulian(&date, GblDate_toJulian(&date));
    GBL_TEST_CALL(verifyDate_(pSelf, &date, 1904, GBL_MONTH_FEBRUARY, 29));

    GblDate_set(&date, 2023, 1, 24);
    GblDate_fromJulian(&date, GblDate_toJulian(&date));
    GBL_TEST_CALL(verifyDate_(pSelf, &date, 2023, 1, 24));

    GblDate_set(&date, 2023, 1, 25);
    GblDate_fromJulian(&date, GblDate_toJulian(&date));
    GBL_TEST_CALL(verifyDate_(pSelf, &date, 2023, 1, 25));

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(dateFromOrdinal) {
    GblDate date;

    GBL_TEST_VERIFY(GblDate_fromOrdinal(&date, 1997, 256));

    GBL_TEST_CALL(verifyDate_(pSelf, &date, 1997, GBL_MONTH_SEPTEMBER, 13));

    GBL_TEST_COMPARE(GblDate_weekDay(&date), GBL_WEEK_DAY_SATURDAY);

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(timeStr) {
    GBL_TEST_COMPARE(GblTime_amPmStr(GBL_FALSE), "AM");
    GBL_TEST_COMPARE(GblTime_amPmStr(GBL_TRUE),  "PM");
    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(timeInitDefault) {
    const GblTime time = { 0 };
    GBL_TEST_VERIFY(GblTime_isValid(&time));
    GBL_TEST_CALL(verifyTime_(pSelf, &time, 0, 0, 0, 0));
    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(timeIsPm) {
    const GblTime time = { .hours = 13 };
    GBL_TEST_VERIFY(GblTime_isPm(&time));
    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(dateTimeInitDefault) {
    const GblDateTime dt = { 0 };

    GBL_TEST_VERIFY(!GblDateTime_isValid(&dt));

    GBL_TEST_CALL(verifyDateTime_(pSelf, &dt, 0, 0, 0, 0, 0, 0.0, 0, 0.0));

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(dateTimeIsUtc) {
    GblDateTime dt = { 0 };

    GBL_TEST_VERIFY(GblDateTime_isUtc(&dt));

    dt.utcOffset = -180.00;
    GBL_TEST_VERIFY(!GblDateTime_isUtc(&dt));

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(dateTimeNormalizeDefault) {
    GblDateTime dt = { 0 };

    GBL_TEST_VERIFY(GblDateTime_normalize(&dt));
    GBL_TEST_VERIFY(GblDateTime_isValid(&dt));

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(dateTimeEquals) {
    GblDateTime dt = { 0 };
    GBL_TEST_VERIFY(GblDateTime_equals(&dt, &dt));

    GBL_TEST_VERIFY(GblDateTime_normalize(&dt));
    GBL_TEST_VERIFY(GblDateTime_equals(&dt, &dt));

    gblRandBuffer(&dt, sizeof(dt));
    GBL_TEST_VERIFY(GblDateTime_equals(&dt, &dt));

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(dateTimeFromUnix) {
    GblDateTime dt;

    GBL_TEST_VERIFY(GblDateTime_fromUnix(&dt, 0));
    GBL_TEST_CALL(verifyDateTime_(pSelf, &dt,
                                  GBL_DATE_UNIX_EPOCH_YEAR, GBL_DATE_UNIX_EPOCH_MONTH, GBL_DATE_UNIX_EPOCH_DAY,
                                  0, 0, 0, 0.0, 0.0));

    GBL_TEST_VERIFY(GblDateTime_fromUnix(&dt, 1674637231));
    GBL_TEST_CALL(verifyDateTime_(pSelf, &dt, 2023, 1, 25, 9, 0, 31.0, 0, 0.0));

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(dateTimeFromLocal) {
    GblDateTime dt;
    struct tm bdTime = { .tm_year = GBL_DATE_TIME_BROKEN_DOWN_YEAR_MIN - GBL_DATE_TIME_BROKEN_DOWN_YEAR_FIRST + 4, .tm_mday = 1, .tm_isdst = -1 };

    GBL_TEST_VERIFY(GblDateTime_fromLocal(&dt, &bdTime));
    GBL_TEST_VERIFY(GblDateTime_isLocal(&dt));
    GBL_TEST_CALL(verifyDateTime_(pSelf, &dt,
                                  GBL_DATE_TIME_BROKEN_DOWN_YEAR_MIN + 4, GBL_MONTH_JANUARY, 1,
                                  0, 0, 0.0, 0, GblTime_localUtcOffset()));

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(dateTimeFromUtc) {
    GblDateTime dt;

    struct tm bdTime = { .tm_year = GBL_DATE_TIME_BROKEN_DOWN_YEAR_MIN - GBL_DATE_TIME_BROKEN_DOWN_YEAR_FIRST + 1, .tm_mday = 1, .tm_isdst = -1 };

    GBL_TEST_VERIFY(GblDateTime_fromUtc(&dt, &bdTime));
    GBL_TEST_VERIFY(GblDateTime_isUtc(&dt));
    GBL_TEST_CALL(verifyDateTime_(pSelf, &dt,
                                  GBL_DATE_TIME_BROKEN_DOWN_YEAR_MIN+1, GBL_MONTH_JANUARY, 1,
                                  0, 0, 0.0, 0, 0.0));


    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(dateTimeNowLocal) {
    GblDateTime dt;
    GBL_TEST_VERIFY(GblDateTime_nowLocal(&dt));
    GBL_TEST_VERIFY(GblDateTime_isValid(&dt));
    GBL_TEST_VERIFY(GblDateTime_isLocal(&dt));
    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(dateTimeNowUtc) {
    GblDateTime dt;
    GBL_TEST_VERIFY(GblDateTime_nowUtc(&dt));
    GBL_TEST_VERIFY(GblDateTime_isValid(&dt));
    GBL_TEST_VERIFY(GblDateTime_isUtc(&dt));
    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(dateTimeParse) {
    GblDateTime dt[4];
    GblDateTime_parse(&dt[0], "2011-02-21", "%Y-%m-%d");
    GBL_TEST_CALL(verifyDateTime_(pSelf, &dt[0],
                                  2011, 2, 21,
                                  0, 0, 0.0, 0, 0.0));

    GblDateTime_parse(&dt[1], "Monday, 3/4/1995 4:38:33 PM", "%A, %m/%d/%Y %I:%M:%S %p");
    GBL_TEST_CALL(verifyDateTime_(pSelf, &dt[1],
                                  1995, 3, 4,
                                  16, 38, 33.0, 0, 0.0));

    GblDateTime_parse(&dt[2], "March 4, 1995 16.38.33 Mon", "%B %d, %Y %H.%M.%S %a");
    GBL_TEST_CALL(verifyDateTime_(pSelf, &dt[2],
                                  1995, 3, 4,
                                  16, 38, 33.0, 0, 0.0));

    GBL_TEST_VERIFY(GblDateTime_equals(&dt[1], &dt[2]));

    GblDateTime_parse(&dt[2],
                     "March 4, 2277 16.38.33 Mon -0530",
                     "%B %d, %Y %H.%M.%S %a %z");

    GBL_TEST_CALL(verifyDateTime_(pSelf, &dt[2],
                                  2277, 3, 4,
                                  16, 38, 33, 0, -330));

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(dateTimeToUnix) {
    GblDateTime dt;
    GblDateTime_set(&dt,
                    GBL_DATE_UNIX_EPOCH_YEAR,
                    GBL_DATE_UNIX_EPOCH_MONTH,
                    GBL_DATE_UNIX_EPOCH_DAY);

    GBL_TEST_COMPARE(GblDateTime_toUnix(&dt), 0);

    GblDateTime_set(&dt,
                    2023, 1, 26,
                    0, 22, 57.0);
    GBL_TEST_COMPARE(GblDateTime_toUnix(&dt), 1674692577);

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(dateTimeToLocal) {
    GblDateTime dt1, dt2;
    struct tm bTime;

    GblDateTime_nowLocal(&dt1);
    GblDateTime_toLocal(&dt1, &bTime);
    GblDateTime_fromLocal(&dt2, &bTime);

    GBL_TEST_VERIFY(GblDateTime_equals(&dt1, &dt2));


    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(dateTimeToUtc) {
    GblDateTime dt1, dt2;
    struct tm bTime;

    GblDateTime_nowUtc(&dt1);
    GblDateTime_toUtc(&dt1, &bTime);
    GblDateTime_fromUtc(&dt2, &bTime);

    GBL_TEST_VERIFY(GblDateTime_equals(&dt1, &dt2));

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(dateTimeFormat) {
    GblDateTime dt;
    GblStringBuffer buffer;
    GblStringBuffer_construct(&buffer);

    GblDateTime_set(&dt,
                    1989, GBL_MONTH_JULY, 23,
                    22, 33, 12, GblTime_localUtcOffset());

    GBL_TEST_COMPARE(GblDateTime_format(&dt,
                                        &buffer,
                                        "%m/%d/%y - %I:%M%p"),
                     "07/23/89 - 10:33PM");

    GblStringBuffer_destruct(&buffer);
    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(dateTimeDiff) {
    GblDateTime dt1, dt2;

    GblDateTime_nowLocal(&dt1);
    memcpy(&dt2, &dt1, sizeof(GblDateTime));
    dt2.time.seconds -= GblTime_localUtcOffset();
    GblDateTime_normalize(&dt2);

    GBL_TEST_COMPARE(GblDateTime_diff(&dt1, &dt2).tv_sec, GblTime_localUtcOffset());

    GblDateTime_set(&dt1,
                    1994, 12, 17,
                    1, 0, 5.0);

    GblDateTime_set(&dt2,
                    1994, 12, 17,
                    1, 30, 5.0);

    const GblSecond diff = GblDateTime_diff(&dt2, &dt1).tv_sec;
    GBL_TEST_VERIFY(gblFloatEquals(diff, 30 * 60.0, 0.000001));

    GblDateTime dt3;
    memcpy(&dt3, &dt1, sizeof(GblDateTime));
    dt3.time.seconds += diff;
    GblDateTime_normalize(&dt3);
    GBL_TEST_COMPARE(GblDateTime_diff(&dt3, &dt2).tv_sec, 0.0);

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(dateTimeAddDays) {
    GblDateTime dt, dt2;

    GblDateTime_set(&dt,
                    1994, 12, 17,
                    1, 30, 5.0);

    GblDateTime_addDays(&dt, 25600);

    GBL_TEST_CALL(verifyDateTime_(pSelf, &dt,
                                  2065, 1, 18,
                                  1, 30, 5.0, 0, 0.0));

    GblDateTime_set(&dt2,
                    2065, 1, 18,
                    1, 30, 5.0);

    GBL_TEST_VERIFY(GblDateTime_equals(&dt, &dt2));


    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(dateTimeAddHours) {
    GblDateTime dt, dt2;

    GblDateTime_set(&dt,
                    1994, 12, 17,
                    1, 30, 5.0, 123, GblTime_localUtcOffset());

    GblDateTime_addHours(&dt, 24);

    GBL_TEST_CALL(verifyDateTime_(pSelf, &dt,
                                  1994, 12, 18,
                                  1, 30, 5.0, 123, GblTime_localUtcOffset()));

    GblDateTime_set(&dt2,
                    1994, 12, 18,
                    1, 30, 5.0, 123, GblTime_localUtcOffset());

    GBL_TEST_VERIFY(GblDateTime_equals(&dt, &dt2));

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(dateTimeAddMinutes) {
    GblDateTime dt, dt2;

    GblDateTime_set(&dt,
                    1994, 12, 17,
                    1, 30, 5.0, 13, GblTime_localUtcOffset());

    GblDateTime_addMinutes(&dt, -24*60);

    GBL_TEST_CALL(verifyDateTime_(pSelf, &dt,
                                  1994, 12, 16,
                                  1, 30, 5.0, 13, GblTime_localUtcOffset()));

    GblDateTime_set(&dt2,
                    1994, 12, 16,
                    1, 30, 5.0, 13, GblTime_localUtcOffset());

    GBL_TEST_VERIFY(GblDateTime_equals(&dt, &dt2));

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(dateTimeAddSeconds) {
    GblDateTime dt, dt2;

    GblDateTime_set(&dt,
                    1994, 12, 17,
                    1, 30, 5.0, 987, GblTime_localUtcOffset());

    GblDateTime_addSeconds(&dt, -24*60*60.0);

    GBL_TEST_CALL(verifyDateTime_(pSelf, &dt,
                                  1994, 12, 16,
                                  1, 30, 5.0, 987, GblTime_localUtcOffset()));

    GblDateTime_set(&dt2,
                    1994, 12, 16,
                    1, 30, 5.0, 987, GblTime_localUtcOffset());

    GBL_TEST_VERIFY(GblDateTime_equals(&dt, &dt2));

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(dateTimeAddWeeks) {
    GblDateTime dt, dt2;

    GblDateTime_set(&dt,
                    1994, 12, 17,
                    1, 30, 5.0, 999, GblTime_localUtcOffset());

    GblDateTime_addWeeks(&dt, 1);

    GBL_TEST_CALL(verifyDateTime_(pSelf, &dt,
                                  1994, 12, 24,
                                  1, 30, 5.0, 999, GblTime_localUtcOffset()));

    GblDateTime_set(&dt2,
                    1994, 12, 24,
                    1, 30, 5.0, 999, GblTime_localUtcOffset());

    GBL_TEST_VERIFY(GblDateTime_equals(&dt, &dt2));

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(dateTimeAddMonths) {
    GblDateTime dt, dt2;

    GblDateTime_set(&dt,
                    1994, 12, 17,
                    1, 30, 5.0, 777, GblTime_localUtcOffset());

    GblDateTime_addMonths(&dt, -13);

    GBL_TEST_CALL(verifyDateTime_(pSelf, &dt,
                                  1993, 11, 17,
                                  1, 30, 5.0, 777, GblTime_localUtcOffset()));

    GblDateTime_set(&dt2,
                    1993, 11, 17,
                    1, 30, 5.0, 777, GblTime_localUtcOffset());

    GBL_TEST_VERIFY(GblDateTime_equals(&dt, &dt2));

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(dateTimeAddYears) {
    GblDateTime dt, dt2;

    GblDateTime_set(&dt,
                    1994, 12, 17,
                    1, 30, 5.0, 8888, GblTime_localUtcOffset());

    GblDateTime_addYears(&dt, 50);

    GBL_TEST_CALL(verifyDateTime_(pSelf, &dt,
                                  2044, 12, 17,
                                  1, 30, 5.0, 8888, GblTime_localUtcOffset()));

    GblDateTime_set(&dt2,
                    2044, 12, 17,
                    1, 30, 5.0, 8888, GblTime_localUtcOffset());

    GBL_TEST_VERIFY(GblDateTime_equals(&dt, &dt2));

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(dateTimeAddNanoSecs) {
    GblDateTime dt, dt2;

    GblDateTime_set(&dt,
                    1994, 12, 17,
                    1, 30, 5.0, 8888, GblTime_localUtcOffset());

    GblDateTime_addNanoSecs(&dt, GBL_TIME_NSECS_PER_SEC);

    GBL_TEST_CALL(verifyDateTime_(pSelf, &dt,
                                  1994, 12, 17,
                                  1, 30, 6.0, 8888, GblTime_localUtcOffset()));

    GblDateTime_set(&dt2,
                    1994, 12, 17,
                    1, 30, 6.0, 8888, GblTime_localUtcOffset());

    GBL_TEST_VERIFY(GblDateTime_equals(&dt, &dt2));

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(dateTimeAddMicroSecs) {
    GblDateTime dt, dt2;

    GblDateTime_set(&dt,
                    1994, 12, 17,
                    1, 30, 5.0, 1, GblTime_localUtcOffset());

    GblDateTime_addMicroSecs(&dt, -GBL_TIME_NSECS_PER_SEC/1000.0 - 2);

    GBL_TEST_CALL(verifyDateTime_(pSelf, &dt,
                                  1994, 12, 17,
                                  1, 30, 4, -1999, GblTime_localUtcOffset()));

    GblDateTime_set(&dt2,
                    1994, 12, 17,
                    1, 30, 4, -1999, GblTime_localUtcOffset());

    GBL_TEST_VERIFY(GblDateTime_equals(&dt, &dt2));

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(dateTimeAddMilliSecs) {
    GblDateTime dt, dt2;

    GblDateTime_set(&dt,
                    1994, 12, 17,
                    1, 30, 5, -500, GblTime_localUtcOffset());

    GblDateTime_addMilliSecs(&dt, 2000);

    GBL_TEST_CALL(verifyDateTime_(pSelf, &dt,
                                  1994, 12, 17,
                                  1, 30, 6, GBL_TIME_NSECS_PER_SEC - 500, GblTime_localUtcOffset()));

    GblDateTime_set(&dt2,
                    1994, 12, 17,
                    1, 30, 6, GBL_TIME_NSECS_PER_SEC - 500, GblTime_localUtcOffset());

    GBL_TEST_VERIFY(GblDateTime_equals(&dt, &dt2));

    GBL_TEST_CASE_END;
}

GBL_TEST_CASE(dateTimeToIso8601) {
    GblDateTime dt;
    GblStringBuffer strBuff;

    GblStringBuffer_construct(&strBuff);

    GblDateTime_set(&dt,
                    1994, 12, 17,
                    1, 30, 5, 0, 3780);

    GBL_TEST_COMPARE(GblDateTime_toIso8601(&dt, &strBuff),
                     "1994-12-17T01:30:05+0103");

    GblDateTime_set(&dt,
                    2041, 4, 20,
                    14, 1, 55, 0, -18000);

    GBL_TEST_COMPARE(GblDateTime_toIso8601(&dt, &strBuff),
                     "2041-04-20T14:01:55-0500");

    GblStringBuffer_destruct(&strBuff);

    GBL_TEST_CASE_END;
}


GBL_TEST_REGISTER(dateIsLeapYear,
                  dateMonthDays,
                  dateMonthString,
                  dateWeekDayString,
                  dateToJulian,
                  dateWeekDay,
                  dateYearDayAndWeek,
                  dateFromJulian,
                  dateFromOrdinal,
                  timeStr,
                  timeInitDefault,
                  timeIsPm,
                  dateTimeInitDefault,
                  dateTimeIsUtc,
                  dateTimeNormalizeDefault,
                  dateTimeEquals,
                  dateTimeFromUnix,
                  dateTimeFromLocal,
                  dateTimeFromUtc,
                  dateTimeNowLocal,
                  dateTimeNowUtc,
                  dateTimeParse,
                  dateTimeToUnix,
                  dateTimeToLocal,
                  dateTimeToUtc,
                  dateTimeFormat,
                  dateTimeDiff,
                  dateTimeAddDays,
                  dateTimeAddHours,
                  dateTimeAddMinutes,
                  dateTimeAddSeconds,
                  dateTimeAddWeeks,
                  dateTimeAddMonths,
                  dateTimeAddYears,
                  dateTimeAddNanoSecs,
                  dateTimeAddMicroSecs,
                  dateTimeAddMilliSecs,
                  dateTimeToIso8601)

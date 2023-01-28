#include <EFEU/UnitTest.h>
#include <EFEU/calendar.h>
#include "utest/mstr.tst.h"

TEST_DECL(str2jahr)
{
	char *ptr = NULL;
	EXPECT(str2jahr("49:Rest", &ptr) == 2049);
	EXPECT_STR(ptr, ":Rest");
	EXPECT(str2jahr("50", NULL) == 1950);
	EXPECT(str2jahr("99", NULL) == 1999);
	EXPECT(str2jahr("100", NULL) == 100);

}

TEST_DECL(str2sec)
{
	char *ptr = NULL;
	EXPECT(str2sec("10sX", &ptr) == 10);
	EXPECT_STR(ptr, "X");
	EXPECT(str2sec("3d", NULL) == 3 * 24 * 60 * 60);
	EXPECT(str2sec("2h", NULL) == 2 * 60 * 60);
	EXPECT(str2sec("15m", NULL) == 15 * 60);
	EXPECT(str2sec("3d2h15m20s", NULL) == \
		((3 * 24 + 2) * 60 + 15) * 60 + 20);
	EXPECT(str2sec("2m 3s", &ptr) == 2 * 60);
	EXPECT_STR(ptr, " 3s");
	EXPECT(str2sec("2 m3s", &ptr) == 2);
	EXPECT_STR(ptr, " m3s");
}

static char *str_cal(const char *str, int flag)
{
	return Calendar2str(NULL, str2Calendar(str, NULL, flag));
}

TEST_DECL(LeapYear)
{
	EXPECT(LeapYear(1900) == 0);
	EXPECT(LeapYear(2000) == 1);
	EXPECT(LeapYear(2100) == 0);
	EXPECT(LeapYear(2004) == 1);
	EXPECT(LeapYear(96) == 1);	/* 1996 */
	EXPECT(LeapYear(0) == 1); 	/* 2000 */
}

TEST_DECL(Calendar)
{
	EXPECT_MSTR(str_cal("2000", 0), "2000-01-01");
	EXPECT_MSTR(str_cal(" 2000", 1), "2000-12-31");
	EXPECT_MSTR(str_cal("10-09", 0), "2010-09-01");
	EXPECT_MSTR(str_cal("\t\n10-09", 1), "2010-09-30");
	EXPECT_MSTR(str_cal("8.20", 0), "2020-08-01");
	EXPECT_MSTR(str_cal("8.20", 1), "2020-08-31");
	EXPECT_MSTR(str_cal("90-03-12", 0), "1990-03-12");
	EXPECT_MSTR(str_cal("90-03-12", 1), "1990-03-12");
	EXPECT_MSTR(str_cal("12.3.33", 0), "2033-03-12");

	EXPECT_MSTR(Calendar2str("%F %T", 0), "0000-00-00 12:00:00");
	EXPECT_MSTR(Calendar2str(NULL,
		CalendarIndex(12,4,2019)), "2019-04-12");
	EXPECT_MSTR(Calendar2str(NULL,
		CalendarIndex1900(12,4,2019)), "2019-04-12");

	EXPECT_MSTR(Calendar2str(NULL, CalBaseStd(1)), "1900-01-01");
	EXPECT(CalBase1900(CalBaseStd(4711)) == 4711);
	EXPECT(CalBase1900(CalBaseStd(0xFFFF)) == 0xFFFF);
	EXPECT(CalBase1900(CalBaseStd(0x10000)) != 0x10000);
	EXPECT(CalendarIndex1900(1,1,1900) == 1);
	EXPECT(CalendarIndex(1,1,1900) == CALENDAR_OFFSET_1900 + 1);

	EXPECT_INT(Calendar_jdiff(
		CalendarIndex(15, 8, 2020),
		CalendarIndex(10, 5, 2010)), 10);
	EXPECT_INT(Calendar_jdiff(
		CalendarIndex(15, 8, 2020),
		CalendarIndex(15, 8, 2010)), 10);
	EXPECT_INT(Calendar_jdiff(
		CalendarIndex(15, 8, 2020),
		CalendarIndex(16, 8, 2010)), 9);
	EXPECT_INT(Calendar_jdiff(
		CalendarIndex(15, 8, 2020),
		CalendarIndex(15, 9, 2010)), 9);
}

TEST_DECL(Time)
{
	CalTimeIndex t1;
	t1 = str2Time(NULL, NULL, 0);
	EXPECT(t1.date == 0);
	EXPECT(t1.time == 0);
	t1 = str2Time("2019-08-04 12", NULL, 0);
	EXPECT_STR(Time2str(NULL, t1), "2019-08-04 12:00:00");

	t1 = str2Time("2019-08-04 12:30:15", NULL, 0);
	EXPECT_STR(Time2str(NULL, t1), "2019-08-04 12:30:15");
	CalInfo *info = TimeCalendar(t1, NULL);
	EXPECT(info->year == 2019);
	EXPECT(info->month == 8);
	EXPECT(info->day == 4);
	EXPECT(info->wday == 0);
	EXPECT(info->yday == 216);
	EXPECT(info->hour == 12);
	EXPECT(info->min == 30);
	EXPECT(info->sec == 15);
	EXPECT(Calendar_week(t1.date) == 31);
	EXPECT_STR(Calendar2str(NULL, t1.date), "2019-08-04");
	EXPECT(t1.time == (12 * 60 + 30) * 60 + 15);
	
	PRINT_STR(Time2str("%B %b", t1));
	EXPECT_STR(Time2str("%A %a", t1), "Sunday Su");
	EXPECT_STR(Time2str("%B %b", t1), "August Aug");
	EXPECT_STR(Time2str("|%m|%-m|%_m|", t1), "|08|8| 8|");
	EXPECT_STR(Time2str("|%y|%Y|", t1), "|19|2019|");
	EXPECT_STR(Time2str("|%d|%e|%j|", t1), "|04| 4|216|");
	EXPECT_STR(Time2str("|%I|%p|%P|", t1), "|12|PM|pm|");
	PRINT_STR(Time2str("|%I|%p|%P|", t1));

	CalTimeIndex t2 = Time_offset(t1, 124);
	EXPECT_STR(Time2str(NULL, t2), "2019-08-04 12:32:19");
	EXPECT(Time_dist(t1, t2) == 124);

	struct tm tm;
	EXPECT(Time2tm(t2, &tm) == &tm);
	EXPECT_STR(tm2str(NULL, &tm), "2019-08-04 12:32:19");
	EXPECT_STR(tm2str(NULL, CalInfo2tm(NULL, NULL)), "1970-01-01 00:00:00");
}

static char *cal_week(int tag, int mon, int jahr)
{
	return Calendar2str("%F %a(%w): %U %V %W",
		CalendarIndex(tag, mon, jahr));
}

TEST_DECL(week)
{
	EXPECT_MSTR(cal_week(31, 12, 2015), "2015-12-31 Th(4): 52 53 52");
	EXPECT_MSTR(cal_week( 1,  1, 2016), "2016-01-01 Fr(5): 00 53 00");

	EXPECT_MSTR(cal_week(31, 12, 2016), "2016-12-31 Sa(6): 52 52 52");
	EXPECT_MSTR(cal_week( 1,  1, 2017), "2017-01-01 Su(0): 00 52 00");

	EXPECT_MSTR(cal_week(31, 12, 2017), "2017-12-31 Su(0): 52 52 52");
	EXPECT_MSTR(cal_week( 1,  1, 2018), "2018-01-01 Mo(1): 00 01 00");
}

TEST_SUITE
{
	PRINT_STR(Time2str(NULL, CurrentTime()));
	EXPECT(CurrentTime().date == TodayIndex());

	TEST_RUN(str2jahr);
	TEST_RUN(str2sec);
	TEST_RUN(LeapYear);
	TEST_RUN(Calendar);
	TEST_RUN(Time);
	TEST_RUN(week);
	CLEANUP_MSTR;
}

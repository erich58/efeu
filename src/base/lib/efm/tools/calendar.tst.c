#include <EFEU/UnitTest.h>
#include <EFEU/calendar.h>

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

TEST_DECL(Calendar)
{
	EXPECT_STR(str_cal("2000", 0), "2000-01-01");
	EXPECT_STR(str_cal("2000", 1), "2000-12-31");
	EXPECT_STR(str_cal("10-09", 0), "2010-09-01");
	EXPECT_STR(str_cal("10-09", 1), "2010-09-30");
	EXPECT_STR(str_cal("8.20", 0), "2020-08-01");
	EXPECT_STR(str_cal("8.20", 1), "2020-08-31");
	EXPECT_STR(str_cal("90-03-12", 0), "1990-03-12");
	EXPECT_STR(str_cal("90-03-12", 1), "1990-03-12");
	EXPECT_STR(str_cal("12.3.33", 0), "2033-03-12");

	EXPECT_STR(Calendar2str(NULL,
		CalendarIndex(12,4,2019)), "2019-04-12");
	EXPECT_STR(Calendar2str(NULL,
		CalendarIndex1900(12,4,2019)), "2019-04-12");

	EXPECT_STR(Calendar2str(NULL, CalBaseStd(1)), "1900-01-01");
	EXPECT(CalBase1900(CalBaseStd(4711)) == 4711);
	EXPECT(CalBase1900(CalBaseStd(0xFFFF)) == 0xFFFF);
	EXPECT(CalBase1900(CalBaseStd(0x10000)) != 0x10000);
	EXPECT(CalendarIndex1900(1,1,1900) == 1);
	EXPECT(CalendarIndex(1,1,1900) == CALENDAR_OFFSET_1900 + 1);
}

TEST_DECL(Time)
{
	CalTimeIndex t1 = str2Time("2019-08-04 12:30:15", NULL, 0);
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
}

TEST_SUITE
{
	PRINT_STR(Time2str(NULL, CurrentTime()));
	TEST_RUN(str2jahr);
	TEST_RUN(str2sec);
	TEST_RUN(Calendar);
	TEST_RUN(Time);
}

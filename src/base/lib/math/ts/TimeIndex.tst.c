#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <EFEU/calendar.h>
#include <Math/TimeSeries.h>
#include <EFEU/utest/mstr.tst.h>

TEST_DECL(setup)
{
	SetupStd();
	SetProgName("ID");
	SetApplPath(UnitTest_tmp(NULL));
	TEST_FILE("TimeSeries.msg",
		"1	$!: index not compatible: $1 - $2.\n"
		"2	$!: unknown index type '$1'.\n"
	);
}

TEST_DECL(index)
{
	EXPECT_INT(tindex_type("jahr"), TS_YEAR);
	EXPECT_INT(tindex_type("year"), TS_YEAR);
	EXPECT_INT(tindex_type("quart"), TS_QUART);
	EXPECT_INT(tindex_type("month"), TS_MONTH);
	EXPECT_INT(tindex_type("day"), TS_DAY);
	EXPECT_INT(tindex_type("hour"), TS_HOUR);
	EXPECT_INT(tindex_type("min"), TS_MIN);
	EXPECT_INT(tindex_type("sec"), TS_SEC);

	UnitTest_stderr("err");
	EXPECT_INT(tindex_type("x"), 0);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("err", "MSG ID: unknown index type 'x'.\n");
}

TEST_DECL(name)
{
	EXPECT_STR(tindex_name(TS_YEAR), "year");
	EXPECT_STR(tindex_name(TS_QUART), "quart");
	EXPECT_STR(tindex_name(TS_MONTH), "month");
	EXPECT_STR(tindex_name(TS_DAY), "day");
	EXPECT_STR(tindex_name(TS_HOUR), "hour");
	EXPECT_STR(tindex_name(TS_MIN), "min");
	EXPECT_STR(tindex_name(TS_SEC), "sec");
	EXPECT_STR(tindex_name(TS_INDEX), "index");
}

static char *t2str(TimeIndex idx)
{
	static char buf[32];
	snprintf(buf,  sizeof buf, "%c %d", idx.type, idx.value);
	return buf;
}

TEST_DECL(str2idx)
{
	char *p = "xxx";
	EXPECT_STR(t2str(str2TimeIndex(NULL, &p)), "# 0");
	EXPECT_STR(p, NULL);
	EXPECT_STR(t2str(str2TimeIndex(" # 5 extra", &p)), "# 5");
	EXPECT_STR(p, " extra");

	EXPECT_STR(t2str(str2TimeIndex(" 0", &p)), "y 2000");
	EXPECT_STR(t2str(str2TimeIndex(" 49", &p)), "y 2049");
	EXPECT_STR(t2str(str2TimeIndex(" 50", &p)), "y 1950");
	EXPECT_STR(t2str(str2TimeIndex(" 99", &p)), "y 1999");
	EXPECT_STR(t2str(str2TimeIndex(" 100", &p)), "y 100");
	EXPECT_STR(t2str(str2TimeIndex(" 2000 @1", &p)), "y 2000");
	EXPECT_STR(p, " @1");

	EXPECT_STR(t2str(str2TimeIndex(" 0@1", &p)), "q 8000");
	EXPECT_STR(t2str(str2TimeIndex(" 50@1", &p)), "q 7800");
	EXPECT_STR(t2str(str2TimeIndex(" 100@1", &p)), "q 400");
	EXPECT_STR(t2str(str2TimeIndex(" 2000@1", &p)), "q 8000");
	EXPECT_STR(t2str(str2TimeIndex(" 2000@2", &p)), "q 8001");

	EXPECT_STR(t2str(str2TimeIndex(" 0-1", &p)), "m 24000");
	EXPECT_STR(t2str(str2TimeIndex(" 50-1", &p)), "m 23400");
	EXPECT_STR(t2str(str2TimeIndex(" 100-1", &p)), "m 1200");
	EXPECT_STR(t2str(str2TimeIndex(" 2000-1", &p)), "m 24000");

	EXPECT_STR(t2str(str2TimeIndex("1000-1-1", &p)), "d 364885");
	EXPECT_STR(t2str(str2TimeIndex("2000-1-1", &p)), "d 730133");
	EXPECT_MSTR(TimeIndex2str(str2TimeIndex("0-1-2", &p), 0),
		"2000-01-02");
	EXPECT_MSTR(TimeIndex2str(str2TimeIndex("50-6-10", &p), 0),
		"1950-06-10");

	EXPECT_STR(t2str(str2TimeIndex("0/0", &p)), "H 0");
	EXPECT_STR(t2str(str2TimeIndex("0/1", &p)), "H 1");
	EXPECT_STR(t2str(str2TimeIndex("1/0", &p)), "H 24");
	EXPECT_STR(t2str(str2TimeIndex("1/10", &p)), "H 34");

	EXPECT_STR(t2str(str2TimeIndex("0/0:0", &p)), "M 0");
	EXPECT_STR(t2str(str2TimeIndex("1/0:30", &p)), "M 1470");
	EXPECT_STR(t2str(str2TimeIndex("0/0:01:30", &p)), "S 90");
	EXPECT_STR(t2str(str2TimeIndex("0:0", &p)), "M 0");
	EXPECT_STR(t2str(str2TimeIndex("0:30", &p)), "M 30");
	EXPECT_STR(t2str(str2TimeIndex("0:01:30", &p)), "S 90");
}

TEST_DECL(idx2str)
{
	EXPECT_MSTR(TimeIndex2str(tindex_create(TS_INDEX, 0), 1), "#1");
	EXPECT_MSTR(TimeIndex2str(tindex_create(TS_YEAR, 2020), 1), "2021");
	EXPECT_MSTR(TimeIndex2str(tindex_create(TS_QUART, 4*2020), 0), "2020@1");
	EXPECT_MSTR(TimeIndex2str(tindex_create(TS_QUART, 4*2020), 5), "2021@2");
	EXPECT_MSTR(TimeIndex2str(tindex_create(TS_MONTH, 12*2020), 0), "2020-01");
	EXPECT_MSTR(TimeIndex2str(tindex_create(TS_MONTH, 12*2020), 8), "2020-09");
	int cidx = CalendarIndex(1, 1, 2020);
	EXPECT_MSTR(TimeIndex2str(tindex_create(TS_DAY, cidx), 0), "2020-01-01");
	EXPECT_MSTR(TimeIndex2str(tindex_create(TS_DAY, cidx), 31), "2020-02-01");
	EXPECT_MSTR(TimeIndex2str(tindex_create(TS_HOUR, 0), 0), "0/00");
	EXPECT_MSTR(TimeIndex2str(tindex_create(TS_HOUR, 0), 25), "1/01");
	EXPECT_MSTR(TimeIndex2str(tindex_create(TS_MIN, 0), 0), "00:00");
	EXPECT_MSTR(TimeIndex2str(tindex_create(TS_MIN, 0), 61), "01:01");
	EXPECT_MSTR(TimeIndex2str(tindex_create(TS_SEC, 0), 0), "00:00:00");
	EXPECT_MSTR(TimeIndex2str(tindex_create(TS_SEC, 0), 61), "00:01:01");
	EXPECT_MSTR(TimeIndex2str(tindex_create(TS_SEC, 0), 3601), "01:00:01");
}

TEST_DECL(idx2dbl)
{
	EXPECT_DBL(TimeIndex2dbl(tindex_create(TS_INDEX, 0), 1, 0), 1.);
	EXPECT_DBL(TimeIndex2dbl(tindex_create(TS_INDEX, 0), 1, 1), 1.5);
	EXPECT_DBL(TimeIndex2dbl(tindex_create(TS_INDEX, 0), 1, 2), 2.);
	EXPECT_DBL(TimeIndex2dbl(tindex_create(TS_YEAR, 2020), 1, 0), 2021.);
	EXPECT_DBL(TimeIndex2dbl(tindex_create(TS_YEAR, 2020), 1, 1), 2021.5);
	EXPECT_DBL(TimeIndex2dbl(tindex_create(TS_QUART, 4*2020), 1, 0), 2020.25);
	EXPECT_DBL(TimeIndex2dbl(tindex_create(TS_QUART, 4*2020), 1, 1), 2020.375);
	EXPECT_DBL(TimeIndex2dbl(tindex_create(TS_MONTH, 12*2020), 3, 0),
		2020. + 3./12.);
	EXPECT_DBL(TimeIndex2dbl(tindex_create(TS_MONTH, 12*2020), 3, 1),
		2020. + 3.5/12.);
	int cidx = CalendarIndex(1, 2, 2020);
	EXPECT_DBL(TimeIndex2dbl(tindex_create(TS_DAY, cidx), 0, 0),
		2020 + 31. / 366.);
	EXPECT_DBL(TimeIndex2dbl(tindex_create(TS_DAY, cidx), 0, 1),
		2020 + 31.5 / 366.);
	EXPECT_DBL(TimeIndex2dbl(tindex_create(TS_HOUR, 10), 0, 1),
		10.5);
	EXPECT_DBL(TimeIndex2dbl(tindex_create(TS_MIN, 100), 0, 1),
		100.5);
	EXPECT_DBL(TimeIndex2dbl(tindex_create(TS_SEC, 1000), 0, 1),
		1000.5);
}


TEST_DECL(length)
{
	EXPECT_INT(tindex_length(tindex_create(TS_INDEX, 42)), 1);
	EXPECT_INT(tindex_length(tindex_create(TS_YEAR, 2020)), 366);
	EXPECT_INT(tindex_length(tindex_create(TS_QUART, 4*2020)), 91);
	EXPECT_INT(tindex_length(tindex_create(TS_MONTH, 12*2020)), 31);
	int cidx = CalendarIndex(1, 2, 2020);
	EXPECT_INT(tindex_length(tindex_create(TS_DAY, cidx)), 1);
	EXPECT_INT(tindex_length(tindex_create(TS_HOUR, 1)), 3600);
	EXPECT_INT(tindex_length(tindex_create(TS_MIN, 1)), 60);
	EXPECT_INT(tindex_length(tindex_create(TS_SEC, 1)), 1);
}

TEST_DECL(year)
{
	EXPECT_INT(tindex_year(tindex_create(TS_INDEX, 42)), 0);
	EXPECT_INT(tindex_year(tindex_create(TS_YEAR, 2020)), 2020);
	EXPECT_INT(tindex_year(tindex_create(TS_QUART, 4*2020)), 2020);
	EXPECT_INT(tindex_year(tindex_create(TS_MONTH, 12*2020)), 2020);
	int cidx = CalendarIndex(1, 2, 2020);
	EXPECT_INT(tindex_year(tindex_create(TS_DAY, cidx)), 2020);
	EXPECT_INT(tindex_year(tindex_create(TS_HOUR, 42)), 0);
	EXPECT_INT(tindex_year(tindex_create(TS_MIN, 42)), 0);
	EXPECT_INT(tindex_year(tindex_create(TS_SEC, 42)), 0);
}

TEST_DECL(quart)
{
	EXPECT_INT(tindex_quart(tindex_create(TS_INDEX, 42)), 0);
	EXPECT_INT(tindex_quart(tindex_create(TS_YEAR, 2020)), 0);
	EXPECT_INT(tindex_quart(tindex_create(TS_QUART, 4*2020 + 1)), 2);
	EXPECT_INT(tindex_quart(tindex_create(TS_MONTH, 12*2020 + 5)), 2);
	int cidx = CalendarIndex(1, 5, 2020);
	EXPECT_INT(tindex_quart(tindex_create(TS_DAY, cidx)), 2);
	EXPECT_INT(tindex_quart(tindex_create(TS_HOUR, 42)), 0);
	EXPECT_INT(tindex_quart(tindex_create(TS_MIN, 42)), 0);
	EXPECT_INT(tindex_quart(tindex_create(TS_SEC, 42)), 0);
}

TEST_DECL(month)
{
	EXPECT_INT(tindex_month(tindex_create(TS_INDEX, 42)), 0);
	EXPECT_INT(tindex_month(tindex_create(TS_YEAR, 2020)), 0);
	EXPECT_INT(tindex_month(tindex_create(TS_QUART, 4*2020 + 1)), 5);
	EXPECT_INT(tindex_month(tindex_create(TS_MONTH, 12*2020 + 4)), 5);
	int cidx = CalendarIndex(10, 5, 2020);
	EXPECT_INT(tindex_month(tindex_create(TS_DAY, cidx)), 5);
	EXPECT_INT(tindex_month(tindex_create(TS_HOUR, 42)), 0);
	EXPECT_INT(tindex_month(tindex_create(TS_MIN, 42)), 0);
	EXPECT_INT(tindex_month(tindex_create(TS_SEC, 42)), 0);
}

TEST_DECL(floor)
{
	EXPECT_INT(tindex_floor(tindex_create(TS_INDEX, 42)), 42);
	EXPECT_INT(tindex_floor(tindex_create(TS_YEAR, 2020)),
		CalendarIndex(1, 1, 2020));
	EXPECT_INT(tindex_floor(tindex_create(TS_QUART, 4*2020 + 1)),
		CalendarIndex(1, 4, 2020));
	EXPECT_INT(tindex_floor(tindex_create(TS_MONTH, 12*2020 + 4)),
		CalendarIndex(1, 5, 2020));
	int cidx = CalendarIndex(10, 5, 2020);
	EXPECT_INT(tindex_floor(tindex_create(TS_DAY, cidx)), cidx);
	EXPECT_INT(tindex_floor(tindex_create(TS_HOUR, 42)), 42);
	EXPECT_INT(tindex_floor(tindex_create(TS_MIN, 42)), 42);
	EXPECT_INT(tindex_floor(tindex_create(TS_SEC, 42)), 42);
}

TEST_DECL(ceil)
{
	EXPECT_INT(tindex_ceil(tindex_create(TS_INDEX, 42)), 42);
	EXPECT_INT(tindex_ceil(tindex_create(TS_YEAR, 2020)),
		CalendarIndex(31, 12, 2020));
	EXPECT_INT(tindex_ceil(tindex_create(TS_QUART, 4*2020 + 1)),
		CalendarIndex(30, 6, 2020));
	EXPECT_INT(tindex_ceil(tindex_create(TS_MONTH, 12*2020 + 4)),
		CalendarIndex(31, 5, 2020));
	int cidx = CalendarIndex(10, 5, 2020);
	EXPECT_INT(tindex_ceil(tindex_create(TS_DAY, cidx)), cidx);
	EXPECT_INT(tindex_ceil(tindex_create(TS_HOUR, 42)), 42);
	EXPECT_INT(tindex_ceil(tindex_create(TS_MIN, 42)), 42);
	EXPECT_INT(tindex_ceil(tindex_create(TS_SEC, 42)), 42);
}

TEST_DECL(diff)
{
	UnitTest_stderr("err");
	EXPECT_INT(tindex_diff(tindex_create(TS_YEAR, 2020),
		tindex_create(TS_QUART, 4*2021)), 0);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("err", "MSG ID: index not compatible: 'y' - 'q'.\n");
	EXPECT_INT(tindex_diff(tindex_create(TS_YEAR, 2020),
		tindex_create(TS_YEAR, 2021)), 1);
}


TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(index);
	TEST_RUN(str2idx);
	TEST_RUN(idx2str);
	TEST_RUN(idx2dbl);
	TEST_RUN(length);
	TEST_RUN(year);
	TEST_RUN(quart);
	TEST_RUN(month);
	TEST_RUN(floor);
	TEST_RUN(ceil);
	TEST_RUN(diff);
	TEST_RUN(name);
	CLEANUP_MSTR;
}

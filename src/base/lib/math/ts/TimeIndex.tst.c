#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <EFEU/calendar.h>
#include <Math/TimeSeries.h>
#include <utest/mstr.tst.h>

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
	EXPECT_INT(tindex_type("week"), TS_WEEK);
	EXPECT_INT(tindex_type("day"), TS_DAY);

	UnitTest_stderr("err");
	EXPECT_INT(tindex_type("x"), 0);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("err", "MSG ID: unknown index type 'x'.\n");
}

TEST_DECL(idx2str)
{
	EXPECT_MSTR(TimeIndex2str(tindex_create(TS_INDEX, 0), 1), "#1");
	EXPECT_MSTR(TimeIndex2str(tindex_create(TS_YEAR, 2020), 1), "2021");
	EXPECT_MSTR(TimeIndex2str(tindex_create(TS_QUART, 4*2020), 0), "2020:1");
	EXPECT_MSTR(TimeIndex2str(tindex_create(TS_QUART, 4*2020), 5), "2021:2");
	EXPECT_MSTR(TimeIndex2str(tindex_create(TS_MONTH, 12*2020), 0), "2020-01");
	EXPECT_MSTR(TimeIndex2str(tindex_create(TS_MONTH, 12*2020), 8), "2020-09");
	int cidx = CalendarIndex(1, 1, 2020);
	EXPECT_MSTR(TimeIndex2str(tindex_create(TS_WEEK, cidx / 7), 0), "1/2020");
	EXPECT_MSTR(TimeIndex2str(tindex_create(TS_DAY, cidx), 0), "2020-01-01");
	EXPECT_MSTR(TimeIndex2str(tindex_create(TS_DAY, cidx), 31), "2020-02-01");
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
	EXPECT_DBL(TimeIndex2dbl(tindex_create(TS_WEEK, cidx / 7), 0, 0), 
		2020. + 26. / 366.); // Montag 0h
	EXPECT_DBL(TimeIndex2dbl(tindex_create(TS_WEEK, cidx / 7), 0, 1), 
		2020. + 28.5 / 366.); // Mittwoch 12h
	EXPECT_DBL(TimeIndex2dbl(tindex_create(TS_WEEK, cidx / 7), 0, 2), 
		2020. + 33. / 366.); // Sonntag 24h
	EXPECT_DBL(TimeIndex2dbl(tindex_create(TS_DAY, cidx), 0, 0),
		2020 + 31. / 366.);
	EXPECT_DBL(TimeIndex2dbl(tindex_create(TS_DAY, cidx), 0, 1),
		2020 + 31.5 / 366.);
}

TEST_DECL(str2idx)
{
	char *p = "xxx";
	EXPECT_MSTR(TimeIndex2str(str2TimeIndex(NULL, &p), 0), "#0");
	EXPECT_STR(p, NULL);
	EXPECT_MSTR(TimeIndex2str(str2TimeIndex(" # 5 extra", &p), 10), "#15");
	EXPECT_STR(p, " extra");

	EXPECT_MSTR(TimeIndex2str(str2TimeIndex(" 2000", &p), 10), "2010");
	EXPECT_MSTR(TimeIndex2str(str2TimeIndex(" 2000 /3", &p), 10), "2010");
	EXPECT_STR(p, " /3");
	EXPECT_MSTR(TimeIndex2str(str2TimeIndex("3/2000", &p), 10), "13/2000");
	EXPECT_MSTR(TimeIndex2str(str2TimeIndex("2000:1", &p), 5), "2001:2");
	EXPECT_MSTR(TimeIndex2str(str2TimeIndex("1.2000", &p), 5), "2000-06");
	EXPECT_MSTR(TimeIndex2str(str2TimeIndex("1.2.2000", &p), 5), "2000-02-06");
	EXPECT_MSTR(TimeIndex2str(str2TimeIndex("2000-1", &p), 5), "2000-06");
	EXPECT_MSTR(TimeIndex2str(str2TimeIndex("2000-2-1", &p), 29), "2000-03-01");
}

TEST_DECL(length)
{
	EXPECT_INT(tindex_length(tindex_create(TS_INDEX, 42)), 1);
	EXPECT_INT(tindex_length(tindex_create(TS_YEAR, 2020)), 366);
	EXPECT_INT(tindex_length(tindex_create(TS_QUART, 4*2020)), 91);
	EXPECT_INT(tindex_length(tindex_create(TS_MONTH, 12*2020)), 31);
	int cidx = CalendarIndex(1, 2, 2020);
	EXPECT_INT(tindex_length(tindex_create(TS_WEEK, cidx / 7)),  7);
	EXPECT_INT(tindex_length(tindex_create(TS_DAY, cidx)), 1);
}

TEST_DECL(year)
{
	EXPECT_INT(tindex_year(tindex_create(TS_INDEX, 42)), 0);
	EXPECT_INT(tindex_year(tindex_create(TS_YEAR, 2020)), 2020);
	EXPECT_INT(tindex_year(tindex_create(TS_QUART, 4*2020)), 2020);
	EXPECT_INT(tindex_year(tindex_create(TS_MONTH, 12*2020)), 2020);
	int cidx = CalendarIndex(1, 2, 2020);
	EXPECT_INT(tindex_year(tindex_create(TS_WEEK, cidx / 7)), 2020);
	EXPECT_INT(tindex_year(tindex_create(TS_DAY, cidx)), 2020);
}

TEST_DECL(quart)
{
	EXPECT_INT(tindex_quart(tindex_create(TS_INDEX, 42)), 0);
	EXPECT_INT(tindex_quart(tindex_create(TS_YEAR, 2020)), 0);
	EXPECT_INT(tindex_quart(tindex_create(TS_QUART, 4*2020 + 1)), 2);
	EXPECT_INT(tindex_quart(tindex_create(TS_MONTH, 12*2020 + 5)), 2);
	int cidx = CalendarIndex(1, 5, 2020);
	EXPECT_INT(tindex_quart(tindex_create(TS_WEEK, cidx / 7)), 2);
	EXPECT_INT(tindex_quart(tindex_create(TS_DAY, cidx)), 2);
}

TEST_DECL(month)
{
	EXPECT_INT(tindex_month(tindex_create(TS_INDEX, 42)), 0);
	EXPECT_INT(tindex_month(tindex_create(TS_YEAR, 2020)), 0);
	EXPECT_INT(tindex_month(tindex_create(TS_QUART, 4*2020 + 1)), 5);
	EXPECT_INT(tindex_month(tindex_create(TS_MONTH, 12*2020 + 4)), 5);
	int cidx = CalendarIndex(10, 5, 2020);
	EXPECT_INT(tindex_month(tindex_create(TS_WEEK, cidx / 7)), 5);
	EXPECT_INT(tindex_month(tindex_create(TS_DAY, cidx)), 5);
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
	EXPECT_INT(tindex_floor(tindex_create(TS_WEEK, cidx / 7)),
		CalendarIndex(4, 5, 2020)); // Montag
	EXPECT_INT(tindex_floor(tindex_create(TS_DAY, cidx)), cidx);
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
	EXPECT_INT(tindex_ceil(tindex_create(TS_WEEK, cidx / 7)),
		CalendarIndex(10, 5, 2020)); // Sonntag
	EXPECT_INT(tindex_ceil(tindex_create(TS_DAY, cidx)), cidx);
}

TEST_DECL(diff)
{
	UnitTest_stderr("err");
	EXPECT_INT(tindex_diff(tindex_create(TS_YEAR, 2020),
		tindex_create(TS_QUART, 4*2021)), 1);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("err", "MSG ID: index not compatible: 'y' - 'q'.\n");
	EXPECT_INT(tindex_diff(tindex_create(TS_YEAR, 2020),
		tindex_create(TS_YEAR, 2021)), 1);
}


TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(index);
	TEST_RUN(idx2str);
	TEST_RUN(idx2dbl);
	TEST_RUN(str2idx);
	TEST_RUN(length);
	TEST_RUN(year);
	TEST_RUN(quart);
	TEST_RUN(month);
	TEST_RUN(floor);
	TEST_RUN(ceil);
	TEST_RUN(diff);
	CLEANUP_MSTR;
}

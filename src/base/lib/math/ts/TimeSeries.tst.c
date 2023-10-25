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

TEST_DECL(create)
{
	TimeSeries *ts = ts_create("test", tindex_create(TS_INDEX, 0), 10);
	EXPECT_MSTR(rd_ident(ts), "test[10] #0 #9");
	ts_expand(ts, 100);
	EXPECT_MSTR(rd_ident(ts), "test[100] #0 #99");
	rd_deref(ts);
}

TEST_DECL(sync)
{
	TimeSeries *ts = ts_create("test", tindex_create(TS_YEAR, 2000), 5);
	EXPECT_MSTR(rd_ident(ts), "test[5] 2000 2004");
	UnitTest_stderr("err");
	ts_sync(ts, tindex_create(TS_MONTH, 0), 2, 2);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("err", "MSG ID: index not compatible: 'y' - 'm'.\n");
	EXPECT_MSTR(rd_ident(ts), "test[5] 2000 2004");

	ts_sync(ts, tindex_create(TS_YEAR, 1990), 2, 0);
	EXPECT_MSTR(rd_ident(ts), "test[2] 1990 1991");

	ts_sync(ts, tindex_create(TS_YEAR, 1900), 0, 0);
	EXPECT_MSTR(rd_ident(ts), "test[92] 1900 1991");

	ts_sync(ts, tindex_create(TS_YEAR, 1920), 10, 0);
	EXPECT_MSTR(rd_ident(ts), "test[10] 1920 1929");

	ts_sync(ts, tindex_create(TS_YEAR, 2000), 0, 0);
	EXPECT_MSTR(rd_ident(ts), "test[0] 2000");

	ts_sync(ts, tindex_create(TS_YEAR, 2001), 5, 0);
	EXPECT_MSTR(rd_ident(ts), "test[5] 2001 2005");
	TEST_EXPR(ts->data[0] = 1.);
	TEST_EXPR(ts->data[1] = 2.);
	TEST_EXPR(ts->data[2] = 3.);
	TEST_EXPR(ts->data[3] = 4.);
	TEST_EXPR(ts->data[4] = 5.);

	ts_sync(ts, tindex_create(TS_YEAR, 2000), 8, 2);
	EXPECT_MSTR(rd_ident(ts), "test[8] 2000 2007");
	EXPECT_DBL(ts->data[0], 2.);
	EXPECT_DBL(ts->data[1], 1.);
	EXPECT_DBL(ts->data[2], 2.);
	EXPECT_DBL(ts->data[3], 3.);
	EXPECT_DBL(ts->data[4], 4.);
	EXPECT_DBL(ts->data[5], 5.);
	EXPECT_DBL(ts->data[6], 4.);
	EXPECT_DBL(ts->data[7], 5.);

	ts_sync(ts, tindex_create(TS_YEAR, 1999), 3, 0);
	EXPECT_MSTR(rd_ident(ts), "test[3] 1999 2001");
	EXPECT_DBL(ts->data[0], 0.);
	EXPECT_DBL(ts->data[1], 2.);
	EXPECT_DBL(ts->data[2], 1.);

	ts_sync(ts, tindex_create(TS_YEAR, 2000), 5, 4);
	EXPECT_MSTR(rd_ident(ts), "test[5] 2000 2004");
	EXPECT_DBL(ts->data[0], 2.);
	EXPECT_DBL(ts->data[1], 1.);
	EXPECT_DBL(ts->data[2], 0.);
	EXPECT_DBL(ts->data[3], 0.);
	EXPECT_DBL(ts->data[4], 2.);

	rd_deref(ts);
}

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(create);
	TEST_RUN(sync);
	CLEANUP_MSTR;
}

#include <EFEU/UnitTest.h>
#include <Math/TimeSeries.h>
#include <utest/out.tst.h>

TEST_DECL(setup)
{
	SetupStd();
	ts_ma(NULL, 0);
	ts_cma(NULL, 0);
}

TEST_DECL(ma1)
{
	TimeSeries *ts = ts_create("test", tindex_create(TS_YEAR, 2000), 0);
	ts_append(ts, 5, 1., 2., 5., 7., 13.);
	EXPECT(ts_print(OUT(), ts, "%.2f") > 0);
	TEST_EXPR(ts_ma(ts, 1));
	EXPECT(ts_print(OUT(), ts, "%.2f") > 0);
	EXPECT_OUT(
		"TimeSeries(\"test\", \"2000\",\n"
		"\t1.00, 2.00, 5.00, 7.00, 13.00\n"
		")\n"
		"TimeSeries(\"test\", \"2000\",\n"
		"\t1.00, 2.00, 5.00, 7.00, 13.00\n"
		")\n"
	);
	rd_deref(ts);
}

TEST_DECL(cma1)
{
	TimeSeries *ts = ts_create("test", tindex_create(TS_YEAR, 2000), 0);
	ts_append(ts, 5, 1., 2., 5., 7., 13.);
	EXPECT(ts_print(OUT(), ts, "%.2f") > 0);
	TEST_EXPR(ts_cma(ts, 1));
	EXPECT(ts_print(OUT(), ts, "%.2f") > 0);
	EXPECT_OUT(
		"TimeSeries(\"test\", \"2000\",\n"
		"\t1.00, 2.00, 5.00, 7.00, 13.00\n"
		")\n"
		"TimeSeries(\"test\", \"2000\",\n"
		"\t1.00, 2.00, 5.00, 7.00, 13.00\n"
		")\n"
	);
	rd_deref(ts);
}

TEST_DECL(ma2)
{
	TimeSeries *ts = ts_create("test", tindex_create(TS_YEAR, 2000), 0);
	ts_append(ts, 5, 1., 2., 5., 7., 13.);
	EXPECT(ts_print(OUT(), ts, "%.2f") > 0);
	TEST_EXPR(ts_ma(ts, 2));
	EXPECT(ts_print(OUT(), ts, "%.2f") > 0);
	EXPECT_OUT(
		"TimeSeries(\"test\", \"2000\",\n"
		"\t1.00, 2.00, 5.00, 7.00, 13.00\n"
		")\n"
		"TimeSeries(\"test\", \"2001\",\n"
		"\t1.50, 3.50, 6.00, 10.00\n"
		")\n"
	);
	rd_deref(ts);
}

TEST_DECL(cma2)
{
	TimeSeries *ts = ts_create("test", tindex_create(TS_YEAR, 2000), 0);
	ts_append(ts, 5, 1., 2., 5., 7., 13.);
	EXPECT(ts_print(OUT(), ts, "%.2f") > 0);
	TEST_EXPR(ts_cma(ts, 2));
	EXPECT(ts_print(OUT(), ts, "%.2f") > 0);
	EXPECT_OUT(
		"TimeSeries(\"test\", \"2000\",\n"
		"\t1.00, 2.00, 5.00, 7.00, 13.00\n"
		")\n"
		"TimeSeries(\"test\", \"2001\",\n"
		"\t2.50, 4.75, 8.00\n"
		")\n"
	);
	rd_deref(ts);
}

TEST_DECL(ma3)
{
	TimeSeries *ts = ts_create("test", tindex_create(TS_YEAR, 2000), 0);
	ts_append(ts, 5, 1., 2., 5., 7., 13.);
	EXPECT(ts_print(OUT(), ts, "%.2f") > 0);
	TEST_EXPR(ts_ma(ts, 3));
	EXPECT(ts_print(OUT(), ts, "%.2f") > 0);
	EXPECT_OUT(
		"TimeSeries(\"test\", \"2000\",\n"
		"\t1.00, 2.00, 5.00, 7.00, 13.00\n"
		")\n"
		"TimeSeries(\"test\", \"2002\",\n"
		"\t2.67, 4.67, 8.33\n"
		")\n"
	);
	rd_deref(ts);
}


TEST_DECL(cma3)
{
	TimeSeries *ts = ts_create("test", tindex_create(TS_YEAR, 2000), 0);
	ts_append(ts, 5, 1., 2., 5., 7., 13.);
	EXPECT(ts_print(OUT(), ts, "%.2f") > 0);
	TEST_EXPR(ts_cma(ts, 3));
	EXPECT(ts_print(OUT(), ts, "%.2f") > 0);
	EXPECT_OUT(
		"TimeSeries(\"test\", \"2000\",\n"
		"\t1.00, 2.00, 5.00, 7.00, 13.00\n"
		")\n"
		"TimeSeries(\"test\", \"2001\",\n"
		"\t2.67, 4.67, 8.33\n"
		")\n"
	);
	rd_deref(ts);
}

TEST_DECL(cma5)
{
	TimeSeries *ts = ts_create("test", tindex_create(TS_YEAR, 2000), 0);
	ts_append(ts, 7, 1., 2., 5., 7., 13., 5., 1.);
	EXPECT(ts_print(OUT(), ts, "%.2f") > 0);
	TEST_EXPR(ts_cma(ts, 5));
	EXPECT(ts_print(OUT(), ts, "%.2f") > 0);
	EXPECT_OUT(
		"TimeSeries(\"test\", \"2000\",\n"
		"\t1.00, 2.00, 5.00, 7.00, 13.00, 5.00,\n"
		"\t1.00\n"
		")\n"
		"TimeSeries(\"test\", \"2002\",\n"
		"\t5.60, 6.40, 6.20\n"
		")\n"
	);
	rd_deref(ts);
}

TEST_DECL(max)
{
	TimeSeries *ts = ts_create("test", tindex_create(TS_YEAR, 2000), 0);
	ts_append(ts, 5, 1., 2., 5., 7., 13.);
	EXPECT(ts_print(OUT(), ts, "%.2f") > 0);
	TEST_EXPR(ts_ma(ts, 10));
	EXPECT(ts_print(OUT(), ts, "%.2f") > 0);
	EXPECT_OUT(
		"TimeSeries(\"test\", \"2000\",\n"
		"\t1.00, 2.00, 5.00, 7.00, 13.00\n"
		")\n"
		"TimeSeries(\"test\", \"2004\",\n"
		"\t5.60\n"
		")\n"
	);
	rd_deref(ts);
}

TEST_DECL(cmax)
{
	TimeSeries *ts = ts_create("test", tindex_create(TS_YEAR, 2000), 0);
	ts_append(ts, 5, 1., 2., 5., 7., 13.);
	EXPECT(ts_print(OUT(), ts, "%.2f") > 0);
	TEST_EXPR(ts_cma(ts, 10));
	EXPECT(ts_print(OUT(), ts, "%.2f") > 0);
	EXPECT_OUT(
		"TimeSeries(\"test\", \"2000\",\n"
		"\t1.00, 2.00, 5.00, 7.00, 13.00\n"
		")\n"
		"TimeSeries(\"test\", \"2002\",\n"
		"\t5.60\n"
		")\n"
	);
	rd_deref(ts);
}

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(ma1);
	TEST_RUN(cma1);
	TEST_RUN(ma2);
	TEST_RUN(cma2);
	TEST_RUN(ma3);
	TEST_RUN(cma3);
	TEST_RUN(cma5);
	TEST_RUN(max);
	TEST_RUN(cmax);
	CLEANUP_OUT;
}

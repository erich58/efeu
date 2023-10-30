#include <EFEU/UnitTest.h>
#include <Math/TimeSeries.h>
#include <utest/out.tst.h>

static const char *ts_out(TimeSeries *ts)
{
	ts_print(OUT(), ts, "%.2f");
	rd_deref(ts);
	return utest_out();
}

#define	EXPECT_TS(ts, s)	\
UnitTest_diff(__FILE__, __LINE__, ts_out(ts), s, # ts, # s)

TEST_DECL(setup)
{
	SetupStd();
	EXPECT(ts_convert(NULL, NULL, 0, NULL) == NULL); 
}

TEST_DECL(jbase)
{
	TimeSeries *base = ts_create("base", tindex_create(TS_YEAR, 2000), 0);
	ts_append(base, 1, 366.);
	EXPECT_TS(ts_convert("conv", base, TS_QUART, NULL),
		"TimeSeries(\"conv\", \"2000@1\",\n"
		"\t366.00, 366.00, 366.00, 366.00\n"
		")\n"
	);
	EXPECT_TS(ts_convert("conv", base, TS_YEAR, "dmode"),
		"TimeSeries(\"conv\", \"2000\",\n"
		"\t366.00\n"
		")\n"
	);
	EXPECT_TS(ts_convert("conv", base, TS_QUART, "anteil"),
		"TimeSeries(\"conv\", \"2000@1\",\n"
		"\t91.50, 91.50, 91.50, 91.50\n"
		")\n"
	);
	EXPECT_TS(ts_convert("conv", base, TS_QUART, "anteil dmode"),
		"TimeSeries(\"conv\", \"2000@1\",\n"
		"\t91.00, 91.00, 92.00, 92.00\n"
		")\n"
	);
	EXPECT_TS(ts_convert("conv", base, TS_MONTH, "anteil dmode"),
		"TimeSeries(\"conv\", \"2000-01\",\n"
		"\t31.00, 29.00, 31.00, 30.00, 31.00, 30.00,\n"
		"\t31.00, 31.00, 30.00, 31.00, 30.00, 31.00\n"
		")\n"
	);
	EXPECT_TS(base,
		"TimeSeries(\"base\", \"2000\",\n"
		"\t366.00\n"
		")\n"
	);
}

TEST_DECL(qbase)
{
	TimeSeries *base = ts_create("base",
		tindex_create(TS_QUART, 4 * 2000), 0);
	ts_append(base, 4, 91., 91., 92., 92.);
	EXPECT_TS(ts_convert("conv", base, TS_QUART, NULL),
		"TimeSeries(\"conv\", \"2000@1\",\n"
		"\t91.00, 91.00, 92.00, 92.00\n"
		")\n"
	);
	EXPECT_TS(ts_convert("conv", base, TS_MONTH, "anteil dmode"),
		"TimeSeries(\"conv\", \"2000-01\",\n"
		"\t31.00, 29.00, 31.00, 30.00, 31.00, 30.00,\n"
		"\t31.00, 31.00, 30.00, 31.00, 30.00, 31.00\n"
		")\n"
	);
	EXPECT_TS(ts_convert("conv", base, TS_YEAR, ""),
		"TimeSeries(\"conv\", \"2000\",\n"
		"\t91.50\n"
		")\n"
	);
	EXPECT_TS(ts_convert("conv", base, TS_YEAR, "first"),
		"TimeSeries(\"conv\", \"2000\",\n"
		"\t91.00\n"
		")\n"
	);
	EXPECT_TS(ts_convert("conv", base, TS_YEAR, "last"),
		"TimeSeries(\"conv\", \"2000\",\n"
		"\t92.00\n"
		")\n"
	);
	EXPECT_TS(ts_convert("conv", base, TS_YEAR, "center"),
		"TimeSeries(\"conv\", \"2000\",\n"
		"\t92.00\n"
		")\n"
	);
	EXPECT_TS(ts_convert("conv", base, TS_YEAR, "anteil"),
		"TimeSeries(\"conv\", \"2000\",\n"
		"\t366.00\n"
		")\n"
	);
	EXPECT_TS(base,
		"TimeSeries(\"base\", \"2000@1\",\n"
		"\t91.00, 91.00, 92.00, 92.00\n"
		")\n"
	);
}

TEST_DECL(mbase)
{
	TimeSeries *base = ts_create("base",
		tindex_create(TS_MONTH, 12 * 2000), 0);
	ts_append(base, 1, 31.);

	EXPECT_TS(ts_convert("conv", base, TS_DAY, "anteil"),
		"TimeSeries(\"conv\", \"2000-01-01\",\n"
		"\t1.00, 1.00, 1.00, 1.00, 1.00, 1.00,\n"
		"\t1.00, 1.00, 1.00, 1.00, 1.00, 1.00,\n"
		"\t1.00, 1.00, 1.00, 1.00, 1.00, 1.00,\n"
		"\t1.00, 1.00, 1.00, 1.00, 1.00, 1.00,\n"
		"\t1.00, 1.00, 1.00, 1.00, 1.00, 1.00,\n"
		"\t1.00\n"
		")\n"
	);
	ts_append(base, 2, 29., 31.);
	EXPECT_TS(ts_convert("conv", base, TS_MONTH, "anteil"),
		"TimeSeries(\"conv\", \"2000-01\",\n"
		"\t31.00, 29.00, 31.00\n"
		")\n"
	);
	EXPECT_TS(ts_convert("conv", base, TS_QUART, "anteil"),
		"TimeSeries(\"conv\", \"2000@1\",\n"
		"\t91.00\n"
		")\n"
	);
}

TEST_DECL(dbase)
{
	TimeSeries *base = ts_create("base",
		str2TimeIndex("2000-01-01", NULL), 366);

	for (int i = 0; i < base->dim; i++)
		base->data[i] = 1.;

	EXPECT_TS(ts_convert("conv", base, TS_YEAR, "anteil"),
		"TimeSeries(\"conv\", \"2000\",\n"
		"\t366.00\n"
		")\n"
	);
}

TEST_DECL(linear)
{
	TimeSeries *base = ts_create("base", tindex_create(TS_YEAR, 2000), 0);
	ts_append(base, 2, 10., 26.);
	EXPECT_TS(ts_convert("conv", base, TS_QUART, "linear anteil"),
		"TimeSeries(\"conv\", \"2000@1\",\n"
		"\t1.00, 2.00, 3.00, 4.00, 5.00, 6.00,\n"
		"\t7.00, 8.00\n"
		")\n"
	);
	EXPECT_TS(ts_convert("conv", base, TS_QUART, "linear"),
		"TimeSeries(\"conv\", \"2000@1\",\n"
		"\t4.00, 8.00, 12.00, 16.00, 20.00, 24.00,\n"
		"\t28.00, 32.00\n"
		")\n"
	);
}

TEST_DECL(spline)
{
	TimeSeries *base = ts_create("base", tindex_create(TS_YEAR, 2000), 0);
	ts_append(base, 3, 10., 20., 10.);
	EXPECT_TS(ts_convert("conv", base, TS_QUART, "spline"),
		"TimeSeries(\"conv\", \"2000@1\",\n"
		"\t4.67, 8.14, 11.86, 15.33, 18.11, 19.71,\n"
		"\t19.71, 18.11, 15.33, 11.86, 8.14, 4.67\n"
		")\n"
	);
}

TEST_DECL(mean)
{
	TimeSeries *base = ts_create("base", tindex_create(TS_YEAR, 2000), 0);
	ts_append(base, 3, 10., 20., 10.);
	EXPECT_TS(ts_convert("conv", base, TS_QUART, "mittel"),
		"TimeSeries(\"conv\", \"2000@1\",\n"
		"\t6.87, 8.12, 10.63, 14.38, 18.75, 21.25,\n"
		"\t21.25, 18.75, 14.38, 10.63, 8.13, 6.87\n"
		")\n"
	);
	EXPECT_TS(ts_convert("conv", base, TS_QUART, "mittel anteil"),
		"TimeSeries(\"conv\", \"2000@1\",\n"
		"\t1.72, 2.03, 2.66, 3.59, 4.69, 5.31,\n"
		"\t5.31, 4.69, 3.59, 2.66, 2.03, 1.72\n"
		")\n"
	);
}

TEST_DECL(dspline)
{
	TimeSeries *base = ts_create("base", tindex_create(TS_YEAR, 2000), 0);
	ts_append(base, 2, 10., 20.);
	EXPECT_TS(ts_convert("conv", base, TS_QUART, "dspline"),
		"TimeSeries(\"conv\", \"2000@1\",\n"
		"\t10.00, 10.00, 10.55, 13.20, 16.80, 19.45,\n"
		"\t20.00, 20.00\n"
		")\n"
	);
}

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(jbase);
	TEST_RUN(qbase);
	TEST_RUN(mbase);
	TEST_RUN(dbase);
	TEST_RUN(linear);
	TEST_RUN(dspline);
	TEST_RUN(spline);
	TEST_RUN(mean);
	CLEANUP_OUT;
}

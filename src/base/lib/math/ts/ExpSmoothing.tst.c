#include <EFEU/UnitTest.h>
#include <Math/TimeSeries.h>
#include <EFEU/utest/out.tst.h>

static const char *ts_out(TimeSeries *ts)
{
	ts_print(OUT(), ts, "%.2f");
	rd_deref(ts);
	return utest_out();
}

#define	EXPECT_TS(ts, s)	\
UnitTest_diff(__FILE__, __LINE__, ts_out(ts), s, # ts, # s)

TEST_SUITE
{
	TimeSeries *base = ts_create("base", tindex_create(TS_INDEX, 0), 0);	
	ts_append(base, 5, 10., 11.5, 12., 12.5, 14.);
	EXPECT_DBL(ExpSmoothingInitial(NULL, 0.5), 0.);
	EXPECT_DBL(ExpSmoothingInitial(base, 0.), 11.);
	EXPECT_DBL(ExpSmoothingInitial(base, 0.5), 12.06451612903226);
	EXPECT_DBL(ExpSmoothingInitial(base, 1.), 14.);
	EXPECT_DBL(ExpSmoothingError(base, 0., 11.), 12.5);
	EXPECT_DBL(ExpSmoothingError(base, 0.5, 12.), 5.33203125);
	EXPECT_DBL(ExpSmoothingError(base, 1., 14.), 5.);

	EXPECT_TS(ExpSmoothing(NULL, base, 0., 42., -1),
		"TimeSeries(NULL, \"#0\",\n"
		"\t42.00, 42.00, 42.00, 42.00, 42.00\n"
		")\n"
	);

	EXPECT_TS(ExpSmoothing("$1-0", base, 0., 11., 1),
		"TimeSeries(\"base-0\", \"#0\",\n"
		"\t11.00, 11.00, 11.00, 11.00, 11.00, 11.00\n"
		")\n"
	);

	EXPECT_TS(ExpSmoothing("$1-0", base, 0.5, 11., 1),
		"TimeSeries(\"base-0\", \"#0\",\n"
		"\t10.50, 11.00, 11.50, 12.00, 13.00, 13.00\n"
		")\n"
	);

	EXPECT_TS(ExpSmoothing("$1-1", base, 1., 14., 1),
		"TimeSeries(\"base-1\", \"#0\",\n"
		"\t10.00, 11.50, 12.00, 12.50, 14.00, 14.00\n"
		")\n"
	);

	EXPECT_TS(base,
		"TimeSeries(\"base\", \"#0\",\n"
		"\t10.00, 11.50, 12.00, 12.50, 14.00\n"
		")\n"
	);

	CLEANUP_OUT;
}

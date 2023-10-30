#include <EFEU/UnitTest.h>
#include <Math/TimeSeries.h>
#include <utest/out.tst.h>

TEST_SUITE
{
	TimeSeries *base = ts_create("base", tindex_create(TS_YEAR, 2000), 0);
	ts_append(base, 5, 1., 2., 5., 7., 13.);
	TimeSeries *copy = ts_copy("copy", base, 2, 4);
	EXPECT(ts_print(OUT(), base, "%.2f") > 0);
	EXPECT(ts_print(OUT(), copy, "%.2f") > 0);
	EXPECT_OUT(
		"TimeSeries(\"base\", \"2000\",\n"
		"\t1.00, 2.00, 5.00, 7.00, 13.00\n"
		")\n"
		"TimeSeries(\"copy\", \"2002\",\n"
		"\t5.00, 7.00\n"
		")\n"
	);
	rd_deref(base);
	rd_deref(copy);
	CLEANUP_OUT;
}

#include <EFEU/UnitTest.h>
#include <EFEU/strbuf.h>
#include <EFEU/io.h>
#include <Math/TimeSeries.h>

TEST_SUITE
{
	SetupStd();

	STRBUF(buf, 1024);
	IO *io = io_strbuf(&buf);

	EXPECT(TimeSeries2mdmat(NULL) == NULL);

	TimeSeries *ts = ts_create("test", tindex_create(TS_YEAR, 2000), 0);
	ts_append(ts, 5, 1., 2., 3., 4., 5.);
	EXPECT(ts_print(io, ts, "%.0f") > 0);
	EXPECT_STR(sb_cstr(&buf),
		"TimeSeries(\"test\", \"2000\",\n"
		"\t1, 2, 3, 4, 5\n"
		")\n"
	);

	mdmat *md = TimeSeries2mdmat(ts);

	io_rewind(io);
	md_print(io_refer(io), md, NULL);
	EXPECT_STR(sb_cstr(&buf),
		"##MDMAT   2.0\n"
		"##Title   test\n"
		"##Type    double\n"
		"##Locale  C\n"
		"##Rows    year\n"
		"##Columns \n"
		"1 .\n"
		"2000 1.0000\n"
		"2001 2.0000\n"
		"2002 3.0000\n"
		"2003 4.0000\n"
		"2004 5.0000\n"
	);
}

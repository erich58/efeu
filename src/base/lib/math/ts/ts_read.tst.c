#include <EFEU/UnitTest.h>
#include <EFEU/strbuf.h>
#include <EFEU/io.h>
#include <Math/TimeSeries.h>

TEST_SUITE
{
	STRBUF(buf, 1024);
	IO *io = io_strbuf(&buf);

	EXPECT(ts_read(NULL) == NULL);
	EXPECT(ts_write(NULL, NULL) == 0);

	TimeSeries *ts = ts_create("test", tindex_create(TS_YEAR, 2000), 0);
	ts_append(ts, 5, 1., 2., 3., 4., 5.);
	EXPECT(ts_print(io, ts, "%.0f") > 0);
	EXPECT_STR(sb_cstr(&buf),
		"TimeSeries(\"test\", \"2000\",\n"
		"\t1, 2, 3, 4, 5\n"
		")\n"
	);

	io_rewind(io);
	EXPECT(ts_write(io, ts) > 0); 
	rd_deref(ts);

	io_rewind(io);
	ts = ts_read(io);

	io_rewind(io);
	EXPECT(ts_print(io, ts, "%.0f") > 0);
	EXPECT_STR(sb_cstr(&buf),
		"TimeSeries(\"test\", \"2000\",\n"
		"\t1, 2, 3, 4, 5\n"
		")\n"
	);
}

#include <EFEU/UnitTest.h>
#include <Math/TimeSeries.h>
#include "utest/eval.tst.h"

extern int PrintFloatPrec;

TEST_DECL(setup)
{
	SetupStd();
	CmdSetup_TimeIndex();
	CmdSetup_TimeSeries();
	TEST_EXPR(PrintFloatPrec = 2);
	EXPECT_EVAL("fmt_float = \"%#*.*f\"", "\"%#*.*f\"");
	EXPECT_EVAL("TimeSeries::fmt = \"%#.2f\"", "\"%#.2f\"");
}

TEST_DECL(member)
{
	EXPECT_EVAL("TimeSeries ts", "NULL");
	EXPECT_EVAL("ts.name", "NULL");
	EXPECT_EVAL("ts.base", "#0");
	EXPECT_EVAL("ts.last", "#0");
	EXPECT_EVAL("ts.dim", "0");

	EXPECT_EVAL("ts = TimeSeries(\"test\","
		"\"2010-01\", 10., 20., 30.)",
		"TimeSeries(\"test\", \"2010-01\",\n"
		"\t10.00, 20.00, 30.00\n"
		")\n"
	);
	EXPECT_EVAL("ts.name", "\"test\"");
	EXPECT_EVAL("ts.base", "2010-01");
	EXPECT_EVAL("ts.last", "2010-03");
	EXPECT_EVAL("ts.dim", "3");
}

TEST_DECL(create)
{
	EXPECT_EVAL("TimeSeries(\"empty\", \"2010\")",
		"TimeSeries(\"empty\", \"2010\")\n"
	);
	EXPECT_EVAL("TimeSeries(\"dim\", \"2010\", 5, 10.0, 1.0)",
		"TimeSeries(\"dim\", \"2010\",\n"
		"\t10.00, 11.00, 12.00, 13.00, 14.00\n"
		")\n"
	);
	EXPECT_EVAL("TimeSeries(\"range\", \"2010\", \"2013\", 10.)",
		"TimeSeries(\"range\", \"2010\",\n"
		"\t10.00, 10.00, 10.00, 10.00\n"
		")\n"
	);
	EXPECT_EVAL("TimeSeries(\"list\", \"2010\", 10., 20.)",
		"TimeSeries(\"list\", \"2010\",\n"
		"\t10.00, 20.00\n"
		")\n"
	);
}

TEST_DECL(io)
{
	EXPECT_EVAL("TimeSeries ts = TimeSeries(\"test\", \"#1\", 42.)",
		"TimeSeries(\"test\", \"#1\",\n"
		"\t42.00\n"
		")\n"
	);
	EXPECT_EVAL("IO io = tmpbuf();", "");
	EXPECT_EVAL("write(io, ts)", "1");
	EXPECT_EVAL("rewind(io)", "0");
	EXPECT_EVAL("ts = NULL", "NULL");
	EXPECT_EVAL("read(io, ts)", "1");
	EXPECT_EVAL("rewind(io)", "0");
	EXPECT_EVAL("ts",
		"TimeSeries(\"test\", \"#1\",\n"
		"\t42.00\n"
		")\n"
	);
	EXPECT_EVAL("print(cout, ts);",
		"TimeSeries(\"test\", \"#1\",\n"
		"\t42.00\n"
		")\n"
	);
}

TEST_DECL(copy)
{
	EXPECT_EVAL("TimeSeries ts = TimeSeries(\"a\", \"2020\", 6, 10., 1.)",
		"TimeSeries(\"a\", \"2020\",\n"
		"\t10.00, 11.00, 12.00, 13.00, 14.00, 15.00\n"
		")\n"
	);
	EXPECT_EVAL("TimeSeries(ts)",
		"TimeSeries(\"a\", \"2020\",\n"
		"\t10.00, 11.00, 12.00, 13.00, 14.00, 15.00\n"
		")\n"
	);
	EXPECT_EVAL("TimeSeries(ts, \"2022\")",
		"TimeSeries(\"a\", \"2022\",\n"
		"\t12.00, 13.00, 14.00, 15.00\n"
		")\n"
	);
	EXPECT_EVAL("TimeSeries(ts, \"2022\", \"2024\")",
		"TimeSeries(\"a\", \"2022\",\n"
		"\t12.00, 13.00, 14.00\n"
		")\n"
	);
	EXPECT_EVAL("TimeSeries(ts, \"2022\", 3)",
		"TimeSeries(\"a\", \"2022\",\n"
		"\t12.00, 13.00, 14.00\n"
		")\n"
	);
	EXPECT_EVAL("TimeSeries(ts, \"2022\", \"2027\")",
		"TimeSeries(\"a\", \"2022\",\n"
		"\t12.00, 13.00, 14.00, 15.00, 0.00, 0.00\n"
		")\n"
	);
	EXPECT_EVAL("TimeSeries(ts, \"2022\", 6)",
		"TimeSeries(\"a\", \"2022\",\n"
		"\t12.00, 13.00, 14.00, 15.00, 0.00, 0.00\n"
		")\n"
	);
	EXPECT_EVAL("TimeSeries(\"b\", ts)",
		"TimeSeries(\"b\", \"2020\",\n"
		"\t10.00, 11.00, 12.00, 13.00, 14.00, 15.00\n"
		")\n"
	);
	EXPECT_EVAL("TimeSeries(\"b\", ts, \"2022\")",
		"TimeSeries(\"b\", \"2022\",\n"
		"\t12.00, 13.00, 14.00, 15.00\n"
		")\n"
	);
	EXPECT_EVAL("TimeSeries(\"b\", ts, \"2022\", \"2024\")",
		"TimeSeries(\"b\", \"2022\",\n"
		"\t12.00, 13.00, 14.00\n"
		")\n"
	);
	EXPECT_EVAL("TimeSeries(\"b\", ts, \"2022\", 3)",
		"TimeSeries(\"b\", \"2022\",\n"
		"\t12.00, 13.00, 14.00\n"
		")\n"
	);
	EXPECT_EVAL("TimeSeries(\"b\", ts, \"2022\", \"2027\")",
		"TimeSeries(\"b\", \"2022\",\n"
		"\t12.00, 13.00, 14.00, 15.00, 0.00, 0.00\n"
		")\n"
	);
	EXPECT_EVAL("TimeSeries(\"b\", ts, \"2022\", 6)",
		"TimeSeries(\"b\", \"2022\",\n"
		"\t12.00, 13.00, 14.00, 15.00, 0.00, 0.00\n"
		")\n"
	);
}

TEST_DECL(resize)
{
	EXPECT_EVAL("TimeSeries ts = TimeSeries(\"a\", \"2020\", 6, 10., 1.)",
		"TimeSeries(\"a\", \"2020\",\n"
		"\t10.00, 11.00, 12.00, 13.00, 14.00, 15.00\n"
		")\n"
	);
	EXPECT_EVAL("ts.resize(\"2022\")",
		"TimeSeries(\"a\", \"2020\",\n"
		"\t10.00, 11.00, 12.00\n"
		")\n"
	);
	EXPECT_EVAL("ts.resize(6, 5.)",
		"TimeSeries(\"a\", \"2020\",\n"
		"\t10.00, 11.00, 12.00, 5.00, 5.00, 5.00\n"
		")\n"
	);
}

TEST_DECL(sync)
{
	EXPECT_EVAL("TimeSeries ts = TimeSeries(\"a\", \"2020\", 6, 10., 1.)",
		"TimeSeries(\"a\", \"2020\",\n"
		"\t10.00, 11.00, 12.00, 13.00, 14.00, 15.00\n"
		")\n"
	);
	EXPECT_EVAL("ts.sync(\"2019\", 3)",
		"TimeSeries(\"a\", \"2019\",\n"
		"\t0.00, 10.00, 11.00\n"
		")\n"
	);
	EXPECT_EVAL("ts.sync(\"2021\", \"2022\")",
		"TimeSeries(\"a\", \"2021\",\n"
		"\t11.00, 0.00\n"
		")\n"
	);
}

TEST_DECL(sum)
{
	EXPECT_EVAL("TimeSeries ts = TimeSeries(\"a\", \"2020\", 1., 3., 5., 8.)",
		"TimeSeries(\"a\", \"2020\",\n"
		"\t1.00, 3.00, 5.00, 8.00\n"
		")\n"
	);
	EXPECT_EVAL("ts.sum()", "17.00");
	EXPECT_EVAL("ts.sum(\"2022\")", "13.00");
	EXPECT_EVAL("ts.sum(\"2021\", \"2022\")", "8.00");
	EXPECT_EVAL("ts.qsum()", "99.00");
	EXPECT_EVAL("ts.qsum(\"2022\")", "89.00");
	EXPECT_EVAL("ts.qsum(\"2021\", \"2022\")", "34.00");
	EXPECT_EVAL("ts.min()", "1.00");
	EXPECT_EVAL("ts.max()", "8.00");
	EXPECT_EVAL("ts = NULL", "NULL");
	EXPECT_EVAL("ts.sum()", "0.00");
	EXPECT_EVAL("ts.qsum()", "0.00");
	EXPECT_EVAL("ts.min()", "0.00");
	EXPECT_EVAL("ts.max()", "0.00");
}

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(member);
	TEST_RUN(create);
	TEST_RUN(io);
	TEST_RUN(copy);
	TEST_RUN(resize);
	TEST_RUN(sync);
	TEST_RUN(sum);
	CLEANUP_EVAL;
}

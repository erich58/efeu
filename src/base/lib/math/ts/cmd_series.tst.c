#include <EFEU/UnitTest.h>
#include <Math/TimeSeries.h>
#include <EFEU/utest/eval.tst.h>

extern int PrintFloatPrec;

TEST_DECL(setup)
{
	SetupStd();
	SetupMdMat();
	SetupMath();
	SetProgName("ID");
	SetApplPath(UnitTest_tmp(NULL));
	TEST_FILE("TimeSeries.msg",
		"1	$!: index not compatible: $1 - $2.\n"
		"2	$!: unknown index type '$1'.\n"
		"11	$!: time index out of bound.\n"
		"21	$!: function for time series is not defined.\n"
	);
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
	EXPECT_EVAL("dim(ts)", "0");
	EXPECT_EVAL("ts << 1", "NULL");
	EXPECT_EVAL("ts >> 1", "NULL");

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
	EXPECT_EVAL("dim(ts)", "3");
	EXPECT_EVAL("ts >> 2",
		"TimeSeries(NULL, \"2010-03\",\n"
		"\t10.00, 20.00, 30.00\n"
		")\n"
	);
	EXPECT_EVAL("ts << 2",
		"TimeSeries(NULL, \"2009-11\",\n"
		"\t10.00, 20.00, 30.00\n"
		")\n"
	);
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

TEST_DECL(ma)
{
	EXPECT_EVAL("TimeSeries ts = TimeSeries(\"test\", "
		"\"2000\", 1., 2., 5., 7., 13.)",
		"TimeSeries(\"test\", \"2000\",\n"
		"\t1.00, 2.00, 5.00, 7.00, 13.00\n"
		")\n"
	);
	EXPECT_EVAL("ts.ma(2)",
		"TimeSeries(\"test\", \"2001\",\n"
		"\t1.50, 3.50, 6.00, 10.00\n"
		")\n"
	);
}

TEST_DECL(cma)
{
	EXPECT_EVAL("TimeSeries ts = TimeSeries(\"test\", "
		"\"2000\", 1., 2., 5., 7., 13.)",
		"TimeSeries(\"test\", \"2000\",\n"
		"\t1.00, 2.00, 5.00, 7.00, 13.00\n"
		")\n"
	);
	EXPECT_EVAL("ts.cma(3)",
		"TimeSeries(\"test\", \"2001\",\n"
		"\t2.67, 4.67, 8.33\n"
		")\n"
	);
}

TEST_DECL(diff)
{
	EXPECT_EVAL("TimeSeries ts = NULL", "NULL");
	EXPECT_EVAL("TimeSeries ts.diff(2)", "NULL");

	EXPECT_EVAL("TimeSeries ts = TimeSeries(\"test\", "
		"\"2000\", 1., 2., 5., 7., 13.)",
		"TimeSeries(\"test\", \"2000\",\n"
		"\t1.00, 2.00, 5.00, 7.00, 13.00\n"
		")\n"
	);
	EXPECT_EVAL("TimeSeries ts.diff(0)", "NULL");
	UnitTest_stderr("stderr");
	EXPECT_EVAL("TimeSeries ts.diff(2, log)", "NULL");
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"MSG ID: function for time series is not defined.\n");

	EXPECT_EVAL("TimeSeries base = ts.diff(2)",
		"TimeSeries(\"test\", \"2000\",\n"
		"\t1.00, 2.00\n"
		")\n"
	);
	EXPECT_EVAL("ts",
		"TimeSeries(\"test\", \"2002\",\n"
		"\t4.00, 5.00, 8.00\n"
		")\n"
	);

	UnitTest_stderr("stderr");
	EXPECT_EVAL("TimeSeries ts.cum(TimeSeries(\"x\", \"#1\"))", "");
	EXPECT_EVAL("TimeSeries ts.cum(base, log)", "");
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"MSG ID: index not compatible: 'y' - '#'.\n"
		"MSG ID: function for time series is not defined.\n"
	);

	EXPECT_EVAL("ts.cum(base)", "");
	EXPECT_EVAL("ts",
		"TimeSeries(\"test\", \"2000\",\n"
		"\t1.00, 2.00, 5.00, 7.00, 13.00\n"
		")\n"
	);

	EXPECT_EVAL("TimeSeries base = ts.diff(-2)",
		"TimeSeries(\"test\", \"2003\",\n"
		"\t7.00, 13.00\n"
		")\n"
	);
	EXPECT_EVAL("ts",
		"TimeSeries(\"test\", \"2000\",\n"
		"\t-4.00, -5.00, -8.00\n"
		")\n"
	);

	EXPECT_EVAL("ts.cum(base)", "");
	EXPECT_EVAL("ts",
		"TimeSeries(\"test\", \"2000\",\n"
		"\t1.00, 2.00, 5.00, 7.00, 13.00\n"
		")\n"
	);
}

TEST_DECL(conv)
{
	EXPECT_EVAL("TimeSeries ts = TimeSeries(\"test\", \"2000\", 366.)",
		"TimeSeries(\"test\", \"2000\",\n"
		"\t366.00\n"
		")\n"
	);
	EXPECT_EVAL("ts.conv(\"quart\", \"anteil dmode\")",
		"TimeSeries(\"test\", \"2000@1\",\n"
		"\t91.00, 91.00, 92.00, 92.00\n"
		")\n"
	);
}

TEST_DECL(ts2md)
{
	EXPECT_EVAL("TimeSeries ts = TimeSeries(\"test\", \"2000\", 1 : 5)",
		"TimeSeries(\"test\", \"2000\",\n"
		"\t1.00, 2.00, 3.00, 4.00, 5.00\n"
		")\n"
	);
	EXPECT_EVAL("mdmat md = ts",
		"MD: double[5] \"test\"\n"
	);
}

TEST_DECL(index)
{
	EXPECT_EVAL("TimeSeries ts = NULL", "NULL");
	UnitTest_stderr("stderr");
	EXPECT_EVAL("ts[2]", "");
	EXPECT_EVAL("TimeSeries(ts)[2]", "0.00");
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"MSG ID: time index out of bound.\n"
		"MSG ID: time index out of bound.\n"
	);

	EXPECT_EVAL("ts = TimeSeries(\"test\", \"2000\", 0 : 5)",
		"TimeSeries(\"test\", \"2000\",\n"
		"\t0.00, 1.00, 2.00, 3.00, 4.00, 5.00\n"
		")\n"
	);

	EXPECT_EVAL("TimeSeries(ts)[2]", "2.00");
	EXPECT_EVAL("ts[\"2003\"]", "3.00");
	EXPECT_EVAL("ts[2] = 5.00", "5.00");
}

TEST_DECL(func)
{
	EXPECT_EVAL("- TimeSeries(\"base\", \"#1\", -10., 42.)",
		"TimeSeries(NULL, \"#1\",\n"
		"\t10.00, -42.00\n"
		")\n"
	);
	EXPECT_EVAL("exp(TimeSeries(\"base\", \"#1\", 0., 1.))",
		"TimeSeries(NULL, \"#1\",\n"
		"\t1.00, 2.72\n"
		")\n"
	);
	EXPECT_EVAL("double func (double x) return x + 0.5;", "");
	EXPECT_EVAL("call(func, TimeSeries(\"base\", \"#1\", 0., 1.))",
		"TimeSeries(NULL, \"#1\",\n"
		"\t0.50, 1.50\n"
		")\n"
	);
}

TEST_DECL(seasonal)
{
	EXPECT_EVAL("TimeSeries ts = TimeSeries(\"base\", \"#1\", "
		"1., 2.6, 3., 4., 5.6, 6.)",
		"TimeSeries(\"base\", \"#1\",\n"
		"\t1.00, 2.60, 3.00, 4.00, 5.60, 6.00\n"
		")\n"
	);
	EXPECT_EVAL("Seasonal(ts, 3)",
		"TimeSeries(\"S(base)\", \"#1\",\n"
		"\t-0.20, 0.40, -0.20, -0.20, 0.40, -0.20\n"
		")\n"
	);
	EXPECT_EVAL("Seasonal(\"name\", ts, 3)",
		"TimeSeries(\"name\", \"#1\",\n"
		"\t-0.20, 0.40, -0.20, -0.20, 0.40, -0.20\n"
		")\n"
	);
	EXPECT_EVAL("TimeSeries ts = TimeSeries(\"base\", \"#1\", "
		"1., 2.6, 3., 4.6, 5.0, 6.6, 7., 8.6)",
		"TimeSeries(\"base\", \"#1\",\n"
		"\t1.00, 2.60, 3.00, 4.60, 5.00, 6.60,\n"
		"\t7.00, 8.60\n"
		")\n"
	);
	EXPECT_EVAL("Seasonal(ts, 4)",
		"TimeSeries(\"S(base)\", \"#1\",\n"
		"\t-0.30, 0.30, -0.30, 0.30, -0.30, 0.30,\n"
		"\t-0.30, 0.30\n"
		")\n"
	);
	EXPECT_EVAL("Seasonal(ts, 10)", "NULL");
}

TEST_DECL(assign)
{
	EXPECT_EVAL("TimeSeries a = TimeSeries(\"a\", \"#1\", 1., 2., 3.)",
		"TimeSeries(\"a\", \"#1\",\n"
		"\t1.00, 2.00, 3.00\n"
		")\n"
	);
	EXPECT_EVAL("a += TimeSeries(\"b\", \"#2\", 10.)",
		"TimeSeries(\"a\", \"#1\",\n"
		"\t1.00, 12.00, 3.00\n"
		")\n"
	);
}

TEST_DECL(term)
{
	EXPECT_EVAL("TimeSeries a = TimeSeries(\"a\", \"#1\", 1., 2., 3.)",
		"TimeSeries(\"a\", \"#1\",\n"
		"\t1.00, 2.00, 3.00\n"
		")\n"
	);
	EXPECT_EVAL("TimeSeries b = TimeSeries(\"b\", \"#2\", 10., 20., 30.)",
		"TimeSeries(\"b\", \"#2\",\n"
		"\t10.00, 20.00, 30.00\n"
		")\n"
	);
	EXPECT_EVAL("a + b",
		"TimeSeries(NULL, \"#2\",\n"
		"\t12.00, 23.00\n"
		")\n"
	);
	EXPECT_EVAL("double f(double x, double y) return 0.5 * (x + y);", "");
	EXPECT_EVAL("call(f, a, b)",
		"TimeSeries(NULL, \"#2\",\n"
		"\t6.00, 11.50\n"
		")\n"
	);
}

TEST_DECL(ExtSmoothing)
{
	EXPECT_EVAL("TimeSeries ts = NULL", "NULL");
	EXPECT_EVAL("ExpSmoothing(ts, 0., 0.)", "NULL");
	EXPECT_EVAL("ExpSmoothingError(ts, 0., 0.)", "0.00");
	EXPECT_EVAL("ExpSmoothingInitial(ts, 0.)", "0.00");
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
	TEST_RUN(ma);
	TEST_RUN(cma);
	TEST_RUN(diff);
	TEST_RUN(conv);
	TEST_RUN(ts2md);
	TEST_RUN(index);
	TEST_RUN(func);
	TEST_RUN(seasonal);
	TEST_RUN(assign);
	TEST_RUN(term);
	TEST_RUN(ExtSmoothing);
	CLEANUP_EVAL;
}

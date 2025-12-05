#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <Math/TimeSeries.h>
#include <EFEU/utest/eval.tst.h>

extern int PrintFloatPrec;

TEST_SUITE
{
	SetupStd();
	CmdSetup_TimeIndex();
	TEST_EXPR(PrintFloatPrec = 2);
	EXPECT_EVAL("fmt_float = \"%#*.*f\"", "\"%#*.*f\"");
	EXPECT_EVAL("TimeIndex t", "#0");
	UnitTest_stderr("err");
	EXPECT_EVAL("t = \"2000-2xxx\"", "2000-02");
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("err", "rest=xxx\n");
	EXPECT_EVAL("(str) t", "\"2000-02\"");
	EXPECT_EVAL("print(cout, t);", "TimeIndex(\"2000-02\")");
	EXPECT_EVAL("t.year", "2000");
	EXPECT_EVAL("t.quart", "1");
	EXPECT_EVAL("t.month", "2");
	EXPECT_EVAL("t.length", "29");
	EXPECT_EVAL("t.beg", "2000-02-01");
	EXPECT_EVAL("t.end", "2000-02-29");
	EXPECT_EVAL("t.floor", "2000.08");
	EXPECT_EVAL("(double) t", "2000.13");
	EXPECT_EVAL("t.ceil", "2000.17");
	EXPECT_EVAL("t + 11", "2001-01");
	EXPECT_EVAL("t - 2", "1999-12");
	EXPECT_EVAL("TimeIndex b = \"2000-5\"", "2000-05");
	EXPECT_EVAL("b - t", "3");
	EXPECT_EVAL("t < b", "true");
	EXPECT_EVAL("t <= b", "true");
	EXPECT_EVAL("t != b", "true");
	EXPECT_EVAL("t == b", "false");
	EXPECT_EVAL("t >= b", "false");
	EXPECT_EVAL("t > b", "false");

	EXPECT_EVAL("t = NULL", "#0");
	CLEANUP_EVAL;
}

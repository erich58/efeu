#include <EFEU/UnitTest.h>
#include <Math/TimeSeries.h>
#include "utest/eval.tst.h"

TEST_DECL(setup)
{
	SetupStd();
	CmdSetup_TimeIndex();
	CmdSetup_TimeSeries();
	EXPECT_EVAL("TimeSeries::fmt = \"%#.2f\"", "\"%#.2f\"");
	EXPECT_STR(TimeSeries_fmt, "%#.2f");
}

TEST_SUITE
{
	TEST_RUN(setup);
	EXPECT_EVAL("TimeSeries(\"test\", \"2000\", 10., 20.)",
		"TimeSeries(\"test\", \"2000\",\n"
		"\t10.00, 20.00\n"
		")\n"
	);
	CLEANUP_EVAL;
}

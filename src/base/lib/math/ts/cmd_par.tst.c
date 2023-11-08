#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <EFEU/efutil.h>
#include <Math/TimeSeries.h>
#include "utest/eval.tst.h"

extern int PrintFloatPrec;

TEST_DECL(setup)
{
	SetupStd();
	SetupUtil();
	SetupTimeSeries();
	TEST_EXPR(PrintFloatPrec = 2);
	EXPECT_EVAL("fmt_float = \"%#*.*f\"", "\"%#*.*f\"");
	EXPECT_EVAL("TimeSeries::fmt = \"%#.2f\"", "\"%#.2f\"");
}

TEST_DECL(par)
{
	EXPECT_EVAL("OLSPar par", "NULL");
	EXPECT_EVAL("par.nr", "0");
	EXPECT_EVAL("par.df", "0");
	EXPECT_EVAL("par.r2", "0.00");
	EXPECT_EVAL("par.rbar2", "0.00");
	EXPECT_EVAL("par.dw", "0.00");
	EXPECT_EVAL("par.see", "0.00");
	EXPECT_EVAL("par.base", "#0");
	EXPECT_EVAL("par.first", "#0");
	EXPECT_EVAL("par.last", "#0");
	EXPECT_EVAL("par.koef", "{}");
	EXPECT_EVAL("par.exogen", "{}");
	EXPECT_EVAL("par.res", "NULL");
}

TEST_DECL(coef)
{
	PRINT_EVAL("OLSCoeff coef");
	EXPECT_EVAL("coef.name", "NULL");
	EXPECT_EVAL("coef.val", "0.00");
	EXPECT_EVAL("coef.se", "0.00");
	EXPECT_EVAL("coef.t", "NaN");
	PRINT_EVAL("coef = OLSCoeff();");
}

TEST_DECL(ols)
{
	EXPECT_EVAL("TimeSeries c = TimeSeries(\"c\", \"#1\", 6, 1.)",
		"TimeSeries(\"c\", \"#1\",\n"
		"\t1.00, 1.00, 1.00, 1.00, 1.00, 1.00\n"
		")\n"
	);
	EXPECT_EVAL("TimeSeries x = TimeSeries(\"x\", \"#1\", 0 : 5)",
		"TimeSeries(\"x\", \"#1\",\n"
		"\t0.00, 1.00, 2.00, 3.00, 4.00, 5.00\n"
		")\n"
	);
	EXPECT_EVAL("TimeSeries y = TimeSeries(\"y\", \"#1\", 10 : 20 : 2)",
		"TimeSeries(\"y\", \"#1\",\n"
		"\t10.00, 12.00, 14.00, 16.00, 18.00, 20.00\n"
		")\n"
	);
	PRINT_EVAL("OLSPar par = OLS(y, c, x)");
	EXPECT_EVAL("par.ident", "\"2\"");
	EXPECT_EVAL("par.nr", "6");
	EXPECT_EVAL("par.df", "4");
	EXPECT_EVAL("par.r2", "1.00");
	EXPECT_EVAL("par.rbar2", "1.00");
	EXPECT_EVAL("par.dw", "2.00");
	EXPECT_EVAL("par.see", "0.00");
	EXPECT_EVAL("par.base", "#1");
	EXPECT_EVAL("par.first", "#1");
	EXPECT_EVAL("par.last", "#6");
	PRINT_EVAL("par.koef[0]");
	EXPECT_EVAL("par.koef[0].name", "\"c\"");
	EXPECT_EVAL("par.koef[0].val", "10.00");
	EXPECT_EVAL("par.koef[0].se", "0.00");
	EXPECT_EVAL("par.koef[0].t", "Inf");
	PRINT_EVAL("par.koef[1]");
	EXPECT_EVAL("par.koef[1].name", "\"x\"");
	EXPECT_EVAL("par.koef[1].val", "2.00");
	EXPECT_EVAL("par.koef[1].se", "0.00");
	EXPECT_EVAL("par.koef[1].t", "Inf");
	EXPECT_EVAL("par.exogen",
		"{TimeSeries(\"c\", \"#1\",\n"
		"\t1.00, 1.00, 1.00, 1.00, 1.00, 1.00\n"
		")\n"
		", TimeSeries(\"x\", \"#1\",\n"
		"\t0.00, 1.00, 2.00, 3.00, 4.00, 5.00\n"
		")\n}"
	);
	EXPECT_EVAL("par.res", 
		"TimeSeries(\"y\", \"#1\",\n"
		"\t0.00, 0.00, 0.00, 0.00, 0.00, 0.00\n"
		")\n"
	);

	EXPECT_EVAL("free(par)", ""); 
}

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(par);
	TEST_RUN(coef);
	TEST_RUN(ols);
	CLEANUP_EVAL;
}

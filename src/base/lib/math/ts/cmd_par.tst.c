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
	SetProgName("ID");
	SetApplPath(UnitTest_tmp(NULL));
	TEST_FILE("TimeSeries.msg",
		"31	$!: OLS: missing exogene varaiable.\n"
		"32	$!: OLS: time index not compatible.\n"
		"33	$!: OLS: to less degrees of freedom.\n"
		"34	$!: OLS: singulary estimation.\n"
	);
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
	EXPECT_EVAL("par.proj()",
		"TimeSeries(NULL, \"#1\",\n"
		"\t10.00, 12.00, 14.00, 16.00, 18.00, 20.00\n"
		")\n"
	);
	EXPECT_EVAL("par.proj(TimeIndex(\"#3\"))",
		"TimeSeries(NULL, \"#3\",\n"
		"\t14.00, 16.00, 18.00, 20.00\n"
		")\n"
	);
	EXPECT_EVAL("par.proj(TimeIndex(\"#3\"), 2)",
		"TimeSeries(NULL, \"#3\",\n"
		"\t14.00, 16.00\n"
		")\n"
	);
	EXPECT_EVAL("par.proj(TimeIndex(\"#2\"), TimeIndex(\"#4\"))",
		"TimeSeries(NULL, \"#2\",\n"
		"\t12.00, 14.00, 16.00\n"
		")\n"
	);

	EXPECT_EVAL("TimeSeries c2 = TimeSeries(\"c\", \"#2\", 3, 1.)",
		"TimeSeries(\"c\", \"#2\",\n"
		"\t1.00, 1.00, 1.00\n"
		")\n"
	);
	PRINT_EVAL("OLSPar par = OLS(y, c2);");
	EXPECT_EVAL("par.res", 
		"TimeSeries(\"y\", \"#2\",\n"
		"\t-2.00, 0.00, 2.00\n"
		")\n"
	);

	EXPECT_EVAL("TimeSeries c3 = TimeSeries(\"c\", \"#10\", 3, 1.)",
		"TimeSeries(\"c\", \"#10\",\n"
		"\t1.00, 1.00, 1.00\n"
		")\n"
	);
	UnitTest_stderr("stderr");
	EXPECT_EVAL("OLSPar par = OLS(y, c3)", "NULL");
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"MSG ID: OLS: to less degrees of freedom.\n");
	UnitTest_stderr("stderr");
	EXPECT_EVAL("OLSPar par = OLS(y, 1)", "NULL");
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"MSG ID: OLS: to less degrees of freedom.\n");

	EXPECT_EVAL("y[1] += 1.", "13.00");
	EXPECT_EVAL("y[4] -= 1.", "17.00");

	PRINT_EVAL("OLSPar par = OLS(y, c, x)");
	EXPECT_EVAL("par.ident", "\"2\"");
	EXPECT_EVAL("par.nr", "6");
	EXPECT_EVAL("par.df", "4");
	EXPECT_EVAL("par.r2", "0.98");
	EXPECT_EVAL("par.rbar2", "0.97");
	EXPECT_EVAL("par.dw", "2.79");
	EXPECT_EVAL("par.see", "0.61");
	EXPECT_EVAL("par.base", "#1");
	EXPECT_EVAL("par.first", "#1");
	EXPECT_EVAL("par.last", "#6");
	PRINT_EVAL("par.koef[0]");
	EXPECT_EVAL("par.koef[0].name", "\"c\"");
	EXPECT_EVAL("par.koef[0].val", "10.43");
	EXPECT_EVAL("par.koef[0].se", "0.44");
	EXPECT_EVAL("par.koef[0].t", "23.64");
	PRINT_EVAL("par.koef[1]");
	EXPECT_EVAL("par.koef[1].name", "\"x\"");
	EXPECT_EVAL("par.koef[1].val", "1.83");
	EXPECT_EVAL("par.koef[1].se", "0.15");
	EXPECT_EVAL("par.koef[1].t", "12.55");
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
		"\t-0.43, 0.74, -0.09, 0.09, -0.74, 0.43\n"
		")\n"
	);

	UnitTest_stderr("stderr");
	EXPECT_EVAL("par = OLS(y)", "NULL");
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"MSG ID: OLS: missing exogene varaiable.\n");

	UnitTest_stderr("stderr");
	EXPECT_EVAL("par = OLS(y, x, x)", "NULL");
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"MSG ID: OLS: singulary estimation.\n");

	EXPECT_EVAL("par.proj()", "NULL");

}

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(par);
	TEST_RUN(coef);
	TEST_RUN(ols);
	CLEANUP_EVAL;
}

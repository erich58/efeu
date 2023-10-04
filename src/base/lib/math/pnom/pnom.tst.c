#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <Math/pnom.h>
#include "utest/eval.tst.h"

extern int PrintFloatPrec;

TEST_DECL(setup)
{
	SetupStd();
	SetupMath();
	SetupPnom();
	TEST_EXPR(PrintFloatPrec = 2);
	EXPECT_EVAL("fmt_float = \"%#*.*f\"", "\"%#*.*f\"");
	EXPECT_EVAL("Polynom p", "NULL");
	EXPECT_EVAL("p.print(cout, \"%.2f\");", "0 0\n");
}

TEST_DECL(zero)
{
	EXPECT_EVAL("p = Polynom()", "Polynom: dim=1 deg=0");
	EXPECT_EVAL("p(0.)", "0.00");
	EXPECT_EVAL("p(1.)", "0.00");
}

TEST_DECL(constant)
{
	EXPECT_EVAL("p = Polynom(1.)", "Polynom: dim=1 deg=0");
	EXPECT_EVAL("p(0.)", "1.00");
	EXPECT_EVAL("p(1.)", "1.00");
}

TEST_DECL(quadratic)
{
	EXPECT_EVAL("p = Polynom(0., 0., 1.)", "Polynom: dim=1 deg=2");
	EXPECT_EVAL("p.print(cout, \"%.2f\");",
		"1 2\n"
		"0.00 0.00 0.00 1.00\n"
	);
	EXPECT_EVAL("p(0.)", "0.00");
	EXPECT_EVAL("p(1.)", "1.00");
	EXPECT_EVAL("p(2.)", "4.00");
	EXPECT_EVAL("p(0., 1)", "0.00");
	EXPECT_EVAL("p(1., 1)", "2.00");
	EXPECT_EVAL("p(2., 1)", "4.00");
	EXPECT_EVAL("p(0., 2)", "2.00");
	EXPECT_EVAL("p(1., 2)", "2.00");
	EXPECT_EVAL("p(2., 2)", "2.00");
	EXPECT_EVAL("p(0., 3)", "0.00");
	EXPECT_EVAL("p(1., 3)", "0.00");
	EXPECT_EVAL("p(2., 3)", "0.00");
}

TEST_DECL(save_load)
{
	PRINT_EVAL(UnitTest_tpl("str tmp = \"@/pn\""));
	PRINT_EVAL("Polynom q = Polynom(1., 2.)");
	EXPECT_EVAL("q.print(cout, \"%.2f\");",
		"1 1\n"
		"0.00 1.00 2.00\n"
	);
	PRINT_EVAL("q.save(tmp)");
	PRINT_EVAL("p = pnload(tmp)");
	EXPECT_EVAL("p.print(cout, \"%.2f\");",
		"1 1\n"
		"0.00 1.00 2.00\n"
	);

	pnsave(NULL, UnitTest_tmp("zero"));
	EXPECT(pnload(UnitTest_tmp("zero")) == NULL);
}

TEST_DECL(linint)
{
	EXPECT_EVAL("double lx[] = { 0., 1., 2.};", "");
	EXPECT_EVAL("double ly[] = { 1., 2., 0.};", "");
	EXPECT_EVAL("p = linint(lx, ly)", "Polynom: dim=2 deg=1");
	EXPECT_EVAL("p.print(cout, \"%.2f\");",
		"2 1\n"
		"0.00 1.00 1.00\n"
		"1.00 2.00 -2.00\n"
	);
	EXPECT_EVAL("p(-1.)", "0.00");
	EXPECT_EVAL("p(0.)", "1.00");
	EXPECT_EVAL("p(0.5)", "1.50");
	EXPECT_EVAL("p(1.)", "2.00");
	EXPECT_EVAL("p(1.5)", "1.00");
	EXPECT_EVAL("p(2.)", "0.00");
	EXPECT_EVAL("p(2.5)", "-1.00");
	EXPECT_EVAL("p(3.)", "-2.00");

	EXPECT(linint(0, NULL, NULL) == NULL);
	double x = 1.;
	double y = 2.;
	Polynom *pn = linint(1, &x, &y);
	pnprint(OUT(), pn, "%.2f");
	EXPECT_OUT(
		"1 0\n"
		"1.00 2.00\n"
	);
}

TEST_DECL(spline)
{
	EXPECT_EVAL("double sx[] = { 0., 1., 2. };", "");
	EXPECT_EVAL("double sy[] = { 0., 1., 0. };", "");
	EXPECT_EVAL("p = spline(sx, sy)", "Polynom: dim=2 deg=3");
	EXPECT_EVAL("p.print(cout, \"%.2f\");",
		"2 3\n"
		"0.00 0.00 1.50 0.00 -0.50\n"
		"1.00 1.00 0.00 -1.50 0.50\n"
	);
	EXPECT_EVAL("p = dspline(sx, sy, 0., 0.)", "Polynom: dim=2 deg=3");
	EXPECT_EVAL("p.print(cout, \"%.2f\");",
		"2 3\n"
		"0.00 0.00 0.00 3.00 -2.00\n"
		"1.00 1.00 0.00 -3.00 2.00\n"
	);
}

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(zero);
	TEST_RUN(constant);
	TEST_RUN(quadratic);
	TEST_RUN(save_load);
	TEST_RUN(linint);
	TEST_RUN(spline);
	CLEANUP_EVAL;
}

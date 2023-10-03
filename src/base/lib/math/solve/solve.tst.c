#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <Math/solve.h>

TEST_DECL(par)
{
	EXPECT(SolveFlag == 0);
	EXPECT_INT(Obj2int(strterm("SolvePar::debug")), 0);

	EXPECT_DBL(SolveEps, 0.000001);
	EXPECT_DBL(Obj2double(strterm("SolvePar::epsilon")), 0.000001);

	EXPECT_INT(SolveStep, 25);
	EXPECT_INT(Obj2int(strterm("SolvePar::maxstep")), 25);

	EXPECT_STR(SolveFmt, "%12G");
	EXPECT_STR(Obj2str(strterm("SolvePar::fmt")), "%12G");
}

TEST_DECL(solve)
{
	TEST_EXPR(streval("double f(double x) return x;"));
	EXPECT_DBL(Obj2double(strterm("regfalsi(f, -2., 2.)")), 0.);
	EXPECT_DBL(Obj2double(strterm("bisection(sgn, -2., 2.)")), 0.);
}

TEST_SUITE
{
	SetupStd();
	SetupMath();
	SetupSolve();
	TEST_RUN(par);
	TEST_RUN(solve);
}

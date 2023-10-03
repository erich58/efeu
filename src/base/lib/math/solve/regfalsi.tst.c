#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <EFEU/math.h>
#include <Math/solve.h>

static double f(void *par, double x)
{
	double x0 = *((double *)par);
	return x * x - x0;
}

TEST_DECL(setup)
{
	SetupStd();
	SetupSolve();
	SetProgName("ID");
	SetApplPath(UnitTest_tmp(NULL));
	TEST_FILE("solve.msg",
		"2	$!: no radical: $1, $2\n"
		"3	$!: termination: step $1\n"
		"5	step $1: $2, $3 (weight: $4)\n"
	);
}

TEST_SUITE
{
	TEST_RUN(setup);

	double x0 = 2.;
	SolveFmt = "%6.3f";
	UnitTest_stderr("stderr");
	EXPECT(isnan(regfalsi(f, &x0, 0., 1.)));
//	PRINT_FMT("%g", regfalsi(f, &x0, 0., 1.));
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"MSG ID: no radical:  0.000,  1.000\n"
	);

	TEST_EXPR(SolveStep = 3);
	TEST_EXPR(SolveFlag = 0);
	UnitTest_stderr("stderr");
	EXPECT_DBL(regfalsi(f, &x0, 1., 2.), 1.409242);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"MSG ID: termination: step 3\n"
	);

	TEST_EXPR(SolveFlag = 1);
	UnitTest_stderr("stderr");
	EXPECT_DBL(regfalsi(f, &x0, 1., 2.), 1.409242);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"MSG step 0:  1.000,  2.000 (weight: 1.00000)\n"
		"MSG step 1:  1.333,  2.000 (weight: 0.500000)\n"
		"MSG step 2:  1.333,  1.533 (weight: 0.750000)\n"
		"MSG step 3:  1.333,  1.416 (weight: 0.875000)\n"
		"MSG ID: termination: step 3\n"
	);

	TEST_EXPR(SolveStep = 25);
	TEST_EXPR(SolveEps = 0.000001);
	TEST_EXPR(SolveFlag = 0);
	UnitTest_stderr("stderr");
	EXPECT_DBL(regfalsi(f, &x0, 1., 2.), sqrt(2.));
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr", "");
}

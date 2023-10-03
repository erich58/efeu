#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <EFEU/math.h>
#include <Math/solve.h>

static int f(void *par, double x)
{
	double x0 = *((double *)par);

	if	(x > x0)	return 1;
	else if	(x < x0) 	return -1;
	else			return 0;
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
		"4	step $1: $2, $3\n"
	);
}

TEST_SUITE
{
	TEST_RUN(setup);

	double x0 = 0.3;
	SolveFmt = "%6.3f";
	UnitTest_stderr("stderr");
	EXPECT(isnan(bisection(f, &x0, 1., 2.)));
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"MSG ID: no radical:  1.000,  2.000\n"
	);

	TEST_EXPR(SolveStep = 3);
	TEST_EXPR(SolveFlag = 0);
	UnitTest_stderr("stderr");
	EXPECT_DBL(bisection(f, &x0, 0., 1.), 0.375);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"MSG ID: termination: step 3\n"
	);

	TEST_EXPR(SolveFlag = 1);
	UnitTest_stderr("stderr");
	EXPECT_DBL(bisection(f, &x0, 0., 1.), 0.375);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"MSG step 0:  0.000,  1.000\n"
		"MSG step 1:  0.000,  0.500\n"
		"MSG step 2:  0.250,  0.500\n"
		"MSG ID: termination: step 3\n"
	);

	TEST_EXPR(SolveStep = 25);
	TEST_EXPR(SolveEps = 0.1);
	UnitTest_stderr("stderr");
	EXPECT_DBL(bisection(f, &x0, 0., 1.), 0.28125);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"MSG step 0:  0.000,  1.000\n"
		"MSG step 1:  0.000,  0.500\n"
		"MSG step 2:  0.250,  0.500\n"
		"MSG step 3:  0.250,  0.375\n"
		"MSG step 4:  0.250,  0.313\n"
	);
}

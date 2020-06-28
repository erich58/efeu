#include <EFEU/UnitTest.h>
#include <EFEU/efutil.h>

#define	PRINT_VEC(x) \
	UnitTest_info(__FILE__, __LINE__, "INFO", "%s", info(x))

#define	EXPECT_VEC(x, a, b, c) \
	if	(!EXPECT(cmp(x, a, b, c))) \
		UnitTest_info(__FILE__, __LINE__, "BECAUSE", "%s", info(x))

static void set(double *x, double a, double b, double c)
{
	x[0] = a;
	x[1] = b;
	x[2] = c;
}

static char *info(double *x)
{
	static char buf[1024];
	snprintf(buf, sizeof buf, "x = { %.2f, %.2f, %.2f }",
		x[0], x[1], x[2]);
	return buf;
}

static int cmp(double *x, double a, double b, double c)
{
	int err = 0;

	if	(!UnitTest_dcmp(x[0], a))
		err++;

	if	(!UnitTest_dcmp(x[1], b))
		err++;

	if	(!UnitTest_dcmp(x[2], c))
		err++;

	return err ? 0 : 1;
}

TEST_SUITE
{
	double x[3] = { 1., -1., 1. };
	roundvec(x, 3, 1.);
	EXPECT_VEC(x, 1., -1., 1.);

	set(x, 0.8, -1.2, 0.5);
	roundvec(x, 3, 1.);
	EXPECT_VEC(x, 1., -1., 1.);

	set(x, 1.32, 1.33, 1.34);
	roundvec(x, 3, 4.);
	EXPECT_VEC(x, 1., 1., 2.);

	set(x, 0.9, 0.85, 0.95);
	roundvec(x, 3, 3.);
	EXPECT_VEC(x, 1., 1., 1.);

	set(x, -0.9, -0.85, -0.95);
	roundvec(x, 3, -3.);
	EXPECT_VEC(x, -1., -1., -1.);
}

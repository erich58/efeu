#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <Math/pnom.h>
#include "utest/pnom.tst.h"

TEST_DECL(spline)
{
	double x[3] = { 0., 1., 2. };
	double y[3] = { 0., 1., 0. };

	EXPECT(spline(0, NULL, NULL) == NULL);

	EXPECT_PNOM(spline(1, x, y),
		"1 0\n"
		"0.00 0.00\n"
	);

	EXPECT_PNOM(spline(2, x, y),
		"1 3\n"
		"0.00 0.00 1.00 0.00 0.00\n"
	);

	EXPECT_PNOM(spline(3, x, y),
		"2 3\n"
		"0.00 0.00 1.50 0.00 -0.50\n"
		"1.00 1.00 0.00 -1.50 0.50\n"
	);
}

TEST_DECL(dspline)
{
	double x[3] = { 0., 1., 2. };
	double y[3] = { 0., 1., 0. };

	EXPECT(dspline(0, NULL, NULL, 0., 0.) == NULL);

	EXPECT_PNOM(dspline(1, x, y, 1., 2.),
		"1 1\n"
		"0.00 0.00 1.50\n"
	);

	EXPECT_PNOM(dspline(2, x, y, 0., 0.),
		"1 3\n"
		"0.00 0.00 0.00 3.00 -2.00\n"
	);

	EXPECT_PNOM(dspline(3, x, y, 0., 0.),
		"2 3\n"
		"0.00 0.00 0.00 3.00 -2.00\n"
		"1.00 1.00 0.00 -3.00 2.00\n"
	);
}

TEST_DECL(cspline)
{
	double x[4] = { 0., 1., 2., 3. };
	double y[4] = { 0., 1., 0., 0. };

	EXPECT(cspline(0, x, y) == NULL);

	EXPECT_PNOM(cspline(1, x, y),
		"1 0\n"
		"0.00 0.00\n"
	);

	EXPECT_PNOM(cspline(2, x, y),
		"1 3\n"
		"0.00 0.00 1.00 0.00 0.00\n"
	);

	EXPECT_PNOM(cspline(3, x, y),
		"2 3\n"
		"0.00 0.00 0.00 3.00 -2.00\n"
		"1.00 1.00 0.00 -3.00 2.00\n"
	);

	EXPECT_PNOM(cspline(4, x, y),
		"3 3\n"
		"0.00 0.00 1.00 1.00 -1.00\n"
		"1.00 1.00 0.00 -2.00 1.00\n"
		"2.00 0.00 -1.00 1.00 0.00\n"
	);
}

TEST_SUITE
{
	TEST_RUN(spline);
	TEST_RUN(dspline);
	TEST_RUN(cspline);
	CLEANUP_PNOM;
}

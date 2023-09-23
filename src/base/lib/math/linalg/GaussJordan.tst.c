#include <EFEU/UnitTest.h>
#include <Math/linalg.h>

TEST_SUITE
{
	EXPECT(GaussJordan(NULL, 0) == 0);
	double x = 2.;
	EXPECT(GaussJordan(&x, 1) == 1);
	EXPECT_DBL(x, 0.5);

	double null2[4] = { 0., 0., 0., 0. };
	EXPECT(GaussJordan(null2, 2) == 0);
	double one2[4] = { 1., 1., 1., 1. };
	EXPECT(GaussJordan(one2, 2) == 1);
	double unit2[4] = { 1., 0., 0., 1. };
	EXPECT(GaussJordan(unit2, 2) == 2);
	double data[4] = { 1., 2., 2., -1. };
	EXPECT(GaussJordan(data, 2) == 2);
	EXPECT_DBL(data[0], 0.2);
	EXPECT_DBL(data[1], 0.4);
	EXPECT_DBL(data[2], 0.4);
	EXPECT_DBL(data[3], -0.2);
	EXPECT(GaussJordan(data, 2) == 2);
	EXPECT_DBL(data[0], 1.);
	EXPECT_DBL(data[1], 2.);
	EXPECT_DBL(data[2], 2.);
	EXPECT_DBL(data[3], -1);
}

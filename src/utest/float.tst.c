#include <EFEU/UnitTest.h>

TEST_SUITE
{
	EXPECT(UnitTest_dcmp(1.000001, 1.) == 0);
	EXPECT(UnitTest_dcmp(1.0000001, 1.) == 1);
	EXPECT(UnitTest_dcmp(0.999999, 1.) == 0);
	EXPECT(UnitTest_dcmp(0.9999999, 1.) == 1);
	EXPECT(UnitTest_dcmp(0., 0.) == 1);
	EXPECT(UnitTest_dcmp(1.e-32, 0.) == 0);
	EXPECT(UnitTest_dcmp(-1.e-32, 0.) == 0);

	EXPECT_DBL(1.0000001, 1.);
	EXPECT_DBL(0.9999999, 1.);

	TEST_FORK
	{
		double d;
		TEST_EXPR(d = 1./3.);
		EXPECT_DBL(d, 0.33333);
		EXPECT_DBL(d, 0.3333333);
		TEST_EXIT;
	}

	CHILD_EXIT(1);
	
	EXPECT_RANGE("%.2f", 1 / 2., 0, 1);

	TEST_FORK
	{
		EXPECT_RANGE("%.2f", -1, 0, 1);
		TEST_EXIT;
	}

	CHILD_EXIT(1);

	TEST_FORK
	{
		EXPECT_RANGE("%.2f", 2, 0, 1);
		TEST_EXIT;
	}

	CHILD_EXIT(1);
}

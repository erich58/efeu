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
}

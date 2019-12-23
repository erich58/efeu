#include <EFEU/UnitTest.h>
#include <EFEU/math.h>

UNIT_TEST
{
	EXPECT_TRUE(!isnan(0.));
	EXPECT_TRUE(!isinf(0.));

	EXPECT_TRUE(!isnan(-1./0.));
	EXPECT_TRUE(isnan(0./0.));
	EXPECT_TRUE(!isnan(1./0.));

	EXPECT_TRUE(isinf(-1./0.));
	EXPECT_TRUE(!isinf(0./0.));
	EXPECT_TRUE(isinf(1./0.));

	EXPECT_TRUE(isnan(ExceptionValue(0.)));
	EXPECT_TRUE(isinf(ExceptionValue(1.)));
	EXPECT_TRUE(isinf(ExceptionValue(-1.)));

	EXPECT_TRUE(ExceptionValue(-1.) < 0.);
	EXPECT_TRUE(ExceptionValue(1.) > 0.);
	EXPECT_TRUE(ExceptionValue(0.) != ExceptionValue(0.));
}

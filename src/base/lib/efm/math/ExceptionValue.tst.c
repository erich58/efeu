#include <EFEU/UnitTest.h>
#include <EFEU/math.h>

TEST_SUITE
{
	EXPECT(!isnan(0.));
	EXPECT(!isinf(0.));

	EXPECT(!isnan(-1./0.));
	EXPECT(isnan(0./0.));
	EXPECT(!isnan(1./0.));

	EXPECT(isinf(-1./0.));
	EXPECT(!isinf(0./0.));
	EXPECT(isinf(1./0.));

	EXPECT(isnan(ExceptionValue(0.)));
	EXPECT(isinf(ExceptionValue(1.)));
	EXPECT(isinf(ExceptionValue(-1.)));

	EXPECT(ExceptionValue(-1.) < 0.);
	EXPECT(ExceptionValue(1.) > 0.);
	EXPECT(ExceptionValue(0.) != ExceptionValue(0.));
}

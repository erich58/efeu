#include <EFEU/UnitTest.h>
#include <Math/func.h>

TEST_DECL(dlognormal)
{
	EXPECT(isnan(dlognormal(0., 0., 0.)));
	EXPECT_DBL(dlognormal(1., 0., 1.), 0.3989422804014327);
}

TEST_DECL(plognormal)
{
	EXPECT(isnan(plognormal(0., 0., 0.)));
	EXPECT_DBL(plognormal(0., 0., 1.), 0.0);
	EXPECT_DBL(plognormal(1., 0., 1.), 0.5);
}

TEST_DECL(qlognormal)
{
	EXPECT_DBL(qlognormal(0., 0., 0.), 0.);
	EXPECT_DBL(qlognormal(0.5, 0., 0.), 1.);
	EXPECT(isinf(qlognormal(1., 0., 1.)));
}

TEST_SUITE
{
	TEST_RUN(dlognormal);
	TEST_RUN(plognormal);
	TEST_RUN(qlognormal);
}


#include <EFEU/UnitTest.h>
#include <Math/func.h>

TEST_DECL(dnormal)
{
	EXPECT(isnan(dnormal(0., 0., 0.)));
	EXPECT_DBL(dnormal(0., 0., 1.), 0.3989422804014327);
}

TEST_DECL(pnormal)
{
	EXPECT(isnan(pnormal(0., 0., 0.)));
	EXPECT_DBL(pnormal(0., 0., 1.), 0.5);
	EXPECT_DBL(pnormal(-1.644853626951473, 0., 1.), 0.05);
	EXPECT_DBL(pnormal(1.644853626951473, 0., 1.), 0.95);
}

TEST_DECL(qnormal)
{
	EXPECT_DBL(qnormal(0.1, 0., 0.), 0.);
	EXPECT_DBL(qnormal(0.5, 0., 0.), 0.);
	EXPECT_DBL(qnormal(0.5, 2., 0.), 2.);
	EXPECT_DBL(qnormal(0.9, 2., 0.), 2.);
	EXPECT(isinf(qnormal(0., 0., 1.)));
	EXPECT_DBL(qnormal(0.05, 0., 1.), -1.644853626951473);
	EXPECT_DBL(qnormal(0.5, 0., 1.), 0.);
	EXPECT_DBL(qnormal(0.5, 2., 1.), 2.);
	EXPECT_DBL(qnormal(0.95, 0., 1.), 1.644853626951473);
	EXPECT_DBL(qnormal(0.95, 0., 2.), 2. * 1.644853626951473);
	EXPECT(isinf(qnormal(1., 0., 1.)));
}

TEST_SUITE
{
	TEST_RUN(dnormal);
	TEST_RUN(pnormal);
	TEST_RUN(qnormal);
}


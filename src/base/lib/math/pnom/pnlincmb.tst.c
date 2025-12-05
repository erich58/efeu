#include <EFEU/UnitTest.h>
#include <Math/pnom.h>
#include <Math/utest/pnom.tst.h>

TEST_DECL(step)
{
	TEST_PNOM(p1,
		"3 0\n"
		"0 0\n" 
		"2 2\n" 
		"4 4\n" 
	);

	EXPECT_DBL(pneval(p1, 0, -1.), 0.);
	EXPECT_DBL(pneval(p1, 0, 1.), 0.);
	EXPECT_DBL(pneval(p1, 0, 3.), 2.);
	EXPECT_DBL(pneval(p1, 0, 5.), 4.);

	TEST_PNOM(p2,
		"3 0\n"
		"1 1\n" 
		"3 3\n" 
		"5 5\n" 
	);

	EXPECT_DBL(pneval(p2, 0, 0.), 1.);
	EXPECT_DBL(pneval(p2, 0, 2.), 1.);
	EXPECT_DBL(pneval(p2, 0, 4.), 3.);
	EXPECT_DBL(pneval(p2, 0, 6.), 5.);

	EXPECT_PNOM(pnlincmb(p1, 1., p2, -2., NULL),
		"5 0\n"
		"0.00 -2.00\n"
		"2.00 0.00\n"
		"3.00 -4.00\n"
		"4.00 -2.00\n"
		"5.00 -6.00\n"
	);
}

TEST_DECL(mixed)
{
	TEST_PNOM(p1,
		"3 2\n"
		"0 0 2 0\n" 
		"1 1 0 0\n" 
		"2 0 0 3\n" 
	);

	TEST_PNOM(p2,
		"2 1\n"
		"1.5 0 -1\n" 
		"2.5 -2 -2\n" 
	);

	EXPECT_PNOM(pnlincmb(p1, 1., p2, 1., NULL),
		"4 2\n"
		"0.00 1.50 1.00 0.00\n"
		"1.00 1.50 -1.00 0.00\n"
		"2.00 -0.50 -1.00 3.00\n"
		"2.50 -1.25 1.00 3.00\n"
	);

	EXPECT_PNOM(pnlincmb(p1, 2., NULL),
		"3 2\n"
		"0.00 0.00 4.00 0.00\n" 
		"1.00 2.00 0.00 0.00\n" 
		"2.00 0.00 0.00 6.00\n" 
	);

	EXPECT_PNOM(pnlincmb(p2, 2., NULL),
		"2 1\n"
		"1.50 0.00 -2.00\n" 
		"2.50 -4.00 -4.00\n" 
	);
}

TEST_SUITE
{
	TEST_RUN(mixed);
	TEST_RUN(step);
	CLEANUP_PNOM;
}



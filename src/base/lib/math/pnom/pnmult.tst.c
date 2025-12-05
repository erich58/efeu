#include <EFEU/UnitTest.h>
#include <Math/pnom.h>
#include <Math/utest/pnom.tst.h>

TEST_SUITE
{
	TEST_PNOM(p1,
		"1 1\n"
		"0 1 1\n" 
	);

	TEST_PNOM(p2,
		"2 2\n"
		"0 0 0 1\n" 
		"1 1 0 0\n" 
	);

	EXPECT_PNOM(pnmult(2., p1, p1),
		"1 2\n"
		"0.00 2.00 4.00 2.00\n" 
	);

	EXPECT_PNOM(pnmult(1., p2, p2),
		"2 4\n"
		"0.00 0.00 0.00 0.00 0.00 1.00\n" 
		"1.00 1.00 0.00 0.00 0.00 0.00\n" 
	);

	EXPECT_PNOM(pnmult(1., p1, p2),
		"2 3\n"
		"0.00 0.00 0.00 1.00 1.00\n" 
		"1.00 2.00 1.00 0.00 0.00\n" 
	);

	EXPECT(pnmult(1., NULL, p1) == 0);
	EXPECT(pnmult(1., p1, NULL) == 0);
	EXPECT(pnmult(0., p1, p1) == 0);
	CLEANUP_PNOM;
}

#include <EFEU/UnitTest.h>
#include <Math/pnom.h>
#include "utest/pnom.tst.h"

TEST_SUITE
{
	TEST_PNOM(p,
		"3 2\n"
		"0 0 2 0\n" 
		"1 1 0 0\n" 
		"2 0 0 3\n" 
	);

	EXPECT_PNOM(pnint(p, 0., 2.),
		"3 3\n"
		"0.00 2.00 0.00 1.00 0.00\n" 
		"1.00 3.00 1.00 0.00 0.00\n" 
		"2.00 4.00 0.00 0.00 1.00\n" 
	);

	EXPECT_PNOM(pnint(NULL, 1., 2.),
		"1 0\n"
		"1.00 2.00\n" 
	);

	CLEANUP_PNOM;
}

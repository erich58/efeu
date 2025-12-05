#include <EFEU/UnitTest.h>
#include <Math/pnom.h>
#include <Math/utest/pnom.tst.h>

TEST_SUITE
{
	EXPECT_DBL(pnarea(NULL, 0., 1.), 0.);

	TEST_PNOM(p,
		"2 1\n"
		"0 0 2\n" 
		"1 1 0\n" 
	);

	EXPECT_DBL(pnarea(p, 0., 2.), 2.);
	CLEANUP_PNOM;
}

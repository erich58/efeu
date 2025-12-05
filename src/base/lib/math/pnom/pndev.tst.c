#include <Math/utest/pnom.tst.h>

TEST_SUITE
{
	TEST_PNOM(pn,
		"3 2\n"
		"0 0 1 0\n" 
		"1 1 0 0\n" 
		"2 0 0 1\n" 
	);

	EXPECT_PNOM(pndev(pn, 0),
		"3 2\n"
		"0.00 0.00 1.00 0.00\n"
		"1.00 1.00 0.00 0.00\n"
		"2.00 0.00 0.00 1.00\n"
	);

	EXPECT_PNOM(pndev(pn, 1),
		"3 1\n"
		"0.00 1.00 0.00\n"
		"1.00 0.00 0.00\n"
		"2.00 0.00 2.00\n"
	);

	EXPECT_PNOM(pndev(pn, 2),
		"3 0\n"
		"0.00 0.00\n"
		"1.00 0.00\n"
		"2.00 2.00\n"
	);

	EXPECT_PNOM(pndev(pn, 3), "0 0\n");
	EXPECT_PNOM(pndev(pn, -1), "0 0\n");
	EXPECT_PNOM(pndev(NULL, 0), "0 0\n");

	CLEANUP_OUT;
}

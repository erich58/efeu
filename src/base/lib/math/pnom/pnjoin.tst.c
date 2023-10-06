#include <EFEU/UnitTest.h>
#include <Math/pnom.h>
#include "utest/pnom.tst.h"

static void tst_op(double x, double **c, size_t *deg, size_t n)
{
	printf("x: %6.2f\n", x);

	for (size_t i = 0; i < n; ++i)
	{
		printf("%d: ", i);

		for (size_t j = 0; j <= deg[i]; ++j)
			printf("%6.2f", c[i][j]);

		printf("\n");
	}
}

TEST_SUITE
{
	TEST_PNOM(p1,
		"3 0\n"
		"0.5 0\n" 
		"1.0 1\n" 
		"1.5 2\n" 
	);

	TEST_PNOM(p2,
		"2 2\n"
		"0 0 0 1\n" 
		"1 1 1 0\n" 
	);

	Polynom *pn[3] = { NULL, p1, p2 };
	EXPECT_INT(pnjoin(pn, 0, NULL), 0);
	EXPECT_INT(pnjoin(pn, 1, NULL), 0);
	EXPECT_INT(pnjoin(pn, 2, NULL), 3);
	EXPECT_INT(pnjoin(pn, 3, NULL), 4);

	UnitTest_stdout("out");
	EXPECT_INT(pnjoin(pn, 3, tst_op), 4);
	UnitTest_stdout(NULL);
	EXPECT_CONTENT("out",
		"x:   0.00\n"
		"0:   0.00\n"
		"1:   0.00\n"
		"2:   0.00  0.00  1.00\n"
		"x:   0.50\n"
		"0:   0.00\n"
		"1:   0.00\n"
		"2:   0.25  1.00  1.00\n"
		"x:   1.00\n"
		"0:   0.00\n"
		"1:   1.00\n"
		"2:   1.00  1.00\n"
		"x:   1.50\n"
		"0:   0.00\n"
		"1:   2.00\n"
		"2:   1.50  1.00\n"
	);

	CLEANUP_PNOM;
}

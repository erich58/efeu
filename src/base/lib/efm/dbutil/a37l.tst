#include <EFEU/UnitTest.h>
#include <EFEU/dbutil.h>

TEST_SUITE
{
	int errcnt = 0;
	int n = 0;

	for (n = 0; n <= 37 * 37; n++)
		if (a37l(l37a(n)) != n)
			errcnt++;


	EXPECT_TRUE(errcnt == 0);
	EXPECT_TRUE(a37l("") == 0u);
	EXPECT_TRUE(a37l("0") == 1u);
	EXPECT_TRUE(a37l("9") == 10u);
	EXPECT_TRUE(a37l("A") == 11u);
	EXPECT_TRUE(a37l("Z") == 36u);
	EXPECT_TRUE(a37l("0 ") == 37u);
	EXPECT_TRUE(a37l("0  ") == 37u *37u);
	EXPECT_TRUE(a37l("0   ") == 37u * 37u *37u);
	EXPECT_TRUE(a37l("0    ") == 37u * 37u * 37u *37u);
	EXPECT_TRUE(a37l("0     ") == 37u * 37u * 37u *37u * 37u);
	EXPECT_TRUE(a37l("0      ") == 37u * 37u * 37u * 37u * 37u * 37u);

	EXPECT_STREQ(l37a(a37l("zzzzzz")), "ZZZZZZ");
	EXPECT_STRNE(l37a(a37l("1234567")), "1234567");
	EXPECT_STREQ(l37a(a37l("aBcDeF")), "ABCDEF");
	EXPECT_STREQ(l37a(a37l("a:b.c ")), "A B C ");
}

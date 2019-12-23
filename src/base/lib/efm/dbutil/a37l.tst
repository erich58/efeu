#include <EFEU/UnitTest.h>
#include <EFEU/dbutil.h>

UNIT_TEST
{
	EXPECT_TRUE(a37l("") == 0);
	EXPECT_TRUE(a37l("0") == 1);
	EXPECT_TRUE(a37l("Z") == 36);
	EXPECT_TRUE(a37l("0 ") == 37);
}

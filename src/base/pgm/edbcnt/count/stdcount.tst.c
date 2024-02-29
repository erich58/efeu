#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include <EFEU/mdcount.h>

TEST_SUITE
{
	int a = 0;
	EXPECT_STR(stdcount->type, "int");
	stdcount->add(stdcount, &a, NULL);
	EXPECT_INT(a, 1);
}

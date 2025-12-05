#include <EFEU/UnitTest.h>
#include "src2doc.h"
#include <EFEU/utest/out.tst.h>

TEST_SUITE
{
	SrcData data;
	SrcData_init(&data, NULL);
	SrcData_write(&data, OUT());
	EXPECT_OUT("\\Name\n -- \n");
	CLEANUP_OUT;
}

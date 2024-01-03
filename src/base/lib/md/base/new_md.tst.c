#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include <EFEU/mdmat.h>
#include "utest/mstr.tst.h"

TEST_SUITE
{
	mdmat *md = new_mdmat();
	EXPECT_PTR(md);
	EXPECT_MSTR(rd_ident(md), "name=NULL, type=., dim=0, size=0");
	CLEANUP_MSTR;
}

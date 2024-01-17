#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include <EFEU/mdmat.h>
#include "utest/mstr.tst.h"

TEST_SUITE
{
	mdmat *md;

	EXPECT(md_scalar(NULL) == NULL);

	EXPECT_PTR(md = new_mdmat());
	EXPECT_MSTR(rd_ident(md), "name=NULL, type=., dim=0, size=0");
	rd_deref(md);

	EXPECT_PTR(md = md_scalar(int2Obj(42)));
	EXPECT_MSTR(rd_ident(md), "name=NULL, type=int, dim=0, size=4");
	rd_deref(md);

	EXPECT_PTR(md = md_create(&Type_int, "title x=a,b,c"));
	EXPECT_MSTR(rd_ident(md), "name=\"title\", type=int, dim=1, size=12");
	rd_deref(md);

	CLEANUP_MSTR;
}

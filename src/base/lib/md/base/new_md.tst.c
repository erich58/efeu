#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include <EFEU/mdmat.h>
#include <EFEU/utest/mstr.tst.h>

TEST_SUITE
{
	mdmat *md;

	EXPECT(md_scalar(NULL) == NULL);

	EXPECT_PTR(md = new_mdmat());
	EXPECT_MSTR(rd_ident(md), ".\n");
	rd_deref(md);

	EXPECT_PTR(md = md_scalar(int2Obj(42)));
	EXPECT_MSTR(rd_ident(md), "int\n");
	rd_deref(md);

	EXPECT_PTR(md = md_create(&Type_int, "title x=a,b,c"));
	EXPECT_MSTR(rd_ident(md), "int[3] \"title\"\n");
	rd_deref(md);

	CLEANUP_MSTR;
}

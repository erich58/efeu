#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>

TEST_DECL(null)
{
	EXPECT_INT(cmp_axis(NULL, NULL, 0), 0);
	mdaxis *axis = new_axis(NULL, 0);
	EXPECT_INT(cmp_axis(axis, NULL, 0), MDXCMP_ALL);
	EXPECT_INT(cmp_axis(NULL, axis, 0), MDXCMP_ALL);
	del_axis(axis);
}

TEST_DECL(equal)
{
	mdmat *md = md_create(&Type_int, "a=a,b,c a=a,b,c");
	EXPECT_INT(cmp_axis(md->axis, md->axis->next, MDXCMP_ALL), 0);
	rd_deref(md);
}

TEST_DECL(name)
{
	mdmat *md = md_create(&Type_int, "a=a,b,c x=a,b,c");
	EXPECT_INT(cmp_axis(md->axis, md->axis->next, MDXCMP_ALL), MDXCMP_NAME);
	rd_deref(md);
}

TEST_DECL(dim)
{
	mdmat *md = md_create(&Type_int, "a=a,b,c a=a,b");
	EXPECT_INT(cmp_axis(md->axis, md->axis->next, MDXCMP_ALL), MDXCMP_DIM);
	rd_deref(md);
}

TEST_DECL(idx)
{
	mdmat *md = md_create(&Type_int, "a=a,b,c a=a,b,x");
	EXPECT_INT(cmp_axis(md->axis, md->axis->next, MDXCMP_ALL), MDXCMP_IDX);
	rd_deref(md);
}

TEST_DECL(all)
{
	mdmat *md = md_create(&Type_int, "a=a,b,c x=x,y");
	EXPECT_INT(cmp_axis(md->axis, md->axis->next, MDXCMP_ALL), MDXCMP_ALL);
	rd_deref(md);
}

TEST_SUITE
{
	SetupMdMat();
	TEST_RUN(null);
	TEST_RUN(equal);
	TEST_RUN(name);
	TEST_RUN(dim);
	TEST_RUN(idx);
	TEST_RUN(all);
}

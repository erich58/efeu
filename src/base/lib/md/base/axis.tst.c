#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>

TEST_DECL(null)
{
	EXPECT_UINT(md_dim(NULL), 0);
	EXPECT_UINT(md_size(NULL, 4), 4);
	del_axis(NULL);
}

TEST_DECL(zero_dim)
{
	mdaxis *axis;
	EXPECT_PTR(axis = new_axis(NULL, 0));
	EXPECT_UINT(md_dim(axis), 1);
	EXPECT_UINT(md_size(axis, 4), 0);
	del_axis(axis);
}

TEST_DECL(no_label)
{
	mdaxis *axis;
	EXPECT_PTR(axis = new_axis(NULL, 3));
	EXPECT_PTR(axis->next = new_axis(NULL, 2));
	EXPECT_UINT(md_dim(axis), 2);
	EXPECT_UINT(md_dim(axis->next), 1);
	EXPECT_UINT(md_size(axis, 4), 24);
	EXPECT_UINT(md_size(axis->next, 4), 8);
	del_axis(axis);
}

TEST_DECL(cpy_with_label)
{
	StrPool *a = NewStrPool();
//	StrPool *b = NewStrPool();

	mdaxis *axis;
	axis = new_axis(a, 2);
	del_axis(axis);
}

TEST_SUITE
{
	/*
	StrPool *pool = NewStrPool();
	ASSERT(pool != NULL);
	*/
	TEST_RUN(null);
	TEST_RUN(zero_dim);
	TEST_RUN(no_label);
	TEST_RUN(cpy_with_label);
}

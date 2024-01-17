#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>

TEST_DECL(null)
{
	EXPECT_UINT(md_dim(NULL), 0);
	del_axis(NULL);
}

TEST_DECL(zero_dim)
{
	mdaxis *axis;
	EXPECT_PTR(axis = new_axis(NULL, 0));
	EXPECT(axis->idx == NULL);
	EXPECT_UINT(md_dim(axis), 1);
	del_axis(axis);
}

TEST_DECL(no_label)
{
	mdaxis *axis;
	EXPECT_PTR(axis = new_axis(NULL, 3));
	EXPECT(axis->idx != NULL);
	EXPECT_PTR(axis->next = new_axis(NULL, 2));
	EXPECT_UINT(md_dim(axis), 2);
	EXPECT_UINT(md_dim(axis->next), 1);
	del_axis(axis);
}

TEST_SUITE
{
	TEST_RUN(null);
	TEST_RUN(zero_dim);
	TEST_RUN(no_label);
}

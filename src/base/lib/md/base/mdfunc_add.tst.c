#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>

TEST_SUITE
{
	SetupMdMat();
	SetProgName("ID");
	SetApplPath(UnitTest_tmp(NULL));
	TEST_FILE("mdmat.msg",
		"16	$!: can not add values of type '$1'.\n"
	);

	EfiFunc *func;

	UnitTest_stderr("stderr");
	EXPECT(mdfunc_add(&Type_void) == NULL);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"MSG ID: can not add values of type 'void'.\n");

	EXPECT_PTR(func = mdfunc_add(&Type_int));
	EXPECT_UINT(func->weight, CONV_STANDARD);
	rd_deref(func);

	EXPECT_PTR(func = mdfunc_add(&Type_str));
	EXPECT_UINT(func->weight, CONV_GENERATED);
	rd_deref(func);
}

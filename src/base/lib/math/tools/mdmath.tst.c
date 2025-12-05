#include <EFEU/UnitTest.h>
#include <Math/mdmath.h>
#include <EFEU/utest/out.tst.h>

TEST_SUITE
{
	SetupStd();
	SetupMdMat();
	SetupMdMath();

	TEST_EXPR(streval("mdmat md = mdread(string!\n"
		"##MDMAT 2.0\n"
		"##Type double\n"
		"##Locale us\n"
		"##Rows y\n"
		"##Columns x\n"
		"2 x1 x2\n"
		"y1 1 2\n"
		"y2 2 -1\n"
		"!)\n"));
	TEST_EXPR(streval("md.mark(\"x\")"));
	EfiObj *obj = EvalObj(strterm("mdinv(md)"), NULL);
	EXPECT_STR(obj->type->name, "mdmat");
	md_print(io_refer(OUT()), Val_mdmat(obj->data), "mode=data");
	EXPECT_NOUT(Val_mdmat(obj->data),
		"0.20000, 0.40000\n0.40000, -0.20000\n");
	CLEANUP_OUT;
}

#include <EFEU/UnitTest.h>
#include <EFEU/EfiOutput.h>
#include <EFEU/printobj.h>
#include <EFEU/utest/out.tst.h>

static int test_write(IO *out, void *data, unsigned flag)
{
	io_printf(out, "%d %#x\n", flag, Val_int(data));
	return 1;
}

static EfiPrintFlag test_flags[] = {
	{ 1, 'a', "Flag-A" },
	{ 2, 'x', "Flag-X" },
};

static EfiPrintDef test_print = {
	test_write, test_flags, 2
};

static EfiOutput test_output[] = {
	EPC_PRINT(&Type_int, "test", "Testausgabe", &test_print),
};

TEST_DECL(binary)
{
	IO *out;
	EfiOutput *odef;
	void *par;
	EfiObj *obj;

	EXPECT_PTR(obj = int2Obj(0));
	EXPECT_PTR(out = rd_refer(OUT()));
	EXPECT_PTR(odef = SearchEfiPar(obj->type, &EfiPar_output, "binary"));
	EXPECT_PTR(par = odef->out_open(out, obj->type, odef->par, NULL, NULL));

	TEST_EXPR(Val_int(obj->data) = 0x01020304);
	EXPECT(odef->out_write(obj->type, obj->data, par) == 4); 
	TEST_EXPR(Val_int(obj->data) = 0x05060708);
	EXPECT(odef->out_write(obj->type, obj->data, par) == 4); 
	EXPECT(rd_deref(par) != NULL);
	EXPECT(rd_deref(obj) == NULL);
	EXPECT_OUT("\001\002\003\004\005\006\007\010");
}

TEST_DECL(test_print)
{
	IO *out;
	EfiOutput *odef;
	void *par;
	EfiObj *obj;

	EXPECT_PTR(obj = int2Obj(0));
	EXPECT_PTR(out = rd_refer(OUT()));
	EXPECT_PTR(odef = SearchEfiPar(obj->type, &EfiPar_output, "test"));
	EXPECT_PTR(par = odef->out_open(out, obj->type, odef->par, NULL, "x"));

	TEST_EXPR(Val_int(obj->data) = 0x10);
	EXPECT(odef->out_write(obj->type, obj->data, par) == 1); 
	TEST_EXPR(Val_int(obj->data) = 0x20);
	EXPECT(odef->out_write(obj->type, obj->data, par) == 1); 
	EXPECT(rd_deref(par) == NULL);
	EXPECT(rd_deref(obj) == NULL);
	EXPECT_OUT("2 0x10\n2 0x20\n");
}

TEST_SUITE
{
	SetupStd();
	SetupEfiOutput();
	AddEfiParTab(&Type_int, test_output, 1, sizeof test_output[0]);
	ListEfiPar(OUT(), &Type_int, &EfiPar_output, NULL, 2);
	EXPECT_OUT(
		"test=ax\tTestausgabe\n"
		"\ta\tFlag-A\n"
		"\tx\tFlag-X\n"
		"binary\n\tbinary output\n"
	);

	TEST_RUN(binary);
	TEST_RUN(test_print);
	CLEANUP_OUT;
}

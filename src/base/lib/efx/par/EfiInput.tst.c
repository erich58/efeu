#include <EFEU/UnitTest.h>
#include <EFEU/EfiInput.h>
#include <EFEU/printobj.h>
#include "utest/out.tst.h"
#include "utest/mstr.tst.h"

TEST_DECL(binary)
{
	IO *in;
	EfiInput *idef;
	void *par;
	EfiObj *obj;

	EXPECT_PTR(obj = int2Obj(0));
	EXPECT_PTR(in = io_string("\001\002\003\004", NULL));
	EXPECT_PTR(idef = SearchEfiPar(obj->type, &EfiPar_input, "binary"));
	EXPECT_PTR(par = idef->in_open(in, obj->type, idef->par, NULL, NULL));
	EXPECT(idef->in_read(obj->type, obj->data, par) == 1); 
	EXPECT(Obj2int(RefObj(obj)) == 0x01020304);
	EXPECT(idef->in_read(obj->type, obj->data, par) == 0); 
	EXPECT(rd_deref(par) == NULL);
	EXPECT(rd_deref(obj) == NULL);
}

TEST_DECL(plain)
{
	IO *in;
	EfiInput *idef;
	void *par;
	EfiObj *obj;

	EXPECT_PTR(obj = int2Obj(0));
	EXPECT_PTR(in = io_string("1\n2\n3\n4\n", NULL));
	EXPECT_PTR(idef = SearchEfiPar(obj->type, &EfiPar_input, "plain"));
	/* opt: lines to skip */
	EXPECT_PTR(par = idef->in_open(in, obj->type, idef->par, "3", NULL));
	EXPECT(idef->in_read(obj->type, obj->data, par) == 1); 
	EXPECT(Obj2int(RefObj(obj)) == 4);
	EXPECT(idef->in_read(obj->type, obj->data, par) == 0); 
	EXPECT(rd_deref(par) == NULL);
	EXPECT(rd_deref(obj) == NULL);

	EXPECT_PTR(obj = NewObj(GetType("TestData"), NULL));
	EXPECT_PTR(in = io_string("1;2;3\n4\n2+3;6;7\n", NULL));
	EXPECT_PTR(idef = SearchEfiPar(obj->type, &EfiPar_input, "plain"));
	EXPECT_PTR(par = idef->in_open(in, obj->type, idef->par, NULL, NULL));
	EXPECT(idef->in_read(obj->type, obj->data, par) == 1); 
	EXPECT_MSTR(Obj2str(RefObj(obj)), "{1, 2, 3}");
	EXPECT(idef->in_read(obj->type, obj->data, par) == 1); 
	EXPECT_MSTR(Obj2str(RefObj(obj)), "{4, 0, 0}");
	EXPECT(idef->in_read(obj->type, obj->data, par) == 1); 
	EXPECT_MSTR(Obj2str(RefObj(obj)), "{5, 6, 7}");
	EXPECT(idef->in_read(obj->type, obj->data, par) == 0); 
	EXPECT(rd_deref(par) == NULL);
	EXPECT(rd_deref(obj) == NULL);
}

TEST_DECL(label)
{
	IO *in;
	EfiInput *idef;
	void *par;
	EfiObj *obj;

	EXPECT_PTR(obj = int2Obj(0));
	EXPECT_PTR(in = io_string("#hdr\n42\n\"10\"\n", NULL));
	EXPECT_PTR(idef = SearchEfiPar(obj->type, &EfiPar_input, "label"));
	EXPECT_PTR(par = idef->in_open(in, obj->type, idef->par, NULL, NULL));
	EXPECT(idef->in_read(obj->type, obj->data, par) == 1); 
	EXPECT(Obj2int(RefObj(obj)) == 42);
	EXPECT(idef->in_read(obj->type, obj->data, par) == 1); 
	EXPECT(Obj2int(RefObj(obj)) == 10);
	EXPECT(idef->in_read(obj->type, obj->data, par) == 0); 
	EXPECT(rd_deref(par) == NULL);
	EXPECT(rd_deref(obj) == NULL);

	EXPECT_PTR(obj = NewObj(GetType("TestData"), NULL));
	EXPECT_PTR(in = io_string(
		"# Comment\n"
		"1\t2; 3\n"
		"4;;\n"
		"2 + 3; 6 ;7",
		NULL));
	EXPECT_PTR(idef = SearchEfiPar(obj->type, &EfiPar_input, "label"));
	EXPECT_PTR(par = idef->in_open(in, obj->type, idef->par, NULL, NULL));
	EXPECT(idef->in_read(obj->type, obj->data, par) == 1); 
	EXPECT_MSTR(Obj2str(RefObj(obj)), "{1, 2, 3}");
	EXPECT(idef->in_read(obj->type, obj->data, par) == 1); 
	EXPECT_MSTR(Obj2str(RefObj(obj)), "{4, 0, 0}");
	EXPECT(idef->in_read(obj->type, obj->data, par) == 1); 
	EXPECT_MSTR(Obj2str(RefObj(obj)), "{2, 6, 7}");
	EXPECT(idef->in_read(obj->type, obj->data, par) == 0); 
	EXPECT(rd_deref(par) == NULL);
	EXPECT(rd_deref(obj) == NULL);
}


TEST_SUITE
{
	SetupStd();
	SetupEfiInput();
	ListEfiPar(OUT(), NULL, &EfiPar_input, NULL, 2);
	EXPECT_OUT(
		"binary\n\tbinary input\n"
		"label\n\tlabel text input\n"
		"plain\n\tplain text input\n"
	);

	TEST_EXPR(streval("struct TestData { int a; int b; int c; }"));
	TEST_RUN(binary);
	TEST_RUN(plain);
	TEST_RUN(label);
	CLEANUP_MSTR;
	CLEANUP_OUT;
}

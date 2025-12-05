#include <EFEU/UnitTest.h>
#include <EFEU/EfiClass.h>
#include <EFEU/printobj.h>
#include <EFEU/stdtype.h>
#include <EFEU/utest/out.tst.h>

static int print_Test (const EfiType *type, const void *data, IO *io)
{
	return io_printf(io, "%d", Val_int(data));
}

EfiType Type_Test = SIMPLE_TYPE("Test", int, &Type_int, print_Test);

static EfiObj *OBJ(int value)
{
	static EfiObj *obj = NULL;
	
	if	(!obj)
		obj = NewObj(&Type_Test, NULL);

	Val_int(obj->data) = value;
	return obj;
}

static Label class_label[] = {
	{ "isOne", "Value 1" },
	{ "isThree", "Value 3" },
};

static int class_update (const EfiObj *base, void *par)
{
	switch (Val_int(base->data))
	{
	case  1:	return 1;
	case  3:	return 2;
	default:	return 0;
	}
}

static EfiClassDef cdef[] = {
	{ "tclass", "Description", 2, class_label, class_update, NULL },
};

TEST_DECL(AddEfiClassDef)
{
	AddEfiClassDef(Type_Test.vtab, cdef, 1);
	ShowVarTab(OUT(), NULL, Type_Test.vtab);
	EXPECT_OUT("Test = {\n"
		"\ttclass\n"
		"}");
	EXPECT(Obj2int(GetVar(Type_Test.vtab, "tclass", OBJ(0))) == 0);
	EXPECT(Obj2int(GetVar(Type_Test.vtab, "tclass", OBJ(1))) == 1);
	EXPECT(Obj2int(GetVar(Type_Test.vtab, "tclass", OBJ(2))) == 0);
	EXPECT(Obj2int(GetVar(Type_Test.vtab, "tclass", OBJ(3))) == 2);
	VarTab_del(Type_Test.vtab, "tclass");
}

TEST_DECL(MakeEfiClass)
{
	EXPECT(MakeEfiClass(&Type_Test, "numclass",
		"switch=5:isFive;*:isNotFive"));
	ShowVarTab(OUT(), NULL, Type_Test.vtab);
	EXPECT_OUT("Test = {\n"
		"\tnumclass\n"
		"}");
	EXPECT(Obj2int(GetVar(Type_Test.vtab, "numclass", OBJ(5))) == 1);
	EXPECT(Obj2int(GetVar(Type_Test.vtab, "numclass", OBJ(1))) == 2);
	VarTab_del(Type_Test.vtab, "numclass");

	EXPECT(MakeEfiClass(&Type_Test, "anyname", "undefined") == 0);

	UnitTest_stderr("stderr");
	EXPECT(MakeEfiClass(&Type_Test, "anyname", NULL) == 0);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"flag         "
		"generic      "
		"switch       "
		"test\n");

	UnitTest_stderr("stderr");
	EXPECT(MakeEfiClass(&Type_Test, "anyname", " \t") == 0);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"flag         "
		"generic      "
		"switch       "
		"test\n");

	UnitTest_stderr("stderr");
	EXPECT(MakeEfiClass(&Type_Test, "anyname", "flag=?") == 0);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"flag[expr]=label desc\n"
		"\tflag expression\n"
	);

	UnitTest_stderr("stderr");
	EXPECT(MakeEfiClass(&Type_Test, "anyname", "?") == 0);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"flag\tflag expression\n"
		"generic\tgeneric class construction\n"
		"switch\tswitch expression\n"
		"test\ttest expression\n"
	);

	UnitTest_stderr("stderr");
	EXPECT(MakeEfiClass(&Type_Test, "anyname", "??") == 0);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"flag[expr]=label desc\n"
		"\tflag expression\n"
		"generic[expr]=case label desc;...\n"
		"\tgeneric class construction\n"
		"switch[fmt]=key label desc;...\n"
		"\tswitch expression\n"
		"test[fmt]=key label desc;...\n"
		"\ttest expression\n"
	);
}

TEST_DECL(EfiClassExpr)
{
	EfiObj *base = int2Obj(0);
	VarTab_xadd(NULL, "x", NULL, RefObj(base));
	EfiObj *expr = strterm("return x + 2");
	EXPECT(Obj2int(EvalExpression(expr)) == 2);

	EXPECT(EfiClassExpr(NULL, "anything") == NULL);
	EfiObj *cl1 = EfiClassExpr(base, "switch=5:isFive;*:isNotFive");
	EfiObj *cl2 = EfiClassExpr(expr, "switch=5:isFive;*:isNotFive");

	EXPECT(Obj2int(EvalExpression(cl1)) == 2);
	EXPECT(Obj2int(EvalExpression(cl2)) == 2);
	Val_int(base->data) = 3;
	EXPECT(Obj2int(EvalExpression(cl1)) == 2);
	EXPECT(Obj2int(EvalExpression(cl2)) == 1);
	Val_int(base->data) = 5;
	EXPECT(Obj2int(EvalExpression(cl1)) == 1);
	EXPECT(Obj2int(EvalExpression(cl2)) == 2);

	UnrefObj(cl1);
	UnrefObj(cl2);
	UnrefObj(expr);
	UnrefObj(base);

	UnitTest_stderr("stderr");
	TEST_FORK
	{
		EXPECT(EfiClassExpr(int2Obj(0), NULL) == 0);
		exit(1); /* not reached */
	}
	CHILD_EXIT(0);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"flag         "
		"generic      "
		"switch       "
		"test\n");

	UnitTest_stderr("stderr");
	TEST_FORK
	{
		EXPECT(EfiClassExpr(int2Obj(0), "?") == 0);
		exit(1); /* not reached */
	}

	CHILD_EXIT(0);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"flag\tflag expression\n"
		"generic\tgeneric class construction\n"
		"switch\tswitch expression\n"
		"test\ttest expression\n"
	);

	UnitTest_stderr("stderr");
	TEST_FORK
	{
		EXPECT(EfiClassExpr(int2Obj(0), "flag=?") == 0);
		exit(1); /* not reached */
	}
	CHILD_EXIT(0);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"flag[expr]=label desc\n"
		"\tflag expression\n"
	);
}


TEST_SUITE
{
	SetupStd();
	AddType(&Type_Test);
	SetupEfiClass();
	PrintType(OUT(), &Type_Test, 1);
	EXPECT_OUT("Test");
	ShowVarTab(OUT(), NULL, Type_Test.vtab);
	EXPECT_OUT("Test = {\n}");
	TEST_RUN(AddEfiClassDef);
	TEST_RUN(MakeEfiClass);
	TEST_RUN(EfiClassExpr);
	CLEANUP_OUT;
}

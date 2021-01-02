#include <EFEU/UnitTest.h>
#include <EFEU/printobj.h>
#include <EFEU/parsedef.h>
#include <EFEU/ioctrl.h>
#include "utest/in.tst.h"
#include "utest/out.tst.h"
#include "utest/mstr.tst.h"

static EfiObj *OBJ(const char *def)
{
	static EfiObj *obj = NULL;
	rd_deref(obj);
	obj = def ? EvalObj(strterm(def), NULL) : NULL;
	return obj;
}

extern NameKeyTab TypeTab;

TEST_DECL(setup)
{
	SetupStd();
	TEST_EXPR(streval("struct DATA { int a; int b; };"));
	TEST_EXPR(streval("DATA DATA(int x, int y) { a = x; b = y; };"));
	TEST_EXPR(streval("struct XDATA { int a; struct { int c; } b; }"));
	TEST_EXPR(streval("XDATA XDATA(int x, int y) { a = x; b.c = y; };"));
	TEST_EXPR(streval("enum ENUM { A, B, C };"));
}

TEST_DECL(func)
{
	EfiFunc *func;

	EXPECT(ConstructFunc("name", NULL, NULL) == NULL);

	func = ConstructFunc("TestFunc", "int x = 42", NULL);
	ListFunc(OUT(), func);
	EXPECT_OUT("virtual TestFunc TestFunc ()");
	nkt_delete(&TypeTab, "TestFunc");
	EXPECT(rd_deref(func) == NULL);

	int a = 0;
	EfiVarDef vdef[] = {
		{ "a", &Type_int, &a, "A-Desc" },
	};
	EfiVarTab *tab = VarTab("TestVar", 0);
	AddVarDef(tab, vdef, 1);

	UnitTest_stderr("stderr");
	TEST_FORK
	{
		ConstructFunc("TestFunc", "?", tab);
		exit(1); /* not reached */
	}
	CHILD_EXIT(0);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr", "var = {\n\ta\n}\n");

	func = ConstructFunc("TestFunc", " { a; } ", tab);
	ListFunc(OUT(), func);
	EXPECT_OUT("virtual TestFunc TestFunc ()");
	PrintType(OUT(), func->type, 1);
	EXPECT_OUT("struct TestFunc { int a }");
	EXPECT_STR(Obj2str(CallFuncObj(func, NULL)), "{0}");
	TEST_EXPR(a = 5);
	EXPECT_STR(Obj2str(CallFuncObj(func, NULL)), "{5}");

	nkt_delete(&TypeTab, "TestFunc");
	EXPECT(rd_deref(func) == NULL);

	func = ConstructFunc("TestFunc", " { * } ", tab);
	ListFunc(OUT(), func);
	EXPECT_OUT("virtual TestFunc TestFunc ()");
	PrintType(OUT(), func->type, 1);
	EXPECT_OUT("struct TestFunc { int a }");
	TEST_EXPR(a = 11);
	EXPECT_STR(Obj2str(CallFuncObj(func, NULL)), "{11}");
}

static EfiFunc *ofunc = NULL;

#define	CREATE_OF(name, def, obj, fdef, tdef)	\
PRINT_LABEL(name); \
EXPECT_PTR(ofunc = ConstructObjFunc(name, def, OBJ(obj))); \
ListFunc(OUT(), ofunc); \
EXPECT_NOUT(ofunc, fdef); \
PrintType(OUT(), ofunc->type, 1); \
EXPECT_NOUT(ofunc->type, tdef);

#define	VERIFY_OF(expr, result)	\
EXPECT_MSTR(Obj2str(CallFuncObj(ofunc, OBJ(expr)->data)), result);

TEST_DECL(objfunc)
{
	EXPECT(ConstructObjFunc("name", "int a", NULL) == NULL);
	EXPECT(ConstructObjFunc("name", NULL, OBJ("5")) == NULL);

	UnitTest_stderr("stderr");
	TEST_FORK
	{
		ConstructObjFunc("TestFunc", "?", OBJ("DATA(1, 2)"));
		exit(1); /* not reached */
	}
	CHILD_EXIT(0);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"struct DATA {\n"
		"\tint a;\n"
		"\tint b;\n"
		"} = {\n"
		"\ta b\n"
		"}\n"
	);

	CREATE_OF("OF1",
		" int x = this.a; this.b ",
		"DATA(1, 2)",
		"virtual OF1 OF1 (DATA = {1, 2})",
		"struct OF1 { int x, b }");
	VERIFY_OF("DATA(47, 11)", "{47, 11}");

	CREATE_OF("OF2",
		" { x = this.a; str z = this.a + this.b; } ",
		"DATA(1, 2)",
		"virtual OF2 OF2 (DATA = {1, 2})",
		"struct OF2 { int x; str z }");
	VERIFY_OF("DATA(8, 20)", "{8, \"28\"}");

	CREATE_OF("OF3",
		" { *; x = this.a; this.b.c }",
		"XDATA(1, 2)",
		"virtual OF3 OF3 (XDATA = {1, {2}})",
		"struct OF3 { int a; struct { int c } b; int x, c }");
	VERIFY_OF("XDATA(7, 11)", "{1, {2}, 7, 11}");
}

#define	CREATE(name, def, type)	\
PRINT_LABEL(name); \
EXPECT_MSTR(Obj2str(PFunc_construct(IN(name def), NULL)), type)
#define	VERIFY(expr, result)	\
EXPECT_MSTR(Obj2str(strterm(expr)), result)

TEST_DECL(construct)
{
	UnitTest_stderr("stderr");
	EXPECT(PFunc_construct(IN("Test"), NULL) == NULL);
	UnitTest_stderr(NULL);
	MATCH_CONTENT("stderr", ".");

	UnitTest_stderr("stderr");
	TEST_FORK
	{
		PFunc_construct(IN("Test : int x, str s { ? }"), NULL);
		exit(1); /* not reached */
	}
	CHILD_EXIT(0);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr", "var = {\n\ts x\n}\n");

	UnitTest_stderr("stderr");
	TEST_FORK
	{
		PFunc_construct(IN("Test : XDATA x { x.b.? }"), NULL);
		exit(1); /* not reached */
	}
	CHILD_EXIT(0);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr", "struct {\n\tint c;\n} = {\n\tc\n}\n");

	CREATE("T0", "{ int x; }", "struct T0 { int x }");
	VERIFY("T0()", "{0}");

	CREATE("T1", " : int a { int x = a; }", "struct T1 { int x }");
	VERIFY("T1(3)", "{3}");

	CREATE("T2", " : int a { { a }; str x = 2 * a; }",
		"struct T2 { int a; str x }");
	VERIFY("T2(3)", "{3, \"6\"}");

	CREATE("T3", " : int a { int x = 2 * a; (a) }",
		"struct T3 { int x, a }");
	VERIFY("T3(3)", "{6, 3}");

	CREATE("T4", " : int a, int b { *; x = a + b }",
		"struct T4 { int a, b, x }");
	VERIFY("T4(1, 2)", "{1, 2, 3}");

	CREATE("T5", " { * int x = 5 }", "struct T5 { int x }");
	VERIFY("T5()", "{5}");

	CREATE("T6", " : int a, int b, int c { [#1, c] }",
		"struct T6 { int a, c }");
	VERIFY("T6(1, 2, 3)", "{1, 3}");

	CREATE("T7", " : int a { enum { X, Y } x = a }",
		"struct T7 { enum { X = 0, Y = 1, } x }");
	VERIFY("T7(1)", "{1}");

	CREATE("T8", " : XDATA d, str s { *; d.[a], x = d.a, d.b.*, y = d.b.c }",
		"struct T8 { struct XDATA { int a; struct { int c } b } d; str s; int a, x, c, y }");
	VERIFY("T8(XDATA(1, 2), \"abc\")", "{{1, {2}}, \"abc\", 0, 1, 0, 2}");
}

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(func);
	TEST_RUN(objfunc);
	TEST_RUN(construct);
	CLEANUP_IN;
	CLEANUP_OUT;
	CLEANUP_MSTR;
	OBJ(NULL);
}

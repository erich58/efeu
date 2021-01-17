#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <EFEU/EDB.h>
#include "utest/eval.tst.h"

static void f_TMP (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = mstrcpy(UnitTest_tmpname(Val_str(arg[0])));
}

static EfiFuncDef func[] = {
	{ 0, &Type_str, "TMP(str name)", f_TMP },
};

TEST_DECL(create)
{
	EXPECT_EVAL("EDB edb", "NULL");
	EXPECT_EVAL("edb = EDB()", "EDB: void");
	EXPECT_EVAL("edb = EDB(int, \"DESC\")", "EDB: int");
	EXPECT_EVAL("edb.desc", "\"DESC\"");
	EXPECT_EVAL("edb.type", "int");
	EXPECT_EVAL("edb.head = \"header\"", "\"header\"");
	EXPECT_EVAL("edb.data = 42", "42");
	EXPECT_EVAL("double x = 10.", "10.000");
	EXPECT_EVAL("edb = EDB(x, NULL)", "EDB: double");
	EXPECT_EVAL("edb.desc", "NULL");
	EXPECT_EVAL("edb.data", "10.000");
	EXPECT_EVAL("edb = str", "EDB: str");
}

TEST_DECL(read)
{
	TEST_FILE("data", "EDB 1.0\nint\n@data plain\n10\n");
	MATCH_EVAL("EDB edb = EDB(open(TMP(\"data\"), \"r\"))",
		"EDB: int in=.*/data");
	EXPECT_EVAL("edb.data", "0");
	EXPECT_EVAL("edb.read()", "true");
	EXPECT_EVAL("edb.data", "10");
	EXPECT_EVAL("edb.read()", "false");
	EXPECT_EVAL("edb.closein()", "");
	EXPECT_EVAL("edb", "EDB: int");
	EXPECT_EVAL("edb.datain((IO) \"1\n2\n3\n\"))", "");
	EXPECT_EVAL("edb", "EDB: int in=\"1\\n2\\n3\\n\":0");
	EXPECT_EVAL("edb.filter(\"clip=1\")", "EDB: int in=\"1\\n2\\n3\\n\":2");
	EXPECT_EVAL("edb.read()", "true");
	EXPECT_EVAL("edb.data", "2");
	EXPECT_EVAL("edb.unread()", "");
	EXPECT_EVAL("edb.read()", "true");
	EXPECT_EVAL("edb.data", "2");
	EXPECT_EVAL("edb.read()", "true");
	EXPECT_EVAL("edb.data", "3");
	EXPECT_EVAL("edb.read()", "false");
	EXPECT_EVAL("edb.closein()", "");
	EXPECT_EVAL("edb = edbopen(TMP(\"data\"), \"conv=str\")", "EDB: str");
	EXPECT_EVAL("edb.read()", "true");
	EXPECT_EVAL("edb.data", "\"10\"");
}

TEST_DECL(write)
{
	EXPECT_EVAL("EDB edb = EDB(int, \"DESC\nNext Line\n\")", "EDB: int");
	EXPECT_EVAL("edb.head = \"\";", "");
	EXPECT_EVAL("edb.fout(TMP(\"output\"), \"verbose\")", "");
	EXPECT_EVAL("edb.data = 11; edb.write()", "");
	EXPECT_EVAL("edb.data = 17; edb.write()", "");
	EXPECT_EVAL("edb.closeout()", "");
	EXPECT_CONTENT("output",
		"EDB\t1.0\n"
		"// DESC\n"
		"// Next Line\n"
		"@head\n"
		"\n"
		"@type\n"
		"int\n"
		"@data plain\n"
		"11\n"
		"17\n"
	);
	EXPECT_EVAL("edb.out(open(TMP(\"output\"), \"w\"), \"nohead\")", "");
	MATCH_EVAL("edb", "EDB: int out=.*/output");
	EXPECT_EVAL("edb.data = 42; edb.write()", "");
	EXPECT_EVAL("edb.closeout()", "");
	EXPECT_CONTENT("output", "42\n");
}

TEST_DECL(vec)
{
	EXPECT_EVAL("int x[] = 1:5:2", "{1, 3, 5}");
	EXPECT_EVAL("x.save(TMP(\"db\"), \"compact\", \"DESC\")", "");
	EXPECT_CONTENT("db",
		"EDB\t1.0\n"
		"int\n"
		"@data plain\n"
		"1\n"
		"3\n"
		"5\n"
	);
	EXPECT_EVAL("int[] y", "Vec: int[0]");
	EXPECT_EVAL("y.load(TMP(\"db\"))", "");
	EXPECT_EVAL("y", "Vec: int[3]");
	EXPECT_EVAL("(List_t) y", "{1, 3, 5}");

	EXPECT_EVAL("int y2[2]", "{0, 0}");
	EXPECT_EVAL("y2.load(TMP(\"db\"))", "");
	EXPECT_EVAL("y2", "{1, 3}");

	EXPECT_EVAL("int y3[5]", "{0, 0, 0, 0, 0}");
	EXPECT_EVAL("y3.load(TMP(\"db\"))", "");
	EXPECT_EVAL("y3", "{1, 3, 5, 0, 0}");

	EXPECT_EVAL("x.save(TMP(\"db\"), \"nohead\", \"DESC\")", "");
	EXPECT_CONTENT("db",
		"1\n"
		"3\n"
		"5\n"
	);
	EXPECT_EVAL("y.resize(0)", "");
	EXPECT_EVAL("(List_t) y", "{}");
	EXPECT_EVAL("y", "Vec: int[0]");
	EXPECT_EVAL("y.xload(TMP(\"db\"), \"plain\", \"clip=0,2\")", "");
	EXPECT_EVAL("(List_t) y", "{1, 3}");
}

TEST_DECL(combine)
{
	EXPECT_EVAL("int x[] = 1:5:1", "{1, 2, 3, 4, 5}");
	EXPECT_EVAL("x.save(TMP(\"db1\"), \"compact\")", "");
	EXPECT_CONTENT("db1",
		"EDB\t1.0\n"
		"int\n"
		"@data plain\n"
		"1\n"
		"2\n"
		"3\n"
		"4\n"
		"5\n"
	);
	EXPECT_EVAL("EDB edb = edbopen(TMP(\"db1\"));", "");
	EXPECT_EVAL("edb.fout(TMP(\"db2\"), \"compact\");", "");
	EXPECT_EVAL("edb.copy(3)", "3");
	EXPECT_EVAL("edb = NULL", "NULL");
	EXPECT_CONTENT("db2",
		"EDB\t1.0\n"
		"int\n"
		"@data plain\n"
		"1\n"
		"2\n"
		"3\n"
	);
	PRINT_LABEL("cat");
	EXPECT_EVAL("EDB db1 = edbopen(TMP(\"db1\"));", "");
	EXPECT_EVAL("EDB db2 = edbopen(TMP(\"db2\"));", "");
	EXPECT_EVAL("edb = edbcat(db1, db2)", "EDB: int");
	EXPECT_EVAL("edb.fout(TMP(\"db\"), \"compact\");", "");
	EXPECT_EVAL("edb.copy(0)", "8");
	EXPECT_EVAL("edb = NULL;", "");
	EXPECT_CONTENT("db",
		"EDB\t1.0\n"
		"int\n"
		"@data plain\n"
		"1\n"
		"2\n"
		"3\n"
		"4\n"
		"5\n"
		"1\n"
		"2\n"
		"3\n"
	);

	PRINT_LABEL("merge");
	EXPECT_EVAL("EDB db1 = edbopen(TMP(\"db1\"));", "");
	EXPECT_EVAL("EDB db2 = edbopen(TMP(\"db2\"));", "");
	EXPECT_EVAL("edb = edbmerge(\".\", false, db1, db2)", "EDB: int");
	EXPECT_EVAL("edb.fout(TMP(\"db\"), \"compact\");", "");
	EXPECT_EVAL("edb.copy(0)", "8");
	EXPECT_EVAL("edb = NULL;", "");
	EXPECT_CONTENT("db",
		"EDB\t1.0\n"
		"int\n"
		"@data plain\n"
		"1\n"
		"1\n"
		"2\n"
		"2\n"
		"3\n"
		"3\n"
		"4\n"
		"5\n"
	);

	PRINT_LABEL("sort and merge");
	EXPECT_EVAL("EDB db1 = edbopen(TMP(\"db1\"));", "");
	EXPECT_EVAL("EDB db2 = edbopen(TMP(\"db2\"));", "");
	EXPECT_EVAL("edb = edbmerge(\"-\", true, db1, db2)", "EDB: int");
	EXPECT_EVAL("edb.fout(TMP(\"db\"), \"compact\");", "");
	EXPECT_EVAL("edb.copy(0)", "8");
	EXPECT_EVAL("edb = NULL;", "");
	EXPECT_CONTENT("db",
		"EDB\t1.0\n"
		"int\n"
		"@data plain\n"
		"5\n"
		"4\n"
		"3\n"
		"3\n"
		"2\n"
		"2\n"
		"1\n"
		"1\n"
	);
	EXPECT_EVAL("edb = edbmerge(\"-\", true)", "NULL");
	PRINT_LABEL("join - test only call"); 
	EXPECT_EVAL("edbjoin(NULL, NULL, NULL)", "NULL");
}

TEST_DECL(trans)
{
	EXPECT_EVAL("struct DATA { int a; int b; };", "");
	EXPECT_EVAL("EDB edb = EDB(DATA)", "EDB: DATA");
	EXPECT_EVAL("edb.trans(\"var=a\")", "EDB: int");
}

TEST_DECL(header)
{
	EXPECT_EVAL("struct DATA { int a; int b[2]; };", "");
	EXPECT_EVAL("EDB edb = EDB(DATA)", "EDB: DATA");
	EXPECT_EVAL("edb.data = { 3, {5, 7}}", "{3, {5, 7}}");
	EXPECT_EVAL("edb.fout(TMP(\"output\"), \"plain\")", "");
	EXPECT_EVAL("edb.write()", "");
	EXPECT_EVAL("edb.closeout()", "");
	EXPECT_CONTENT("output",
		"EDB\t1.0\n"
		"int a;\n"
		"int b[2];\n"
		"@data plain\n"
		"#a;b[2]\n"
		"3;{5, 7}\n"
	);
}

TEST_SUITE
{
	SetupStd();
	SetupEDB();
	AddFuncDef(func, tabsize(func));
	TEST_EXEC(create);
	TEST_EXEC(read);
	TEST_EXEC(write);
	TEST_EXEC(vec);
	TEST_EXEC(combine);
	TEST_EXEC(trans);
	TEST_EXEC(header);
	CLEANUP_EVAL;
}

#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <EFEU/parsedef.h>
#include <EfiType/SC.h>
#include "utest/term.tst.h"
#include "utest/eval.tst.h"

static void tmpvar()
{
	static EfiParseDef pdef[] = { "TMP", PFunc_str, NULL };

	pdef[0].data = mstrcpy(UnitTest_tmp("TMP"));
	AddParseDef(pdef, 1);
}

TEST_DECL(create)
{
	PushVarTab(NULL, NULL);
	EXPECT_TERM("SC sc = SC(2,4)", "SC: 2x4", NULL);
	EXPECT_EVAL("sc.data(1, 1)", "0.");
	EXPECT_EVAL("sc.data(2, 4)", "0.");
	EXPECT_EVAL("sc.data(2, 5)", "NULL");
	EXPECT_EVAL("sc.data(3, 4)", "NULL");
	PopVarTab();
}

TEST_DECL(index)
{
	EXPECT_EVAL("SCIndex(0,0)", "\"A0\"");
	EXPECT_EVAL("SCIndex(1,1)", "\"A0\"");
	EXPECT_EVAL("SCIndex(1,-1)", "\"$A0\"");
	EXPECT_EVAL("SCIndex(1,2)", "\"B0\"");
	EXPECT_EVAL("SCIndex(1,26)", "\"Z0\"");
	EXPECT_EVAL("SCIndex(1,27)", "\"AA0\"");

	EXPECT_EVAL("SCIndex(2,1)", "\"A1\"");
	EXPECT_EVAL("SCIndex(-2,1)", "\"A$1\"");
	EXPECT_EVAL("SCIndex(100,1)", "\"A99\"");
}

TEST_DECL(range)
{
	EXPECT_EVAL("SCRange(\"Sum\",1,1)", "\"Sum(A0:A0)\"");
	EXPECT_EVAL("SCRange(\"Sum\",1,1,-2,-2)", "\"Sum(A0:$B$1)\"");
}

TEST_DECL(io)
{
	PushVarTab(NULL, NULL);
	EXPECT_EVAL("SC sc", "NULL");
	EXPECT_EVAL("print(open(TMP, \"w\"), sc)", "4");
	EXPECT_CONTENT("TMP", "NULL");
	EXPECT_EVAL("write(open(TMP, \"w\"), sc)", "0");
	EXPECT_CONTENT("TMP", "");
	EXPECT_EVAL("sc.rows", "0");
	EXPECT_EVAL("sc.cols", "0");
	EXPECT_EVAL("sc.x", "0");
	EXPECT_EVAL("sc.y", "0");

	EXPECT_EVAL("sc = SC(3,4)", "SC: 3x4");
	EXPECT_EVAL("sc.format(2,8,1);", "");
	EXPECT_EVAL("sc.label(1,1) = \"Label\"", "\"Label\"");
	MATCH_EVAL("sc.data(1,2) = 42", "42.0*");
	EXPECT_EVAL("sc.expr(1,3) = SCIndex(1,2) + \" + 3\"", "\"B0 + 3\"");
	EXPECT_EVAL("sc.type(1,1) == SC_LABEL", "true");
	EXPECT_EVAL("sc.type(1,2) == SC_DATA", "true");
	EXPECT_EVAL("sc.type(1,3) == SC_EXPR", "true");
	EXPECT_EVAL("sc.type(1,4) == 0", "true");

	EXPECT_EVAL("sc.label(4,1)", "NULL");
	EXPECT_EVAL("sc.data(4,2)", "NULL");
	EXPECT_EVAL("sc.expr(4,3)", "NULL");
	EXPECT_EVAL("sc.get(2,1)", "NULL");
	EXPECT_EVAL("sc.get(4,1)", "NULL");

	EXPECT_EVAL("sc.rows", "3");
	EXPECT_EVAL("sc.cols", "4");
	EXPECT_EVAL("sc.x", "0");
	EXPECT_EVAL("sc.y", "0");
	EXPECT_EVAL("sc.x = 1", "1");
	EXPECT_EVAL("sc.y = 2", "2");

	MATCH_EVAL("print(open(TMP, \"w\"), sc)", "3");
	EXPECT_CONTENT("TMP", "3x4");

	MATCH_EVAL("sc.save(open(TMP, \"w\"))", "SC: 3x4");
	PRINT_CONTENT("TMP");
	MATCH_EVAL("sc = NULL", "NULL");
	MATCH_EVAL("sc = SC(open(TMP, \"r\"))", "SC: 1x3");
	MATCH_EVAL("sc.get(1,1)", "\"Label\"");
	MATCH_EVAL("sc.get(1,2)", "42");
	MATCH_EVAL("sc.get(1,3)", "45");
	MATCH_EVAL("sc.save(open(TMP, \"w\"))", "SC: 1x3");
	PRINT_CONTENT("TMP");
	PopVarTab();
}

TEST_DECL(scan)
{
	PushVarTab(NULL, NULL);
	TEST_FILE("TMP",
		"format AA 10 2 0\n"
		"let AA5 = 3. * 5.\n"
	);
	MATCH_EVAL("SC sc = SC(open(TMP, \"r\"))", "SC: 6x27");
	MATCH_EVAL("sc.get(6,27)", "15");
	MATCH_EVAL("sc.save(open(TMP, \"w\"))", "SC: 6x27");
	PRINT_CONTENT("TMP");
	PopVarTab();
}

TEST_SUITE
{
	SetupStd();
	SC_setup();
	tmpvar();
	TEST_RUN(create);
	TEST_RUN(index);
	TEST_RUN(range);
	TEST_RUN(io);
	TEST_RUN(scan);
	CLEANUP_TERM;
}

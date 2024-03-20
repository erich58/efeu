#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <EFEU/parsedef.h>
#include <EfiType/Json.h>
#include "utest/eval.tst.h"

static void tmpvar()
{
	static EfiParseDef pdef = { "TMP", PFunc_str, NULL };

	pdef.data = mstrcpy(UnitTest_tmp("TMP"));
	AddParseDef(&pdef, 1);
}

TEST_DECL(create)
{
	PushVarTab(NULL, NULL);
	EXPECT_EVAL("Json j", "{}");
	EXPECT_EVAL("j.empty()", "true");
	EXPECT_EVAL("j.size()", "0");
	EXPECT_EVAL("j.push_back(\"a\", int) = 42", "42");
	MATCH_EVAL("j.push_front(\"x\", double) = 2.5", "2.5");
	EXPECT_EVAL("j.empty()", "false");
	EXPECT_EVAL("j.size()", "2");
	EXPECT_EVAL("j.data(\"a\")", "42");
	EXPECT_EVAL("j[\"a\"]", "42");
	MATCH_EVAL("j[\"x\"]", "2.50*");
	MATCH_EVAL("j", "\\{\"x\":2.50*,\"a\":42}");
	EXPECT_EVAL("j.clear()", "");
	EXPECT_EVAL("j.size()", "0");
	EXPECT_EVAL("j[\"a\"]", "");
	PopVarTab();
}

TEST_DECL(str2json)
{
	PushVarTab(NULL, NULL);
	EXPECT_EVAL("Json j", "{}");
	EXPECT_EVAL("j = \"\\\"a\\\" : 42\"", "{\"a\":42}");
	PopVarTab();
}

TEST_DECL(io)
{
	PushVarTab(NULL, NULL);
	TEST_FILE("TMP", "{\n"
		"\"a\" : 42,\n"
		"\"b\" : [ 1, 2, 3 ], \n"
		"\"c\" : { \"x\" : 11 }\n"
	"}\n");
	EXPECT_EVAL("Json j", "{}");
	EXPECT_EVAL("j.parse(open(TMP, \"r\"))", "");
	EXPECT_EVAL("j[\"b\"]", "Vec: int[3]");
	EXPECT_EVAL("j[\"b\"][1]", "2");
	EXPECT_EVAL("j[\"c\"]", "{\"x\":11}");
	EXPECT_EVAL("j[\"c\"][\"x\"]", "11");
	EXPECT_EVAL("Json x = j", "{\"a\":42,\"b\":[1,2,3],\"c\":{\"x\":11}}");
	EXPECT_EVAL("print(open(TMP, \"w\"), x)", "33");
	EXPECT_CONTENT("TMP", "{\"a\":42,\"b\":[1,2,3],\"c\":{\"x\":11}}");
	PopVarTab();
}

TEST_SUITE
{
	SetupStd();
	Json_setup();
	tmpvar();
	TEST_RUN(create);
	TEST_RUN(str2json);
	TEST_RUN(io);
	EXPECT(1);
	CLEANUP_EVAL;
}

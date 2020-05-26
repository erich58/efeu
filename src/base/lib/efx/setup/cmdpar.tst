#!/usr/bin/env .build/script.tst

pconfig !
Res1 = "Test"
!

EXPECT_STR(getres("Res1", "Alt"), "Test");
EXPECT_STR(getres("Res2", "Alt"), "Alt");
setres("Res2", "1");
EXPECT(flagres("Res2"));
EXPECT(listres("Res") == { "Res1", "Res2" });

ApplPath = UnitTest::tmp();
TEST_FILE("test.msg", "1\tABC\n2\t\"DEF\"");
ShowFmtTab(OUT(), "test", "$0-$1: $2\n");
EXPECT_OUT("test-1: ABC\ntest-2: DEF\n");
EXPECT_STR(getfmt("[test:1]Format"), "ABC");

ArgList argl = NULL;
EXPECT_REPR(argl, "NULL");
argl = "test";
EXPECT_REPR(argl, "ArgList: test");
argl += 5;
EXPECT_REPR(argl, "ArgList: test \"5\"");
EXPECT_STR(argl[1], "5");
argl = ArgList("name", "first", "second");
EXPECT_REPR(argl, "ArgList: name \"first\" \"second\"");

EXPECT_STR(psub("$1-$2", argl), "first-second");
EXPECT_STR(psub("${3*5}"), "15");
EXPECT_STR(psub("$1-$2", 10, 20), "10-20");
EXPECT_STR(psub("$1-$2", { }), "-");

fpsub(OUT(), "$1-$2", argl);
EXPECT_OUT("first-second");
fpsub(OUT(), "${3*5}");
EXPECT_OUT("15");
fpsub(OUT(), "$1-$2", 10, 20);
EXPECT_OUT("10-20");

StrPool s = StrPool();
s.psub("$1-$2", argl);
s.psub("${3*5}");
s.psub("$1-$2", 10, 20);
s.dump(OUT());
MATCH_OUT(".*\"first-second\"\n.*\"15\"\n.*\"10-20\"\n");

EXPECT(dim(argl) == 3);
argl.set(1, "abc");
EXPECT_STR(argl[1], "abc");
EXPECT_STR(psub("$1", argl), "abc");

CmdPar p = CmdPar();

p.usage(OUT(), "@proto \rtypelist");
EXPECT_OUT("|List_t typelist ()|\n");
p.usage(OUT(), "@proto \rType_t::info ");
EXPECT_OUT("|void Type_t::info (str mode = NULL)|\n");
p.usage(OUT(), IN("Usage\n"));
EXPECT_OUT("Usage\n");

p = CmdPar("test");
TEST_FILE("test.hlp", "HELP\n");
p.manpage(OUT());
EXPECT_OUT("HELP\n");
p.read(IN("@ENV|\no:name|Opt\n:a|Arg\n"));
p.write(OUT());
EXPECT_OUT("Arg = NULL\nOpt = NULL\n\n@ENV|\no:name|Opt\n:a|Arg\n");
EXPECT(p.eval({ "run-name", "-o", "x", "y" }) == { "run-name" });
EXPECT_STR(p.get(NULL), "test");
EXPECT_STR(p.get("Opt"), "x");
EXPECT_STR(p.get("Arg"), "y");
p.set("Res", "true");
EXPECT(p.list() == { "Arg", "Opt", "Res" });
EXPECT_STR(p.psub("{Arg}:$1", 42), "y:42");
EXPECT_STR(p.psub(NULL), NULL);

p = CmdPar("test");
p.read(IN("o|\n*args|\n"));
p.write(OUT());
EXPECT_OUT("\no|\n*args|\n");
EXPECT(p.eval({ "test", "-o", "a", "-o", "x" }) == { "test", "a", "x", });
EXPECT(p.xeval({ "test", "-o", "a", "-o", "x" }) == { "test", "a", "-o", "x", });

p = CmdPar("test");
TEST_FILE("test.cnf", "Parameter = 5\n");
p.load("test");
p.write(OUT());
EXPECT_OUT("Parameter = \"5\"\n\n");

copy(psubfilter(IN("-$1-"), "a"), OUT());
EXPECT_OUT("-a-");

/* Sonstige Funktionen */

TEST_FORK
{
	ProgDir = "dir";
	setenv("PATH", "/usr/bin");
	EXPECT_STR(mkpath("test"), "dir:/usr/test");
	TEST_EXIT;
}

CHILD_EXIT(0);

UnitTest::stderr("stderr");
message("Message $1", "Test");
EXPECT_CONTENT("stderr", "Message Test\n");
UnitTest::stderr("stderr");
ionull.debug("Debug");
EXPECT_CONTENT("stderr", "Debug\n");
UnitTest::stderr("stderr");

TEST_FORK
{
	error("Error $1", "Test");
	TEST_EXIT;
}

CHILD_EXIT(1);
EXPECT_CONTENT("stderr", "Error Test\n");

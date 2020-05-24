#!/usr/bin/env .build/script.tst

ArgList argl = NULL;
EXPECT_REPR(argl, "NULL");
argl = "test";
EXPECT_REPR(argl, "ArgList: test");
argl += 5;
EXPECT_REPR(argl, "ArgList: test \"5\"");
EXPECT_STR(argl[1], "5");
argl = ArgList("name", "first", "second");
EXPECT_REPR(argl, "ArgList: name \"first\" \"second\"");

CmdPar p = CmdPar();

p.usage(OUT(), "@proto \rtypelist");
EXPECT_OUT("|List_t typelist ()|\n");
p.usage(OUT(), "@proto \rType_t::info ");
EXPECT_OUT("|void Type_t::info (str mode = NULL)|\n");

TEST_FORK
{
	ProgDir = "dir";
	setenv("PATH", "/usr/bin");
	EXPECT_STR(mkpath("test"), "dir:/usr/test");
	TEST_EXIT;
}

CHILD_EXIT(0);

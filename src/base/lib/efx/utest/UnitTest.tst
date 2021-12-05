#!/usr/bin/env esh
loadlib(".build/UnitTest.gcov.so", "UnitTest_setup");

PRINT_LABEL("lock and coverage");
loadlib(".build/UnitTest.gcov.so", "UnitTest_setup");

EXPECT(true);
ASSERT(true);
MATCH_STR(UnitTest::repr(3.5), ".");
EXPECT_REPR(3.5, "3.5000");

EXPECT(UnitTest::dcmp(2./3., 0.666666666));
EXPECT_DBL(1./3., 0.333333333);

PRINT_LABEL("temporary files");
EXPECT(UnitTest::tpl("@/x") == UnitTest::tmp("x"));
TEST_DIR("sub");
TEST_FILE("sub/a", "Content\n");
EXPECT_CONTENT("sub/a", "Content\n");
MATCH_CONTENT("sub/a", "C.*\n");

EXPECT(UnitTest::tmp("sub/b") == UnitTest::mkfile("sub/b", "Another File\n"));
EXPECT_CONTENT("sub/b", "Another File\n");

TEST_FORK
{
	UnitTest::tstout("tstout");
	UnitTest::info("ID", "fmt: %d", 42);
	UnitTest::expr("ID", "expr");
	UnitTest::strinfo("ID", "content", "var");
	TEST_EXIT;
}

CHILD_EXIT(0);
EXPECT_CONTENT("tstout", "\
.\tINFO\tID\tfmt: 42\n\
.\tEXPR\tID\texpr\n\
.\tINFO\tID\tvar -> \"content\"\n\
");

TEST_EXPR(str tmp = UnitTest::tmp());
TEST_EXPR(UnitTest::tmpclean());
EXPECT(tmp != UnitTest::tmp());

TEST_FILE("stdin", "Input\n");
UnitTest::stdin("stdin");
EXPECT_STR(fgets(iostd), "Input");
UnitTest::stdin();

UnitTest::stdout("stdout");
EXPECT(printf("message\n"));
UnitTest::stdout();
EXPECT_CONTENT("stdout", "message\n");

UnitTest::stderr("stderr");
EXPECT(fprintf(ioerr, "error message\n"));
UnitTest::stderr();
EXPECT_CONTENT("stderr", "error message\n");

puts("Data\n", OUT());
EXPECT_OUT("Data\n");
puts("Other\n", OUT());
MATCH_OUT("O.*\n");

EXPECT_IOTEST(NULL, "NULL");
EXPECT_IOTEST("a", "\"a\"");

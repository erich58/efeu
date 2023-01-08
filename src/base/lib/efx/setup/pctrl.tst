#!/bin/sh
#if 0
export LANG=C
exec .build/script.tst $0 arg
#endif

PRINT(getpid());
PRINT(getppid());

UnitTest::stderr("stderr");

TEST_FORK
{
	atexit([fprintf(ioerr, "FIRST\n");]);
	atexit([fprintf(ioerr, "LAST\n");]);
	exit(1);
}

CHILD_EXIT(1);
EXPECT_CONTENT("stderr", "LAST\nFIRST\n");
UnitTest::stderr();

EXPECT(system("false") == 1);
EXPECT(system("echo", "test", ">", UnitTest::tmp("output")) == 0);
EXPECT_CONTENT("output", "test\n");

EXPECT(rename(UnitTest::tmp("output"), UnitTest::tmp("copy")));
EXPECT_CONTENT("output", NULL);
EXPECT_CONTENT("copy", "test\n");
EXPECT(rename(UnitTest::tmp("output"), UnitTest::tmp("copy")) == false);
EXPECT(remove(UnitTest::tmp("copy")));
EXPECT_CONTENT("copy", NULL);

EXPECT(remove(UnitTest::tmp("copy")) == false);
UnitTest::stderr("stderr");
TEST_EXPR(perror("Test"));
UnitTest::stderr();
EXPECT_CONTENT("stderr", "Test: No such file or directory\n");

UnitTest::stderr("stderr");
memstat("memstat");
UnitTest::stderr();
MATCH_CONTENT("stderr", "^memstat: ");

UnitTest::stderr("stderr");
allocstat("allocstat");
UnitTest::stderr();
MATCH_CONTENT("stderr", "^allocstat: ");

// Nur coverage, Test in Baisbibltiothek efm 
UnitTest::stderr("stderr");
memstat();
memcheck();
UnitTest::stderr();
MATCH_CONTENT("stderr", ".");
AllocTab_stat(OUT());
MATCH_OUT(".");

UnitTest::stderr("stderr");

TEST_FORK
{
	str t1 = newtemp(NULL, "newtemp_");
	MATCH_STR(t1, "newtemp_");
	tempstat();
	EXPECT(deltemp(t1));
	EXPECT(deltemp(t1) == false);

	str d1 = newtempdir(NULL, "newtempdir_");
	MATCH_STR(d1, "newtempdir_");
	tempstat();
	EXPECT(deltempdir(d1));
	EXPECT(deltempdir(d1) == false);
	TEST_EXIT;
}

CHILD_EXIT(0);
UnitTest::stderr();
MATCH_CONTENT("stderr", string !
0: file .*newtemp_.*
deltemp: unknown path .*newtemp_.*
0: dir .*newtempdir_.*
deltemp: unknown path .*newtempdir_.*
!);

EXPECT_STR(ExpandPath("~"), getenv("HOME"));

TEST_FORK
{
	EXPECT(setenv("Test_Environ", "Test", true));
	EXPECT_STR(getenv("Test_Environ"), "Test");
	EXPECT(setenv("Test_Environ", "New Value", false));
	EXPECT_STR(getenv("Test_Environ"), "Test");
	EXPECT(setenv("Test_Environ", NULL, true));
	EXPECT_STR(getenv("Test_Environ"), NULL);
	EXPECT_STR(getenv("Test_Environ", "Default"), "Default");
	TEST_EXIT;
}

CHILD_EXIT(0);

EXPECT_STR(dirname("dir/name/"), "dir");
EXPECT_STR(dirname("dir/name/", true), "dir/");
str ext;
EXPECT_STR(basename("dir/name.ext"), "name.ext");
EXPECT_STR(basename("dir/name.ext", ext), "name");
EXPECT_STR(ext, "ext");
EXPECT_STR(filename("dir/name.ext"), "name");
EXPECT_STR(filetype("dir/name.ext"), "ext");

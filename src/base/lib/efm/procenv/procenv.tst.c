#include <EFEU/UnitTest.h>
#include <EFEU/mkpath.h>
#include <EFEU/mstring.h>
#include <EFEU/Resource.h>

TEST_DECL(mkpath)
{
	STRBUF(buf, 256);
	EXPECT(mkpath_bin(&buf, "bin", "lib", "else", NULL));
	EXPECT_STR(sb_nul(&buf), "lib:else");
	sb_clean(&buf);
	EXPECT(!mkpath_bin(&buf, "x", "lib", "else", NULL));
	EXPECT_STR(sb_nul(&buf), "");
	EXPECT(!mkpath_bin(&buf, "bin32", "lib", "else", NULL));
	EXPECT_STR(sb_nul(&buf), "");
	EXPECT(mkpath_bin(&buf, "x/bin", "lib", "else", NULL));
	EXPECT_STR(sb_nul(&buf), "x/lib:x/else");

	char *path = mstrcpy(getenv("PATH")); /* Save PATH */
	TEST_EXPR(setenv("PATH", "a/bin:b/bin", 1));

	sb_clean(&buf);
	TEST_EXPR(mkpath_base(&buf, "lib", "else", NULL));
	EXPECT_STR(sb_nul(&buf), "a/lib:a/else:b/lib:b/else");

	char *p;
	TEST_EXPR(setenv("TESTENV", "x:y", 1));
	TEST_EXPR(p = mkpath("z/bin", "TESTENV", "s", "t", NULL));
	EXPECT_STR(p, "z/s:z/t:x:y:a/s:a/t:b/s:b/t");

	memfree(p);
	TEST_EXPR(setenv("HOME", "/home/test", 1));
	TEST_EXPR(p = ExpandPath("/bin:~/bin:~root/bin"));
	EXPECT_STR(p, "/bin:/home/test/bin:/root/bin");

	setenv("PATH", path, 1);	/* Restore PATH */
}

static int c_flags = 0;

static void c_init()
{
	c_flags |= 1;
}

static void c_exit()
{
	c_flags |= 2;
}

TEST_DECL(callproc)
{
	Shell = "true";
	EXPECT(callproc(NULL) == 0);
	Shell = NULL;
	EXPECT(callproc(NULL) == -1);
	callproc_init = c_init;
	callproc_exit = c_exit;
	EXPECT(callproc("true") == 0);
	EXPECT(c_flags == 3);

	EXPECT(callproc("exit 5") == 5);
	EXPECT(callproc("kill -15 $$") == 128 + 15);

	UnitTest_stderr("");
	EXPECT(callproc(UnitTest_tpl("@/x.sh")) == 127);
	TEST_FILE("x.sh", "kill -2 $$\n");
	EXPECT(callproc(UnitTest_tpl("@/x.sh")) == 126);
	EXPECT(callproc(UnitTest_tpl("sh @/x.sh")) == 128 + 2);
	PRINT_INT(callproc(UnitTest_tpl("sh @/x.sh")));
	EXPECT(callproc(UnitTest_tpl("chmod a+x @/x.sh")) == 0);
	EXPECT(callproc(UnitTest_tpl("@/x.sh")) == 128 + 2);
	PRINT_INT(callproc(UnitTest_tpl("@/x.sh")));
}

TEST_DECL(Resource)
{
	EXPECT(GetResource("Version", NULL) == NULL);
	SetVersion("2.5");
	EXPECT_STR(GetResource("Version", NULL), "2.5");
	SetVersion("$ 4.8 $");
	EXPECT_STR(GetResource("Version", NULL), "4.8");
	SetResource("IntValue", "42");
	EXPECT(GetIntResource("IntValue", 0) == 42);

	EXPECT_STR(GetResource("Undefined", "Default"), "Default");
	EXPECT_STR(GetResource(NULL, "Default"), "Default");
	EXPECT(GetIntResource(NULL, 123) == 123);

	EXPECT(GetFlagResource("Flag") == 0);
	SetResource("Flag", "1");
	EXPECT(GetFlagResource("Flag") == 1);
	SetResource("Flag", "false");
	EXPECT(GetFlagResource("Flag") == 0);
	SetResource("Flag", "true");
	EXPECT(GetFlagResource("Flag") == 1);
	SetResource("Flag", "");
	EXPECT(GetFlagResource("Flag") == 0);
	SetResource("Flag", "T");
	EXPECT(GetFlagResource("Flag") == 1);

	TEST_FILE("dummy.cnf",
		"o|Option = TEST\n\tOption\n"
		"*args|\n\tArgumente\n"
		);

	TEST_FORK
	{
		char *cmd = mstrcpy(UnitTest_tmpname("dummy"));
		char *args[] = { cmd, "-o", "arg", NULL };
		int nargs = 3;
		SetApplPath(UnitTest_tmpname(NULL));
		ParseCommand(&nargs, args);
		EXPECT(nargs == 2);
		EXPECT_STR(GetResource("Option", NULL), "TEST");
		EXPECT_STR(args[1], "arg");
		TEST_EXIT;
	}

	CHILD_EXIT(0);

	TEST_FORK
	{
		char *cmd = mstrcpy(UnitTest_tmpname("dummy"));
		char *args[] = { cmd, "-x", "bad", NULL };
		int nargs = 3;
		setenv("LANG", "C", 1);
		freopen(UnitTest_tmpname("err.log"), "w", stderr);
		SetApplPath(UnitTest_tmpname(NULL));
		ParseCommand(&nargs, args);
		TEST_EXIT;
	}

	CHILD_EXIT(1);
	PRINT_CONTENT("err.log");
}

TEST_DECL(Lang)
{
	EXPECT_STR(CurrentLocale, NULL);
	ChangeLocale("C");
	EXPECT_STR(CurrentLocale, "C");
	ChangeLocale("C");
	ChangeLocale(NULL);
	EXPECT_STR(CurrentLocale, "C");
}

TEST_SUITE
{
	TEST_RUN(mkpath);
	TEST_RUN(callproc);
	TEST_RUN(Resource);
	TEST_RUN(Lang);
}

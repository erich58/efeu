#include <EFEU/UnitTest.h>
#include <stdlib.h>
#include <dirent.h>

TEST_DECL(bad_path)
{
	EXPECT_STR(UnitTest_mkfile(".", "Data"), NULL);
}

TEST_SUITE
{
	TEST_FORK
	{
		PRINT_LABEL("mktemp error");
		UnitTest_stderr("/dev/null");
		setenv("TMPDIR", "./bad/dir/name", 1);
		UnitTest_tmp(NULL);
		exit(0);
	}

	CHILD_EXIT(1);

	char *tmp = strdup(UnitTest_tmp(NULL));
	PRINT_STR(UnitTest_tmp(NULL));
	EXPECT_STRCMP(tmp, ==, UnitTest_tmp(NULL));
	UnitTest_tmpclean();

	TEST_FORK
	{
		PRINT_STR(UnitTest_tmp(NULL));
		EXPECT_STRCMP(tmp, !=, UnitTest_tmp(NULL));
		TEST_EXIT;
	}

	CHILD_EXIT(0);
	PRINT_STR(UnitTest_tmp(NULL));
	EXPECT_STRCMP(tmp, !=, UnitTest_tmp(NULL));

	TEST_FILE(NULL, NULL);
	TEST_FILE("x", "Test\n");
	EXPECT_CONTENT("x", "Test\n");
	TEST_FILE("x", NULL);
	EXPECT_CONTENT("x", NULL);

	EXPECT_STR(UnitTest_mkdir(NULL), UnitTest_tmp(NULL));
	EXPECT_STR(UnitTest_mkdir("/dir"), UnitTest_tmp("dir"));
	EXPECT_STR(UnitTest_mkdir("dir"), UnitTest_tmp("dir"));
	TEST_FILE("dir/file", "Testdaten\n" "Neue Zeile\n");

	EXPECT_CONTENT("dir/file", "Testdaten\n" "Neue Zeile\n");
	MATCH_CONTENT("dir/file", "^Test");
	EXPECT(UnitTest_content(NULL) != NULL);
	PRINT_STR(UnitTest_content(NULL));

	TEST_FORK
	{
		const char *res = "X\n";
		MATCH_CONTENT("dir/file", "^X");
		EXPECT_CONTENT("dir/file", res);
		TEST_EXIT;
	}

	CHILD_EXIT(1);

//	UnitTest_stderr("stderr");
	TEST_EXEC(bad_path);
//	UnitTest_stderr(NULL);
//	PRINT_CONTENT("stderr");

	EXPECT_CONTENT("dir/x", NULL);
	EXPECT_CONTENT(NULL, NULL);
	PRINT_STR(UnitTest_content(NULL));
	EXPECT_STR(UnitTest_tpl(NULL), NULL);
	EXPECT_STR(UnitTest_tpl("@"), UnitTest_tmp(NULL));
	EXPECT_STR(UnitTest_tpl("@/x\\@bc"), UnitTest_tmp("x@bc"));
	EXPECT_STR(UnitTest_tpl("abc"), "abc");
	EXPECT_STR(UnitTest_tpl("abc\\"), "abc");
	EXPECT_STR(UnitTest_tpl("abc\\x"), "abcx");

	DIR *dir = opendir(UnitTest_tmp("dir"));

	if	(EXPECT(dir != NULL))
	{
		struct dirent *entry;

		while ((entry = readdir(dir)))
		{
			if	(entry->d_name[0] != '.')
				EXPECT_STR(entry->d_name, "file");
		}

		EXPECT(closedir(dir) == 0);
	}

	TEST_FILE("dir/file", NULL);
	EXPECT_CONTENT("dir/file", NULL);
	TEST_FILE("dir/a/b/c/f", "Data");
	EXPECT_CONTENT("dir/a/b/c/f", "Data");

	EXPECT_STR(UnitTest_mkcmd("cmd", "#!/bin/sh\necho $1\n"),
		UnitTest_tmp("bin/cmd"));
	EXPECT_STR(UnitTest_pgets("cmd arg",EOF), "arg\n");
}

#include <EFEU/UnitTest.h>
#include <stdlib.h>
#include <dirent.h>

TEST_DECL(bad_path)
{
	freopen("/dev/null", "w", stderr);
	EXPECT_STR(UnitTest_mkfile("x/sub", NULL), NULL);
}

TEST_SUITE
{
	TEST_FORK
	{
		PRINT_LABEL("mktemp error");
		freopen("/dev/null", "w", stderr);
		setenv("TMPDIR", "./bad/dir/name", 1);
		UnitTest_tmpname(NULL);
		exit(0);
	}

	CHILD_EXIT(1);

	TEST_DIR("dir");
	TEST_FILE("dir/file", "Testdaten\n");

	EXPECT_CONTENT("dir/file", "Testdaten\n");
	MATCH_CONTENT("dir/file", "^Test");
	EXPECT_TRUE(UnitTest_content(NULL) != NULL);
	PRINT_STR(UnitTest_content(NULL));

	TEST_FORK
	{
		const char *res = "X\n";
		MATCH_CONTENT("dir/file", "^X");
		EXPECT_CONTENT("dir/file", res);
		TEST_EXIT;
	}

	CHILD_EXIT(1);

	TEST_EXEC(bad_path);
	EXPECT_CONTENT("dir/x", NULL);
	EXPECT_FALSE(UnitTest_content(NULL));
	PRINT_STR(UnitTest_content(NULL));
	EXPECT_STR(UnitTest_tpl(NULL), NULL);
	EXPECT_STR(UnitTest_tpl("@"), UnitTest_tmpname(NULL));
	EXPECT_STR(UnitTest_tpl("@/x\\@bc"), UnitTest_tmpname("x@bc"));
	EXPECT_STR(UnitTest_tpl("abc"), "abc");
	EXPECT_STR(UnitTest_tpl("abc\\"), "abc");
	EXPECT_STR(UnitTest_tpl("abc\\x"), "abcx");

	DIR *dir = opendir(UnitTest_tmpname("dir"));

	if	(EXPECT_TRUE(dir != NULL))
	{
		struct dirent *entry;

		while ((entry = readdir(dir)))
		{
			if	(entry->d_name[0] != '.')
				EXPECT_STR(entry->d_name, "file");
		}

		EXPECT_TRUE(closedir(dir) == 0);
	}
}

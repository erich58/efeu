#include <EFEU/UnitTest.h>
#include <dirent.h>

TEST_SUITE
{
	TEST_DIR("dir");
	TEST_FILE("dir/file", "Testdaten\n");

	TEST_CONTENT("dir/file", "Testdaten\n");
	MATCH_CONTENT("dir/file", "^Test");
	EXPECT_TRUE(UnitTest_content(NULL) != NULL);
	PRINT_STR(UnitTest_content(NULL));

	TEST_CONTENT("dir/x", NULL);
	EXPECT_FALSE(UnitTest_content(NULL));
	PRINT_STR(UnitTest_content(NULL));

	DIR *dir = opendir(UnitTest_tempname("dir"));

	if	(EXPECT_TRUE(dir != NULL))
	{
		struct dirent *entry;

		while ((entry = readdir(dir)))
		{
			if	(entry->d_name[0] != '.')
				EXPECT_STREQ(entry->d_name, "file");
		}

		EXPECT_TRUE(closedir(dir) == 0);
	}
}

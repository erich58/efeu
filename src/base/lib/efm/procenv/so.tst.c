#include <EFEU/UnitTest.h>
#include <EFEU/mkpath.h>
#include <EFEU/mstring.h>

TEST_SUITE
{
	IO *in = io_popen("efeu-config cc", "r");
	char *cc = io_getline(in, NULL, EOF);
	io_close(in);
	setenv("CC", cc, 1);

	TEST_FILE("sotest.c", "void tst_init(void);\nvoid tst_init(void) {}\n");

	TEST_FORK
	{
		EXPECT_TRUE(chdir(UnitTest_tmpname(NULL)) == 0);
		system("$CC -o libsotest.so -shared -nostdlib sotest.c");
		TEST_EXIT;
	}

	TEST_WAIT;
	TEST_EXPR(ProgDir = mstrcpy(UnitTest_tmpname(NULL)));
	EXPECT_FALSE(loadlib("undefined", "tst_init"));
	EXPECT_TRUE(loadlib("libsotest.so", NULL));
	EXPECT_TRUE(loadlib(UnitTest_tmpname("libsotest.so"), NULL));
	EXPECT_TRUE(loadlib("sotest", "tst_init"));
	EXPECT_FALSE(loadlib("sotest", "undefined"));

	void *handle;
	EXPECT_TRUE((handle = so_open("sotest")) != NULL);
	so_close(handle);
}

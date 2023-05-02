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
		EXPECT(chdir(UnitTest_tmp(NULL)) == 0);
		system("$CC -o libsotest.so -shared -nostdlib sotest.c");
		TEST_EXIT;
	}

	CHILD_EXIT(0);

	TEST_EXPR(ProgDir = mstrcpy(UnitTest_tmp(NULL)));
	EXPECT(!loadlib("undefined", "tst_init"));
	EXPECT(loadlib("libsotest.so", NULL));
	EXPECT(loadlib(UnitTest_tmp("libsotest.so"), NULL));
	EXPECT(loadlib("sotest", "tst_init"));
	UnitTest_stderr("stderr");
	EXPECT(loadlib("sotest", "undefined") == 0);
	MATCH_CONTENT("stderr", "^dlsym: ");

	void *handle;
	EXPECT((handle = so_open("sotest")) != NULL);
	so_close(handle);
}

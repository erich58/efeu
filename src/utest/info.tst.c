#include <EFEU/UnitTest.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <pty.h>

#define	STRING(a)	# a

TEST_SUITE
{
	UnitTest_info(__FILE__, __LINE__, "*GOOD", "");
	UnitTest_info(__FILE__, __LINE__, "!BAD", "");
	UnitTest_info(__FILE__, __LINE__, "?CHECK", "");

	TEST_FORK
	{
		char buf[1024];
		int master;
		int slave;
		setenv("TERM", "vt100", 1);
		ASSERT(openpty(&master, &slave, NULL, NULL, NULL) == 0);
		int fderr = dup(2);

		dup2(slave, 2);
		UnitTest_info(__FILE__, __LINE__, "*GOOD", "");
		dup2(fderr, 2);
		EXPECT(read(master, buf, sizeof buf) > 0);
		EXPECT_PTR(strstr(buf, "\033["));

		dup2(slave, 2);
		UnitTest_info(__FILE__, __LINE__, "!BAD", "");
		dup2(fderr, 2);
		EXPECT(read(master, buf, sizeof buf) > 0);
		EXPECT_PTR(strstr(buf, "\033["));

		dup2(slave, 2);
		UnitTest_info(__FILE__, __LINE__, "?CHECK", "");
		dup2(fderr, 2);
		EXPECT(read(master, buf, sizeof buf) > 0);
		EXPECT_PTR(strstr(buf, "\033["));

		close(slave);
		close(master);
		TEST_EXIT;
	}

	CHILD_EXIT(0);

	const char *str;
	PRINT_INT(6 * 7);
	PRINT_SIZE(-1);
	PRINT_SSIZE(~0ll);
	PRINT_LONG(1234567890123456789ll);
	PRINT_FMT("%.2f", 10. / 3.);
	PRINT_FMT("%llu", ~0);
	PRINT_FMT("%llu", ~0l);
	PRINT_FMT("%llu", ~0ll);
	PRINT_STR(0);
	TEST_EXPR(str = "Infostring");
	PRINT_STR(str);
	PRINT_PTR(str);
	TEST_EXPR(str = "String mit\nZeilenumbruch\n");
	PRINT_STR(str);
	PRINT_STR("Einfacher String");
	PRINT_STR("Zusammengesetzter " "String");
	PRINT_STR(NULL);

	TEST_FILE("output", NULL);
	UnitTest_strinfo("Test", 1, "INFO", "xy", STRING("x" "y"));
	UnitTest_strinfo("Test", 2, "INFO", "a\nb\n", NULL);
	UnitTest_strinfo("Test", 3, "INFO", "a\nb\n", "name");

	int fdsave = dup(2);
	dup2(creat(UnitTest_tmp("output"), 0644), 2);
	UnitTest_strinfo("Test", 1, "INFO", "xy", STRING("x" "y"));
	UnitTest_strinfo("Test", 2, "INFO", "a\nb\n", NULL);
	UnitTest_strinfo("Test", 3, "INFO", "a\nb\n", "name");
	dup2(fdsave, 2);

	EXPECT_CONTENT("output",
		"INFO\tTest:1\t\"x\"\n"
		"+\tTest:1\t\"y\"\n"
		"INFO\tTest:2\t\"a\\n\"\n"
		"+\tTest:2\t\"b\\n\"\n"
		"INFO\tTest:3\tname -> \n"
		"+\tTest:3\t\t\"a\\n\"\n"
		"+\tTest:3\t\t\"b\\n\"\n"
	);
}

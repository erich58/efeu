#include <EFEU/UnitTest.h>
#include <EFEU/appl.h>
#include <stdlib.h>
#include <EFEU/io.h>

static const char *appl_content(const char *name, int type)
{
	IO *appl = io_applfile (name, type);
	const char *content = io_getline(appl, NULL, EOF);
	io_close(appl);
	return content;
}

#define	APPL_FILE(name)	TEST_FILE(name, name)

TEST_SUITE
{
	TEST_DIR("lib");
	TEST_DIR("lib/config");
	TEST_DIR("lib/messages");
	TEST_DIR("lib/help");
	TEST_DIR("extra");
	APPL_FILE("lib/config/a.cnf");
	APPL_FILE("lib/config/b.cnf");
	APPL_FILE("lib/c.cnf");
	APPL_FILE("lib/messages/a.msg");
	APPL_FILE("lib/help/a.hlp");
	APPL_FILE("extra/a.cnf");
	APPL_FILE("extra/b.msg");

	setenv("APPLPATH", UnitTest_tmpname("lib/%S"), 1);
	SetApplPath(UnitTest_tmpname("extra"));
	PRINT_STR(ApplPath);

	EXPECT_STR(appl_content("a", APPL_CNF), "extra/a.cnf");
	EXPECT_STR(appl_content("a", APPL_MSG), "lib/messages/a.msg");
	EXPECT_STR(appl_content("a", APPL_HLP), "lib/help/a.hlp");
	EXPECT_STR(appl_content("b", APPL_CNF), "lib/config/b.cnf");
	EXPECT_STR(appl_content("b.cnf", APPL_CNF), "lib/config/b.cnf");
	EXPECT_STR(appl_content("b.cnf", 0), NULL);
	EXPECT_STR(appl_content("c", APPL_CNF), NULL);
	EXPECT_STR(appl_content("c.cnf", APPL_CNF), NULL);
	EXPECT_STR(appl_content("c.cnf", 0), "lib/c.cnf");
}

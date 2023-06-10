#include <UnitTest.h>

#define	UNEXPECT_FILE(name, cont) \
TEST_FORK { EXPECT_FILE(name, cont); TEST_EXIT; } CHILD_EXIT(1)

TEST_SUITE
{
	TEST_FILE("null", NULL);
	TEST_FILE("empty", "");
	TEST_FILE("short", "Testdaten");
	TEST_FILE("data", "Testdaten\n" "EOD");

	EXPECT_FILE(NULL, NULL);
	EXPECT_FILE("null", NULL);
	EXPECT_FILE("empty", "");
	EXPECT_FILE("short", "Testdaten");
	EXPECT_FILE("data", "Testdaten\n" "EOD");

	UNEXPECT_FILE("empty", NULL);
	UNEXPECT_FILE("short", NULL);
	UNEXPECT_FILE("data", NULL);

	UNEXPECT_FILE("null", "");
	UNEXPECT_FILE("short", "");
	UNEXPECT_FILE("data", "");

	const char *data = "Testdaten\nErwartete Daten\nEOD";
	UNEXPECT_FILE("null", data);
	UNEXPECT_FILE("empty", data);
	UNEXPECT_FILE("short", data);
	UNEXPECT_FILE("data", data);
}


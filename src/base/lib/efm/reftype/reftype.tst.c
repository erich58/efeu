#include <EFEU/UnitTest.h>
#include <EFEU/refdata.h>
#include <EFEU/LogConfig.h>
#include <EFEU/mstring.h>

static char *data = "DATA"; 
static int clean_count = 0;

static void clean (void *data)
{
	++clean_count;
}
 
TEST_DECL(data)
{
	void *rd;
	EXPECT_PTR(rd = rd_alloc(32));
	EXPECT(rd_ident(rd) == NULL);
	EXPECT(rd_deref(rd) == NULL);

	clean_count = 0;
	EXPECT_PTR(rd = rd_wrap(data, clean));
	EXPECT(rd_data(rd) == data);
	EXPECT_PTR(rd_refer(rd));
	EXPECT_PTR(rd_deref(rd));
	EXPECT(clean_count == 0);
	EXPECT(rd_deref(rd) == NULL);
	EXPECT(clean_count == 1);
}

static char *dummy_ident (const void *data) { return mstrcpy("ID"); }
static LogControl dummy_log = LOG_CONTROL("Dummy", LOGLEVEL_ERR);
static RefType dummy_reftype = REFTYPE_EXT("dummy", dummy_ident, NULL, &dummy_log);

TEST_DECL(logging)
{
	RefData rd;
	UnitTest_stderr("stderr");
	rd_init(&dummy_reftype, &rd);
	rd_refer(&rd);
	rd_refer(&rd);
	rd_deref(&rd);
	rd_deref(&rd);
	UnitTest_stderr(NULL);
	MATCH_CONTENT("stderr",
		"alloc\\(dummy 1 .*) ID\n"
#if	0
		"refer\\(dummy 1 .*) ID\n"
		"refer\\(dummy 2 .*) ID\n"
		"deref\\(dummy 2 .*) ID\n"
		"deref\\(dummy 1 .*) ID\n"
#endif
		);

	UnitTest_stderr("stderr");
	rd_debug(&rd, "message");
	MATCH_CONTENT("stderr", "debug\\(dummy 1 .*) message\n");

	UnitTest_stderr("stderr");
	rd_log(&rd, "$0: $1", "s", "message");
	MATCH_CONTENT("stderr", "dummy\\(.* 1): message\n");

	UnitTest_stderr("stderr");
	rd_clean(&rd);
	MATCH_CONTENT("stderr", "clean\\(dummy 1 .*) ID\n");

	UnitTest_stderr("stderr");
	TEST_FORK
	{
		rd_refer(&rd);
		exit(0);
	}
	CHILD_EXIT(1);
	MATCH_CONTENT("stderr", "fatal error.*deleted object.*\n");

	UnitTest_stderr("stderr");
	TEST_FORK
	{
		rd.refmagic = 0;
		rd_deref(&rd);
		exit(0);
	}
	CHILD_SIGNALED(6);
	MATCH_CONTENT("stderr", "fatal error.*bad magic number.\n");

	TEST_EXPR(rd.reftype = NULL);
	EXPECT(rd_refer(&rd) == NULL);
	EXPECT(rd_refer(NULL) == NULL);

}

TEST_SUITE
{
	TEST_RUN(data);
	TEST_RUN(logging);
}

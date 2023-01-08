#include <EFEU/UnitTest.h>
#include <EFEU/LogConfig.h>
#include <EFEU/Debug.h>
#include <EFEU/procenv.h>
#include <EFEU/ftools.h>
#include <EFEU/io.h>
#include <ctype.h>

static LogControl logctrl = LOG_CONTROL("test", LOGLEVEL_DEBUG);

static void enable_log(const char *flags)
{
	char buf[64];
	TEST_FILE("log.out", NULL);
	snprintf(buf, sizeof buf, "test.debug=%s[%s]",
		UnitTest_tmpname("log.out"), flags);
	LogConfig(buf);
}

static const char *disable_log()
{
	LogConfig("");
	return UnitTest_content("log.out");
}

#define	MATCH_LOG(data)	\
UnitTest_linematch(__FILE__, __LINE__, disable_log(), data, "\"log.out\"")

TEST_DECL(open)
{
	IO *io;
	EXPECT((io = LogOpen(&logctrl)) == NULL);
	enable_log("time,type");
	log_timestamp("20200101120000");
	EXPECT((io = LogOpen(&logctrl)) != NULL);
	io_puts("first line\nlast line", io);
	io_close(io);
	disable_log();
	MATCH_LOG(
		"2020-01-01 12:00:00\\....... \\[test\\.debug] first line\n"
		"2020-01-01 12:00:00\\....... \\[test\\.debug] last line\n"
		);
	EXPECT((io = LogOpen(&logctrl)) == NULL);
}

TEST_SUITE
{
	TEST_RUN(open);
}

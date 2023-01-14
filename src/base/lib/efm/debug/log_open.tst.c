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
	EXPECT((io = log_open(&logctrl)) == NULL);
	enable_log("time,type");
	log_timestamp("20200101120000");
	EXPECT((io = log_open(&logctrl)) != NULL);
	io_puts("first line\nlast line", io);
	io_close(io);
	disable_log();
	MATCH_LOG(
		"2020-01-01 12:00:00\\....... \\[test\\.debug] first line\n"
		"2020-01-01 12:00:00\\....... \\[test\\.debug] last line\n"
		);
	EXPECT((io = log_open(&logctrl)) == NULL);
}

TEST_DECL(write)
{
	enable_log("time");
	log_timestamp("20200101120000");
	log_write(&logctrl, "TESTDATEN\n", 0);
	disable_log();
	MATCH_LOG("TESTDATEN\n");
}

TEST_DECL(open2)
{
	IO *io;
	EXPECT((io = log_open(&logctrl)) == NULL);
	enable_log("time,type");
	log_timestamp("20200101120000");
	EXPECT((io = log_open(&logctrl)) != NULL);
	io_puts("first line\n", io);

	for (int i = 0; i < 20; ++i)
		io_puts("abcdefghi ", io);

	io_putc('\n', io);

	for (int i = 0; i < 30; ++i)
		io_puts("1234567890", io);

	EXPECT(io_rewind(io) == EOF);
	io_puts("\nlast line", io);
	io_close(io);
	disable_log();
	/*
	MATCH_LOG(
		"2020-01-01 12:00:00\\....... \\[test\\.debug] first line\n"
		"2020-01-01 12:00:00\\....... \\[test\\.debug] last line\n"
	);
	*/
	MATCH_CONTENT("log.out",
		"2020-01-01 12:00:00\\....... \\[test\\.debug] first line\n"
		".*"
		"2020-01-01 12:00:00\\....... \\[test\\.debug] last line\n"
	);
	EXPECT((io = log_open(&logctrl)) == NULL);
}

TEST_SUITE
{
	TEST_RUN(open);
	TEST_RUN(write);
	TEST_RUN(open2);
}

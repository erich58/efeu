#include <EFEU/UnitTest.h>
#include <EFEU/LogConfig.h>
#include <EFEU/Debug.h>
#include <EFEU/procenv.h>
#include <EFEU/ftools.h>
#include <EFEU/io.h>
#include <ctype.h>

static LogControl logctrl = LOG_CONTROL("test", LOGLEVEL_DEBUG);

static void enable_log()
{
	TEST_FILE("log.out", NULL);
	log_timestamp("20200101120000");
	LogConfig(UnitTest_tpl("test.debug=@/log.out"));
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

	enable_log();
	EXPECT((io = log_open(&logctrl)) != NULL);
	io_puts("first line\nlast line", io);
	io_close(io);
	MATCH_LOG(
		"@fussy\n"
		"DBG first line\n"
		"DBG last line\n"
		);
	EXPECT((io = log_open(&logctrl)) == NULL);
}

TEST_DECL(write)
{
	enable_log();
	log_write(&logctrl, "TESTDATEN\n", 0, 10);
	MATCH_LOG("@fussy\nTESTDATEN\n");
}

TEST_DECL(open2)
{
	IO *io;
	EXPECT((io = log_open(&logctrl)) == NULL);
	enable_log();
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
	MATCH_LOG(
		"@fussy\n"
		"*\\[test] DBG [0-9]+\n"
		"*\\[test] DBG [abcdefghi ]+\n"
		"\\[test] DBG first line\n"
		"\\[test] DBG last line\n"
	);
	EXPECT((io = log_open(&logctrl)) == NULL);
}

TEST_SUITE
{
	TEST_RUN(open);
	TEST_RUN(write);
	TEST_RUN(open2);
}

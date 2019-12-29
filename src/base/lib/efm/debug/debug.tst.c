#include <EFEU/UnitTest.h>
#include <EFEU/LogConfig.h>
#include <EFEU/Debug.h>

static LogControl *nlog = NULL;
static LogControl *dlog = NULL;

static int check_stdlog(LogControl *log, int level, int active)
{
	if	(LogUpdate(log) != active)
		return 0;

	return (log->name == NULL && log->level == level && log->sync != 0);
}

static const char *config(const char *cfg)
{
	static char buf[16];
	size_t pos = 0;

	LogConfig(cfg);
	buf[pos++] = LogUpdate(ErrLog) ? 'E' : 'e';
	buf[pos++] = LogUpdate(WarnLog) ? 'W' : 'w';
	buf[pos++] = LogUpdate(NoteLog) ? 'N' : 'n';
	buf[pos++] = LogUpdate(InfoLog) ? 'I' : 'i';
	buf[pos++] = LogUpdate(DebugLog) ? 'D' : 'd';
	buf[pos++] = LogUpdate(TraceLog) ? 'T' : 't';
	buf[pos++] = '-';
	buf[pos++] = LogUpdate(nlog) ? 'N' : 'n';
	buf[pos++] = LogUpdate(dlog) ? 'D' : 'd';
	buf[pos] = 0;
	return buf;
}

TEST_DECL(setup)
{
	static LogControl LogData = LOG_CONTROL("X", LOGLEVEL_NOTE);
	nlog = &LogData;
	dlog = LogControl_create("X.debug");

	EXPECT_STREQ(rd_ident(nlog), "X.note");
	EXPECT_STREQ(rd_ident(dlog), "X.debug");

	LogUpdate(ErrLog);
	int sync = ErrLog->sync;
	EXPECT_STREQ(config(NULL), "EWNidt-Nd");
	EXPECT_TRUE(nlog->sync == sync);
	EXPECT_STREQ(config(""), "EWNidt-Nd");
	EXPECT_TRUE(nlog->sync == ++sync);
	EXPECT_STREQ(config(" "), "EWNidt-Nd");
	EXPECT_TRUE(dlog->sync == ++sync);
}

TEST_DECL(LogLevel)
{
	for (int i = 0; i <= LOGLEVEL_TRACE; i++)
		EXPECT_TRUE(LogLevelIndex(LogLevelName(i)) == i);

	EXPECT_STREQ(LogLevelName(LOGLEVEL_ERR), "err");
	EXPECT_STREQ(LogLevelName(LOGLEVEL_WARN), "warn");
	EXPECT_STREQ(LogLevelName(LOGLEVEL_NOTE), "note");
	EXPECT_STREQ(LogLevelName(LOGLEVEL_INFO), "info");
	EXPECT_STREQ(LogLevelName(LOGLEVEL_DEBUG), "debug");
	EXPECT_STREQ(LogLevelName(LOGLEVEL_TRACE), "trace");
	EXPECT_STREQ(LogLevelName(0), "none");
	EXPECT_STREQ(LogLevelName(99), "none");
	EXPECT_TRUE(LogLevelIndex("anything") == 0);
}

TEST_DECL(StdLog)
{
	EXPECT_STREQ(rd_ident(ErrLog), ".err");
	EXPECT_TRUE(check_stdlog(ErrLog, LOGLEVEL_ERR, 1));
	EXPECT_STREQ(rd_ident(WarnLog), ".warn");
	EXPECT_TRUE(check_stdlog(WarnLog, LOGLEVEL_WARN, 1));
	EXPECT_STREQ(rd_ident(NoteLog), ".note");
	EXPECT_TRUE(check_stdlog(NoteLog, LOGLEVEL_NOTE, 1));
	EXPECT_STREQ(rd_ident(InfoLog), ".info");
	EXPECT_TRUE(check_stdlog(InfoLog, LOGLEVEL_INFO, 0));
	EXPECT_STREQ(rd_ident(DebugLog), ".debug");
	EXPECT_TRUE(check_stdlog(DebugLog, LOGLEVEL_DEBUG, 0));
	EXPECT_STREQ(rd_ident(TraceLog), ".trace");
	EXPECT_TRUE(check_stdlog(TraceLog, LOGLEVEL_TRACE, 0));
}

TEST_DECL(LogControl)
{
	EXPECT_STREQ(config("+"), "EWNIdt-Nd");
	EXPECT_STREQ(config("+"), "EWNIDt-ND");
	EXPECT_STREQ(config("+"), "EWNIDT-ND");
	EXPECT_STREQ(config("+"), "EWNIDT-ND");
	EXPECT_STREQ(config("-"), "EWNIDt-ND");
	EXPECT_STREQ(config("-"), "EWNIdt-Nd");
	EXPECT_STREQ(config("-"), "EWNidt-Nd");
	EXPECT_STREQ(config("-"), "EWnidt-nd");
	EXPECT_STREQ(config("-"), "Ewnidt-nd");
	EXPECT_STREQ(config("-"), "Ewnidt-nd");
	EXPECT_STREQ(config(":"), "EWNidt-Nd");

	EXPECT_STREQ(config("@X.warn"), "EWNidt-Nd");
	EXPECT_STREQ(config("@X.debug"), "EWNidt-ND");
	EXPECT_STREQ(config("@X.note"), "EWNidt-Nd");
	EXPECT_STREQ(config("@X.info"), "EWNidt-Nd");
	EXPECT_STREQ(config("X.note"), "EWNidt-Nd");
	EXPECT_STREQ(config("~X.info"), "EWNidt-Nd");

	EXPECT_STREQ(config("@.debug"), "EWNIDt-ND");
}

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(LogLevel);
	TEST_RUN(StdLog);
	TEST_RUN(LogControl);
	rd_deref(nlog);
	rd_deref(dlog);
}

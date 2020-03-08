#include <EFEU/UnitTest.h>
#include <EFEU/LogConfig.h>
#include <EFEU/Debug.h>
#include <EFEU/procenv.h>
#include <EFEU/ftools.h>
#include <EFEU/io.h>
#include <ctype.h>

#include "utest/data.tst.h"

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

static const char *fconfig(const char *fmt, const char *name)
{
	char buf[256];
	remove(name);
	snprintf(buf, sizeof buf, fmt, name);
	return config(buf);
}

static const char *logid(LogControl *log)
{
	return cstr(rd_ident(log));
}

#if	0
static const char *loginfo(LogControl *log)
{
	if	(!log)
		return NULL;

	LogUpdate(log);
	IO *io = out();
	LogControlEntry *entry = log->entry;

	io_puts(log->name, io);
	io_putc(':', io);

	for (entry = log->entry; entry; entry = entry->next)
	{
		if	(entry->out)
		{
			io_putc(' ', io);
			io_puts(entry->out->name, io);
		}
	}

	return data();
}
#endif

TEST_DECL(setup)
{
	static LogControl LogData = LOG_CONTROL("X", LOGLEVEL_NOTE);
	nlog = &LogData;
	dlog = LogControl_create("X.debug");

	EXPECT_STR(logid(nlog), "X.note");
	EXPECT_STR(logid(dlog), "X.debug");

	LogUpdate(ErrLog);
	int sync = ErrLog->sync;
	EXPECT_STR(config(NULL), "EWNidt-Nd");
	EXPECT_TRUE(nlog->sync == sync);
	EXPECT_STR(config(""), "EWNidt-Nd");
	EXPECT_TRUE(nlog->sync == ++sync);
	EXPECT_STR(config(" "), "EWNidt-Nd");
	EXPECT_TRUE(dlog->sync == ++sync);
}

TEST_DECL(LogLevel)
{
	int i;

	for (i = 0; i <= LOGLEVEL_TRACE; i++)
		EXPECT_TRUE(LogLevelIndex(LogLevelName(i)) == i);

	EXPECT_STR(LogLevelName(LOGLEVEL_ERR), "err");
	EXPECT_STR(LogLevelName(LOGLEVEL_WARN), "warn");
	EXPECT_STR(LogLevelName(LOGLEVEL_NOTE), "note");
	EXPECT_STR(LogLevelName(LOGLEVEL_INFO), "info");
	EXPECT_STR(LogLevelName(LOGLEVEL_DEBUG), "debug");
	EXPECT_STR(LogLevelName(LOGLEVEL_TRACE), "trace");
	EXPECT_STR(LogLevelName(0), "none");
	EXPECT_STR(LogLevelName(99), "none");
	EXPECT_TRUE(LogLevelIndex("anything") == 0);
}

TEST_DECL(LogControl)
{
	EXPECT_STR(logid(ErrLog), ".err");
	EXPECT_TRUE(check_stdlog(ErrLog, LOGLEVEL_ERR, 1));
	EXPECT_STR(logid(WarnLog), ".warn");
	EXPECT_TRUE(check_stdlog(WarnLog, LOGLEVEL_WARN, 1));
	EXPECT_STR(logid(NoteLog), ".note");
	EXPECT_TRUE(check_stdlog(NoteLog, LOGLEVEL_NOTE, 1));
	EXPECT_STR(logid(InfoLog), ".info");
	EXPECT_TRUE(check_stdlog(InfoLog, LOGLEVEL_INFO, 0));
	EXPECT_STR(logid(DebugLog), ".debug");
	EXPECT_TRUE(check_stdlog(DebugLog, LOGLEVEL_DEBUG, 0));
	EXPECT_STR(logid(TraceLog), ".trace");
	EXPECT_TRUE(check_stdlog(TraceLog, LOGLEVEL_TRACE, 0));

	EXPECT_STR(config("+"), "EWNIdt-Nd");
	EXPECT_STR(config("+"), "EWNIDt-ND");
	EXPECT_STR(config("+"), "EWNIDT-ND");
	EXPECT_STR(config("+"), "EWNIDT-ND");
	EXPECT_STR(config("-"), "EWNIDt-ND");
	EXPECT_STR(config("-"), "EWNIdt-Nd");
	EXPECT_STR(config("-"), "EWNidt-Nd");
	EXPECT_STR(config("-"), "EWnidt-nd");
	EXPECT_STR(config("-"), "Ewnidt-nd");
	EXPECT_STR(config("-"), "Ewnidt-nd");
	EXPECT_STR(config(":"), "EWNidt-Nd");

	EXPECT_STR(config("@X.warn"), "EWNidt-Nd");
	EXPECT_STR(config("@X.debug"), "EWNidt-ND");
	EXPECT_STR(config("@X.note"), "EWNidt-Nd");
	EXPECT_STR(config("@X.info"), "EWNidt-Nd");
	EXPECT_STR(config("X.note"), "EWNidt-Nd");
	EXPECT_STR(config("~X.info"), "EWNidt-Nd");

	EXPECT_STR(config("@.debug"), "EWNIDt-ND");
}

static int match(const char *mask, const char *content)
{
	if	(!mask)		return content == NULL;
	if	(!content)	return 0;

	while (*mask != 0)
	{
		if	(*content == 0)
			return 0;
		else if	(*mask == '?')
			;
		else if	(*mask == '#' && isdigit(*content))
			;
		else if (*mask != *content)
			return 0;

		mask++;
		content++;
	}

	return 1;
}

static int log_check(const char *mask)
{
	const char *content = UnitTest_content("log");

	if	(!match(mask, content))
	{
		PRINT_STR(content);
		return 0;
	}

	return 1;
}

TEST_DECL(LogOutput)
{
	const char *logfile;
	IO *out;
	
	EXPECT_TRUE((logfile = UnitTest_tmpname("log")) != NULL);
	EXPECT_STR(config(""), "EWNidt-Nd");
	EXPECT_TRUE(LogOpen(dlog) == NULL);

	EXPECT_STR(fconfig("@ 'X'.debug=%s", logfile), "EWNidt-ND");
	EXPECT_TRUE(log_check(NULL));
	EXPECT_TRUE((out = LogOpen(dlog)) != NULL);
	io_puts("TEST\n", out);
	io_close(out);
	EXPECT_TRUE(log_check("TEST\n"));

	log_printf(dlog, "ABC\n");
	EXPECT_TRUE(log_check("TEST\nABC\n"));
	EXPECT_CONTENT("log", "TEST\nABC\n");

	EXPECT_STR(fconfig("@X.debug=%s[time]", logfile), "EWNidt-ND");
	log_printf(dlog, "X\n");
	EXPECT_TRUE(log_check("####-##-## ##:##:##.###### X\n"));

	EXPECT_STR(fconfig("X.debug=[utf8]%s", logfile), "EWNidt-ND");
	log_printf(dlog, "Ä\n");
	EXPECT_TRUE(log_check("Ä\n"));

	EXPECT_STR(fconfig("@X.debug=%s[time,latin9]", logfile), "EWNidt-ND");
	log_printf(dlog, "Ä\n");
	EXPECT_TRUE(log_check("####-##-## ##:##:##.###### \304\n"));

	EXPECT_STR(fconfig("@ =%s", logfile), "EWNidt-Nd");
	log_printf(NoteLog, "NOTE\n");
	va_list list;
	log_vprintf(nlog, "X\n", list);
	EXPECT_TRUE(log_check("NOTE\nX\n"));
}

TEST_DECL(LogOutputStat)
{
	TEST_FILE("stderr.out", NULL);

	TEST_FORK
	{
		freopen(UnitTest_tmpname("stderr.out"), "w", stderr);
		LogOutputStat();

		log_psub(ErrLog, "DEFAULT\n", NULL);
		LogConfig("=stderr");
		log_psub(ErrLog, "STDERR\n", NULL);
		
/*
#if	HAS_SYSLOG
		LogConfig("=syslog");
		log_psub(ErrLog, "SYSLOG\n", NULL);
#endif
*/
		LogConfig(UnitTest_tpl("='|cat > @/pipe.out'"));
		log_psub(ErrLog, "PIPE\n", NULL);

		LogConfig("=");
		log_psub(ErrLog, "Keine Ausgabe\n", NULL);

		LogConfig("@");
		log_psub(ErrLog, "END\n", NULL);
		TEST_EXIT;
	}

	CHILD_EXIT(0);
	EXPECT_CONTENT("stderr.out",
		"stderr 0\n"
#if	HAS_SYSLOG
		"syslog 0\n"
		"syslog.err 0\n"
		"syslog.warn 0\n"
		"syslog.notice 0\n"
		"syslog.info 0\n"
		"syslog.debug 0\n"
#endif
		"DEFAULT\n"
		"STDERR\n"
		"END\n"
		);
	EXPECT_CONTENT("pipe.out", "PIPE\n");
}

TEST_DECL(log_file)
{
	const char *logfile;
	FILE *file;
	
	EXPECT_TRUE((logfile = UnitTest_tmpname("log")) != NULL);
	EXPECT_STR(fconfig("@X.debug=%s[time]", logfile), "EWNidt-ND");
	EXPECT_TRUE((file = log_file(dlog)) != NULL);
	fprintf(file, "TEST\n");
	fileclose(file);
	usleep(1000); /* Wait for log child */
	EXPECT_TRUE(log_check("####-##-## ##:##:##.###### TEST\n"));
}

TEST_DECL(GetFormat)
{
	SetApplPath(UnitTest_tmpname(NULL));
	TEST_FILE("tst.msg", "1\tFehlermeldung\n");

	LogConfig("@:");
	LogConfig(UnitTest_tpl("msg.debug=@/msg.log"));
	TEST_FILE("msg.log", NULL);
	EXPECT_STR(GetFormat("ABC"), "ABC");
	EXPECT_STR(GetFormat("[]ABC"), "ABC");
	EXPECT_STR(GetFormat("[ubk:123]ABC"), "ABC");
	MATCH_CONTENT("msg.log", "key \"123\" not found");
	EXPECT_STR(GetFormat("[%res]ABC"), "ABC");
	PRINT_CONTENT("msg.log");
	EXPECT_STR(GetFormat("[tst:1]ABC"), "Fehlermeldung");

	SetResource("tstfmt","XYZ");
	EXPECT_STR(GetFormat("[%tstfmt]ABC"), "XYZ");

	IO *io;
	EXPECT_TRUE((io = io_fileopen(UnitTest_tmpname("tst.msg"), "r")) != NULL);
	FormatTabLoad(io, "tst2", EOF);
	EXPECT_STR(GetFormat("[tst2:1]ABC"), "Fehlermeldung");
	io_close(io);

	TEST_FILE("tst3.msg", "1\tABC\n2\t\"DEF\"");
	FormatTabShow(out(), "tst3", "$0-$1-$2\n");
	EXPECT_STR(data(), "tst3-1-ABC\ntst3-2-DEF\n");
}

TEST_DECL(stdlog)
{
	LogConfig("@:");
	LogConfig(UnitTest_tpl("msg.note=@/msg.log =@/log"));

	TEST_FILE("log", NULL);
	log_psub(WarnLog, NULL, NULL);
	EXPECT_CONTENT("log", "WARNING:\n");
	log_psubarg(WarnLog, "FMT", NULL, NULL, NULL);
	EXPECT_CONTENT("log", "WARNING:\nFMT\n");

	TEST_FILE("log", NULL);
	TEST_FILE("msg.log", NULL);
	log_psub(ErrLog, "[ubk:123]", NULL);
	EXPECT_CONTENT("log", "ERROR [ubk:123]:\n");
	MATCH_CONTENT("msg.log", "key \"123\" not found");

	TEST_FILE("log", NULL);
	log_note(NULL, "Meldung $1", "s", "ABC");
	EXPECT_CONTENT("log", "Meldung ABC\n");

	TEST_FILE("log", NULL);

	TEST_FORK
	{
		log_error(NULL, "Fehler $2 ($1)", "sd", "Text", 42);
		TEST_EXIT;
	}

	CHILD_EXIT(1);
	EXPECT_CONTENT("log", "Fehler 42 (Text)\n");
}

TEST_SUITE
{
	TEST_RUN(LogLevel);
	TEST_RUN(LogOutputStat);
	TEST_RUN(setup);
	TEST_RUN(LogControl);
	TEST_RUN(LogOutput);
	TEST_RUN(log_file);
	TEST_RUN(GetFormat);
	TEST_RUN(stdlog);
	rd_deref(nlog);
	rd_deref(dlog);

	AllocTab_stat(out(), NULL, 1);
	MATCH_STR(data(), "byte");
}

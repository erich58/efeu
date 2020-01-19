#include <EFEU/UnitTest.h>
#include <EFEU/LogConfig.h>
#include <EFEU/Debug.h>
#include <EFEU/procenv.h>
#include <EFEU/ftools.h>
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
	int i;

	for (i = 0; i <= LOGLEVEL_TRACE; i++)
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

TEST_DECL(LogControl)
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
	EXPECT_STREQ(config(""), "EWNidt-Nd");
	EXPECT_TRUE(LogOpen(dlog) == NULL);

	EXPECT_STREQ(fconfig("@ 'X'.debug=%s", logfile), "EWNidt-ND");
	EXPECT_TRUE(log_check(NULL));
	EXPECT_TRUE((out = LogOpen(dlog)) != NULL);
	io_puts("TEST\n", out);
	io_close(out);
	EXPECT_TRUE(log_check("TEST\n"));

	log_printf(dlog, "ABC\n");
	EXPECT_TRUE(log_check("TEST\nABC\n"));
	TEST_CONTENT("log", "TEST\nABC\n");

	EXPECT_STREQ(fconfig("@X.debug=%s[time]", logfile), "EWNidt-ND");
	log_printf(dlog, "X\n");
	EXPECT_TRUE(log_check("####-##-## ##:##:##.###### X\n"));

	EXPECT_STREQ(fconfig("X.debug=[utf8]%s", logfile), "EWNidt-ND");
	log_printf(dlog, "Ä\n");
	EXPECT_TRUE(log_check("Ä\n"));

	EXPECT_STREQ(fconfig("@X.debug=%s[time,latin9]", logfile), "EWNidt-ND");
	log_printf(dlog, "Ä\n");
	EXPECT_TRUE(log_check("####-##-## ##:##:##.###### \304\n"));

	EXPECT_STREQ(fconfig("@ =%s", logfile), "EWNidt-Nd");
	log_printf(NoteLog, "NOTE\n");
	va_list list;
	log_vprintf(nlog, "X\n", list);
	EXPECT_TRUE(log_check("NOTE\nX\n"));
}

TEST_DECL(log_file)
{
	const char *logfile;
	FILE *file;
	
	EXPECT_TRUE((logfile = UnitTest_tmpname("log")) != NULL);
	EXPECT_STREQ(fconfig("@X.debug=%s[time]", logfile), "EWNidt-ND");
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
	LogConfig(UnitTest_tpl("msg.note=@/msg.log"));
	TEST_FILE("msg.log", NULL);
	EXPECT_STREQ(GetFormat("ABC"), "ABC");
	EXPECT_STREQ(GetFormat("[]ABC"), "ABC");
	EXPECT_STREQ(GetFormat("[ubk:123]ABC"), "ABC");
	MATCH_CONTENT("msg.log", "key \"123\" not found");
	EXPECT_STREQ(GetFormat("[%res]ABC"), "ABC");
	PRINT_CONTENT("msg.log");
	EXPECT_STREQ(GetFormat("[tst:1]ABC"), "Fehlermeldung");

	SetResource("tstfmt","XYZ");
	EXPECT_STREQ(GetFormat("[%tstfmt]ABC"), "XYZ");
}

TEST_DECL(stdlog)
{
	LogConfig("@:");
	LogConfig(UnitTest_tpl("msg.note=@/msg.log =@/log"));

	TEST_FILE("log", NULL);
	log_psub(WarnLog, NULL, NULL);
	TEST_CONTENT("log", "WARNING:\n");

	TEST_FILE("log", NULL);
	TEST_FILE("msg.log", NULL);
	log_psub(ErrLog, "[ubk:123]", NULL);
	TEST_CONTENT("log", "ERROR [ubk:123]:\n");
	MATCH_CONTENT("msg.log", "key \"123\" not found");

	TEST_FILE("log", NULL);
	log_note(NULL, "Meldung $1", "s", "ABC");
	TEST_CONTENT("log", "Meldung ABC\n");

	TEST_FILE("log", NULL);

	TEST_FORK
	{
		log_error(NULL, "Fehler $2 ($1)", "sd", "Text", 42);
		TEST_EXIT;
	}

	CHILD_EXIT(1);
	TEST_CONTENT("log", "Fehler 42 (Text)\n");
}

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(LogLevel);
	TEST_RUN(LogControl);
	TEST_RUN(LogOutput);
	TEST_RUN(log_file);
	TEST_RUN(GetFormat);
	TEST_RUN(stdlog);
	rd_deref(nlog);
	rd_deref(dlog);

	AllocTab_stat(out(), NULL, 1);
	EXPECT_MATCH(data(), "byte");
}

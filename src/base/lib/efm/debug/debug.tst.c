#include <EFEU/UnitTest.h>
#include <EFEU/LogConfig.h>
#include <EFEU/Debug.h>
#include <EFEU/procenv.h>
#include <EFEU/ftools.h>
#include <EFEU/io.h>
#include <ctype.h>

#include "utest/out.tst.h"
#include "utest/mstr.tst.h"

#define	MATCH_LOG(log, data)	\
UnitTest_linematch(__FILE__, __LINE__, \
UnitTest_content(log), "@fussy\n" data, # log)

static LogControl *nlog = NULL;
static LogControl *dlog = NULL;

static int check_stdlog(LogControl *log, int level, int active)
{
	if	(LogUpdate(log) != active)	return 0;

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

TEST_DECL(setup)
{
	static LogControl LogData = LOG_CONTROL("X", LOGLEVEL_NOTE);
	nlog = &LogData;
	dlog = LogControl_create("X.debug");

	EXPECT_MSTR(rd_ident(nlog), "X.MSG");
	EXPECT_MSTR(rd_ident(dlog), "X.DBG");

	LogUpdate(ErrLog);
	int sync = ErrLog->sync;
	EXPECT_STR(config(NULL), "EWNidt-Nd");
	EXPECT(nlog->sync == sync);
	EXPECT_STR(config(""), "EWNidt-Nd");
	EXPECT(nlog->sync == ++sync);
	EXPECT_STR(config(" "), "EWNidt-Nd");
	EXPECT(dlog->sync == ++sync);
}

TEST_DECL(LogLevel)
{
	EXPECT(LogLevelIndex("err") == LOGLEVEL_ERR);
	EXPECT(LogLevelIndex("warn") == LOGLEVEL_WARN);
	EXPECT(LogLevelIndex("note") == LOGLEVEL_NOTE);
	EXPECT(LogLevelIndex("info") == LOGLEVEL_INFO);
	EXPECT(LogLevelIndex("debug") == LOGLEVEL_DEBUG);
	EXPECT(LogLevelIndex("trace") == LOGLEVEL_TRACE);
	EXPECT(LogLevelIndex("anything") == 0);

	EXPECT_STR(LogLevelName(LOGLEVEL_ERR), "ERR");
	EXPECT_STR(LogLevelName(LOGLEVEL_WARN), "WNG");
	EXPECT_STR(LogLevelName(LOGLEVEL_NOTE), "MSG");
	EXPECT_STR(LogLevelName(LOGLEVEL_INFO), "INF");
	EXPECT_STR(LogLevelName(LOGLEVEL_DEBUG), "DBG");
	EXPECT_STR(LogLevelName(LOGLEVEL_TRACE), "TRC");
	EXPECT_STR(LogLevelName(0), "XXX");
	EXPECT_STR(LogLevelName(99), "XXX");
}

TEST_DECL(create)
{
	LogControl *ctrl;

	ctrl = LogControl_create("name.info");
	EXPECT_MSTR(rd_ident(ctrl), "name.INF");
	rd_deref(ctrl);

	ctrl = LogControl_create(NULL);
	EXPECT_MSTR(rd_ident(ctrl), ".MSG");
	rd_deref(ctrl);
}

TEST_DECL(LogControl)
{
	EXPECT_MSTR(rd_ident(ErrLog), ".ERR");
	EXPECT(check_stdlog(ErrLog, LOGLEVEL_ERR, 1));
	EXPECT_MSTR(rd_ident(WarnLog), ".WNG");
	EXPECT(check_stdlog(WarnLog, LOGLEVEL_WARN, 1));
	EXPECT_MSTR(rd_ident(NoteLog), ".MSG");
	EXPECT(check_stdlog(NoteLog, LOGLEVEL_NOTE, 1));
	EXPECT_MSTR(rd_ident(InfoLog), ".INF");
	EXPECT(check_stdlog(InfoLog, LOGLEVEL_INFO, 0));
	EXPECT_MSTR(rd_ident(DebugLog), ".DBG");
	EXPECT(check_stdlog(DebugLog, LOGLEVEL_DEBUG, 0));
	EXPECT_MSTR(rd_ident(TraceLog), ".TRC");
	EXPECT(check_stdlog(TraceLog, LOGLEVEL_TRACE, 0));

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

TEST_DECL(LogOutput)
{
	const char *logfile;
	IO *out;
	
	EXPECT((logfile = UnitTest_tmpname("log")) != NULL);
	EXPECT_STR(config(""), "EWNidt-Nd");
	EXPECT(log_open(dlog) == NULL);

	EXPECT_STR(fconfig("@ 'X'.debug=%s", logfile), "EWNidt-ND");
	MATCH_LOG("log", "");
	EXPECT((out = log_open(dlog)) != NULL);
	io_puts("TEST\n", out);
	io_close(out);
	MATCH_LOG("log", "\\[X] DBG TEST\n");

	EXPECT_STR(fconfig("@X.debug=%s", logfile), "EWNidt-ND");
	log_printf(dlog, "X\n");
	MATCH_LOG("log", "\\[X] DBG X\n");

	EXPECT_STR(fconfig("@ =%s", logfile), "EWNidt-Nd");
	log_printf(NoteLog, "NOTE\n");
	va_list list;
	log_vprintf(nlog, "X\n", list);
	MATCH_LOG("log", "\\MSG NOTE\n\\[X] MSG X\n");
}

TEST_DECL(log_file)
{
	const char *logfile;
	FILE *file;
	
	EXPECT((logfile = UnitTest_tmpname("log")) != NULL);
	EXPECT_STR(fconfig("@X.debug=%s", logfile), "EWNidt-ND");
	EXPECT((file = log_file(dlog)) != NULL);

	if	(file)
	{
		fprintf(file, "first line\n");
		fprintf(file, "last line\n");
		fileclose(file);
	}

	usleep(1000); /* Wait for log child */
	MATCH_LOG("log",
		"\\[X] DBG first line\n"
		"\\[X] DBG last line\n"
	);
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
	MATCH_LOG("msg.log",
		"*WNG : message catalog \"ubk\" not found\n"
		"WNG : key \"123\" not found");
	EXPECT_STR(GetFormat("[%res]ABC"), "ABC");
	EXPECT_STR(GetFormat("[tst:1]ABC"), "Fehlermeldung");

	SetResource("tstfmt","XYZ");
	EXPECT_STR(GetFormat("[%tstfmt]ABC"), "XYZ");

	IO *io;
	EXPECT((io = io_fileopen(UnitTest_tmpname("tst.msg"), "r")) != NULL);
	FormatTabLoad(io, "tst2", EOF);
	EXPECT_STR(GetFormat("[tst2:1]ABC"), "Fehlermeldung");
	io_close(io);

	TEST_FILE("tst3.msg", "1\tABC\n2\t\"DEF\"");
	TEST_EXPR(FormatTabShow(OUT(), "tst3", "$0-$1-$2\n"));
	EXPECT_OUT("tst3-1-ABC\ntst3-2-DEF\n");
}

TEST_DECL(stdlog)
{
	LogConfig("@:");
	LogConfig(UnitTest_tpl("msg.note=@/msg.log =@/log"));

	TEST_FILE("log", NULL);
	log_psub(WarnLog, NULL, NULL);
	MATCH_LOG("log", " WNG\n");
	log_psubarg(WarnLog, "FMT", NULL, NULL, NULL);
	MATCH_LOG("log", 
		" WNG\n"
		" WNG FMT\n"
	);

	TEST_FILE("log", NULL);
	TEST_FILE("msg.log", NULL);
	log_psub(ErrLog, "[ubk:123]", NULL);
	MATCH_LOG("log", "ERR \\[ubk:123]\n");
	MATCH_LOG("msg.log",
		"*WNG : message catalog \"ubk\" not found\n"
		"WNG : key \"123\" not found\n");

	TEST_FILE("log", NULL);
	log_note(NULL, "Meldung $1", "s", "ABC");
	MATCH_LOG("log", "MSG Meldung ABC\n");

	TEST_FILE("log", NULL);

	TEST_FORK
	{
		TEST_EXPR(log_error(NULL, "Fehler $2 ($1)", "sd", "Text", 42));
		TEST_EXIT;
	}

	CHILD_EXIT(1);
	MATCH_LOG("log", "ERR Fehler 42 \\(Text)\n");
}

TEST_SUITE
{
	TEST_RUN(LogLevel);
	TEST_RUN(create);
	TEST_RUN(setup);
	TEST_RUN(LogControl);
	TEST_RUN(LogOutput);
	TEST_RUN(log_file);
	TEST_RUN(GetFormat);
	TEST_RUN(stdlog);
	rd_deref(nlog);
	rd_deref(dlog);

	TEST_EXPR(AllocTab_stat(OUT(), NULL, 1));
	MATCH_OUT("byte");
	CLEANUP_MSTR;
	CLEANUP_OUT;
}

#include <EFEU/extension.h>
#include <EFEU/UnitTest.h>
#include <EFEU/LogConfig.h>
#include <EFEU/Debug.h>

#if	HAS_SYSLOG

#include <syslog.h>

void syslog(int pri, const char *fmt, ...)
{
	fprintf(stderr, "[%d] ", pri);
	va_list list;
	va_start(list, fmt);
	vfprintf(stderr, fmt, list);
	va_end(list);
}

#endif

static void log_output(const char *name)
{
	LogOutput *out = NewLogOutput(name);

	if	(out)
		out->write(out, "PFX DATA\n", 4, 9);

	DelLogOutput(out);
}

TEST_SUITE
{
	UnitTest_stderr("err.out");
	log_output("stderr");
#if	HAS_SYSLOG
	log_output("syslog");
	log_output("syslog.err");
	log_output("syslog.warning");
	log_output("syslog.notice");
	log_output("syslog.info");
	log_output("syslog.debug");
#endif
	log_output(UnitTest_tmpname("log.out"));
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("err.out",
		"PFX DATA\n"
#if	HAS_SYSLOG
		"[3] DATA\n"
		"[3] DATA\n"
		"[4] DATA\n"
		"[5] DATA\n"
		"[6] DATA\n"
		"[7] DATA\n"
#endif
	);
	EXPECT_CONTENT("log.out",
		"PFX DATA\n"
	);
}

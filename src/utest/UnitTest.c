#include <EFEU/UnitTest.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

char *UnitTest_name = NULL;
static int count_good = 0;
static int count_bad = 0;
static int fork_depth = 0;

typedef void (*runFunction)(void);

/* Atomare Infozeile
*/

static void vinfo(const char *pfx, const char *file, int line,
	const char *fmt, va_list list)
{
	char buf[1024];
	size_t pos = 0;

	for (int n = 0; n < fork_depth; ++n)
		pos += snprintf(buf + pos, sizeof buf - pos + 1, ".\t");

	pos += snprintf(buf + pos, sizeof buf - pos - 1,
		"%s\t%s:%d\t", pfx, file, line);
	pos += vsnprintf(buf + pos, sizeof buf - pos - 1, fmt, list);

	if	(pos >= sizeof buf)
		pos = sizeof buf - 1;

	buf[pos++] = '\n';
	write(2, buf, pos);
}

void UnitTest_info(const char *file, int line, const char *fmt, ...)
{
	va_list list;

	va_start (list, fmt);
	vinfo("INFO", file, line, fmt, list);
	va_end(list);
}

int UnitTest_result(const char *file, int line, int res, const char *fmt, ...)
{
	va_list list;

	if	(res)
		count_good++;
	else
		count_bad++;

	va_start (list, fmt);
	vinfo(res ? "GOOD" : "BAD", file, line, fmt, list);
	va_end(list);
	return res;
}

int main(int argc, char **argv)
{
	char *p;

	if	((p = strrchr(argv[0], '/')))
		UnitTest_name = p + 1;
	else
		UnitTest_name = argv[0];

	fprintf(stderr, "%s\t%s\n", "#RUN", UnitTest_name);
	runUnitTest();
	printf("%s\t%d\t%d\t%d\t%s\n", UnitTest_name, count_good + count_bad,
		count_good, count_bad, count_bad ? "FAILURE" : "SUCCESS");
	return count_bad ? 1 : 0;
}

int UnitTest_wait(void)
{
	int status;
	pid_t pid = wait(&status);

	if	(pid < 0)
		return 0;

	if	(!WIFEXITED(status))
		return 0;

	return WEXITSTATUS(status) == 0;
}

int UnitTest_fork(const char *file, int line)
{
	pid_t pid = fork();

	if	(pid == 0)
	{
		count_good = 0;
		count_bad = 0;
		fork_depth++;
		return 1;
	}

	UnitTest_result(file, line, pid > 0, "fork");
	return 0;
}

void UnitTest_exit(void)
{
	exit(count_bad ? 1 : 0);
}

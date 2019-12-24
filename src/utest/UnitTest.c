#include <EFEU/UnitTest.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static int count_good = 0;
static int count_bad = 0;

typedef void (*runFunction)(void);

int UnitTest_result(const char *file, int line, int res, const char *fmt, ...)
{
	va_list list;

	if	(res)
	{
		count_good++;
	}
	else
	{
		count_bad++;
	}

	char buf[1024];
	va_start (list, fmt);
	vsnprintf(buf, sizeof buf, fmt, list);
	va_end(list);

	fprintf(stderr, "%s\t%s:%d\t%s\n",
		res ? "GOOD" : "BAD", file, line, buf);
	return res;
}

int UnitTest_summary(void)
{
	printf("good: %d\n", count_good);
	printf("bad:  %d\n", count_bad);
	return count_bad != 0;
}

int main(int argc, char **argv)
{
	runUnitTest();
	return UnitTest_summary();
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
		return 1;
	}

	UnitTest_result(file, line, pid > 0, "fork");
	return 0;
}

void UnitTest_exit(void)
{
	exit(count_bad ? 1 : 0);
}

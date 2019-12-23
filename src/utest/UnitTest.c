#include <EFEU/UnitTest.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

static int count_good = 0;
static int count_bad = 0;

typedef void (*runFunction)(void);

int UnitTest_result(const char *file, int line, int result,
	const char *fmt, ...)
{
	va_list list;

	if	(result)
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
		result ? "GOOD" : "BAD", file, line, buf);
	return result;
}

int UnitTest_status(void)
{
	printf("good: %d\n", count_good);
	printf("bad:  %d\n", count_bad);
	return count_bad != 0;
}

int main(int argc, char **argv)
{
	runUnitTest();
	return UnitTest_status();
}

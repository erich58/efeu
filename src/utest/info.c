#include <EFEU/UnitTest.h>
#include <stdio.h>
#include <stdarg.h>

void UnitTest_info(const char *file, int line, const char *fmt, ...)
{
	va_list list;

	fprintf(stderr, "INFO\t%s:%d\t", file, line);
	va_start (list, fmt);
	vfprintf(stderr, fmt, list);
	va_end(list);
	putc('\n', stderr);
}

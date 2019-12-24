#include <EFEU/UnitTest.h>
#include <stdio.h>
#include <stdlib.h>

UNIT_TEST
{
	const char *str = "Infostring";
	PRINT_INT(6 * 7);
	PRINT_SIZE(-1);
	PRINT_SSIZE(~0ll);
	PRINT_LONG(1234567890123456789ll);
	PRINT_FMT("%.2f", 10. / 3.);
	PRINT_FMT("%llu", ~0);
	PRINT_FMT("%llu", ~0l);
	PRINT_FMT("%llu", ~0ll);
	PRINT_STR(0);
	PRINT_STR(str);
	PRINT_PTR(str);
}

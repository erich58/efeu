#include <EFEU/UnitTest.h>

TEST_SUITE
{
	const char *str;
	PRINT_INT(6 * 7);
	PRINT_SIZE(-1);
	PRINT_SSIZE(~0ll);
	PRINT_LONG(1234567890123456789ll);
	PRINT_FMT("%.2f", 10. / 3.);
	PRINT_FMT("%llu", ~0);
	PRINT_FMT("%llu", ~0l);
	PRINT_FMT("%llu", ~0ll);
	PRINT_STR(0);
	TEST_EXPR(str = "Infostring");
	PRINT_STR(str);
	PRINT_PTR(str);
	TEST_EXPR(str = "String mit\nZeilenumbruch\n");
	PRINT_STR(str);
	PRINT_STR("Einfacher String");
	PRINT_STR("Zusammengesetzter " "String");
}

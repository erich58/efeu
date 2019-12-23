#include <EFEU/UnitTest.h>
#include <string.h>

int UnitTest_strcmp(const char *a, const char *b)
{
	if	(a == b)	return 0;
	if	(a == NULL)	return -1;
	if	(b == NULL)	return 1;

	return strcmp(a, b);
}


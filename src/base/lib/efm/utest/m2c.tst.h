/*	Konvertiert einen dynamisch generierten String
	auf einen konstanten String mit Speicherfreigabe
*/

#ifndef	_utest_m2c_out_h
#define	_utest_m2c_out_h	1

#include <EFEU/UnitTest.h>
#include <EFEU/memalloc.h>

static const char *M2C(char *s)
{
	if	(s)
	{
		const char *c = UnitTest_cstr(s);
		memfree(s);
		return c;
	}

	return NULL;
}

#endif

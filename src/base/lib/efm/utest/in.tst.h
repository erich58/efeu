/*	Test auf dynamisch generierten String mit Freigabe
*/

#ifndef	_utest_in_tst_h
#define	_utest_in_tst_h	1

#include <EFEU/UnitTest.h>
#include <EFEU/memalloc.h>

static IO *IN(char *def)
{
	static IO *io = NULL;
	io_close(io);
	io = def ? io_string(def, NULL) : NULL;
	return io;
}

#endif

/*	String-Eingabestruktur ohne Fehlerabbruch mit Speicherfreigabe
*/

#ifndef	_utest_in_tst_h
#define	_utest_in_tst_h	1

#include <EFEU/UnitTest.h>
#include <EFEU/memalloc.h>
#include <EFEU/ioctrl.h>

static int IN_get(IO *io)
{
	return io_getc(io->data);
}

static int IN_ctrl(IO *io, int req, va_list list)
{
	switch (req)
	{
	case IO_ERROR:	return 0;
	default:	return io_vctrl(io->data, req, list);
	}
}

static IO *IN_create(const char *def)
{
	IO *io = NULL;

	if	(def)
	{
		io = io_alloc();
		io->get = IN_get;
		io->ctrl = IN_ctrl;
		io->data = io_cstr(def);
	}

	return io;
}

static IO *IN(const char *def)
{
	static IO *io = NULL;
	io_close(io);
	io = IN_create(def);
	return io;
}

#define	EXPECT_IO2STR(func, input, val, rem)	\
do { \
	IO *in = IN_create(input); 	\
	const char *value = func(in); \
	UnitTest_strcheck(__FILE__, __LINE__, "==", value, val, \
		# func "(" # input ")", # val); \
	if	(rem)	{ \
		char remaining[32]; \
		io_gets(remaining, sizeof remaining, in); \
		EXPECT_STR(remaining, rem); \
	} \
	io_close(in); \
} while (0)


#define	CLEANUP_IN	IN(NULL)

#endif

/*	Dummy-Ein/Ausgabe
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/io.h>
#include <EFEU/ioctrl.h>

static int null_put (int c, void *ptr)
{
	return c;
}

static int null_ctrl (void *ptr, int c, va_list list)
{
	switch (c)
	{
	case IO_REWIND:	return 0;
	case IO_IDENT:	*va_arg(list, char **) = ptr; return 0;
	default:	return EOF;
	}
}

static io_t ios_null = STD_IODATA (NULL, null_put, null_ctrl, "<null>");

io_t *ionull = &ios_null;

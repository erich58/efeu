/*	Standard Ein-Ausgabe (Plain)
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/io.h>
#include <EFEU/ioctrl.h>

/*	Lesen von Standardeingabe
*/

static int std_get (void *ptr)
{
	return getchar();
}


/*	Zeichen nach Standardausgabe schreiben
*/

static int std_put (int c, void *ptr)
{
	return putchar(c);
}


/*	Zeichen nach Standardfehler schreiben
*/

static int err_put (int c, void *ptr)
{
	return putc(c, stderr);
}

static int std_ctrl (void *ptr, int c, va_list list)
{
	switch (c)
	{
	case IO_CLOSE:	return 0;
	case IO_IDENT:	*va_arg(list, char **) = ptr; return 0;
	default:	return EOF;
	}
}

static io_t ios_batch = STD_IODATA(std_get, std_put, std_ctrl, "<stdin,stdout>");
static io_t ios_err = STD_IODATA(NULL, err_put, std_ctrl, "<stderr>");

io_t *iostd = &ios_batch;
io_t *iomsg = &ios_batch;
io_t *ioerr = &ios_err;


/*	Protokollausgabe
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

	Version 0.4
*/

#include <EFEU/io.h>
#include <EFEU/ioctrl.h>

/*	Zeichen nach Standardfehler schreiben
*/

static int last_out = '\n';

static int log_put (int c, void *ptr)
{
	last_out = c;
	return putc(c, stderr);
}

static int log_ctrl (void *ptr, int c, va_list list)
{
	switch (c)
	{
	case IO_REWIND:
	case IO_FLUSH:
	case IO_CLOSE:

		if	(last_out != '\n')
			putc('\n', stderr);

		last_out = '\n';
		return 0;

	case IO_IDENT:

		*va_arg(list, char **) = ptr;
		return 0;

	default:

		return EOF;
	}
}

static io_t ios_log = IODATA (NULL, log_put, log_ctrl, "<stderr>");

io_t *iolog = &ios_log;


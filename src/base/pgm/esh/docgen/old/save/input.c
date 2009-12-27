/*	Eingabefilter
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include "eisdoc.h"
#include <EFEU/ioctrl.h>


/*	Eingabestruktur
*/

typedef struct {
	io_t *in;		/* Eingabestruktur */
	unsigned nl : 1;	/* Flag für neue Zeile */
	unsigned line : (8*sizeof(unsigned) - 1);
} IFILTER;


/*	Zeichen lesen
*/

static int ifilter_get(IFILTER *ifilter)
{
	int c;

	ifilter->line += ifilter->nl;
	c = io_getc(ifilter->in);
	ifilter->nl = (c == '\n' ? 1 : 0);
	return c;
}


/*	Kontrollfunktion
*/

static int ifilter_ctrl(IFILTER *ifilter, int req, va_list list)
{
	int stat;
	char *p;
	int n;

	switch (req)
	{
	case IO_CLOSE:

		stat = io_close(ifilter->in);
		memfree(ifilter);
		return stat;

	case IO_REWIND:

		if	(io_rewind(ifilter->in) == EOF)	return EOF;

		ifilter->nl = 0;
		ifilter->line = 1;
		return 0;
		
	case IO_IDENT:

		*va_arg(list, char **) = io_xident(ifilter->in,
			"ifilter(%*):%d", ifilter->line);
		return 0;

	case IO_LINE:

		return ifilter->line;

	case IO_ERROR:

		return 0;

	default:

		return io_vctrl(ifilter->in, req, list);
	}
}


io_t *InputFilter(io_t *io)
{
	if	(io)
	{
		IFILTER *ifilter = memalloc(sizeof(IFILTER));
		ifilter->in = io;
		ifilter->nl = 0;
		ifilter->line = 1;
		io = io_alloc();
		io->get = (io_get_t) ifilter_get;
		io->ctrl = (io_ctrl_t) ifilter_ctrl;
		io->data = ifilter;
	}

	return io;
}

/*	IO-Struktur mit Zeilenzähler
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/io.h>
#include <EFEU/ioctrl.h>


/*	Eingabestruktur
*/

typedef struct {
	io_t *io;		/* Eingabestruktur */
	unsigned nl : 1;	/* Flag für neue Zeile */
	unsigned line : (8*sizeof(unsigned) - 1);
} LNUM;


/*	Zeichen lesen
*/

static int lnum_get(LNUM *lnum)
{
	int c;

	lnum->line += lnum->nl;
	c = io_getc(lnum->io);
	lnum->nl = (c == '\n' ? 1 : 0);
	return c;
}


/*	Kontrollfunktion
*/

static int lnum_ctrl(LNUM *lnum, int req, va_list list)
{
	int stat;

	switch (req)
	{
	case IO_CLOSE:

		stat = io_close(lnum->io);
		memfree(lnum);
		return stat;

	case IO_REWIND:

		if	(io_rewind(lnum->io) == EOF)	return EOF;

		lnum->nl = 0;
		lnum->line = 1;
		return 0;
		
	case IO_IDENT:

		*va_arg(list, char **) = io_xident(lnum->io,
			"%*:%d", lnum->line);
		return 0;

	case IO_LINE:

		return lnum->line;

	default:

		return io_vctrl(lnum->io, req, list);
	}
}


io_t *io_lnum(io_t *io)
{
	if	(io && io_ctrl(io, IO_LINE) == EOF)
	{
		LNUM *lnum = memalloc(sizeof(LNUM));
		lnum->io = io;
		lnum->nl = 0;
		lnum->line = 1;
		io = io_alloc();
		io->get = (io_get_t) lnum_get;
		io->ctrl = (io_ctrl_t) lnum_ctrl;
		io->data = lnum;
	}

	return io;
}

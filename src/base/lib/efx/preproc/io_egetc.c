/*	Multizeilen zusasmmenfassen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/


#include <EFEU/efmain.h>
#include <EFEU/efio.h>
#include <EFEU/efutil.h>

static int subgetc (io_t *io, const char *delim);

int io_egetc(io_t *io, const char *delim)
{
	int c;

	while ((c = subgetc(io, delim)) == '/')
	{
		if (iocpy_cskip(io, NULL, c, NULL, 1) == EOF) break;
	}

	return c;
}


static int subgetc(io_t *io, const char *delim)
{
	int c;

	c = io_getc(io);

	while (c == '\\')
	{
		c = io_getc(io);

		if	(c != EOF && c != '\n' && listcmp(delim, c))
			return c;

		io_ungetc(c, io);
		return '\\';
	}

	if	(listcmp(delim, c))
	{
		io_ungetc(c, io);
		return EOF;
	}

	return c;
}

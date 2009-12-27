/*	String mit Markierung ausgeben
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/


#include <EFEU/efmain.h>
#include <EFEU/efio.h>

int io_mputc(int c, io_t *io, const char *delim)
{
	int n = 0;

	if	(listcmp(delim, c))
		if	(io_putc('\\', io) != EOF)	n++;

	if	(io_putc(c, io) != EOF)	n++;

	return n;
}


int io_mputs(const char *str, io_t *io, const char *delim)
{
	if	(str != NULL)
	{
		const uchar_t *p = (const uchar_t *) str;
		int n;

		for (n = 0; *p != 0; p++)
			n += io_mputc(*p, io, delim);

		return n;
	}
	else	return 0;
}

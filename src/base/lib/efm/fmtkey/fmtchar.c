/*	Konvertierung einer Zeichendefinition
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/fmtkey.h>


int fmt_char(io_t *io, const fmtkey_t *key, int c)
{
	int n;

	if	(key->flags & FMT_ALTMODE)
	{
		n = io_nputc('\'', io, 1);
		n += io_xputc(c, io, "'");
		n += io_nputc('\'', io, 1);
		return n;
	}

	if	((key->flags & FMT_RIGHT) && key->width > 0)
	{
		n = io_nputc(' ', io, key->width - 1);
	}
	else	n = 0;

	n += io_nputc(c, io, 1);
	n += io_nputc(' ', io, key->width - n);
	return n;
}

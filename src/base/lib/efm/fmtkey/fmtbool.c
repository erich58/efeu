/*	Konvertierung einer Zeichendefinition
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/fmtkey.h>

int fmt_bool(io_t *io, const fmtkey_t *key, int val)
{
	switch (key->mode)
	{
	case 'b':	return io_puts(val ? "1" : "0", io);
	default:	break;
	}

	return io_puts(val ? "true" : "false", io);
}

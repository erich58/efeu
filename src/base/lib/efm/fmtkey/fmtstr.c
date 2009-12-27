/*	String standardisiert ausgeben
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/


#include <EFEU/fmtkey.h>

int fmt_str(io_t *io, const fmtkey_t *key, const char *str)
{
	int n, k;

	if	(key->flags & FMT_ALTMODE)
	{
		if	(str != NULL)
		{
			n = io_puts("\"", io);
			n += io_xputs(str, io, "\"");
			n += io_puts("\"", io);
		}
		else	n = io_puts("NULL", io);

		return n;
	}

	if	(str == NULL)
	{
		return io_nputc(' ', io, key->width);
	}

	n = strlen(str);

	if	(key->flags & FMT_NOPREC)
	{
		k = n;
	}
	else if	(key->flags & FMT_NEGPREC)
	{
		if	(n > key->prec)
		{
			str += (n - key->prec);
			k = key->prec;
		}
		else	k = n;
	}
	else	k = n < key->prec ? n : key->prec;

	if	(key->flags & FMT_RIGHT)
	{
		n = io_nputc(' ', io, key->width - k);
	}
	else	n = 0;

	while (k-- > 0)
	{
		n += io_nputc(*str, io, 1);
		str++;
	}

	if	(n < key->width)
	{
		n += io_nputc(' ', io, key->width - n);
	}

	return n;
}

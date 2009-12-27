/*
String standardisiert ausgeben

$Copyright (C) 1994 Erich Frühstück
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
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

/*
Ausgabe einer Polynomstruktur

$Copyright (C) 1991 Erich Frühstück
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

#include <Math/pnom.h>

int pnprint(io_t *io, pnom_t *p, const char *fmt)
{
	int n, i, j;

	if	(p == NULL)
	{
		return io_puts("0 0\n", io);
	}

	n = io_printf(io, "%d %d", p->dim, p->deg);

	for (i = 0; i < p->dim; ++i)
	{
		n += io_nputc('\n', io, 1);
		n += io_printf(io, fmt, p->x[i]);

		for (j = 0; j <= p->deg; ++j)
		{
			n += io_nputc(' ', io, 1);
			n += io_printf(io, fmt, p->c[i][j]);
		}
	}

	n += io_nputc('\n', io, 1);
	return n;
}

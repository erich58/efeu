/*
Schicht generieren

$Copyright (C) 1995 Erich Frühstück
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

#include <EFEU/oldpixmap.h>

#define	LINE	"\\put(%d,%d){\\rule{%d\\unitlength}{\\unitlength}}\n"

void OldPixMap_TeX(Func_t *func, void *rval, void **arg)
{
	OldPixMap_t *pix;
	io_t *io;
	uchar_t *row;
	int i, j, n;
	int color;

	pix = Val_OldPixMap(arg[0]);
	io = Val_io(arg[1]);
	color = Val_int(arg[2]);

	if	(pix == NULL)	return;

	io_printf(io, "\\begin{picture}(%d,%d)\n", pix->cols, pix->rows);

	for (i = 0; i < pix->rows; i++)
	{
		row = pix->pixel + pix->cols * i;
		n = 0;

		while (n < pix->cols)
		{
			while (n < pix->cols && row[n] != color)
				n++;

			j = n;

			while (j < pix->cols && row[j] == color)
				j++;

			if	(j > n)
				io_printf(io, LINE, n, pix->rows - i - 1, j - n);

			n = j;
		}
	}

	io_printf(io, "\\end{picture}\n");
}

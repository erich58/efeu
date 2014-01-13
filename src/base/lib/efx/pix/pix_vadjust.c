/*
Vertikale Farbverschiebung

$Copyright (C) 2000 Erich Frühstück
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

#include <EFEU/Pixmap.h>

void Pixmap_vadjust (EPixmap *pix, int idx, int n)
{
	int i, j;
	unsigned char *p1, *p0;

	if	(!pix)	return;

	idx = (idx > 0) ? idx % 3 : -idx % 3;

	if	(n > 0)
	{
		p1 = pix->data;
		p0 = p1 + n * pix->cols * 3;
		i = pix->rows;

		for (; i > n; i--)
			for (j = pix->cols; j-- > 0; p1 += 3, p0 += 3)
				p1[idx] = p0[idx];

		for (; i > 0; i--)
			for (j = pix->cols; j-- > 0; p1 += 3)
				p1[idx] = 0;
	}
	else if	(n < 0)
	{
		p1 = pix->data + pix->rows * pix->cols * 3;
		p0 = p1 + n * pix->cols * 3;
		i = pix->rows;

		for (; i > -n; i--)
			for (j = pix->cols; j-- > 0;)
				p1 -= 3, p0 -= 3, p1[idx] = p0[idx];

		for (; i > 0; i--)
			for (j = pix->cols; j-- > 0;)
				p1 -= 3, p1[idx] = 0;
	}
}

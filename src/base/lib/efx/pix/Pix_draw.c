/*
Elemente Zeichnen

$Copyright (C) 2022 Erich Frühstück
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
#include <EFEU/object.h>
#include <EFEU/stdtype.h>

static void order (int *a, int *b)
{
	if	(*a > *b)
	{
		int x = *a;
		*a = *b;
		*b = x;
	}
}

static int normalize (int *a, int *b, int lim)
{
	order(a, b);

	if	(*a >= lim || *b < 0)
		return 0;

	if	(*a < 0)
		*a = 0;

	if	(*b >= lim)
		*b = lim - 1;

	return 1;
} // normalize

static void point(EPixmap *pix, int x, int y, int color)
{
	unsigned char *p = pix->data + (y * pix->width + x) * 3;
	p[0] = (color >> 16) & 0xff;
	p[1] = (color >> 8) & 0xff;
	p[2] = color & 0xff;
}

void Pix_fill(EPixmap *pix, int x0, int y0, int x1, int y1, int color)
{
	if	(!pix)
		return;

	if	(!normalize(&x0, &x1, pix->width))
		return;

	if	(!normalize(&y0, &y1, pix->height))
		return;

	for (int y = y0; y <= y1; ++y)
	{
		for (int x = x0; x <= x1; ++x)
		{
			point(pix, x, y, color);
		}
	}
}

void Pix_line(EPixmap *pix, int x0, int y0, int x1, int y1, int w, int color)
{
	if	(!pix || w < 1)
		return;

	int w1 = w / 2;
	--w;

	int sx = x0 < x1 ? 1 : -1;
	int sy = y0 < y1 ? 1 : -1;

	if	(x0 == x1 || y0 == y1)
	{
		x0 -= w1 * sx;
		y0 -= w1 * sy;
		x1 += (w - w1) * sx;
		y1 += (w - w1) * sy;
		Pix_fill(pix, x0, y0, x1, y1, color);
		return;
	}

	x0 -= w1;
	y0 -= w1;
	x1 -= w1;
	y1 -= w1;

	int dx = (x1 - x0) * sx;
	int dy = (y1 - y0) * sy;
	int cnt = (dx > dy) ? dx : dy;
	double fx = sx * dx / (double) cnt;
	double fy = sy * dy / (double) cnt;

	for (int i = 0; i <= cnt; ++i)
	{
		int x = x0 + i * fx + 0.5;
		int y = y0 + i * fy + 0.5;
		Pix_fill(pix, x, y, x + w, y + w, color);
	}

	return;
}

void Pix_frame(EPixmap *pix, int x0, int y0, int x1, int y1, int w, int color)
{
	if	(!pix || w < 1)
		return;

	order(&x0, &x1);
	order(&y0, &y1);
	Pix_fill(pix, x0 - w, y0 - w, x1 + w , y0 - 1, color);
	Pix_fill(pix, x0 - w, y1 + 1, x1 + w , y1 + w, color);

	Pix_fill(pix, x0 - w, y0, x0 - 1 , y1, color);
	Pix_fill(pix, x1 + 1, y0, x1 + w , y1, color);
	return;
}


/*
Farbwerte

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

int ColorValue(double x)
{
	if	(x <= 0.)	return 0;
	else if	(x >= 1.)	return 255;
	else			return (int) (255. * x + 0.5);
}

COLOR SetColor(double red, double green, double blue)
{
	COLOR color;

	color.red = ColorValue(red);
	color.green = ColorValue(green);
	color.blue = ColorValue(blue);
	return color;
}


/*	Die am nähesten liegende Farbe bestimmen
*/

static int dist (int a, int b)
{
	if	(a < b)	return b - a;
	else if	(a > b)	return a - b;
	else		return 0;
}


int GetColor(COLOR color, COLOR *tab, size_t dim)
{
	int i, n, d, last;

	d = 255 + 255 + 255;
	last = 0;

	for (i = 0; i < dim; i++)
	{
		n = dist(color.red, tab[i].red) + dist(color.green, tab[i].green)
			+ dist(color.blue, tab[i].blue);

		if	(n < d)
		{
			last = i;
			d = n;
		}
	}

	return last;
}

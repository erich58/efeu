/*
Dithering generieren

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

#define LIMIT	128
#define MAXVAL	256

void OldPixMap_Dither(EfiFunc *func, void *rval, void **arg)
{
	OldPixMap *pix, *npix;
	COLOR *color;
	int size;
	int i;
	int idx;
	int red, green, blue;

	pix = Val_OldPixMap(arg[0]);

	if	(pix == NULL)
	{
		Val_OldPixMap(rval) = NULL;
		return;
	}

	npix = new_OldPixMap(pix->rows, pix->cols);
	npix->colors = 8;

	for (i = 0; i < npix->colors; i++)
	{
		npix->color[i].idx = i;
		npix->color[i].red = i % 2 ? 255 : 0;
		npix->color[i].green = (i / 2) % 2 ? 255 : 0;
		npix->color[i].blue = (i / 4) % 2 ? 255 : 0;
	}

	size = pix->rows * pix->cols;
	red = green = blue = 0.;

	for (i = 0; i < size; i++)
	{
		color = pix->color + pix->pixel[i];
		red += color->red;
		green += color->green;
		blue += color->blue;
		idx = (red >= LIMIT) ? 1 : 0;
		idx += (green >= LIMIT) ? 2 : 0;
		idx += (blue >= LIMIT) ? 4 : 0;
		npix->pixel[i] = idx;

		if	(red >= LIMIT)	red -= MAXVAL;
		if	(green >= LIMIT)	green -= MAXVAL;
		if	(blue >= LIMIT)	blue -= MAXVAL;
	}

	Val_OldPixMap(rval) = npix;
}

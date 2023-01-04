/*
Teilbild generieren

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

EPixmap *Pix_clip(EPixmap *pix, int x0, int y0,
	size_t width, size_t height, int color)
{
	EPixmap *new = NewPixmap(width, height, color);

	if	(!pix)
		return pix;

	int w = width;
	int h = height;

	if	(x0 < 0)
		w += x0, x0 = 0;

	if	(y0 < 0)
		h += y0, y0 = 0;

	if	(x0 + w >= pix->width)
		w = pix->width - x0;

	if	(y0 + h >= pix->height)
		h = pix->height - y0;

	if	(w <= 0 || h <= 0)
		return pix;

	unsigned char *src = pix->data + (y0 * pix->width + x0) * 3;
	unsigned char *tg = new->data;

	for (int y = 0; y < h; ++y)
	{
		memcpy(tg, src, w * 3);
		src += pix->width * 3;
		tg += new->width * 3;
	}

	return new;
}

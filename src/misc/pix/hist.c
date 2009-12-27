/*
Histogramm generieren

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

void OldPixMap_Histogramm(Func_t *func, void *rval, void **arg)
{
	OldPixMap_t *pix;
	io_t *io;
	int i, size, *vec;

	pix = Val_OldPixMap(arg[0]);
	io = Val_io(arg[1]);

	if	(pix == NULL)	return;

	size = pix->rows * pix->cols;
	vec = ALLOC(PIX_CDIM, int);
	memset(vec, 0, PIX_CDIM * sizeof(int));

	for (i = 0; i < size; i++)
		vec[pix->pixel[i]]++;

	for (i = 0; i < pix->colors; i++)
		io_printf(io, "%3d, { %5.3f, %5.3f, %5.3f },%10d\n",  i,
			pix->color[i].red / 255., pix->color[i].green / 255.,
			pix->color[i].blue / 255., vec[i]);

	memfree(vec);
}

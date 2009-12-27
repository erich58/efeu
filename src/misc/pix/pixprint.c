/*
Pixelfile ausgeben

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

#include <EFEU/pconfig.h>
#include <ctype.h>
#include <EFEU/oldpixmap.h>

void print_OldPixMap(OldPixMap_t *pix, FILE *file)
{
	int i, j;

	fprintf(file, "PIX %d\n%d %d", pix->colors, pix->rows, pix->cols);

	for (i = 0; i < pix->colors; i++)
	{
		fprintf(file, "\n%d %d %d %d", pix->color[i].idx, pix->color[i].red,
			pix->color[i].green, pix->color[i].blue);
	}

	for (i = 0; i < pix->rows; i++)
	{
		for (j = 0; j < pix->cols; j++)
		{
			putc(j % 32 == 0 ? '\n' : ' ', file);
			fprintf(file, "%d", pix->pixel[j + i * pix->cols]);
		}
	}

	putchar('\n');
}

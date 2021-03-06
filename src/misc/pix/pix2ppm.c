/*
Pixelfile in PPM-File konvertieren

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

void OldPixMapToPPM(OldPixMap *pm, const char *name)
{
	int i, j;
	unsigned char *p;
	FILE *file;

	if	(pm == NULL)	return;

	file = fileopen(name, "w");
	fprintf(file, "P3\n%d %d\n255\n", pm->cols, pm->rows);
	p = pm->pixel;

	for (i = 0; i < pm->rows; i++)
	{
		for (j = 0; j < pm->cols; j++)
		{
			if	(j == 0)	;
			else if	(j % 6 == 0)	putc('\n', file);
			else			putc(' ', file), putc(' ', file);

			fprintf(file, "%d %d %d", pm->color[*p].red,
				pm->color[*p].green, pm->color[*p].blue);
			p++;
		}

		putc('\n', file);
	}

	fileclose(file);
}

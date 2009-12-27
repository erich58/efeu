/*
Pixelfile in XPM2-File konvertieren

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

void OldPixMapToXPM2(OldPixMap_t *pix, const char *name)
{
	int i, j;
	char *fmt;
	FILE *file;
	int n;

	file = fileopen(name, "w");

	if	(pix->colors <= 16)
	{
		fmt = "%X";
		n = 1;
	}
	else
	{
		fmt = "%02X";
		n = 2;
	}

	fprintf(file, "/* XPM2 C */\n static char ** bixmap = {\n\"");
	fprintf(file, "%d %d %d %d", pix->cols, pix->rows, pix->colors, n);

	for (i = 0; i < pix->colors; i++)
	{
		fputs("\",\n\"", file);
		fprintf(file, fmt, i);
		fprintf(file, " m C%d c #%02X%02X%02X", i, pix->color[i].red,
			pix->color[i].green, pix->color[i].blue);
	}

	for (i = 0; i < pix->rows; i++)
	{
		fputs("\",\n\"", file);

		for (j = 0; j < pix->cols; j++)
			fprintf(file, fmt, pix->pixel[i*pix->cols + j]);
	}

	fputs("\"}\n", file);
	fileclose(file);
}

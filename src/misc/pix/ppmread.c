/*
PPM-File einlesen

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
#include <ctype.h>

#define	VBSIZE	32

static char *get_val(FILE *file)
{
	static char buf[VBSIZE];
	int pos;
	int c;

	pos = 0;

	while ((c = getc(file)) != EOF)
	{
		if	(c == '#')
		{
			do
			{
				c = getc(file);

				if	(c == EOF)	break;
			}
			while	(c != '\n');
		}
		else if	(isspace(c))
		{
			if	(pos)	break;
		}
		else if	(pos < VBSIZE - 1)
		{
			buf[pos++] = c;
		}
	}

	buf[pos++] = 0;
	return buf;
}


/*	Bitmapfile
*/

static int get_pbm (FILE *file, VecBuf *buf, int n)
{
	int c;

	while ((c = getc(file)) != EOF)
	{
		if	(c == '1')	return 0;
		if	(c == '0')	return 1;
	}

	return 1;
}

static int get_pbmraw (FILE *file, VecBuf *buf, int n)
{
	static int data;
	n = n % 8;

	if	(n == 0)
		data = getc(file);

	return (data & (1 << (7 - n)) ? 0 : 1);
}

static int cmp_color (const void *pa, const void *pb)
{
	const COLOR *a = pa;
	const COLOR *b = pb;

	if	(a->red < b->red)	return -1;
	else if	(a->red > b->red)	return 1;
	else if	(a->green < b->green)	return -1;
	else if	(a->green > b->green)	return 1;
	else if	(a->blue < b->blue)	return -1;
	else if	(a->blue > b->blue)	return 1;
	else				return 0;
}

static int color_value (VecBuf *buf, int red, int green, int blue)
{
	COLOR key, *ptr;

	key.red = red;
	key.green = green;
	key.blue = blue;
	key.idx = buf->used;
	ptr = vb_search(buf, &key, cmp_color, VB_SEARCH);

	if	(ptr)
		return ptr->idx;

	if	(buf->used >= 255)
	{
		log_error(NULL, "[pixmap:2]", NULL);
		return 0;
	}

	ptr = vb_search(buf, &key, cmp_color, VB_ENTER);
	return ptr->idx;
}

static int gray_value (VecBuf *buf, int gray)
{
	return color_value(buf, gray, gray, gray);
}

/*	Graustufenfile
*/

static int get_pgm (FILE *file, VecBuf *buf, int n)
{
	return gray_value(buf, atoi(get_val(file)));
}

static int get_pgmraw (FILE *file, VecBuf *buf, int n)
{
	return gray_value(buf, getc(file));
}


/*	Farbdatei
*/

static int get_ppm (FILE *file, VecBuf *buf, int n)
{
	int red = atoi(get_val(file));
	int green = atoi(get_val(file));
	int blue = atoi(get_val(file));
	return color_value(buf, red, green, blue);
}

static int get_ppmraw (FILE *file, VecBuf *buf, int n)
{
	int red = getc(file);
	int green = getc(file);
	int blue = getc(file);
	return color_value(buf, red, green, blue);
}


OldPixMap *read_PPMFile (FILE *file)
{
	int cols, rows;
	int i, n;
	int bitmap;
	OldPixMap *pm;
	char *p;
	double x;
	int (*get) (FILE *file, VecBuf *buf, int n); 
	VecBuf buf;

	p = get_val(file);

	if	(p[0] != 'P')
	{
		log_error(NULL, "[pixmap:1]", NULL);
		return NULL;
	}

	switch (p[1])
	{
	case '1':	get = get_pbm; bitmap = 1; break;
	case '2':	get = get_pgm; bitmap = 0; break;
	case '3':	get = get_ppm; bitmap = 0; break;
	case '4':	get = get_pbmraw; bitmap = 1; break;
	case '5':	get = get_pgmraw; bitmap = 0; break;
	case '6':	get = get_ppmraw; bitmap = 0; break;
	default:	log_error(NULL, "[pixmap:1]", NULL); return NULL;
	}

	cols = atoi(get_val(file));
	rows = atoi(get_val(file));
	vb_init(&buf, 512, sizeof(COLOR));

	if	(bitmap)
	{
		pm = new_OldPixMap(rows, cols);
		pm->color[0].red = pm->color[0].green = pm->color[0].blue = 0;
		pm->color[1].red = pm->color[1].green = pm->color[1].blue = 255;
		pm->colors = 2;
		x = 0.; /* make cc happy */
	}
	else
	{
		pm = new_OldPixMap(rows, cols);
		x = 255. / atoi(get_val(file));
	}

	n = cols *rows;

	for (i = 0; i < n; i++)
		pm->pixel[i] = get(file, &buf, i);

	if	(!bitmap)
	{
		COLOR *color = buf.data;
		
		for (i = 0; i < buf.used; i++)
		{
			pm->color[i].red = color[i].red * x + 0.5;
			pm->color[i].green = color[i].green * x + 0.5;
			pm->color[i].blue = color[i].blue * x + 0.5;
		}

		pm->colors = buf.used;
	}

	vb_free(&buf);
	return pm;
}

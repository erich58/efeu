/*	Bilddatei laden
	(c) 2000 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/ftools.h>
#include <EFEU/procenv.h>
#include <EFEU/parsub.h>
#include <EFEU/strbuf.h>
#include <ctype.h>
#include "korrscan.h"

#define	VBSIZE	32

static double color_scale = 1.;

static int skip_white (FILE *file)
{
	int c;

	do	c = getc(file);
	while	(isspace(c));

	ungetc(c, file);
	return c;
}

static char *get_val (FILE *file)
{
	static char buf[VBSIZE];
	int pos;
	int c;

	pos = 0;

	while ((c = getc(file)) != EOF)
	{
		if	(c == '#')
		{
			do	c = getc(file);
			while	(c != EOF && c != '\n');
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

static void set_pbm (FILE *file, Point_t *p, int n)
{
	int c;

	while ((c = getc(file)) != EOF)
	{
		switch(c)
		{
		case '1':	p->red = p->green = p->blue = 0; return;
		case '0':	p->red = p->green = p->blue = 1; return;
		default:	break;
		}
	}
}

static void set_pbmraw (FILE *file, Point_t *p, int n)
{
	static int data;
	n = n % 8;

	if	(n == 0)
		data = getc(file);

	p->red = (data & (1 << (7 - n)) ? 0 : 1);
	p->green = p->blue = p->red;
}

/*	Graustufenfile
*/

static void set_pgm (FILE *file, Point_t *p, int n)
{
	int val = color_scale * atoi(get_val(file)) + 0.5;
	p->red = p->green = p->blue = val;
}

static void set_pgmraw (FILE *file, Point_t *p, int n)
{
	p->red = p->green = p->blue = getc(file);
}


/*	Farbdatei
*/

static void set_ppm (FILE *file, Point_t *p, int n)
{
	p->red = color_scale * atoi(get_val(file)) + 0.5;
	p->green = color_scale * atoi(get_val(file)) + 0.5;
	p->blue = color_scale * atoi(get_val(file)) + 0.5;
}

static void set_ppmraw (FILE *file, Point_t *p, int n)
{
	p->red = getc(file);
	p->green = getc(file);
	p->blue = getc(file);
}

Picture_t *LoadPicture (FILE *file)
{
	Picture_t *pic;
	size_t rows, cols, maxval;
	void (*set) (FILE *file, Point_t *p, int col); 
	char *label;
	size_t i, n;
	int flag;

	pic = NULL;

	if	(getc(file) != 'P')
	{
		fileerror(file, NULL, 1, 0);
		return NULL;
	}

	switch (getc(file))
	{
	case '1':	set = set_pbm; flag = 0; break;
	case '2':	set = set_pgm; flag = 1; break;
	case '3':	set = set_ppm; flag = 1; break;
	case '4':	set = set_pbmraw; flag = 0; break;
	case '5':	set = set_pgmraw; flag = 1; break;
	case '6':	set = set_ppmraw; flag = 1; break;
	default:	fileerror(file, NULL, 1, 0); return NULL;
	}

	if	(skip_white(file) == '#')
	{
		strbuf_t *buf;
		int c;
		
		buf = new_strbuf(0);
		getc(file);
		c = getc(file);

		while	(c == ' ' || c == '\t')
			c = getc(file);

		while 	(c != EOF && c != '\n')
		{
			sb_putc(c, buf);
			c = getc(file);
		}

		label = sb2str(buf);
	}
	else	label = NULL;

	rows = cols = 0;
	cols = atoi(get_val(file));
	rows = atoi(get_val(file));
	maxval = flag ? atoi(get_val(file)) : 2;

	if	(maxval > 255)
	{
		color_scale = 255. / maxval;
		maxval = 255;
	}
	else	color_scale = 1.;

	pic = AllocPicture(rows, cols);
	pic->maxval = maxval;
	pic->label = label;
	n = cols * rows;

	for (i = 0; i < n; i++)
		set(file, pic->point + i, i);

	return pic;
}

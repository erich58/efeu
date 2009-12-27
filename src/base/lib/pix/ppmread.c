/*	PPM-File einlesen
	(c) 1995 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6
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

static int get_pbm(FILE *file, int n)
{
	int c;

	while ((c = getc(file)) != EOF)
	{
		if	(c == '1')	return 0;
		if	(c == '0')	return 1;
	}

	return 1;
}

static int get_pbmraw(FILE *file, int n)
{
	static int data;
	n = n % 8;

	if	(n == 0)
	{
		data = getc(file);
	}

	return (data & (1 << (7 - n)) ? 0 : 1);
}

static Color_t color;
static Color_t *colordata = NULL;
static int colordim = 0;
static xtab_t *colortab;


static int cmp_color(Color_t *a, Color_t *b)
{
	if	(a->red < b->red)	return -1;
	else if	(a->red > b->red)	return 1;
	else if	(a->green < b->green)	return -1;
	else if	(a->green > b->green)	return 1;
	else if	(a->blue < b->blue)	return -1;
	else if	(a->blue > b->blue)	return 1;
	else				return 0;
}

static int color_value(void)
{
	Color_t *ptr;

	if	((ptr = xsearch(colortab, &color, XS_FIND)))
	{
		return (ptr - colordata);
	}

	if	(colordim >= 255)
	{
		liberror(MSG_PIXMAP, 2);
		return 0;
	}

	colordata[colordim] = color;
	xsearch(colortab, colordata + colordim, XS_ENTER);
	return colordim++;
}


/*	Graustufenfile
*/

static int get_pgm(FILE *file, int n)
{
	color.red = atoi(get_val(file));
	color.green = color.green;
	color.blue = color.blue;
	return color_value();
}

static int get_pgmraw(FILE *file, int n)
{
	color.red = getc(file);
	color.green = color.green;
	color.blue = color.blue;
	return color_value();
}


/*	Farbdatei
*/

static int get_ppm(FILE *file, int n)
{
	color.red = atoi(get_val(file));
	color.green = atoi(get_val(file));
	color.blue = atoi(get_val(file));
	return color_value();
}

static int get_ppmraw(FILE *file, int n)
{
	color.red = getc(file);
	color.green = getc(file);
	color.blue = getc(file);
	return color_value();
}


OldPixMap_t *read_PPMFile(FILE *file)
{
	int cols, rows;
	int i, n;
	int bitmap;
	OldPixMap_t *pm;
	char *p;
	double x;
	int (*get) (FILE *file, int n); 

	p = get_val(file);

	if	(p[0] != 'P')
	{
		liberror(MSG_PIXMAP, 1);
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
	default:	liberror(MSG_PIXMAP, 1); return NULL;
	}

	cols = atoi(get_val(file));
	rows = atoi(get_val(file));

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
		colordata = pm->color;
		colortab = xcreate(256, (comp_t) cmp_color);
		colordim = 0;
	}

	n = cols *rows;

	for (i = 0; i < n; i++)
		pm->pixel[i] = get(file, i);

	if	(!bitmap)
	{
		for (i = 0; i < colordim; i++)
		{
			colordata[i].red = colordata[i].red * x + 0.5;
			colordata[i].green = colordata[i].green * x + 0.5;
			colordata[i].blue = colordata[i].blue * x + 0.5;
		}

		xdestroy(colortab, NULL);
		pm->colors = colordim;
	}

	return pm;
}

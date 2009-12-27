/*	Pixeldatei laden
	(c) 2000 Erich Fr�hst�ck
	A-3423 St.Andr�/W�rdern, S�dtirolergasse 17-21/5
*/

#include <EFEU/Pixmap.h>
#include <EFEU/procenv.h>
#include <ctype.h>

#define	VBSIZE	32

static int color_maxval = 255;
static double color_scale = 1.;


static int get_val (io_t *io)
{
	static char buf[VBSIZE];
	int pos;
	int c;

	pos = 0;

	while ((c = io_getc(io)) != EOF)
	{
		if	(c == '#')
		{
			do	c = io_getc(io);
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
	return atoi(buf);
}

/*	Bitmapfile
*/

static int pbm_color(io_t *io)
{
	for (;;)
	{
		switch (io_getc(io))
		{
		case EOF:
		case '1':	return 0;
		case '0':	return 255;
		default:	break;
		}
	}
}

static void load_pbm (io_t *io, Pixmap_t *pix)
{
	size_t n;
	uchar_t *p;

	for (p = pix->data, n = pix->rows * pix->cols; n-- > 0; p += 3)
		p[0] = p[1] = p[2] = pbm_color(io);
}

static void load_pbmraw (io_t *io, Pixmap_t *pix)
{
	size_t i, j, k, data;
	uchar_t *p;

	data = 0;
	p = pix->data;

	for (i = 0; i < pix->rows; i++)
	{
		for (j = 0; j < pix->cols; j++)
		{
			k = j % 8;

			if	(k == 0)
				data = io_getc(io);

			p[0] = p[1] = p[2] = (data & (1 << (7 - k)) ? 0 : 255);
			p += 3;
		}
	}
}

/*	Graustufenfile
*/

static void load_pgm (io_t *io, Pixmap_t *pix)
{
	size_t n;
	uchar_t *p;

	for (p = pix->data, n = pix->rows * pix->cols; n-- > 0; p += 3)
		p[0] = p[1] = p[2] = color_scale * get_val(io) + 0.5;
}

static void load_pgmraw (io_t *io, Pixmap_t *pix)
{
	size_t n;
	uchar_t *p;

	for (p = pix->data, n = pix->rows * pix->cols; n-- > 0; p += 3)
		p[0] = p[1] = p[2] = color_scale * io_getc(io) + 0.5;
}


/*	Farbdatei
*/

static void load_ppm (io_t *io, Pixmap_t *pix)
{
	size_t n;
	uchar_t *p;

	for (p = pix->data, n = pix->rows * pix->cols * 3; n-- > 0; p++)
		*p = color_scale * get_val(io) + 0.5;
}

static void load_ppmraw (io_t *io, Pixmap_t *pix)
{
	io_dbread(io, pix->data, 1, 1, pix->rows * pix->cols * 3);

	if	(color_maxval != 255)
	{
		size_t n = pix->rows * pix->cols * 3;
		uchar_t *p = pix->data;

		for (; n-- > 0; p++)
			*p = color_scale * *p + 0.5;
	}
}

Pixmap_t *LoadPixmap (io_t *io)
{
	Pixmap_t *pix;
	size_t rows, cols;
	void (*load) (io_t *io, Pixmap_t *pix); 
	int flag;

	if	(io_getc(io) != 'P')
	{
		io_error(io, NULL, 1, 0);
		return NULL;
	}

	switch (io_getc(io))
	{
	case '1':	load = load_pbm; flag = 0; break;
	case '2':	load = load_pgm; flag = 1; break;
	case '3':	load = load_ppm; flag = 1; break;
	case '4':	load = load_pbmraw; flag = 0; break;
	case '5':	load = load_pgmraw; flag = 1; break;
	case '6':	load = load_ppmraw; flag = 1; break;
	default:	io_error(io, NULL, 1, 0); return NULL;
	}

	rows = cols = 0;
	cols = get_val(io);
	rows = get_val(io);

	if	(flag)
	{
		color_maxval = get_val(io);
		color_scale = color_maxval ? 255. / color_maxval : 0.;
	}

	pix = NewPixmap(rows, cols, 0);
	load(io, pix);
	return pix;
}

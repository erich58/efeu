/*	Pixelfile laden/ausgeben
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/pconfig.h>
#include <EFEU/ftools.h>
#include <ctype.h>
#include <EFEU/oldpixmap.h>

static int pixtype(FILE *file)
{
	int c;

	if	(getc(file) != 'P')	return 0;

	c = getc(file);

	switch(c)
	{
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':	ungetc(c, file); ungetc('P', file); return 1;
	case 'I':	break;
	default:	return 0;
	}

	if	(getc(file) != 'X')	return 0;

	return 2;
}


OldPixMap_t *load_OldPixMap(const char *name)
{
	FILE *file;
	OldPixMap_t *pix;
	unsigned rows, cols;
	int ncol;

	file = fileopen(name, "rz");

	switch (pixtype(file))
	{
	case 0:
		fileclose(file);
		reg_cpy(1, name);
		liberror(MSG_PIXMAP, 11);
		return NULL;
	case 1:
		pix = read_PPMFile(file);
		fileclose(file);
		return pix;
	default:
		break;
	}

	ncol = getc(file);
	rows = get4byte(file);
	cols = get4byte(file);
	pix = new_OldPixMap(rows, cols);
	pix->colors = loadvec(file, pix->color, sizeof(Color_t), ncol);
	fread(pix->pixel, 1, rows * cols, file);
	fileclose(file);
	return pix;
}


void save_OldPixMap(OldPixMap_t *pix, const char *name)
{
	FILE *file;

	file = fileopen(name, "wz");
	putc('P', file);
	putc('I', file);
	putc('X', file);
	putc(pix->colors, file);
	put4byte(pix->rows, file);
	put4byte(pix->cols, file);

	savevec(file, pix->color, sizeof(Color_t), pix->colors);

	fwrite(pix->pixel, 1, pix->rows * pix->cols, file);
	fileclose(file);
}

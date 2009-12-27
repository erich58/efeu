/*	Dithering generieren
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/oldpixmap.h>

#define LIMIT	128
#define MAXVAL	256

void OldPixMap_Dither(Func_t *func, void *rval, void **arg)
{
	OldPixMap_t *pix, *npix;
	Color_t *color;
	int size;
	int i;
	int idx;
	int red, green, blue;

	pix = Val_OldPixMap(arg[0]);

	if	(pix == NULL)
	{
		Val_OldPixMap(rval) = NULL;
		return;
	}

	npix = new_OldPixMap(pix->rows, pix->cols);
	npix->colors = 8;

	for (i = 0; i < npix->colors; i++)
	{
		npix->color[i].idx = i;
		npix->color[i].red = i % 2 ? 255 : 0;
		npix->color[i].green = (i / 2) % 2 ? 255 : 0;
		npix->color[i].blue = (i / 4) % 2 ? 255 : 0;
	}

	size = pix->rows * pix->cols;
	red = green = blue = 0.;

	for (i = 0; i < size; i++)
	{
		color = pix->color + pix->pixel[i];
		red += color->red;
		green += color->green;
		blue += color->blue;
		idx = (red >= LIMIT) ? 1 : 0;
		idx += (green >= LIMIT) ? 2 : 0;
		idx += (blue >= LIMIT) ? 4 : 0;
		npix->pixel[i] = idx;

		if	(red >= LIMIT)	red -= MAXVAL;
		if	(green >= LIMIT)	green -= MAXVAL;
		if	(blue >= LIMIT)	blue -= MAXVAL;
	}

	Val_OldPixMap(rval) = npix;
}

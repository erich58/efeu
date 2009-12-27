/*	Histogramm generieren
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
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

	FREE(vec);
}

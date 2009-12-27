/*	Schicht generieren
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/oldpixmap.h>

#define	LINE	"\\put(%d,%d){\\rule{%d\\unitlength}{\\unitlength}}\n"

void OldPixMap_TeX(Func_t *func, void *rval, void **arg)
{
	OldPixMap_t *pix;
	io_t *io;
	uchar_t *row;
	int i, j, n;
	int color;

	pix = Val_OldPixMap(arg[0]);
	io = Val_io(arg[1]);
	color = Val_int(arg[2]);

	if	(pix == NULL)	return;

	io_printf(io, "\\begin{picture}(%d,%d)\n", pix->cols, pix->rows);

	for (i = 0; i < pix->rows; i++)
	{
		row = pix->pixel + pix->cols * i;
		n = 0;

		while (n < pix->cols)
		{
			while (n < pix->cols && row[n] != color)
				n++;

			j = n;

			while (j < pix->cols && row[j] == color)
				j++;

			if	(j > n)
				io_printf(io, LINE, n, pix->rows - i - 1, j - n);

			n = j;
		}
	}

	io_printf(io, "\\end{picture}\n");
}

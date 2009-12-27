/*	Vertikale Farbverschiebung
	(c) 2000 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/Pixmap.h>

void Pixmap_vadjust (Pixmap_t *pix, int idx, int n)
{
	int i, j;
	uchar_t *p1, *p0;

	if	(!pix)	return;

	idx = (idx > 0) ? idx % 3 : -idx % 3;

	if	(n > 0)
	{
		p1 = pix->data;
		p0 = p1 + n * pix->cols * 3;
		i = pix->rows;

		for (; i > n; i--)
			for (j = pix->cols; j-- > 0; p1 += 3, p0 += 3)
				p1[idx] = p0[idx];

		for (; i > 0; i--)
			for (j = pix->cols; j-- > 0; p1 += 3)
				p1[idx] = 0;
	}
	else if	(n < 0)
	{
		p1 = pix->data + pix->rows * pix->cols * 3;
		p0 = p1 + n * pix->cols * 3;
		i = pix->rows;

		for (; i > -n; i--)
			for (j = pix->cols; j-- > 0;)
				p1 -= 3, p0 -= 3, p1[idx] = p0[idx];

		for (; i > 0; i--)
			for (j = pix->cols; j-- > 0;)
				p1 -= 3, p1[idx] = 0;
	}
}

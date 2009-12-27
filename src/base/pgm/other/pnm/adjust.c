/*	Farbverschiebung
	(c) 2000 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/ftools.h>
#include <EFEU/procenv.h>
#include <EFEU/parsub.h>
#include <EFEU/strbuf.h>
#include <ctype.h>
#include "korrscan.h"

static Point_t Black = { 0, 0, 0, 0 };

void Adjust_red (Point_t *y, Point_t *x) { y->red = x->red; }
void Adjust_green (Point_t *y, Point_t *x) { y->green = x->green; }
void Adjust_blue (Point_t *y, Point_t *x) { y->blue = x->blue; }

void AdjustPicture (Picture_t *pic, Adjust_t func, int n)
{
	int i, j;
	Point_t *p1, *p0;

	if	(!pic)	return;

	if	(n > 0)
	{
		p1 = pic->point;
		p0 = pic->point + n * pic->cols;
		i = pic->rows;

		for (; i > n; i--)
			for (j = pic->cols; j-- > 0;)
				func(p1++, p0++);

		for (; i > 0; i--)
			for (j = pic->cols; j-- > 0;)
				func(p1++, &Black);
	}
	else if	(n < 0)
	{
		p1 = pic->point + pic->rows * pic->cols;
		p0 = p1 + n * pic->cols;
		i = pic->rows;

		for (; i > -n; i--)
			for (j = pic->cols; j-- > 0;)
				func(--p1, --p0);

		for (; i > 0; i--)
			for (j = pic->cols; j-- > 0;)
				func(--p1, &Black);
	}
}

/*	Bilddatei laden/ausgeben
	(c) 2000 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/ftools.h>
#include <EFEU/procenv.h>
#include <EFEU/parsub.h>
#include "korrscan.h"

Picture_t *AllocPicture (size_t rows, size_t cols)
{
	Picture_t *pic = memalloc(sizeof(Picture_t));
	pic->label = NULL;
	pic->rows = rows;
	pic->cols = cols;
	pic->maxval = 255;
	pic->point = memalloc(rows * cols * sizeof(Point_t));
	return pic;
}

void FreePicture (Picture_t *pic)
{
	if	(pic)
	{
		memfree(pic->label);
		memfree(pic->point);
		memfree(pic);
	}
}

void ShowPicture (Picture_t *pic, FILE *file)
{
	if	(pic == NULL)	return;

	fprintf(file, "%d %d %d", pic->rows, pic->cols, pic->maxval);

	if	(pic->label)
	{
		putc(' ', file);
		fputs(pic->label, file);
	}

	putc('\n', file);
}

void SavePicture (Picture_t *pic, FILE *file)
{
	Point_t *p;
	int i, j;

	if	(pic == NULL)	return;

	fprintf(file, "P6\n");

	if	(pic->label)
		fprintf(file, "# %s\n", pic->label);

	fprintf(file, "%d %d\n%d\n", pic->cols, pic->rows, pic->maxval);
	p = pic->point;

	for (i = 0; i < pic->rows; i++)
	{
		for (j = 0; j < pic->cols; j++)
		{
			putc(p->red, file);
			putc(p->green, file);
			putc(p->blue, file);
			p++;
		}
	}
}

/*	Schicht generieren
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/oldpixmap.h>

void OldPixMap_Slice(Func_t *func, void *rval, void **arg)
{
	OldPixMap_t *pix, *newpix;
	ObjList_t *list;
	int size;
	int i, n;

	pix = Val_OldPixMap(arg[0]);

	if	(pix == NULL)
	{
		Val_OldPixMap(rval) = NULL;
		return;
	}

	newpix = new_OldPixMap(pix->rows, pix->cols);
	newpix->colors = 0;
	newpix->color[newpix->colors++] = SetColor(0., 0., 0.);

	for (i = 0; i < pix->colors; i++)
		pix->color[i].idx = 0;

	for (list = Val_list(arg[1]); list != NULL; list = list->next)
	{
		n = Obj2int(RefObj(list->obj));

		if	(n < pix->colors && newpix->colors < PIX_CDIM)
		{
			pix->color[n].idx = newpix->colors;
			newpix->color[newpix->colors] = pix->color[n];
			newpix->color[newpix->colors].idx = 0;
			newpix->colors++;
		}
	}

	size = pix->rows * pix->cols;

	for (i = 0; i < size; i++)
		newpix->pixel[i] = pix->color[pix->pixel[i]].idx;

	for (i = 0; i < pix->colors; i++)
		pix->color[i].idx = 0;

	Val_OldPixMap(rval) = newpix;
}

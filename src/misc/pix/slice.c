/*
Schicht generieren

$Copyright (C) 1995 Erich Frühstück
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/oldpixmap.h>

void OldPixMap_Slice(EfiFunc *func, void *rval, void **arg)
{
	OldPixMap *pix, *newpix;
	EfiObjList *list;
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

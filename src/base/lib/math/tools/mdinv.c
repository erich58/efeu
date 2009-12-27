/*
Matrixinversion

$Copyright (C) 1997 Erich Frühstück
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

#include <Math/mdmath.h>
#include <Math/linalg.h>

static void getdata (EfiKonv *konv, mdaxis *x, mdaxis *x1, mdaxis *x2,
	char *p, char *data);

mdmat *mdinv (mdmat *base)
{
	mdaxis *axis, *xpre, *xpost, **ppre, **ppost;
	EfiKonv konv;
	mdmat *md;
	int lines, cols;

	if	(base == NULL)
	{
		dbg_error(NULL, "[mdmath:1]", NULL);
		return NULL;
	}

	if	(GetKonv(&konv, base->type, &Type_double) == NULL)
	{
		dbg_note(NULL, "[mdmath:2]", "m", type2str(base->type));
		return NULL;
	}

/*	Ergebnisachsen zusammenstellen
*/
	axis = base->axis;
	xpre = NULL;
	xpost = NULL;
	ppre = &xpre;
	ppost = &xpost;
	lines = cols = 1;

	for (axis = base->axis; axis != NULL; axis = axis->next)
	{
		if	(axis->flags & MDXFLAG_MARK)
		{
			*ppost = cpy_axis(axis, 0);
			(*ppost)->flags = MDXFLAG_MARK;
			ppost = &(*ppost)->next;
			cols *= axis->dim;
		}
		else
		{
			*ppre = cpy_axis(axis, 0);
			ppre = &(*ppre)->next;
			lines *= axis->dim;
		}
	}

	*ppre = xpost;

	if	(lines != cols)
	{
		dbg_error(NULL, "[mdmath:3]", NULL);
		return NULL;
	}

/*	Datenmatrix generieren
*/
	md = new_mdmat();
	md->axis = xpre;
	md->type = &Type_double;
	md->size = md_size(md->axis, md->type->size);
	md->data = memalloc(md->size);

	getdata(&konv, md->axis, base->axis, base->axis, md->data, base->data);

	if	((cols = GaussJordan(md->data, lines)) != lines)
	{
		del_mdmat(md);
		dbg_error(NULL, "[mdmath:4]", "dd", lines, cols);
		return NULL;
	}

	md->axis = xpost;
	*ppost = xpre;
	*ppre = NULL;
	md->size = md_size(md->axis, md->type->size);
	return md;
}


static void getdata (EfiKonv *konv, mdaxis *x, mdaxis *x1, mdaxis *x2,
	char *p, char *data)
{
	if	(x != NULL)
	{
		int i;
		size_t size;

		while (x1 && (x1->flags & MDXFLAG_MARK))
			x1 = x1->next;

		while (x2 && !(x2->flags & MDXFLAG_MARK))
			x2 = x2->next;

		if	(x->flags & MDXFLAG_MARK)
		{
			size = x2->size;
			x2 = x2->next;
		}
		else
		{
			size = x1->size;
			x1 = x1->next;
		}

		for (i = 0; i < x->dim; i++)
		{
			getdata(konv, x->next, x1, x2, p, data);
			p += x->size;
			data += size;
		}
	}
	else	KonvData(konv, p, data);
}

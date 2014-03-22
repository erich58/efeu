/*
:de:Achsen einer Datenmatrix aufsummieren

$Copyright (C) 1994, 2007 Erich Frühstück
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

#include <EFEU/mdmat.h>

static mdaxis *add_axis (StrPool *pool, mdaxis **tab, size_t dim);
static void setval (EfiKonv *konv, mdaxis *x, char *px, mdaxis *y, char *py);

mdmat *md_cat (const char *def, mdmat **tab, size_t dim)
{
	size_t i;
	mdmat *md;
	mdlist *list;
	mdaxis **x;
	char *fmt;
	char *p;
	mdaxis **ptr;
	EfiKonv konv;
	size_t ndim;

	if	(dim == 0)	return NULL;

	list = str2mdlist(def, 0);
	md = new_mdmat();
	md->sbuf = NewStrPool();

	if	(list == NULL)	list = str2mdlist("X", 0);

	fmt = NULL;

	if	(list->name && (fmt = strchr(list->name, '%')))
		*fmt++ = 0;

/*	Hauptachse und Datentype bestimmen, Achsenpointer setzen
*/
	md->type = NULL;
	ndim = max(dim, list->dim);
	md->axis = new_axis(md->sbuf, ndim);
	md->axis->i_name = StrPool_xadd(md->sbuf, list->name);

	x = memalloc(ndim * sizeof(mdaxis *));

	for (i = 0; i < ndim; i++)
	{
		if	(i >= list->dim)
		{
			if	(fmt)
			{
				p = mpsubarg(fmt, "ssd", tab[i] ?
					StrPool_get(tab[i]->sbuf,
					tab[i]->i_name) : NULL,
					list->name, i + 1);
			}
			else	p = msprintf("%s%d", list->name, i + 1);

			md->axis->idx[i].i_name = StrPool_xadd(md->sbuf, p);
			memfree(p);
		}
		else	
		{
			md->axis->idx[i].i_name = StrPool_xadd(md->sbuf,
					list->list[i]);
		}

		if	(i >= dim || tab[i] == NULL)
		{
			x[i] = NULL;
			continue;
		}
		else	x[i] = tab[i]->axis;

		if	(md->type == NULL || KonvDist(md->type, tab[i]->type)
				< KonvDist(tab[i]->type, md->type))
		{
			md->type = tab[i]->type;
		}
	}

	del_mdlist(list);

	if	(md->type == NULL)	md->type = &Type_ptr;

/*	Teilachsen bestimmen
*/
	ptr = &md->axis->next;

	while ((*ptr = add_axis(md->sbuf, x, dim)) != NULL)
		ptr = &(*ptr)->next;

	memfree(x);

/*	Datenmatrix generieren
*/
	md_alloc(md);
	memset(md->data, 0, md->size);

/*	Werte einfügen
*/
	p = md->data;

	for (i = 0; i < dim; i++)
	{
		if	(tab[i])
		{
			Md_KonvDef(&konv, tab[i]->type, md->type);
			setval(&konv, md->axis->next, p,
				tab[i]->axis, tab[i]->data);
		}

		p += md->axis->size;
	}

	return md;
}


static mdaxis *add_axis(StrPool *pool, mdaxis **tab, size_t dim)
{
	int i;
	mdaxis *axis;

	axis = NULL;

	for (i = 0; i < dim; i++)
	{
		if	(tab[i] == NULL)	continue;

		if	(axis == NULL || axis->dim < tab[i]->dim)
			axis = tab[i];

		tab[i] = tab[i]->next;
	}

	return cpy_axis(pool, axis, 0);
}

static void setval (EfiKonv *konv, mdaxis *x, char *px,
	mdaxis *y, char *py)
{
	if	(x != NULL)
	{
		int dim, sx, sy;

		dim = x->dim;
		sx = x->size;
		x = x->next;

		if	(y != NULL)
		{
			if	(dim > y->dim)	dim = y->dim;

			sy = y->size;
			y = y->next;
		}
		else	sy = 0;
			
		while (dim-- > 0)
		{
			setval(konv, x, px, y, py);
			py += sy;
			px += sx;
		}
	}
	else	KonvData(konv, px, py);
}

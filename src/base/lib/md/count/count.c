/*
Daten zählen

$Copyright (C) 1994 Erich Frühstück
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
#include <EFEU/mdcount.h>

double MdCountWeight = 1.;

void (*md_count_hook) (MdCountList *clist, mdmat *md) = NULL;

void md_count (mdmat *ctab, const void *data)
{
	register MdCntGrp *s;
	register mdaxis *x;
	MdCountList *list;
	int flag;

	if	(!ctab)	return;

	if	(!ctab->x_priv)
	{
		log_note(md_note, "[mdmat:303]", NULL);
		return;
	}

/*	Zähler setzen
*/
	flag = 0;

	for (list = ctab->x_priv; list != NULL; list = list->next)
	{
		list->flag = (!list->cnt->set ||
			list->cnt->set(list->cnt, list->data, data));
		flag |= list->flag;
	}

	if	(!flag)	return;

/*	Selektionsindizes setzen
*/
	for (x = ctab->axis; x && x->priv; x = x->next)
	{
		flag = 0;

		for (s = x->priv; s != NULL; s = s->next)
		{
			s->idx = MdClassify(s->cdef, data);
			s->flag = (s->idx >= 0 && s->idx < s->cdef->dim);
			flag |= s->flag;
		}

		if	(!flag)	return;
	}

/*	Addition durchführen
*/
	list = ctab->x_priv;

	if	(md_count_hook)
		md_count_hook(list, ctab);
	else
		md_count_add(list, ctab->axis, ctab->data);
}


/*	Additionshilfsprogramm
*/

void md_count_add (MdCountList *cl, mdaxis *x, char *ptr)
{
	if	(x && x->priv)
	{
		MdCntGrp *s;

		for (s = x->priv; s != NULL; s = s->next)
		{
			if	(s->flag)
				md_count_add(cl, x->next,
					ptr + s->idx * x->size);
	
			ptr += s->cdef->dim * x->size;
		}
	}
	else
	{
		for (; cl; cl = cl->next)
		{
			if	(cl->flag)
				cl->cnt->add(cl->cnt, ptr + cl->offset,
					cl->data);
		}
	}
}

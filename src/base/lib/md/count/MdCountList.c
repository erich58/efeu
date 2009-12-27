/*
:*:list of count objects
:de:Liste von Zählobjekten

$Copyright (C) 2004 Erich Frühstück
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
A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/mdcount.h>

#define	BSIZE	60

static char *cl_ident (const void *ptr)
{
	const MdCountPar *tab = ptr;
	return msprintf("%s[%d]", tab->reftype->label, tab->vtab.used);
}

static void cl_clean (void *ptr)
{
	MdCountList *list = ptr;
	rd_deref(list->next);
	memfree(list);
}

static RefType reftype = REFTYPE_INIT("MdCountList", cl_ident, cl_clean);

MdCountList *MdCountList_create (MdCount *cnt)
{
	MdCountList *list;
	EfiType *type;

	type = mdtype(cnt->type);
	list = memalloc(sizeof *list + type->size);
	list->cnt = cnt;
	list->type = type;
	list->offset = 0;
	list->flag = 0;
	list->next = NULL;
	list->data = (list + 1);
	return rd_init(&reftype, list);
}

MdCountList *MdCountList_add (MdCountList *list, MdCount *cnt)
{
	if	(!cnt)	return list;

	if	(list)
	{
		MdCountList *x = list;

		while (x->next)
			x = x->next;

		x->next = MdCountList_create(cnt);
		return list;
	}

	return MdCountList_create(cnt);
}

MdCountList *MdCountList_get (MdCountPar *tab, const char *def)
{
	MdCount *cnt;
	MdCountList *list;
	char *p;

	for (list = NULL; def != NULL; def = p)
	{
		if	((p = strchr(def, ',')))
		{
			char *s = mstrncpy(def, p - def);
			p++;

			if	((cnt = MdCount_get(tab, s)) == NULL)
			{
				dbg_error("md", "[mdmat:45]", "m", s);
				continue;
			}

			memfree(s);
		}
		else
		{
			if	((cnt = MdCount_get(tab, def)) == NULL)
			{
				dbg_error("md", "[mdmat:45]", "s", def);
				break;
			}
		}

		list = MdCountList_add(list, cnt);
	}

	return list;
}

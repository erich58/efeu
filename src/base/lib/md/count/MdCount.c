/*
:*:standard count objects
:de:Standardzählobjekte

$Copyright (C) 2002 Erich Frühstück
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

static int cmp_vtab (const void *pa, const void *pb)
{
	MdCount * const *a = pa;
	MdCount * const *b = pb;
	return mstrcmp((*a)->name, (*b)->name);
}

MdCount *MdCount_get (MdCountPar *tab, const char *name)
{
	MdCount buf, *key, **ptr;
	buf.name = (char *) name;
	key = &buf;
	ptr = tab ? vb_search(&tab->vtab, &key, cmp_vtab, VB_SEARCH) : NULL;
	return ptr ? *ptr : NULL;
}

void AddMdCount (MdCountPar *tab, void *entry)
{
	MdCount *key = entry;
	vb_search(&tab->vtab, &key, cmp_vtab, VB_REPLACE);
}

void MdCount_add (MdCountPar *tab, MdCount *entry, size_t dim)
{
	if	(tab)
	{
		for (; dim-- > 0; entry++)
		{
			MdCount *key = entry;
			vb_search(&tab->vtab, &key, cmp_vtab, VB_REPLACE);
		}
	}
	else	dbg_note("md", "[mdmat:301]", NULL);
}

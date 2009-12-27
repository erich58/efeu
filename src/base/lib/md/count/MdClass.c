/*
:*:standard classifications
:de:Standardklassifikationen

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

#define	BSIZE	1000

static char *rt_ident (const void *ptr)
{
	const MdClassTab *tab = ptr;
	return msprintf("%s[%d]", tab->reftype->label, tab->tab.used);
}

static void rt_clean (void *ptr)
{
	MdClassTab *tab = ptr;
	vb_clean(&tab->tab, NULL);
	memfree(tab);
}

static const RefType reftype = REFTYPE_INIT("MdClassTab", rt_ident, rt_clean);

MdClassTab* MdClassTab_create (void)
{
	MdClassTab *tab = memalloc(sizeof(MdClassTab));
	vb_init(&tab->tab, BSIZE, sizeof(MdClass *));
	return rd_init(&reftype, tab);
}

static int cmp (const void *pa, const void *pb)
{
	MdClass * const *a = pa;
	MdClass * const *b = pb;
	return mstrcmp((*a)->name, (*b)->name);
}

void MdClass_add (MdClassTab *tab, MdClass *entry, size_t dim)
{
	for (; dim-- > 0; entry++)
	{
		MdClass *key = entry;
		vb_search(&tab->tab, &key, cmp, VB_REPLACE);
	}
}

MdClass *MdClass_get (MdClassTab *tab, const char *name)
{
	MdClass buf, *key, **ptr;
	buf.name = (char *) name;
	key = &buf;
	ptr = vb_search(&tab->tab, &key, cmp, VB_SEARCH);
	return ptr ? *ptr : NULL;
}


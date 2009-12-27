/*
:*:count variables
:de:Zählvariablen

$Copyright (C) 2006 Erich Frühstück
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

#define	VBSIZE	60
#define	CBSIZE	1000

static char *rt_ident (const void *ptr)
{
	const MdCountTab *tab = ptr;
	return msprintf("%s[%d, %d]", tab->reftype->label,
		tab->vtab.used, tab->ctab.used);
}

static void rt_clean (void *ptr)
{
	MdCountTab *tab = ptr;
	vb_clean(&tab->vtab, NULL);
	vb_clean(&tab->ctab, NULL);
	memfree(tab);
}

static const RefType reftype = REFTYPE_INIT("MdCountTab", rt_ident, rt_clean);

MdCountTab* MdCountTab_create (void)
{
	MdCountTab *tab = memalloc(sizeof *tab);
	vb_init(&tab->vtab, VBSIZE, sizeof(MdCount *));
	vb_init(&tab->ctab, CBSIZE, sizeof(MdClass *));
	return rd_init(&reftype, tab);
}

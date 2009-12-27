/*
:*:count objects with data pointers
:de:Zählobjekte mit Datenpointer

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

MdPtrCount *MdPtrCount_copy (MdPtrCount *src, size_t dim,
	const char *ext, void *ptr)
{
	MdPtrCount *tg;
	size_t n;

	tg = lmalloc(dim * sizeof(MdPtrCount));

	for (n = 0; n < dim; n++)
	{
		tg[n] = src[n];
		tg[n].name = mstrpaste(".", src[n].name, ext);
		tg[n].ptr = ptr;
	}

	return tg;
}

void MdPtrCount_add (MdCountTab *tab, MdPtrCount *entry, size_t dim)
{
	if	(tab)
	{
		for (; dim-- > 0; entry++)
			MdCount_add(tab, (MdCount *) entry, 1);
	}
	else	dbg_note("md", "[mdmat:301]", NULL);
}

void MdPtrCount_xadd (MdCountTab *tab, MdPtrCount *entry, size_t dim,
	const char *ext, void *ptr)
{
	MdPtrCount_add(tab, MdPtrCount_copy(entry, dim, ext, ptr), dim);
}

/*
:*:classifications with data pointers
:de:Klassifikationsdefinitionen mit Datenpointer

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

MdPtrClass *MdPtrClass_copy (MdPtrClass *src, size_t dim,
	const char *ext, void *ptr)
{
	MdPtrClass *tg;
	size_t n;

	tg = lmalloc(dim * sizeof(MdPtrClass));

	for (n = 0; n < dim; n++)
	{
		tg[n] = src[n];
		tg[n].name = mstrpaste(".", src[n].name, ext);
		tg[n].ptr = ptr;
	}

	return tg;
}

void MdPtrClass_add (MdClassTab *tab, MdPtrClass *entry, size_t dim)
{
	for (; dim-- > 0; entry++)
		MdClass_add(tab, (MdClass *) entry, 1);
}

void MdPtrClass_xadd (MdClassTab *tab, MdPtrClass *entry, size_t dim,
	const char *ext, void *ptr)
{
	MdPtrClass_add(tab, MdPtrClass_copy(entry, dim, ext, ptr), dim);
}

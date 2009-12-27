/*
:de:Vektoren bezüglich gleicher Einträge reduzieren
:en:reduce array regarding to uniq entries

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
*/

#include <EFEU/vecbuf.h>
#include <EFEU/memalloc.h>

/*
Die Funktion |$1| reduziert den Vektor <base> der Größe <dim>
bezüglich gleicher Einträge.
*/

size_t vuniq (void *base, size_t dim, size_t size,
	int (*comp) (const void *a, const void *b))
{
	size_t n, i;
	char *last, *ptr;

	if	(dim < 2 || comp == NULL)
		return dim;

	last = base;
	ptr = last + size;

	for (n = 0, i = 1; i < dim; i++, ptr += size)
	{
		if	(comp(last, ptr) != 0)
		{
			last += size;
			n++;

			if	(last != ptr)
				memswap(last, ptr, size);
		}
	}

	return n + 1;
}

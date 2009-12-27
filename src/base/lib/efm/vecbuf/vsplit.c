/*
Vektoren aufspalten

$Copyright (C) 1999 Erich Frühstück
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

size_t vsplit (void *base, size_t dim, size_t size, test_t test)
{
	size_t i0, i1;
	char *p0, *p1;

	if	(dim == 0)	return 0;
	if	(test == NULL)	return dim;

	i0 = 0;
	p0 = base;
	i1 = dim - 1;
	p1 = p0 + i1 * size;

	do
	{
		while (i0 <= i1 && test(p0))
			i0++, p0 += size;

		while (i1 > i0 && !test(p1))
			i1--, p1 -= size;

		if	(i0 < i1)
		{
			memswap(p0, p1, size);
			p0 += size;
			p1 -= size;
			i0++;
			i1--;
		}
		else	break;
	}
	while (i0 <= i1);

	return i0;
}

/*
Zufallsauswahl

$Copyright (C) 2001 Erich Frühstück
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

#include <EFEU/Random.h>
#include <math.h>

size_t RandomChoice (Random *rd, void *base, size_t dim, size_t size,
	size_t lim)
{
	unsigned n, k;
	unsigned char *ptr;

	if	(dim <= lim)	return lim;
	if	(lim == 0)	return 0;

	ptr = base;

	for (n = 0; n < lim; n++)
	{
		k = RandomIndex(rd, dim);

		if	(k)
		{
			unsigned char *a = ptr;
			unsigned char *b = ptr + k * size;
			size_t s = size;

			while (s--)
			{
				unsigned char c = *a;
				*a++ = *b;
				*b++ = c;
			}
		}

		ptr += size;
		dim--;
	}

	return lim;
}

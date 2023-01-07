/*
:*:memmory allocation with exit on error
:de:Speicherplatzanforderung mit Abbruchg bei Fehler

$Copyright (C) 2023 Erich Frühstück
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

#include <EFEU/memalloc.h>

#define	ERR_ALLOC	"sorry: realloc(%p, %zu) failed\n"

/*
Falls die Speicherplatzanforderung fehlschlägt, kann nicht
auf die Standardfehlerroutinen zurückgegriffen werden, da
diese eventuell selbst ein Speichersegment anfordern müssen.
*/

void *realloc_errcheck(void *data, size_t size)
{
	if	(!data && !size)
		return NULL;

	void *ndata = realloc(data, size);

	if	(size && !ndata)
	{
		fprintf(stderr, ERR_ALLOC, data, size);
		exit(EXIT_FAILURE);
	}

	return ndata;
}

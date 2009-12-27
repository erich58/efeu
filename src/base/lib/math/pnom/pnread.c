/*
Lesen von Polynomdaten aus einem File

$Copyright (C) 1991 Erich Frühstück
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

#include <EFEU/efmain.h>
#include <Math/pnom.h>


Polynom *pnread(FILE *file)
{
	Polynom *p;
	int n, k;
	int i, j;

	if	(fscanf(file, "%d%d", &n, &k) != 2)
	{
		dbg_error(NULL, "[pnom:11]", NULL);
	}

	p = pnalloc((size_t) n, (size_t) k);

	for (i = 0; i < n; ++i)
	{
		if	(fscanf(file, "%lf", p->x + i) != 1)
		{
			dbg_error(NULL, "[pnom:11]", NULL);
		}

		for (j = 0; j <= k; ++j)
		{
			if	(fscanf(file, "%lf", p->c[i] + j) != 1)
			{
				dbg_error(NULL, "[pnom:11]", NULL);
			}
		}
	}

	return p;
}

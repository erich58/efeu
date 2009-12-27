/*
Matrixinversion mit Gauß-Jordan-Algorithmus

$Copyright (C) 1997 Erich Frühstück
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

#include <Math/linalg.h>
#include <EFEU/memalloc.h>

size_t GaussJordan (double *data, size_t dim)
{
	int *idx;
	int i, j, k;
	double *line, *ptr, m, x;

	idx = memalloc(dim * sizeof(int));

	for (j = 0; j < dim; j++)
		idx[j] = j;

	for (j = 0; j < dim; j++)
	{
		line = data + j * dim;

	/*	Pivotsuche
	*/
		ptr = line + j;
		k = j;
		m = abs(*ptr);

		for (i = j + 1; i < dim; i++)
		{
			ptr += dim;
			x = abs(*ptr);

			if	(m < x)
			{
				m = x;
				k = i;
			}
		}

	/*	Zeilentausch
	*/
		if	(k > j)
		{
			ptr = data + k * dim;

			for (i = 0; i < dim; i++)
			{
				x = line[i];
				line[i] = ptr[i];
				ptr[i] = x;
			}

			i = idx[j];
			idx[j] = idx[k];
			idx[k] = i;
		}

	/*	Transformation
	*/
		if	(line[j] == 0.)	return j; /* Singuläre Matrix */

		x = 1. / line[j];

		ptr = data + j;

		for (i = 0; i < dim; i++, ptr += dim)
			*ptr *= x;

		line[j] = x;

		for (k = 0; k < dim; k++)
		{
			if	(k == j)	continue;

			for (i = 0; i < dim; i++)
			{
				if	(i == j)	continue;

				ptr = data + i * dim;
				ptr[k] -= ptr[j] * line[k];
			}

			line[k] *= -x;
		}
	}

/*	Spaltentausch
*/
	ptr = memalloc(dim * sizeof(double));
	line = data;

	for (i = 0; i < dim; i++)
	{
		for (k = 0; k < dim; k++)
			ptr[idx[k]] = line[k];

		for (k = 0; k < dim; k++)
			line[k] = ptr[k];

		line += dim;
	}
	
	memfree(ptr);
	memfree(idx);
	return dim;
}

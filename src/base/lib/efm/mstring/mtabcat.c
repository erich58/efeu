/*
Zusammenfügen einer Tabelle von Strings

$Copyright (C) 1994 Erich Frühstück
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

#include <EFEU/mstring.h>


char *mtabcat (const char *delim, char **list, size_t dim)
{
	int i, j, n;
	char *x;

	if	(list == NULL || dim == 0)
		return NULL;

/*	Bestimmung des Speicherbedarfs
*/
	n = (delim != NULL ? (dim - 1) * strlen(delim) : 0);

	for (i = 0; i < dim; i++)
		if (list[i] != NULL) n += strlen(list[i]);

/*	Kopieren der Strings
*/
	x = memalloc(n + 1);
	n = 0;

	for (i = 0; i < dim; i++)
	{
		if	(i > 0 && delim != NULL)
		{
			for (j = 0; delim[j] != 0; j++)
				x[n++] = delim[j];
		}

		if	(list[i] != NULL)
		{
			for (j = 0; list[i][j] != 0; j++)
				x[n++] = list[i][j];
		}
	}

	x[n] = 0;
	return x;
}

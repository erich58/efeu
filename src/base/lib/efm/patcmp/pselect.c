/*
Selektion mit Musterliste

$Copyright (C) 1994 Erich Fr�hst�ck
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


#include <EFEU/patcmp.h>


int patselect(const char *name, char **list, size_t dim)
{
	int flag;	/* Selektionsflag */
	int mode;	/* Selektionstyp */
	char *pat;	/* Selektionsmuster */
	int i;		/* Hilfsz�hler */

	flag = 1;

	for (i = 0; i < dim; i++)
	{
		pat = list[i];

		switch (*pat)
		{
		case '+':	mode = 1; pat++; break;
		case '-':	mode = 0; pat++; break;
		default:	mode = 1; break;
		}

		if	(i > 0 && mode == flag)	continue;

		flag = (patcmp(pat, name, NULL) ? mode : !mode);
	}

	return flag;
}

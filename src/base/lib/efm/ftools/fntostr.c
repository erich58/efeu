/*
Umwandeln einer fname_t - Struktur in einen Filenamen

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

#include <EFEU/ftools.h>

char *fntostr (const fname_t *fname)
{
	char *p[5];
	size_t n;

	if	(fname == NULL || fname->name == NULL)
	{
		return NULL;
	}

	n = 0;

	if	(fname->path != NULL)
	{
		p[n++] = fname->path;
		p[n++] = "/";
	}

	p[n++] = fname->name;

	if	(fname->type != NULL)
	{
		p[n++] = ".";
		p[n++] = fname->type;
	}

	return listcat(NULL, p, n);
}

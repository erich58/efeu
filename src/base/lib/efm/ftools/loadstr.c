/*
String einlesen

$Copyright (C) 1996 Erich Frühstück
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
#include <EFEU/ftools.h>

char *loadstr (FILE *file)
{
	register size_t size;
	register char *str;
	
	if	((size = get2byte(file)) == 0)
		return NULL;

	str = memalloc(size);

	if	(fread(str, 1, size, file) != size)
	{
		memfree(str);
		fprintf(stderr, "loadstr: Unerwartetes Fileende.\n");
		exit(1);
	}

	return str;
}

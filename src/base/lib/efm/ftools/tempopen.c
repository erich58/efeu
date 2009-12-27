/*
Tempor�re Datei �ffnen

$Copyright (C) 1999 Erich Fr�hst�ck
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
#include <EFEU/procenv.h>
#include <EFEU/Debug.h>

#define	FMT_ERR	"[ftools:5]$!: could not create temporary file.\n"

FILE *tempopen (void)
{
	FILE *file;

	if	((file = tmpfile()) == NULL)
	{
		dbg_error(NULL, FMT_ERR, NULL);
		return NULL;
	}
	
	filenotice("<temp>", "w+b", file, fclose);
	return file;
}


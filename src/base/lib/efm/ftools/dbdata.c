/*
Ein/Ausgabefunktionen

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

#include <EFEU/ftools.h>

#define E21	"[ftools:21]$!: file $0: unexpected EOF.\n"
#define E22	"[ftools:22]$!: file $0: could not write data.\n"

/*	Datensätze lesen
*/

size_t dbread (FILE *file, void *dptr, size_t size, size_t n)
{
	if	(fread_compat(dptr, size, n, file) != n)
		fileerror(file, E21, NULL);

	return n;
}


/*	Datensätze ausgeben
*/

size_t dbwrite (FILE *file, const void *dptr, size_t size, size_t n)
{
	if	(fwrite_compat(dptr, size, n, file) != n)
		fileerror(file, E22, NULL);

	return n;
}

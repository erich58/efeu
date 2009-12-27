/*
Vektorbuffer aus Datei laden

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

#include <EFEU/vecbuf.h>
#include <EFEU/ftools.h>
#include <EFEU/procenv.h>

#define E21	"[ftools:21]$!: file $0: unexpected EOF.\n"
#define E22	"[ftools:22]$!: file $0: could not write data.\n"

size_t loadvec (FILE *file, void *ptr, size_t size, size_t dim)
{
	size_t n = fread_compat(ptr, size, dim, file);

	if	(n != dim)
		fileerror(file, E21, NULL);

	return n;
}

size_t savevec (FILE *file, const void *ptr, size_t size, size_t dim)
{
	size_t n = fwrite_compat(ptr, size, dim, file);

	if	(n != dim)
		fileerror(file, E22, NULL);

	return n;
}

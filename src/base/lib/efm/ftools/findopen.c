/*
Datei suchen und zum Lesen öffnen

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
#include <EFEU/procenv.h>


FILE *findopen (const char *path, const char *pfx,
	const char *name, const char *ext, const char *mode)
{
	char *fname = fsearch(path, pfx, name, ext);

	if	(fname != NULL)
	{
		FILE *file = fileopen(fname, mode);
		memfree(fname);
		return file;
	}

	fname = mstrpaste(".", name ? name : "", ext);
	message("findopen", MSG_FTOOLS, 6, 1, fname);
	memfree(fname);
	exit(EXIT_FAILURE);
	return NULL;
}

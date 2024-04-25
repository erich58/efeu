/*
EDB-Datentype aus String bestimmen

$Copyright (C) 2006 Erich Frühstück
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

#include <EFEU/EDB.h>
#include <EFEU/preproc.h>
#include <ctype.h>

EfiType *edb_type (const char *def)
{
 	EfiType *type;
	IO *io;
	int c;

	if	(!def)	return NULL;

	io = io_cmdpreproc(io_cstr(def));
	type = Parse_type(io, NULL);

	c = io_eat(io, " \t");

	if	(isalpha(c) || c == '_')
	{
		EfiStruct *var = GetStructEntry(io, type);

		if	(var)
		{
			var = GetStruct(var, io, EOF);
			type = MakeStruct(NULL, NULL, var);
		}
	}

	io_close(io);
	return type;
}

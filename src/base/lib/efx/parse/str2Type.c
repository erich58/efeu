/*
Datentype aus String bestimmen

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

#include <EFEU/object.h>
#include <EFEU/preproc.h>
#include <EFEU/parsedef.h>
#include <ctype.h>

EfiType *str2Type (const char *def)
{
	if	(!def)	return NULL;

	IO *io = io_cstr(def);
	EfiType *type = Parse_type(io, NULL);
	int c = io_eat(io, " \t");

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

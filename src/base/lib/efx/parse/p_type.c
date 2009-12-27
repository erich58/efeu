/*
Datentype bestimmen

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

#include <EFEU/object.h>

EfiType *Parse_type(IO *io, EfiType *type)
{
	EfiObj *obj;

	if	(type == NULL)
	{
		obj = Parse_obj(io, SCAN_NAME);

		if	(obj && obj->type == &Type_type)
			type = Val_type(obj->data);

		UnrefObj(obj);
	}

	if	(type == NULL)
		io_error(io, "[efmain:123]", NULL);

	while (io_eat(io, " \t") == '[')
	{
		io_getc(io);
		obj = EvalObj(Parse_index(io), &Type_int);

		if	(obj == NULL)	return NULL;

		type = NewVecType(type, Val_int(obj->data));
		UnrefObj(obj);
	}

	return type;
}

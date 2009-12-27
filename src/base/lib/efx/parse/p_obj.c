/*
Parser für Objekte

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
#include <EFEU/parsedef.h>
#include <ctype.h>


/*	Basisobjekt ohne Operatoren
*/

EfiObj *Parse_obj(IO *io, int flags)
{
	void *p;
	EfiObj *obj;
	EfiParseDef *parse;
	EfiType *type;
	int mode;

	obj = NULL;
	p = NULL;
	mode = io_scan(io, flags, &p);

	switch (mode & SCAN_TYPEMASK)
	{
	case SCAN_INT:

		return (mode & SCAN_UNSIGNED) ?
			NewObj(&Type_uint, p) : NewObj(&Type_int, p);

	case SCAN_INT64:

		return (mode & SCAN_UNSIGNED) ?
			NewObj(&Type_varsize, p) : NewObj(&Type_varint, p);

	case SCAN_FLOAT:

		return NewObj(&Type_float, p);

	case SCAN_DOUBLE:

		return NewObj(&Type_double, p);

	case SCAN_STR:

		return str2Obj(p);

	case SCAN_CHAR:

		return NewObj(&Type_char, p);

	case SCAN_UCS:

		return NewObj(&Type_wchar, p);

	case SCAN_NAME:		

		if	(p == NULL)
		{
			return ptr2Obj(NULL);
		}
		else if	((type = GetType(p)) != NULL)
		{
			obj = PFunc_type(io, type);
		}
		else if	((parse = GetParseDef(p)) != NULL)
		{
			obj = (*parse->func)(io, parse->data);
		}
		else	return NewPtrObj(&Type_name, p);

		break;

	default:

		break;
	}

	memfree(p);
	return obj;
}

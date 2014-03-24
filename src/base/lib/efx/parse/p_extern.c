/*
Externa Schlüsselwort

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

#include <EFEU/object.h>
#include <EFEU/Op.h>
#include <EFEU/parsedef.h>

EfiObj *PFunc_extern (IO *io, void *arg)
{
	EfiObj *obj;
	EfiParseDef *parse;
	void *p;

	if	(!io_scan(io, SCAN_NAME, &p))
		return NULL;

	if	(mstrcmp(p, "struct") == 0)
	{
		obj = PFunc_struct(io, "extern");
	}
	else if	(mstrcmp(p, "enum") == 0)
	{
		obj = PFunc_enum(io, "extern");
	}
	else if	((parse = GetParseDef(p)) != NULL)
	{
		obj = (*parse->func)(io, parse->data);
	}
	else
	{
		EfiType *type = XGetType(p);

		if	(!type)
			return NewPtrObj(&Type_name, p);

		obj = PFunc_type(io, type);
	}

	memfree(p);
	return obj;
}

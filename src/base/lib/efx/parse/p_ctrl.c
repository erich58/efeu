/*
Kontrollfunktionen

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

EfiObj *PFunc_for (IO *io, void *data)
{
	EfiObj *a, *b, *c, *d;

	if	(!Parse_fmt(io, "(T", &a))
	{
		return NULL;
	}

	io_eat(io, " \t");

	if	(io_testkey(io, "in"))
	{
		EfiObjList *list;

		if	(a->type != &Type_name)
		{
			io_error(io, "[efmain:95]", NULL);
		}

		list = Parse_list(io, ')');

		if	(ObjListLen(list) == 1)
		{
			b = list->obj;
			list->obj = NULL;
			DelObjList(list);
		}
		else	b = Obj_list(list);

		if	(Parse_fmt(io, " C", &c))
			return Obj_call(Expr_forin, NULL, MakeObjList(3, a, b, c));

		UnrefObj(b);
	}
	else if	(Parse_fmt(io, "; T; T) C", &b, &c, &d))
	{
		return Obj_call(Expr_for, NULL, MakeObjList(4, a, b, c, d));
	}
	
	UnrefObj(a);
	return NULL;
}

EfiObj *PFunc_while (IO *io, void *data)
{
	EfiObj *a, *b;

	if	(Parse_fmt(io, "(T) C", &a, &b))
	{
		return Obj_call(Expr_for, NULL, MakeObjList(4, NULL, a, NULL, b));
	}
	else	return NULL;
}

EfiObj *PFunc_do (IO *io, void *data)
{
	EfiObj *a, *b;

	if	(Parse_fmt(io, "C while(T)", &a, &b))
	{
		return Obj_call(Expr_for, NULL,
			MakeObjList(4, a, b, NULL, RefObj(a)));
	}
	else	return NULL;
}

EfiObj *PFunc_if (IO *io, void *data)
{
	EfiObj *a, *b, *c;

	if	(!Parse_fmt(io, "(T) C ", &a, &b))
		return NULL;

	if	(!io_testkey(io, "else"))
	{
		if	(io_peek(io) != ']')
			io_ungetc('\n', io);	/* !!! */
		c = NULL;
	}
	else	c = Parse_cmd(io);

	return Obj_call(Expr_void, NULL, NewObjList(CondTerm(a, b, c)));
}


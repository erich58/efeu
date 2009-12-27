/*
Typedefinition

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
#include <EFEU/Op.h>
#include <EFEU/parsedef.h>

/*	Typedefinition: Falls kein Vektortype angegeben ist, wird nur
	ein Aliasname generiert.
*/

void CopyDataFunc (EfiFunc *func, void *rval, void **arg)
{
	CopyData(func->type, rval, arg[0]);
}

extern void CopyKonv(EfiType *type, EfiType *base);

EfiObj *PFunc_typedef(IO *io, void *arg)
{
	EfiType *type;
	EfiVar *st;
	char *def;

	if	((st = GetStructEntry(io, NULL)) == NULL)
		return NULL;

	type = NewType(st->name);
	st->name = NULL;
	type->base = st->dim ? NewVecType(st->type, st->dim) : st->type;
	type->copy = type->base->copy;
	type->clean = type->base->clean;
	type->destroy = type->base->destroy;
	type->size = type->base->size;
	type->recl = type->base->recl;
	type->read = type->base->read;
	type->write = type->base->write;
	type->defval = st->data;
	st->data = NULL;
	DelVar(st);

	CopyKonv(type, type->base);
	AddType(type);
	def = msprintf("%s ()", type->name);
	SetFunc(FUNC_PROMOTION, type->base, def, CopyDataFunc);
	memfree(def);
	return type2Obj(type);
}


/*	Strukturkomponente bestimmen
*/

EfiVar *GetStructEntry(IO *io, EfiType *type)
{
	EfiVar *st;
	EfiObj *obj;
	void *name;
	size_t dim;
	int n;
	int c;

	if	((type = Parse_type(io, type)) == NULL)
		return NULL;

	if	((name = io_getname(io)) == NULL)
	{
		io_error(io, "[efmain:123]", NULL);
		return NULL;
	}

	dim = 0;

	while ((c = io_eat(io, " \t")) == '[')
	{
		io_getc(io);

		if	((io_eat(io, " \t") == ']'))
		{
			io_getc(io);
			type = NewVecType(type, 0);
			continue;
		}

		obj = EvalObj(Parse_index(io), &Type_int);
		n = obj ? Val_int(obj->data) : 0;
		UnrefObj(obj);

		if	(n <= 0)
		{
			io_error(io, "[efmain:124]", "m", name);
			return NULL;
		}
		else if	(dim)
		{
			type = NewVecType(type, n);
		}
		else	dim = n;
	}

	st = NewVar(type, name, dim);
	st->member = StructMember;
	st->offset = 0;

	if	(c == '=')
	{
		io_getc(io);
		Obj2Data(Parse_term(io, OpPrior_Assign), st->type, st->data);
	}

	return st;
}


/*	Strukturliste generieren
*/

EfiVar *GetStruct (IO *io, int delim)
{
	EfiType *type;
	EfiVar *st, **ptr;
	int c;

	type = NULL;
	st = NULL;
	ptr = &st;

	while ((c = io_eat(io, " \t")) != delim)
	{
		switch (c)
		{
		case EOF:

			DelVar(st);
			io_error(io, "[efmain:121]", NULL);
			return NULL;

		case '\n':
		case ';':	type = NULL; /* FALLTHROUGH */
		case ',':	io_getc(io); continue;
		default:	break;
		}

		if	((*ptr = GetStructEntry(io, type)) == NULL)
		{
			DelVar(st);
			return NULL;
		}

		type = (*ptr)->type;
		ptr = &(*ptr)->next;
	}

	return st;
}

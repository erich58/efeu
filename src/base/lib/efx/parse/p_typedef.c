/*
Typedefinition

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

EfiObj *PFunc_typedef (IO *io, void *arg)
{
	EfiType *type;
	EfiStruct *st;
	char *def;

	if	((st = GetStructEntry(io, NULL)) == NULL)
		return NULL;

	type = NewType(st->name);
	st->name = NULL;
	type->base = st->dim ? NewVecType(st->type, st->dim) : st->type;
	type->copy = type->base->copy;
	type->clean = type->base->clean;
	type->size = type->base->size;
	type->recl = type->base->recl;
	type->read = type->base->read;
	type->write = type->base->write;
	AddType(type);

	CopyData(type, type->defval,
		st->defval ? st->defval->data : type->base->defval);
	rd_deref(st);

	CopyKonv(type, type->base);
	def = msprintf("%s ()", type->name);
	SetFunc(FUNC_PROMOTION, type->base, def, CopyDataFunc);
	memfree(def);
	return type2Obj(type);
}


/*	Strukturkomponente bestimmen
*/

EfiStruct *GetStructEntry (IO *io, EfiType *type)
{
	EfiStruct *st;
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

	st = NewEfiStruct(type, name, dim);
	st->offset = 0;

	if	(c == '=')
	{
		io_getc(io);
		st->defval = Parse_term(io, OpPrior_Assign);
		
		if	(dim)
		{
			st->defval = EvalObj(st->defval,
				NewVecType(st->type, st->dim));
		}
		else	st->defval = EvalObj(st->defval, st->type);
	}

	return st;
}


/*	Strukturliste generieren
*/

EfiStruct *GetStruct (IO *io, int delim)
{
	EfiType *type;
	EfiStruct *st, **ptr;
	int c;

	type = NULL;
	st = NULL;
	ptr = &st;

	while ((c = io_eat(io, " \t")) != delim)
	{
		switch (c)
		{
		case EOF:

			rd_deref(st);
			io_error(io, "[efmain:121]", NULL);
			return NULL;

		case '\n':
		case ';':	type = NULL; /* FALLTHROUGH */
		case ',':	io_getc(io); continue;
		default:	break;
		}

		if	((*ptr = GetStructEntry(io, type)) == NULL)
		{
			rd_deref(st);
			return NULL;
		}

		type = (*ptr)->type;
		ptr = &(*ptr)->next;
	}

	return st;
}

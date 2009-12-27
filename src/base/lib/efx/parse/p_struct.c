/*
Struktur generieren

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

#define	PROMPT	"struct { >>> "

#define	ALIGN(x, y)	((y) * (((x) + (y) - 1) / (y)))

static void StClean (const Type_t *type, void *tg);
static void StCopy (const Type_t *type, void *tg, const void *src);
static size_t StIOData (const Type_t *t, iofunc_t f, void *p, void *d, size_t n);


Obj_t *PFunc_struct(io_t *io, void *data)
{
	Type_t *type;
	Type_t *base;
	Var_t *st;
	void *p;
	char *prompt;
	char *bname;
	int c;

	p = io_getname(io);
	c = io_eat(io, "%s");
	bname = NULL;

	if	(c == ':')
	{
		io_getc(io);
		base = Parse_type(io, NULL);
		bname = io_getname(io);
		c = io_eat(io, "%s");
	}
	else	base = NULL;

	if	(c != '{')
	{
		io_error(io, MSG_EFMAIN, 156, 1, p);
		memfree(p);
		return NULL;
	}

	io_getc(io);
	prompt = io_prompt(io, PROMPT);
	st = GetStruct(io, '}');
	io_prompt(io, prompt);

	if	(st == NULL)	return NULL;

	io_getc(io);

	if	(base)
	{
		Var_t *bvar = NewVar(base, NULL, 0);
		bvar->name = bname;
		bvar->member = StructMember;
		type = MakeStruct(p, bvar, st);
	}
	else	type = MakeStruct(p, NULL, st);

	return type2Obj(type);
}


static void ExpandKonv (Func_t *func, void *rval, void **arg)
{
	CopyData(func->arg[0].type, rval, arg[0]);
}


Type_t *MakeStruct(char *name, Var_t *base, Var_t *list)
{
	Type_t *type;
	Var_t *st;
	size_t tsize, size, recl;
	int vrecl;
	Clean_t clean;
	Copy_t copy;
	IOData_t iodata;
	char *p;
	
	tsize = 0;
	copy = NULL;
	clean = NULL;
	iodata = StIOData;
	recl = 0;
	vrecl = 0;

	if	(base)
	{
		base->next = list;
		list = base;
	}

	for (st = list; st != NULL; st = st->next)
	{
		if	(st->type->copy)	copy = StCopy;
		if	(st->type->clean)	clean = StClean;
		if	(!st->type->iodata)	iodata = NULL;

		if	(st->type->recl)
		{
			recl += st->type->recl * (st->dim ? st->dim : 1);
		}
		else	vrecl = 1;

		size = st->type->size * (st->dim ? st->dim : 1);

		if	(st->type->size < sizeof(size_t))
			st->offset = ALIGN(tsize, st->type->size);
		else	st->offset = ALIGN(tsize, sizeof(size_t));

		tsize = st->offset + size;
	}

	tsize = ALIGN(tsize, sizeof(size_t));

/*	Test, ob Struktur bereits definiert ist
*/
	if	((type = FindStruct(list, tsize)) != NULL)
	{
		if	(name == NULL || mstrcmp(name, type->name) == 0)
			return type;
	}

/*	Bei fehlenden Namen: Standardnamen generieren
*/
	if	(name == NULL)
	{
		strbuf_t *sb = new_strbuf(0);
		io_t *io = io_strbuf(sb);

		for (st = list; st != NULL; st = st->next)
			io_printf(io, "_%s%d", st->type->name, st->dim);

		io_close(io);
		name = sb2str(sb);
	}

	type = NewType(name);
	type->size = tsize;
	type->recl = vrecl ? 0 : recl;
	type->iodata = iodata;
	type->eval = NULL;
	type->clean = clean;
	type->copy = copy;
	type->list = list;
	type->base = base ? base->type : NULL;
	type->vtab = VarTab(mstrcpy(name), 0);

	for (st = list; st != NULL; st = st->next)
		AddVar(type->vtab, st, 1);

	AddType(type);
	p = msprintf("%s ()", type->name);
	SetFunc(FUNC_RESTRICTED, &Type_list, p, Struct2List);
	memfree(p);
	p = msprintf("%s (List_t)", type->name);
	SetFunc(0, type, p, List2Struct);
	memfree(p);

	if	(type->base)
	{
		p = msprintf("%s (%s)", type->name, type->base->name);
		SetFunc(0, type, p, ExpandKonv);
		memfree(p);
	}

	return type;
}



/*	Kopierfunktion für Strukturtypen
*/

static void StCopy(const Type_t *type, void *tg, const void *src)
{
	Var_t *st;

	for (st = type->list; st != NULL; st = st->next)
	{
		if	(st->dim)
			CopyVecData(st->type, st->dim, (char *) tg + st->offset,
				(const char *) src + st->offset);
		else	CopyData(st->type, (char *) tg + st->offset,
				(const char *) src + st->offset);
	}
}


/*	Löschfunktion für Strukturtypen
*/

static void StClean(const Type_t *type, void *tg)
{
	Var_t *st;

	for (st = type->list; st != NULL; st = st->next)
	{
		if	(st->dim)
			CleanVecData(st->type, st->dim, (char *) tg + st->offset);
		else	CleanData(st->type, (char *) tg + st->offset);
	}
}

static size_t StIOData (const Type_t *type, iofunc_t f, void *p, void *d, size_t n)
{
	Var_t *st;
	size_t recl;

	for (recl = 0; n > 0; n--, d = (char *) d + type->size)
	{
		for (st = type->list; st != NULL; st = st->next)
		{
			void *dptr = (char *) d + st->offset;

			if	(st->dim)
				recl += IOVecData(st->type, f, p, st->dim, dptr);
			else	recl += IOData(st->type, f, p, dptr);
		}
	}

	return recl;
}

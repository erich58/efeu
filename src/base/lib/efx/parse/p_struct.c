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

static void StDestroy (const EfiType *type, void *tg);
static void StClean (const EfiType *type, void *tg);
static void StCopy (const EfiType *type, void *tg, const void *src);
static size_t StRead (const EfiType *type, void *data, IO *io);
static size_t StWrite (const EfiType *type, const void *data, IO *io);


EfiObj *PFunc_struct(IO *io, void *data)
{
	EfiType *type;
	EfiType *base;
	EfiVar *st;
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
		io_error(io, "[efmain:156]", "m", p);
		memfree(bname);
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
		EfiVar *bvar = NewVar(base, NULL, 0);
		bvar->name = bname;
		bvar->member = StructMember;
		type = MakeStruct(p, bvar, st);
	}
	else	type = MakeStruct(p, NULL, st);

	return type2Obj(Parse_type(io, type));
}


static void ExpandKonv (EfiFunc *func, void *rval, void **arg)
{
	CopyData(func->arg[0].type, rval, arg[0]);
}


EfiType *MakeStruct(char *name, EfiVar *base, EfiVar *list)
{
	EfiType *type;
	EfiVar *st;
	size_t tsize, size, recl;
	int vrecl;
	void (*destroy) (const EfiType *type, void *data);
	void (*clean) (const EfiType *type, void *data);
	void (*copy) (const EfiType *type, void *tg, const void *src);
	size_t (*read) (const EfiType *type, void *data, IO *io);
	size_t (*write) (const EfiType *type, const void *data, IO *io);
	char *p;
	
	if	(!list)
		return &Type_void;

	tsize = 0;
	copy = NULL;
	clean = NULL;
	destroy = NULL;
	read = StRead;
	write = StWrite;
	recl = 0;
	vrecl = 0;

	if	(base)
	{
		base->next = list;
		list = base;
	}

	for (st = list; st != NULL; st = st->next)
	{
		st->member = StructMember;

		if	(st->type->copy)	copy = StCopy;
		if	(st->type->clean)	clean = StClean;
		if	(st->type->destroy)	destroy = StDestroy;

		if	(!st->type->recl)
		{
			if	(!st->type->read)	read = NULL;
			if	(!st->type->write)	write = NULL;

			vrecl = 1;
		}
		else	recl += st->type->recl * (st->dim ? st->dim : 1);

		size = st->type->size * (st->dim ? st->dim : 1);

		if	(st->type->size > sizeof(size_t))
			st->offset = ALIGN(tsize, sizeof(size_t));
		else if	(st->type->size)
			st->offset = ALIGN(tsize, st->type->size);
		else	st->offset = tsize;

		tsize = st->offset + size;
	}

	tsize = ALIGN(tsize, sizeof(size_t));

/*	Test, ob Struktur bereits definiert ist
*/
	if	((type = FindStruct(list, tsize)) != NULL)
	{
		if	(name == NULL || mstrcmp(name, type->name) == 0)
		{
			DelVarList(list);
			return type;
		}
	}

/*	Bei fehlenden Namen: Standardnamen generieren
*/
	if	(name == NULL)
	{
		StrBuf *sb = sb_create(0);
		IO *io = io_strbuf(sb);

		for (st = list; st != NULL; st = st->next)
			io_printf(io, "_%s_%s_%d", st->type->name,
				st->name, st->dim);

		io_close(io);
		name = sb2str(sb);
	}

	type = NewType(name);
	type->size = tsize;
	type->recl = vrecl ? 0 : recl;
	type->read = read;
	type->write = write;
	type->eval = NULL;
	type->destroy = destroy;
	type->clean = clean;
	type->copy = copy;
	type->list = list;
	type->base = base ? base->type : NULL;
	type->vtab = VarTab(mstrcpy(name), 0);
	type->defval = memalloc(type->size);

	for (st = list; st != NULL; st = st->next)
	{
		p = (char *) type->defval + st->offset;

		if	(st->dim)
		{
			CopyVecData(st->type, st->dim, p, st->data);
		}
		else	CopyData(st->type, p, st->data);

		AddVar(type->vtab, st, 1);
	}

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

void AddMemberFunc (EfiType *type)
{
	EfiVar *st;
	int vrecl;
	size_t recl;
	char *p;

	void (*destroy) (const EfiType *type, void *data);
	void (*clean) (const EfiType *type, void *data);
	void (*copy) (const EfiType *type, void *tg, const void *src);
	size_t (*read) (const EfiType *type, void *data, IO *io);
	size_t (*write) (const EfiType *type, const void *data, IO *io);

	if	(!type->list)	return;

	copy = NULL;
	clean = NULL;
	destroy = NULL;
	read = StRead;
	write = StWrite;
	recl = 0;
	vrecl = 0;

	for (st = type->list; st != NULL; st = st->next)
	{
		if	(st->type->copy)	copy = StCopy;
		if	(st->type->clean)	clean = StClean;
		if	(st->type->destroy)	destroy = StDestroy;

		if	(!st->type->recl)
		{
			if	(!st->type->read)	read = NULL;
			if	(!st->type->write)	write = NULL;

			vrecl = 1;
		}
		else	recl += st->type->recl * (st->dim ? st->dim : 1);
	}

	type->recl = vrecl ? 0 : recl;
	type->read = read;
	type->write = write;
	type->copy = copy;
	type->clean = clean;
	type->destroy = destroy;

	p = msprintf("%s ()", type->name);
	SetFunc(FUNC_RESTRICTED, &Type_list, p, Struct2List);
	memfree(p);
	p = msprintf("%s (List_t)", type->name);
	SetFunc(0, type, p, List2Struct);
	memfree(p);
}


/*	Kopierfunktion für Strukturtypen
*/

static void StCopy(const EfiType *type, void *tg, const void *src)
{
	EfiVar *st;

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

static void StDestroy(const EfiType *type, void *tg)
{
	EfiVar *st;

	for (st = type->list; st != NULL; st = st->next)
	{
		if	(st->dim)
			DestroyVecData(st->type, st->dim,
				(char *) tg + st->offset);
		else	DestroyData(st->type, (char *) tg + st->offset);
	}
}


static void StClean(const EfiType *type, void *tg)
{
	EfiVar *st;

	for (st = type->list; st != NULL; st = st->next)
	{
		if	(st->dim)
			CleanVecData(st->type, st->dim,
				(char *) tg + st->offset);
		else	CleanData(st->type, (char *) tg + st->offset);
	}
}

static size_t StRead (const EfiType *type, void *data, IO *io)
{
	EfiVar *st;
	size_t recl;

	for (recl = 0, st = type->list; st != NULL; st = st->next)
	{
		void *dptr = (char *) data + st->offset;

		if	(st->dim)
		{
			recl += ReadVecData(st->type, st->dim, dptr, io);
		}
		else	recl += ReadData(st->type, dptr, io);
	}

	return recl;
}

static size_t StWrite (const EfiType *type, const void *data, IO *io)
{
	EfiVar *st;
	size_t recl;

	for (recl = 0, st = type->list; st != NULL; st = st->next)
	{
		void *dptr = (char *) data + st->offset;

		if	(st->dim)
		{
			recl += WriteVecData(st->type, st->dim, dptr, io);
		}
		else	recl += WriteData(st->type, dptr, io);
	}

	return recl;
}

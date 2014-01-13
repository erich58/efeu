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
#include <EFEU/stdtype.h>

#define	PROMPT	"struct { >>> "

#define	ALIGN(x, y)	((y) * (((x) + (y) - 1) / (y)))

static int check_struct (const EfiType *type, EfiStruct *base, EfiStruct *list);


EfiObj *PFunc_struct(IO *io, void *data)
{
	EfiType *type;
	EfiStruct *st;
	EfiStruct *bvar;
	void *p;
	char *prompt;
	int c;

	p = io_getname(io);

	if	(p == NULL)	type = NULL;
	else if	(data)		type = XGetType(p);
	else			type = GetType(p);

	EfiType_version(type, io);
	c = io_eat(io, "%s");

	if	(c == ':')
	{
		io_getc(io);
		bvar = NewEfiStruct(Parse_type(io, NULL), NULL, 0);
		bvar->name = io_getname(io);
		c = io_eat(io, "%s");
	}
	else	bvar = NULL;

	if	(c != '{')
	{
		io_error(io, "[efmain:156]", "m", p);
		rd_deref(bvar);
		return NULL;
	}

	io_getc(io);
	prompt = io_prompt(io, PROMPT);
	st = GetStruct(NULL, io, '}');
	io_prompt(io, prompt);

	if	(st == NULL)	return NULL;

	io_getc(io);

	if	(type)
	{
		if	(check_struct(type, bvar, st))
		{
			DelEfiStruct(st);
			rd_deref(bvar);
		}
		else	io_error(io, "[efmain:157b]", "m", p);
	}
	else	type = MakeStruct(p, bvar, st);

	return type2Obj(Parse_type(io, type));
}


static void ExpandKonv (EfiFunc *func, void *rval, void **arg)
{
	CopyData(func->arg[0].type, rval, arg[0]);
}


EfiType *MakeStruct (char *name, EfiStruct *base, EfiStruct *list)
{
	EfiType *type;
	EfiStruct *st;
	size_t tsize, size, recl;
	int vrecl;
	unsigned flags;
	void (*clean) (const EfiType *type, void *data, int mode);
	void (*copy) (const EfiType *type, void *tg, const void *src);
	size_t (*read) (const EfiType *type, void *data, IO *io);
	size_t (*write) (const EfiType *type, const void *data, IO *io);
	char *p;
	
	if	(!list)
		return &Type_void;

	tsize = 0;
	copy = NULL;
	clean = NULL;
	read = Struct_read;
	write = Struct_write;
	recl = 0;
	vrecl = 0;
	flags = 0;

	if	(base)
	{
		base->next = list;
		list = base;
	}

	for (st = list; st != NULL; st = st->next)
	{
		if	(st->type->copy)	copy = Struct_copy;
		if	(st->type->clean)	clean = Struct_clean;

		if	(!st->type->recl)
		{
			if	(!st->type->read)	read = NULL;
			if	(!st->type->write)	write = NULL;

			vrecl = 1;
		}
		else	recl += st->type->recl * (st->dim ? st->dim : 1);

		if	(st->type->flags & TYPE_MALLOC)
			flags |= TYPE_MALLOC;

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
			DelEfiStruct(list);
			return type;
		}
	}

/*	Bei fehlenden Namen: Standardnamen generieren
*/
	if	(name == NULL)
	{
		StrBuf *sb = sb_acquire();
		IO *io = io_strbuf(sb);

		for (st = list; st != NULL; st = st->next)
			io_xprintf(io, "_%s_%s_%d", st->type->name,
				st->name, st->dim);

		io_close(io);
		name = sb_cpyrelease(sb);
	}

	type = NewType(name);
	type->size = tsize;
	type->recl = vrecl ? 0 : recl;
	type->read = read;
	type->write = write;
	type->eval = NULL;
	type->clean = clean;
	type->copy = copy;
	type->flags = flags;
	type->list = list;
	type->base = base ? base->type : NULL;
	type->vtab = VarTab(mstrcpy(name), 0);
	type->defval = memalloc(type->size);

	for (st = list; st != NULL; st = st->next)
	{
		if	(st->member)
		{
			/* TODO */
			continue;
		}

		p = (char *) type->defval + st->offset;

		if	(st->defval)
		{
			void *src = st->defval->data;

			if	(st->dim)
			{
				CopyVecData(st->type, st->dim, p, src);
			}
			else	CopyData(st->type, p, src);
		}
		else if	(st->dim)
		{
			int n;

			for (n = 0; n < st->dim; n++)
			{
				CopyData(st->type, p, st->type->defval);
				p += st->type->size;
			}
		}
		else	CopyData(st->type, p, st->type->defval);

		AddStruct(type->vtab, st, 1);
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
	EfiStruct *st;
	int vrecl;
	size_t recl;

	void (*clean) (const EfiType *type, void *data, int mode);
	void (*copy) (const EfiType *type, void *tg, const void *src);
	size_t (*read) (const EfiType *type, void *data, IO *io);
	size_t (*write) (const EfiType *type, const void *data, IO *io);

	if	(!type->list)	return;

	copy = NULL;
	clean = NULL;
	read = Struct_read;
	write = Struct_write;
	recl = 0;
	vrecl = 0;

	for (st = type->list; st != NULL; st = st->next)
	{
		if	(st->type->copy)	copy = Struct_copy;
		if	(st->type->clean)	clean = Struct_clean;

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
}

void AddListConv (EfiType *type, EfiFuncCall l2d, EfiFuncCall d2l)
{
	char *p;

	p = msprintf("%s (List_t)", type->name);
	SetFunc(0, type, p, l2d);
	memfree(p);

	p = msprintf("%s ()", type->name);
	SetFunc(FUNC_RESTRICTED, &Type_list, p, d2l);
	memfree(p);
}

/*	Kopierfunktion für Strukturtypen
*/

void Struct_copy (const EfiType *type, void *tg, const void *src)
{
	EfiStruct *st;

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


void Struct_clean (const EfiType *type, void *tg, int mode)
{
	EfiStruct *st;

	for (st = type->list; st != NULL; st = st->next)
	{
		if	(st->dim)
			CleanVecData(st->type, st->dim,
				(char *) tg + st->offset, mode);
		else	CleanData(st->type, (char *) tg + st->offset, mode);
	}
}

size_t Struct_read (const EfiType *type, void *data, IO *io)
{
	EfiStruct *st;
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

size_t Struct_write (const EfiType *type, const void *data, IO *io)
{
	EfiStruct *st;
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

static int cmp_var (EfiStruct *a, EfiStruct *b)
{
	if	(a->type != b->type)			return 1;
	if	(a->dim != b->dim)			return 1;
	if	(mstrcmp(a->name, b->name) != 0)	return 1;

	return 0;
}

static int check_struct (const EfiType *type, EfiStruct *base, EfiStruct *list)
{
	EfiStruct *st = type->list;

	if	(base)
	{
		if	(type->base != base->type)	return 0;

		if	(!st)			return 0;
		if	(cmp_var(base, st))	return 0;

		st = st->next;
	}

	while (list && st)
	{
		if	(cmp_var(list, st))	return 0;

		list = list->next;
		st = st->next;
	}

	if	(list || st)	return 0;

	return 1;
}

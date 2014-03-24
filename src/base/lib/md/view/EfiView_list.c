/*
Inhalt der Variablentabelle auflisten

$Copyright (C) 2007 Erich Frühstück
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

#include <EFEU/EfiView.h>
#include <EFEU/printobj.h>
#include <ctype.h>

#define	MAX_DEPTH	6

static int name_is_std (const char *str)
{
	if	(!str)	return 0;
	if	(*str != '_' && !isalpha(*str))	return 0;

	while (*(++str) != 0)
		if (*str != '_' && !isalnum(*str)) return 0;

	return 1;
}

static int var_is_func (EfiType *type)
{
	if	(type == &Type_func)	return 1;
	if	(type == &Type_vfunc)	return 1;
	if	(type == &Type_ofunc)	return 1;
	return 0;
}

static void print_entry (IO *io, int depth, const char *pfx, VarTabEntry *var);

static void list_type (IO *io, int depth, const char *pfx, EfiType *type)
{
	VarTabEntry *var;
	int i;

	for (var = type->vtab->tab.data, i = type->vtab->tab.used;
			i-- > 0; var++)
	{
		if	(var->type == type)	continue;

		if	(depth >= MAX_DEPTH)
		{
			io_puts(pfx, io);
			io_puts("...\n", io);
			return;
		}

		print_entry(io, depth, pfx, var);
	}
}

static EfiType *time_func (EfiFunc *func)
{
	if	(func->dim != 2)	return NULL;
	if	(func->arg[1].type != &Type_Date)	return NULL;

	return func->type;
}

static int is_time_vec (EfiType *type)
{
	EfiVirFunc *tab;
	EfiFunc **ftab;
	int i;

	if	(!IsTypeClass(type, &Type_vec))	return 0;

	tab = VirFunc(GetGlobalFunc("[]"));
	ftab = tab->tab.data;

	for (i = 0; i < tab->tab.used; i++)
	{
		if	(ftab[i]->dim != 2)
			continue;

		if	(ftab[i]->arg[1].type != &Type_Date)
			continue;

		if	(IsTypeClass(type, ftab[i]->arg[0].type))
			return 1;
	}

	return 0;
}

static EfiType *time_vfunc (EfiFunc *func)
{
	if	(IsVirFunc(func))
	{
		EfiVirFunc *vf = (void *) func;
		EfiFunc **ftab = vf->tab.data;
		EfiType *type;
		int i;

		for (i = 0; i < vf->tab.used; i++)
		{
			if	((type = time_func(ftab[i])))
				return type;
		}

		return NULL;
	}
	else	return time_func(func);
}

static void print_entry (IO *io, int depth, const char *pfx, VarTabEntry *var)
{
	char *p;
	int n;

	depth++;

	if	(!name_is_std(var->name))	return;
	if	(pfx && !var->get)		return;

	if	(var->type == &Type_ofunc)
	{
		EfiType *type = time_vfunc(var->data);

		if	(!type)
			return;

		if	(pfx)
			p = msprintf("%s.%s(t)", pfx, var->name);
		else	p = msprintf("%s(t)", var->name);

		n = io_puts(p, io);
		io_puts("\n", io);
		list_type(io, depth, p, type);
		memfree(p);
		return;
	}
	else if	(var_is_func(var->type))
	{
		return;
	}
	else
	{
		p = mstrpaste(".", pfx, var->name);
		n = io_puts(p, io);
	}

	if	(var->desc)
	{
		if	(n <  8)	io_puts("\t\t", io);
		else if	(n < 15)	io_puts("\t", io);
		else			io_puts("\n\t\t", io);

		io_puts(var->desc, io);
	}

	io_puts("\n", io);

	if	(var->type)
	{
		if	(is_time_vec(var->type))
		{
			EfiVec *vec = Val_ptr(var->type->defval);
			char *p2;

			p2 = msprintf("%s[t]", p);
			io_puts(p2, io);
			io_puts("\n", io);
			list_type(io, depth, p2, vec->type);
			memfree(p2);
		}

		list_type(io, depth, p, var->type);
	}

	memfree(p);
}

void EfiView_list (IO *io, const char *pfx, EfiVarTab *vtab)
{
	VarTabEntry *var;
	int i;

	if	(!io || !vtab)	return;

	for (var = vtab->tab.data, i = vtab->tab.used; i-- > 0; var++)
	{
		io_putc('\n', io);
		print_entry(io, 0, pfx, var);
	}
}

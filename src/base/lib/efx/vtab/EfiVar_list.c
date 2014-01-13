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

#include <EFEU/printobj.h>
#include <EFEU/stdtype.h>
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

static void print_entry (IO *io, int depth, const char *pfx, VarTabEntry *var)
{
	int n;
	int flag;

	if	(!name_is_std(var->name))	return;
	if	(pfx && !var->get)		return;

	n = 0;

#if	0
	if	(var->type->name == NULL)
		n += io_puts(".", io);
	else if	(var->type->name[0] != '_')
		n += io_puts(var->type->name, io);
	else if	(var->type->flags & TYPE_ENUM)
		n += io_puts("enum", io);
	else if	(var->type->flags & TYPE_ENUM)
		n += io_puts("enum", io);
	else	n += io_puts("?", io);

	n += io_puts(" ", io);
#endif

	if	(pfx)
	{
		n += io_puts(pfx, io);
		n += io_puts(".", io);
	}

	n += io_puts(var->name, io);
	flag = 0;

	if	(var_is_func(var->type))
	{
		n += io_puts("()", io);
	}
	else if	(IsTypeClass(var->type, &Type_vec))
	{
		n += io_puts("[]", io);
	}
	else	flag = 1;

	if	(var->desc)
	{
		if	(n <  8)	io_puts("\t\t", io);
		else if	(n < 15)	io_puts("\t", io);
		else			io_puts("\n\t\t", io);

		io_puts(var->desc, io);
	}

	io_puts("\n", io);

	if	(var->type && flag)
	{
		char *p = mstrpaste(".", pfx, var->name);
		list_type(io, depth + 1, p, var->type);
		memfree(p);
	}
}

void EfiVar_list (IO *io, const char *pfx, EfiVarTab *vtab)
{
	VarTabEntry *var;
	int i;

	if	(!io || !vtab)	return;

	for (var = vtab->tab.data, i = vtab->tab.used; i-- > 0; var++)
		print_entry(io, 0, pfx, var);
}

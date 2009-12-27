/*
Variablentabelle ausgeben

$Copyright (C) 1995 Erich Frühstück
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
#include <EFEU/printobj.h>
#include <ctype.h>

#define	MAX_POS	75

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

int DumpVarTab (IO *io, const char *pfx, EfiVarTab *vtab)
{
	int i, n;
	VarTabEntry *var;

	if	(vtab == NULL)	return io_puts("NULL\n", io);

	if	(pfx)		n = io_xprintf(io, "%s", pfx);
	else if	(vtab->name)	n = io_xprintf(io, "%s", vtab->name);
	else			n = io_xprintf(io, "%#p", vtab);

	n += io_puts(" = {\n", io);
	var = vtab->tab.data;

	for (i = vtab->tab.used; i-- > 0; var++)
	{
		if	(var_is_func(var->type))
			continue;

		n += io_puts("\t", io);

		if	(var->type)
			n += io_xprintf(io, "%s ", var->type->name);
		else	n += io_puts(". ", io);

		if	(name_is_std(var->name))
			n += io_puts(var->name, io);
		else	n += io_xprintf(io, "%#s", var->name);

		if	(var->obj)
		{
			char *p = Obj_ident(var->obj);
			n += io_printf(io, " [%s] = ", p);
			memfree(p);
			n += PrintObj(io, var->obj);
		}

		n += io_puts("\n", io);
	}

	n += io_puts("}\n", io);
	return n;
}

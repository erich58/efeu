/*
Aufzählungstype generieren

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
#include <EFEU/Op.h>

#define	PROMPT	"enum { >>> "

#define	ALIGN(x, y)	((y) * (((x) + (y) - 1) / (y)))

static int get_start (Type_t *type, int start)
{
	xtab_t *tab;
	Var_t *var;
	int i;

	if	(!(type && type->vtab))	return start;

	tab = &type->vtab->tab;

	for (i = 0; i < tab->dim; i++)
	{
		var = tab->tab[i];

		if	(var->type != type)
			continue;

		if	(start <= (int) var->par)
			start = (int) var->par + 1;
	}

	return get_start(type->base, start);
}

static void add_key (Type_t *type, io_t *io, int delim)
{
	int start;
	int c;
	char *name;

	start = get_start(type->base, 0);

	while ((c = io_eat(io, " \t\n,")) != EOF)
	{
		if	(c == delim)
		{
			io_getc(io);
			return;
		}

		name = io_getname(io);
		c = io_eat(io, " \t");

		if	(c == '=')
		{
			io_getc(io);
			Obj2Data(Parse_term(io, OpPrior_Assign),
				&Type_int, &start);
		}

		if	(name)
		{
			AddEnumKey(type, name, start);
			memfree(name);
			start++;
		}
	}

	do	c = io_getc(io);
	while	(c != EOF && c != delim);
}

Obj_t *PFunc_enum(io_t *io, void *data)
{
	Type_t *type;
	Type_t *base;
	char *name, *prompt;
	int c;

	name = io_getname(io);
	c = io_eat(io, "%s");

	if	(c == ':')
	{
		io_getc(io);
		base = Parse_type(io, NULL);
		memfree(io_getname(io));
		c = io_eat(io, "%s");
	}
	else	base = NULL;

	if	(base && !IsTypeClass(base, &Type_enum))
	{
		io_error(io, MSG_EFMAIN, 186, 1, base->name);
		base = NULL;
	}

	if	(c != '{')
	{
		io_error(io, MSG_EFMAIN, 185, 1, name);
		memfree(name);
		return NULL;
	}

	type = NewEnumType(name, NULL, 0);
	memfree(name);

	if	(base)
		type->base = base;

	io_getc(io);
	prompt = io_prompt(io, PROMPT);
	add_key(type, io, '}');
	io_prompt(io, prompt);
	return type2Obj(type);
}

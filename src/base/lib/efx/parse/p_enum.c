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
#include <EFEU/ioctrl.h>
#include <ctype.h>

#define	PROMPT	"enum { >>> "

#define	ALIGN(x, y)	((y) * (((x) + (y) - 1) / (y)))

int NextEnumCode (const EfiType *type, int start)
{
	if	(type && type->vtab)
	{
		VarTabEntry *p = type->vtab->tab.data;
		size_t n = type->vtab->tab.used;

		for (; n-- > 0; p++)
		{
			if	(p->obj && p->obj->type == type)
			{
				int val = Val_int(p->obj->data);

				if	(start <= val)
					start = val + 1;
			}
		}

		return NextEnumCode(type->base, start);
	}
	else	return start;
}

static void add_key (EfiType *type, IO *io, int delim)
{
	int start;
	int c;
	char *name;

	io_ctrl(io, IOPP_COMMENT, NULL);
	start = NextEnumCode(type->base, 0);
	c = io_eat(io, " \t\n,");

	if	(io_ctrl(io, IOPP_COMMENT, &type->desc) != EOF)
		mtrim(type->desc);

	while (c != EOF)
	{
		if	(c == delim)
		{
			io_getc(io);
			return;
		}

		if	(c == '"')
		{
			io_getc(io);
			name = io_xgets(io, "\"");
			io_getc(io);
		}
		else	name = io_getname(io);

		if	(name == NULL)
		{
			io_error(io, "[efmain:185]", NULL);
			break;
		}

		c = io_eat(io, " \t");

		if	(c == '=')
		{
			io_getc(io);
			Obj2Data(Parse_term(io, OpPrior_Assign),
				&Type_int, &start);
		}

		c = io_eat(io, " \t\n,");

		if	(name)
		{
			char *p = NULL;
			io_ctrl(io, IOPP_COMMENT, &p);
			mtrim(p);
			AddEnumKey(type, name, p, start);
			start++;
		}
	}

	do	c = io_getc(io);
	while	(c != EOF && c != delim);
}

EfiObj *PFunc_enum (IO *io, void *data)
{
	EfiType *type;
	EfiType *base;
	char *name, *prompt;
	size_t recl;
	int c;

	name = io_getname(io);

	if	(name == NULL)	type = NULL;
	else if	(data)		type = XGetType(name);
	else			type = GetType(name);

	EfiType_version(type, io);
	c = io_eat(io, "%s");
	recl = 4;
	base = NULL;

	if	(c == ':')
	{
		io_getc(io);
		c = io_eat(io, "%s");

		if	(isdigit(c))
		{
			void *ptr;
			io_valscan(io, SCAN_INT, &ptr);
			recl = Val_int(ptr);
		}
		else
		{
			base = Parse_type(io, NULL);
			recl = base->recl;
			memfree(io_getname(io));
		}

		c = io_eat(io, "%s");
	}

	if	(base && !IsTypeClass(base, &Type_enum))
	{
		io_error(io, "[efmain:186]", "s", base->name);
		base = NULL;
		recl = 4;
	}

	if	(c != '{')
	{
		io_error(io, "[efmain:185]", "m", name);
		return NULL;
	}

	io_getc(io);
	prompt = io_prompt(io, PROMPT);

	if	(!type)
	{
		type = NewEnumType(name, recl);

		if	(base)
			type->base = base;

		add_key(type, io, '}');
		type = AddEnumType(type);
	}
	else
	{
		/* Nur überlesen, derzeit keine Prüfung auf Äquivalenz! */

		while ((c = io_skipcom(io, NULL, 0)) != '}')
			;
	}

	io_prompt(io, prompt);
	memfree(name);
	return type2Obj(type);
}

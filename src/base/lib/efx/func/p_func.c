/*
Funktionsdefinition parsen

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
#include <ctype.h>


/*	Prototype - Generierung
*/

static Obj_t *get_func(Func_t *func)
{
	Func_t *f;
	VirFunc_t *vf;

	if	(func->bound)
		vf = GetTypeFunc(func->arg[0].type, func->name);
	else	vf = GetGlobalFunc(func->name);

	f = XGetFunc(func->type, vf, func->arg, func->dim);

	if	(f && f->refcount == 1)
		f->virtual = func->virtual;

	rd_deref(func);
	return NewObj(&Type_func, &f);
}


Obj_t *Parse_func(io_t *io, Type_t *type, Name_t *name, int flags)
{
	Func_t *func;
	char *prompt;
	int c;

	func = MakePrototype(io, type, name, flags);

	if	(func)
	{
		prompt = io_prompt(io, "function >>> ");
		c = io_eat(io, "%s");
		io_prompt(io, prompt);

		if	(c == ';' || c == ',' || c == '(' || c == ')')
		{
			if (c == ';') io_getc(io);

			return get_func(func);
		}
		else if	(c == '{')
		{
			io_getc(io);
			func->par = Parse_block(io, '}');
		}
		else
		{
			func->par = Parse_term(io, 0);

			if	(io_eat(io, " \t") == ';')
				io_getc(io);

			func->eval = Func_inline;
		}

		func->clean = (clean_t) UnrefObj;
		AddFunc(func);
	}

	return Obj_noop();
}

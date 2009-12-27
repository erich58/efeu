/*
Funktionsdefinition anhängen

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


void SetFunc(unsigned flags, Type_t *type, const char *fmt, FuncEval_t eval)
{
	Func_t *func = Prototype(fmt, type, NULL, flags);

	if	(func)
	{
		func->eval = eval;
		AddFunc(func);
	}
}

void AddFuncDef(FuncDef_t *def, size_t dim)
{
	for (; dim-- != 0; def++)
		SetFunc(def->flags, def->type, def->prot, def->eval);
}


void DelFuncArg(FuncArg_t *arg, size_t dim)
{
	if	(arg)
	{
		while (dim-- != 0)
		{
			memfree(arg[dim].name);
			UnrefObj(arg[dim].defval);
		}

		memfree(arg);
	}
}

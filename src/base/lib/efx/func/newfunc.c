/*
Funktionen

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
#include <EFEU/refdata.h>

static ALLOCTAB(tab_func, 0, sizeof(EfiFunc));

static void func_clean (void *data)
{
	EfiFunc *tg = data;

	DelFuncArg(tg->arg, tg->dim);
	memfree(tg->name);

	if	(tg->clean)
		tg->clean(tg->par);

	del_data(&tab_func, tg);
}

static char *func_ident (const void *data)
{
	const EfiFunc *func = data;
	char *p;

	if	(func->name)	p = func->name;
	else if	(func->type)	p = func->type->name;
	else			p = Type_obj.name;

	return msprintf("%s()", p);
}

static const RefType FuncRefType = REFTYPE_INIT("Func",
	func_ident, func_clean);

int IsFunc (void *ptr)
{
	EfiFunc *func = ptr;
	return (func && func->reftype == &FuncRefType);
}

EfiFunc *NewFunc (void)
{
	return rd_init(&FuncRefType, new_data(&tab_func));
}

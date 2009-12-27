/*
Funktionen aufrufen

$Copyright (C) 2001 Erich Frühstück
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


static void **make_args (Func_t *func, va_list list)
{
	if	(func->dim)
	{
		void **args;
		int n;

		args = memalloc(func->dim * sizeof(void *));

		for (n = 0; n < func->dim; n++)
			args[n] = va_arg(list, void *);

		return args;
	}

	return NULL;
}


Obj_t *CallFuncObj (Func_t *func, ...)
{
	va_list list;
	void **args;
	Obj_t *obj;

	if	(func == NULL)	return NULL;

	va_start(list, func);
	args = make_args(func, list);
	va_end(list);

	obj = ConstRetVal(func, args);
	memfree(args);
	return obj;
}


void CallFunc (Type_t *type, void *ptr, Func_t *func, ...)
{
	va_list list;
	void **args;

	if	(func == NULL)	return;

	va_start(list, func);
	args = make_args(func, list);
	va_end(list);

	if	(func->lretval || type != func->type)
	{
		Obj2Data(ConstRetVal(func, args), type, ptr);
	}
	else	func->eval(func, ptr, args);

	memfree(args);
}


void CallVoidFunc (Func_t *func, ...)
{
	va_list list;
	void **args;

	if	(func == NULL)	return;

	va_start(list, func);
	args = make_args(func, list);
	va_end(list);

	if	(func->lretval || func->type == &Type_void)
	{
		func->eval(func, NULL, args);
	}
	else if	(func->type == NULL)
	{
		Obj_t *obj = NULL;
		func->eval(func, &obj, args);
		UnrefObj(obj);
	}
	else
	{
		static size_t size = 0;
		static void *data = NULL;

		if	(size < func->type->size)
		{
			memfree(data);
			size = func->type->size;
			data = memalloc(size);
		}

		func->eval(func, data, args);
		CleanData(func->type, data);
	}

	memfree(args);
}




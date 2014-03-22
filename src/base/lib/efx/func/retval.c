/*
Rückgabewerte berechnen

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


EfiObj *MakeRetVal(EfiFunc *func, EfiObj *firstarg, void **arg)
{
	EfiObj *obj;

	if	(func->lretval)
	{
		func->eval(func, NULL, arg);
		obj = RefObj(firstarg);
	}
	else if	(func->type == NULL)
	{
		obj = NULL;
		func->eval(func, &obj, arg);
	}
	else
	{
		obj = NewObj(func->type, NULL);
		func->eval(func, obj->data, arg);
	}

	return obj;
}


EfiObj *ConstRetVal (EfiFunc *func, void **args)
{
	EfiObj *obj;

	if	(func->lretval)
	{
		func->eval(func, NULL, args);
		obj = ConstObj(func->type, args[0]);
	}
	else if	(func->type == NULL)
	{
		obj = NULL;
		func->eval(func, &obj, args);
	}
	else
	{
		obj = NewObj(func->type, NULL);
		func->eval(func, obj->data, args);
	}

	return obj;
}


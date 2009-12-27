/*
Standardfunktion für Bereichsliste

$Copyright (C) 2002 Erich Frühstück
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

#include <EFEU/Op.h>

void RangeFunc (EfiFunc *func, void *rval, void **arg)
{
	EfiObjList **ptr;
	EfiFunc *f_cmp;
	EfiFunc *f_step;
	int c, d;
	
	ptr = rval;
	*ptr = NewObjList(ConstObj(func->arg[0].type, arg[0]));
	ptr = &(*ptr)->next;

	f_cmp = GetFunc(&Type_int, GetGlobalFunc("cmp"), 2,
		func->arg[0].type, 0, func->arg[1].type, 0);

	if	(f_cmp == NULL)
	{
		log_note(NULL, "[efmain:216]", NULL);
		*ptr = NewObjList(ConstObj(func->arg[1].type, arg[1]));
		return;
	}

	CallFunc(&Type_int, &c, f_cmp, arg[0], arg[1]);

	if	(c == 0)
		return;

	if	(func->dim > 2)
	{
		f_step = GetFunc(NULL,
			GetTypeFunc(func->arg[0].type, c > 0 ? "-=" : "+="),
			2, func->arg[0].type, 1, func->arg[2].type, 0); 
	}
	else
	{
		f_step = GetFunc(NULL,
			GetTypeFunc(func->arg[0].type, c > 0 ? "--" : "++"),
			1, func->arg[0].type, 1); 
	}

	if	(f_step == NULL)
	{
		log_note(NULL, "[efmain:217]", NULL);
		*ptr = NewObjList(ConstObj(func->arg[1].type, arg[1]));
		return;
	}

	do
	{
		CallVoidFunc(f_step, arg[0], arg[2]);
		CallFunc(&Type_int, &d, f_cmp, arg[0], arg[1]);

		if	(d && d != c)	break;

		*ptr = NewObjList(ConstObj(func->arg[0].type, arg[0]));
		ptr = &(*ptr)->next;
	}
	while (d != 0);
}

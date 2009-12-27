/*
Objekte auswerten/konvertieren

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
#include <EFEU/konvobj.h>


void UnrefEval(Obj_t *obj)
{
	UnrefObj(EvalObj(obj, NULL));
}


Obj_t *EvalObj(Obj_t *x, Type_t *type)
{
	Obj_t *y;

	while (x && x->type)
	{
		y = x;

		if	(y->type == type)
		{
			break;
		}
		else if	(y->type->eval)
		{
			x = y->type->eval(y->type, y->data);

			if	(x == y)
			{
				reg_cpy(1, x->type->name);
				errmsg(MSG_EFMAIN, 137);
				UnrefObj(x);
				x = NULL;
			}
		}
		/*
		else if	(type && !type->eval)
		*/
		else if	(type)
		{
			x = KonvObj(y, type);
			UnrefObj(y);
			break;
		}
		else	break;

		UnrefObj(y);
	}

	return x;
}
